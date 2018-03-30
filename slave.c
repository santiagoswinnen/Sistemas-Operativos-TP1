
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "slave.h"
#define FALSE 0
#define TRUE 1
#define MAX_FILENAME 255
//Debe recibir el nombre del pipe asignado
int main(int argc, char * argv []) {
    char pipeName [11];
    strcpy(pipeName,argv[1]);
    char pipeData[MAX_FILENAME];
    int bytesRead;
    int endSignalReceived = FALSE;
    int bytesToRead;
    do {
        bytesRead = (int)readPipe(pipeName,pipeData,3*sizeof(char));
        if(bytesRead == 1 && pipeData[0] == ':') {
            endSignalReceived = TRUE;
        } else if(bytesRead == 3){
            bytesToRead = atoi(pipeData);
            readPipe(pipeName,pipeData,(size_t)bytesToRead);
            md5hash(pipeData, bytesRead);
            sendHashToMaster(pipeName,pipeData);
        } else {
            tellMasterImFree(pipeName);
        }
    } while(!endSignalReceived);

}

ssize_t readPipe(char * pipeName, char * receiver, size_t length) {
    int fd = open(pipeName,O_RDONLY);
    ssize_t bytesRead = read(fd,receiver,length);
    close(fd);
    return bytesRead;
}

void sendHashToMaster(char * pipeName, char * pipeData) {
    size_t dataLength = strlen(pipeData);
    char * lengthInChars = malloc(3* sizeof(char));
    strcpy(lengthInChars,numberToThreeDigitArray(dataLength));
    int fd = open(pipeName,O_WRONLY);
    write(fd,lengthInChars,(strlen(pipeData)+1)*sizeof(char));
    write(fd,pipeData,strlen(pipeData)* sizeof(char));
    close(fd);

}

void tellMasterImFree(char * pipeName) {
    char message [1];
    message[0] = 1;
    int fd = open(pipeName,O_WRONLY);
    write(fd,message,1);
    close(fd);
}

char * md5hash(char * fileName, int length) {
    //TODO: Calcular el MD5 y traerlo
}

char * numberToThreeDigitArray(size_t num) {
    char * ret = malloc(3* sizeof(char));
    ret[0] = (char) ('0' + num/100);
    ret[1] = (char) ('0' + num/10);
    ret[2] = (char) ('0' + num%10);
    return ret;
}

