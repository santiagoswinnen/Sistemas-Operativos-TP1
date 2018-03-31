//
// Created by santiago on 30/03/18.
//

#include <unistd.h>
#include <glob.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "pipeUtilities.h"

int * openPipes(char ** pipeNames, int amount, int read) {
    int i;
    int currentFd;
    int * fileDescriptors = malloc(amount*sizeof(int));
    for(i = 0; i < amount; i++) {
        if(read) {
            currentFd = open(pipeNames[i],O_RDONLY);
        } else {
            currentFd = open(pipeNames[i],O_WRONLY);
        }
        fileDescriptors[i] = currentFd;
    }
    return fileDescriptors;
}

ssize_t readPipe(int fd, char * receiver, size_t length) {
    ssize_t  bytesRead;

    bytesRead = read(fd,receiver,length);
    close(fd);
    return bytesRead;
}

void writePipe(int fd, char * file) {
    size_t messageLength;
    messageLength = strlen(file);

    char lengthInChars [4];
    strcpy(lengthInChars, numberToThreeDigitArray(messageLength+1));
    write(fd,lengthInChars,strlen(lengthInChars));
    write(fd,file,strlen(file));
}

char * numberToThreeDigitArray(size_t num) {
    char * ret = malloc(4* sizeof(char));
    ret[0] = (char) ('0' + num/100);
    ret[1] = (char) ('0' + num/10);
    ret[2] = (char) ('0' + num%10);
    ret[3] = 0;
    return ret;
}

void closePipes(int * fds, int amount) {
    int i;
    for(i = 0; i < amount; i++) {
        close(fds[i]);
    }
}