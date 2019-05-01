//
// Created by Mads Grau Kristensen on 2019-05-01.
//

#ifndef HELLO_LINKED_LIST_H
#define HELLO_LINKED_LIST_H

typedef struct LinkedListNode {
    void *value;
    struct LinkedListNode *_next;
} LinkedListNode;

typedef struct LinkedList {
    LinkedListNode *first;
    LinkedListNode *last;
} LinkedList;

LinkedList *makeLinkedList();
void appendLinkedList(LinkedList *list, void *value);
void prependLinkedList(LinkedList *list, void *value);
LinkedListNode *iteratorLinkedList(LinkedList *list);
LinkedListNode *nextIteratorLinkedList(LinkedListNode *node);
LinkedList *mapLinkedList(LinkedList *list, void *(*transformer)(void*));

#endif //HELLO_LINKED_LIST_H
