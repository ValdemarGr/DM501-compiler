//
// Created by valde on 2/2/19.
//

#ifndef TESTS_TEST_H
#define TESTS_TEST_H

#include <stdio.h>
#if !__APPLE__
#include <malloc.h>
#else
#include <stdlib.h>
#endif
#include <string.h>

#include "queue.h"

typedef struct {
    char* taskName;
    int (*f)(void*);
    void *arg;
} Task;

void appendTask(Queue *q, int (*fnc)(void*), char *name, void *arg);
void runTaks(Queue *q);

#endif //TESTS_TEST_H
