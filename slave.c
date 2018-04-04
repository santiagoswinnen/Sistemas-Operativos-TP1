#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "slave.h"
#include "pipeUtilities.h"

#define FALSE 0
#define TRUE 1
#define MAX_FILENAME 255
#define READ_END 0
#define WRITE_END 1
#define MD5_BYTES 32

int main(int argc, char * argv []) {
    int incoming_pipe_fd;
    int outgoing_pipe_fd;
    char pipe_data[MAX_FILENAME];
    char length_read[4];
    char *md5 = NULL;
    int bytes_read;
    int end_signal_received = FALSE;
    size_t bytes_to_read;
    char *incoming_pipe_name = argv[1];
    char *outgoing_pipe_name = argv[2];

    incoming_pipe_fd = open(incoming_pipe_name, O_RDONLY);
    outgoing_pipe_fd = open(outgoing_pipe_name, O_WRONLY);

    do {
        bytes_read = (int)read(incoming_pipe_fd, length_read, 3);
        length_read[bytes_read] = 0;

        if ((bytes_read == 1) && (pipe_data[0] == ':')) {
            end_signal_received = TRUE;
        } else if (bytes_read == 3) {
            bytes_to_read = (size_t)atoi(length_read);
            bytes_read = (int)read(incoming_pipe_fd, pipe_data, bytes_to_read);
            pipe_data[bytes_read] = 0;

            if (bytes_read != 0) {
              md5 = md5hash(pipe_data, bytes_read);
              write_pipe(outgoing_pipe_fd, md5);
            } else {
              write_pipe(outgoing_pipe_fd, "");
            }
        } else if (bytes_read == 0) {
            end_signal_received=TRUE;
        }
    } while (!end_signal_received);

    close(incoming_pipe_fd);
    close(outgoing_pipe_fd);

    if (md5 != NULL)
        free(md5);
}

char *
md5hash (char *file_name, int length) {
    pid_t pid;
    int status;
    int read;
    int size = length + MD5_BYTES + 6; // 6 due to the symbols
    char *md5 = malloc(size);
    int fds[] = {-1, -1};

    pipe(fds);

    if ((pid = fork()) == 0) {
        close(fds[READ_END]);
        dup2(fds[WRITE_END], 1); // 1 == stdout
        char *args[3] = {"md5sum", file_name, NULL};
        execvp("md5sum", args);
        perror("Could not run md5sum.\n");
    }

    close(fds[WRITE_END]);
    dup2(fds[READ_END], 0); // 0 == stdin
    while (wait(&status) > 0);
    read = scanf("%s  %s", md5 + length + 4, md5 + 1);

    if (read <= 0) {
        strcpy(md5, "md5sum failed\n");
        return md5;
    }

    md5[0] = md5[length + 3] = '<';
    md5[length + 1] = md5[size - 2] = '>';
    md5[length + 2] = ':';
    md5[size - 1] = 0;

    return md5;
}
