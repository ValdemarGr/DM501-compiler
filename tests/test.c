//
// Created by valde on 2/2/19.
//

#include "test.h"

TestBlock *initTestBlock(int tasks) {
    TestBlock* tb = (TestBlock *)malloc(sizeof(TestBlock));

    tb->testCnt = tasks;
    tb->completed = 1;

    return tb;
}

void testFor(TestBlock* tb, char *str) {
    printf("[%i/%i] %s ->", tb->completed, tb->testCnt, str);
}

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

        printf("[%i/%i] %s -> ", max_size - q->remaining_elements, max_size, t->taskName);

        int o = t->f(t->arg);

        evaluateTest(o);
    }
}

