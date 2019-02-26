//
// Created by valde on 2/24/19.
//

#include "type_checker.h"
#include "../ast/tree.h"
#include "../error/error.h"
#include "../symbol/symbol.h"

Error *typeCheckExpression(Expression *expression, TypeKind expectedType, SymbolTable *symbolTable);

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

            if (symbol->value->kind != typeK) {
                //Incorrect error
                e = NEW(Error);

                e->error = SYMBOL_NOT_FOUND;
                e->val.SYMBOL_NOT_FOUND_S.id = variable->val.idD.id;
                e->val.SYMBOL_NOT_FOUND_S.lineno = variable->lineno;

                return e;
            }

            if (symbol->value->val.typeD.tpe->kind != expectedType) {
                //Incorrect error
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

            e = typeCheckExpression(variable->val.arrayIndexD.idx, typeIntK, symbolTable);
            if (e != NULL) return e;
            break;
        case recordLookupK:
            e = typeCheckVariable(variable->val.recordLookupD.var, expectedType, symbolTable);
            if (e != NULL) return e;

            variable->val.recordLookupD.

            break;
    }

    return NULL;
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

            if (symbol->value->kind != typeFunctionK) {
                e = NEW(Error);

                e->error = TYPE_TERM_IS_NOT_FUNCTION;
                e->val.TYPE_TERM_IS_NOT_FUNCTION_S.lineno = term->lineno;
                e->val.TYPE_TERM_IS_NOT_FUNCTION_S.fid = term->val.functionCallD.functionId;

                return e;
            }

            if (symbol->value->val.typeFunctionD.returnType->kind != expectedType) {
                e = NEW(Error);

                e->error = TYPE_TERM_INVALID_FUNCTION_CALL_RETURN_TYPE;
                e->val.TYPE_TERM_INVALID_FUNCTION_CALL_RETURN_TYPE_S.lineno = term->lineno;
                e->val.TYPE_TERM_INVALID_FUNCTION_CALL_RETURN_TYPE_S.fid = term->val.functionCallD.functionId;

                return e;
            }

            ExpressionList *expressionList = term->val.functionCallD.expressionList;
            VarDelList *varDelList = symbol->value->val.typeFunctionD.tpe;

            while (expressionList != NULL && varDelList != NULL) {

                expressionList = expressionList->next;
                varDelList = varDelList->next;
            }

            if ((varDelList == NULL && expressionList != NULL) || (varDelList != NULL && expressionList == NULL)) {
                //Error
                e = NEW(Error);

                e->error = TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH;
                e->val.TYPE_.lineno = term->lineno;
                e->val.TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH_S.fid = term->val.functionCallD.functionId;
                e->val.TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH_S.foundCount = paramNum;

                return e;
            }

            break;
        case parenthesesK:
            e = typeCheckExpression(term->val.parenthesesD.expression, expectedType, symbolTable);
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

Error *typeCheckExpression(Expression *expression, TypeKind expectedType, SymbolTable *symbolTable) {

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

            //If types do not match
            if ((isBoolean == true && expectedType != typeBoolK ) ||
                    (isBoolean == false && expectedType != typeIntK)) {
                e = NEW(Error);

                e->error = TYPE_EXPRESSION_IS_NOT_AS_EXPECTED;
                e->val.TYPE_EXPRESSION_IS_NOT_AS_EXPECTED_S.lineno = expression->lineno;
                e->val.TYPE_EXPRESSION_IS_NOT_AS_EXPECTED_S.expThatCausedError = expression;
                e->val.TYPE_EXPRESSION_IS_NOT_AS_EXPECTED_S.expectedType = expectedType;

                return e;
            }


            //If they do match, we check children!
            e = typeCheckExpression(expression->val.op.left, expectedType, symbolTable);
            if (e != NULL) return e;
            e = typeCheckExpression(expression->val.op.right, expectedType, symbolTable);
            if (e != NULL) return e;

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
Error *typeCheckStatement(Statement *statement, TypeKind functionReturnType) {
    Error *e = NULL;
    StatementList *statementList;
    SYMBOL *symbol;

    switch (statement->kind) {
        case statReturnK:
            e = typeCheckExpression(statement->val.returnD.exp,
                    functionReturnType,
                    statement->symbolTable);
            if (e != NULL) return e;
            break;
        case statWriteK:
            e = typeCheckExpression(statement->val.writeD.exp,
                                    typeBoolK,
                                    statement->symbolTable);
            if (e != NULL) return e;

            e = typeCheckExpression(statement->val.writeD.exp,
                                    typeIntK,
                                    statement->symbolTable);
            if (e != NULL) return e;
            break;
        /*case statAllocateK:
            e = typeCheckExpression(statement->val.allocateD.exp,);
            if (e != NULL) return e;
            break;*/
        case statAllocateLenK:
            e = typeCheckExpression(statement->val.allocateLenD.len,
                    typeIntK,
                                    statement->symbolTable);
            if (e != NULL) return e;
            break;
        case statIfK:
            e = typeCheckExpression(statement->val.ifD.exp,
                    typeBoolK,
                                    statement->symbolTable);
            if (e != NULL) return e;

            e = typeCheckStatement(statement->val.ifD.statement,
                                   functionReturnType);
            if (e != NULL) return e;
            break;
        case statIfElK:
            e = typeCheckExpression(statement->val.ifElD.exp,
                                    typeBoolK,
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
                                    typeBoolK,
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
            symbol = getSymbol(statement->symbolTable, statement->val.assignmentD.var->val.idD.id);

            if (symbol == NULL) {
                e = NEW(Error);

                e->error = SYMBOL_NOT_FOUND;
                e->val.SYMBOL_NOT_FOUND_S.id = statement->val.assignmentD.var->val.idD.id;
                e->val.SYMBOL_NOT_FOUND_S.lineno = statement->lineno;

                return e;
            }

            e = typeCheckExpression(statement->val.assignmentD.exp,
                                    symbol->tpe->kind,
                                    statement->symbolTable);
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

        e = typeCheckStatement(statementList->statement,
                functionReturnType);
        if (e != NULL) return e;

        statementList = statementList->next;
    }

    return NULL;
}
