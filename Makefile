all: application slave

application: application.c main.c pipeUtilities.c
	gcc -o application application.c main.c pipeUtilities.c -Wall -pedantic

slave: slave.c pipeUtilities.c
	gcc -o slave slave.c pipeUtilities.c -Wall -pedantic