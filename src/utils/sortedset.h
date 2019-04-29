//
// Created by valde on 4/29/19.
//

#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#ifndef HELLO_SORTEDSET_H
#define HELLO_SORTEDSET_H

typedef struct SortedSet {
    int data;
    struct SortedSet *_next;
} SortedSet;

SortedSet *initHeadedSortedSet();

SortedSet *initSortedSet(int data);

bool exists(SortedSet *sortedSet, int data);

void insertSortedSet(SortedSet *sortedSet, int data);

size_t length(SortedSet *sortedSet);

#endif //HELLO_SORTEDSET_H
