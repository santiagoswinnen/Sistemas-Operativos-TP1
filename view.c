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
#include <ctype.h>

#define SEM_ERROR "Error creating semaphore"
#define _VIEW 1
#define _APP 0


int main(int argc, char * argv[]) {
	pid_t app_pid;
	char *shm_address, *process_open_atm, *process_using_shm;
	sem_t *sem;

	if (argc != 2 || !is_number(argv[1])) {
		printf("View process expects parent's pid as parameter\n");
		exit(1);
	}

	app_pid = atoi(argv[1]);

	printf("Connected to Application process with ID: %d\n\n",
		app_pid);

	shm_address = get_shared_memory(app_pid);
	open_semaphore(&sem);

	process_open_atm = shm_address;
	process_using_shm = shm_address + 1;
	*process_open_atm = _VIEW;

	while (*process_open_atm == _VIEW) {
	    switch (*process_using_shm) {
	        case _VIEW:
	            printf("%s\n", shm_address + 2);
	            *process_using_shm = _APP;
	            sem_post(sem);
	            break;

	        case _APP:
	            if (*process_open_atm == _VIEW)
	            	sem_wait(sem);
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

int
is_number (char *s) {
	int s_length = strlen(s);
	for (size_t i = 0; i < s_length; i++) {
		if (!isdigit(*(s + i))) {
			return 0;
		}
	}

	return 1;
}
