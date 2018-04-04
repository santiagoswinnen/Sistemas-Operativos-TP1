all: application slave view

application: application.c main.c pipeUtilities.c
	gcc -o application application.c main.c pipeUtilities.c semaphoreUtilities.c -lpthread -lrt -Wall -pedantic

slave: slave.c pipeUtilities.c
	gcc -o slave slave.c pipeUtilities.c -Wall -pedantic

view: view.c
	gcc -o view view.c semaphoreUtilities.c -lpthread -lrt -Wall -pedantic
