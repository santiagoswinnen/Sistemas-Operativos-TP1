#include <sys/param.h>

#ifndef TP1_SLAVE_H
#define TP1_SLAVE_H

#endif //TP1_SLAVE_H

ssize_t readPipe(char * pipeName, char * receiver, size_t length);
char * md5hash(char * fileName, int length);
char * numberToThreeDigitArray(size_t num);
void sendHashToMaster(char * pipeName, char * pipeData);
void tellMasterImFree(char * pipeName);