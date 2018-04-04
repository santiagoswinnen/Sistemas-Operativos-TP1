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

#define SEM_ERROR "Error creating semaphore"

int main(int argc, char * argv[]) {
	pid_t app_pid;
	char *shm_address;
	sem_t *sem;

	if (argc != 2) {
		printf("View process expects parent's pid as parameter\n");
		exit(1);
	}

	// Application pid will be used as key to create shared memory.
	app_pid = atoi(argv[1]);

	printf("Connected to Application process with ID: %d\n\n",
		app_pid);

	shm_address = get_shared_memory(app_pid);
	open_semaphore(&sem);

	//Connect with Application process.
	*shm_address = 1;

	while (*shm_address) {
	    switch (*(shm_address + 1)) {
	        case 1:
	            printf("%s\n", shm_address + 2);
	            *(shm_address + 1) = 0;
	            sem_post(sem);
	            break;
	        case 0:
	            sem_wait(sem);
	            break;
	        default:
	            perror("Invalid reading of shared memory\n");
	            exit(1);
	    }
	}

	return 0;
}


char *
get_shared_memory (key_t key) {
	int shm_id;
	char *address;

	if ((shm_id = shmget(key, SHM_SIZE, 0666)) < 0) {
		perror(ERROR_MSG);
		exit(1);
	}

	if ((address = shmat(shm_id, NULL, 0)) == (char *)-1)
		perror(ERROR_MSG);

	return address;
}


void
open_semaphore (sem_t **semaphore_ptr ) {
    if ((*semaphore_ptr = sem_open("/my_semaphore", O_CREAT, 0660, 0))
		== SEM_FAILED) {
        perror(SEM_ERROR);
        exit(1);
    }
}


void
close_semaphore (sem_t **semaphore_ptr) {
    sem_unlink("/my_semaphore");
    sem_close(*semaphore_ptr);
}
