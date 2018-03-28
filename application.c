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

int applicationMain(int fileNum, char ** files) {
    char ** fileNames;
    int argIndex = 0;
    Queue requests = newQueue();
    int isApplication = TRUE;

    for(argIndex = 0; argIndex < fileNum && isApplication; argIndex++) {
        if(isFile(files[argIndex])) {
            enqueue(requests,files[argIndex],strlen(files[argIndex]));
        }
        if(argIndex%10 == 0) {
            pid_t pid =fork();
            if(pid == 0) {
                isApplication = FALSE;
                assignTasks(requests);
            }
        }
    }
    if(isApplication) {
        assignTasks(requests);
    }
}

void assignTasks(Queue requests) {
    char ** md5buffer;

    /*TODO: crear una queue de procesos libres (modificar TAD para que sea void *) */
    while(!isEmpty(requests)) {
        /*TODO: leer pipes y agregar los libres */
        /*TODO: hacer dequeue de proceso libre y request y ponerlos a trabajar*/
    }
}

int isFile(const char* file) {
    struct stat buf;
    stat(file, &buf);
    return !S_ISDIR(buf.st_mode);
}
