//
// Created by valde on 2/2/19.
//

#include "test.h"


void evaluateTest(int result) {
    if (result == 0) {
        printf ("\033[31m FAILED \033[0m\n");
    } else if (result == 2) {
        printf ("\033[33m PENDING \033[0m\n");
    } else {
        printf ("\033[32m OK \033[0m\n");
    }
}

void appendTask(Queue *q, int (*fnc)(void*), char* name, void* arg) {
    Task *t = (Task*)malloc(sizeof(Task));

    t->f = fnc;
    t->taskName = (char*)malloc(sizeof(char) *strlen(name));
    strcpy(t->taskName, name);
    t->arg = arg;

    addElement(q, t);
}

void runTaks(Queue *q) {
    size_t max_size = q->remaining_elements;

    for (int i = 0; i < max_size; i++) {
        Task *t = (Task*)dequeue(q);

        printf("[%zu/%zu] %s -> ", max_size - q->remaining_elements, max_size, t->taskName);
        evaluateTest(2);
        fflush(stdout);

        int o = t->f(t->arg);

        printf("[%zu/%zu] %s -> ", max_size - q->remaining_elements, max_size, t->taskName);
        evaluateTest(o);
        fflush(stdout);
    }
}

