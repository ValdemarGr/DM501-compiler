//
// Created by valde on 4/14/19.
//

#include "stack.h"

Stack *initStack() {
    Stack *s = (Stack*)malloc(sizeof(Stack));

    s->last = NULL;

    return s;
}

void push(Stack *s, void *toPush) {
    Element *newE = (Element*)malloc(sizeof(Element));
    newE->e = toPush;

    if (s->last == NULL) {
        newE->prev = NULL;
    } else {
        newE->prev = s->last;
    }

    s->last = newE;
}

void *pop(Stack *s) {
    Element *toReturn = s->last;

    s->last = s->last->prev;

    return toReturn->e;
}

void *viewTopStack(Stack *s) {
    return s->last;
}

bool isEmpty(Stack *s) {
    return s->last == NULL;
}





