#include <sys/types.h>
#include "queue.h"

int applicationMain(int fileNum,char ** files);
void manageChildren(int fileNum, char ** files, char ** pipeNames);
void createSlaves(int parentPid, char ** pipeNames);
void endSlave(char * pipeName);
char ** generatePipeNames(int slaves);
int isFile(const char* file);
