all: application slave

application: application.c main.c pipeUtilities.c
	gcc -o application application.c main.c pipeUtilities.c -lpthread -lrt -Wall -pedantic

slave: slave.c pipeUtilities.c
	gcc -o slave slave.c pipeUtilities.c -Wall -pedantic

vista: vista.c
	gcc -o vista vista.c -lpthread -lrt -Wall -pedantic
