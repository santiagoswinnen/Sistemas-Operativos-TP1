#include <stdlib.h>
#include <string.h>
#include "queue.h"

Queue newQueue(void) {
    Queue q = malloc(sizeof(queueStruct));
    q->size = 0;
    return q;
}

int getSize(Queue queue) {
    return queue->size;
}

int isEmpty(Queue queue) {
    return queue->size == 0;
}

void enqueue(Queue queue, char * elem) {
    Element newElement = malloc(sizeof(queueElement));
    Element currentLast = queue->last;
    strcpy(newElement->info, elem);
    if(queue->size == 0) {
        queue->first = newElement;
    }
    newElement->previous = currentLast;
    currentLast->next = newElement;
    queue->last = newElement;
}

char * dequeue(Queue queue) {
    char * ret;
    Element firstElement = queue->first;
    ret = firstElement->info;
    queue->first = firstElement->next;
    free(firstElement);
    return ret;
}