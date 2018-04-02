#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <vista.h>

char * getSharedMemory(key_t key);

int main(int argc, char * argv[]) {

	pid_t app_pid;
	key_t key;
	char * shm_address;

	if(argc != 2) {

		printf("Vista process expects parents pid as parameter");
		exit(1);
	}

	//Application process id will be used as key to create memory
	int app_pid = atoi(argv[1]);

	shm_address = getSharedMemory(app_pid);


	while( *shm_address != 0) {

	} 


	return 0;
}

char * getSharedMemory(key_t key) {

	int shmid;
	char * address;
	
	if (shmid = shmget(key,SHMSIZE,0666) < 0) {
		perror(ERROR_MSG);
		exit(1);
	}


	if ((address = shmat(shmid,NULL,0)) == (char *) -1) {
		perror(ERROR_MSG);

	}
	return address;
}