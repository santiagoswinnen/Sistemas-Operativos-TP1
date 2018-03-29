#include <sys/types.h>
#include "queue.h"

int applicationMain(int fileNum,char ** files);
void sendTaskToSlave(char * pipeName, char * file);
ssize_t readPipe(char * pipeName, char * receiver, size_t length);
void endSlave(char * pipeName);
char ** generatePipenames(int slaves);
int isFile(const char* file);
