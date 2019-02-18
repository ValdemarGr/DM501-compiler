#include <stdio.h>
#include "pretty.h"
#include "scan_parse.h"

char *getTypeName(Type *type);

void prettyKeyword(char *keyword) {
    printf("\033[1;34m%s\033[0m", keyword);
}

void prettyType(Type *type) {
    switch(type->kind) {
        case idT:
            printf("\033[0;36m%s\033[0m", type->val.idType.id);
            break;
        case arrayT:
            printf("\033[0;32marray of \033[0m");
            prettyType(type->val.arrayType.type);
            break;
        case recordT:
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
}

void prettyDeclaration(Declaration *decl) {
    switch (decl->kind) {
        case varsK:
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
        case typeK:
            prettyKeyword("type");
            printf(" %s = ", decl->val.typeD.id);
            prettyType(decl->val.typeD.type);
            printf(";\n");
            break;
        case functionK:
            prettyFunction(decl->val.functionD.function);
            break;
        default:
            break;
    }
}

char *getTypeName(Type *type) {
    switch (type->kind) {
        case intT:
            return "int";
        case boolT:
            return "bool";
        default:
            return "other type";
    }
}

void prettyDeclarationList(DeclarationList *list) {
    while(list != NULL) {
        prettyDeclaration(list->declaration);
        list = list->next;
    }
}

void prettyBody(Body *body) {
    prettyDeclarationList(body->declarationList);
}

void prettyEXP(EXP *e) {
    switch (e->kind) {
        case idK:
            printf("%s", e->val.idE);
            break;
        case intconstK:
            printf("%i", e->val.intconstE);
            break;
        case timesK:
            printf("(");
            prettyEXP(e->val.timesE.left);
            printf("*");
            prettyEXP(e->val.timesE.right);
            printf(")");
            break;
        case divK:
            printf("(");
            prettyEXP(e->val.divE.left);
            printf("/");
            prettyEXP(e->val.divE.right);
            printf(")");
            break;
        case plusK:
            printf("(");
            prettyEXP(e->val.plusE.left);
            printf("+");
            prettyEXP(e->val.plusE.right);
            printf(")");
            break;
        case minusK:
            printf("(");
            prettyEXP(e->val.minusE.left);
            printf("-");
            prettyEXP(e->val.minusE.right);
            printf(")");
            break;
        case functionK:
            printf("func ");
            printf("%s", e->val.functionE.identifier);
            printf("\n");
            prettyEXP(e->val.functionE.body);
            break;
    }
}
