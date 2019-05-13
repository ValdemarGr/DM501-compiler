//
// Created by valde on 5/13/19.
//


#include "sandboxer.h"
#include "ast_to_ir/intermediate_representation.h"
#include <unistd.h>
#include <stdio.h>

int sandboxBody(Body *body) {
    Instructions *instructions = generateInstructionTree(body);
    FILE * pFile;
    pFile = fopen("/tmp/KITTY_SANDBOX.s", "w+");

    //Prepend mandatory instructions
    Instructions *head = newInstruction();
    head->kind = METADATA_BEGIN_BODY_BLOCK;

    //Instructions *i1 = newInstruction();
    //i1->kind = METADATA_CREATE_MAIN;
    //i1->val.mainHeader.pointerSet = initHeadedSortedSet();
    //i1->val.mainHeader.tableForFunction = initSymbolTable();
    head->next = instructions;
    //i1->next = instructions;

    generate(pFile, head);
    fflush(pFile);

    system("gcc -no-pie -o /tmp/KITTY_SANDBOX_OUT /tmp/KITTY_SANDBOX.s");
    system("/tmp/KITTY_SANDBOX_OUT > /tmp/KITTY_SANDBOX_RESULT");

    fclose(pFile);

    pFile = fopen("/tmp/KITTY_SANDBOX_RESULT", "r");
    char line[128];
    fgets(line, sizeof(line), pFile);
    return (int)atoi(line);
}

