#include <sys/param.h>

#ifndef TP1_SLAVE_H
#define TP1_SLAVE_H

#endif //TP1_SLAVE_H

char * md5hash(char * fileName, int length);
char * newOutgoingPipe(const char * pipeNumber);
void tellMasterImFree(int fd);