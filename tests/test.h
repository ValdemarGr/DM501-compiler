//
// Created by valde on 2/2/19.
//

#ifndef TESTS_TEST_H
#define TESTS_TEST_H

#include <stdio.h>
#include <malloc.h>
#include <string.h>

typedef struct {
    int testCnt;
    int completed;
} TestBlock;

TestBlock *initTestBlock(int tasks);
void testFor(TestBlock* tb, char *str);
void evaluateTest(int result, TestBlock* tb);

#endif //TESTS_TEST_H
