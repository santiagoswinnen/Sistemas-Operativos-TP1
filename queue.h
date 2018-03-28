#include <printf.h>

#ifndef TP1_QUEUE_H
#define TP1_QUEUE_H

#endif

typedef struct queueStruct {
    struct queueElement * first;
    struct queueElement * last;
    int length;
    size_t size;
} queueStruct;

typedef queueStruct * Queue;

typedef struct queueElement {
    void * info;
    struct queueElement * next;
    struct queueElement * previous;
} queueElement;

typedef struct queueElement * Element;

Queue newQueue(void);
int isEmpty(Queue queue);
int getSize(Queue queue);
void enqueue(Queue queue, void * elem, int length);
void * dequeue(Queue queue);

