//
// Created by valde on 2/24/19.
//

#include "type_checker.h"
#include "../ast/tree.h"
#include "../error/error.h"
#include "../symbol/symbol.h"

struct Type booleanStaticType = {.kind = typeBoolK};
struct Type intStaticType = {.kind = typeIntK};
//This is a hack, it is very hacky. Do not do this at home.

Error *typeCheckExpression(Expression *expression, Type *expectedType, SymbolTable *symbolTable);
Type *unwrapTypedef(Type *type, SymbolTable *symbolTable);
bool areTypesEqual(Type *first, Type *second, SymbolTable *symbolTable);
Type *unwrapVariable(Variable *variable, SymbolTable *symbolTable);
Type *evaluateExpressionType(Expression *expression, SymbolTable *symbolTable);
//Go all the way down through variable, then once at bottom get type
//when we get type start returning, apply subscripting and such as we go up

Type *getClassSubtype(char* varId, char* subtype, SymbolTable *symbolTable) {
    SYMBOL *symbol = getSymbol(symbolTable, varId);
    SYMBOL *inner = NULL;

    if (symbol == NULL) {
        return NULL;
    }

    if (symbol->value->kind == typeK) {
        if (symbol->value->val.typeD.tpe->kind == typeIdK) {
            inner = getSymbol(symbolTable, symbol->value->val.typeD.tpe->val.idType.id);

            if (inner == NULL) {
                return NULL;
            }

            if (inner->value->kind == symTypeClassK) {
                symbol = inner;
            }
        }
    }

    if (symbol->value->kind != symTypeClassK) {
        return NULL;
    }

    TypeList *extended = symbol->value->val.typeClassD.extendedClasses;

    while (extended != NULL) {

        if (strcmp(subtype, extended->type->val.typeClass.classId) == 0) {
            return extended->type;
        }

        //Check all subclasses of this subclass
        Type *pot = getClassSubtype(extended->type->val.typeClass.classId, subtype, symbolTable);
        if (pot != NULL) {
            return pot;
        }

        extended = extended->next;
    }

    return NULL;
}

Type *evaluateTermType(Term *term, SymbolTable *symbolTable) {
    SYMBOL *symbol;
    Type *type;

    switch (term->kind) {
        case variableK:
            return unwrapVariable(term->val.variableD.var, symbolTable);
            break;
        case functionCallK:
            symbol = getSymbol(symbolTable, term->val.functionCallD.functionId);

            if (symbol == NULL) {
                return NULL;
            }

            if (symbol->value->kind == typeFunctionK) {
                return symbol->value->val.typeFunctionD.returnType;
            }

            if (symbol->value->kind == typeK) {
                if (symbol->value->val.typeD.tpe->kind == typeLambdaK) {
                    return symbol->value->val.typeD.tpe->val.typeLambdaK.returnType;
                } else if (symbol->value->val.typeD.tpe->kind == typeIdK) {
                    Type *unwrapped = unwrapTypedef(symbol->value->val.typeD.tpe, symbolTable);

                    if (unwrapped->kind == typeLambdaK) {
                        return unwrapped->val.typeLambdaK.returnType;
                    }
                }
            }

            return NULL;
            break;
        case parenthesesK:
            return evaluateExpressionType(term->val.parenthesesD.expression, symbolTable);
            break;
        case negateK:
            return evaluateTermType(term->val.negateD.term, symbolTable);
            break;
        case absK:
            return evaluateExpressionType(term->val.absD.expression, symbolTable);
            break;
        case numK:
            return &intStaticType;
            break;
        case trueK:
            return &booleanStaticType;
            break;
        case falseK:
            return &booleanStaticType;
            break;
        case nullK:
            return NULL_KITTY_VALUE_INDICATOR;
            break;
        case lambdaK:
            type = NEW(Type);
            type->kind = typeLambdaK;
            type->val.typeLambdaK.returnType = term->val.lambdaD.lambda->returnType;
            type->val.typeLambdaK.typeList = NULL;

            //Now for converting the "advanced" type list
            VarDelList *vdl = term->val.lambdaD.lambda->declarationList;
            TypeList *typeList = NEW(TypeList);
            typeList->next = NULL;

            while (vdl != NULL) {
                typeList->type = vdl->type;

                if (type->val.typeLambdaK.typeList == NULL) {
                    type->val.typeLambdaK.typeList = typeList;
                }

                if (vdl->next == NULL) {
                    typeList->next = NULL;
                    vdl = vdl->next;
                } else {
                    typeList->next = NEW(TypeList);
                    typeList = typeList->next;
                    vdl = vdl->next;
                }
            }

            return type;
            break;
        case classDowncastk:
            type = getClassSubtype(term->val.classDowncastD.varId, term->val.classDowncastD.downcastId, symbolTable);

            return type;
            break;
    }

    return NULL;
}

Type *evaluateExpressionType(Expression *expression, SymbolTable *symbolTable) {
    bool isBoolean;

    switch (expression->kind) {
        case opK:
            isBoolean = true;

            switch (expression->val.op.operator->kind) {
                case opMultK:
                    isBoolean = false;
                    break;
                case opDivK:
                    isBoolean = false;
                    break;
                case opPlusK:
                    isBoolean = false;
                    break;
                case opMinusK:
                    isBoolean = false;
                    break;
                default:
                    break;
            }

            if (isBoolean == true) {
                return &booleanStaticType;
            } else {
                return &intStaticType;
            }

            break;
        case termK:
            return evaluateTermType(expression->val.termD.term, symbolTable);
            break;
    }

    return NULL;
}

bool areTypesEqual(Type *first, Type *second, SymbolTable *symbolTable) {
    if (first == NULL || second == NULL) {
        return false;
    }

    VarDelList *firstDelList;
    VarDelList *secondDelList;

    if (first->kind == second->kind) {
        if (first->kind == typeIntK || first->kind == typeBoolK) {
            return true;
        } else if (first->kind == typeArrayK) {
            return areTypesEqual(first->val.arrayType.type, second->val.arrayType.type, symbolTable);
        } else if (first->kind == typeRecordK) {
            firstDelList = first->val.recordType.types;
            secondDelList = second->val.recordType.types;
            VarDelList *newHead = NULL;
            VarDelList *newSecond = NULL;

            //Create new list from second
            while (secondDelList != NULL) {
                if (newHead == NULL) {
                    newSecond = NEW(VarDelList);
                    newHead = newSecond;
                } else {
                    newSecond->next = NEW(VarDelList);
                    newSecond = newSecond->next;
                }

                newSecond->type = secondDelList->type;
                newSecond->identifier = secondDelList->identifier;
                newSecond->next = NULL;

                secondDelList = secondDelList->next;
            }

            while (firstDelList != NULL) {
                bool found = false;

                newSecond = newHead;

                while (newSecond != NULL) {

                    if (areTypesEqual(firstDelList->type, newSecond->type, symbolTable) == true) {
                        found = true;

                        //Remove this new second node from the newHead list
                        //If the node is the first we displace with one
                        if (newSecond == newHead) {
                            newHead = newHead->next;
                        } else {
                            //Find the node and remove it
                            newSecond = newHead;

                            while (newSecond != NULL) {

                                if (newHead->next == newSecond) {
                                    newHead->next = newSecond->next;
                                    break;
                                }

                                newSecond = newSecond->next;
                            }

                        }

                        break;
                    }

                    newSecond = newSecond->next;
                }

                if (found == false) {
                    return false;
                }

                firstDelList = firstDelList->next;
            }

            if (firstDelList != NULL || newHead != NULL) {
                return false;
            } else {
                return true;
            }
        } else if (first->kind == typeLambdaK) {
            //Are return types equal?
            if (areTypesEqual(first->val.typeLambdaK.returnType, second->val.typeLambdaK.returnType, symbolTable) == false) {
                return false;
            }

            //Check if all arguments are equal also
            TypeList *firstTypeList = first->val.typeLambdaK.typeList;
            TypeList *secondTypeList = second->val.typeLambdaK.typeList;

            while (firstTypeList != NULL && secondTypeList != NULL) {

                if (areTypesEqual(firstTypeList->type, secondTypeList->type, symbolTable) == false) {
                    return false;
                }

                firstTypeList = firstTypeList->next;
                secondTypeList = secondTypeList->next;
            }

            if (firstTypeList != NULL || secondTypeList != NULL) {
                return false;
            }

            return true;

        } else if (first->kind == typeGenericK) {
            return strcmp(first->val.typeGeneric.genericName, second->val.typeGeneric.genericName) == 0;
        } else {
            //TypeIdK
            //return areTypesEqual(unwrapTypedef(first, symbolTable), unwrapTypedef(second, symbolTable), symbolTable);
            return strcmp(first->val.idType.id, second->val.idType.id) == 0;
            /*if (strcmp(first->val.idType.id, second->val.idType.id) == 0) {
                return true;
            } else {
                return false;
                return areTypesEqual(unwrapTypedef(first, symbolTable), unwrapTypedef(second, symbolTable), symbolTable);
            }*/
        }
    } else {
        return false;
    }
}

