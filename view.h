#ifndef VIEW_H
#define VIEW_H

#include <semaphore.h>

#define MD5_SIZE 32
#define SHM_SIZE (MD5_SIZE + FILENAME_MAX)
#define ERROR_MSG "Error connecting to shared memory\n"

char *get_shared_memory (key_t key);
void open_semaphore (sem_t **semaphore_ptr);
void close_semaphore (sem_t **semaphore_ptr);

#endif
