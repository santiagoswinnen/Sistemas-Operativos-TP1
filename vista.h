#define MD5SIZE 32
#define SHMSIZE (MD5SIZE + FILENAME_MAX)
#define ERROR_MSG "Error connecting to shared memory"

char * getSharedMemory(key_t key);