Type *unwrapTypedef(Type *type, SymbolTable *symbolTable) {
    SYMBOL *symbol;

    switch (type->kind) {
        case typeIdK:
                symbol = getSymbol(symbolTable, type->val.idType.id);

                if (symbol == NULL) {
                    return NULL;
                }

                if (symbol->value->kind == typeFunctionK) {
                    symbol->value->val.typeFunctionD;
                }

                return unwrapTypedef(symbol->value->val.typeD.tpe, symbolTable);
            break;
        default:
            return type;
            break;
    }
}

Type *idMatchesDecl(Declaration *declaration, char* id) {
    Declaration *looper;

    switch (declaration->kind) {
        case declVarK:
            if (strcmp(declaration->val.varD.id, id) == 0) {
                return declaration->val.varD.type;
            }
            break;
        case declVarsK:
            looper = declaration;

            while (looper != NULL) {
                Type *result = idMatchesDecl(looper->val.varsD.var, id);
                if (result != NULL) return result;
                looper = looper->val.varsD.next;
            }

            break;
        case declTypeK:
            if (strcmp(declaration->val.typeD.id, id) == 0) {
                return declaration->val.typeD.type;
            }
            break;
        case declFuncK:
            if (strcmp(declaration->val.functionD.function->head->indentifier, id) == 0) {
                return declaration->val.functionD.function->head->returnType;
            }
            break;
        case declValK:
            if (strcmp(declaration->val.valD.id, id) == 0) {
                return declaration->val.valD.tpe;
            }
            break;
        case declClassK:
            if (strcmp(declaration->val.classD.id, id) == 0) {
                //This a small hack because of how classes are both custom types and hold declarations
                Type *hack = NEW(Type);

                hack->kind = typeClassK;
                hack->val.typeClass.classId = id;

                return hack;
            }
            break;
    }

    return NULL;
}

Type *findTypeThroughGenericWithSubtype(SymbolTable *symbolTable, char *subType, char *lookupId) {
    //Get the class
    SYMBOL *symbol = getSymbol(symbolTable, subType);

    if (symbol == NULL) {
        return NULL;
    }

    if (symbol->value->kind != symTypeClassK) {
        return NULL;
    }

    //Go through the decls and look for the right one
    DeclarationList *declarationList = symbol->value->val.typeClassD.declarationList;

    while (declarationList != NULL) {
        Type *ret = idMatchesDecl(declarationList->declaration, lookupId);

        if (ret != NULL) {
            return ret;
        }

        declarationList = declarationList->next;
    }

    TypeList *typeList = symbol->value->val.typeClassD.generics;

    while (typeList != NULL) {



        typeList = typeList->next;
    }
}

typedef struct BoundAndGenericPair {
    Type *generic;
    Type *bound;
    struct BoundAndGenericPair *next;
} BoundAndGenericPair;

Type *bindGenericTypes(BoundAndGenericPair *bagp, Type *typeToBindOn, SymbolTable *symbolTable) {
    switch (typeToBindOn->kind) {
        case typeIdK:
            return bindGenericTypes(bagp, unwrapTypedef(typeToBindOn, symbolTable), symbolTable);
            break;
        case typeIntK:
            return typeToBindOn;
            break;
        case typeBoolK:
            return typeToBindOn;
            break;
        case typeArrayK:
            //Create new array type with bounded type
            {
                Type *inner = bindGenericTypes(bagp, typeToBindOn->val.arrayType.type, symbolTable);

                Type *bound = NEW(Type);

                bound->kind = typeArrayK;
                bound->val.arrayType.type = inner;

                return bound;
            }
            break;
        case typeRecordK:
            //Check if contains generic
            {
                Type *boundRecord = NEW(Type);
                boundRecord->kind = typeRecordK;

                VarDelList *varDelList = typeToBindOn->val.recordType.types;
                VarDelList *boundList = NULL;

                while (varDelList != NULL) {
                    if (boundList == NULL) {
                        boundList = NEW(VarDelList);
                        boundRecord->val.recordType.types = boundList;
                    } else {
                        boundList->next = NEW(VarDelList);
                        boundList = boundList->next;
                    }

                    boundList->type = bindGenericTypes(bagp, varDelList->type, symbolTable);
                    boundList->next = NULL;

                    varDelList = varDelList->next;
                }

                return boundRecord;
            }
            break;
        case typeLambdaK:
            //Check if contains generic
            {
                Type *boundLambda = NEW(Type);
                boundLambda->kind = typeLambdaK;

                TypeList *params = typeToBindOn->val.typeLambdaK.typeList;
                TypeList *boundParams = NULL;

                while (params != NULL) {
                    if (boundParams == NULL) {
                        boundParams = NEW(TypeList);
                        boundLambda->val.typeLambdaK.typeList = boundParams;
                    } else {
                        boundParams->next = NEW(TypeList);
                        boundParams = boundParams->next;
                    }

                    boundParams->next = NULL;
                    boundParams->type = bindGenericTypes(bagp, params->type, symbolTable);


                    params = params->next;
                }

                boundLambda->val.typeLambdaK.returnType = bindGenericTypes(bagp, typeToBindOn->val.typeLambdaK.returnType, symbolTable);

                return boundLambda;
            }
            break;
        case typeClassK:
            return typeToBindOn;
            break;
        case typeGenericK:
            {
                //Find the bound type by generic type index

                //We are looking for the (counter - it) element
                int it = typeToBindOn->val.typeGeneric.typeIndex;
                int counter = 0;

                while (it > 0) {
                    bagp = bagp->next;
                    it--;
                }
                return bagp->bound;
            }
            break;
    }

    return NULL;
}

