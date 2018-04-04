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
#include "view.h"
#include "semaphoreUtilities.h"
#include <semaphore.h>
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

	app_pid = atoi(argv[1]);

	printf("Connected to Application process with ID: %d\n\n",
		app_pid);

	shm_address = get_shared_memory(app_pid);
	open_semaphore(&sem);
	
	*shm_address = 1;

	while (*shm_address) {
	    switch (*(shm_address + 1)) {
	        case 1:
	            printf("%s\n", shm_address + 2);
	            *(shm_address + 1) = 0;
	            sem_post(sem);
	            break;
	        case 0:
	            if(*shm_address) {
	            	sem_wait(sem);	
	            }
	            
	            break;
	        default:
	            perror("Invalid reading of shared memory");
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



