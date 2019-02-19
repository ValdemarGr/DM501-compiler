#include <stddef.h>
#include <stdio.h>
#include "../utils/memory.h"
#include "tree.h"

extern int lineno;

Expression *makeEXPid(char *id) {
    Expression *e;
    e = NEW(Expression);
    e->lineno = lineno;
    e->kind = idK;
    e->val.idE = id;
    return e;
}

Expression *makeEXPintconst(int intconst) {
    Expression *e;
    e = NEW(Expression);
    e->lineno = lineno;
    e->kind = intconstK;
    e->val.intconstE = intconst;
    return e;
}
/*
EXP *makeEXPtimes(EXP *left, EXP *right) {
    EXP *e;
    e = NEW(EXP);
=======

Expression *makeEXPtimes(Expression *left, Expression *right) {
    Expression *e;
    e = NEW(Expression);
>>>>>>> f5afe80b98e9f432e79a4d82d73ab1a47128ab01
    e->lineno = lineno;
    e->kind = expMultK;
    e->val.timesE.left = left;
    e->val.timesE.right = right;
    return e;
}

Expression *makeEXPdiv(Expression *left, Expression *right) {
    Expression *e;
    e = NEW(Expression);
    e->lineno = lineno;
    e->kind = expDivK;
    e->val.divE.left = left;
    e->val.divE.right = right;
    return e;
}

Expression *makeEXPplus(Expression *left, Expression *right) {
    Expression *e;
    e = NEW(Expression);
    e->lineno = lineno;
    e->kind = expPlusK;
    e->val.plusE.left = left;
    e->val.plusE.right = right;
    return e;
}

Expression *makeEXPminus(Expression *left, Expression *right) {
    Expression *e;
    e = NEW(Expression);
    e->lineno = lineno;
    e->kind = expMinusK;
    e->val.minusE.left = left;
    e->val.minusE.right = right;
    return e;
}
*/

Expression *makeEXPOpEXP(Expression *lhs, Operator *op, Expression *rhs) {
    Expression *e = NEW(Expression);

    e->lineno = lhs->lineno;
    e->kind = opK;
    e->val.op.left = lhs;
    e->val.op.operator = op;
    e->val.op.right = rhs;

    return e;
}

//OPERATORS START
Operator *makeMultOp() {
    Operator *op = NEW(Operator);

    op->lineno = lineno;
    op->kind = opMultK;

    return op;
}

Operator *makeDivOp() {
    Operator *op = NEW(Operator);

    op->lineno = lineno;
    op->kind = opDivK;

    return op;

}

Operator *makePlusOp() {
    Operator *op = NEW(Operator);

    op->lineno = lineno;
    op->kind = opPlusK;

    return op;
}

Operator *makeMinusOp() {
    Operator *op = NEW(Operator);

    op->lineno = lineno;
    op->kind = opMinusK;

    return op;
}

Operator *makeEqualityOp() {
    Operator *op = NEW(Operator);

    op->lineno = lineno;
    op->kind = opEqualityK;

    return op;
}

Operator *makeInequalityOp() {
    Operator *op = NEW(Operator);

    op->lineno = lineno;
    op->kind = opInequalityK;

    return op;
}

Operator *makeGreaterOp() {
    Operator *op = NEW(Operator);

    op->lineno = lineno;
    op->kind = opGreaterK;

    return op;
}

Operator *makeLessOp() {
    Operator *op = NEW(Operator);

    op->lineno = lineno;
    op->kind = opLessK;

    return op;
}

Operator *makeGeqOp() {
    Operator *op = NEW(Operator);

    op->lineno = lineno;
    op->kind = opGeqK;

    return op;
}

Operator *makeLeqOp() {
    Operator *op = NEW(Operator);

    op->lineno = lineno;
    op->kind = opLeqK;

    return op;
}

Operator *makeAndOp() {
    Operator *op = NEW(Operator);

    op->lineno = lineno;
    op->kind = opAndK;

    return op;
}

Operator *makeOrOp() {
    Operator *op = NEW(Operator);

    op->lineno = lineno;
    op->kind = opOrK;

    return op;
}

//OPERATORS END

FunctionHead *makeFunctionHead(char *identifier, VarDelList *declerationList, Type *type) {
    FunctionHead *head;
    head = NEW(FunctionHead);
    head->indentifier = identifier;
    head->declarationList = declerationList;
    head->returnType = type;
    return head;
}

FunctionTail *makeFunctionTail(char *identifier) {
    FunctionTail *tail;
    tail = NEW(FunctionTail);
    tail->indentifier = identifier;
    return tail;
}

Function *makeFunction(FunctionHead *head, Body *body, FunctionTail *tail) {
    Function *function = NEW(Function);

    function->head = head;
    function->body = body;
    function->tail = tail;

    return function;
}

VarDelList *makeVarDelList(char *identifier, Type *type, VarDelList *next) {
    VarDelList *list;
    list = NEW(VarDelList);
    list->identifier = identifier;
    list->type = type;
    list->next = next;
    return list;
}

