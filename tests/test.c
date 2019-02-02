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

void evaluateTest(int result, TestBlock* tb) {
    if (result == 0) {
        printf ("\033[31m FAILED \033[0m\n");
    } else if (result == 2) {
        printf ("\033[33m PENDING \033[0m\n");
    } else {
        printf ("\033[32m OK \033[0m\n");
    }
    tb->completed++;
}

