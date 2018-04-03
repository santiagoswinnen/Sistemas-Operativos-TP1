#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/types.h>
//shared memory includes
#include <sys/shm.h>
#include <sys/ipc.h>

//semaphore includes
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>



#include "application.h"
#include "pipeUtilities.h"

#define MD5_LEN 32
#define FALSE 0
#define TRUE 1
#define SLAVE_NUM 1
#define CHAR 1
#define INT 1
#define SHMSIZE (MD5_LEN + FILENAME_MAX)
#define ERROR_MSG "Error creating shared memory"
#define SEM_ERRORM "Error creating semaphore"


int applicationMain(int fileNum, char ** files) {

    int i;
    char ** outgoingPipeNames;
    char ** incomingPipeNames;
    int * outgoingPipesFd;
    int * incomingPipesFd;

     pid_t parentPid = getpid();
    char * shm_address;
    sem_t * sem;
     //Remove previously created memory
    //cleanShm(parentPid);
    //Create shared memory
    shm_address = createSharedMemory(parentPid);
    //Indicates there is no vista yet
    * shm_address = 0;
    *(shm_address +1) = 0;
    openSemaphore(&sem);

    //Give 1 second for vista process to start
    sleep(1);

    int slaveNumber;

    if(fileNum == 0)
        return 0;

    slaveNumber = (fileNum > SLAVE_NUM) ? SLAVE_NUM : fileNum;
    outgoingPipeNames = generateOutgoingPipeNames(slaveNumber);
    incomingPipeNames = generateIncomingPipeNames(slaveNumber);
    outgoingPipesFd = malloc(slaveNumber*sizeof(int));
    incomingPipesFd = malloc(slaveNumber*sizeof(int));
    createSlaves(parentPid, slaveNumber, outgoingPipeNames,incomingPipeNames,
        outgoingPipesFd,incomingPipesFd);

    for(i = 0; i < slaveNumber; i++) {
        if(isFile(files[i])) {
            writePipe(outgoingPipesFd[i], files[i]);
        } else {
          writePipe(outgoingPipesFd[i], "");
        }
    }

    manageChildren(fileNum, slaveNumber, files, outgoingPipesFd,
      incomingPipesFd,shm_address,parentPid, sem);
    closePipes(incomingPipesFd, slaveNumber);
    closePipes(outgoingPipesFd, slaveNumber);

    freeResources(outgoingPipeNames, slaveNumber);
    freeResources(incomingPipeNames, slaveNumber);
    free(incomingPipesFd);
    free(outgoingPipesFd);
    return 0;
}

void createSlaves(int parentPid, int slaveNumber, char ** outgoingPipeNames, char ** incomingPipeNames,
                  int * outgoingFds, int * incomingFds) {

    int i;
    for(i = 0; (i < slaveNumber) && (getpid() == parentPid); i++) {
        pid_t newPid = fork();
        if(newPid == 0) {
            execl("./slave", "./slave", outgoingPipeNames[i], incomingPipeNames[i], (char *)NULL);
        }
        createPipe(outgoingPipeNames[i],incomingPipeNames[i], outgoingFds, incomingFds, i);
    }
}

