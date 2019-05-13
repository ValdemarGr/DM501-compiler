//
// Created by valde on 5/12/19.
//

#include "constant_fold.h"
#include "../ast/tree.h"
#include "../symbol/symbol.h"
#include "../sandboxer.h"

ConstMap *constantFunctionEvalutationMap = NULL;
bool inClosedContext = false;
bool hasAffectedOutside = false;
bool isAffectedByParameters = false;

void constantFoldExpression(Expression *expression, SymbolTable *symbolTable);
bool termEvaluatesToConstant(Term *term);
bool expressionEvaluatesToConstant(Expression *expression);
void constantFoldBody(Body *body);
int evaluateExpressionIntConstant(Expression *expression, SymbolTable *symbolTable) ;
int evaluateTermIntConstant(Term *expression, SymbolTable *symbolTable);
TermKind evaluateExpressionBoolConstant(Expression *expression, SymbolTable *symbolTable) ;
TermKind evaluateTermBoolConstant(Term *expression, SymbolTable *symbolTable);
bool doesBodyAffectOutside(Body *body, int evaluationLevel);

TermKind evaluateExpressionBoolConstant(Expression *expression, SymbolTable *symbolTable) {
    switch (expression->kind) {
        case opK: {
            switch (expression->val.op.operator->kind) {
                //Boolean
                case opAndK: {
                    TermKind lhs = evaluateExpressionBoolConstant(expression->val.op.left, symbolTable);
                    TermKind rhs = evaluateExpressionBoolConstant(expression->val.op.right, symbolTable);

                    expression->kind = termK;

                    //Simply move them into rhs
                    if (lhs == trueK && rhs == trueK) {
                        return trueK;
                    } else {
                        return falseK;
                    }

                } break;
                case opOrK:{
                    TermKind lhs = evaluateExpressionBoolConstant(expression->val.op.left, symbolTable);
                    TermKind rhs = evaluateExpressionBoolConstant(expression->val.op.right, symbolTable);

                    expression->kind = termK;

                    //Simply move them into rhs
                    if (lhs == trueK || rhs == trueK) {
                        return trueK;
                    } else {
                        return falseK;
                    }

                }break;
                    //Can be both
                case opEqualityK: {
                        TermKind lhs = evaluateExpressionBoolConstant(expression->val.op.left, symbolTable);
                        TermKind rhs = evaluateExpressionBoolConstant(expression->val.op.right, symbolTable);

                        expression->kind = termK;

                        //Simply move them into rhs
                        if (lhs == rhs) {
                            return trueK;
                        } else {
                            return falseK;
                        }
                } break;
                case opInequalityK:{
                        TermKind lhs = evaluateExpressionBoolConstant(expression->val.op.left, symbolTable);
                        TermKind rhs = evaluateExpressionBoolConstant(expression->val.op.right, symbolTable);

                        expression->kind = termK;

                        //Simply move them into rhs
                        if (lhs != rhs) {
                            return trueK;
                        } else {
                            return falseK;
                        }
                } break;
            }
        }break;
        case termK:
            return evaluateTermBoolConstant(expression->val.termD.term, symbolTable); break;
    }
}
TermKind evaluateTermBoolConstant(Term *term, SymbolTable *symbolTable) {
    switch (term->kind) {
        case parenthesesK: return evaluateExpressionBoolConstant(term->val.parenthesesD.expression, symbolTable); break;
        case negateK:return evaluateTermBoolConstant(term->val.negateD.term, symbolTable); break;
        case trueK: return trueK; break;
        case falseK: return falseK; break;
    }
}
int evaluateExpressionIntConstant(Expression *expression, SymbolTable *symbolTable) {
    switch (expression->kind) {
        case opK: {
                switch (expression->val.op.operator->kind) {
                        //Can be both
                    case opEqualityK: {
                            int lhs = evaluateExpressionIntConstant(expression->val.op.left, symbolTable);
                            int rhs = evaluateExpressionIntConstant(expression->val.op.right, symbolTable);

                            if (rhs == lhs) {
                                return trueK;
                            } else {
                                return falseK;
                            }

                    } break;
                    case opInequalityK:{
                            int lhs = evaluateExpressionIntConstant(expression->val.op.left, symbolTable);
                            int rhs = evaluateExpressionIntConstant(expression->val.op.right, symbolTable);

                            if (rhs != lhs) {
                                return trueK;
                            } else {
                                return falseK;
                            }

                    } break;
                        //Int only
                    case opGreaterK:{
                        int lhs = evaluateExpressionIntConstant(expression->val.op.left, symbolTable);
                        int rhs = evaluateExpressionIntConstant(expression->val.op.right, symbolTable);

                        //Simply move them into rhs
                        rhs = lhs > rhs;

                        if (rhs == true) {
                            return trueK;
                        } else {
                            return falseK;
                        }

                    }break;
                    case opLessK:{
                        int lhs = evaluateExpressionIntConstant(expression->val.op.left, symbolTable);
                        int rhs = evaluateExpressionIntConstant(expression->val.op.right, symbolTable);

                        //Simply move them into rhs
                        rhs = lhs < rhs;

                        if (rhs == true) {
                            return trueK;
                        } else {
                            return falseK;
                        }

                    }break;
                    case opGeqK:{
                        int lhs = evaluateExpressionIntConstant(expression->val.op.left, symbolTable);
                        int rhs = evaluateExpressionIntConstant(expression->val.op.right, symbolTable);

                        //Simply move them into rhs
                        rhs = lhs >= rhs;

                        if (rhs == true) {
                            return trueK;
                        } else {
                            return falseK;
                        }

                    }break;
                    case opLeqK: {
                        int lhs = evaluateExpressionIntConstant(expression->val.op.left, symbolTable);
                        int rhs = evaluateExpressionIntConstant(expression->val.op.right, symbolTable);

                        //Simply move them into rhs
                        rhs = lhs <= rhs;

                        if (rhs == true) {
                            return trueK;
                        } else {
                            return falseK;
                        }

                    } break;
                    case opMultK: {
                        int lhs = evaluateExpressionIntConstant(expression->val.op.left, symbolTable);
                        int rhs = evaluateExpressionIntConstant(expression->val.op.right, symbolTable);

                        //Simply move them into rhs
                        rhs = lhs * rhs;

                        return rhs;

                    } break;
                    case opDivK:  {
                        int lhs = evaluateExpressionIntConstant(expression->val.op.left, symbolTable);
                        int rhs = evaluateExpressionIntConstant(expression->val.op.right, symbolTable);

                        //Simply move them into rhs
                        rhs = lhs / rhs;

                        return rhs;
                    } break;
                    case opPlusK: {
                        int lhs = evaluateExpressionIntConstant(expression->val.op.left, symbolTable);
                        int rhs = evaluateExpressionIntConstant(expression->val.op.right, symbolTable);

                        //Simply move them into rhs
                        rhs = lhs + rhs;

                        return rhs;
                    } break;
                    case opMinusK: {
                        int lhs = evaluateExpressionIntConstant(expression->val.op.left, symbolTable);
                        int rhs = evaluateExpressionIntConstant(expression->val.op.right, symbolTable);

                        //Simply move them into rhs
                        rhs = lhs - rhs;

                        return rhs;
                    } break;
            }

        }break;
        case termK:
            return evaluateTermIntConstant(expression->val.termD.term, symbolTable); break;
    }
}
int evaluateTermIntConstant(Term *term, SymbolTable *symbolTable) {
    switch (term->kind) {
        case parenthesesK: return evaluateExpressionIntConstant(term->val.parenthesesD.expression, symbolTable); break;
        case absK: return abs(evaluateExpressionIntConstant(term->val.absD.expression, symbolTable)); break;
        case numK: return term->val.numD.num; break;
    }

    return INT_MAX;
}

