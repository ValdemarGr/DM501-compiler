//
// Created by valde on 2/25/19.
//

#include "decorate_ast.h"
#include "../ast/tree.h"
#include "../error/error.h"
#include "symbol.h"

int lambdaCount = 0;
bool inClassContext = false;
bool inLambdaContextCurrently = false;
bool inConstructorContext = false;

void findAndDecorateFunctionCall(Expression *expression, SymbolTable *symbolTable);
Type *unwrapTypedef(Type *type, SymbolTable *symbolTable);
Type *evaluateExpressionType(Expression *expression, SymbolTable *symbolTable);
void decorateFunction(char *id, Type *returnType, SymbolTable *symbolTable,
                     VarDelList *params, Body *body, int stmDeclNum, bool isConst, bool isLambda, int lambdaId);

void alterIdTypesToGenerics(Type *tpe, SymbolTable *symbolTable) {
    SYMBOL *symbol;
    VarDelList *vdl;
    TypeList *tpeLst;

    switch (tpe->kind) {
        case typeIdK:
            symbol = getSymbol(symbolTable, tpe->val.idType.id);

            if (symbol != NULL) {
                if (symbol->value->kind == typeK) {
                    if (symbol->value->val.typeD.tpe->kind == typeGenericK) {
                        //Try to get subtype

                        tpe->kind = typeGenericK;
                        tpe->val.typeGeneric.genericName = tpe->val.idType.id;
                        tpe->val.typeGeneric.subType = symbol->value->val.typeD.tpe->val.typeGeneric.subType;
                    }
                }
            }
            break;
        //Unwrap
        case typeArrayK:
            alterIdTypesToGenerics(tpe->val.arrayType.type, symbolTable);
            break;
        case typeRecordK:
            vdl = tpe->val.recordType.types;

            while (vdl != NULL) {
                alterIdTypesToGenerics(vdl->type, symbolTable);
                vdl = vdl->next;
            }

            break;
        case typeLambdaK:
            tpeLst = tpe->val.typeLambdaK.typeList;

            while (tpeLst != NULL) {
                alterIdTypesToGenerics(tpeLst->type, symbolTable);
                tpeLst = tpeLst->next;
            }

            alterIdTypesToGenerics(tpe->val.typeLambdaK.returnType, symbolTable);

            break;
        case typeClassK:
            tpeLst = tpe->val.typeClass.genericBoundValues;

            while (tpeLst != NULL) {
                alterIdTypesToGenerics(tpeLst->type, symbolTable);
                tpeLst = tpeLst->next;
            }

            break;
        default:
            return;
            break;

    }
}

//We want to look for R-value lambdas to decorate their bodies
//Function calls can contain a lambda
Error *decorateRValue(Expression *exp, SymbolTable *symbolTable) {
    Error *e = NULL;

    if (exp->kind == termK) {
        if (exp->val.termD.term->kind == functionCallK) {

            ExpressionList *expressionList = exp->val.termD.term->val.functionCallD.expressionList;

            while (expressionList != NULL) {

                Type* valType = evaluateExpressionType(expressionList->expression, symbolTable);

                //Check if the lambda is a variable or an R-value
                //If it is a variable,  we don't need to do any of the R-value decorating this time
                if (valType->kind == typeLambdaK && expressionList->expression->val.termD.term->kind != variableK) {
                    Lambda *lambda = expressionList->expression->val.termD.term->val.lambdaD.lambda;
                    decorateFunction(NULL,
                                     lambda->returnType,
                                     symbolTable,
                                     lambda->declarationList,
                                     lambda->body,
                                     0,
                                     false,
                                     true,
                                     lambda->id);
                }

                expressionList = expressionList->next;
            }

        } else if (exp->val.termD.term->kind == shorthandCallK) {

            ExpressionList *expressionList = exp->val.termD.term->val.shorthandCallD.expressionList;

            while (expressionList != NULL) {

                Type* valType = evaluateExpressionType(expressionList->expression, symbolTable);

                //Check if the lambda is a variable or an R-value
                //If it is a variable,  we don't need to do any of the R-value decorating this time
                if (valType->kind == typeLambdaK && expressionList->expression->val.termD.term->kind != variableK) {
                    Lambda *lambda = expressionList->expression->val.termD.term->val.lambdaD.lambda;
                    decorateFunction(NULL,
                                     lambda->returnType,
                                     symbolTable,
                                     lambda->declarationList,
                                     lambda->body,
                                     0,
                                     false,
                                     true,
                                     lambda->id);
                }

                expressionList = expressionList->next;
            }
        }
    } else if (exp->kind == opK) {
        decorateRValue(exp->val.op.left, symbolTable);
        decorateRValue(exp->val.op.right, symbolTable);
    }

    return e;
}

