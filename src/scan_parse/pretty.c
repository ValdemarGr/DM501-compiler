#include <stdio.h>
#include "pretty.h"
#include "scan_parse.h"

static int indentation = 0;

void prettyStatementList(StatementList *statementList);

void printCurrentIndent() {
    for (int i = 0; i < indentation; i++) {
        printf("\t");
    }
}

void prettyTwoExpOperation(Expression *lhs, Operator *operator, Expression *rhs) {
    prettyEXP(lhs);

    switch(operator->kind) {
        case opMultK:
            printf(" * ");
            break;
        case opDivK:
            printf(" / ");
            break;
        case opPlusK:
            printf(" + ");
            break;
        case opMinusK:
            printf(" - ");
            break;
        case opEqualityK:
            printf(" == ");
            break;
        case opInequalityK:
            printf(" != ");
            break;
        case opGreaterK:
            printf(" > ");
            break;
        case opLessK:
            printf(" < ");
            break;
        case opGeqK:
            printf(" >= ");
            break;
        case opLeqK:
            printf(" <= ");
            break;
        case opAndK:
            printf(" && ");
            break;
        case opOrK:
            printf(" || ");
            break;
    }

    prettyEXP(rhs);
}

void prettyArgument(char *id) {
    printf("\033[0;35m%s\033[0m", id);
}

void prettyFunctionName(char *id) {
    printf("\033[0;31m%s\033[0m", id);
}

void prettyExpressionList(ExpressionList *expressionList) {
    ExpressionList *el = expressionList;

    while(el != NULL) {
        prettyEXP(el->expression);
        el = el->next;
    }
}

char *getTypeName(Type *type);

void prettyKeyword(char *keyword) {
    printf("\033[1;34m%s\033[0m", keyword);
}

void prettyVariable(Variable* variable) {
    switch(variable->kind) {
        case varIdK:
            printf("%s", variable->val.idD);
            break;
        case arrayIndexK:
            prettyVariable(variable->val.arrayIndexD.var);
            printf("[");
            prettyEXP(variable->val.arrayIndexD.idx);
            printf("]");
            break;
        case recordLookupK:
            prettyVariable(variable->val.recordLookupD.var);
            printf(".");
            printf(variable->val.recordLookupD.id);
            break;
    }
}

void prettyTerm(Term *term) {
    switch(term->kind) {
        case variableK:
            prettyVariable(term->val.variableD.var);
            break;
        case functionCallK:
            prettyFunctionName(term->val.functionCallD.functionId);
            printf("(");
            prettyExpressionList(term->val.functionCallD.expressionList);
            printf(")");
            break;
        case negateK:
            printf("!");
            prettyTerm(term->val.negateD.term);
            break;
        case absK:
            printf("|");
            prettyEXP(term->val.absD.expression);
            printf("|");
            break;
        case trueK:
            prettyKeyword("true");
            break;
        case falseK:
            prettyKeyword("false");
            break;
        case nullK:
            prettyKeyword("null");
            break;

    }
}

void prettyType(Type *type) {
    switch(type->kind) {
        case typeIdK:
            printf("\033[0;36m%s\033[0m", type->val.idType.id);
            break;
        case typeArrayK:
            printf("\033[0;32marray of \033[0m");
            prettyType(type->val.arrayType.type);
            break;
        case typeRecordK:
            printf("\033[0;32mrecord of\033[0m {");
            VarDelList *dels = type->val.recordType.types;

            while (dels != NULL) {
                printf("%s : ", dels->identifier);
                prettyType(dels->type);
                printf(", ");
                dels = dels->next;
            }
            printf("\b\b}");
            break;
        default:
            printf("\033[0;36m%s\033[0m", getTypeName(type));
            break;
    }
}

void prettyFunction(Function *f) {
    prettyKeyword("\nfunc");
    printf(" ");
    prettyFunctionName(f->head->indentifier);
    printf("(");
    VarDelList *dels = f->head->declarationList;

    while (dels != NULL) {
        prettyArgument(dels->identifier);
        printf(" : ");
        prettyType(dels->type);
        printf(", ");
        dels = dels->next;
    }

    printf("\b\b) : ");
    prettyType(f->head->returnType);
    printf("\n");
    prettyBody(f->body);
    prettyKeyword("end ");
    prettyFunctionName(f->tail->indentifier);
    printf("\n");
}

