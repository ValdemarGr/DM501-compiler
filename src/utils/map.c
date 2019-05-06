//
// Created by valde on 2/23/19.
//

#include "map.h"

Key *makeIntKey(int iK) {
    Key *k = (Key*)malloc(sizeof(Key));
    k->keyTypes = eIK;
    k->key.iK = iK;
    return k;
}

Key *makeCharKey(char *cK) {
    Key *k = (Key*)malloc(sizeof(Key));
    k->keyTypes = eCK;
    k->key.cK = cK;
    return k;
}

Pair *maxBy(ConstMap *m, size_t (*unpacker)(void*)) {
    Pair* largestPair = NULL;
    size_t largestValue = 0;

    for (int i = 0; i < m->size; i++) {
        Pair *this = m->entries[i];

        while (this != NULL) {
            size_t evaluated = unpacker(this->v);

            if (evaluated > largestValue) {
                largestValue = evaluated;
                largestPair = this;
            }

            this = this->next;
        }
    }

    return largestPair;
}

Pair *minBy(ConstMap *m, size_t (*unpacker)(void*)) {
    Pair* smallestPair = NULL;
    size_t smallestValue = UINT_MAX;

    for (int i = 0; i < m->size; i++) {
        Pair *this = m->entries[i];

        while (this != NULL) {
            size_t evaluated = unpacker(this->v);

            if (evaluated < smallestValue) {
                smallestValue = evaluated;
                smallestPair = this;
            }

            this = this->next;
        }
    }

    return smallestPair;
}

size_t findAtleastLargerThanNPrime(size_t n) {

    if (n <= 5) {
        return 5;
    }

    size_t cur = n;
    while (1) {
        int incorrectTrials = 0;

        for (int i = 2; i < cur; i++) {
            if (cur % i == 0) {
                incorrectTrials = 1;
                break;
            }
        }

        if (incorrectTrials == 0) {
            return cur;
        } else {
            cur++;
        }
    }
}

ConstMap *initMap(size_t tableSize) {
    ConstMap *m = NEW(ConstMap);

    size_t nearestPrime = findAtleastLargerThanNPrime(tableSize);

    m->size = nearestPrime;
    m->entries = (Pair**)malloc(sizeof(Pair) * nearestPrime);

    for (int i = 0; i < nearestPrime; i++) {
        m->entries[i] = NULL;
    }

    return m;
}

size_t hashFromKey(ConstMap *m, Key *k) {
    if (k->keyTypes == eCK) {
        char *str = k->key.cK;
        size_t len = strlen(str);

        //If no chars, no need to run though
        if (len == 0) {
            return 0;
        }

        int sum = (int)str[0];

        //Start at 1, we initial the sum for first character above
        for (int i = 1; i < len; i++) {
            //Shift
            sum = sum << 1;
            //Add to sum
            sum = sum + (int)str[i];
        }

        return sum % m->size;
    } else {
        return k->key.iK % m->size;
    }
}

void insert(ConstMap *m, Key *k, void *v) {
    size_t idx = hashFromKey(m, k);

    Pair *newEle = NEW(Pair);

    newEle->k = k;
    newEle->v = v;
    newEle->next = NULL;

    Pair *tail = getTail(m->entries[idx]);

    if (tail == NULL) {
        m->entries[idx] = newEle;
    } else {
        tail->next = newEle;
    }
}

Pair *getTail(Pair *pair) {
    if (pair == NULL) {
        return NULL;
    }

    Pair *cur = pair;


    while (cur->next != NULL) {
        cur = cur->next;
    }

    return cur;
}

Pair *get(ConstMap *m, Key *k) {
    size_t idx = hashFromKey(m, k);

    Pair* cur = m->entries[idx];

    if (cur != NULL) {
        if (cur->k->keyTypes == eIK && k->keyTypes == eIK && k->key.iK == cur->k->key.iK) {
            return cur;
        } else if (cur->k->keyTypes == eCK && k->keyTypes == eCK && strcmp(k->key.cK, cur->k->key.cK) == 0) {
            return cur;
        }
    }


    return NULL;
}

