#include "application.h"


int main(int argc, char * argv[]) {
    if(argc <= 1)
    	return 0;
    int ret = applicationMain(argc - 1, argv + 1);
    return ret;
}