#include <sys/types.h>
#include <bits/semaphore.h>

int applicationMain(int fileNum,char ** files);
void manageChildren(int fileNum, int slaveNumber, char ** files, int * pipesFd, int * returningPipesFd,char * shm_address,key_t key,sem_t * sem);
void createSlaves(int parentPid, int slaveNumber, char ** outgoingPipeNames, char ** incomingPipeNames,
                  int * outgoingFds, int * incomingFds);
int biggestDescriptor(const int * descriptors, int length);
int isFile(const char* file);

void cleanShm(key_t key);
char * createSharedMemory(key_t key);
void clearBufferMemory(char *address);
void openSemaphore(sem_t ** semaphorePtr );
void closeSemaphore(sem_t ** semaphorePtr);