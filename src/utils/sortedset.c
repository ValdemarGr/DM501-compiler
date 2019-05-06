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

bool exists(SortedSet *sortedSet, int data) {
    SortedSet *iter = sortedSet;

    while (iter != NULL) {

        if (iter->data == data) {
            return true;
        }

        iter = iter->_next;
    }

    return false;
}

size_t length(SortedSet *sortedSet) {
    size_t counter = 0;

    SortedSet *iter = sortedSet;

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

SortedSet *sortedSetUnion(SortedSet *set1, SortedSet *set2) {
    SortedSet *iter1 = set1;
    SortedSet *iter2 = set2;

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

SortedSet *sortedSetDiff(SortedSet *set1, SortedSet *set2) {
    SortedSet *iter1 = set1;
    SortedSet *iter2 = set2;

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

SortedSet *iterateSortedSet(SortedSet* set) {
    return set->_next;
}

int sortedSetFirst(SortedSet* set) {
    return set->_next->data;
}

SortedSet *first(SortedSet *set) {
    return set->_next;
}
