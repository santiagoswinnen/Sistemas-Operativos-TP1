#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "semaphoreUtilities.h"
#include "application.h"
#include "pipeUtilities.h"

#define MD5_LEN 32
#define FALSE 0
#define TRUE 1
#define SLAVE_NUM 1
#define CHAR 1
#define INT 1
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define SHMSIZE (MD5_LEN + FILENAME_MAX)
#define ERROR_MSG "Error creating shared memory"
#define SEM_ERROR "Error creating semaphore"
#define SLEEP_TIME 8
#define _VIEW 1
#define _APP 0


int
application_main (int file_amount, char **files) {

    int slave_amount;
    char **outgoing_pipe_names, **incoming_pipe_names;
    int *outgoing_pipes_fd, *incoming_pipes_fd;
    char *shm_address, *process_open_atm, *process_using_shm;
    pid_t parent_pid = getpid();
    sem_t *sem;
    FILE *results_fp = fopen("./results.txt", "a");
    unsigned int results_fd = fileno(results_fp);
    char *separator = "-----------------------------------------\n";

    printf("Application PID: %d\n", parent_pid);

    dup2(results_fd, STDOUT);
    dup2(results_fd, STDERR);
    printf("%s", separator);

    shm_address = create_shared_memory(parent_pid);
    process_open_atm = shm_address;
	process_using_shm = shm_address + 1;
    *process_open_atm = _APP;
    *process_using_shm = _APP;

    open_semaphore(&sem);

    sleep(SLEEP_TIME);

    if (file_amount == 0)
        return 0;

    slave_amount = (file_amount > SLAVE_NUM) ? SLAVE_NUM : file_amount;
    outgoing_pipe_names = generate_outgoing_pipe_names(slave_amount);
    incoming_pipe_names = generate_incoming_pipe_names(slave_amount);
    if ((outgoing_pipes_fd = malloc(slave_amount * sizeof(int))) == NULL) {
        perror("Memory could not be allocated");
        exit(1);
    }

    if ((incoming_pipes_fd = malloc(slave_amount * sizeof(int))) == NULL) {
        perror("Memory could not be allocated");
        exit(1);
    }


    create_slaves(parent_pid, slave_amount, outgoing_pipe_names,
        incoming_pipe_names, outgoing_pipes_fd, incoming_pipes_fd);

    for (int i = 0; i < slave_amount; i++) {
        if (is_file(files[i]))
            write_pipe(outgoing_pipes_fd[i], files[i]);
        else
            write_pipe(outgoing_pipes_fd[i], "");
    }

    manage_children(file_amount, slave_amount, files, outgoing_pipes_fd,
        incoming_pipes_fd, shm_address, parent_pid, sem);
    close_pipes(incoming_pipes_fd, incoming_pipe_names, slave_amount);
    close_pipes(outgoing_pipes_fd, outgoing_pipe_names, slave_amount);

    free_resources(outgoing_pipe_names, slave_amount);
    free_resources(incoming_pipe_names, slave_amount);
    free(incoming_pipes_fd);
    free(outgoing_pipes_fd);
    fclose(results_fp);

    return 0;
}

void
create_slaves (int parent_pid, int slave_amount, char **outgoing_pipe_names,
    char **incoming_pipe_names, int *outgoing_fds, int *incoming_fds) {
    for (int i = 0; (i < slave_amount) && (getpid() == parent_pid); i++) {
        pid_t newPid = fork();

        if (newPid == 0)
            execl("./slave", "./slave", outgoing_pipe_names[i],
            incoming_pipe_names[i], (char *)NULL);

        create_double_pipe(outgoing_pipe_names[i], incoming_pipe_names[i],
            outgoing_fds, incoming_fds, i);
    }
}