bool expressionEvaluatesToConstant(Expression *expression) {
    switch (expression->kind) {
        case opK: {
            if (expressionEvaluatesToConstant(expression->val.op.left) && expressionEvaluatesToConstant(expression->val.op.right)) {
                return true;
            }
        } break;
        case termK: {
            return termEvaluatesToConstant(expression->val.termD.term);
        } break;
    }

    return false;

}

bool termEvaluatesToConstant(Term *term) {
    switch (term->kind) {
        case parenthesesK: return expressionEvaluatesToConstant(term->val.parenthesesD.expression); break;
        case negateK: return termEvaluatesToConstant(term->val.negateD.term); break;
        case absK: return expressionEvaluatesToConstant(term->val.absD.expression); break;
        case numK: return true; break;
        case trueK: return true; break;
        case falseK: return true; break;
        default: break;
    }

    return false;
}

void constantFoldTerm(Term *term, SymbolTable *symbolTable) {
    switch (term->kind) {
        case functionCallK: {
            char *baseName = term->val.functionCallD.functionId;
            int extra = 16;
            char *buf = (char*)malloc(sizeof(char) * (strlen(baseName) + extra));

            SYMBOL *symbol = getSymbol(symbolTable, term->val.functionCallD.functionId);

            sprintf(buf, "%s__%i", baseName, (int)(symbol->distanceFromRoot));

            Pair *constantEvaluatedFunction = get(constantFunctionEvalutationMap, makeCharKey(buf));

            if (constantEvaluatedFunction != NULL) {
                Type *unwrappedReturn = unwrapTypedef(symbol->value->val.typeFunctionD.returnType, symbolTable, NULL);
                if (unwrappedReturn->kind == typeIntK) {
                    term->kind = numK;
                    term->val.numD.num = *((int*)constantEvaluatedFunction->v);
                } else {
                    int asBool = *((int*)constantEvaluatedFunction->v);
                    if (asBool) {
                        term->kind = trueK;
                    } else {
                        term->kind = falseK;
                    }
                }
            } else {
                ExpressionList *iter = term->val.functionCallD.expressionList;

                while (iter != NULL) {
                    constantFoldExpression(iter->expression, symbolTable);
                    iter = iter->next;
                }
            }
        }break;
        case parenthesesK: {
            constantFoldExpression(term->val.parenthesesD.expression, symbolTable);
        }break;
        case negateK: {
            constantFoldTerm(term->val.negateD.term, symbolTable);
        }break;
        case absK: {
            constantFoldExpression(term->val.absD.expression, symbolTable);
        }break;
        case shorthandCallK: {
            ExpressionList *iter = term->val.shorthandCallD.expressionList;

            while (iter != NULL) {
                constantFoldExpression(iter->expression, symbolTable);
                iter = iter->next;
            }
        }break;
        case lambdaK: {
            constantFoldBody(term->val.lambdaD.lambda->body);
        } break;
        default: break;
    }
}
bool doesVariableAffectOutside(Variable *variable, SymbolTable *symbolTable, int evaluationLevel, bool isLhs);
bool doesExpressionAffectOutside(Expression *expression, SymbolTable *symbolTable, int evaluationLevel);

