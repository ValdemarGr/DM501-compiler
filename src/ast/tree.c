#include <stddef.h>
#include <stdio.h>
#include "../utils/memory.h"
#include "tree.h"

extern int lineno;
extern int stmDeclNum;
int typeIndex = 0;
int lambdaId = 0;

TypeList *makeTypeList(TypeList* next, Type *elem) {
    TypeList *tpeLst = NEW(TypeList);

    tpeLst->next = next;
    tpeLst->type = elem;

    return tpeLst;
}

TypeList *makeGenericTypeList(TypeList* next, char* id, char* subType){
    TypeList *tpeLst = NEW(TypeList);

    tpeLst->next = next;

    Type *tpe = NEW(Type);

    tpe->kind = typeGenericK;
    tpe->val.typeGeneric.genericName = id;
    tpe->val.typeGeneric.subType = subType;
    tpe->val.typeGeneric.typeIndex = typeIndex;
    typeIndex++;

    tpeLst->type = tpe;

    return tpeLst;
}

Expression *makeEXPFromTerm(Term *term) {
    Expression *returning = NEW(Expression);

    returning->lineno = lineno;
    returning->kind = termK;
    returning->val.termD.term = term;

    return returning;
}

Variable* makeVariable(char *id) {
    Variable* returning = NEW(Variable);

    returning->lineno = lineno;
    returning->kind = varIdK;
    returning->val.idD.id = id;

    return returning;
}

Variable* makeArraySubscript(Variable *variable, Expression *expression) {
    Variable* returning = NEW(Variable);

    returning->lineno = lineno;
    returning->kind = arrayIndexK;
    returning->val.arrayIndexD.var = variable;
    returning->val.arrayIndexD.idx = expression;

    return returning;
}

Variable* makeRecordSubscript(Variable* variable, char *id) {
    Variable* returning = NEW(Variable);

    returning->lineno = lineno;
    returning->kind = recordLookupK;
    returning->val.recordLookupD.var = variable;
    returning->val.recordLookupD.id = id;

    return returning;
}

Term *makeTermFromVariable(Variable *variable) {
    Term* returning = NEW(Term);

    returning->lineno = lineno;
    returning->val.variableD.var = variable;
    returning->kind = variableK;

    return returning;
}

Term *makeFunctionCallTerm(char *functionId, ExpressionList *expressionList) {
    Term* returning = NEW(Term);

    returning->lineno = lineno;
    returning->val.functionCallD.expressionList = expressionList;
    returning->val.functionCallD.functionId = functionId;
    returning->kind = functionCallK;

    return returning;
}

Term *makeParentheses(Expression *expression) {
    Term* returning = NEW(Term);

    returning->lineno = lineno;
    returning->val.parenthesesD.expression = expression;
    returning->kind = parenthesesK;

    return returning;
}

Term *makeNegatedTerm(Term *term) {
    Term* returning = NEW(Term);

    returning->lineno = lineno;
    returning->val.negateD.term = term;
    returning->kind = negateK;

    return returning;
}

Term *makeAbsTerm(Expression *expression) {
    Term* returning = NEW(Term);

    returning->lineno = lineno;
    returning->val.absD.expression = expression;
    returning->kind = absK;

    return returning;
}

Term *makeNumTerm(int n) {
    Term* term = NEW(Term);

    term->lineno = lineno;
    term->val.numD.num = n;
    term->kind = numK;

    return term;
}

Term *makeTrueTerm() {
    Term* returning = NEW(Term);

    returning->lineno = lineno;
    returning->kind = trueK;

    return returning;
}

Term *makeFalseTerm() {
    Term* returning = NEW(Term);

    returning->lineno = lineno;
    returning->kind = falseK;

    return returning;
}

Term *makeNullTerm() {
    Term* returning = NEW(Term);

    returning->lineno = lineno;
    returning->kind = nullK;

    return returning;
}

Term *makeLambdaTerm(Lambda *lambda) {
    Term* returning = NEW(Term);

    returning->kind = lambdaK;
    returning->val.lambdaD.lambda = lambda;

    return returning;
}

Term *makeDowncastTerm(char* varId, char *downcastId) {
    Term* returning = NEW(Term);

    returning->kind = classDowncastk;
    returning->val.classDowncastD.varId =varId;
    returning->val.classDowncastD.downcastId =downcastId;

    return returning;
}

Term *makeShorthandLambdaCall(Variable *access, ExpressionList *expressionList) {
    Term* returning = NEW(Term);

    returning->kind = shorthandCallK;
    returning->val.shorthandCallD.var = access;
    returning->val.shorthandCallD.expressionList = expressionList;

    return returning;
}

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
    tail->lineno = lineno;
    return tail;
}

Function *makeFunction(FunctionHead *head, Body *body, FunctionTail *tail) {
    Function *function = NEW(Function);

    function->head = head;
    function->body = body;
    function->tail = tail;

    return function;
}

Lambda *makeLambda(VarDelList* varDelList, Type *returnType, Body *body) {
    Lambda *lambda = NEW(Lambda);

    lambda->returnType = returnType;
    lambda->body = body;
    lambda->declarationList = varDelList;
    lambda->id = lambdaId;
    lambdaId++;
    lambda->inClassContext = false;

    return lambda;
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
    stmDeclNum++;
    result->internal_stmDeclNum = stmDeclNum;

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
    stmDeclNum++;
    statement->internal_stmDeclNum = stmDeclNum;

    statement->lineno = lineno;
    statement->kind = statReturnK;
    statement->val.returnD.exp = exp;
    return statement;
}

