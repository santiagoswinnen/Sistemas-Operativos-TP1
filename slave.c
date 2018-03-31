
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
    char pipeName [11];
    strcpy(pipeName,argv[1]);
    char pipeData[MAX_FILENAME];
    int bytesRead;
    int endSignalReceived = FALSE;
    int bytesToRead;
    char * pipeNumber = argv[1] + 4* sizeof(char);
    char * returningPipeName = newReturningPipe(pipeNumber);


    do {
        bytesRead = (int)readPipe(pipeName,pipeData,3);
        printf("Leo longitud: %s\n", pipeData);
        if(bytesRead == 1 && pipeData[0] == ':') {
            endSignalReceived = TRUE;
        } else if(bytesRead == 3){
            bytesToRead = atoi(pipeData);
            printf("BYTES TO READ: %d\n", bytesToRead);
            readPipe(pipeName,pipeData,(size_t)bytesToRead);
            printf("Recibi un file para procesar! Se llama %s\n", pipeData);
            //md5hash(pipeData, bytesRead);
            //printf("%s\n", pipeData);
            //writePipe(returningPipeName,pipeData);
        } else {
            printf("Lei %d bytes: %s\n",bytesRead, pipeData);
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
    pid_t pid;
    int status;
    char * md5 = malloc(MD5_BYTES);
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

    return md5;
}

char * newReturningPipe(const char * pipeNumber) {
    char pipeName [10] = "retPipe";
    char * ret;

    pipeName[7] = pipeNumber[0];
    pipeName[8] = pipeNumber[1];
    pipeName[9] = 0;
    ret = malloc(sizeof(char)*10);
    strcpy(ret,pipeName);
    mkfifo(ret,0666);
    return ret;
}



