#include <sys/types.h>

int applicationMain(int fileNum,char ** files);
void manageChildren(int fileNum, int slaveNumber, char ** files, int * pipesFd, int * returningPipesFd);
void createSlaves(int parentPid, int slaveNumber, char ** outgoingPipeNames, char ** incomingPipeNames,
                  int * outgoingFds, int * incomingFds);
int biggestDescriptor(const int * descriptors, int length);
int isFile(const char* file);

