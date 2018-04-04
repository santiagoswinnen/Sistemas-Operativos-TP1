#include <semaphore.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>


int application_main (int file_amount, char **files);
void manage_children (int file_amount, int slave_amount, char **files,
    int *outgoing_pipes_fd, int *incoming_pipes_fd, char *shm_address,
    key_t key, sem_t *sem);
void create_slaves (int parent_pid, int slave_amount,
    char **outgoing_pipe_names, char **incoming_pipe_names, int *outgoing_fds,
    int *incoming_fds);
int biggest_descriptor (const int *descriptors, int length);
int is_file (const char *file);
void clean_shm(key_t key);
char *create_shared_memory (key_t key);
void clear_buffer_memory (char *address);
void open_semaphore (sem_t **semaphore_ptr);
void close_semaphore (sem_t **semaphore_ptr);
void writeToMD5(char ** md5, char * pipe_content, int md5_index, size_t message_length);
void sendDataToVista(char * shm_address, sem_t * sem);
