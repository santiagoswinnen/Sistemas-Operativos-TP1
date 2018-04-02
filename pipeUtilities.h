

#include <stdio.h>
#include <glob.h>

#ifndef TP1_PIPEUTILITIES_H
#define TP1_PIPEUTILITIES_H

#endif //TP1_PIPEUTILITIES_H

void writePipe(int fd, char * file);
char * numberToThreeDigitArray(size_t num);
void closePipes(int * fds, int amount);
void freeResources(char ** array, int size);
void endSlaves(int * fds, int amount);
char ** generateOutgoingPipeNames(int slaves);
char ** generateIncomingPipeNames(int slaves);
void createPipe(char * outgoingPipeName ,char * incomingPipeName,
                int * outgoingFds, int * incomingFds, int index);