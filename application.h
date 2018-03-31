#include <sys/types.h>

int applicationMain(int fileNum,char ** files);
void manageChildren(int fileNum, char ** files, char ** pipeNames, char ** returningPipeNames);
void createSlaves(int parentPid, char ** pipeNames);
void endSlave(char * pipeName);
char ** generatePipeNames(int slaves);
char ** generateReturningPipeNames(int slaves);
int isFile(const char* file);