Type *unwrapVariable(Variable *variable, SymbolTable *symbolTable) {
    SYMBOL *symbol;
    Type *varType;
    VarDelList *varDelList;
    Type *innerType;
    Error *e = NULL;

    switch (variable->kind) {
        case varIdK:
            symbol = getSymbol(symbolTable, variable->val.idD.id);

            if (symbol == NULL) {
                return NULL;
            }

            return unwrapTypedef(symbol->value->val.typeD.tpe, symbolTable);
            break;
        case arrayIndexK:
            innerType = unwrapVariable(variable->val.arrayIndexD.var, symbolTable);

            varType = unwrapTypedef(innerType, symbolTable);

            //Check if expression is int
            e = typeCheckExpression(variable->val.arrayIndexD.idx, &intStaticType, symbolTable);
            if (e != NULL) return NULL;

            //Since we are in an array subscript, we want to subscript by removing a type layer
            if (varType->kind == typeArrayK) {
                return varType->val.arrayType.type;
            } else {
                return NULL;
            }

            break;
        case recordLookupK:
            varType = unwrapVariable(variable->val.recordLookupD.var, symbolTable);

            if (varType == NULL) {
                return NULL;
            }

            innerType = unwrapTypedef(varType, symbolTable);

            if (innerType == NULL) {
                return NULL;
            }

            //If the var type is a generic, and we are accessing it, we are trying to access the subtype
            if (innerType->kind == typeGenericK) {
                //Get the class
                symbol = getSymbol(symbolTable, innerType->val.typeGeneric.subType);

                if (symbol == NULL) {
                    return NULL;
                }

                if (symbol->value->kind != symTypeClassK) {
                    return NULL;
                }

                //Go through the decls and look for the right one
                DeclarationList *declarationList = symbol->value->val.typeClassD.declarationList;

                while (declarationList != NULL) {
                    Type *ret = idMatchesDecl(declarationList->declaration, variable->val.recordLookupD.id);

                    if (ret != NULL) {
                        return ret;
                    }

                    declarationList = declarationList->next;
                }

            } else if (innerType->kind == typeClassK) {
                //We hack the record type to work with classes
                //Look the class up in the symbol table
                symbol = getSymbol(symbolTable, innerType->val.typeClass.classId);

                DeclarationList *classBody = symbol->value->val.typeClassD.declarationList;

                //Find same bound index as the one of the ret
                TypeList *boundTypes = innerType->val.typeClass.genericBoundValues;
                TypeList *generics = symbol->value->val.typeClassD.generics;
                BoundAndGenericPair *head = NULL;
                BoundAndGenericPair *current = NULL;

                //Length has been checked earlier
                while (generics != NULL && boundTypes != NULL) {
                    if (head == NULL) {
                        head = NEW(BoundAndGenericPair);
                        current = head;
                    } else {
                        current->next = NEW(BoundAndGenericPair);
                        current = current->next;
                    }

                    current->bound = boundTypes->type;
                    current->generic = generics->type;
                    current->next = NULL;

                    boundTypes = boundTypes->next;
                    generics = generics->next;
                }



                while (classBody != NULL) {

                    Type* ret = idMatchesDecl(classBody->declaration, variable->val.recordLookupD.id);

                    if (ret != NULL) {
                        return bindGenericTypes(head, ret, symbolTable);
                    }

                    classBody = classBody->next;
                }

            } else if (innerType->kind == typeRecordK) {
                //Check if id is in the type's vardellist
                varDelList = innerType->val.recordType.types;

                while (varDelList != NULL) {

                    if (strcmp(variable->val.recordLookupD.id, varDelList->identifier) == 0) {
                        //ID
                        return varDelList->type;
                    }

                    varDelList = varDelList->next;
                }
            } else if (innerType->kind == typeGenericK && innerType->val.typeGeneric.subType != NULL) {
                //If the generic subtypes another type, we can look the other type up

                //Get the subtype class
                symbol = getSymbol(symbolTable, innerType->val.typeGeneric.subType);

                DeclarationList *classBody = symbol->value->val.typeClassD.declarationList;

                while (classBody != NULL) {

                    Type* ret = idMatchesDecl(classBody->declaration, variable->val.recordLookupD.id);

                    if (ret != NULL) {
                        //ID
                        return ret;
                    }

                    classBody = classBody->next;
                }
            }


            return NULL;

            break;
    }

    return NULL;
}

/*
Type *unwrapNestedType(Type *type, Variable *variable, SymbolTable *symbolTable) {
    SYMBOL *symbol;
    Variable *base;
    VarDelList *varDelList;

    switch (type->kind) {
        case typeIdK:
            symbol = getSymbol(symbolTable, type->val.idType.id);
            return unwrapNestedType(symbol->value->val.typeD.tpe, variable, symbolTable);
            break;
        case typeIntK:
            return type;
            break;
        case typeBoolK:
            return type;
            break;
        case typeArrayK:

            if (variable->kind == arrayIndexK) {
                return unwrapNestedType(type->val.arrayType.type, variable->val.arrayIndexD.var, symbolTable);
            } else {
                return type;
            }
            break;
        case typeRecordK:

            if (variable->kind == recordLookupK) {
                varDelList = type->val.recordType.types;

                while (varDelList != NULL) {

                    if (strcmp(variable->val.recordLookupD.id, varDelList->identifier) == 0) {
                        //ID
                        return unwrapNestedType(varDelList->type, variable->val.recordLookupD.var, symbolTable);
                    }

                    varDelList = varDelList->next;
                }


                return NULL;
            } else {
                return type;
            }
            break;
    }
    return NULL;
}
*/

