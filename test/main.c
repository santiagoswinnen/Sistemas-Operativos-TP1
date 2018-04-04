#include "application.h"


int main(int argc, char * argv[]) {
    int ret;

    if (argc <= 1)
    	return 0;

    ret = application_main(argc - 1, argv + 1);

    return ret;
}