void prettyDeclaration(Declaration *decl) {
    printCurrentIndent();

    switch (decl->kind) {
        case declVarsK:
            prettyKeyword("var ");
            Declaration *p = decl;
            Declaration *d;
            while (p != NULL) {
                d = p->val.varsD.var;
                printf("%s : ", d->val.varD.id);
                prettyType(d->val.varD.type);
                printf(", ");
                p = p->val.varsD.next;
            }
            printf("\b\b;\n");
            break;
        case declTypeK:
            prettyKeyword("type");
            printf(" %s = ", decl->val.typeD.id);
            prettyType(decl->val.typeD.type);
            printf(";\n");
            break;
        case declFuncK:
            indentation++;
            prettyFunction(decl->val.functionD.function);
            indentation--;
            break;
        default:
            break;
    }
}


void prettyStatement(Statement *statement) {
    printCurrentIndent();

    switch (statement->kind) {
        case statReturnK:
            prettyKeyword("return ");
            prettyEXP(statement->val.returnD.exp);
            printf(";\n");
            break;
        case statWriteK:
            prettyKeyword("write ");
            prettyEXP(statement->val.writeD.exp);
            printf(";\n");
            break;
        case assignmentK:
            prettyVariable(statement->val.assignmentD.var);
            prettyKeyword(" = ");
            prettyEXP(statement->val.assignmentD.exp);
            printf(";\n");
            break;
        case statAllocateK:
            prettyKeyword("allocate ");
            prettyVariable(statement->val.allocateD.var);
            printf(";\n");
            break;
        case statAllocateLenK:
            prettyKeyword("allocate ");
            prettyVariable(statement->val.allocateLenD.var);
            prettyKeyword(" of length ");
            prettyEXP(statement->val.allocateLenD.len);
            printf(";\n");
            break;
        case statIfK:
            prettyKeyword("if ");
            prettyEXP(statement->val.ifD.exp);
            prettyKeyword(" then ");
            printf("{\n");
            indentation++;
            prettyStatement(statement->val.ifD.statement);
            indentation--;
            printCurrentIndent();
            printf("}\n");
            break;
        case statIfElK:
            prettyKeyword("if ");
            prettyEXP(statement->val.ifElD.exp);
            prettyKeyword(" then ");
            printf("{\n");
            indentation++;
            prettyStatement(statement->val.ifElD.statement);
            indentation--;
            printCurrentIndent();
            printf("}");
            prettyKeyword(" else ");
            printf("{\n");
            indentation++;
            prettyStatement(statement->val.ifElD.elseStatement);
            indentation--;
            printCurrentIndent();
            printf("}\n");
            break;
        case statWhileK:
            prettyKeyword("while ");
            prettyEXP(statement->val.whileD.exp);
            prettyKeyword(" do ");
            printf("{\n");
            indentation++;
            prettyStatement(statement->val.whileD.statement);
            indentation--;
            printCurrentIndent();
            printf("}\n");
            break;
        case stmListK:
            printf("\b");
            prettyStatementList(statement->val.stmListD.statementList);
            break;
    }
}

void prettyStatementList(StatementList *statementList) {
    StatementList *st = statementList;

    while(st != NULL) {
        prettyStatement(st->statement);
        st = st->next;
    }
}

char *getTypeName(Type *type) {
    switch (type->kind) {
        case typeIntK:
            return "int";
        case typeBoolK:
            return "bool";
        default:
            return "other type";
    }
}

void prettyBody(Body *body) {
    DeclarationList *delc_lst = body->declarationList;
    StatementList *stm_lst = body->statementList;

    //Now we traverse them until they both exhaust and while we're doing it we print the smallest line number first
    while (delc_lst != NULL || stm_lst != NULL) {
        //Check if we only need delc list or stm list or both of them
        if (stm_lst == NULL) {
            prettyDeclaration(delc_lst->declaration);
            delc_lst = delc_lst->next;
        } else if (delc_lst == NULL) {
            prettyStatement(stm_lst->statement);
            stm_lst = stm_lst->next;
        } else {
            //Here we check the ordering on lineno
            if (stm_lst->statement->lineno > delc_lst->declaration->lineno) {
                prettyDeclaration(delc_lst->declaration);
                delc_lst = delc_lst->next;
            } else {
                prettyStatement(stm_lst->statement);
                stm_lst = stm_lst->next;
            }
        }
    }
}

void prettyEXP(Expression *e) {
    switch (e->kind) {
        case idK:
            printf("%s", e->val.idE);
            break;
        case intconstK:
            printf("%i", e->val.intconstE);
            break;
        case opK:
            prettyTwoExpOperation(e->val.op.left, e->val.op.operator, e->val.op.right);
            break;
        case termK:
            prettyTerm(e->val.termD.term);
            break;
    }
}
