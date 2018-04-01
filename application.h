#include <sys/types.h>

int applicationMain(int fileNum,char ** files);
void manageChildren(int fileNum, char ** files, int * pipesFd, int * returningPipesFd);
void createPipes(char ** outgoingPipeNames ,char ** incomingPipeNames, int amount);
void createSlaves(int parentPid, char ** outgoingPipeNames,char ** incomingPipeNames);
void endSlave(int fd);
char ** generateOutgoingPipeNames(int slaves);
char ** generateIncomingPipeNames(int slaves);
int isFile(const char* file);
