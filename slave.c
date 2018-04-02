
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "slave.h"
#include "pipeUtilities.h"

#define FALSE 0
#define TRUE 1
#define MAX_FILENAME 255
#define READ_END 0
#define WRITE_END 1
#define MD5_BYTES 32

int main(int argc, char * argv []) {
    int incomingPipeFd;
    int outgoingPipeFd;
    char pipeData[MAX_FILENAME];
    char lengthRead[4];
    char * md5;
    int bytesRead;
    int endSignalReceived = FALSE;
    size_t bytesToRead;
    char * incomingPipeName = argv[1];
    char * outgoingPipeName = argv[2];

    incomingPipeFd = open(incomingPipeName,O_RDONLY);
    outgoingPipeFd = open(outgoingPipeName,O_WRONLY);

    do {
        printf("MI fd es %d\n", incomingPipeFd);
        bytesRead = (int)read(incomingPipeFd,lengthRead,3);
        lengthRead[bytesRead] = 0;
        if(bytesRead == 1 && pipeData[0] == ':') {
            endSignalReceived = TRUE;
        } else if(bytesRead == 3){
            printf("LONGITUD: %s PTR %p\n",lengthRead, (void *)pipeData);
            bytesToRead = (size_t)atoi(lengthRead);
            bytesRead = (int)read(incomingPipeFd,pipeData,bytesToRead);
            pipeData[bytesRead] = 0;
            printf("DESPUES DE ESCRIBIR: %s PTR %p\n",pipeData, (void *)pipeData);
            md5 = md5hash(pipeData, bytesRead);
            writePipe(outgoingPipeFd,md5);
        } else {
            if(bytesRead == 0) {endSignalReceived=TRUE;}
        }

    } while(!endSignalReceived);
    close(incomingPipeFd);
    close(outgoingPipeFd);
}

char * md5hash(char * fileName, int length) {
    pid_t pid;
    int status;
    char * md5 = malloc(MD5_BYTES+1);
    char fileNameConsumer[length]; //guarda el filename que md5sum deja en el buffer
    int fds[] = {-1, -1};

    pipe(fds);
    pid = fork();
    if(pid == 0) {
        close(fds[READ_END]);
        dup2(fds[WRITE_END], 1); // 1 == stdout
        char *   args [3] = {"md5sum", fileName, NULL};
        execvp("md5sum", args);
        perror("Could not run md5sum.\n");
    }
    close(fds[WRITE_END]);
    dup2(fds[READ_END], 0); // 0 == stdin
    while(wait(&status) > 0);
    scanf("%s  %s", md5, fileNameConsumer);
    md5[MD5_BYTES] = 0;

    return md5;
}