bool doesTermAffectOutside(Term *term, SymbolTable *symbolTable, int evaluationLevel) {
    switch (term->kind) {
        case variableK: return doesVariableAffectOutside(term->val.variableD.var, symbolTable, evaluationLevel, false); break;
        case functionCallK: {
            SYMBOL *symbol = getSymbol(symbolTable, term->val.functionCallD.functionId);
            if (symbol->distanceFromRoot > evaluationLevel) {
                return true;
            }
            //If not the decl is inside and the recursive decl checker will get it
        } break;
        case parenthesesK: {
            return doesExpressionAffectOutside(term->val.parenthesesD.expression, symbolTable, evaluationLevel);
        } break;
        case negateK: {
            return doesTermAffectOutside(term->val.negateD.term, symbolTable, evaluationLevel);
        } break;
        case absK: {
            return doesExpressionAffectOutside(term->val.absD.expression, symbolTable, evaluationLevel);
        } break;
        case lambdaK: {
            return doesBodyAffectOutside(term->val.lambdaD.lambda->body, evaluationLevel);
        }break;
        case shorthandCallK: {
            return true;
        } break;
    }

    return false;
}

bool doesExpressionAffectOutside(Expression *expression, SymbolTable *symbolTable, int evaluationLevel) {
    switch (expression->kind) {
        case opK: {
            bool r1 = doesExpressionAffectOutside(expression->val.op.left, symbolTable, evaluationLevel);
            bool r2= doesExpressionAffectOutside(expression->val.op.right, symbolTable, evaluationLevel);

            if (r1 == true) return true;
            if (r2 == true) return true;
        } break;
        case termK: {
            return doesTermAffectOutside(expression->val.termD.term, symbolTable, evaluationLevel);
        } break;
    }

    return false;
}