DeclarationList *makeDeclarationList(Declaration *declaration, DeclarationList *next) {
    DeclarationList *result;
    result = NEW(DeclarationList);
    result->declaration = declaration;
    result->next = next;
    return result;
}

Declaration *makeVarDeclaration(char *id, Type *type) {
    Declaration *result;
    result = NEW(Declaration);
    result->lineno = lineno;
    result->kind = declVarK;
    result->val.varD.id = id;
    result->val.varD.type = type;
    return result;
}

StatementList *makeStatementList(Statement *statement, StatementList *next) {
    StatementList *stmList = NEW(StatementList);
    stmList->statement = statement;
    stmList->next = next;

    return stmList;
}

Statement *makeReturnStatement(Expression *exp) {
    Statement *statement = NEW(Statement);

    statement->lineno = lineno;
    statement->kind = statReturnK;
    statement->val.returnD.exp = exp;
    return statement;
}

Statement *makeIfStatement(Expression *exp, Statement *statement) {
    Statement *returning = NEW(Statement);

    returning->lineno = lineno;
    returning->kind = statIfK;
    returning->val.ifElD.exp = exp;
    returning->val.ifElD.statement = statement;
    return returning;
}

Statement *makeIfElseStatement(Expression *exp, Statement *statement, Statement *elseStatement) {
    Statement *returning = NEW(Statement);

    returning->lineno = lineno;
    returning->kind = statIfElK;
    returning->val.ifElD.exp = exp;
    returning->val.ifElD.statement = statement;
    returning->val.ifElD.elseStatement = elseStatement;
    return returning;
}

Statement *makeAllocateStatement(Expression *exp) {
    Statement *statement = NEW(Statement);

    statement->lineno = lineno;
    statement->kind = statAllocateK;
    statement->val.allocateD.exp = exp;
    return statement;
}

Statement *makeAllocateOfLenStatement(Expression *exp, Expression *len) {
    Statement *statement = NEW(Statement);

    statement->lineno = lineno;
    statement->kind = statAllocateLenK;
    statement->val.allocateLenD.exp = exp;
    statement->val.allocateLenD.len = len;
    return statement;
}

Statement *makeWriteStatement(Expression *exp) {
    Statement *statement = NEW(Statement);

    statement->lineno = lineno;
    statement->kind = statWriteK;
    statement->val.writeD.exp = exp;
    return statement;
}

Statement *makeWhileStatement(Expression *exp, Body *localBody) {
    Statement *statement = NEW(Statement);

    statement->lineno = lineno;
    statement->kind = statWhileK;
    statement->val.whileD.exp = exp;
    statement->val.whileD.localBody = localBody;
    return statement;
}

Statement *makeWhileSingleStatement(Expression *exp, Statement *statement) {
    Statement *returning = NEW(Statement);

    returning->lineno = lineno;
    returning->kind = statWhileSSK;
    returning->val.whileSSD.exp = exp;
    returning->val.whileSSD.statement = statement;
    return returning;
}

Declaration *makeVarsDeclaration(char *id, Type *type, Declaration *next) {
    Declaration *result;
    result = NEW(Declaration);
    result->lineno = lineno;
    result->kind = declVarsK;
    result->val.varsD.var = makeVarDeclaration(id, type);
    result->val.varsD.next = next;
    return result;
}

Type *makeIntType() {
    Type *result = NEW(Type);
    result->kind = typeIntK;
    return result;
}

Type *makeBoolType() {
    Type *result = NEW(Type);
    result->kind = typeBoolK;
    return result;
}

Declaration *makeVarDeclarations(VarDelList *vars) {
    if (vars == NULL) {
        return NULL;
    }

    Declaration *result;
    result = makeVarsDeclaration(vars->identifier, vars->type,
                                 makeVarDeclarations(vars->next));
    return result;
}

Body *makeBody(DeclarationList *declarationList, StatementList *statementList) {
    Body *body = NEW(Body);
    body->declarationList = declarationList;
    body->statementList = statementList;

    return body;
}

Declaration *makeFunctionDecleration(Function *function) {
    Declaration *declaration = NEW(Declaration);

    declaration->lineno = lineno;
    declaration->kind = declFuncK;
    declaration->val.functionD.function = function;

    return declaration;
}

Declaration *makeTypeDeclaration(char *id, Type *type) {
    Declaration *result;
    result = NEW(Declaration);
    result->lineno = lineno;
    result->kind = declTypeK;
    result->val.typeD.id = id;
    result->val.typeD.type = type;
    return result;
}

Type *makeIdType(char *id) {
    Type *type;
    type = NEW(Type);

    type->kind = typeIdK;
    type->val.idType.id = id;

    return type;
}

Type *makeArrayType(Type *type) {
    Type *t;
    t = NEW(Type);

    t->kind = typeArrayK;
    t->val.arrayType.type = type;

    return t;
}

Type *makeRecordType(VarDelList *record) {
    Type *t;
    t = NEW(Type);

    t->kind = typeRecordK;
    t->val.recordType.types = record;

    return t;
}

