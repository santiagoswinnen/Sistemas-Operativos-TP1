#include <unistd.h>
#include <glob.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "pipeUtilities.h"

void
write_pipe (int fd, char * file) {
    size_t message_length = strlen(file);
    char * length_in_chars = number_to_three_digit_array(message_length);
    int bytes_written = 0;

    do {
        bytes_written += write(fd, length_in_chars, 3);
    } while (bytes_written < 3);

    bytes_written = 0;

    do {
        bytes_written +=  write(fd, file, strlen(file));
    } while (bytes_written < message_length);

    free(length_in_chars);
}

char *
number_to_three_digit_array (size_t num) {
    char *ret = malloc(3 * sizeof(char));

    ret[0] = (char)('0' + num/100);
    ret[1] = (char)('0' + num/10);
    ret[2] = (char)('0' + num%10);

    return ret;
}

void
close_pipes (int *fds, int amount) {
    for(int i = 0; i < amount; i++)
        close(fds[i]);
}


void
free_resources (char **array, int size ) {
    for (int i = 0; i < size; i++)
        free(array[i]);

    free(array);
}


void
end_slaves (int *fds, int amount) {
    char *endMessage = malloc(sizeof(char));

    *endMessage = ':';

    for(int i = 0; i < amount; i++) {
        write(fds[i], endMessage, sizeof(char));
    }

    free(endMessage);
}


void
create_double_pipe (char *outgoing_pipe_name, char * incoming_pipe_name,
    int *outgoing_fds, int *incoming_fds, int index) {
    int mkfifo_ret[2];

    //create_pipe(outgoing_pipe_name, mkfifoRet, 0);
    //create_pipe(incoming_pipe_name, mkfifoRet, 1);

    mkfifo_ret[0] = mkfifo(outgoing_pipe_name, 0777);

    if (mkfifo_ret[0] == -1) {
        perror("Pipe could not be created\n");
        exit(1);
    }

    mkfifo_ret[1] = mkfifo(incoming_pipe_name, 0777);

    if (mkfifo_ret[1] == -1) {
        perror("Pipe could not be created\n");
        exit(1);
    }

    outgoing_fds[index] = open(outgoing_pipe_name, O_WRONLY);
    incoming_fds[index] = open(incoming_pipe_name, O_RDONLY);
}


/*void
create_pipe (char *pipe_name, int *mkfifo_ret, int pipe_direction) {
   mkfifo_ret[pipe_direction] = mkfifo(pipe_name, 0777);

    if (mkfifo_ret[pipe_direction] == -1) {
        perror("Pipe could not be created\n");
        exit(1);
    }
}*/


char **
generate_outgoing_pipe_names (int slaves_amount) {
    char pipe_name [7] = "pipe";
    char ** ret = malloc(slaves_amount * sizeof(char*));

    for(int i = 0; i < slaves_amount; i++) {
        pipe_name[4] = (char)('0' + i/10);
        pipe_name[5] = (char)('0' + i%10);
        pipe_name[6] = 0;
        ret[i] = malloc(sizeof(char) * 11);
        strcpy(ret[i], pipe_name);
    }

    return ret;
}


char **
generate_incoming_pipe_names (int slaves_amount) {
    char pipe_name[10] = "retPipe";
    char **ret = malloc(slaves_amount * sizeof(char*));

    for (int i = 0; i < slaves_amount; i++) {
        pipe_name[7] = (char)('0' + i/10);
        pipe_name[8] = (char)('0' + i%10);
        pipe_name[9] = 0;
        ret[i] = malloc(sizeof(char) * 11);
        strcpy(ret[i], pipe_name);
    }

    return ret;
}
