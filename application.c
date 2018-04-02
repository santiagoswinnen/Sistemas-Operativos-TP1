#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "application.h"
#include "pipeUtilities.h"

#define MD5_LEN 32
#define FALSE 0
#define TRUE 1
#define SLAVE_NUM 1
#define READ 1
#define WRITE 0

int applicationMain(int fileNum, char ** files) {
    int i;
    pid_t parentPid = getpid();
    char ** outgoingPipeNames;
    char ** incomingPipeNames;
    int * outgoingPipesFd;
    int * incomingPipesFd;

    outgoingPipeNames = generateOutgoingPipeNames(SLAVE_NUM);
    incomingPipeNames = generateIncomingPipeNames(SLAVE_NUM);
    outgoingPipesFd = malloc(SLAVE_NUM*sizeof(int));
    incomingPipesFd = malloc(SLAVE_NUM*sizeof(int));
    createSlaves(parentPid,outgoingPipeNames,incomingPipeNames, outgoingPipesFd,incomingPipesFd);

    for(i = 0; i < SLAVE_NUM; i++) {
        if(isFile(files[i])) {
            printf("FILE EN LA DISTRIBUCION: %s\n", files[i]);
            writePipe(outgoingPipesFd[i], files[i]);
        }
    }
    manageChildren(fileNum, files, outgoingPipesFd, incomingPipesFd);
    closePipes(incomingPipesFd, SLAVE_NUM);
    closePipes(outgoingPipesFd, SLAVE_NUM);
    return 1;
}

void createPipe(char * outgoingPipeName ,char * incomingPipeName, int * outgoingFds, int * incomingFds, int index) {

    mkfifo(outgoingPipeName,0777);
    mkfifo(incomingPipeName,0777);
    outgoingFds[index] = open(outgoingPipeName, O_WRONLY);
    incomingFds[index] = open(incomingPipeName, O_RDONLY);
}

void createSlaves(int parentPid, char ** outgoingPipeNames, char ** incomingPipeNames,
                  int * outgoingFds, int * incomingFds) {
    int i;
    for(i = 0; (i < SLAVE_NUM) && (getpid() == parentPid); i++) {
        pid_t newPid = fork();
        if(newPid == 0) {
            execl("./slave", "./slave", outgoingPipeNames[i], incomingPipeNames[i], (char *)NULL);

        }
        createPipe(outgoingPipeNames[i],incomingPipeNames[i], outgoingFds, incomingFds, i);
}
}

void manageChildren(int fileNum, char ** files, int * outgoingPipesFd, int * incomingPipesFd) {
    ssize_t bytesRead;
    size_t messageLength;
    int allTasksCompleted = FALSE;
    int i;
    int fileIndex = SLAVE_NUM;
    char pipeContent[MD5_LEN + FILENAME_MAX + 2];
    char lengthRead [4];
    char **md5 = malloc(fileNum * sizeof(char *));
    int md5index = 0;
    int nfds = biggestDescriptor(incomingPipesFd, SLAVE_NUM);

    fd_set readfds;
    printf("Llegue a select\n");
    while (!allTasksCompleted) {
        FD_ZERO(&readfds);
        for (i = 0; i < SLAVE_NUM; i++) {
            FD_SET(incomingPipesFd[i], &readfds);
        }
        int selectRet = select(nfds, &readfds, NULL, NULL, NULL);
        if (selectRet == -1) {
            perror("Error at select function\n");
        } else if (selectRet == 0) {
            printf("NO HAY NADA QUE LEER\n");
        } else {
            for (i = 0; i < SLAVE_NUM; i++) {
                if (FD_ISSET(incomingPipesFd[i], &readfds) && ((bytesRead = read(incomingPipesFd[i], lengthRead, 3)) >= 0)) {
                    lengthRead[bytesRead] = 0;
                    if (bytesRead == 3) {
                        messageLength = (size_t) atoi(lengthRead); //NOLINT
                        bytesRead = read(incomingPipesFd[i], pipeContent, messageLength);
                        pipeContent[bytesRead] = 0;
                        md5[md5index] = malloc((messageLength + 1) * sizeof(char));
                        strcpy(md5[md5index++], pipeContent);
                        md5[md5index-1][messageLength] = 0;
                        if (fileIndex < fileNum) {
                            char * fileToWrite = files[fileIndex++];
                            writePipe(outgoingPipesFd[i], fileToWrite);
                        }
                    } else {
                        if (fileIndex >= fileNum) {
                            allTasksCompleted = TRUE;
                        }
                    }
                }
            }
        }
    }
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

char ** generateOutgoingPipeNames(int slaves) {
    char pipeName [7] = "pipe";
    char ** ret = malloc(slaves* sizeof(char*));
    int i;

    for(i = 0; i < slaves; i++) {
        pipeName[4] = (char)('0'+i/10);
        pipeName[5] = (char)('0'+i%10);
        pipeName[6] = 0;
        ret[i] = malloc(sizeof(char)*11);
        strcpy(ret[i],pipeName);
    }
    return ret;
}

char ** generateIncomingPipeNames(int slaves) {
    char pipeName [10] = "retPipe";
    char ** ret = malloc(slaves* sizeof(char*));
    int i;

    for(i = 0; i < slaves; i++) {
        pipeName[7] = (char)('0'+i/10);
        pipeName[8] = (char)('0'+i%10);
        pipeName[9] = 0;
        ret[i] = malloc(sizeof(char)*11);
        strcpy(ret[i],pipeName);
    }
    return ret;
}

void endSlave(int fd) {
    char * endMessage;

    endMessage = malloc(sizeof(char));
    endMessage[0] = ':';
    write(fd,endMessage ,sizeof(char));
}



