#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include "vista.h"
#include <sys/param.h>

#define SEM_ERRORM "Error creating semaphore"

int main(int argc, char * argv[]) {

	pid_t app_pid;
	char * shm_address;
	sem_t * sem;

	if(argc != 2) {
		printf("Vista process expects parents pid as parameter");
		exit(1);
	}

	//Application process id will be used as key to create memory
	app_pid = atoi(argv[1]);

	printf("Connected to Application Process with ID: %d", app_pid);

	shm_address = getSharedMemory(app_pid);
	openSemaphore(&sem);

	//Connect with application process
	*shm_address = 1;

	while(*shm_address)	{

	    switch( *(shm_address + 1)) {

	        case 1:
	            printf("%s\n",shm_address + 2);
	            * (shm_address + 1) = 0;
	            sem_post(sem);
	            break;
	        case 0:
	            sem_wait(sem);
	            break;
	        default:
	            perror("Invalid reading of shared memory");
	            exit(1);
	    }

	}

	return 0;
}

char * getSharedMemory(key_t key) {

	int shmid;
	char * address;

	if ((shmid = shmget(key,SHMSIZE,0666)) < 0) {
		perror(ERROR_MSG);
		exit(1);
	}


	if ((address = shmat(shmid,NULL,0)) == (char *) -1) {
		perror(ERROR_MSG);

	}
	return address;
}

void openSemaphore(sem_t ** semaphorePtr ) {

    if((*semaphorePtr = sem_open("/my_semaphore", O_CREAT, 0660, 0)) == SEM_FAILED) {
        perror(SEM_ERRORM);
        exit(1);
    }
}

void closeSemaphore(sem_t ** semaphorePtr) {

    sem_unlink("/my_semaphore");
    sem_close(*semaphorePtr);
}
