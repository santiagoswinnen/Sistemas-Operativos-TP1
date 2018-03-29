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
#define SLAVE_NO 10

int applicationMain(int fileNum, char ** files) {
    int j;
    int i;
    pid_t parentPid = getpid();
    int fds[SLAVE_NO];
    char ** pipeNames = generatePipenames(SLAVE_NO);

    for(j = 0; (j < SLAVE_NO) && (getpid() == parentPid); j++) {

        mkfifo(pipeNames[j],0666);
        //fds[j] = open(pipeName,O_WRONLY);
        //close(fds[j]);
        pid_t newPid = fork();
        if(newPid == 0) {
            execl("./slave", pipeNames[j], NULL);
        }
    }

    for(i=0; i < fileNum/2; i++) {
        if(isFile(files[i])) {
            int fd = open(pipeNames[i%SLAVE_NO],O_WRONLY);
            write(fd,files[i],(strlen(files[i])+1)* sizeof(char));
            close(fd);
        }
    }

    while("Los pipes no esten todos vacios") {
        if(i<fileNum) {
            "Mando nuevos a hashear";
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