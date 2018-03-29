#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "application.h"
#include "queue.h"
#define MD5_LEN 16
#define FALSE 0
#define TRUE 1
#define SLAVE_NUM 10

int applicationMain(int fileNum, char ** files) {
    int i;
    int j;
    int k;
    ssize_t bytesRead;
    size_t numMessageLen;
    int allTasksCompleted = FALSE;
    pid_t parentPid = getpid();
    char pipeContent [MD5_LEN];
    char charMessageLen [3];
    char ** pipeNames = generatePipenames(SLAVE_NUM);
    char ** md5 = malloc(fileNum* sizeof(char*));
    int md5index = 0;

    for(j = 0; (j < SLAVE_NUM) && (getpid() == parentPid); j++) {

        mkfifo(pipeNames[j],0666);
        pid_t newPid = fork();
        if(newPid == 0) {
            execl("./slave", pipeNames[j], NULL);
        }
    }

    for(i = 0; i < fileNum/2; i++) {
        if(isFile(files[i])) {
            sendTaskToSlave(pipeNames[i%SLAVE_NUM],files[i]);
        }
    }

    while(!allTasksCompleted) {
        allTasksCompleted = TRUE;
        for(k = 0; k < SLAVE_NUM; k++) {
            bytesRead = readPipe(pipeNames[k], charMessageLen, 3);
            if(bytesRead == 3) {  //Lee primero la longitud del proximo mensaje y despues el mensaje
                numMessageLen = (size_t)atoi(charMessageLen); // NOLINT
                readPipe(pipeNames[k], pipeContent, numMessageLen);
                md5[md5index] = malloc(MD5_LEN * sizeof(char));
                strcpy(md5[md5index++], pipeContent);
                allTasksCompleted = FALSE;
            } else if (bytesRead == 1 && i < fileNum) { //Lee el byte que indica que esta libre
                sendTaskToSlave(pipeNames[k],files[i++]);
                allTasksCompleted = FALSE;
            } else if (bytesRead == 1) {
                endSlave(pipeNames[k]);
            }
        }
    }
}

int isFile(const char* file) {
    struct stat buf;
    stat(file, &buf);
    return !S_ISDIR(buf.st_mode);
}

char ** generatePipenames(int slaves) {
    char pipeName [11] = "pipeData";
    char ** ret = malloc(slaves* sizeof(char*));
    int i;
    for(i = 0; i < slaves; i++) {
        pipeName[9] = (char)(i/10);
        pipeName[10] = (char)(i%10);
        pipeName[11] = 0;
        ret[i] = malloc(sizeof(char)*11);
        strcpy(ret[i],pipeName);
    }
    return ret;
}

void sendTaskToSlave(char * pipeName, char * file) {
    int fd = open(pipeName,O_WRONLY);
    write(fd,file,(strlen(file)+1)*sizeof(char));
    close(fd);
}

ssize_t readPipe(char * pipeName, char * receiver, size_t length) {
    int fd = open(pipeName,O_RDONLY);
    ssize_t bytesRead = read(fd,receiver,length);
    close(fd);
    return bytesRead;
}

void endSlave(char * pipeName) {
    char * endMessage = malloc(sizeof(char));
    endMessage[0] = ':';
    int fd = open(pipeName,O_WRONLY);
    write(fd,endMessage ,sizeof(char));
    close(fd);
}