Error *typeCheckVariable(Variable* variable, Type *expectedType, SymbolTable *symbolTable) {
    Error *e = NULL;
    SYMBOL *symbol = NULL;
    VarDelList *recordItems = NULL;
    Type *symAsType = NULL;
    Type *unwrapped;
    Type *toCompare;

    switch (variable->kind) {
        case varIdK:

            symbol = getSymbol(symbolTable, variable->val.idD.id);

            if (symbol == NULL) {
                e = NEW(Error);

                e->error = SYMBOL_NOT_FOUND;
                e->val.SYMBOL_NOT_FOUND_S.id = variable->val.idD.id;
                e->val.SYMBOL_NOT_FOUND_S.lineno = variable->lineno;

                return e;
            }

            if (symbol->value->kind == typeK) {
                symAsType = unwrapTypedef(symbol->value->val.typeD.tpe, symbolTable);
                Type *unwrapped = unwrapTypedef(expectedType, symbolTable);

                if (areTypesEqual(symAsType, unwrapped, symbolTable) == false) {
                    e = NEW(Error);

                    e->error = VARIABLE_UNEXPECTED_TYPE;
                    e->val.VARIABLE_UNEXPECTED_TYPE_S.id = variable->val.idD.id;
                    e->val.VARIABLE_UNEXPECTED_TYPE_S.lineno = variable->lineno;
                    e->val.VARIABLE_UNEXPECTED_TYPE_S.expectedType = expectedType->kind;
                    e->val.VARIABLE_UNEXPECTED_TYPE_S.foundType = symAsType->kind;


                    return e;
                }
            } else {
                if (expectedType->kind == typeLambdaK) {
                    Type *returnType = unwrapTypedef(expectedType->val.typeLambdaK.returnType, symbolTable);

                    TypeList *typeList = expectedType->val.typeLambdaK.typeList;
                    VarDelList *varDelList = symbol->value->val.typeFunctionD.tpe;

                    //Compare the argument types sequentially
                    while (typeList != NULL && varDelList != NULL) {

                        if (areTypesEqual(typeList->type, varDelList->type, symbolTable) == false) {
                            e = NEW(Error);

                            e->error = VARIABLE_UNEXPECTED_TYPE;
                            e->val.VARIABLE_UNEXPECTED_TYPE_S.id = "FIX ME 1";
                            e->val.VARIABLE_UNEXPECTED_TYPE_S.lineno = variable->lineno;
                            e->val.VARIABLE_UNEXPECTED_TYPE_S.expectedType = expectedType->kind;
                            e->val.VARIABLE_UNEXPECTED_TYPE_S.foundType = returnType->kind;


                            return e;
                        }

                        typeList = typeList->next;
                        varDelList = varDelList->next;
                    }

                    if (typeList != NULL || varDelList != NULL) {
                        e = NEW(Error);

                        e->error = TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH;
                        e->val.TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH_S.lineno = variable->lineno;
                        e->val.TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH_S.fid = "FIX ME 2";
                        e->val.TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH_S.foundCount = 0;
                        e->val.TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH_S.expectedCount = 0;

                        return e;
                    }
                }

            }

            /*if (symAsType->kind == typeArrayK) {
                symAsType = symAsType->val.arrayType.type;
            }*/



            break;
        case arrayIndexK:
            e = typeCheckExpression(variable->val.arrayIndexD.idx, &intStaticType, symbolTable);
            if (e != NULL) return e;

            toCompare = unwrapTypedef(unwrapVariable(variable->val.arrayIndexD.var, symbolTable), symbolTable);

            if (unwrapTypedef(toCompare, symbolTable)->kind != typeArrayK) {
                e = NEW(Error);

                e->error = VARIABLE_UNEXPECTED_TYPE;
                e->val.VARIABLE_UNEXPECTED_TYPE_S.id = variable->val.recordLookupD.id;
                e->val.VARIABLE_UNEXPECTED_TYPE_S.lineno = variable->lineno;
                e->val.VARIABLE_UNEXPECTED_TYPE_S.expectedType = typeArrayK;
                e->val.VARIABLE_UNEXPECTED_TYPE_S.foundType = toCompare->kind;

                return e;
            }

            if (areTypesEqual(unwrapTypedef(toCompare->val.arrayType.type, symbolTable),
                    unwrapTypedef(expectedType, symbolTable),
                    symbolTable) == false) {
                e = NEW(Error);

                e->error = VARIABLE_UNEXPECTED_TYPE;
                e->val.VARIABLE_UNEXPECTED_TYPE_S.id = variable->val.recordLookupD.id;
                e->val.VARIABLE_UNEXPECTED_TYPE_S.lineno = variable->lineno;
                e->val.VARIABLE_UNEXPECTED_TYPE_S.expectedType = expectedType->kind;
                e->val.VARIABLE_UNEXPECTED_TYPE_S.foundType = toCompare->val.arrayType.type->kind;

                return e;
            }

            break;
        case recordLookupK:
            //This needs work

            //We call further down with the variables now bound


            //The id subscript decides the return type
            /*
            e = typeCheckVariable(variable->val.recordLookupD.var, expectedType, symbolTable);
            if (e != NULL) return e;*/

            unwrapped = unwrapVariable(variable, symbolTable);

            //If unwrap fails, the record id does not exist
            if (unwrapped == NULL) {
                e = NEW(Error);

                e->error = SYMBOL_NOT_FOUND;
                e->val.SYMBOL_NOT_FOUND_S.id = variable->val.recordLookupD.id;
                e->val.SYMBOL_NOT_FOUND_S.lineno = variable->lineno;

                return e;
            }

            symAsType = unwrapTypedef(unwrapped, symbolTable);

            if (areTypesEqual(unwrapTypedef(expectedType, symbolTable), symAsType, symbolTable) == false) {
                e = NEW(Error);

                e->error = SYMBOL_NOT_FOUND;
                e->val.SYMBOL_NOT_FOUND_S.id = variable->val.recordLookupD.id;
                e->val.SYMBOL_NOT_FOUND_S.lineno = variable->lineno;

                return e;
            } else {
                return NULL;
            }

            symbol = getSymbol(symbolTable, variable->val.recordLookupD.id);

            if (symbol == NULL) {
                e = NEW(Error);

                e->error = SYMBOL_NOT_FOUND;
                e->val.SYMBOL_NOT_FOUND_S.id = variable->val.idD.id;
                e->val.SYMBOL_NOT_FOUND_S.lineno = variable->lineno;

                return e;
            }

            if (symbol->value->kind != typeK) {
                e = NEW(Error);

                e->error = SYMBOL_NOT_FOUND;
                e->val.SYMBOL_NOT_FOUND_S.id = variable->val.idD.id;
                e->val.SYMBOL_NOT_FOUND_S.lineno = variable->lineno;

                return e;
            }

            recordItems = symbol->value->val.typeD.tpe->val.recordType.types;

            while (recordItems != NULL) {

                //If we are at the desired id
                if (strcmp(recordItems->identifier, variable->val.recordLookupD.id) == 0) {
                    break;
                }

                recordItems = recordItems->next;
            }

            if (recordItems == NULL) {
                e = NEW(Error);

                e->error = VARIABLE_COULD_NOT_FIND_RECORD_ITEM;
                e->val.VARIABLE_COULD_NOT_FIND_RECORD_ITEM_S.lineno = variable->lineno;

                return e;
            }


            if (areTypesEqual(recordItems->type, expectedType, symbolTable) == false) {
                e = NEW(Error);

                e->error = VARIABLE_UNEXPECTED_TYPE;
                e->val.VARIABLE_UNEXPECTED_TYPE_S.id = variable->val.recordLookupD.id;
                e->val.VARIABLE_UNEXPECTED_TYPE_S.lineno = variable->lineno;
                e->val.VARIABLE_UNEXPECTED_TYPE_S.expectedType = expectedType->kind;
                e->val.VARIABLE_UNEXPECTED_TYPE_S.foundType = symbol->value->val.typeD.tpe->kind;

                return e;
            }

            break;
    }

    return NULL;
}

