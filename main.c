#include "application.h"


int main(int argc, char * argv[]) {
    int ret = applicationMain(argc - 1, argv + 1);
    return ret;
}