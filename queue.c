#include <stdlib.h>
#include <string.h>
#include "queue.h"

Queue newQueue(size_t size) {
    Queue q = malloc(sizeof(queueStruct));
    q->length = 0;
    q->size = size;
    q->last = NULL;
    return q;
}

int getLength(Queue queue) {
    return queue->length;
}

int isEmpty(Queue queue) {
    return queue->length == 0;
}

void enqueue(Queue queue, void * elem, size_t elemQty) {
    size_t assignedMemory = (queue->size)*elemQty;
    Element newElement = malloc(sizeof(queueElement));
    Element currentLast = queue->last;
    newElement->info = malloc(assignedMemory);
    memcpy(newElement->info, elem, assignedMemory);
    if(queue->length == 0) {
        queue->first = newElement;
    }
    newElement->previous = currentLast;
    if(currentLast != NULL) {
        currentLast->next = newElement;
    }
    queue->last = newElement;
    queue->length++;
}

void * dequeue(Queue queue) {
    if(isEmpty(queue)) {
        return NULL;
    }
    void * ret;
    Element firstElement = queue->first;
    ret = firstElement->info;
    queue->first = firstElement->next;
    queue->length--;
    free(firstElement);
    return ret;
}