bool doesVariableAffectOutside(Variable *variable, SymbolTable *symbolTable, int evaluationLevel, bool isLhs) {
    switch (variable->kind) {
        case varIdK: {
            SYMBOL *sym = getSymbol(symbolTable, variable->val.idD.id);
            if ((sym->distanceFromRoot > evaluationLevel && isLhs) || sym->isArgument) {
                return true;
            }
        } break;
        case arrayIndexK: {
            return doesVariableAffectOutside(variable->val.arrayIndexD.var, symbolTable, evaluationLevel, isLhs);
        } break;
        case recordLookupK: {
            return doesVariableAffectOutside(variable->val.recordLookupD.var, symbolTable, evaluationLevel, isLhs);
        } break;
    }

    return false;
}

bool doesStatementAffectOutside(Statement *statement, int evaluationLevel) {
    switch (statement->kind) {
        case statReturnK: {
            return doesExpressionAffectOutside(statement->val.returnD.exp, statement->symbolTable, evaluationLevel);
        } break;
        case statWriteK: {
            return true;
        } break;
        case statAllocateK: {
            return doesVariableAffectOutside(statement->val.allocateD.var, statement->symbolTable, evaluationLevel, true);
        } break;
        case statAllocateLenK: {
            return doesVariableAffectOutside(statement->val.allocateLenD.var, statement->symbolTable, evaluationLevel, true);
        } break;
        case statIfK: {
            return doesStatementAffectOutside(statement->val.ifD.statement, evaluationLevel);
        } break;
        case statIfElK: {
            bool r1 = doesStatementAffectOutside(statement->val.ifElD.statement, evaluationLevel);
            bool r2 = doesStatementAffectOutside(statement->val.ifElD.elseStatement, evaluationLevel);

            if (r1 == true) return true;
            if (r2 == true) return true;
        } break;
        case statWhileK: {
            return doesStatementAffectOutside(statement->val.whileD.statement, evaluationLevel);
        } break;
        case stmListK: {
            StatementList *iter = statement->val.stmListD.statementList;

            while (iter != NULL) {
                bool r = doesStatementAffectOutside(iter->statement, evaluationLevel);
                if (r == true) return true;
                iter = iter->next;
            }
        } break;
        case assignmentK: {
            return doesVariableAffectOutside(statement->val.assignmentD.var, statement->symbolTable, evaluationLevel, true);
        } break;
        case emptyK: {
            return doesExpressionAffectOutside(statement->val.empty.exp, statement->symbolTable, evaluationLevel);
        } break;
    }

    return false;
}

bool doesDeclarationAffectOutside(Declaration *declaration, int evaluationLevel) {
    switch (declaration->kind) {
        case declFuncK: {
            return doesBodyAffectOutside(declaration->val.functionD.function->body, evaluationLevel);
        } break;
        case declValK: {
            return doesExpressionAffectOutside(declaration->val.valD.rhs, declaration->symbolTable, evaluationLevel);
        } break;
        case declClassK: {
            return true;
        } break;
        default: break;
    }

    return false;
}

bool doesBodyAffectOutside(Body *body, int evaluationLevel) {
    DeclarationList *declIter = body->declarationList;
    while (declIter != NULL) {
        bool r = doesDeclarationAffectOutside(declIter->declaration, evaluationLevel);
        if (r == true) return true;
        declIter = declIter->next;
    }

    StatementList *stmIter = body->statementList;
    while (stmIter != NULL) {
        bool r = doesStatementAffectOutside(stmIter->statement, evaluationLevel);
        if (r == true) return true;
        stmIter = stmIter->next;
    }

}

