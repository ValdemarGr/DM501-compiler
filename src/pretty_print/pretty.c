#include <stdio.h>
#include "stdbool.h"
#include "pretty.h"
#include "../ast/tree.h"

static int indentation = 0;
extern bool printWithTypes;

void prettyStatementList(StatementList *statementList);
void prettyLambda(Lambda *lambda);

void printCurrentIndent() {
    for (int i = 0; i < indentation; i++) {
        printf("\t");
    }
}

void prettyTwoExpOperation(Expression *lhs, Operator *operator, Expression *rhs, SymbolTable *symbolTable) {
    prettyEXP(lhs, symbolTable);

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

    prettyEXP(rhs, symbolTable);
}

void prettyArgument(char *id) {
    printf("\033[0;35m%s\033[0m", id);
}

void prettyFunctionName(char *id) {
    printf("\033[0;31m%s\033[0m", id);
}

void prettyExpressionList(ExpressionList *expressionList, SymbolTable *symbolTable) {
    ExpressionList *el = expressionList;
    bool isEmpty = el == NULL;

    while(el != NULL) {
        prettyEXP(el->expression, symbolTable);
        printf(", ");
        el = el->next;
    }

    if (!isEmpty) {
        printf("\b\b");
    }
}

char *getTypeName(Type *type);

void prettyKeyword(char *keyword) {
    printf("\033[1;34m%s\033[0m", keyword);
}

void prettyVariable(Variable* variable, SymbolTable *symbolTable) {
    switch(variable->kind) {
        case varIdK:
            printf("%s", variable->val.idD.id);
            break;
        case arrayIndexK:
            prettyVariable(variable->val.arrayIndexD.var, symbolTable);
            printf("[");
            prettyEXP(variable->val.arrayIndexD.idx, symbolTable);
            printf("]");
            break;
        case recordLookupK:
            prettyVariable(variable->val.recordLookupD.var, symbolTable);
            printf(".");
            printf("%s", variable->val.recordLookupD.id);
            break;
    }
}

void prettyTerm(Term *term, SymbolTable *symbolTable) {
    switch(term->kind) {
        case variableK:
            prettyVariable(term->val.variableD.var, symbolTable);
            break;
        case functionCallK:
            prettyFunctionName(term->val.functionCallD.functionId);
            printf("(");
            prettyExpressionList(term->val.functionCallD.expressionList, symbolTable);
            printf(")");
            break;
        case negateK:
            printf("!");
            prettyTerm(term->val.negateD.term, symbolTable);
            break;
        case absK:
            printf("|");
            prettyEXP(term->val.absD.expression, symbolTable);
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
        case numK:
            printf("%i", term->val.numD.num);
            break;
        case lambdaK:
            prettyLambda(term->val.lambdaD.lambda);
            break;
        case classDowncastk:
            printf("%s : \033[0;36m%s\033[0m", term->val.classDowncastD.varId, term->val.classDowncastD.downcastId);
            break;
        case shorthandCallK: {
            prettyVariable(term->val.shorthandCallD.var, symbolTable);
            printf("(");
            prettyExpressionList(term->val.shorthandCallD.expressionList, symbolTable);
            printf(")");
        } break;
    }
}

void prettyType(Type *type) {
    if (type == NULL_KITTY_VALUE_INDICATOR) {
        printf("\033[0;36mnull\033[0m");
        return;
    }

    if (type == NULL) {
        printf("IDK MAN");
        return;
    }

    switch(type->kind) {
        case typeVoidK: {
            printf("\033[0;36mvoid\033[0m");
        } break;
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
        case typeLambdaK:
            printf("(");
            TypeList *typeList = type->val.typeLambdaK.typeList;

            while (typeList != NULL) {
                prettyType(typeList->type);

                if (typeList->next != NULL) {
                    printf(", ");
                }

                typeList = typeList->next;
            }
            printf(") -> ");

            prettyType(type->val.typeLambdaK.returnType);
            break;
        case typeClassK:
            printf("\033[0mclass \033[36m%s\033[0m", type->val.typeClass.classId);

            if (type->val.typeClass.genericBoundValues != NULL) {
                TypeList *typeList = type->val.typeClass.genericBoundValues;
                printf("\033[0m[");

                while (typeList != NULL) {

                    //printf("\033[36m%s", typeToString(typeList->type));
                    prettyType(typeList->type);

                    if (typeList->next != NULL) {
                        printf("\033[0m, \033[36m");
                    }

                    typeList = typeList->next;
                }
                printf("\033[0m]");

            }

            break;

        default:
            printf("\033[0;36m%s\033[0m", typeToString(type));
            break;
    }
}

