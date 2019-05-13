#include <stddef.h>
#include <stdio.h>
#include "../utils/memory.h"
#include "tree.h"

extern int lineno;
extern int stmDeclNum;
int typeIndex = 0;
int lambdaId = 0;

TypeList *makeTypeList(TypeList* next, Type *elem, Location location) {
    TypeList *tpeLst = NEW(TypeList);

    tpeLst->next = next;
    tpeLst->type = elem;
    tpeLst->location = location;

    return tpeLst;
}

TypeList *makeGenericTypeList(TypeList* next, char* id, char* subType, Location location){
    TypeList *tpeLst = NEW(TypeList);

    tpeLst->next = next;
    tpeLst->location = location;

    Type *tpe = NEW(Type);

    tpe->kind = typeGenericK;
    tpe->val.typeGeneric.genericName = id;
    tpe->val.typeGeneric.subType = subType;
    tpe->val.typeGeneric.typeIndex = typeIndex;
    typeIndex++;

    tpeLst->type = tpe;

    return tpeLst;
}

Expression *makeEXPFromTerm(Term *term, Location location) {
    Expression *returning = NEW(Expression);

    returning->lineno = lineno;
    returning->kind = termK;
    returning->val.termD.term = term;
    returning->location = location;

    return returning;
}

Variable* makeVariable(char *id, Location location) {
    Variable* variable = NEW(Variable);

    variable->lineno = lineno;
    variable->kind = varIdK;
    variable->val.idD.id = id;
    variable->location = location;

    return variable;
}

Variable* makeArraySubscript(Variable *variable, Expression *expression, Location location) {
    Variable* var = NEW(Variable);

    var->lineno = lineno;
    var->kind = arrayIndexK;
    var->val.arrayIndexD.var = variable;
    var->val.arrayIndexD.idx = expression;
    var->location = location;

    return var;
}

Variable* makeRecordSubscript(Variable* variable, char *id, Location location) {
    Variable* var = NEW(Variable);

    var->lineno = lineno;
    var->kind = recordLookupK;
    var->val.recordLookupD.var = variable;
    var->val.recordLookupD.id = id;
    var->location=location;

    return var;
}

Term *makeTermFromVariable(Variable *variable, Location location) {
    Term* term = NEW(Term);

    term->lineno = lineno;
    term->val.variableD.var = variable;
    term->kind = variableK;
    term->location = location;

    return term;
}

Term *makeFunctionCallTerm(char *functionId, ExpressionList *expressionList, Location location) {
    Term* term = NEW(Term);

    term->lineno = lineno;
    term->val.functionCallD.expressionList = expressionList;
    term->val.functionCallD.functionId = functionId;
    term->kind = functionCallK;
    term->location = location;

    return term;
}

Term *makeParentheses(Expression *expression, Location location) {
    Term* term = NEW(Term);

    term->lineno = lineno;
    term->val.parenthesesD.expression = expression;
    term->kind = parenthesesK;
    term->location = location;

    return term;
}

Term *makeNegatedTerm(Term *termToNegate, Location location) {
    Term* term = NEW(Term);

    term->lineno = lineno;
    term->val.negateD.term = termToNegate;
    term->kind = negateK;
    term->location = location;

    return term;
}

Term *makeAbsTerm(Expression *expression, Location location) {
    Term* term = NEW(Term);

    term->lineno = lineno;
    term->val.absD.expression = expression;
    term->kind = absK;
    term->location = location;

    return term;
}

Term *makeNumTerm(int n, Location location) {
    Term* term = NEW(Term);

    term->lineno = lineno;
    term->val.numD.num = n;
    term->kind = numK;
    term->location = location;

    return term;
}

Term *makeTrueTerm(Location location) {
    Term* term = NEW(Term);

    term->lineno = lineno;
    term->kind = trueK;
    term->location = location;

    return term;
}

Term *makeFalseTerm(Location location) {
    Term* term = NEW(Term);

    term->lineno = lineno;
    term->kind = falseK;
    term->location = location;

    return term;
}

Term *makeNullTerm(Location location) {
    Term* term = NEW(Term);

    term->lineno = lineno;
    term->kind = nullK;
    term->location = location;

    return term;
}

Term *makeLambdaTerm(Lambda *lambda, Location location) {
    Term* term = NEW(Term);

    term->kind = lambdaK;
    term->val.lambdaD.lambda = lambda;
    term->location = location;

    return term;
}

Term *makeDowncastTerm(Variable *var, Type *toCastTo, Location location) {
    Term* term = NEW(Term);

    term->kind = classDowncastk;
    term->val.classDowncastD.var = var;
    term->val.classDowncastD.toCastTo = toCastTo;
    term->location = location;

    return term;
}

