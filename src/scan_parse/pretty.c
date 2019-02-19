#include <stdio.h>
#include "pretty.h"
#include "scan_parse.h"

static int indentation = 0;

void printCurrentIndent() {
    for (int i = 0; i < indentation; i++) {
        printf("\t");
    }
}

char *getTypeName(Type *type);

void prettyKeyword(char *keyword) {
    printf("\033[1;34m%s\033[0m", keyword);
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

void prettyArgument(char *id) {
    printf("\033[0;35m%s\033[0m", id);
}

void prettyFunctionName(char *id) {
    printf("\033[0;31m%s\033[0m", id);
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
        case statAllocateK:
            prettyKeyword("allocate ");
            prettyEXP(statement->val.allocateD.exp);
            printf(";\n");
            break;
        case statAllocateLenK:
            prettyKeyword("allocate ");
            prettyEXP(statement->val.allocateLenD.exp);
            prettyKeyword(" of length ");
            prettyEXP(statement->val.allocateLenD.len);
            printf(";\n");
            break;
        case statIfK:
            prettyKeyword("if ");
            prettyEXP(statement->val.ifD.exp);
            prettyKeyword(" then\n");
            indentation++;
            prettyStatement(statement->val.ifD.statement);
            indentation--;
            printf("\n");
            break;
        case statIfElK:
            prettyKeyword("if ");
            prettyEXP(statement->val.ifElD.exp);
            prettyKeyword(" then\n");
            indentation++;
            prettyStatement(statement->val.ifElD.statement);
            indentation--;
            prettyKeyword("else\n");
            indentation++;
            prettyStatement(statement->val.ifElD.elseStatement);
            indentation--;
            printf("\n");
            break;
        case statWhileK:
            prettyKeyword("while ");
            prettyEXP(statement->val.whileD.exp);
            prettyKeyword(" do ");
            printf("{\n");
            indentation++;
            prettyBody(statement->val.whileD.localBody);
            indentation--;
            printf("}\n");
            break;
        case statWhileSSK:
            prettyKeyword("while ");
            prettyEXP(statement->val.whileSSD.exp);
            prettyKeyword(" do \n");
            indentation++;
            prettyStatement(statement->val.whileSSD.statement);
            indentation--;
            break;
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
        case expIdK:
            printf("%s", e->val.idE);
            break;
        case expNumK:
            printf("%i", e->val.intconstE);
            break;
        case expMultK:
            printf("(");
            prettyEXP(e->val.timesE.left);
            printf("*");
            prettyEXP(e->val.timesE.right);
            printf(")");
            break;
        case expDivK:
            printf("(");
            prettyEXP(e->val.divE.left);
            printf("/");
            prettyEXP(e->val.divE.right);
            printf(")");
            break;
        case expPlusK:
            printf("(");
            prettyEXP(e->val.plusE.left);
            printf("+");
            prettyEXP(e->val.plusE.right);
            printf(")");
            break;
        case expMinusK:
            printf("(");
            prettyEXP(e->val.minusE.left);
            printf("-");
            prettyEXP(e->val.minusE.right);
            printf(")");
            break;
    }
}