Error *typeCheckTerm(Term *term, Type *expectedType, SymbolTable *symbolTable) {
    Type *typeMatch = NULL;
    Error *e = NULL;
    SYMBOL *symbol = NULL;
    Type* type;

    switch (term->kind) {
        case variableK:

            e = typeCheckVariable(term->val.variableD.var, expectedType, symbolTable);
            if (e != NULL) return e;
            break;
        case functionCallK:
            //Here we both want to check if the function we are checking is returning the correct type
            //Additionally we also want to check if the parameters are correct also

            //This can also be a lambda
            symbol = getSymbol(symbolTable, term->val.functionCallD.functionId);

            if (symbol == NULL) {
                e = NEW(Error);

                e->error = SYMBOL_NOT_FOUND;
                e->val.SYMBOL_NOT_FOUND_S.id = term->val.functionCallD.functionId;
                e->val.SYMBOL_NOT_FOUND_S.lineno = term->lineno;

                return e;
            }

            if (symbol->value->kind != typeFunctionK) {
                //Check for lambda
                Type *ret = unwrapTypedef(symbol->value->val.typeD.tpe, symbolTable);

                if (ret == NULL) {
                    e = NEW(Error);

                    e->error = SYMBOL_NOT_FOUND;
                    e->val.SYMBOL_NOT_FOUND_S.id = "userType";
                    e->val.SYMBOL_NOT_FOUND_S.lineno = term->lineno;

                    return e;
                }

                if (ret->kind != typeLambdaK) {
                    e = NEW(Error);

                    e->error = TYPE_TERM_IS_NOT_FUNCTION;
                    e->val.TYPE_TERM_IS_NOT_FUNCTION_S.lineno = term->lineno;
                    e->val.TYPE_TERM_IS_NOT_FUNCTION_S.fid = term->val.functionCallD.functionId;

                    return e;
                }
            }

            if ((symbol->value->kind == typeFunctionK &&
               areTypesEqual(unwrapTypedef(symbol->value->val.typeFunctionD.returnType, symbolTable),
                       unwrapTypedef(expectedType, symbolTable), symbolTable) == false) ||
               (symbol->value->kind == typeK &&
               symbol->value->val.typeD.tpe->kind == typeLambdaK &&
               areTypesEqual(unwrapTypedef(symbol->value->val.typeD.tpe->val.typeLambdaK.returnType, symbolTable),
                       unwrapTypedef(expectedType, symbolTable), symbolTable) == false)) {
                e = NEW(Error);

                e->error = TYPE_TERM_INVALID_FUNCTION_CALL_RETURN_TYPE;
                e->val.TYPE_TERM_INVALID_FUNCTION_CALL_RETURN_TYPE_S.lineno = term->lineno;
                e->val.TYPE_TERM_INVALID_FUNCTION_CALL_RETURN_TYPE_S.fid = term->val.functionCallD.functionId;

                return e;
            }

            ExpressionList *expressionList = term->val.functionCallD.expressionList;

            if (symbol->value->kind == typeFunctionK) {
                VarDelList *varDelList = symbol->value->val.typeFunctionD.tpe;

                int paramNum = 0;

                while (expressionList != NULL && varDelList != NULL) {
                    e = typeCheckExpression(expressionList->expression, varDelList->type, symbolTable);
                    if (e != NULL) return e;

                    expressionList = expressionList->next;
                    varDelList = varDelList->next;
                    paramNum++;
                }

                if ((varDelList == NULL && expressionList != NULL) || (varDelList != NULL && expressionList == NULL)) {
                    //Error
                    e = NEW(Error);

                    int expectedCount = paramNum;

                    if (varDelList == NULL) {
                        while (expressionList != NULL) {
                            paramNum++;
                            expressionList = expressionList->next;
                        }
                    } else {
                        while (varDelList != NULL) {
                            expectedCount++;
                            varDelList = varDelList->next;
                        }
                    }

                    e->error = TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH;
                    e->val.TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH_S.lineno = term->lineno;
                    e->val.TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH_S.fid = term->val.functionCallD.functionId;
                    e->val.TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH_S.foundCount = paramNum;
                    e->val.TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH_S.expectedCount = expectedCount;

                    return e;
                }
            } else {
                TypeList *varDelList;
                if (symbol->value->val.typeD.tpe->kind == typeIdK) {
                    varDelList = unwrapTypedef(symbol->value->val.typeD.tpe, symbolTable)->val.typeLambdaK.typeList;
                    if (varDelList == NULL) {
                        e = NEW(Error);

                        e->error = SYMBOL_NOT_FOUND;
                        e->val.SYMBOL_NOT_FOUND_S.id = "userType";
                        e->val.SYMBOL_NOT_FOUND_S.lineno = term->lineno;

                        return e;
                    }
                } else {
                    varDelList = symbol->value->val.typeD.tpe->val.typeLambdaK.typeList;
                }

                int paramNum = 0;

                while (expressionList != NULL && varDelList != NULL) {
                    e = typeCheckExpression(expressionList->expression, varDelList->type, symbolTable);
                    if (e != NULL) return e;

                    expressionList = expressionList->next;
                    varDelList = varDelList->next;
                    paramNum++;
                }

                if ((varDelList == NULL && expressionList != NULL) || (varDelList != NULL && expressionList == NULL)) {
                    //Error
                    e = NEW(Error);

                    int expectedCount = paramNum;

                    if (varDelList == NULL) {
                        while (expressionList != NULL) {
                            paramNum++;
                            expressionList = expressionList->next;
                        }
                    } else {
                        while (varDelList != NULL) {
                            expectedCount++;
                            varDelList = varDelList->next;
                        }
                    }

                    e->error = TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH;
                    e->val.TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH_S.lineno = term->lineno;
                    e->val.TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH_S.fid = term->val.functionCallD.functionId;
                    e->val.TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH_S.foundCount = paramNum;
                    e->val.TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH_S.expectedCount = expectedCount;

                    return e;
                }

            }

            break;
        case lambdaK:
            typeMatch = NEW(Type);

            typeMatch->kind = typeLambdaK;
            typeMatch->val.typeLambdaK.returnType = term->val.lambdaD.lambda->returnType;
            typeMatch->val.typeLambdaK.typeList = NULL;

            //Now for converting the "advanced" type list
            VarDelList *vdl = term->val.lambdaD.lambda->declarationList;
            TypeList *typeList = NEW(TypeList);
            typeList->next = NULL;

            while (vdl != NULL) {
                typeList->type = vdl->type;

                if (typeMatch->val.typeLambdaK.typeList == NULL) {
                    typeMatch->val.typeLambdaK.typeList = typeList;
                }

                if (vdl->next == NULL) {
                    typeList->next = NULL;
                    vdl = vdl->next;
                } else {
                    typeList->next = NEW(TypeList);
                    typeList = typeList->next;
                    vdl = vdl->next;
                }
            }

            //Check if lhs and rhs are same type
            if (areTypesEqual(expectedType, typeMatch, symbolTable) == false) {
                //Better error please
                e = NEW(Error);

                e->error = VARIABLE_UNEXPECTED_TYPE;
                e->val.VARIABLE_UNEXPECTED_TYPE_S.id = "lambda";
                e->val.VARIABLE_UNEXPECTED_TYPE_S.lineno =  term->lineno;
                e->val.VARIABLE_UNEXPECTED_TYPE_S.expectedType = expectedType->kind;
                e->val.VARIABLE_UNEXPECTED_TYPE_S.foundType = term->val.lambdaD.lambda->returnType->kind;


                return e;
            }

            e = typeCheck(term->val.lambdaD.lambda->body, term->val.lambdaD.lambda->returnType);
            if (e != NULL) return e;

            break;
        case parenthesesK:
            e = typeCheckExpression(term->val.parenthesesD.expression, expectedType, symbolTable);
            if (e != NULL) return e;
            break;
        case negateK:
            if (areTypesEqual(&booleanStaticType, expectedType, symbolTable) == false) {
                e = NEW(Error);

                e->error = TYPE_TERM_NOT_BOOLEAN;
                e->val.TYPE_TERM_NOT_BOOLEAN_S.lineno = term->lineno;
                e->val.TYPE_TERM_NOT_BOOLEAN_S.termThatCausedError = term;

                return e;
            }

            e = typeCheckTerm(term->val.negateD.term, expectedType, symbolTable);
            if (e != NULL) return e;

            break;
        case absK:
            if (areTypesEqual(&intStaticType, expectedType, symbolTable) == false) {
                e = NEW(Error);

                e->error = TYPE_TERM_NOT_INTEGER;
                e->val.TYPE_TERM_NOT_INTEGER_S.lineno = term->lineno;
                e->val.TYPE_TERM_NOT_INTEGER_S.termThatCausedError = term;

                return e;
            }

            if (unwrapTypedef(evaluateExpressionType(term->val.absD.expression, symbolTable), symbolTable)->kind == typeArrayK) {
                return NULL;
            }

            e = typeCheckExpression(term->val.absD.expression, expectedType, symbolTable);
            if (e != NULL) return e;

            break;
        case numK:
            if (areTypesEqual(&intStaticType, expectedType, symbolTable) == false) {
                e = NEW(Error);

                e->error = TYPE_TERM_NOT_INTEGER;
                e->val.TYPE_TERM_NOT_INTEGER_S.lineno = term->lineno;
                e->val.TYPE_TERM_NOT_INTEGER_S.termThatCausedError = term;

                return e;
            }
            break;
        case trueK:
            if (areTypesEqual(&booleanStaticType, expectedType, symbolTable) == false) {
                e = NEW(Error);

                e->error = TYPE_TERM_NOT_BOOLEAN;
                e->val.TYPE_TERM_NOT_BOOLEAN_S.lineno = term->lineno;
                e->val.TYPE_TERM_NOT_BOOLEAN_S.termThatCausedError = term;

                return e;
            }
            break;
        case falseK:
            if (areTypesEqual(&booleanStaticType, expectedType, symbolTable) == false) {
                e = NEW(Error);

                e->error = TYPE_TERM_NOT_BOOLEAN;
                e->val.TYPE_TERM_NOT_BOOLEAN_S.lineno = term->lineno;
                e->val.TYPE_TERM_NOT_BOOLEAN_S.termThatCausedError = term;

                return e;
            }
            break;
        case nullK:
            //Null satisfies all types
            return NULL_KITTY_VALUE_INDICATOR;
            break;
        case classDowncastk:
            type = getClassSubtype(term->val.classDowncastD.varId, term->val.classDowncastD.downcastId, symbolTable);

            if (areTypesEqual(type, expectedType, symbolTable) == false) {
                e = NEW(Error);

                e->error = SYMBOL_NOT_FOUND;
                e->val.SYMBOL_NOT_FOUND_S.id = "WRONG ERROR; FIX ME 1173";
                e->val.SYMBOL_NOT_FOUND_S.lineno = 42;

                return e;
            }

            break;
        default:
            break;
    }

    return NULL;
}

