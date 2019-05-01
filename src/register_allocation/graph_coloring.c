//
// Created by Andreas Mortensen on 30-04-2019.
//

#include "graph_coloring.h"
#include "stdlib.h"
#include "../utils/stack.h"
#include "../utils/sortedset.h"
#include "../utils/memory.h"
#include <stdbool.h>
extern size_t currentTemporary;

typedef struct Node {
    int  value;
    struct Node* next;
} Node;



int *colorGraph(SortedSet *livenessResult[], int numberOfSets,  int colors){
    int max_size = (int)currentTemporary;
    Node *graph = (Node *) malloc(sizeof(Node) * max_size);
    int *color_overview = (int *) malloc(sizeof(int) * max_size);
    memset(color_overview, -1, sizeof(int) * max_size);

    //Init Empty graph
    for (int i = 0; i < max_size; i++) {
        graph[i].value = -1;
        graph[i].next = NULL;
    }

    //Fill Graph
    for (int set = 0; set < numberOfSets; ++set) {
        SortedSet pos1 = *livenessResult[set];
        while (pos1._next != NULL){
            SortedSet pos2 = *livenessResult[set];
            while (pos2._next != NULL){
                if(pos1.data != pos2.data){
                    Node *iterator = &graph[pos1.data];
                    while (iterator->next != NULL){
                        iterator = iterator->next;
                        if(iterator->value == pos1.data){
                            break;
                        }
                        if(iterator->next == NULL){
                            Node *tempNode = NEW(Node);
                            tempNode->value = pos2.data;
                            tempNode->next = NULL;
                            iterator->next = tempNode;
                            graph[pos2.data].value++;
                            break;
                        }
                    }
                }
            }
        }
    }


    Stack *stack = initStack();
    bool couldSimplify = true;

    //Simplify
    while (couldSimplify == true){
        couldSimplify = false;
        for (int i = 0; i < max_size; i++){
            if ( 0 <= graph[i].value && graph[i].value < colors){
                Node iterator = graph[i];
                while (iterator.next != NULL) {
                    iterator = *iterator.next;
                    graph[iterator.value].value--;
                }
                graph[i].value = i;
                push(stack, (void *) &graph[i]);
                graph[i].value = -1;
                couldSimplify = true;
            }
        }
    }

    //Color nodes
    while (isEmpty(stack) == false) {
        Node poppedNode = *(Node *) pop(stack);
        bool colorsUsedByNeighbors[colors];
        memset(colorsUsedByNeighbors, false, sizeof(int) * colors);

        Node iterator = poppedNode;
        while(iterator.next != NULL){
            iterator = *iterator.next;
            if(color_overview[iterator.value] != -1){
                colorsUsedByNeighbors[color_overview[iterator.value]] = true;
            }
        }

        for (int i = 0; i < colors; i++) {
            if (colorsUsedByNeighbors[i] == false){
                color_overview[poppedNode.value] = i;
                break;
            }
        }
    }
}