void manageChildren(int fileNum, int slaveNumber, char ** files,
        int * outgoingPipesFd, int * incomingPipesFd, char * shm_address, key_t key,sem_t * sem) {

    ssize_t bytesRead;
    size_t messageLength;
    int i;
    int fileIndex = slaveNumber;
    char pipeContent[MD5_LEN + FILENAME_MAX + 2];
    char lengthRead [4];
    char ** md5 = malloc(fileNum * sizeof(char *));
    int md5index = 0, zeroCount = 0;
    int nfds = biggestDescriptor(incomingPipesFd, slaveNumber);
    int selectRet;
    char * fileToWrite;
    fd_set readfds;

    while (md5index + zeroCount < fileNum) {
        FD_ZERO(&readfds);
        for (i = 0; i < slaveNumber; i++) {
            FD_SET(incomingPipesFd[i], &readfds);
        }

        selectRet = select(nfds, &readfds, NULL, NULL, NULL);

        if (selectRet == -1) {
            perror("Error at select function\n");
        } else if(selectRet > 0){
            for (i = 0; i < slaveNumber; i++) {
                if (FD_ISSET(incomingPipesFd[i], &readfds) &&
                        ((bytesRead = read(incomingPipesFd[i], lengthRead, 3)) >= 0)) {

                    lengthRead[bytesRead] = 0;

                    if (bytesRead == 3) {
                        messageLength = (size_t) atoi(lengthRead); //NOLINT
                        bytesRead = read(incomingPipesFd[i], pipeContent, messageLength);
                        pipeContent[bytesRead] = 0;

                        if (messageLength != 0) {
                          md5[md5index] = malloc((messageLength + 1) * sizeof(char));
                          strcpy(md5[md5index++], pipeContent);
                          md5[md5index - 1][messageLength] = 0;
                          printf("escribi el md5: %s\n", md5[md5index - 1]);
                        } else {
                          zeroCount++;
                        }

                        if (fileIndex < fileNum) {
                            fileToWrite = files[fileIndex++];

                            if(isFile(fileToWrite))
                            {
                              writePipe(outgoingPipesFd[i], fileToWrite);
                            } else {
                              writePipe(outgoingPipesFd[i], "");
                            }
                        }
                    }
                }
            }
        }
    }
    endSlaves(outgoingPipesFd,slaveNumber);


    /* Initialize sencond shared memory bytes, first byte indicates if vista is present,
    / second if vista is working(1) or app is (0)
    */
    *(shm_address + 1) = 0;

    for(i=0 ; i + zeroCount < fileNum ; i++) {

        switch(*(shm_address+1) ) {

            case 0: clearBufferMemory(shm_address);
                    printf("MD5: %s\n", md5[i]);
                    memcpy(shm_address+2, md5[i], strlen(md5[i]) + 1);
                    *(shm_address+1) = 1;
                    sem_post(sem);
                    break;

            case 1: if(*(shm_address) ) { //Vista is connected to shared memory
                        sem_wait(sem);
                    } else {
                        *(shm_address + 1) = 0;
                    }
                    break;

            default: perror("Invalid reading of shared memory");
                     exit(1);
        }
    }

    //Disconnect from vista process
    if( *shm_address) {
        sem_post(sem);
        *shm_address = 0;
        *(shm_address + 1) = 0;
    }

    //Free shared memory space and close sempaphores
    closeSemaphore(&sem);
    cleanShm(key);
    freeResources(md5,md5index);
}

int isFile(const char* file) {

    struct stat buf;

    stat(file, &buf);
    return !S_ISDIR(buf.st_mode);
}

int biggestDescriptor(const int * descriptors, int length) {

    int biggest = 0;
    int i;
    for(i = 0 ; i < length; i++) {
        if(descriptors[i] > biggest) {
            biggest = descriptors[i];
        }
    }
    biggest += 1;
    return biggest;
}

void cleanShm(key_t key) {

    char str[100];
    sprintf(str,"ipcrm -M %d", (int)key);
    //Execute shell command to clean memory
    system(str);
}


void clearBufferMemory(char *address) {

    for(int i=2 ; i<SHMSIZE ; i++) {
        * ((char *)(address + i)) = 0;
    }

}

char * createSharedMemory(key_t key) {

    char * shm_address;
    int shmid;

    if ((shmid = shmget(key, SHMSIZE, 0666 | IPC_CREAT | IPC_EXCL )) < 0) {
        perror(ERROR_MSG);
        exit(1);
    }

    if ((shm_address = shmat(shmid,NULL,0)) == (char *) -1 ) {
        perror(ERROR_MSG);
        exit(1);
    }

    return shm_address;
}


void openSemaphore(sem_t ** semaphorePtr ) {

    if((*semaphorePtr = sem_open("/my_semaphore", O_CREAT, 0660, 0)) == SEM_FAILED) {
        perror(SEM_ERRORM);
        exit(1);
    }
}

void closeSemaphore(sem_t ** semaphorePtr) {

    sem_unlink("/my_semaphore");
    sem_close(*semaphorePtr);
}
