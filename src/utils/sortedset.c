//
// Created by valde on 4/29/19.
//

#include "sortedset.h"

SortedSet *initHeadedSortedSet() {
    SortedSet *sortedSet = (SortedSet*)(malloc(sizeof(SortedSet)));
    sortedSet->_next = NULL;
    sortedSet->data = INT_MIN;

    return sortedSet;
}

SortedSet *initSortedSet(int data) {
    SortedSet *sortedSet = (SortedSet*)(malloc(sizeof(SortedSet)));
    sortedSet->_next = NULL;
    sortedSet->data = data;

    return sortedSet;
}

bool exists(const SortedSet *sortedSet, int data) {
    const SortedSet *iter = sortedSet;

    while (iter != NULL) {

        if (iter->data == data) {
            return true;
        }

        iter = iter->_next;
    }

    return false;
}

size_t length(const SortedSet *sortedSet) {
    size_t counter = 0;

    const SortedSet *iter = sortedSet;

    while (iter != NULL) {
        counter = counter + 1;
        iter = iter->_next;
    }

    return counter - 1;
}

void insertSortedSet(SortedSet *sortedSet, int data) {
    SortedSet *iter = sortedSet;

    while (iter != NULL) {

        if (iter->data == data) {
            return;
        }

        if (iter->_next == NULL) {
            iter->_next = initSortedSet(data);
            return;
        }

        if (iter->data < data && data < iter->_next->data) {
            SortedSet *temp = initSortedSet(data);
            temp->_next = iter->_next;
            iter->_next = temp;
            return;
        }

        if (data < iter->data) {
            SortedSet *temp = initSortedSet(data);
            temp->_next = iter;
            iter->_next = temp;
            return;
        }

        iter = iter->_next;
    }
}

SortedSet *sortedSetUnion(const SortedSet *set1, const SortedSet *set2) {
    const SortedSet *iter1 = set1;
    const SortedSet *iter2 = set2;

    SortedSet *newSet = initHeadedSortedSet();

    while (iter1 != NULL) {
        insertSortedSet(newSet, iter1->data);
        iter1 = iter1->_next;
    }

    while (iter2 != NULL) {
        insertSortedSet(newSet, iter2->data);
        iter2 = iter2->_next;
    }

    return newSet;
}

SortedSet *sortedSetDiff(const SortedSet *set1, const SortedSet *set2) {
    const SortedSet *iter1 = set1;
    const SortedSet *iter2 = set2;

    SortedSet *newSet = initHeadedSortedSet();

    while (iter1 != NULL && iter2 != NULL) {
        if (iter1->data < iter2->data) {
            insertSortedSet(newSet, iter1->data);
            iter1 = iter1->_next;
        } else if (iter1->data > iter2->data) {
            iter2 = iter2->_next;
        } else if (iter1->data == iter2->data) {
            iter1 = iter1->_next;
            iter2 = iter2->_next;
        }
    }

    while (iter1 != NULL) {
        insertSortedSet(newSet, iter1->data);
        iter1 = iter1->_next;
    }

    return newSet;
}

void freeSortedSet(SortedSet* sortedSet) {
    SortedSet *iter = sortedSet;
    SortedSet *next;

    while (iter != NULL) {
        next = iter->_next;

        free(iter);

        iter = next;
    }
}

SortedSet *iterateSortedSet(const SortedSet* set) {
    return set->_next;
}

int sortedSetFirst(const SortedSet* set) {
    return set->_next->data;
}

SortedSet *first(const SortedSet *set) {
    if (set == NULL) return NULL;
    return set->_next;
}

char *sortedSetToString(const SortedSet *sortedSet) {
    SortedSet *iter = sortedSet->_next;
    char *buffer = malloc(sizeof(char) * 16);
    size_t allocated = 16;
    size_t len = 1;
    size_t bufLen = 0;

    char *start = "{";
    memmove(buffer, start, 1);
    while (iter != NULL) {
        char buf[24];
        if (len == 1) {
            sprintf(buf, "%d", iter->data);
        } else {
            sprintf(buf, ", %d", iter->data);
        }

        bufLen = strlen(buf);

        if (bufLen + len + 2 > allocated) {
            while (bufLen + len + 2 > allocated) {
                allocated *= 2;
            }
            buffer = realloc(buffer, allocated);
        }

        memmove(buffer + len, buf, bufLen);
        len += bufLen;

        iter = iter->_next;
    }
    char *end = "}";
    memmove(buffer + len, end, 1);
    len += 1;

    return buffer;
}

SortedSet *getByIndex(SortedSet *head, int index) {
    int counter = index;
    while (counter != 0) {
        head = head->_next;
        counter--;
    }

    return head.
}