Term *makeShorthandLambdaCall(Variable *access, ExpressionList *expressionList, Location location) {
    Term* term = NEW(Term);

    term->kind = shorthandCallK;
    term->val.shorthandCallD.var = access;
    term->val.shorthandCallD.expressionList = expressionList;
    term->location = location;

    return term;
}

Expression *makeEXPOpEXP(Expression *lhs, Operator *op, Expression *rhs, Location location) {
    Expression *e = NEW(Expression);

    e->lineno = lhs->lineno;
    e->kind = opK;
    e->val.op.left = lhs;
    e->val.op.operator = op;
    e->val.op.right = rhs;
    e->location = location;

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

FunctionHead *makeFunctionHead(char *identifier, VarDelList *declerationList, Type *type, Location location) {
    FunctionHead *head;
    head = NEW(FunctionHead);
    head->indentifier = identifier;
    head->declarationList = declerationList;
    head->returnType = type;
    head->lineno = lineno;
    head->location = location;
    return head;
}

FunctionTail *makeFunctionTail(char *identifier, Location location) {
    FunctionTail *tail;
    tail = NEW(FunctionTail);
    tail->indentifier = identifier;
    tail->lineno = lineno;
    tail->location = location;
    return tail;
}

Function *makeFunction(FunctionHead *head, Body *body, FunctionTail *tail, Location location) {
    Function *function = NEW(Function);

    function->head = head;
    function->body = body;
    function->tail = tail;
    function->location = location;

    return function;
}

Lambda *makeLambda(VarDelList* varDelList, Type *returnType, Body *body, Location location) {
    Lambda *lambda = NEW(Lambda);

    lambda->returnType = returnType;
    lambda->body = body;
    lambda->declarationList = varDelList;
    lambda->id = lambdaId;
    lambdaId++;
    lambda->inClassContext = false;
    lambda->location = location;

    return lambda;
}

VarDelList *makeVarDelList(VarType *varType, VarDelList *next, Location location) {
    VarDelList *list;
    list = NEW(VarDelList);
    list->identifier = varType->identifier;
    list->type = varType->type;
    list->next = next;
    list->location = location;
    return list;
}

DeclarationList *makeDeclarationList(Declaration *declaration, DeclarationList *next, Location location) {
    DeclarationList *result;
    result = NEW(DeclarationList);
    result->declaration = declaration;
    result->next = next;
    result->location = location;
    return result;
}

Declaration *makeVarDeclaration(char *id, Type *type, Location location) {
    Declaration *result;
    result = NEW(Declaration);
    stmDeclNum++;
    result->internal_stmDeclNum = stmDeclNum;

    result->lineno = lineno;
    result->kind = declVarK;
    result->val.varD.id = id;
    result->val.varD.type = type;
    result->location = location;
    return result;
}

StatementList *makeStatementList(Statement *statement, StatementList *next, Location location) {
    StatementList *stmList = NEW(StatementList);
    stmList->statement = statement;
    stmList->next = next;
    stmList->location = location;

    return stmList;
}

Statement *makeReturnStatement(Expression *exp, Location location) {
    Statement *statement = NEW(Statement);
    stmDeclNum++;
    statement->internal_stmDeclNum = stmDeclNum;

    statement->lineno = lineno;
    statement->kind = statReturnK;
    statement->val.returnD.exp = exp;
    statement->location = location;
    return statement;
}

Statement *makeIfStatement(Expression *exp, Statement *statement, Location location) {
    Statement *s = NEW(Statement);
    stmDeclNum++;
    s->internal_stmDeclNum = stmDeclNum;

    s->lineno = lineno;
    s->kind = statIfK;
    s->val.ifElD.exp = exp;
    s->val.ifElD.statement = statement;
    s->location = location;
    return s;
}

Statement *makeIfElseStatement(Expression *exp, Statement *statement, Statement *elseStatement, Location location) {
    Statement *s = NEW(Statement);
    stmDeclNum++;
    s->internal_stmDeclNum = stmDeclNum;

    s->lineno = lineno;
    s->kind = statIfElK;
    s->val.ifElD.exp = exp;
    s->val.ifElD.statement = statement;
    s->val.ifElD.elseStatement = elseStatement;
    s->location = location;
    return s;
}

Statement *makeAssignment(Variable* variable, Expression *exp, Location location) {
    Statement *statement = NEW(Statement);
    stmDeclNum++;
    statement->internal_stmDeclNum = stmDeclNum;

    statement->lineno = lineno;
    statement->kind = assignmentK;
    statement->val.assignmentD.exp = exp;
    statement->val.assignmentD.var = variable;
    statement->location = location;
    return statement;
}

Statement *makeAllocateStatement(Variable *var, Location location) {
    Statement *statement = NEW(Statement);
    stmDeclNum++;
    statement->internal_stmDeclNum = stmDeclNum;

    statement->lineno = lineno;
    statement->kind = statAllocateK;
    statement->val.allocateD.var = var;
    statement->location = location;
    return statement;
}

Statement *makeAllocateOfLenStatement(Variable *var, Expression *len, Location location) {
    Statement *statement = NEW(Statement);
    stmDeclNum++;
    statement->internal_stmDeclNum = stmDeclNum;

    statement->lineno = lineno;
    statement->kind = statAllocateLenK;
    statement->val.allocateLenD.var = var;
    statement->val.allocateLenD.len = len;
    statement->location = location;
    return statement;
}

Statement *makeWriteStatement(Expression *exp, Location location) {
    Statement *statement = NEW(Statement);
    stmDeclNum++;
    statement->internal_stmDeclNum = stmDeclNum;

    statement->lineno = lineno;
    statement->kind = statWriteK;
    statement->val.writeD.exp = exp;
    statement->location = location;
    return statement;
}

Statement *makeWhileStatement(Expression *exp, Statement *stm, Location location) {
    Statement *statement = NEW(Statement);
    stmDeclNum++;
    statement->internal_stmDeclNum = stmDeclNum;

    statement->lineno = lineno;
    statement->kind = statWhileK;
    statement->val.whileD.exp = exp;
    statement->val.whileD.statement = stm;
    statement->location = location;
    return statement;
}

Statement *makeStatementFromList(StatementList *statementList, Location location) {
    Statement *s = NEW(Statement);
    stmDeclNum++;
    s->internal_stmDeclNum = stmDeclNum;

    s->lineno = lineno;
    s->kind = stmListK;
    s->val.stmListD.statementList = statementList;
    s->location = location;
    return s;
}

Statement *makeEmptyExpression(Expression *expression, Location location) {
    Statement *s = NEW(Statement);
    stmDeclNum++;
    s->internal_stmDeclNum = stmDeclNum;

    s->lineno = lineno;
    s->kind = emptyK;
    s->val.empty.exp = expression;
    s->location = location;
    return s;
}

Declaration *makeVarsDeclaration(char *id, Type *type, Declaration *next, Location location) {
    Declaration *d;
    d = NEW(Declaration);
    d->lineno = lineno;
    d->kind = declVarsK;
    d->val.varsD.var = makeVarDeclaration(id, type, location);
    d->val.varsD.next = next;
    d->location = location;
    return d;
}

Type *makeIntType(Location location) {
    Type *result = NEW(Type);
    result->kind = typeIntK;
    result->location = location;
    return result;
}

Type *makeBoolType(Location location) {
    Type *result = NEW(Type);
    result->kind = typeBoolK;
    result->location = location;
    return result;
}

Declaration *makeVarDeclarations(VarDelList *vars, Location location) {
    if (vars == NULL) {
        return NULL;
    }

    Declaration *d;
    d = makeVarsDeclaration(vars->identifier, vars->type,
                                 makeVarDeclarations(vars->next, location), vars->location);


    stmDeclNum++;
    d->internal_stmDeclNum = stmDeclNum;
    d->location = location;
    return d;
}

Body *makeBody(DeclarationList *declarationList, StatementList *statementList, Location location) {
    Body *body = NEW(Body);
    body->declarationList = declarationList;
    body->statementList = statementList;
    body->location = location;

    return body;
}

Declaration *makeFunctionDecleration(Function *function, Location location) {
    Declaration *declaration = NEW(Declaration);
    stmDeclNum++;
    declaration->internal_stmDeclNum = stmDeclNum;

    declaration->lineno = lineno;
    declaration->kind = declFuncK;
    declaration->val.functionD.function = function;
    declaration->location = location;

    return declaration;
}

Declaration *makeTypeDeclaration(char *id, Type *type, Location location) {
    Declaration *result;
    result = NEW(Declaration);
    result->lineno = lineno;
    result->kind = declTypeK;
    result->val.typeD.id = id;
    result->val.typeD.type = type;
    stmDeclNum++;
    result->internal_stmDeclNum = stmDeclNum;
    result->location = location;
    return result;
}

Type *makeIdType(char *id, Location location) {
    Type *type;
    type = NEW(Type);

    type->kind = typeIdK;
    type->val.idType.id = id;
    type->location = location;

    return type;
}

Type *makeArrayType(Type *type, Location location) {
    Type *t;
    t = NEW(Type);

    t->kind = typeArrayK;
    t->val.arrayType.type = type;
    t->location = location;

    return t;
}

Type *makeRecordType(VarDelList *record, Location location) {
    Type *t;
    t = NEW(Type);

    t->kind = typeRecordK;
    t->val.recordType.types = record;
    t->location = location;

    return t;
}

Type *makeLambdaType(TypeList *typeList, Type *type, Location location) {
    Type *t = NEW(Type);

    t->kind = typeLambdaK;
    t->val.typeLambdaK.typeList = typeList;
    t->val.typeLambdaK.returnType = type;
    t->location = location;

    return t;
}

Type *makeVoidType(Location location) {
    Type *t = NEW(Type);

    t->kind = typeVoidK;
    t->location = location;

    return t;
}

ExpressionList *makeExpList(Expression *exp, ExpressionList *next, Location location) {
    ExpressionList *list = NEW(ExpressionList);
    list->lineno = lineno;
    list->expression = exp;
    list->next = next;
    list->location = location;
    return list;
}

Declaration *makeValDeclaration(char *id, Expression *rhs, Location location) {
    Declaration *declaration = NEW(Declaration);
    stmDeclNum++;
    declaration->internal_stmDeclNum = stmDeclNum;

    declaration->kind = declValK;
    declaration->val.valD.id = id;
    declaration->val.valD.rhs = rhs;
    declaration->location = location;

    return declaration;
}

Declaration *makeClassDeclaration(char *id, DeclarationList *declarationList, TypeList *typeList, TypeList* extensionList, Constructor* constructor, Location location) {
    Declaration *declaration = NEW(Declaration);
    stmDeclNum++;
    declaration->internal_stmDeclNum = stmDeclNum;

    declaration->kind = declClassK;
    declaration->val.classD.id = id;
    declaration->val.classD.declarationList = declarationList;
    declaration->val.classD.genericTypeParameters = typeList;
    declaration->val.classD.extendedClasses = extensionList;
    declaration->val.classD.constructor = constructor;
    declaration->location = location;
    typeIndex = 0;

    return declaration;
}

Type *makeClassType(char *id, TypeList *genericBoundTypes, Location location) {
    Type *type;
    type = NEW(Type);

    type->kind = typeClassK;
    type->val.typeClass.classId = id;
    type->val.typeClass.genericBoundValues = genericBoundTypes;
    type->location = location;

    return type;
}

TypeList *makeExtensionList(TypeList *next, char* class, TypeList *boundTypes, Location location) {
    TypeList *tpeLst = NEW(TypeList);

    tpeLst->location = location;
    tpeLst->next = next;

    Type *type = NEW(Type);

    type->kind = typeClassK;
    type->val.typeClass.classId = class;
    type->val.typeClass.genericBoundValues = boundTypes;
    type->val.typeGeneric.typeIndex = typeIndex;
    // TODO: What about when it is NULL ?
    if (boundTypes != NULL) {
        type->location = boundTypes->location;
    }
    typeIndex++;

    tpeLst->type = type;

    return tpeLst;
}

Constructor *makeClassConstructor(VarDelList *vdl, Body *body, Location location) {
    Constructor *constructor;
    constructor = NEW(Constructor);

    constructor->body = body;
    constructor->declarationList = vdl;
    constructor->location = location;

    return constructor;
}

Statement *makeAllocateWithConstructorStatement(Variable *var, ExpressionList *expressionList, Location location) {
    Statement *statement = NEW(Statement);
    stmDeclNum++;
    statement->internal_stmDeclNum = stmDeclNum;

    statement->lineno = lineno;
    statement->kind = statAllocateK;
    statement->val.allocateD.var = var;
    statement->val.allocateD.constructorList = expressionList;
    statement->location = location;
    return statement;
}

Statement *makeGCStatement(Location location) {
    Statement *returning = NEW(Statement);
    stmDeclNum++;
    returning->internal_stmDeclNum = stmDeclNum;
    returning->lineno = lineno;
    returning->kind = gcK;
    returning->location = location;
    return returning;
}

Statement *makeGCDebugStatement(Location location) {
    Statement *returning = NEW(Statement);
    stmDeclNum++;
    returning->internal_stmDeclNum = stmDeclNum;
    returning->lineno = lineno;
    returning->kind = gcDebugK;
    returning->location = location;
    return returning;
}

VarType *makeVarType(char *id, Type *type, Location location) {
    VarType *varType = NEW(VarType);
    varType->type = type;
    varType->identifier = id;
    varType->location = location;
    return varType;
}

Statement *makeWriteAnyStatement(Expression *exp, Location location) {
    Statement *returning = NEW(Statement);
    stmDeclNum++;
    returning->internal_stmDeclNum = stmDeclNum;
    returning->lineno = lineno;
    returning->kind = writeAny;
    returning->location = location;
    returning->val.writeD.exp = exp;
    return returning;
}

Type *makeCharType(Location location) {
    Type *result = NEW(Type);
    result->kind = typeCharK;
    result->location = location;
    return result;
}

Term *makeCharTerm(char *c, Location location) {
    Term* term = NEW(Term);

    term->lineno = lineno;
    term->val.charD.c = c;
    term->kind = charK;
    term->location = location;

    return term;
}
