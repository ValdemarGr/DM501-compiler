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

SortedSet *sortedSetUnion(SortedSet *set1, SortedSet *set2);

SortedSet *sortedSetDiff(SortedSet *set1, SortedSet *set2);

void freeSortedSet(SortedSet* sortedSet);

SortedSet *iterateSortedSet(SortedSet* set);

int sortedSetFirst(SortedSet* set);

#endif //HELLO_SORTEDSET_H
