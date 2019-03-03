//
// Created by valde on 2/25/19.
//

#include "decorate_ast.h"
#include "../ast/tree.h"
#include "../error/error.h"
#include "symbol.h"

Type *evaluateExpressionType(Expression *expression, SymbolTable *symbolTable);
void decorateFunction(char *id, Type *returnType, SymbolTable *symbolTable,
                     VarDelList *params, Body *body, int stmDeclNum);

void alterIdTypesToGenerics(Type *tpe, SymbolTable *symbolTable) {
    if (tpe->kind == typeIdK) {
        SYMBOL *symbol = getSymbol(symbolTable, tpe->val.idType.id);

        if (symbol != NULL) {
            if (symbol->value->kind == typeK) {
                if (symbol->value->val.typeD.tpe->kind == typeGenericK) {
                    tpe->kind = typeGenericK;
                    tpe->val.typeGeneric.genericName = tpe->val.idType.id;
                }
            }
        }
    }
}

//We want to look for R-value lambdas to decorate their bodies
//Function calls can contain a lambda
Error *decorateRValue(Expression *exp, SymbolTable *symbolTable) {
    Error *e = NULL;

    if (exp->kind == termK) {
        if (exp->val.termD.term->kind == functionCallK) {

            ExpressionList *expressionList = exp->val.termD.term->val.functionCallD.expressionList;

            int counter = 0;

            while (expressionList != NULL) {

                Type* valType = evaluateExpressionType(expressionList->expression, symbolTable);

                dumpSymbolTable(symbolTable);
                //Check if the lambda is a variable or an R-value
                //If it is a variable,  we don't need to do any of the R-value decorating this time
                if (valType->kind == typeLambdaK && expressionList->expression->val.termD.term->kind != variableK) {
                    Lambda *lambda = expressionList->expression->val.termD.term->val.lambdaD.lambda;
                    char intToString[16];

                    sprintf(intToString, "%i", counter);

                    //Give the lambda a unique id
                    int suffix_len = strlen(LAMBDA_SUFFIX);

                    char *function_name = exp->val.termD.term->val.functionCallD.functionId;
                    int var_len = strlen(function_name);

                    char *lambda_id = (char*)malloc(sizeof(char) * (var_len + suffix_len) + 16);

                    strcat(lambda_id, function_name);
                    strcat(lambda_id, LAMBDA_SUFFIX);
                    strcat(lambda_id, intToString);

                    decorateFunction(lambda_id,
                                     lambda->returnType,
                                     symbolTable,
                                     lambda->declarationList,
                                     lambda->body,
                                     0);
                }

                expressionList = expressionList->next;
            }

        }
    } else if (exp->kind == opK) {
        decorateRValue(exp->val.op.left, symbolTable);
        decorateRValue(exp->val.op.right, symbolTable);
    }
}

