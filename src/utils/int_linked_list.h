//
// Created by Mads Grau Kristensen on 2019-05-01.
//

#ifndef HELLO_INT_LINKED_LIST_H
#define HELLO_INT_LINKED_LIST_H

#include "linked_list.h"

typedef struct IntBoxLinkedList {
    int value;
} IntBoxLinkedList;

typedef LinkedList IntLinkedList;
typedef LinkedListNode IntLinkedListNode;

IntLinkedList *makeIntLinkedList();
void appendIntLinkedList(IntLinkedList *list, int value);
void prependIntLinkedList(IntLinkedList *list, int value);
IntLinkedListNode *iteratorIntLinkedList(IntLinkedList *list);
IntLinkedListNode *nextIteratorIntLinkedList(IntLinkedListNode *node);
int getValueIteratorIntLinkedList(LinkedListNode *node)
IntLinkedList *mapIntLinkedList(IntLinkedList *list, int (*transformer)(int));

#endif //HELLO_INT_LINKED_LIST_H