void constantFoldExpression(Expression *expression, SymbolTable *symbolTable) {
    switch (expression->kind) {
        case opK: {
            if (expressionEvaluatesToConstant(expression->val.op.left) && expressionEvaluatesToConstant(expression->val.op.right)) {

                //Type checker has already confirmed valid type operation
                Type *evalType = evaluateExpressionType(expression->val.op.left, symbolTable);

                switch (expression->val.op.operator->kind) {
                    //Boolean
                    case opAndK: {
                        TermKind lhs = evaluateExpressionBoolConstant(expression->val.op.left, symbolTable);
                        TermKind rhs = evaluateExpressionBoolConstant(expression->val.op.right, symbolTable);

                        expression->kind = termK;

                        //Simply move them into rhs
                        if (rhs == trueK && lhs == trueK) {
                            expression->val.termD.term->kind = trueK;
                        } else {
                            expression->val.termD.term->kind = falseK;
                        }

                        expression->kind = termK;
                    } break;
                    case opOrK:{
                        TermKind lhs = evaluateExpressionBoolConstant(expression->val.op.left, symbolTable);
                        TermKind rhs = evaluateExpressionBoolConstant(expression->val.op.right, symbolTable);

                        expression->kind = termK;

                        //Simply move them into rhs
                        if (rhs == trueK || lhs == trueK) {
                            expression->val.termD.term->kind = trueK;
                        } else {
                            expression->val.termD.term->kind = falseK;
                        }

                        expression->kind = termK;
                    }break;
                    //Can be both
                    case opEqualityK: {
                        if (evalType->kind == typeIntK) {
                            int lhs = evaluateExpressionIntConstant(expression->val.op.left, symbolTable);
                            int rhs = evaluateExpressionIntConstant(expression->val.op.right, symbolTable);

                            if (rhs == lhs) {
                                expression->val.termD.term->kind = trueK;
                            } else {
                                expression->val.termD.term->kind = falseK;
                            }

                            expression->kind = termK;
                        } else {
                            TermKind lhs = evaluateExpressionBoolConstant(expression->val.op.left, symbolTable);
                            TermKind rhs = evaluateExpressionBoolConstant(expression->val.op.right, symbolTable);

                            expression->kind = termK;

                            //Simply move them into rhs
                            if (rhs == lhs) {
                                expression->val.termD.term->kind = trueK;
                            } else {
                                expression->val.termD.term->kind = falseK;
                            }

                            expression->kind = termK;
                        }
                    } break;
                    case opInequalityK:{
                        if (evalType->kind == typeIntK) {
                            int lhs = evaluateExpressionIntConstant(expression->val.op.left, symbolTable);
                            int rhs = evaluateExpressionIntConstant(expression->val.op.right, symbolTable);

                            if (rhs != lhs) {
                                expression->val.termD.term->kind = trueK;
                            } else {
                                expression->val.termD.term->kind = falseK;
                            }

                            expression->kind = termK;
                        } else {
                            TermKind lhs = evaluateExpressionBoolConstant(expression->val.op.left, symbolTable);
                            TermKind rhs = evaluateExpressionBoolConstant(expression->val.op.right, symbolTable);

                            expression->kind = termK;

                            //Simply move them into rhs
                            if (rhs != lhs) {
                                expression->val.termD.term->kind = trueK;
                            } else {
                                expression->val.termD.term->kind = falseK;
                            }

                            expression->kind = termK;
                        }
                    } break;
                    //Int only
                    case opGreaterK:{
                        int lhs = evaluateExpressionIntConstant(expression->val.op.left, symbolTable);
                        int rhs = evaluateExpressionIntConstant(expression->val.op.right, symbolTable);

                        //Simply move them into rhs
                        rhs = lhs > rhs;

                        if (rhs == true) {
                            expression->val.termD.term->kind = trueK;
                        } else {
                            expression->val.termD.term->kind = falseK;
                        }

                        expression->kind = termK;
                    }break;
                    case opLessK:{
                        int lhs = evaluateExpressionIntConstant(expression->val.op.left, symbolTable);
                        int rhs = evaluateExpressionIntConstant(expression->val.op.right, symbolTable);

                        //Simply move them into rhs
                        rhs = lhs < rhs;

                        if (rhs == true) {
                            expression->val.termD.term->kind = trueK;
                        } else {
                            expression->val.termD.term->kind = falseK;
                        }

                        expression->kind = termK;
                    }break;
                    case opGeqK:{
                        int lhs = evaluateExpressionIntConstant(expression->val.op.left, symbolTable);
                        int rhs = evaluateExpressionIntConstant(expression->val.op.right, symbolTable);

                        //Simply move them into rhs
                        rhs = lhs >= rhs;

                        if (rhs == true) {
                            expression->val.termD.term->kind = trueK;
                        } else {
                            expression->val.termD.term->kind = falseK;
                        }

                        expression->kind = termK;
                    }break;
                    case opLeqK: {
                        int lhs = evaluateExpressionIntConstant(expression->val.op.left, symbolTable);
                        int rhs = evaluateExpressionIntConstant(expression->val.op.right, symbolTable);

                        //Simply move them into rhs
                        rhs = lhs <= rhs;

                        if (rhs == true) {
                            expression->val.termD.term->kind = trueK;
                        } else {
                            expression->val.termD.term->kind = falseK;
                        }

                        expression->kind = termK;
                    } break;
                    case opMultK: {
                        int lhs = evaluateExpressionIntConstant(expression->val.op.left, symbolTable);
                        int rhs = evaluateExpressionIntConstant(expression->val.op.right, symbolTable);

                        //Simply move them into rhs
                        rhs = lhs * rhs;

                        expression->kind = termK;
                        expression->val.termD.term->kind = numK;
                        expression->val.termD.term->val.numD.num = rhs;
                    } break;
                    case opDivK:  {
                        int lhs = evaluateExpressionIntConstant(expression->val.op.left, symbolTable);
                        int rhs = evaluateExpressionIntConstant(expression->val.op.right, symbolTable);

                        //Simply move them into rhs
                        rhs = lhs / rhs;

                        expression->kind = termK;
                        expression->val.termD.term->kind = numK;
                        expression->val.termD.term->val.numD.num = rhs;
                    } break;
                    case opPlusK: {
                        int lhs = evaluateExpressionIntConstant(expression->val.op.left, symbolTable);
                        int rhs = evaluateExpressionIntConstant(expression->val.op.right, symbolTable);

                        //Simply move them into rhs
                        rhs = lhs + rhs;

                        expression->kind = termK;
                        expression->val.termD.term->kind = numK;
                        expression->val.termD.term->val.numD.num = rhs;
                    } break;
                    case opMinusK: {
                        int lhs = evaluateExpressionIntConstant(expression->val.op.left, symbolTable);
                        int rhs = evaluateExpressionIntConstant(expression->val.op.right, symbolTable);

                        //Simply move them into rhs
                        rhs = lhs - rhs;

                        expression->kind = termK;
                        expression->val.termD.term->kind = numK;
                        expression->val.termD.term->val.numD.num = rhs;
                    } break;
                }
            } else {
                constantFoldExpression(expression->val.op.left, symbolTable);
                constantFoldExpression(expression->val.op.right, symbolTable);
            }

        }break;
        case termK:
            constantFoldTerm(expression->val.termD.term, symbolTable); break;
    }
}

