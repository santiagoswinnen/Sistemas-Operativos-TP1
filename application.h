#include <sys/types.h>
#include "queue.h"

int applicationMain(int fileNum,char ** files);
char ** generatePipenames(int slaves);
void receiveTask(Queue requests);
int isFile(const char* file);
