//
// Created by valde on 4/14/19.
//

#ifndef HELLO_STACK_H
#define HELLO_STACK_H

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Element {
    struct Element *prev;
    void *e;
} Element;

typedef struct Stack {
    struct Element *last;
} Stack;

Stack *initStack();

void push(Stack *s, void *toPush);

void *pop(Stack *s);

void *viewTopStack(Stack *s);

bool isEmpty(Stack *s);

#endif //HELLO_STACK_H
