#ifndef PIPE_UTILITIES_H
#define PIPE_UTILITIES_H

#include <stdio.h>
#include <glob.h>

void write_pipe (int fd, char * file);
char *number_to_three_digit_array (size_t num);
void close_pipes (int * fds, int amount);
void free_resources (char **array, int size);
void end_slaves (int *fds, int amount);
char **generate_outgoing_pipe_names (int slaves);
char **generate_incoming_pipe_names (int slaves);
void create_double_pipe (char *outgoing_pipe_name ,char * incoming_pipe_name,
    int *outgoing_fds, int *incoming_fds, int index);
void create_pipe (char *pipe_name, int *mkfifo_ret, int pipe_direction)

#endif
