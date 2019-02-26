//
// Created by valde on 2/24/19.
//

#include "type_checker.h"
#include "../ast/tree.h"
#include "../error/error.h"

Error *typeCheckExpression(Expression *expression, TypeKind expectedType);

Error *typeCheckVariable(Variable* variable, TypeKind expectedType, SymbolTable *symbolTable) {
    Error *e = NULL;
    SYMBOL *symbol = NULL;

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

            if (symbol->tpe->kind != expectedType) {
                e = NEW(Error);

                e->error = SYMBOL_NOT_FOUND;
                e->val.SYMBOL_NOT_FOUND_S.id = variable->val.idD.id;
                e->val.SYMBOL_NOT_FOUND_S.lineno = variable->lineno;

                return e;
            }
            break;
        case arrayIndexK:
            e = typeCheckVariable(variable->val.arrayIndexD.var, expectedType, symbolTable);
            if (e != NULL) return e;

            e = typeCheckExpression(variable->val.arrayIndexD.idx, typeIntK);
            if (e != NULL) return e;
            break;
        case recordLookupK:
            e = typeCheckVariable(variable->val.recordLookupD.var, expectedType, symbolTable);
            if (e != NULL) return e;

            //How do record?

            break;
    }
}

Error *typeCheckTerm(Term *term, TypeKind expectedType, SymbolTable *symbolTable) {
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

            symbol = getSymbol(symbolTable, term->val.functionCallD.functionId);

            if (symbol == NULL) {
                e = NEW(Error);

                e->error = SYMBOL_NOT_FOUND;
                e->val.SYMBOL_NOT_FOUND_S.id = term->val.functionCallD.functionId;
                e->val.SYMBOL_NOT_FOUND_S.lineno = term->lineno;

                return e;
            }

            if (!symbol->isFunc) {
                e = NEW(Error);

                e->error = TYPE_TERM_IS_NOT_FUNCTION;
                e->val.TYPE_TERM_IS_NOT_FUNCTION_S.lineno = term->lineno;
                e->val.TYPE_TERM_IS_NOT_FUNCTION_S.fid = term->val.functionCallD.functionId;

                return e;
            }

            if (symbol->tpe->kind != expectedType) {
                e = NEW(Error);

                e->error = TYPE_TERM_INVALID_FUNCTION_CALL_RETURN_TYPE;
                e->val.TYPE_TERM_INVALID_FUNCTION_CALL_RETURN_TYPE_S.lineno = term->lineno;
                e->val.TYPE_TERM_INVALID_FUNCTION_CALL_RETURN_TYPE_S.fid = term->val.functionCallD.functionId;

                return e;
            }

            //Now for checking the parameter list
            //First we need to find the function
            int paramNum = 0;
            char *fncName = term->val.functionCallD.functionId;
            char *paramName = (char*)malloc(sizeof(char) * (strlen(fncName) + 10));
            char asString[16];

            //We also need to traverse the call
            ExpressionList *expressionList = term->val.functionCallD.expressionList;

            //This could be done as do while, but its fine
            sprintf(asString, "%i", paramNum);
            strcat(paramName, fncName);
            strcat(paramName, FUNCTION_PARAM_SUFFIX);
            strcat(paramName, asString);

            symbol = getSymbol(symbolTable, paramName);

            while (symbol != NULL && expressionList != NULL) {
                //This MIGHT cause an error, test later
                e = typeCheckExpression(expressionList->expression, expectedType);
                if (e != NULL) {
                    e->error = TYPE_TERM_FUNCTION_CALL_EXPRESSION_NOT_MATCH_SIGNATURE;
                    e->val.TYPE_TERM_FUNCTION_CALL_EXPRESSION_NOT_MATCH_SIGNATURE_S.lineno = term->lineno;
                    e->val.TYPE_TERM_FUNCTION_CALL_EXPRESSION_NOT_MATCH_SIGNATURE_S.fid = term->val.functionCallD.functionId;
                    e->val.TYPE_TERM_FUNCTION_CALL_EXPRESSION_NOT_MATCH_SIGNATURE_S.expectedType = symbol->tpe->kind;
                    e->val.TYPE_TERM_FUNCTION_CALL_EXPRESSION_NOT_MATCH_SIGNATURE_S.foundType = expectedType;
                    e->val.TYPE_TERM_FUNCTION_CALL_EXPRESSION_NOT_MATCH_SIGNATURE_S.argNumber = paramNum;

                    return e;
                }

                sprintf(asString, "%i", paramNum);
                strcat(paramName, fncName);
                strcat(paramName, FUNCTION_PARAM_SUFFIX);
                strcat(paramName, asString);

                symbol = getSymbol(symbolTable, paramName);
                expressionList = expressionList->next;

                paramName[0] = '\0'; //Empty string
                paramNum++;
            }

            if ((symbol == NULL && expressionList != NULL) || (symbol != NULL && expressionList == NULL)) {
                //Error
                e = NEW(Error);

                e->error = TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH;
                e->val.TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH_S.lineno = term->lineno;
                e->val.TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH_S.fid = term->val.functionCallD.functionId;
                e->val.TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH_S.foundCount = paramNum;

                return e;
            }


            break;
        case parenthesesK:
            e = typeCheckExpression(term->val.parenthesesD.expression, expectedType);
            if (e != NULL) return e;
            break;
        case negateK:
            if (expectedType != typeBoolK) {
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
            if (expectedType != typeIntK) {
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
            if (expectedType != typeIntK) {
                e = NEW(Error);

                e->error = TYPE_TERM_NOT_INTEGER;
                e->val.TYPE_TERM_NOT_INTEGER_S.lineno = term->lineno;
                e->val.TYPE_TERM_NOT_INTEGER_S.termThatCausedError = term;

                return e;
            }
            break;
        case trueK:
            if (expectedType != typeBoolK) {
                e = NEW(Error);

                e->error = TYPE_TERM_NOT_BOOLEAN;
                e->val.TYPE_TERM_NOT_BOOLEAN_S.lineno = term->lineno;
                e->val.TYPE_TERM_NOT_BOOLEAN_S.termThatCausedError = term;

                return e;
            }
            break;
        case falseK:
            if (expectedType != typeBoolK) {
                e = NEW(Error);

                e->error = TYPE_TERM_NOT_BOOLEAN;
                e->val.TYPE_TERM_NOT_BOOLEAN_S.lineno = term->lineno;
                e->val.TYPE_TERM_NOT_BOOLEAN_S.termThatCausedError = term;

                return e;
            }
            break;
        case nullK:
            //What do?
            break;
        default:
            break;
    }

    return NULL;
}

Error *typeCheckExpression(Expression *expression, TypeKind expectedType) {
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
            isBoolean = false;

            switch (expression->val.op.operator->kind) {
                case opMultK:
                    isBoolean = true;
                    break;
                case opDivK:
                    isBoolean = true;
                    break;
                case opPlusK:
                    isBoolean = true;
                    break;
                case opMinusK:
                    isBoolean = true;
                    break;
                default:
                    break;
            }

            //If types do not match
            if (isBoolean == true && expectedType != typeBoolK) {
                e = NEW(Error);

                e->error = TYPE_EXPRESSION_IS_NOT_BOOLEAN;
                e->val.TYPE_EXPRESSION_IS_NOT_BOOLEAN_S.lineno = expression->lineno;
                e->val.TYPE_EXPRESSION_IS_NOT_BOOLEAN_S.expThatCausedError = expression;
                e->val.TYPE_EXPRESSION_IS_NOT_BOOLEAN_S.expectedType = expectedType;

                return e;
            }

            //If they do match, we check children!
            e = typeCheckExpression(expression->val.op.left, expectedType);
            if (e != NULL) return e;
            e = typeCheckExpression(expression->val.op.right, expectedType);
            if (e != NULL) return e;

            break;
        case termK:
            e = typeCheckTerm(expression->val.termD.term,
                    expectedType,
                    expression->symbolTable);
            if (e != NULL) return e;
            break;
        default:
            break;
    }

    return NULL;
}

