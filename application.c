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
#define SLAVE_NUM 10
#define CHAR 1
#define INT 1

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
            writePipe(outgoingPipesFd[i], files[i]);
        }
    }

    manageChildren(fileNum, files, outgoingPipesFd, incomingPipesFd);
    closePipes(incomingPipesFd, SLAVE_NUM);
    closePipes(outgoingPipesFd, SLAVE_NUM);

    freeResources(outgoingPipeNames, SLAVE_NUM);
    freeResources(incomingPipeNames, SLAVE_NUM);
    free(incomingPipesFd);
    free(outgoingPipesFd);
    return 0;
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
    int i;
    int fileIndex = SLAVE_NUM;
    char pipeContent[MD5_LEN + FILENAME_MAX + 2];
    char lengthRead [4];
    char ** md5 = malloc(fileNum * sizeof(char *));
    int md5index = 0;
    int nfds = biggestDescriptor(incomingPipesFd, SLAVE_NUM);
    fd_set readfds;

    while (md5index < fileNum) {
        FD_ZERO(&readfds);
        for (i = 0; i < SLAVE_NUM; i++) {
            FD_SET(incomingPipesFd[i], &readfds);
        }
        int selectRet = select(nfds, &readfds, NULL, NULL, NULL);

        if (selectRet == -1) {
            perror("Error at select function\n");
        } else if(selectRet > 0){
            for (i = 0; i < SLAVE_NUM; i++) {
                if (FD_ISSET(incomingPipesFd[i], &readfds) &&
                        ((bytesRead = read(incomingPipesFd[i], lengthRead, 3)) >= 0)) {

                    lengthRead[bytesRead] = 0;
                    if (bytesRead == 3) {
                        messageLength = (size_t) atoi(lengthRead); //NOLINT
                        bytesRead = read(incomingPipesFd[i], pipeContent, messageLength);
                        pipeContent[bytesRead] = 0;
                        md5[md5index] = malloc((messageLength + 1) * sizeof(char));
                        strcpy(md5[md5index++], pipeContent);
                        md5[md5index - 1][messageLength] = 0;
                        if (fileIndex < fileNum) {
                            char *fileToWrite = files[fileIndex++];
                            writePipe(outgoingPipesFd[i], fileToWrite);
                        }
                    }
                }
            }
        }
    }
    endSlaves(outgoingPipesFd,SLAVE_NUM);
    for(i = 0; i < fileNum ; i++) {
        printf("MD5: %s\n", md5[i]);
    }
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
