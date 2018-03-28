#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "application.h"
#include "queue.h"
#define MD5_LEN 16
#define FILE_PER_SLAVE 10
#define FALSE 0
#define TRUE 1

int applicationMain(int fileNum, char ** files) {
    char ** fileNames;
    int argIndex = 0;
    Queue requests = newQueue(sizeof(char*));
    Queue freeProcesses = newQueue(sizeof(pid_t));
    int isApplication = TRUE;

    for(argIndex = 0; argIndex < fileNum && isApplication; argIndex++) {
        if(isFile(files[argIndex])) {
            enqueue(requests,files[argIndex],strlen(files[argIndex])+1);
        }
        if(argIndex % FILE_PER_SLAVE == 0) {
            pid_t pid = fork();
            if(pid == 0) {
                isApplication = FALSE;
                receiveTask(requests);
            }
        }
    }
}

void receiveTask(Queue requests) {
    char * parameters [] = {"md5sum",NULL,NULL};
    while("" == 0/*TODO: El padre no le diga por el pipe que ya esta todo*/) {
        parameters[1] = dequeue(requests);
        execvp("md5sum", parameters);
        /*TODO: hacer un pipe que reciba el md5*/
    }
}

int isFile(const char* file) {
    struct stat buf;
    stat(file, &buf);
    return !S_ISDIR(buf.st_mode);
}


void findFreeSlaves(Queue freeProcesses /*PIPES*/) {
    /*TODO: leer pipes y agregar los libres */
}