void prettyFunction(Function *f) {
    printf("\n");
    printCurrentIndent();
    indentation++;
    prettyKeyword("func");
    printf(" ");
    prettyFunctionName(f->head->indentifier);
    printf("(");
    VarDelList *dels = f->head->declarationList;
    bool hasParameter = dels != NULL;

    while (dels != NULL) {
        prettyArgument(dels->identifier);
        printf(" : ");
        prettyType(dels->type);
        printf(", ");
        dels = dels->next;
    }

    if (hasParameter){
        printf("\b\b");
    }

    printf(") : ");

    prettyType(f->head->returnType);
    printf("\n");
    prettyBody(f->body);
    indentation--;
    printCurrentIndent();
    prettyKeyword("end ");
    prettyFunctionName(f->tail->indentifier);
    printf("\n");
}

void prettyLambda(Lambda *lambda) {
    printf("(");
    VarDelList *dels = lambda->declarationList;
    bool hasParameter = dels != NULL;

    while (dels != NULL) {
        prettyArgument(dels->identifier);
        printf(" : ");
        prettyType(dels->type);
        printf(", ");
        dels = dels->next;
    }

    if (hasParameter){
        printf("\b\b");
    }

    printf(") : ");

    prettyType(lambda->returnType);
    printf(" -> {\n");
    indentation++;
    prettyBody(lambda->body);
    indentation--;
    printCurrentIndent();
    printf("}");
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
            prettyFunction(decl->val.functionD.function);
            break;
        case declValK:
            prettyKeyword("val ");
            printf("%s = ", decl->val.valD.id);
            prettyEXP(decl->val.valD.rhs, decl->symbolTable);
            printf(";\n");
            break;
        case declClassK:
            prettyKeyword("class ");
            printf("%s ", decl->val.classD.id);

            if (decl->val.classD.genericTypeParameters != NULL) {
                printf("\033[0m[");

                TypeList *typeList = decl->val.classD.genericTypeParameters;

                while (typeList != NULL) {

                    printf("\033[0;32m%s", typeList->type->val.typeGeneric.genericName);

                    if (typeList->type->val.typeGeneric.subType != NULL) {
                        printf("\033[0m: \033[0;36m%s", typeList->type->val.typeGeneric.subType);
                    }

                    if (typeList->next != NULL) {
                        printf("\033[0m, \033[0m");
                    }
                    typeList = typeList->next;
                }

                printf("\033[0m]");
            }

            if (decl->val.classD.extendedClasses != NULL) {
                prettyKeyword("with ");

                TypeList *typeList = decl->val.classD.extendedClasses;

                while (typeList != NULL) {

                    printf("\033[0;36m%s", typeList->type->val.typeClass.classId);

                    if (typeList->next != NULL) {
                        printf(" \033[0m");
                    }
                    typeList = typeList->next;
                }

            }

            printf("\033[0m { \n");
            indentation++;
            DeclarationList *declarationList = decl->val.classD.declarationList;

            while (declarationList != NULL) {
                prettyDeclaration(declarationList->declaration);
                declarationList = declarationList->next;
            }

            indentation--;
            printf("};\n");
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
            prettyEXP(statement->val.returnD.exp, statement->symbolTable);
            printf(";\n");
            break;
        case statWriteK:
            prettyKeyword("write ");
            prettyEXP(statement->val.writeD.exp, statement->symbolTable);
            printf(";\n");
            break;
        case assignmentK:
            prettyVariable(statement->val.assignmentD.var, statement->symbolTable);
            prettyKeyword(" = ");
            prettyEXP(statement->val.assignmentD.exp, statement->symbolTable);
            printf(";\n");
            break;
        case statAllocateK:
            prettyKeyword("allocate ");
            prettyVariable(statement->val.allocateD.var, statement->symbolTable);
            printf(";\n");
            break;
        case statAllocateLenK:
            prettyKeyword("allocate ");
            prettyVariable(statement->val.allocateLenD.var, statement->symbolTable);
            prettyKeyword(" of length ");
            prettyEXP(statement->val.allocateLenD.len, statement->symbolTable);
            printf(";\n");
            break;
        case statIfK:
            prettyKeyword("if ");
            prettyEXP(statement->val.ifD.exp, statement->symbolTable);
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
            prettyEXP(statement->val.ifElD.exp, statement->symbolTable);
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
            prettyEXP(statement->val.whileD.exp, statement->symbolTable);
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
        case emptyK: {
            prettyEXP(statement->val.empty.exp, statement->symbolTable);
            printf(";\n");
        } break;
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

void prettyEXP(Expression *e, SymbolTable *symbolTable) {
    Type *o = evaluateExpressionType(e, symbolTable);
    Type *ret = NULL;
    switch (e->kind) {
        case opK:
            if (printWithTypes == true) {
                ret = evaluateExpressionType(e, symbolTable);
                prettyType(ret);
                printf(":(");
            } else {
                printf("(");
            }
            prettyTwoExpOperation(e->val.op.left, e->val.op.operator, e->val.op.right, symbolTable);
            printf(")");
            break;
        case termK:
            if (printWithTypes == true) {
                ret = evaluateExpressionType(e, symbolTable);
                prettyType(ret);
                printf(":(");
            }
            prettyTerm(e->val.termD.term, symbolTable);
            if (printWithTypes == true) {
                printf(")");
            }
            break;
    }
}
