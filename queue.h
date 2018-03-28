#include <printf.h>

#ifndef TP1_QUEUE_H
#define TP1_QUEUE_H

#endif
#pragma pack(push)
#pragma pack(1)
typedef struct queueStruct {
    struct queueElement * first;
    struct queueElement * last;
    int length;
    size_t size;
} queueStruct;
#pragma pack(pop)

typedef queueStruct * Queue;

typedef struct queueElement {
    void * info;
    struct queueElement * next;
    struct queueElement * previous;
} queueElement;

typedef struct queueElement * Element;

Queue newQueue(size_t size);
int isEmpty(Queue queue);
int getSize(Queue queue);
void enqueue(Queue queue, void * elem, size_t length);
void * dequeue(Queue queue);

