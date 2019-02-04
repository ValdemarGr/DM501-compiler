//
// Created by valde on 2/2/19.
//

#ifndef TESTS_TEST_H
#define TESTS_TEST_H

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "queue.h"

typedef struct {
    int testCnt;
    int completed;
} TestBlock;

TestBlock *initTestBlock(int tasks);
void testFor(TestBlock* tb, char *str);
void evaluateTest(int result);

typedef struct {
    char* taskName;
    int (*f)(void*);
    void *arg;
} Task;

void appendTask(Queue *q, int (*fnc)(void*), char *name, void *arg);
void runTaks(Queue *q);

#endif //TESTS_TEST_H
