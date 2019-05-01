//
// Created by Mads Grau Kristensen on 2019-05-01.
//

#include "int_linked_list.h"
#include "memory.h"

IntBoxLinkedList *makeIntBox(int value) {
    IntBoxLinkedList *box = NEW(IntBoxLinkedList);
    box->value = value;
    return box;
}

IntLinkedList *makeIntLinkedList() {
    return makeLinkedList();
}

void appendIntLinkedList(IntLinkedList *list, int value) {
    appendLinkedList(list, makeIntBox(value));
}

void prependIntLinkedList(IntLinkedList *list, int value) {
    prependLinkedList(list, makeIntBox(value));
}

IntLinkedListNode *iteratorIntLinkedList(IntLinkedList *list) {
    return (IntLinkedListNode *) iteratorLinkedList(list);
}

IntLinkedListNode *nextIteratorIntLinkedList(IntLinkedListNode *node) {
    return (IntLinkedListNode *) nextIteratorLinkedList((LinkedListNode *) node);
}

IntLinkedList *mapIntLinkedList(IntLinkedList *list, int (*transformer)(int)) {
    return mapLinkedList(list, (void *(*)(void *)) transformer);
}
