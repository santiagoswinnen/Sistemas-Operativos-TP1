#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "application.h"
#include "queue.h"
#define MD5_LEN 16
#define FALSE 0
#define TRUE 1
#define SLAVE_NO 10

int applicationMain(int fileNum, char ** files) {
    int createdSlaves;
    pid_t parentPid = getpid();
    int i;


    for(createdSlaves = 0; (createdSlaves < SLAVE_NO) && (getpid() == parentPid); createdSlaves++) {
            pid_t newPid = fork();
            if(newPid == 0) {
                execl("./slave",NULL, NULL);
                /*TODO: crear el named pipe*/
            }
    }
    
    for(i=0; i < fileNum; i++) {

    }

}

void manageTasks(Queue requests, ) {

}

int isFile(const char* file) {
    struct stat buf;
    stat(file, &buf);
    return !S_ISDIR(buf.st_mode);
}