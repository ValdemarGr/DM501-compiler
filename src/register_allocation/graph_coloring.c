//
// Created by Andreas Mortensen on 30-04-2019.
//

#include "graph_coloring.h"
#include "stdlib.h"
#include "../utils/stack.h"
#include "../utils/sortedset.h"
#include "../utils/memory.h"
#include <stdbool.h>
extern size_t maxTemporary;

typedef struct Node {
    int  value;
    struct Node* next;
    bool beenSimplified;
} Node;



int *colorGraph(SortedSet **livenessResult, int numberOfSets,  int colors){
    int max_size = (int)maxTemporary;
    Node *graph = (Node *) malloc(sizeof(Node) * max_size);
    int *color_overview = (int *) malloc(sizeof(int) * max_size);
    memset(color_overview, -1, sizeof(int) * max_size);

    //Init Empty graph
    for (int i = 0; i < max_size; i++) {
        graph[i].value = -1;
        graph[i].next = NULL;
        graph[i].beenSimplified = false;
    }

    //Fill Graph
    for (int set = 0; set < numberOfSets; ++set) {
        SortedSet *pos1 = livenessResult[set];
        pos1 = iterateSortedSet(pos1);
        while (pos1 != NULL){
            SortedSet *pos2 = livenessResult[set];
            pos2 = iterateSortedSet(pos2);
            while (pos2 != NULL){
                if (pos1->data == 0 || pos2->data == 0) {
                    pos2 = iterateSortedSet(pos2);
                    continue;
                }

                if(pos1->data != pos2->data){
                    Node *iterator = &graph[pos1->data];
                    while (iterator != NULL){
                        if(iterator->next == NULL){
                            Node *tempNode = NEW(Node);
                            tempNode->value = pos2->data;
                            tempNode->next = NULL;
                            iterator->next = tempNode;
                            graph[pos2->data].value++;
                            break;
                        }
                        if(iterator->value == pos1->data){
                            break;
                        }
                        iterator = iterator->next;
                    }
                }
                pos2 = iterateSortedSet(pos2);
            }
            pos1 = iterateSortedSet(pos1);
        }
    }


    Stack *stack = initStack();
    bool couldSimplify = true;
    int stackSize = 0;
    int nodesRemoved = 1;
    Node *toRemove = NULL;

    graph[0].beenSimplified = true;
    //Simplify
    while (couldSimplify == true){
        couldSimplify = false;
        for (int i = 1; i < max_size; i++){
            if (graph[i].value < colors && !graph[i].beenSimplified){
                Node *iterator = graph[i].next;
                while (iterator != NULL) {
                    if (!graph[iterator->value].beenSimplified) {
                        graph[iterator->value].value--;
                    }
                    iterator = iterator->next;
                }
                graph[i].value = i;
                push(stack, (void *) &graph[i]);
                stackSize++;
                graph[i].beenSimplified = true;
                couldSimplify = true;
                nodesRemoved++;
            } else {
                toRemove = &graph[i];
            }
        }

        if (!couldSimplify && nodesRemoved < max_size) {
                Node *iterator = toRemove->next;
                while (iterator != NULL) {
                    if (!graph[iterator->value].beenSimplified) {
                        graph[iterator->value].value--;
                    }
                    iterator = iterator->next;
                }
                toRemove->beenSimplified = true;
                couldSimplify = true;
                nodesRemoved++;
        }
    }

    bool colorFound;
    Node *poppedNode;
    bool colorsUsedByNeighbors[colors];
    Node *iterator;
    color_overview[0] = 0;
    //Color nodes
    while (isEmpty(stack) == false) {
        poppedNode = pop(stack);
        memset(colorsUsedByNeighbors, false, sizeof(bool) * colors);
        colorFound = false;

        if (poppedNode->value == 0) {
            continue;
        }

        iterator = poppedNode;
        iterator = iterator->next;
        while(iterator != NULL){
            if(color_overview[iterator->value] != -1){
                colorsUsedByNeighbors[color_overview[iterator->value]] = true;
            }
            iterator = iterator->next;
        }

        for (int i = 1; i < colors; i++) {
            if (colorsUsedByNeighbors[i] == false){
                color_overview[poppedNode->value] = i;
                colorFound = true;
                break;
            }
        }

        if (!colorFound) {
            color_overview[poppedNode->value] = -1;
        }
    }
    return color_overview;
}