Error *typeCheckExpression(Expression *expression, Type *expectedType, SymbolTable *symbolTable) {

    Error *e = NULL;
    SYMBOL *symbol;
    bool isBoolean;
    Type *expressionType;
    Type *returnType;

    switch (expression->kind) {
        /*case idK:
            symbol = getSymbol(expression->symbolTable, expression->val.idE);

            if (expression->val->)

            break;
        case intconstK:

            if (expectedType)

            break;*/
        case opK:
            //Check if the operator takes boolean or integer as rhs/lhs
            expressionType = &booleanStaticType;

            switch (expression->val.op.operator->kind) {
                /* GIT BLAME: MADS */
                case opGreaterK:
                case opLessK:
                case opGeqK:
                case opLeqK:
                    expressionType = &intStaticType;
                    break;
                default:
                    break;
            }

            //We want to check if the operation returns the expected type
            returnType = &booleanStaticType;

            switch (expression->val.op.operator->kind) {
                case opMultK:
                case opDivK:
                case opPlusK:
                case opMinusK:
                    returnType = &intStaticType;
            }


            if (areTypesEqual(expectedType, returnType, symbolTable) == false) {
                e = NEW(Error);

                e->error = TYPE_EXPRESSION_IS_NOT_AS_EXPECTED;
                e->val.TYPE_EXPRESSION_IS_NOT_AS_EXPECTED_S.lineno = expression->lineno;
                e->val.TYPE_EXPRESSION_IS_NOT_AS_EXPECTED_S.expThatCausedError = expression;
                e->val.TYPE_EXPRESSION_IS_NOT_AS_EXPECTED_S.expectedType = expectedType->kind;
                e->val.TYPE_EXPRESSION_IS_NOT_AS_EXPECTED_S.expressionType = expressionType->kind;

                return e;
            }

            Error *e1 = NULL;
            Error *e2 = NULL;

            //We also want to check if try to perform an integer operation on a null, this we cannot do
            if (expressionType->kind == intStaticType.kind) {
                if (evaluateExpressionType(expression->val.op.left, symbolTable) == NULL_KITTY_VALUE_INDICATOR ||
                        evaluateExpressionType(expression->val.op.right, symbolTable) == NULL_KITTY_VALUE_INDICATOR){
                    e = NEW(Error);

                    e->error = SYMBOL_NOT_FOUND;
                    e->val.SYMBOL_NOT_FOUND_S.id = "WRONG ERROR TYPE 1178";
                    e->val.SYMBOL_NOT_FOUND_S.lineno = 42;

                    return e;
                }
            }

            //If we have and or or we have to check that both sides are boolean
            if (expression->val.op.operator->kind == opAndK ||
            expression->val.op.operator->kind == opOrK) {
                //Check if lhs is boolean expression
                e = typeCheckExpression(expression->val.op.left, &booleanStaticType, symbolTable);
                e2 = typeCheckExpression(expression->val.op.right, &booleanStaticType, symbolTable);

                if (e == NULL_KITTY_VALUE_INDICATOR || e2 == NULL_KITTY_VALUE_INDICATOR) {
                    e = NEW(Error);

                    e->error = SYMBOL_NOT_FOUND;
                    e->val.SYMBOL_NOT_FOUND_S.id = "WRONG ERROR TYPE 1200";
                    e->val.SYMBOL_NOT_FOUND_S.lineno = expression->lineno;

                    return e;
                }

                if (e != NULL) return e;
                if (e2 != NULL) return e2;
            }

            //The idea here is that either one of the sides are null or they are both integer or both boolean.
            //Equality and inequality works on both booleans and integers
            if (expression->val.op.operator->kind == opEqualityK ||
                    expression->val.op.operator->kind == opInequalityK) {
                //Get lhs type
                Type* lhsType = evaluateExpressionType(expression->val.op.left, symbolTable);

                if (lhsType == NULL_KITTY_VALUE_INDICATOR) {
                    if (expression->val.op.right->kind == termK &&
                        expression->val.op.right->val.termD.term->kind != variableK &&
                        expression->val.op.right->val.termD.term->kind != functionCallK) {
                        //Cannot compare with null
                        e = NEW(Error);

                        e->error = SYMBOL_NOT_FOUND;
                        e->val.SYMBOL_NOT_FOUND_S.id = "WRONG ERROR TYPE 1243";
                        e->val.SYMBOL_NOT_FOUND_S.lineno = 42;

                        return e;
                    } else {
                        return NULL;
                    }
                }

                e2 = typeCheckExpression(expression->val.op.right, lhsType, symbolTable);

                if (e2 == NULL_KITTY_VALUE_INDICATOR) {
                    if (expression->val.op.left->kind == termK &&
                        expression->val.op.left->val.termD.term->kind != variableK &&
                        expression->val.op.left->val.termD.term->kind != functionCallK) {
                        //Cannot compare with null
                        e = NEW(Error);

                        e->error = SYMBOL_NOT_FOUND;
                        e->val.SYMBOL_NOT_FOUND_S.id = "WRONG ERROR TYPE 1259";
                        e->val.SYMBOL_NOT_FOUND_S.lineno = 42;

                        return e;
                    } else {
                        return NULL;
                    }
                }

                if (e2 != NULL) return e2;

                return NULL;
            }

            break;
        case termK:
            e = typeCheckTerm(expression->val.termD.term,
                    expectedType,
                    symbolTable);
            if (e != NULL) return e;

            break;
        default:
            break;
    }


    return NULL;
}

bool isTypedef(char* id, SymbolTable *symbolTable) {

    SYMBOL *symbol = getSymbol(symbolTable, id);

    if (symbol->value->kind == typeK) {
        return symbol->value->val.typeD.isTypedef;
    }

    return false;
}