void decorateFunction(char *id, Type *returnType, SymbolTable *symbolTable,
                      VarDelList *params, Body *body, int stmDeclNum, bool isConst, bool isLambda, int lambdaId) {
    Value *value = NULL;
    SymbolTable *child = scopeSymbolTable(symbolTable);
    VarDelList *vdl = params;

    //Put the function definition in the scope
    value = NEW(Value);

    value->kind = typeFunctionK;
    value->val.typeFunctionD.tpe = vdl;
    value->val.typeFunctionD.returnType = returnType;
    value->val.typeFunctionD.isLambda = isLambda;
    value->val.typeFunctionD.lambdaId = lambdaId;

    if (id != NULL) {
        putSymbol(symbolTable,
                  id,
                  value,
                  stmDeclNum,
                  isConst);
    }

    //Put the parameters in the child scope
    while (vdl != NULL) {
        alterIdTypesToGenerics(vdl->type, symbolTable);

        value = NEW(Value);

        value->kind = typeK;
        value->val.typeD.tpe = vdl->type;
        value->val.typeD.isTypedef = false;

        putSymbol(child,
                  vdl->identifier,
                  value, stmDeclNum,
                  true);

        vdl = vdl->next;
    }

    //Make room for 1 more potential argument (lambda capture context)
    if (isLambda) {
        child->nextSymbolId++;
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
            e = decorateNestedStatementBody(statement->val.ifD.statement, symbolTable);
            if (e != NULL) return e;
            break;
        case statIfElK:
            e = decorateNestedStatementBody(statement->val.ifElD.statement, symbolTable);
            if (e != NULL) return e;
            e = decorateNestedStatementBody(statement->val.ifElD.elseStatement, symbolTable);
            if (e != NULL) return e;
            break;
        case statWhileK:
            e = decorateNestedStatementBody(statement->val.whileD.statement, symbolTable);
            if (e != NULL) return e;
            break;
        //Maybe we're in a multi statement body?
        case stmListK:
            statementList = statement->val.stmListD.statementList;

            while (statementList != NULL) {
                //child = scopeSymbolTable(symbolTable);
                e = decorateNestedStatementBody(statementList->statement, symbolTable);
                if (e != NULL) return e;
                statementList = statementList->next;
            }

            break;
        //All statements that can create an R-value lambda definition
        case assignmentK:
            //If the lambda is R-value assigned to a var
            if (statement->val.assignmentD.exp->kind == termK) {
                if (statement->val.assignmentD.exp->val.termD.term->kind == lambdaK) {
                    if (inConstructorContext) {
                        e = NEW(Error);

                        e->error = NO_LAMBDA_IN_CONSTRUCTOR;
                        e->location = statement->val.assignmentD.exp->val.termD.term->location;

                        return e;
                    }
                    if (inLambdaContextCurrently) {
                        e = NEW(Error);

                        e->error = NESTED_LAMBDA;
                        e->location = statement->val.assignmentD.exp->val.termD.term->location;

                        return e;
                    } else

                    inLambdaContextCurrently = true;
                    //Give the lambda a name for future reference
                    statement->val.assignmentD.exp->val.termD.term->val.lambdaD.lambda->inClassContext = inClassContext;

                    decorateFunction(NULL,
                            statement->val.assignmentD.exp->val.termD.term->val.lambdaD.lambda->returnType,
                            symbolTable,
                            statement->val.assignmentD.exp->val.termD.term->val.lambdaD.lambda->declarationList,
                            statement->val.assignmentD.exp->val.termD.term->val.lambdaD.lambda->body,
                            statement->internal_stmDeclNum,
                            false,
                            true,
                            statement->val.assignmentD.exp->val.termD.term->val.lambdaD.lambda->id);

                    //Remember to assign the id to the type
                    unwrapVariable(statement->val.assignmentD.var, symbolTable)->val.typeLambdaK.lambdaId =
                            statement->val.assignmentD.exp->val.termD.term->val.lambdaD.lambda->id;
                    inLambdaContextCurrently = false;
                }
            }
            //If the rhs is an expression that contains a function call we have to check if the call itself
            //Has an r-value lambda
            decorateRValue(statement->val.assignmentD.exp, statement->symbolTable);
            break;
        case statReturnK:
            //decorateRValue(statement->val.returnD.exp, statement->symbolTable);
            findAndDecorateFunctionCall(statement->val.returnD.exp, statement->symbolTable);
            break;
        case emptyK:
            findAndDecorateFunctionCall(statement->val.empty.exp, statement->symbolTable);
            break;
        default:
            break;
    }

    return NULL;
}