void
manage_children (int file_amount, int slave_amount, char **files,
    int *outgoing_pipes_fd, int *incoming_pipes_fd, char *shm_address, key_t key,
    sem_t *sem) {

    ssize_t bytes_read;
    size_t message_length;
    int i;
    int file_index = slave_amount;
    char pipe_content[MD5_LEN + FILENAME_MAX + 2];
    char length_read[4];
    char **md5;
    int md5_index = 0, folder_count = 0;
    int nfds = biggest_descriptor(incoming_pipes_fd, slave_amount);
    char *file_to_write;
    char *process_open_atm = shm_address;
    fd_set readfds;

    if ((md5 = malloc(file_amount * sizeof(char *))) == NULL) {
        perror("Memory could not be allocated");
        exit(1);
    }

    while (md5_index + folder_count < file_amount) {
        int select_ret;

        FD_ZERO(&readfds);

        for (i = 0; i < slave_amount; i++)
            FD_SET(incoming_pipes_fd[i], &readfds);

        select_ret = select(nfds, &readfds, NULL, NULL, NULL);

        if (select_ret == -1) {
            perror("Error at select function");

        } else if (select_ret > 0){

            for (i = 0; i < slave_amount; i++) {

                if (FD_ISSET(incoming_pipes_fd[i], &readfds) &&
                    ((bytes_read = read(incoming_pipes_fd[i], length_read, 3))
                    >= 0)) {

                    length_read[bytes_read] = 0;

                    if (bytes_read == 3) {
                        message_length = (size_t) atoi(length_read); //NOLINT
                        bytes_read = read(incoming_pipes_fd[i], pipe_content,
                            message_length);
                        pipe_content[bytes_read] = 0;

                        if (message_length != 0) {
                            write_to_md5(md5, pipe_content, md5_index,
                                message_length);

                            if (*process_open_atm == _VIEW)
                                send_data_to_view(shm_address, sem, md5,
                                    md5_index, file_amount, folder_count);
                            else
                                printf("%s\n", md5[md5_index]);

                            md5_index++;
                        } else {
                          folder_count++;
                        }

                        if (file_index < file_amount) {
                            file_to_write = files[file_index++];

                            if (is_file(file_to_write))
                                write_pipe(outgoing_pipes_fd[i], file_to_write);
                            else
                                write_pipe(outgoing_pipes_fd[i], "");
                        }
                    }
                }
            }
        }
    }

    end_slaves(outgoing_pipes_fd, slave_amount);

    disconnect_view_process(process_open_atm, sem);

    close_semaphore(&sem);
    clean_shm(key);
    free_resources(md5, md5_index);
}

void
write_to_md5 (char **md5, char *pipe_content, int md5_index,
    size_t message_length) {

    md5[md5_index] = malloc((message_length + 1) * sizeof(char));
    strcpy(md5[md5_index], pipe_content);
    md5[md5_index][message_length] = 0;
}

void
send_data_to_view (char *shm_address, sem_t *sem, char **md5, int md5_index,
    int file_amount, int folder_count) {

    char *process_open_atm, *process_using_shm;

    process_open_atm = shm_address;
    process_using_shm = shm_address + 1;

    switch (*process_using_shm) {
        case _APP:
            clear_buffer_memory(shm_address);
            printf("%s\n", md5[md5_index]);
            memcpy(shm_address + 2, md5[md5_index], strlen(md5[md5_index]) + 1);
            *process_using_shm = _VIEW;
            sem_post(sem);
            break;

        case 1:
            if (*process_open_atm == _VIEW)
                sem_wait(sem);
            else
               *process_using_shm = _APP;
            break;

        default:
            perror("Invalid reading of shared memory");
            exit(1);
    }
}


void
disconnect_view_process (char *process_open_atm, sem_t *sem) {

    char * process_using_shm = process_open_atm + 1;

    if (*process_open_atm == _VIEW) {
        *process_open_atm = _APP;
        *process_using_shm = _APP;
        sem_post(sem);
    }
}


int
is_file (const char *file) {

    struct stat buf;

    stat(file, &buf);

    return !S_ISDIR(buf.st_mode);
}


int
biggest_descriptor (const int *descriptors, int length) {

    int biggest = 0;

    for (int i = 0 ; i < length; i++)
        if(descriptors[i] > biggest)
            biggest = descriptors[i];

    return biggest + 1;
}


void
clean_shm (key_t key) {

    char str[100];

    sprintf(str,"ipcrm -M %d", (int)key);
    system(str);
}


void
clear_buffer_memory (char *address) {

    for (int i = 2 ; i < SHMSIZE ; i++)
        *((char *)(address + i)) = 0;
}


char *
create_shared_memory (key_t key) {

    char *shm_address;
    int shm_id;

    if ((shm_id = shmget(key, SHMSIZE, 0666 | IPC_CREAT | IPC_EXCL )) < 0) {
        perror(ERROR_MSG);
        exit(1);
    }

    if ((shm_address = shmat(shm_id, NULL, 0)) == (char *)-1 ) {
        perror(ERROR_MSG);
        exit(1);
    }

    return shm_address;
}
