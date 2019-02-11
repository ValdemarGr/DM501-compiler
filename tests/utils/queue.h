//
// Created by valde on 2/4/19.
//

#ifndef TESTS_QUEUE_H
#define TESTS_QUEUE_H

#define DefaultQueueSize 16

#include <stdio.h>
#if !__APPLE__
#include <malloc.h>
#else
#include <stdlib.h>
#endif

typedef struct {
    size_t remaining_elements;
    size_t allocated_size;
    void **elements;
} Queue;

Queue *initQueue();
void addElement(Queue *q, void *e);
void* dequeue(Queue *q);

#endif //TESTS_QUEUE_H
