//
// Created by valde on 2/25/19.
//

#include "symbol_from_ast.h"
#include "../ast/tree.h"

void symbolTableFromDeclaration(SymbolTable *tableForThisScope, Declaration *declaration) {

    de

}

void symbolTableFromStatement(SymbolTable *tableForThisScope, Statement *statement) {

}

SymbolTable *symbolFromFucntion(Function *fnc) {
    DeclarationList *declarationList = fnc->body->declarationList;
    StatementList *statementList = fnc->body->statementList;

    DeclarationList *paramList = fnc->head->declarationList;

    SymbolTable *scope = initSymbolTable();

    while (declarationList != NULL) {


        declarationList = declarationList->next;
    }
}

//We have to treat things a bit differently in global scope
//We do not allow some free floating statements in global space for instance; an if statement for instance
SymbolTable *symbolFromAst(Body *body) {
    DeclarationList *declarationList = body->declarationList;
    StatementList *statementList = body->statementList;
    SymbolTable *globalScope = initSymbolTable();

    while (declarationList != NULL) {

        switch (declarationList->declaration->kind) {
            case declVarK:
                break;
            case declVarsK:
                break;
            case declTypeK:

                break;
            case declFuncK:
                symbolFromFucntion(declarationList->declaration->val.functionD.function);
                break;
        }

        declarationList = declarationList->next;
    }

    while (statementList != NULL) {

        switch (statementList->statement->kind) {
            //Catch all "non garbage" statmeents


            default:
                return NULL;
                break;
        }

        declarationList = statementList->next;
    }
}