void findAndDecorateFunctionCall(Expression *expression, SymbolTable *symbolTable) {


    switch (expression->kind) {
        case opK:
            findAndDecorateFunctionCall(expression->val.op.left, symbolTable);
            findAndDecorateFunctionCall(expression->val.op.right, symbolTable);
            break;
        case termK:
            if (expression->val.termD.term->kind == functionCallK) {
                //Go though all the items
                ExpressionList *expressionList = expression->val.termD.term->val.functionCallD.expressionList;

                while (expressionList != NULL) {

                    findAndDecorateFunctionCall(expressionList->expression, symbolTable);

                    expressionList = expressionList->next;
                }

            } else if (expression->val.termD.term->kind == shorthandCallK) {
                //Go though all the items
                ExpressionList *expressionList = expression->val.termD.term->val.shorthandCallD.expressionList;

                while (expressionList != NULL) {

                    findAndDecorateFunctionCall(expressionList->expression, symbolTable);

                    expressionList = expressionList->next;
                }
            } else if (expression->val.termD.term->kind == lambdaK) {
                Lambda *lambda = expression->val.termD.term->val.lambdaD.lambda;
                lambda->inClassContext = inClassContext;
                /*char intToString[16];

                sprintf(intToString, "%i", lambdaCount);
                lambdaCount++;

                //Give the lambda a unique id
                int suffix_len = strlen(LAMBDA_SUFFIX);

                char *lambda_id = (char*)malloc(sizeof(char) * (suffix_len) + 16);

                strcat(lambda_id, LAMBDA_SUFFIX);
                strcat(lambda_id, intToString);*/

                decorateFunction(NULL,
                                 lambda->returnType,
                                 symbolTable,
                                 lambda->declarationList,
                                 lambda->body,
                                 0,
                                 false,
                                 true,
                                 lambda->id);
            }
            break;
        default:
            break;
    }
}

typedef struct IdKindPair {
    char *id;
    TypeKind kind;
    struct IdKindPair *next;
} IdKindPair;

IdKindPair *pairForDeclaration(Declaration *declaration) {
    IdKindPair *head = NULL;

    switch (declaration->kind) {
        case declVarK:
            head = NEW(IdKindPair);

            head->next = NULL;
            head->id = declaration->val.varD.id;
            head->kind = declaration->val.varD.type->kind;

            break;
        case declVarsK:
            head = NEW(IdKindPair);

            head->id = declaration->val.varsD.var->val.varD.id;
            head->kind = declaration->val.varsD.var->val.varD.type->kind;

            head->next = pairForDeclaration(declaration->val.varsD.next);

            break;
        case declTypeK:
            head = NEW(IdKindPair);

            head->id = declaration->val.typeD.id;
            head->kind = declaration->val.typeD.type->kind;

            head->next = NULL;

            break;
        case declFuncK:
            return NULL;
            break;
        case declValK:
            head = NEW(IdKindPair);

            head->id = declaration->val.valD.id;
            head->kind = evaluateExpressionType(declaration->val.valD.rhs, declaration->symbolTable)->kind;

            head->next = NULL;
            break;
        case declClassK:
            return NULL;
            break;
    }

    return head;
}

