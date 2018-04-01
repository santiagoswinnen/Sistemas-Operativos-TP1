#include <sys/types.h>

int applicationMain(int fileNum,char ** files);
void manageChildren(int fileNum, char ** files, int * pipesFd, int * returningPipesFd);
void createPipe(char * outgoingPipeName ,char * incomingPipeName, int * outgoingFds, int * incomingFds, int index);
void createSlaves(int parentPid, char ** outgoingPipeNames, char ** incomingPipeNames,
                  int * outgoingFds, int * incomingFds);
void endSlave(int fd);
char ** generateOutgoingPipeNames(int slaves);
char ** generateIncomingPipeNames(int slaves);
int isFile(const char* file);
