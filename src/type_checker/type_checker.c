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
#define NULL_KITTY_VALUE_INDICATOR ((void *)1)

Error *typeCheckExpression(Expression *expression, Type *expectedType, SymbolTable *symbolTable);
Type *unwrapTypedef(Type *type, SymbolTable *symbolTable);
bool areTypesEqual(Type *first, Type *second, SymbolTable *symbolTable);
Type *unwrapVariable(Variable *variable, SymbolTable *symbolTable);
Type *evaluateExpressionType(Expression *expression, SymbolTable *symbolTable);
//Go all the way down through variable, then once at bottom get type
//when we get type start returning, apply subscripting and such as we go up


Type *evaluateTermType(Term *term, SymbolTable *symbolTable) {
    SYMBOL *symbol;
    Type *type;

    switch (term->kind) {
        case variableK:
            return unwrapVariable(term->val.variableD.var, symbolTable);
            break;
        case functionCallK:
            symbol = getSymbol(symbolTable, term->val.functionCallD.functionId);

            if (symbol == NULL || symbol->value->kind != typeFunctionK) {
                return NULL;
            }

            return symbol->value->val.typeFunctionD.returnType;
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

            while (firstDelList != NULL && secondDelList != NULL) {

                if (areTypesEqual(firstDelList->type, secondDelList->type, symbolTable) == false) {
                    return false;
                }

                firstDelList = firstDelList->next;
                secondDelList = secondDelList->next;
            }

            if (firstDelList != NULL || secondDelList != NULL) {
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

                return unwrapTypedef(symbol->value->val.typeD.tpe, symbolTable);
            break;
        default:
            return type;
            break;
    }
}

Type *unwrapVariable(Variable *variable, SymbolTable *symbolTable) {
    SYMBOL *symbol;
    Variable *base;
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

            //Check if expression is int
            e = typeCheckExpression(variable->val.arrayIndexD.idx, &intStaticType, symbolTable);
            if (e != NULL) return NULL;

            //Since we are in an array subscript, we want to subscript by removing a type layer
            if (innerType->kind == typeArrayK) {
                return innerType->val.arrayType.type;
            } else {
                return NULL;
            }

            break;
        case recordLookupK:
            innerType = unwrapTypedef(unwrapVariable(variable->val.recordLookupD.var, symbolTable), symbolTable);

            if (innerType == NULL) {
                return NULL;
            }

            //Check if id is in the type's vardellist
            varDelList = innerType->val.recordType.types;

            while (varDelList != NULL) {

                if (strcmp(variable->val.recordLookupD.id, varDelList->identifier) == 0) {
                    //ID
                    return varDelList->type;
                }

                varDelList = varDelList->next;
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

            if (symbol->value->kind != typeK) {
                e = NEW(Error);

                e->error = SYMBOL_NOT_FOUND;
                e->val.SYMBOL_NOT_FOUND_S.id = variable->val.idD.id;
                e->val.SYMBOL_NOT_FOUND_S.lineno = variable->lineno;

                return e;
            }

            //This segfaults, pls fix, it stack overflows
            symAsType = symbol->value->val.typeD.tpe;

            if (areTypesEqual(symAsType, expectedType, symbolTable) == false) {
                e = NEW(Error);

                e->error = VARIABLE_UNEXPECTED_TYPE;
                e->val.VARIABLE_UNEXPECTED_TYPE_S.id = variable->val.idD.id;
                e->val.VARIABLE_UNEXPECTED_TYPE_S.lineno = variable->lineno;
                e->val.VARIABLE_UNEXPECTED_TYPE_S.expectedType = expectedType->kind;
                e->val.VARIABLE_UNEXPECTED_TYPE_S.foundType = symAsType->kind;


                return e;
            }

            break;
        case arrayIndexK:
            e = typeCheckVariable(variable->val.arrayIndexD.var, expectedType, symbolTable);
            if (e != NULL) return e;

            e = typeCheckExpression(variable->val.arrayIndexD.idx, &intStaticType, symbolTable);
            if (e != NULL) return e;
            break;
        case recordLookupK:

            //The id subscript decides the return type
            /*
            e = typeCheckVariable(variable->val.recordLookupD.var, expectedType, symbolTable);
            if (e != NULL) return e;*/

            if (areTypesEqual(expectedType, unwrapVariable(variable, symbolTable), symbolTable) == false) {
                e = NEW(Error);

                e->error = SYMBOL_NOT_FOUND;
                e->val.SYMBOL_NOT_FOUND_S.id = variable->val.idD.id;
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
               areTypesEqual(symbol->value->val.typeFunctionD.returnType, expectedType, symbolTable) == false) ||
               (symbol->value->val.typeD.tpe->kind == typeLambdaK &&
               areTypesEqual(symbol->value->val.typeD.tpe->val.typeLambdaK.returnType, expectedType, symbolTable) == false)) {
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

            e = typeCheckTerm(term->val.negateD.term, expectedType, symbolTable);
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
        default:
            break;
    }

    return NULL;
}

Error *typeCheckExpression(Expression *expression, Type *expectedType, SymbolTable *symbolTable) {

    Error *e = NULL;
    SYMBOL *symbol;
    bool isBoolean;

    switch (expression->kind) {
        /*case idK:
            symbol = getSymbol(expression->symbolTable, expression->val.idE);

            if (expression->val->)

            break;
        case intconstK:

            if (expectedType)

            break;*/
        case opK:
            //We want to check if the operator is boolean or not
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

            //Check if the operator matches the expected return type
            if ((isBoolean == true && areTypesEqual(expectedType, &booleanStaticType, symbolTable) == false) ||
                    (isBoolean == false && areTypesEqual(expectedType, &intStaticType, symbolTable) == false)) {
                e = NEW(Error);

                e->error = TYPE_EXPRESSION_IS_NOT_AS_EXPECTED;
                e->val.TYPE_EXPRESSION_IS_NOT_AS_EXPECTED_S.lineno = expression->lineno;
                e->val.TYPE_EXPRESSION_IS_NOT_AS_EXPECTED_S.expThatCausedError = expression;
                e->val.TYPE_EXPRESSION_IS_NOT_AS_EXPECTED_S.expectedType = expectedType->kind;

                return e;
            }

            Error *e1 = NULL;
            Error *e2 = NULL;

            //The idea here is that either one of the sides are null or they are both integer or both boolean.

            //Equality and inequality works on both booleans and integers
            if (expression->val.op.operator->kind == opEqualityK ||
                    expression->val.op.operator->kind == opInequalityK) {
                //Check for int
                e1 = typeCheckExpression(expression->val.op.left, &intStaticType, symbolTable);
                e2 = typeCheckExpression(expression->val.op.right, &intStaticType, symbolTable);

                if (e1 == NULL_KITTY_VALUE_INDICATOR || e2 == NULL_KITTY_VALUE_INDICATOR) {
                    return NULL;
                }

                if (e1 == NULL && e2 == NULL) {
                    return NULL;
                }

                //Also check for bool
                e1 = typeCheckExpression(expression->val.op.left, &booleanStaticType, symbolTable);
                e2 = typeCheckExpression(expression->val.op.right, &booleanStaticType, symbolTable);

                if (e1 == NULL_KITTY_VALUE_INDICATOR || e2 == NULL_KITTY_VALUE_INDICATOR) {
                    return NULL;
                }

                if (e1 == NULL && e2 == NULL) {
                    return NULL;
                }

                if (e1 != NULL) return e1;
                if (e2 != NULL) return e2;
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

//We need to track the return type, since we can have deeeeeeeep dwelling return statements
Error *typeCheckStatement(Statement *statement, Type *functionReturnType) {
    Error *e = NULL;
    StatementList *statementList;
    SYMBOL *symbol;
    VarDelList *varDelList;
    Variable *topVariable;
    char *innerId;
    Type *lhsType;

    switch (statement->kind) {
        case statReturnK:
            e = typeCheckExpression(statement->val.returnD.exp,
                    functionReturnType,
                    statement->symbolTable);
            if (e != NULL) return e;
            dumpSymbolTable(statement->symbolTable);
            break;
        case statWriteK:
            e = typeCheckExpression(statement->val.writeD.exp,
                                    &booleanStaticType,
                                    statement->symbolTable);
            if (e != NULL) return e;

            e = typeCheckExpression(statement->val.writeD.exp,
                                    &intStaticType,
                                    statement->symbolTable);
            if (e != NULL) return e;
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
            //Assignment is a bit funny, we have to check if the  and RHS are the same
            //We have to find out what the LHS is first

            lhsType = unwrapVariable(statement->val.assignmentD.var, statement->symbolTable);

            e = typeCheckExpression(statement->val.assignmentD.exp, lhsType, statement->symbolTable);
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

Error *checkDeclValidity(Declaration *declaration) {
    Error *e = NULL;
    Declaration *decl;
    int lineno = declaration->lineno;

    switch (declaration->kind) {
        case declVarK:
            e = checkTypeExist(declaration->val.varD.type, declaration->symbolTable, lineno, NULL);
            if (e != NULL) return e;
            break;
        case declVarsK:
            decl = declaration;

            while (decl != NULL) {
                e = checkTypeExist(decl->val.varsD.var->val.varD.type, decl->symbolTable, lineno, NULL);
                if (e != NULL) return e;

                if (declaration->kind == declVarK) {
                    //Last one
                    e = checkTypeExist(decl->val.varD.type, declaration->symbolTable, lineno, NULL);
                    if (e != NULL) return e;
                    break;
                }

                decl = declaration->val.varsD.next;
            }

            break;
        case declTypeK:
            e = checkTypeExist(declaration->val.typeD.type, declaration->symbolTable, lineno, NULL);
            if (e != NULL) return e;
            break;
    }
}

//We want to unwrap the expression types and compare them, effectively folding the nest of types
//An easy way to do this is to for every expression, we want to check the symbol table for a matching type
//We have been preemptively smart and decorated the AST's statements with symbol pointers also!
Error *typeCheck(Body *body, Type *functionReturnType) {
    Error *e = NULL;
    DeclarationList *declarationList = body->declarationList;
    StatementList *statementList = body->statementList;
    SYMBOL *symbol;
    Declaration *declaration;

    while (declarationList != NULL) {

        switch (declarationList->declaration->kind)  {
            case declFuncK:
                e = typeCheck(declarationList->declaration->val.functionD.function->body,
                        declarationList->declaration->val.functionD.function->head->returnType);
                if (e != NULL) return e;
                break;
            case declValK:
                if (declarationList->declaration->val.valK.tpe->kind == typeLambdaK) {
                    //We know expression is lambda
                    Lambda *lambda = declarationList->declaration->val.valK.rhs->val.termD.term->val.lambdaD.lambda;

                    e = typeCheck(lambda->body, lambda->returnType);
                    if (e != NULL) return e;
                }
                break;
            //In any other case we want to check if the types they are assigned to even exist
            default:
                e = checkDeclValidity(declarationList->declaration);
                if (e != NULL) return e;
                break;
        }

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
