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
            printf("WRITE: Voy a escribir %s de longitud %d al fd %d\n",lengthInChars, (int)strlen(lengthInChars), fd);
            bytesWritten += write(fd,lengthInChars,3);
            printf("WRITE: Ya escribi %d bytes\n", bytesWritten);
        } while(bytesWritten < 3);

        bytesWritten = 0;
        do {
            printf("WRITE: Voy a escribir %s de longitud %d\n",file, (int)strlen(file));
            bytesWritten +=  write(fd,file,strlen(file));
            printf("WRITE: Ya escribi %d bytes\n", bytesWritten);
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