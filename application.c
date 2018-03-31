#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "application.h"
#include "queue.h"
#include "pipeUtilities.h"

#define MD5_LEN 16
#define FALSE 0
#define TRUE 1
#define SLAVE_NUM 1

int applicationMain(int fileNum, char ** files) {
    int i;
    pid_t parentPid = getpid();
    char ** pipeNames;
    char ** returningPipeNames;

    pipeNames = generatePipeNames(SLAVE_NUM);
    returningPipeNames = generateReturningPipeNames(SLAVE_NUM);

    createSlaves(parentPid,pipeNames);
    for(i = 0; i < fileNum/2; i++) {
        if(isFile(files[i])) {
            writePipe(pipeNames[i%SLAVE_NUM],files[i]);
        }
    }
    manageChildren(fileNum, files, pipeNames, returningPipeNames);


}
void createSlaves(int parentPid, char ** pipeNames) {
    int i;

    for(i = 0; (i < SLAVE_NUM) && (getpid() == parentPid); i++) {
        mkfifo(pipeNames[i],0666);
        pid_t newPid = fork();
        if(newPid == 0) {
            execl("./slave", "./slave", pipeNames[i], (char *)NULL);
        }
    }
}

void manageChildren(int fileNum, char ** files, char ** pipeNames, char ** returningPipeNames) {
    ssize_t bytesRead;
    size_t messageLength;
    int allTasksCompleted = FALSE;
    int i;
    char pipeContent [MD5_LEN];
    char ** md5 = malloc(fileNum* sizeof(char*));
    int md5index = 0;

    while(!allTasksCompleted) {
        allTasksCompleted = TRUE;
        for(i = 0; i < SLAVE_NUM; i++) {
            bytesRead = readPipe(returningPipeNames[i], pipeContent, 3*sizeof(char));
            if(bytesRead == 3) {
                messageLength = (size_t)atoi(pipeContent); // NOLINT
                readPipe(returningPipeNames[i], pipeContent, messageLength);
                md5[md5index] = malloc(MD5_LEN * sizeof(char));
                strcpy(md5[md5index++], pipeContent);
                allTasksCompleted = FALSE;
            } else if (bytesRead == 1 && i < fileNum) {
                writePipe(pipeNames[i],files[i++]);
                allTasksCompleted = FALSE;
            } else if (bytesRead == 1) {
                endSlave(pipeNames[i]);
            }
        }
    }
}

int isFile(const char* file) {
    struct stat buf;

    stat(file, &buf);
    return !S_ISDIR(buf.st_mode);
}

char ** generatePipeNames(int slaves) {
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

char ** generateReturningPipeNames(int slaves) {
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

void endSlave(char * pipeName) {
    char * endMessage;
    int fd = open(pipeName,O_WRONLY);;

    endMessage = malloc(sizeof(char));
    endMessage[0] = ':';
    write(fd,endMessage ,sizeof(char));
    close(fd);
}



