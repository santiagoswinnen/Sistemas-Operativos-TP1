
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "slave.h"
#include "pipeUtilities.h"

#define FALSE 0
#define TRUE 1
#define MAX_FILENAME 255

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
            writePipe(pipeName,pipeData);
        } else {
            tellMasterImFree(pipeName);
        }
    } while(!endSignalReceived);

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

