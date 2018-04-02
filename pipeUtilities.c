//
// Created by santiago on 30/03/18.
//

#include <unistd.h>
#include <glob.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include "pipeUtilities.h"

void writePipe(int fd, char * file) {
    size_t messageLength;
    messageLength = strlen(file);
    char * lengthInChars = numberToThreeDigitArray(messageLength);
    int bytesWritten = 0;

    if(messageLength != 0) {
        do {
            bytesWritten += write(fd,lengthInChars,3);
        } while(bytesWritten < 3);

        bytesWritten = 0;
        do {
            bytesWritten +=  write(fd,file,strlen(file));
        } while(bytesWritten < messageLength);
    }


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