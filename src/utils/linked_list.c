//
// Created by Mads Grau Kristensen on 2019-05-01.
//

#include "linked_list.h"
#include "memory.h"
#include "stdlib.h"

LinkedList *makeLinkedList() {
    LinkedList *list = NEW(LinkedList);
    return list;
}

LinkedListNode *_makeLinkedListNode(void *value) {
    LinkedListNode *node = NEW(LinkedListNode);
    node->value = value;
    return node;
}

void appendLinkedList(LinkedList *list, void *value) {
    if (list->last == NULL) {
        list->first = _makeLinkedListNode(value);
        list->last = _makeLinkedListNode(value);
    } else {
        list->last->_next = _makeLinkedListNode(value);
        list->last = list->last->_next;
    }
}

void prependLinkedList(LinkedList *list, void *value) {
    if (list->first == NULL) {
        list->first = _makeLinkedListNode(value);
        list->last = _makeLinkedListNode(value);
    } else {
        LinkedListNode *previousFirst = list->first;
        list->first = _makeLinkedListNode(value);
        list->first->_next = previousFirst;
    }
}

LinkedListNode *iteratorLinkedList(LinkedList *list) {
    return list->first;
}

LinkedListNode *nextIteratorLinkedList(LinkedListNode *node) {
    return node->_next;
}

int getValueIteratorIntLinkedList(LinkedListNode *node) {
    return (long) node->value;
}

LinkedList *mapLinkedList(LinkedList *list, void *(*transformer)(void*)) {
    LinkedList *newList = makeLinkedList();
    LinkedListNode *iter = list->first;

    while(iter != NULL) {
        appendLinkedList(newList, transformer(iter->value));
    }

    return newList;
}