//We need to track the return type, since we can have deeeeeeeep dwelling return statements
Error *typeCheckStatement(Statement *statement, Type *functionReturnType) {
    Error *e = NULL;
    Error *e2 = NULL;
    StatementList *statementList;
    SYMBOL *symbol;
    VarDelList *varDelList;
    Variable *topVariable;
    char *innerId;
    Type *lhsType;

    SymbolTable *parentScope = statement->symbolTable;

    switch (statement->kind) {
        case statReturnK:
            if (statement->symbolTable->next != NULL) {
                parentScope = statement->symbolTable->next;
            }

            e = typeCheckExpression(statement->val.returnD.exp,
                    functionReturnType,
                    statement->symbolTable);
            if (e != NULL && e != NULL_KITTY_VALUE_INDICATOR) return e;

            //dumpSymbolTable(statement->symbolTable);
            break;
        case statWriteK:
            e = typeCheckExpression(statement->val.writeD.exp,
                                    &booleanStaticType,
                                    statement->symbolTable);

            e2 = typeCheckExpression(statement->val.writeD.exp,
                                    &intStaticType,
                                    statement->symbolTable);

            if (e != NULL && e2 != NULL) {
                if (e != NULL) return e;
                if (e2 != NULL) return e2;
            }

            return NULL;
            break;
        /*case statAllocateK:
            e = typeCheckExpression(statement->val.allocateD.exp,);
            if (e != NULL) return e;
            break;*/
        case statAllocateLenK:
            e = typeCheckExpression(statement->val.allocateLenD.len,
                                    &intStaticType,
                                    statement->symbolTable);
            if (e != NULL) return e;
            break;
        case statIfK:
            e = typeCheckExpression(statement->val.ifD.exp,
                                    &booleanStaticType,
                                    statement->symbolTable);
            if (e != NULL) return e;

            e = typeCheckStatement(statement->val.ifD.statement,
                                   functionReturnType);
            if (e != NULL) return e;
            break;
        case statIfElK:
            e = typeCheckExpression(statement->val.ifElD.exp,
                                    &booleanStaticType,
                                    statement->symbolTable);
            if (e != NULL) return e;

            e = typeCheckStatement(statement->val.ifElD.statement,
                                   functionReturnType);
            if (e != NULL) return e;

            e = typeCheckStatement(statement->val.ifElD.elseStatement,
                                   functionReturnType);
            if (e != NULL) return e;
            break;
        case statWhileK:
            e = typeCheckExpression(statement->val.whileD.exp,
                                    &booleanStaticType,
                                    statement->symbolTable);
            if (e != NULL) return e;

            e = typeCheckStatement(statement->val.whileD.statement,
                                   functionReturnType);
            if (e != NULL) return e;
            break;
        case stmListK:
            statementList = statement->val.stmListD.statementList;

            while (statementList != NULL) {

                e = typeCheckStatement(statementList->statement,
                                       functionReturnType);
                if (e != NULL) return e;

                statementList = statementList->next;
            }

            break;
        case assignmentK:
            //Assignment is a bit funny, we have to check if the LHS and RHS are the same
            //We have to find out what the LHS is first

            if (statement->val.assignmentD.var->kind == varIdK &&
            isTypedef(statement->val.assignmentD.var->val.idD.id, statement->symbolTable) == true) {
                e = NEW(Error);

                e->error = SYMBOL_NOT_FOUND;
                e->val.SYMBOL_NOT_FOUND_S.id = "WRONG ERROR TYPE 1374";
                e->val.SYMBOL_NOT_FOUND_S.lineno = statement->lineno;

                return e;
            }

            lhsType = unwrapVariable(statement->val.assignmentD.var, statement->symbolTable);

            if (lhsType == NULL) {
                e = NEW(Error);

                e->error = SYMBOL_NOT_FOUND;
                e->val.SYMBOL_NOT_FOUND_S.id = "WRONG ERROR TYPE 1375";
                e->val.SYMBOL_NOT_FOUND_S.lineno = statement->lineno;

                return e;
            }

            e = typeCheckExpression(statement->val.assignmentD.exp, lhsType, statement->symbolTable);

            if (e == NULL_KITTY_VALUE_INDICATOR &&
                    (lhsType->kind == typeIntK ||
            lhsType->kind == typeBoolK)) {
                e = NEW(Error);

                e->error = SYMBOL_NOT_FOUND;
                e->val.SYMBOL_NOT_FOUND_S.id = "WRONG ERROR TYPE 1409";
                e->val.SYMBOL_NOT_FOUND_S.lineno = statement->lineno;

                return e;
            }

            if (e == NULL_KITTY_VALUE_INDICATOR) return NULL;
            if (e != NULL) return e;

            break;
        default:
            break;
    }

    return NULL;
}

typedef struct TypedefEncounteredLL {
    char* encountered;
    struct TypedefEncounteredLL *next;
} TypedefEncounteredLL;

Error *checkTypeExist(Type *type, SymbolTable *symbolTable, int lineno, TypedefEncounteredLL *typedefEncounteredLL) {
    Error *e;
    SYMBOL *symbol;
    VarDelList *vdl;
    TypedefEncounteredLL *iter;

    switch (type->kind) {
        case typeIdK:
            //If we have already encountered the current type
            iter = typedefEncounteredLL;

            while (iter != NULL) {
                if (strcmp(iter->encountered, type->val.idType.id) == 0) {
                    return NULL;
                }

                iter = iter->next;
            }

            symbol = getSymbol(symbolTable, type->val.idType.id);

            if (symbol == NULL) {
                e = NEW(Error);

                e->error = SYMBOL_NOT_FOUND;
                e->val.SYMBOL_NOT_FOUND_S.id = type->val.idType.id;
                e->val.SYMBOL_NOT_FOUND_S.lineno = lineno;

                return e;
            }

            //Prepend this type
            TypedefEncounteredLL *newLL = NEW(TypedefEncounteredLL);
            newLL->next = typedefEncounteredLL;
            newLL->encountered = type->val.idType.id;

            if (symbol->value->kind == typeK) {
                e = checkTypeExist(symbol->value->val.typeD.tpe, symbolTable, lineno, newLL);
                if (e != NULL) return e;
            } else if (symbol->value->kind == symTypeClassK) {
                return NULL;
            } else {
                e = checkTypeExist(symbol->value->val.typeFunctionD.returnType, symbolTable, lineno, newLL);
                if (e != NULL) return e;

                vdl = symbol->value->val.typeFunctionD.tpe;

                while (vdl != NULL) {

                    e = checkTypeExist(vdl->type, symbolTable, lineno, newLL);
                    if (e != NULL) return e;

                    vdl = vdl->next;
                }
            }
            return NULL;
            break;
        case typeArrayK:
            return checkTypeExist(type->val.arrayType.type, symbolTable, lineno, typedefEncounteredLL);
            break;
        case typeRecordK:
            vdl = type->val.recordType.types;

            while (vdl != NULL) {

                e = checkTypeExist(vdl->type, symbolTable, lineno, typedefEncounteredLL);
                if (e != NULL) return e;

                vdl = vdl->next;
            }
            break;
        case typeLambdaK:
            e = checkTypeExist(type->val.typeLambdaK.returnType, symbolTable, lineno, typedefEncounteredLL);
            if (e != NULL) return e;

            TypeList *typeList = type->val.typeLambdaK.typeList;

            while (typeList != NULL) {

                e = checkTypeExist(typeList->type, symbolTable, lineno, typedefEncounteredLL);
                if (e != NULL) return e;

                typeList = typeList->next;
            }

            break;
        default:
            return NULL;
            break;
    }
}

SYMBOL *unwrapTypedefToClassId(Type *type, SymbolTable *symbolTable) {
    SYMBOL *symbol = NULL;
    switch (type->kind) {
        case typeIdK:
            symbol = getSymbol(symbolTable, type->val.idType.id);

            if (symbol == NULL) {
                return NULL;
            }

            if (symbol->value->kind != symTypeClassK) {
                return NULL;
            }

            return symbol;
            break;
        default:
            return NULL;
            break;
    }
}

bool checkIfIsSubtype(TypeList *extended, char *subType, SymbolTable *symbolTable) {
    TypeList *ext = extended;

    while (ext != NULL) {
        //If the extension name matches the id
        if (strcmp(subType, extended->type->val.idType.id) == 0) {
            return true;
        }
        //Check all extensions of this extension
        SYMBOL *symbol = getSymbol(symbolTable, extended->type->val.idType.id);

        if (symbol == NULL) {
            return false;
        }

        if (symbol->value->kind != symTypeClassK) {
            return false;
        }

        if (checkIfIsSubtype(symbol->value->val.typeClassD.extendedClasses, subType, symbolTable) == true) {
            return true;
        }

        ext = ext->next;
    }

    return false;
}

Error *checkNestedGenericBoundType(TypeList *bound, TypeList *generic, SymbolTable *symbolTable) {
    Error *e = NULL;

    TypeList *boundCounter = bound;
    TypeList *genericCounter = generic;

    while (boundCounter != NULL && genericCounter != NULL) {
        //If the bound type is a class check all the classes required arguments
        if (boundCounter->type->kind == typeClassK) {
            SYMBOL *classSymbol = getSymbol(symbolTable, boundCounter->type->val.typeClass.classId);

            if (classSymbol->value->kind == typeK) {
                classSymbol = unwrapTypedefToClassId(classSymbol->value->val.typeD.tpe, symbolTable);
            }

            if (classSymbol == NULL) {
                e = NEW(Error);

                e->error = SYMBOL_NOT_FOUND;
                e->val.SYMBOL_NOT_FOUND_S.id = "WRONG ERROR TYPE; FIX LATER LINE 1497";
                e->val.SYMBOL_NOT_FOUND_S.lineno = 42;

                return e;
            }

            if (classSymbol->value->kind != symTypeClassK) {
                e = NEW(Error);

                e->error = SYMBOL_NOT_FOUND;
                e->val.SYMBOL_NOT_FOUND_S.id = "WRONG ERROR TYPE; FIX LATER LINE 1507";
                e->val.SYMBOL_NOT_FOUND_S.lineno = 42;

                return e;
            }

            e = checkNestedGenericBoundType(boundCounter->type->val.typeClass.genericBoundValues,
                    classSymbol->value->val.typeClassD.generics,
                    symbolTable);
            if (e != NULL) return e;
        }

        boundCounter = boundCounter->next;
        genericCounter = genericCounter->next;
    }

    //If the count mismatches
    if (boundCounter != NULL) {
        e = NEW(Error);

        e->error = SYMBOL_NOT_FOUND;
        e->val.SYMBOL_NOT_FOUND_S.id = "WRONG ERROR TYPE; FIX LATER LINE 1536";
        e->val.SYMBOL_NOT_FOUND_S.lineno = 42;

        return e;
    }

    //If the count mismatches
    if (genericCounter != NULL) {
        e = NEW(Error);

        e->error = SYMBOL_NOT_FOUND;
        e->val.SYMBOL_NOT_FOUND_S.id = "WRONG ERROR TYPE; FIX LATER LINE 1547";
        e->val.SYMBOL_NOT_FOUND_S.lineno = 42;

        return e;
    }

    return NULL;
}

