//
// Created by santiago on 30/03/18.
//

#include <unistd.h>
#include <glob.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "pipeUtilities.h"

void writePipe(int fd, char * file) {

    size_t messageLength;
    messageLength = strlen(file);
    char * lengthInChars = numberToThreeDigitArray(messageLength);
    int bytesWritten = 0;

    do {
        bytesWritten += write(fd,lengthInChars,3);
    } while(bytesWritten < 3);

    bytesWritten = 0;

    do {
        bytesWritten +=  write(fd,file,strlen(file));
    } while(bytesWritten < messageLength);

    free(lengthInChars);
}

char * numberToThreeDigitArray(size_t num) {

    char * ret = malloc(3* sizeof(char));

    ret[0] = (char) ('0' + num/100);
    ret[1] = (char) ('0' + num/10);
    ret[2] = (char) ('0' + num%10);
    return ret;
}

void closePipes(int * fds, int amount) {
    int i;
    for(i = 0; i < amount; i++) {
        close(fds[i]);
    }
}

void freeResources(char ** array, int size ) {

    int i;

    for(i = 0; i < size; i++ ) {
        free(array[i]);
    }
    free(array);
}

void endSlaves(int * fds, int amount) {

    char * endMessage = malloc(sizeof(char));
    int i;

    *endMessage = ':';
    for(i = 0; i < amount; i++) {
        write(fds[i],endMessage ,sizeof(char));
    }
    free(endMessage);
}

void createPipe(char * outgoingPipeName ,char * incomingPipeName, int * outgoingFds,
                int * incomingFds, int index) {

    int mkfifoRet [2];

    mkfifoRet[0] = mkfifo(outgoingPipeName,0777);
    if(mkfifoRet[0] == -1) {
        perror("Pipe could not be created");
        exit(1);
    }
    mkfifoRet[1] = mkfifo(incomingPipeName,0777);
    if(mkfifoRet[1] == -1) {
        perror("Pipe could not be created");
        exit(1);
    }

    outgoingFds[index] = open(outgoingPipeName, O_WRONLY);
    incomingFds[index] = open(incomingPipeName, O_RDONLY);
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
