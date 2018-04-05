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
#include <semaphore.h>
#include "semaphoreUtilities.h"
#define SEM_ERROR "Error creating semaphore"

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