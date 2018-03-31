//
// Created by santiago on 30/03/18.
//

#include <unistd.h>
#include <glob.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "pipeUtilities.h"

ssize_t readPipe(char * pipeName, char * receiver, size_t length) {
    int fd = open(pipeName,O_RDONLY);
    ssize_t  bytesRead;

    bytesRead = read(fd,receiver,length);
    close(fd);
    return bytesRead;
}

void writePipe(char * pipeName, char * file) {
    int fd = open(pipeName,O_WRONLY);
    size_t messageLength;
    messageLength = strlen(file);

    char lengthInChars [4];
    strcpy(lengthInChars, numberToThreeDigitArray(messageLength+1));
    write(fd,lengthInChars,strlen(lengthInChars));
    write(fd,file,strlen(file));
    close(fd);
}

char * numberToThreeDigitArray(size_t num) {
    char * ret = malloc(4* sizeof(char));
    ret[0] = (char) ('0' + num/100);
    ret[1] = (char) ('0' + num/10);
    ret[2] = (char) ('0' + num%10);
    ret[3] = 0;
    return ret;
}