void constantFoldStatement(Statement *statement) {
    switch (statement->kind) {
        case statReturnK:
            constantFoldExpression(statement->val.returnD.exp, statement->symbolTable); break;
        case statWriteK:
            constantFoldExpression(statement->val.writeD.exp, statement->symbolTable);  break;
        case statAllocateK:break;
        case statAllocateLenK:
            constantFoldExpression(statement->val.allocateLenD.len, statement->symbolTable); break;
        case statIfK: {
            constantFoldExpression(statement->val.ifD.exp, statement->symbolTable);
            //If condition has been constant folded to true/false we can just execute the statements or not
            if (expressionEvaluatesToConstant(statement->val.ifD.exp)) {
                TermKind expConst = evaluateExpressionBoolConstant(statement->val.ifElD.exp, statement->symbolTable);
                if (expConst == trueK) {
                    statement->kind = statement->val.ifD.statement->kind;
                    statement->symbolTable = statement->val.ifD.statement->symbolTable;
                    statement->val = statement->val.ifD.statement->val;
                } else if (statement->val.ifD.exp->val.termD.term->kind == falseK) {
                    statement->kind = noop;
                }
            }
        } break;
        case statIfElK: {
            constantFoldExpression(statement->val.ifElD.exp, statement->symbolTable);
            //If condition has been constant folded to true/false we can just execute the statements or not
            if (expressionEvaluatesToConstant(statement->val.ifElD.exp)) {
                TermKind expConst = evaluateExpressionBoolConstant(statement->val.ifElD.exp, statement->symbolTable);

                if (expConst == trueK) {
                    statement->kind =           statement->val.ifElD.statement->kind;
                    statement->symbolTable =    statement->val.ifElD.statement->symbolTable;
                    statement->val =            statement->val.ifElD.statement->val;
                } else if (expConst == falseK) {
                    statement->kind =           statement->val.ifElD.elseStatement->kind;
                    statement->symbolTable =    statement->val.ifElD.elseStatement->symbolTable;
                    statement->val =            statement->val.ifElD.elseStatement->val;
                }
            }
        } break;
        case statWhileK: {
            constantFoldExpression(statement->val.whileD.exp, statement->symbolTable);

            //If eval to false no need
            if (expressionEvaluatesToConstant(statement->val.ifElD.exp)) {
                TermKind expConst = evaluateExpressionBoolConstant(statement->val.ifElD.exp, statement->symbolTable);
                if (expConst == falseK) {
                    statement->kind = noop;
                }
            }
        }break;
        case stmListK: {
            StatementList *iter = statement->val.stmListD.statementList;

            while (iter != NULL) {
                constantFoldStatement(iter->statement);
                iter = iter->next;
            }
        } break;
        case assignmentK:
            constantFoldExpression(statement->val.assignmentD.exp, statement->symbolTable); break;
        case emptyK:break;
        case gcK:break;
    }

}

