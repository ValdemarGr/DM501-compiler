//
// Created by valde on 4/29/19.
//

#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#ifndef HELLO_SORTEDSET_H
#define HELLO_SORTEDSET_H

#include <stdio.h>
#include <string.h>

typedef struct SortedSet {
    int data;
    struct SortedSet *_next;
} SortedSet;

SortedSet *initHeadedSortedSet();

SortedSet *initSortedSet(int data);

bool exists(const SortedSet *sortedSet, int data);

void insertSortedSet(SortedSet *sortedSet, int data);

size_t length(const SortedSet *sortedSet);

SortedSet *sortedSetUnion(const SortedSet *set1, const SortedSet *set2);

SortedSet *sortedSetDiff(const SortedSet *set1, const SortedSet *set2);

void freeSortedSet(SortedSet* sortedSet);

SortedSet *iterateSortedSet(const SortedSet* set);

int sortedSetFirst(const SortedSet* set);

SortedSet *first(const SortedSet *set);

char *sortedSetToString(const SortedSet *sortedSet);

SortedSet *getByIndex(SortedSet *head, int index);

#endif //HELLO_SORTEDSET_H
