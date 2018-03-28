#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include "application.h"
#include "queue.h"
#define MD5_LEN 16

int applicationMain(char ** files) {
    char ** fileNames;
    int argIndex = 0;
    Queue requests = newQueue();

    for(argIndex = 1; files[argIndex] != NULL; argIndex++) {
        if(isFile(files[argIndex])) {
            enqueue(requests,files[argIndex]);
        }
    }
    assignTasks(requests);
}

void assignTasks(Queue requests) {
    char * md5buffer[MD5_LEN];
    /*TODO: abrir pipes a los esclavos*/
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
