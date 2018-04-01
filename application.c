#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "application.h"
#include "pipeUtilities.h"

#define MD5_LEN 16
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
    createSlaves(parentPid,outgoingPipeNames,incomingPipeNames);
    outgoingPipesFd = openPipes(outgoingPipeNames,SLAVE_NUM, WRITE);
    incomingPipesFd = openPipes(incomingPipeNames,SLAVE_NUM ,READ);

    for(i = 0; i < fileNum/2; i++) {
        if(isFile(files[i])) {
            int pipeNum = i%SLAVE_NUM;
            writePipe(outgoingPipesFd[pipeNum], files[i]);
        }
    }
    manageChildren(fileNum, files, outgoingPipesFd, incomingPipesFd);
    closePipes(incomingPipesFd, SLAVE_NUM);
    closePipes(outgoingPipesFd, SLAVE_NUM);
}

void createSlaves(int parentPid, char ** outgoingPipeNames,char ** incomingPipeNames) {
    int i;

    for(i = 0; (i < SLAVE_NUM) && (getpid() == parentPid); i++) {
        mkfifo(outgoingPipeNames[i],0666);
        mkfifo(incomingPipeNames[i],0666);
        pid_t newPid = fork();
        if(newPid == 0) {
            printf("Creando esclavos\n");
            execl("./slave", "./slave", outgoingPipeNames[i], incomingPipeNames[i], (char *)NULL);
        }
    }
}

void manageChildren(int fileNum, char ** files, int * outgoingPipesFd, int * incomingPipesFd) {
    ssize_t bytesRead;
    size_t messageLength;
    int allTasksCompleted = FALSE;
    int i;
    char pipeContent [MD5_LEN];
    char ** md5 = malloc(fileNum* sizeof(char*));
    int md5index = 0;
    int selectRet;
    fd_set incomingSet;

    FD_ZERO(&incomingSet);

    while(!allTasksCompleted) {
        allTasksCompleted = TRUE;
        selectRet = select(SLAVE_NUM,incomingPipesFd,NULL, NULL, 1);
        for(i = 0; i < SLAVE_NUM; i++) {
            bytesRead = readPipe(incomingPipesFd[i], pipeContent, 3*sizeof(char));
            if(bytesRead == 3) {
                messageLength = (size_t)atoi(pipeContent); // NOLINT
                readPipe(incomingPipesFd[i], pipeContent, messageLength);
                md5[md5index] = malloc(MD5_LEN * sizeof(char));
                strcpy(md5[md5index++], pipeContent);
                allTasksCompleted = FALSE;
            } else if (bytesRead == 1 && i < fileNum) {
                writePipe(outgoingPipesFd[i],files[i++]);
                allTasksCompleted = FALSE;
            } else if (bytesRead == 1) {
                endSlave(outgoingPipesFd[i]);
            }
        }
    }
}

int isFile(const char* file) {
    struct stat buf;

    stat(file, &buf);
    return !S_ISDIR(buf.st_mode);
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



