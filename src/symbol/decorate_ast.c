//
// Created by valde on 2/25/19.
//

#include "decorate_ast.h"
#include "../ast/tree.h"
#include "../error/error.h"

Error *decorateNestedStatementBody(Statement *statement, SymbolTable *symbolTable) {
    Error *e = NULL;
    StatementList *statementList = NULL;
    SymbolTable *child = NULL;

    statement->symbolTable = symbolTable;

    switch (statement->kind) {
        //Catch all non-garbage statements
        case statIfK:
            child = scopeSymbolTable(symbolTable);
            e = decorateNestedStatementBody(statement->val.ifD.statement, child);
            if (e != NULL) return e;
            break;
        case statIfElK:
            child = scopeSymbolTable(symbolTable);
            e = decorateNestedStatementBody(statement->val.ifElD.statement, child);
            if (e != NULL) return e;
            child = scopeSymbolTable(symbolTable);
            e = decorateNestedStatementBody(statement->val.ifElD.elseStatement, child);
            if (e != NULL) return e;
            break;
        case statWhileK:
            child = scopeSymbolTable(symbolTable);
            e = decorateNestedStatementBody(statement->val.whileD.statement, child);
            if (e != NULL) return e;
            break;
        //Maybe we're in a multi statement body?
        case stmListK:
            statementList = statement->val.stmListD.statementList;

            while (statementList != NULL) {
                child = scopeSymbolTable(symbolTable);
                e = decorateNestedStatementBody(statementList->statement, child);
                if (e != NULL) return e;
                statementList = statementList->next;
            }

            break;
        default:
            break;
    }

    return NULL;
}

Error *decorateDeclaration(Declaration *declaration, SymbolTable *symbolTable) {
    Error *e = NULL;
    Declaration *varList = NULL;
    SymbolTable *child = NULL;
    VarDelList *functionParams = NULL;

    switch (declaration->kind) {
        case declVarK:
            putSymbol(symbolTable,
                      declaration->val.varD.id,
                      declaration->val.varD.type,
                      false);
            break;
        case declVarsK:
            varList = declaration;

            while (varList != NULL) {

                varList->symbolTable = symbolTable;
                e = decorateDeclaration(varList->val.varsD.var, symbolTable);
                if (e != NULL) return e;

                if (varList->kind == declVarsK) {
                    varList = varList->val.varsD.next;
                } else {
                    varList = NULL;
                }
            }

            break;
        case declTypeK:
            putSymbol(symbolTable,
                      declaration->val.typeD.id,
                      declaration->val.typeD.type,
                      false);
            break;
            //This can never happen in non-global scope, weeder will catch this
        case declFuncK:
            child = scopeSymbolTable(symbolTable);

            //We need to account for the parameters
            //This is done in a somewhat hacky way
            //We name the arguments from 0 to x and append them to the function name like following
            //functionname-param-0, functionname-param-1, functionname-param-2...
            //This is so we can look them up in the future instead of researching the AST
            //Also future expansion like lambdas will be easier and more flexible this way
            functionParams = declaration->val.functionD.function->head->declarationList;

            int paramNum = 0;

            while (functionParams != NULL) {
                char* fncName = declaration->val.functionD.function->head->indentifier;

                char *paramName = (char*)malloc(sizeof(char) * (strlen(fncName) + 10));

                char asString[16];
                sprintf(asString, "%i", paramNum);

                strcat(paramName, fncName);
                strcat(paramName, FUNCTION_PARAM_SUFFIX);
                strcat(paramName, asString);

                putSymbol(child,
                          paramName,
                        functionParams->type,
                        false);

                paramNum++;
                functionParams = functionParams->next;
            }

            //We have also gotta put the function type here as well
            putSymbol(symbolTable,
                    declaration->val.functionD.function->head->indentifier,
                    declaration->val.functionD.function->head->returnType,
                    true);


            e = decorateAstWithSymbols(declaration->val.functionD.function->body, child);
            if (e != NULL) return e;
            break;
        default:
            break;
    }

    return NULL;
}

//We have to treat things a bit differently in global scope
//We do not allow some free floating statements in global space for instance; an if statement for instance
Error *decorateAstWithSymbols(Body *body, SymbolTable *symbolTable) {
    DeclarationList *declarationList = body->declarationList;
    StatementList *statementList = body->statementList;
    Declaration *varList = NULL;
    SymbolTable *child = NULL;
    Error *e = NULL;

    while (declarationList != NULL) {
        //Set the decls scope
        declarationList->declaration->symbolTable = symbolTable;

        e = decorateDeclaration(declarationList->declaration, symbolTable);
        if (e != NULL) return e;

        declarationList = declarationList->next;
    }

    while (statementList != NULL) {

        e = decorateNestedStatementBody(statementList->statement, symbolTable);
        if (e != NULL) return e;

        statementList = statementList->next;
    }

    return NULL;
}