Error *decorateDeclaration(Declaration *declaration, SymbolTable *symbolTable) {
    Error *e = NULL;
    Declaration *varList = NULL;
    SymbolTable *child = NULL;
    Value *value = NULL;
    Type* valType;

    switch (declaration->kind) {
        case declVarK:
            alterIdTypesToGenerics(declaration->val.varD.type, symbolTable);
            declaration->symbolTable = symbolTable;

            value = NEW(Value);

            value->kind = typeK;
            value->val.typeD.tpe = declaration->val.varD.type;
            value->val.typeD.isTypedef = false;

            putSymbol(symbolTable,
                      declaration->val.varD.id,
                      value,
                      declaration->internal_stmDeclNum,
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
            alterIdTypesToGenerics(declaration->val.typeD.type, symbolTable);

            value = NEW(Value);

            value->kind = typeK;
            value->val.typeD.isTypedef = true;
            value->val.typeD.tpe = declaration->val.typeD.type;

            putSymbol(symbolTable,
                      declaration->val.typeD.id,
                      value,
                      declaration->internal_stmDeclNum,
                      true);
            break;
            //This can never happen in non-global scope, weeder will catch this
        case declFuncK:
            if (inConstructorContext) {
                e = NEW(Error);

                e->error = NO_FUNC_IN_CONSTRUCTOR;
                e->location = declaration->location;

                return e;
            }
            if (inLambdaContextCurrently) {
                e = NEW(Error);

                e->error = NESTED_LAMBDA;
                e->location = declaration->location;

                return e;
            } else if (inClassContext) {
                e = NEW(Error);

                e->error = DECLARATIONS_IN_CLASS;
                e->location = declaration->location;

                return e;
            }

            alterIdTypesToGenerics(declaration->val.functionD.function->head->returnType, symbolTable);

            decorateFunction(declaration->val.functionD.function->head->indentifier,
                             declaration->val.functionD.function->head->returnType,
                             symbolTable,
                             declaration->val.functionD.function->head->declarationList,
                             declaration->val.functionD.function->body,
                             declaration->internal_stmDeclNum,
                             true,
                             false,
                             0);

            break;
        case declValK:
            //Determine the type by the rhs type
            valType = unwrapTypedef(evaluateExpressionType(declaration->val.valD.rhs, symbolTable), symbolTable);

            alterIdTypesToGenerics(valType, symbolTable);

            //Update the decl
            declaration->val.valD.tpe = valType;
            declaration->symbolTable = symbolTable;

            //Ensure that we decorate all r-value lambdas in function calls
            findAndDecorateFunctionCall(declaration->val.valD.rhs, symbolTable);

            //If its a lambda, we want to decorate it
            if (valType->kind == typeLambdaK && declaration->val.valD.rhs->val.termD.term->kind == lambdaK) {
                if (inConstructorContext) {
                    e = NEW(Error);

                    e->error = NO_LAMBDA_IN_CONSTRUCTOR;
                    e->location = declaration->val.valD.rhs->val.termD.term->location;

                    return e;
                }
                if (inLambdaContextCurrently) {
                    e = NEW(Error);

                    e->error = NESTED_LAMBDA;
                    e->location = declaration->val.valD.rhs->val.termD.term->location;

                    return e;
                }

                inLambdaContextCurrently = true;
                Lambda *lambda = declaration->val.valD.rhs->val.termD.term->val.lambdaD.lambda;
                lambda->inClassContext = inClassContext;
                declaration->val.valD.tpe->val.typeLambdaK.lambdaId = lambda->id;

                decorateFunction(declaration->val.valD.id,
                                 lambda->returnType,
                                 symbolTable,
                                 lambda->declarationList,
                                 lambda->body,
                                 declaration->internal_stmDeclNum,
                                 true,
                                 true,
                                 lambda->id);
                inLambdaContextCurrently = false;
            } else  {
                value = NEW(Value);

                value->kind = typeK;
                value->val.typeD.tpe = valType;
                value->val.typeD.isTypedef = false;

                putSymbol(symbolTable,
                          declaration->val.valD.id,
                          value,
                          declaration->internal_stmDeclNum,
                          true);
            }

            break;
        case declClassK:
            if (inConstructorContext) {
                e = NEW(Error);

                e->error = NO_CLASS_IN_CONSTRUCTOR;
                e->location = declaration->location;

                return e;
            }
            inClassContext = true;
            value = NEW(Value);
            SymbolTable *newSt = scopeSymbolTable(symbolTable);

            //Apply inheritance mixin before the actual class
            //For every mixin for every decl add the sym table
            DeclarationList *newHead = NULL;
            DeclarationList *newTail = NULL;
            TypeList *extensions = declaration->val.classD.extendedClasses;

            while (extensions != NULL) {
                //A bit of early type checking
                if (extensions->type->kind != typeClassK) {
                    e = NEW(Error);

                    e->error = NOT_TYPE;
                    e->val.NOT_TYPE.id = extensions->type->val.idType.id;
                    e->val.NOT_TYPE.lineno = declaration->lineno;
                    e->location = extensions->type->location;

                    return e;
                }

                SYMBOL *mixin = getSymbol(symbolTable, extensions->type->val.typeClass.classId);

                if (mixin->value->kind != symTypeClassK) {
                    e = NEW(Error);

                    e->error = NOT_CLASS;
                    e->val.NOT_CLASS.id = extensions->type->val.idType.id;
                    e->val.NOT_CLASS.lineno = declaration->lineno;
                    e->location = extensions->type->location;

                    return e;
                }

                //For true'er polymorphism, overriding can be added by traversing by collisions and always
                //preferring the current classes version
                DeclarationList *internalDeclList = mixin->value->val.typeClassD.declarationList;
                //Get the binds for the generic
                ConstMap *genericToBoundMap = initMap(10);

                TypeList *genericIter = mixin->value->val.typeClassD.generics;
                TypeList *boundIter = extensions->type->val.typeClass.genericBoundValues;

                while (genericIter != NULL && boundIter) {
                    insert(genericToBoundMap, makeCharKey(genericIter->type->val.typeGeneric.genericName), (void*)boundIter);

                    genericIter = genericIter->next;
                    boundIter = boundIter->next;
                }

                while (internalDeclList != NULL) {
                    //Check if declaration collides with current classes

                    if (newHead == NULL) {
                        newHead = NEW(DeclarationList);
                        newHead->declaration = internalDeclList->declaration;
                        newHead->declaration = NEW(Declaration);
                        memcpy(newHead->declaration, internalDeclList->declaration, sizeof(Declaration));

                        newHead->next = NULL;
                        newTail = newHead;
                    } else {
                        newTail->next = NEW(DeclarationList);
                        newTail = newTail->next;
                        newTail->next = NULL;
                        newTail->declaration = NEW(Declaration);
                        memcpy(newTail->declaration, internalDeclList->declaration, sizeof(Declaration));


                    }


                    internalDeclList = internalDeclList->next;
                }

                extensions = extensions->next;
            }

            if (newHead != NULL) {
                newTail->next = declaration->val.classD.declarationList;

                declaration->val.classD.declarationList = newHead;
                value->val.typeClassD.declarationList = newHead;
            } else {
                newHead = declaration->val.classD.declarationList;
                value->val.typeClassD.declarationList = declaration->val.classD.declarationList;
            }

            //Set all the sym tables
            value->val.typeClassD.tableForClassBody = newSt;
            DeclarationList *symSetter = newHead;

            while (symSetter != NULL) {
                symSetter->declaration->symbolTable = newSt;
                symSetter = symSetter->next;
            }

            value->kind = symTypeClassK;
            value->val.typeClassD.extendedClasses = declaration->val.classD.extendedClasses;
            value->val.typeClassD.generics = declaration->val.classD.genericTypeParameters;
            value->val.typeClassD.constructor = declaration->val.classD.constructor;

            putSymbol(symbolTable,
                      declaration->val.classD.id,
                      value,
                      declaration->internal_stmDeclNum,
                      false);

            //Also remember the generic type parameters
            TypeList *generics = declaration->val.classD.genericTypeParameters;

            while (generics != NULL) {
                value = NEW(Value);

                value->kind = typeK;
                value->val.typeD.tpe = generics->type;
                value->val.typeD.isTypedef = false;


                putSymbol(newSt,
                          generics->type->val.typeGeneric.genericName,
                          value,
                          declaration->internal_stmDeclNum,
                          false);

                generics = generics->next;
            }

            //And for the class body
            DeclarationList *declarationList = newHead;

            if (declarationList == NULL) {
                e = NEW(Error);

                e->error = EMPTY_CLASS;
                e->location = declaration->location;

                return e;
            }

            while (declarationList != NULL) {
                //No classes or funcs inside of class
                if (declarationList->declaration->kind == declClassK ||
                        declarationList->declaration->kind == declFuncK) {
                    e = NEW(Error);

                    e->error = DECLARATIONS_IN_CLASS;
                    e->val.DECLARATIONS_IN_CLASS.classId = declaration->val.classD.id;
                    e->val.DECLARATIONS_IN_CLASS.lineno = declaration->lineno;
                    e->location = declarationList->declaration->location;

                    return e;
                }

                e = decorateDeclaration(declarationList->declaration, newSt);
                if (e != NULL) return e;

                declarationList = declarationList->next;
            }

            //For the constructor
            if (declaration->val.classD.constructor != NULL) {
                Constructor *constructor = declaration->val.classD.constructor;
                //Create new sym table for constructor body
                SymbolTable *scoped = scopeSymbolTable(newSt);

                //Put params in body
                VarDelList *vdl = constructor->declarationList;
                //Put the parameters in the child scope
                while (vdl != NULL) {
                    alterIdTypesToGenerics(vdl->type, scoped);

                    value = NEW(Value);

                    value->kind = typeK;
                    value->val.typeD.tpe = vdl->type;
                    value->val.typeD.isTypedef = false;

                    putSymbol(scoped,
                              vdl->identifier,
                              value, 0,
                              true);

                    vdl = vdl->next;
                }

                inConstructorContext = true;
                //Generate as function
                //Fix up body
                Error *er = decorateAstWithSymbols(constructor->body, scoped);
                if (er != NULL) return er;
                inConstructorContext = false;
            }

            inClassContext = false;
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
    Error *e = NULL;

    if (symbolTable == NULL) {
        symbolTable = initSymbolTable();
    }
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


