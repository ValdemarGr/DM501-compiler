//
// Created by valde on 2/4/19.
//

#include "queue.h"

Queue *initQueue() {
    Queue *q = (Queue*)malloc(sizeof(Queue));

    q->allocated_size = 0;
    q->elements = (void*)malloc(sizeof(void*) * DefaultQueueSize);
    q->allocated_size = DefaultQueueSize;

    return q;
}

void addElement(Queue *q, void *e) {
    if (q == NULL || e == NULL) {
        return;
    }

    //Allocate new space if needed
    if (q->remaining_elements + 1 > q->allocated_size - 1) {
        void **newElements = (void*)malloc(sizeof(void*) * q->allocated_size * 2);

        //Copy elements over
        for (int i = 0; i < q->remaining_elements; i++) {
            newElements[i] = q->elements[i];
        }

        free(q->elements);

        q->elements = newElements;
        q->allocated_size = q->allocated_size * 2;
    }

    //Insert the element
    q->elements[q->remaining_elements] = e;

    //Update the count
    q->remaining_elements = q->remaining_elements + 1;
}

void *dequeue(Queue *q) {
    //Error check
    if (q == NULL || q->remaining_elements == 0) {
        return NULL;
    }

    void* e = q->elements[0];
    q->remaining_elements = q->remaining_elements - 1;

    //Move all elements back
    for (int i = 0; i < q->remaining_elements; i++) {
        q->elements[i] = q->elements[i + 1];
    }

    return e;
}

