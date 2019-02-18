#include <stddef.h>
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

Body *makeBody(DeclarationList *declarationList) {
    Body *body = NEW(Body);
    body->declarationList = declarationList;

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
