//
// Created by valde on 5/12/19.
//

#include "constant_fold.h"
#include "../ast/tree.h"

void constantFoldExpression(Expression *expression, SymbolTable *symbolTable);
bool termEvaluatesToConstant(Term *term);
bool expressionEvaluatesToConstant(Expression *expression);
void constantFoldBody(Body *body);
int evaluateExpressionIntConstant(Expression *expression, SymbolTable *symbolTable) ;
int evaluateTermIntConstant(Term *expression, SymbolTable *symbolTable);
TermKind evaluateExpressionBoolConstant(Expression *expression, SymbolTable *symbolTable) ;
TermKind evaluateTermBoolConstant(Term *expression, SymbolTable *symbolTable);

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
            ExpressionList *iter = term->val.functionCallD.expressionList;

            while (iter != NULL) {
                constantFoldExpression(iter->expression, symbolTable);
                iter = iter->next;
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
        } break;
    }

}

void constantFoldBody(Body *body) {
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