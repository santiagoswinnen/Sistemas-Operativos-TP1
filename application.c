#include "application.h"

int applicationMain(char * path) {
    char name[MAX_PATH];
    DIR * directoryStream;
    struct dirent * direntStruct;
    /******************************/
    char * md5buffer[MD_LEN];
    int bufferSize = 0;
    /******************************/

    if((directoryStream = opendir(path)) == NULL) {
        fprintf(stderr,"Can't open %s\n",path);
    }
    while((direntStruct = readdir(directoryStream)) != NULL) {
        if(strcmp(direntStruct->d_name,".") == 0 || strcmp(direntStruct->d_name,"..") == 0) {
            continue;
            /* TODO: hay que ver como se trata al directorio actual */
        }
        if(strlen(path)+strlen(direntStruct->d_name) > sizeof(name)) {
            fprintf(stderr, "Name %s/%s is too long\n", path, direntStruct->d_name);
        } else {
            manageFile(name);
        }
    }
    closedir(directoryStream);
}

void manageFile(char * name) {
    /*TODO: aca se tiene que ver a que esclavo mandarle el path actual */
}