//We need to track the return type, since we can have deeeeeeeep dwelling return statements
Error *typeCheckStatement(Statement *statement, TypeKind functionReturnType) {
    Error *e = NULL;
    StatementList *statementList;
    SYMBOL *symbol;

    switch (statement->kind) {
        case statReturnK:
            e = typeCheckExpression(statement->val.returnD.exp,
                    functionReturnType);
            if (e != NULL) return e;
            break;
        case statWriteK:
            e = typeCheckExpression(statement->val.writeD.exp,
                                    typeBoolK);
            if (e != NULL) return e;

            e = typeCheckExpression(statement->val.writeD.exp,
                                    typeIntK);
            if (e != NULL) return e;
            break;
        /*case statAllocateK:
            e = typeCheckExpression(statement->val.allocateD.exp,);
            if (e != NULL) return e;
            break;*/
        case statAllocateLenK:
            e = typeCheckExpression(statement->val.allocateLenD.len,
                    typeIntK);
            if (e != NULL) return e;
            break;
        case statIfK:
            e = typeCheckExpression(statement->val.ifD.exp,
                    typeBoolK);
            if (e != NULL) return e;

            e = typeCheckStatement(statement->val.ifD.statement,
                                   functionReturnType);
            if (e != NULL) return e;
            break;
        case statIfElK:
            e = typeCheckExpression(statement->val.ifElD.exp,
                                    typeBoolK);
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
                                    typeBoolK);
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
            symbol = getSymbol(statement->symbolTable, statement->val.assignmentD.var->val.idD.id);

            if (symbol == NULL) {
                e = NEW(Error);

                e->error = SYMBOL_NOT_FOUND;
                e->val.SYMBOL_NOT_FOUND_S.id = statement->val.assignmentD.var->val.idD.id;
                e->val.SYMBOL_NOT_FOUND_S.lineno = statement->lineno;

                return e;
            }

            e = typeCheckExpression(statement->val.assignmentD.exp,
                                    symbol->tpe->kind);
            if (e != NULL) return e;
            break;
        default:
            break;
    }

    return NULL;
}

//We want to unwrap the expression types and compare them, effectively folding the nest of types
//An easy way to do this is to for every expression, we want to check the symbol table for a matching type
//We have been preemptively smart and decorated the AST's statements with symbol pointers also!
Error *typeCheck(Body *body, TypeKind functionReturnType) {
    Error *e = NULL;
    DeclarationList *declarationList = body->declarationList;
    StatementList *statementList = body->statementList;

    while (declarationList != NULL) {

        switch (declarationList->declaration->kind)  {
            case declFuncK:
                e = typeCheck(declarationList->declaration->val.functionD.function->body,
                        declarationList->declaration->val.functionD.function->head->returnType->kind);
                if (e != NULL) return e;
                break;
            default:
                break;
        }

        declarationList = declarationList->next;
    }

    while (statementList != NULL) {

        typeCheckStatement(statementList->statement,
                functionReturnType);

        statementList = statementList->next;
    }

    return NULL;
}
