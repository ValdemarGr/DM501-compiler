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
