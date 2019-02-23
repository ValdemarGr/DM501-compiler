//
// Created by valde on 2/23/19.
//

#ifndef HELLO_MAP_H
#define HELLO_MAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "memory.h"

typedef struct Key{
    enum { eIK, eCK } keyTypes;
    union {
        int iK;
        char *cK;
    } key;
} Key;

typedef struct Pair {
    struct Key *k;
    void *v;
    struct Pair *next;
} Pair;

typedef struct ConstMap {
    size_t size;
    Pair **entries;
} ConstMap;

size_t findAtleaseLargerThanNPrime(size_t n);

ConstMap *initMap(size_t tableSize);

size_t hashFromKey(ConstMap *m, Key *k);

void insert(ConstMap *m, Key *k, void *v);

Pair *getTail(Pair *pair);

Pair *get(ConstMap *m, Key *k);

#endif //HELLO_MAP_H
