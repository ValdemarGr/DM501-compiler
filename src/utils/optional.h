//
// Created by valde on 4/28/19.
//

#ifndef HELLO_OPTIONAL_H
#define HELLO_OPTIONAL_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct Optional{
    enum { some, none } state;
    void *value;
    bool isDefined;
} Optional;

Optional *map(Optional *optional, void *(*transformer)(void*));

static Optional *Some(void *value);
static Optional *None = &((Optional){
    .state = none,
    .isDefined = false
});

//Optional *None = &((Optional){.state = none});

#endif //HELLO_OPTIONAL_H