void decorateFunction(char *id, Type *returnType, SymbolTable *symbolTable,
                      VarDelList *params, Body *body, int stmDeclNum) {
    Value *value = NULL;
    SymbolTable *child = scopeSymbolTable(symbolTable);

    //Put the function definition in the scope
    value = NEW(Value);

    value->kind = typeFunctionK;
    value->val.typeFunctionD.tpe = params;
    value->val.typeFunctionD.returnType = returnType;

    putSymbol(symbolTable,
              id,
              value,
              stmDeclNum);

    //Put the parameters in the child scope
    while (params != NULL) {
        alterIdTypesToGenerics(params->type, symbolTable);

        value = NEW(Value);

        value->kind = typeK;
        value->val.typeD.tpe = params->type;

        putSymbol(child,
                  params->identifier,
                  value, stmDeclNum);

        params =params->next;
    }

    //Recurse to body
    decorateAstWithSymbols(body, child);
}

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
        //All statements that can create an R-value lambda definition
        case assignmentK:
            //If the lambda is R-value assigned to a var
            if (statement->val.assignmentD.exp->kind == termK) {
                if (statement->val.assignmentD.exp->val.termD.term->kind == lambdaK) {
                    //Give the lambda a name for future reference
                    int suffix_len = strlen(LAMBDA_SUFFIX);

                    //Unwrap the whole variable
                    Variable *var = statement->val.assignmentD.var;

                    while (var->kind != varIdK) {
                        switch (var->kind) {
                            case arrayIndexK:
                                var = var->val.arrayIndexD.var;
                                break;
                            case recordLookupK:
                                var = var->val.recordLookupD.var;
                                break;
                            default:
                                break;
                        }
                    }

                    char *var_name = var->val.idD.id;
                    int var_len = strlen(var_name);

                    char *lambda_id = (char*)malloc(sizeof(char) * (var_len + suffix_len));

                    strcat(lambda_id, var_name);
                    strcat(lambda_id, LAMBDA_SUFFIX);

                    decorateFunction(lambda_id,
                            statement->val.assignmentD.exp->val.termD.term->val.lambdaD.lambda->returnType,
                            symbolTable,
                            statement->val.assignmentD.exp->val.termD.term->val.lambdaD.lambda->declarationList,
                            statement->val.assignmentD.exp->val.termD.term->val.lambdaD.lambda->body,
                            statement->internal_stmDeclNum);
                }
            }
            //If the rhs is an expression that contains a function call we have to check if the call itself
            //Has an r-value lambda
            decorateRValue(statement->val.assignmentD.exp, statement->symbolTable);
            break;
        case statReturnK:
            decorateRValue(statement->val.returnD.exp, statement->symbolTable);
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
    Value *value = NULL;
    Type* valType;

    switch (declaration->kind) {
        case declVarK:
            alterIdTypesToGenerics(declaration->val.varD.type, symbolTable);

            value = NEW(Value);

            value->kind = typeK;
            value->val.typeD.tpe = declaration->val.varD.type;


            putSymbol(symbolTable,
                      declaration->val.varD.id,
                      value,
                      declaration->internal_stmDeclNum);
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
            alterIdTypesToGenerics(declaration->val.typeD.type, symbolTable);

            value = NEW(Value);

            value->kind = typeK;
            value->val.typeD.tpe = declaration->val.typeD.type;

            putSymbol(symbolTable,
                      declaration->val.typeD.id,
                      value,
                      declaration->internal_stmDeclNum);
            break;
            //This can never happen in non-global scope, weeder will catch this
        case declFuncK:
            alterIdTypesToGenerics(declaration->val.functionD.function->head->returnType, symbolTable);

            decorateFunction(declaration->val.functionD.function->head->indentifier,
                             declaration->val.functionD.function->head->returnType,
                             symbolTable,
                             declaration->val.functionD.function->head->declarationList,
                             declaration->val.functionD.function->body,
                             declaration->internal_stmDeclNum);

            break;
        case declValK:
            //Determine the type by the rhs type
            valType = evaluateExpressionType(declaration->val.valD.rhs, symbolTable);

            alterIdTypesToGenerics(valType, symbolTable);

            //Update the decl
            declaration->val.valD.tpe = valType;

            //If its a lambda, we want to decorate it
            if (valType->kind == typeLambdaK) {
                Lambda *lambda = declaration->val.valD.rhs->val.termD.term->val.lambdaD.lambda;

                decorateFunction(declaration->val.valD.id,
                                 lambda->returnType,
                                 symbolTable,
                                 lambda->declarationList,
                                 lambda->body,
                                 declaration->internal_stmDeclNum);
            } else {
                value = NEW(Value);

                value->kind = typeK;
                value->val.typeD.tpe = valType;

                putSymbol(symbolTable,
                          declaration->val.valD.id,
                          value,
                          declaration->internal_stmDeclNum);
            }

            break;
        case declClassK:
            value = NEW(Value);
            value->kind = symTypeClassK;
            value->val.typeClassD.declarationList = declaration->val.classD.declarationList;

            SymbolTable *newSt = scopeSymbolTable(symbolTable);

            putSymbol(symbolTable,
                      declaration->val.classD.id,
                      value,
                      declaration->internal_stmDeclNum);


            //Also remember the generic type parameters
            TypeList *generics = declaration->val.classD.genericTypeParameters;
            //The generics also need their indexes reversed
            while (generics != NULL) {
                value = NEW(Value);

                value->kind = typeK;
                value->val.typeD.tpe = generics->type;

                putSymbol(newSt,
                          generics->type->val.typeGeneric.genericName,
                          value,
                          declaration->internal_stmDeclNum);

                generics = generics->next;
            }


            DeclarationList *declarationList = declaration->val.classD.declarationList;

            while (declarationList != NULL) {
                //Set the decls scope
                declarationList->declaration->symbolTable = symbolTable;

                e = decorateDeclaration(declarationList->declaration, newSt);
                if (e != NULL) return e;

                declarationList = declarationList->next;
            }

            break;
        default:
            break;
    }

    return NULL;
}

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