Statement *makeIfStatement(Expression *exp, Statement *statement) {
    Statement *returning = NEW(Statement);
    stmDeclNum++;
    returning->internal_stmDeclNum = stmDeclNum;

    returning->lineno = lineno;
    returning->kind = statIfK;
    returning->val.ifElD.exp = exp;
    returning->val.ifElD.statement = statement;
    return returning;
}

Statement *makeIfElseStatement(Expression *exp, Statement *statement, Statement *elseStatement) {
    Statement *returning = NEW(Statement);
    stmDeclNum++;
    returning->internal_stmDeclNum = stmDeclNum;

    returning->lineno = lineno;
    returning->kind = statIfElK;
    returning->val.ifElD.exp = exp;
    returning->val.ifElD.statement = statement;
    returning->val.ifElD.elseStatement = elseStatement;
    return returning;
}

Statement *makeAssignment(Variable* variable, Expression *exp) {
    Statement *statement = NEW(Statement);
    stmDeclNum++;
    statement->internal_stmDeclNum = stmDeclNum;

    statement->lineno = lineno;
    statement->kind = assignmentK;
    statement->val.assignmentD.exp = exp;
    statement->val.assignmentD.var = variable;
    return statement;
}

Statement *makeAllocateStatement(Variable *var) {
    Statement *statement = NEW(Statement);
    stmDeclNum++;
    statement->internal_stmDeclNum = stmDeclNum;

    statement->lineno = lineno;
    statement->kind = statAllocateK;
    statement->val.allocateD.var = var;
    return statement;
}

Statement *makeAllocateOfLenStatement(Variable *var, Expression *len) {
    Statement *statement = NEW(Statement);
    stmDeclNum++;
    statement->internal_stmDeclNum = stmDeclNum;

    statement->lineno = lineno;
    statement->kind = statAllocateLenK;
    statement->val.allocateLenD.var = var;
    statement->val.allocateLenD.len = len;
    return statement;
}

Statement *makeWriteStatement(Expression *exp) {
    Statement *statement = NEW(Statement);
    stmDeclNum++;
    statement->internal_stmDeclNum = stmDeclNum;

    statement->lineno = lineno;
    statement->kind = statWriteK;
    statement->val.writeD.exp = exp;
    return statement;
}

Statement *makeWhileStatement(Expression *exp, Statement *stm) {
    Statement *statement = NEW(Statement);
    stmDeclNum++;
    statement->internal_stmDeclNum = stmDeclNum;

    statement->lineno = lineno;
    statement->kind = statWhileK;
    statement->val.whileD.exp = exp;
    statement->val.whileD.statement = stm;
    return statement;
}

Statement *makeStatementFromList(StatementList *statementList) {
    Statement *returning = NEW(Statement);
    stmDeclNum++;
    returning->internal_stmDeclNum = stmDeclNum;

    returning->lineno = lineno;
    returning->kind = stmListK;
    returning->val.stmListD.statementList = statementList;
    return returning;
}

Statement *makeEmptyExpression(Expression *expression) {
    Statement *returning = NEW(Statement);
    stmDeclNum++;
    returning->internal_stmDeclNum = stmDeclNum;

    returning->lineno = lineno;
    returning->kind = emptyK;
    returning->val.empty.exp = expression;
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


    stmDeclNum++;
    result->internal_stmDeclNum = stmDeclNum;
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
    stmDeclNum++;
    declaration->internal_stmDeclNum = stmDeclNum;

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
    stmDeclNum++;
result->internal_stmDeclNum = stmDeclNum;
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

Type *makeLambdaType(TypeList *typeList, Type *type) {
    Type *t = NEW(Type);

    t->kind = typeLambdaK;
    t->val.typeLambdaK.typeList = typeList;
    t->val.typeLambdaK.returnType = type;

    return t;
}

ExpressionList *makeExpList(Expression *exp, ExpressionList *next) {
    ExpressionList *list = NEW(ExpressionList);
    list->lineno = lineno;
    list->expression = exp;
    list->next = next;
    return list;
}

Declaration *makeValDeclaration(char *id, Expression *rhs) {
    Declaration *declaration = NEW(Declaration);
    stmDeclNum++;
    declaration->internal_stmDeclNum = stmDeclNum;

    declaration->kind = declValK;
    declaration->val.valD.id = id;
    declaration->val.valD.rhs = rhs;

    return declaration;
}

Declaration *makeClassDeclaration(char *id, DeclarationList *declarationList, TypeList *typeList, TypeList* extensionList) {
    Declaration *declaration = NEW(Declaration);
    stmDeclNum++;
    declaration->internal_stmDeclNum = stmDeclNum;

    declaration->kind = declClassK;
    declaration->val.classD.id = id;
    declaration->val.classD.declarationList = declarationList;
    declaration->val.classD.genericTypeParameters = typeList;
    declaration->val.classD.extendedClasses = extensionList;
    typeIndex = 0;

    return declaration;
}

Type *makeClassType(char *id, TypeList *genericBoundTypes) {
    Type *type;
    type = NEW(Type);

    type->kind = typeClassK;
    type->val.typeClass.classId = id;
    type->val.typeClass.genericBoundValues = genericBoundTypes;

    return type;
}

TypeList *makeExtensionList(TypeList *next, char* class, TypeList *boundTypes) {
    TypeList *tpeLst = NEW(TypeList);

    tpeLst->next = next;

    Type *type = NEW(Type);

    type->kind = typeClassK;
    type->val.typeClass.classId = class;
    type->val.typeClass.genericBoundValues = boundTypes;
    type->val.typeGeneric.typeIndex = typeIndex;
    typeIndex++;

    tpeLst->type = type;

    return tpeLst;
}