Error *checkDeclValidity(Declaration *declaration) {
    Error *e = NULL;
    Declaration *decl;
    int lineno = declaration->lineno;

    switch (declaration->kind) {
        case declVarK:
            e = checkTypeExist(declaration->val.varD.type, declaration->symbolTable, lineno, NULL);
            if (e != NULL) return e;
            //Todo FIX THIS CODE FOR GENERIC BINDINGS
            //return NULL;
            //If its type is a class, check the generic bindings' validity
            if (declaration->val.varD.type->kind == typeClassK) {
                TypeList *boundVars = declaration->val.varD.type->val.typeClass.genericBoundValues;
                SYMBOL *symbol = getSymbol(declaration->symbolTable, declaration->val.varD.type->val.typeClass.classId);

                if (symbol == NULL) {
                    e = NEW(Error);

                    e->error = SYMBOL_NOT_FOUND;
                    e->val.SYMBOL_NOT_FOUND_S.id = "WRONG ERROR TYPE; FIX LATER LINE 1415";
                    e->val.SYMBOL_NOT_FOUND_S.lineno = 42;

                    return e;
                }

                if (symbol->value->kind != symTypeClassK) {
                    e = NEW(Error);

                    e->error = SYMBOL_NOT_FOUND;
                    e->val.SYMBOL_NOT_FOUND_S.id = "WRONG ERROR TYPE; FIX LATER LINE 1425";
                    e->val.SYMBOL_NOT_FOUND_S.lineno = 42;

                    return e;
                }

                TypeList *classGenerics = symbol->value->val.typeClassD.generics;

                //We check the count, because the next part can abruptly return
                e = checkNestedGenericBoundType(boundVars, classGenerics, declaration->symbolTable);
                if (e != NULL) return e;

                //The real handling
                while (boundVars != NULL && classGenerics != NULL) {
                    //Check if bound var is of allowed type

                    if (classGenerics->type->val.typeGeneric.subType != NULL) {
                        if (strcmp(classGenerics->type->val.typeGeneric.subType, boundVars->type->val.idType.id) == 0) {
                            return NULL;
                        }

                        SYMBOL *symbols = getSymbol(declaration->symbolTable, boundVars->type->val.typeClass.classId);

                        if (symbols == NULL) {
                            e = NEW(Error);

                            e->error = SYMBOL_NOT_FOUND;
                            e->val.SYMBOL_NOT_FOUND_S.id = "WRONG ERROR TYPE; FIX LATER LINE 1504";
                            e->val.SYMBOL_NOT_FOUND_S.lineno = 42;

                            return e;
                        }

                        if (symbols->value->kind != symTypeClassK) {
                            e = NEW(Error);

                            e->error = SYMBOL_NOT_FOUND;
                            e->val.SYMBOL_NOT_FOUND_S.id = "WRONG ERROR TYPE; FIX LATER LINE 1514";
                            e->val.SYMBOL_NOT_FOUND_S.lineno = 42;

                            return e;
                        }

                        if (symbols->value->val.typeClassD.extendedClasses == NULL) {
                            e = NEW(Error);

                            e->error = SYMBOL_NOT_FOUND;
                            e->val.SYMBOL_NOT_FOUND_S.id = "WRONG ERROR TYPE; FIX LATER LINE 1524";
                            e->val.SYMBOL_NOT_FOUND_S.lineno = 42;

                            return e;
                        }

                        if (checkIfIsSubtype(symbols->value->val.typeClassD.extendedClasses,
                                classGenerics->type->val.typeGeneric.subType,
                                declaration->symbolTable) == true) {
                            return NULL;
                        }
                    }

                    classGenerics = classGenerics->next;
                    boundVars = boundVars->next;
                }


            }

            break;
        case declVarsK:
            decl = declaration;

            while (decl != NULL) {
                e = checkDeclValidity(decl->val.varsD.var);
                if (e != NULL) return e;

                decl = decl->val.varsD.next;
            }

            break;
        case declTypeK:
            e = checkTypeExist(declaration->val.typeD.type, declaration->symbolTable, lineno, NULL);
            if (e != NULL) return e;
            break;
        default:
            return NULL;
            break;
    }

    return NULL;
}

Error *typeCheckDeclaration(Declaration *declaration) {
    DeclarationList *classDeclList;
    Error *e = NULL;

    switch (declaration->kind)  {
        case declFuncK:
            e = typeCheck(declaration->val.functionD.function->body,
                          declaration->val.functionD.function->head->returnType);
            if (e != NULL) return e;
            break;
        case declValK:
            if (declaration->val.valD.tpe->kind == typeLambdaK) {
                //Expression can be lambda or an id that references to a lambda
                if (declaration->val.valD.rhs->val.termD.term->kind == variableK) {
                    //Unpack the id and get the lambda
                    /*Variable *var = declaration->val.valD.rhs->val.termD.term->val.variableD.var;
                    Type *unwrapped = unwrapVariable(var, declaration->symbolTable);
                    printf("Hello");*/
                    //Actually, if the lambda has already been bound, no reason to type check its body.
                } else {
                    Lambda *lambda = declaration->val.valD.rhs->val.termD.term->val.lambdaD.lambda;

                    e = typeCheck(lambda->body, lambda->returnType);
                    if (e != NULL) return e;
                }
            } else {
                e = typeCheckExpression(declaration->val.valD.rhs, declaration->val.valD.tpe, declaration->symbolTable);
                if (e != NULL) return e;
            }
            break;
        case declClassK:
            classDeclList = declaration->val.classD.declarationList;

            while (classDeclList != NULL) {
                e = typeCheckDeclaration(classDeclList->declaration);
                if (e != NULL) return e;

                classDeclList = classDeclList->next;
            }

            break;
            //In any other case we want to check if the types they are assigned to even exist
        default:
            e = checkDeclValidity(declaration);
            if (e != NULL) return e;
            break;
    }

    return NULL;
}

//We want to unwrap the expression types and compare them, effectively folding the nest of types
//An easy way to do this is to for every expression, we want to check the symbol table for a matching type
//We have been preemptively smart and decorated the AST's statements with symbol pointers also!
Error *typeCheck(Body *body, Type *functionReturnType) {
    DeclarationList *declarationList = body->declarationList;
    StatementList *statementList = body->statementList;
    SYMBOL *symbol;
    Declaration *declaration;
    Error *e = NULL;

    while (declarationList != NULL) {

        e = typeCheckDeclaration(declarationList->declaration);
        if (e != NULL) return e;

        declarationList = declarationList->next;
    }

    while (statementList != NULL) {

        e = typeCheckStatement(statementList->statement,
                functionReturnType);
        if (e != NULL) return e;

        statementList = statementList->next;
    }

    return NULL;
}
