#include <stddef.h>
#include <stdio.h>
#include "../utils/memory.h"
#include "tree.h"

extern int lineno;

EXP *makeEXPid(char *id) {
    EXP *e;
    e = NEW(EXP);
    e->lineno = lineno;
    e->kind = idK;
    e->val.idE = id;
    return e;
}

EXP *makeEXPintconst(int intconst) {
    EXP *e;
    e = NEW(EXP);
    e->lineno = lineno;
    e->kind = intconstK;
    e->val.intconstE = intconst;
    return e;
}
/*
EXP *makeEXPtimes(EXP *left, EXP *right) {
    EXP *e;
    e = NEW(EXP);
    e->lineno = lineno;
    e->kind = timesK;
    e->val.timesE.left = left;
    e->val.timesE.right = right;
    return e;
}

EXP *makeEXPdiv(EXP *left, EXP *right) {
    EXP *e;
    e = NEW(EXP);
    e->lineno = lineno;
    e->kind = divK;
    e->val.divE.left = left;
    e->val.divE.right = right;
    return e;
}

EXP *makeEXPplus(EXP *left, EXP *right) {
    EXP *e;
    e = NEW(EXP);
    e->lineno = lineno;
    e->kind = plusK;
    e->val.plusE.left = left;
    e->val.plusE.right = right;
    return e;
}

EXP *makeEXPminus(EXP *left, EXP *right) {
    EXP *e;
    e = NEW(EXP);
    e->lineno = lineno;
    e->kind = minusK;
    e->val.minusE.left = left;
    e->val.minusE.right = right;
    return e;
}
*/

EXP *makeEXPOpEXP(EXP *lhs, Operator *op, EXP *rhs) {
    EXP *e = NEW(EXP);

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
    op->kind = multK;

    return op;
}

Operator *makeDivOp() {

}

Operator *makePlusOp() {

}

Operator *makeMinusOp() {

}

Operator *makeEqualityOp() {

}

Operator *makeInequalityOp() {

}

Operator *makeGreaterOp() {

}

Operator *makeLessOp() {

}

Operator *makeGeqOp() {

}

Operator *makeLeqOp() {

}

Operator *makeAndOp() {

}

Operator *makeOrOp() {

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
    result->kind = varK;
    result->val.varD.id = id;
    result->val.varD.type = type;
    return result;
}

StatementList *makeStatementList(Statement *statement, Statement *next) {
    StatementList *stmList = NEW(StatementList);
    stmList->statement = statement;
    stmList->next = next;

    return stmList;
}

Statement *makeReturnStatement(EXP *exp) {
    Statement *statement = NEW(Statement);

    statement->lineno = lineno;
    statement->kind = returnK;
    statement->val.returnD.exp = exp;
    return statement;
}

Statement *makeIfStatement(EXP *exp, Statement *statement) {
    Statement *returning = NEW(Statement);

    returning->lineno = lineno;
    returning->kind = ifK;
    returning->val.ifElD.exp = exp;
    returning->val.ifElD.statement = statement;
    return returning;
}

Statement *makeIfElseStatement(EXP *exp, Statement *statement, Statement *elseStatement) {
    Statement *returning = NEW(Statement);

    returning->lineno = lineno;
    returning->kind = ifElK;
    returning->val.ifElD.exp = exp;
    returning->val.ifElD.statement = statement;
    returning->val.ifElD.elseStatement = elseStatement;
    return returning;
}

Statement *makeAllocateStatement(EXP *exp) {
    Statement *statement = NEW(Statement);

    statement->lineno = lineno;
    statement->kind = allocateK;
    statement->val.allocateD.exp = exp;
    return statement;
}

Statement *makeAllocateOfLenStatement(EXP *exp, EXP *len) {
    Statement *statement = NEW(Statement);

    statement->lineno = lineno;
    statement->kind = allocateLenK;
    statement->val.allocateLenD.exp = exp;
    statement->val.allocateLenD.len = len;
    return statement;
}

Statement *makeWriteStatement(EXP *exp) {
    Statement *statement = NEW(Statement);

    statement->lineno = lineno;
    statement->kind = writeK;
    statement->val.writeD.exp = exp;
    return statement;
}

Statement *makeWhileStatement(EXP *exp, Body *localBody) {
    Statement *statement = NEW(Statement);

    statement->lineno = lineno;
    statement->kind = whileK;
    statement->val.whileD.exp = exp;
    statement->val.whileD.localBody = localBody;
    return statement;
}

Statement *makeWhileSingleStatement(EXP *exp, Statement *statement) {
    Statement *returning = NEW(Statement);

    returning->lineno = lineno;
    returning->kind = whileSSK;
    returning->val.whileSSD.exp = exp;
    returning->val.whileSSD.statement = statement;
    return returning;
}

Declaration *makeVarsDeclaration(char *id, Type *type, Declaration *next) {
    Declaration *result;
    result = NEW(Declaration);
    result->lineno = lineno;
    result->kind = varsK;
    result->val.varsD.var = makeVarDeclaration(id, type);
    result->val.varsD.next = next;
    return result;
}

Type *makeIntType() {
    Type *result = NEW(Type);
    result->kind = intT;
    return result;
}

Type *makeBoolType() {
    Type *result = NEW(Type);
    result->kind = boolT;
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
    declaration->kind = functionK;
    declaration->val.functionD.function = function;

    return declaration;
}

Declaration *makeTypeDeclaration(char *id, Type *type) {
    Declaration *result;
    result = NEW(Declaration);
    result->lineno = lineno;
    result->kind = typeK;
    result->val.typeD.id = id;
    result->val.typeD.type = type;
    return result;
}

Type *makeIdType(char *id) {
    Type *type;
    type = NEW(Type);

    type->kind = idT;
    type->val.idType.id = id;

    return type;
}

Type *makeArrayType(Type *type) {
    Type *t;
    t = NEW(Type);

    t->kind = arrayT;
    t->val.arrayType.type = type;

    return t;
}

Type *makeRecordType(VarDelList *record) {
    Type *t;
    t = NEW(Type);

    t->kind = recordT;
    t->val.recordType.types = record;

    return t;
}

