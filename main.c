#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include "activityAuxiliaries.h"

#define MAX_PATH 1024

int main(int argc, char **argv) {
    char name[MAX_PATH];
    DIR * directoryStream;
    struct dirent * direntStruct;


    if(argc != 1) {
        printf("Invalid amount of parameters\n");
        exit(0);
    }
    if((directoryStream = opendir(argv[1])) == NULL) {
        fprintf(stderr,"Can't open %s\n",argv[1]);
    }
    while((direntStruct = readdir(directoryStream)) != NULL) {
        if(strcmp(direntStruct->d_name,".") == 0 || strcmp(direntStruct->d_name,"..") == 0) {
            continue;
            /* TODO: hay que ver como se trata al directorio actual */
        }
        if(strlen(argv[1])+strlen(direntStruct->d_name) > sizeof(name)) {
            fprintf(stderr, "Name %s/%s is too long\n", argv[1], direntStruct->d_name);
        } else {
            manageFile(name);
        }
    }
    closedir(directoryStream);

}