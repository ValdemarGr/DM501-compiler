//
// Created by Andreas Mortensen on 30-04-2019.
//

#include "graph_coloring.h"
#include "stdlib.h"
#include "../utils/stack.h"
#include <stdbool.h>

typedef struct Node {
    int  value;
    struct Node* next;
} Node;



int *colorGraph(/*some data*/ int size, int colors){
    Node *graph = (Node *) malloc(sizeof(Node) * size);

    //@TODO Init graph

    int *color_overview = (int *) malloc(sizeof(int) * size);
    memset(color_overview, -1, sizeof(int) * size);
    Stack *stack = initStack();
    bool couldSimplify = true;

    //Simplify
    while (couldSimplify == true){
        couldSimplify = false;
        for (int i = 0; i < size; ++i){
            if ( 0 <= graph[i].value && graph[i].value < colors){
                Node tempNode = graph[i];
                while (tempNode.next != NULL) {
                    tempNode = *tempNode.next;
                    graph[tempNode.value].value--;
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

        Node tempNode = poppedNode;
        while(tempNode.next != NULL){
            tempNode = *tempNode.next;
            if(color_overview[tempNode.value] != -1){
                colorsUsedByNeighbors[color_overview[tempNode.value]] = true;
            }
        }

        for (int i = 0; i < colors; ++i) {
            if (colorsUsedByNeighbors[i] == false){
                color_overview[poppedNode.value] = i;
                break;
            }
        }
    }
}