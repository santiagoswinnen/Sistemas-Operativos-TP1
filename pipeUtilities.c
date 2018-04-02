//
// Created by santiago on 30/03/18.
//

#include <unistd.h>
#include <glob.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "pipeUtilities.h"


ssize_t readPipe(int fd, char * receiver, size_t length) {
    ssize_t  bytesRead;

    bytesRead = read(fd,receiver,length);
    return bytesRead;
}

void writePipe(int fd, char * file) {
    size_t messageLength;
    messageLength = strlen(file);
    char * lengthInChars = numberToThreeDigitArray(messageLength);
    printf("FILE EN WRITE: %s\n",file);
    write(fd,lengthInChars,3);
    write(fd,file,strlen(file));
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