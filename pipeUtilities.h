

#include <stdio.h>
#include <glob.h>

#ifndef TP1_PIPEUTILITIES_H
#define TP1_PIPEUTILITIES_H

#endif //TP1_PIPEUTILITIES_H

ssize_t readPipe(char * pipeName, char * receiver, size_t length);
void writePipe(char * pipeName, char * file);
char * numberToThreeDigitArray(size_t num);