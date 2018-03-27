#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include "application.h"

#define MAX_PATH 1024
#define MD_LEN 16

int main(int argc, char ** argv) {
    int ret;

    if(argc != 2) {
        printf("Invalid amount of parameters\n");
        exit(0);
    } else {
        ret = applicationMain(argv[1]);
    }
    return ret;

}