

#include <stdio.h>
#include <glob.h>

#ifndef TP1_PIPEUTILITIES_H
#define TP1_PIPEUTILITIES_H

#endif //TP1_PIPEUTILITIES_H

void writePipe(int fd, char * file);
char * numberToThreeDigitArray(size_t num);
void closePipes(int * fds, int amount);