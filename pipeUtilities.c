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
    size_t messageLength = strlen(file);

    char lengthInChars [3];
    strcpy(lengthInChars, numberToThreeDigitArray(messageLength));
    write(fd,lengthInChars,3*sizeof(char));
    write(fd,file,(strlen(file)+1)*sizeof(char));
    close(fd);
}

char * numberToThreeDigitArray(size_t num) {
    char * ret = malloc(3* sizeof(char));
    ret[0] = (char) ('0' + num/100);
    ret[1] = (char) ('0' + num/10);
    ret[2] = (char) ('0' + num%10);
    return ret;
}