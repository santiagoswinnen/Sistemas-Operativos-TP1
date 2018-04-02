#include <sys/types.h>

int applicationMain(int fileNum,char ** files);
void manageChildren(int fileNum, char ** files, int * pipesFd, int * returningPipesFd);
void createSlaves(int parentPid, char ** outgoingPipeNames, char ** incomingPipeNames,
                  int * outgoingFds, int * incomingFds);
int biggestDescriptor(const int * descriptors, int length);
int isFile(const char* file);