void constantFoldDeclaration(Declaration *declaration) {
    switch (declaration->kind) {
        case declVarK:break;
        case declVarsK:break;
        case declTypeK:break;
        case declFuncK: {
            constantFoldBody(declaration->val.functionD.function->body);

            Type *unwrappedReturn = unwrapTypedef(declaration->val.functionD.function->head->returnType, declaration->symbolTable, NULL);

            if (unwrappedReturn->kind == typeIntK || unwrappedReturn->kind == typeBoolK) {
                if (doesBodyAffectOutside(declaration->val.functionD.function->body, declaration->symbolTable->distanceFromRoot + 1) == false) {
                    //Constant evaluate function
                    Body *dummyBody = NEW(Body);
                    DeclarationList *dummyDeclList = NEW(DeclarationList);
                    dummyDeclList->next = NULL;
                    dummyDeclList->declaration = declaration;
                    Term *writeTerm = NEW(Term);
                    writeTerm->kind = functionCallK;
                    writeTerm->val.functionCallD.functionId = declaration->val.functionD.function->head->indentifier;
                    writeTerm->val.functionCallD.expressionList = NULL;
                    Expression *writeExpr = NEW(Expression);
                    writeExpr->kind = termK;
                    writeExpr->val.termD.term = writeTerm;
                    Statement *dummyWrite = NEW(Statement);
                    dummyWrite->kind = statWriteK;
                    dummyWrite->val.writeD.exp = writeExpr;
                    dummyWrite->symbolTable = declaration->symbolTable;
                    StatementList *dummyStatementList = NEW(StatementList);
                    dummyStatementList->statement = dummyWrite;

                    dummyBody->declarationList = dummyDeclList;
                    dummyBody->statementList = dummyStatementList;

                    int r = sandboxBody(dummyBody);
                    int *mr = malloc(sizeof(int));
                    *mr = r;

                    char *baseName = declaration->val.functionD.function->head->indentifier;
                    int extra = 16;
                    char *buf = (char*)malloc(sizeof(char) * (strlen(baseName) + extra));

                    sprintf(buf, "%s__%i", baseName, (int)(getSymbol(declaration->symbolTable, declaration->val.functionD.function->head->indentifier)->distanceFromRoot));

                    insert(constantFunctionEvalutationMap, makeCharKey(buf), mr);

                    declaration->kind = nodecl;
                }
            }
        } break;
        case declValK: {
            constantFoldExpression(declaration->val.valD.rhs, declaration->symbolTable);
        } break;
        case declClassK: {
            DeclarationList *iter = declaration->val.classD.declarationList;

            while (iter != NULL) {
                constantFoldDeclaration(iter->declaration);
                iter = iter->next;
            }

            constantFoldBody(declaration->val.classD.constructor->body);
        } break;
        case nodecl: break;
    }

}

void constantFoldBody(Body *body) {
    if (constantFunctionEvalutationMap == NULL) {
        constantFunctionEvalutationMap = initMap(50);
    }

    DeclarationList *declIter = body->declarationList;
    while (declIter != NULL) {
        constantFoldDeclaration(declIter->declaration);
        declIter = declIter->next;
    }

    StatementList *stmIter = body->statementList;
    while (stmIter != NULL) {
        constantFoldStatement(stmIter->statement);
        stmIter = stmIter->next;
    }
}