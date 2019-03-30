#include "abstract_asm_tree.h"
#include "../ast/tree.h"

static int uniqueId = 0;

Instructions *createVariable() {

}

Instructions *generateInstructionTreeForStatement(Statement *statement) {


    switch (statement->kind) {
        case statReturnK:
            break;
        case statWriteK:
            break;
        case statAllocateK:
            break;
        case statAllocateLenK:
            break;
        case statIfK:
            break;
        case statIfElK:
            break;
        case statWhileK:
            break;
        case stmListK:
            break;
        case assignmentK:
            break;
    }

    return NULL;
}

Instructions *generateInstructionTreeForDeclaration(Declaration *declaration) {

    switch (declaration->kind) {
        case declVarK: {
            SYMBOL *symbol = getSymbol(declaration->symbolTable, declaration->val.varD.id);
            Instructions *ret = NEW(Instructions);
            ret->next = NULL;
            ret->kind = INSTRUCTION_VAR;
            ret->val.var = symbol->uniqueId;
            return ret;
        }
        break;
        case declVarsK: {
            Instructions *head = NULL;
            Instructions *current = NULL;

            Declaration *declCurrent = declaration;
            while (declCurrent->kind == declVarsK) {
                if (head == NULL) {
                    head = NEW(Instructions);
                    current = head;
                } else {

                }

                declCurrent->val.varsD.var;

                declCurrent = declCurrent->val.varsD.next;
            }

            Declaration *thisVar = declaration->val.varsD.var;

            SYMBOL *symbol = getSymbol(declaration->symbolTable, thisVar->val.varD.id);

            head->next = NULL;

            Instructions *out = generateInstructionTreeForDeclaration(declaration->val.varsD.next);

            ret->next = NULL;
            ret->kind = INSTRUCTION_VAR;
            ret->val.var = symbol->uniqueId;



            return ret;
        }
        break;
        case declTypeK:
            break;
        case declFuncK:
            break;
        case declValK:
            break;
        case declClassK:
            break;
    }
}

Instructions *generateInstructionTree(Body *body) {
    DeclarationList *declarationList = body->declarationList;
    StatementList *statementList = body->statementList;
    Instructions *head = NULL;
    Instructions *current = NULL;

    while (declarationList != NULL) {
        if (head == NULL) {
            head = generateInstructionTreeForDeclaration(declarationList->declaration);
            current = head;
        } else {
            current = generateInstructionTreeForDeclaration(declarationList->declaration);
        }

        declarationList = declarationList->next;
    }

    while (statementList != NULL) {
        if (head == NULL) {
            head = generateInstructionTreeForStatement(statementList->statement);
            current = head;
        } else {
            current = generateInstructionTreeForStatement(statementList->statement);
        }

        statementList = statementList->next;
    }

}