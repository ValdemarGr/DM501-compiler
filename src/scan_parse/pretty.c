#include <stdio.h>
#include "pretty.h"
#include "scan_parse.h"

char *getTypeName(Type *type);

void prettyDeclaration(Declaration *decl) {
    switch (decl->kind) {
        case varsK:
            printf("var ");
            Declaration *p = decl;
            Declaration *d;
            while (p != NULL) {
                d = p->val.varsD.var;
                printf("\t%s : %s\n", d->val.varD.id, getTypeName(d->val.varD.type));
                p = p->val.varsD.next;
            }
            printf(";\n");
            break;
        case typeK:
            printf("type %s : %s;", decl->val.typeD.id, getTypeName(decl->val.typeD.type));
            break;
    }
}

char *getTypeName(Type *type) {
    switch (type->kind) {
        case intT:
            return "int";
        case boolT:
            return "bool";
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
