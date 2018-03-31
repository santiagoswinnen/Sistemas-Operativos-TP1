

#include <stdio.h>
#include <glob.h>

#ifndef TP1_PIPEUTILITIES_H
#define TP1_PIPEUTILITIES_H

#endif //TP1_PIPEUTILITIES_H

ssize_t readPipe(int fd, char * receiver, size_t length);
void writePipe(int fd, char * file);
char * numberToThreeDigitArray(size_t num);
int * openPipes(char ** pipeNames, int amount, int read);
void closePipes(int * fds, int amount);