//
// Created by valde on 2/23/19.
//

#include "weed_functions.h"
#include "../../ast/tree.h"
#include "../../utils/map.h"
#include "../../error/error.h"

Error *weed_function_name(Body *body) {
    //Find all functions
    DeclarationList *l = body->declarationList;

    while (l != NULL) {
        //Check for nest, if nests are found this means that the rule cannot be upheld
        if (l->declaration->kind == declFuncK) {
            Body *innerBody = l->declaration->val.functionD.function->body;

            DeclarationList *innerDeclarationList = innerBody->declarationList;

            while (innerDeclarationList != NULL) {
                if (innerDeclarationList->declaration->kind == declFuncK) {
                    return WEED_FUNC_HAS_NO_END;
                }

                innerDeclarationList = innerDeclarationList->next;
            }

            //We also want to check the names match
            char *thisHead = l->declaration->val.functionD.function->head->indentifier;
            char *thisTail = l->declaration->val.functionD.function->tail->indentifier;

            if (strcmp(thisHead, thisTail) != 0) {
                Error *e = NEW(Error);
                e->error = WEED_FUNC_HAS_NO_END;
                e->val.WEED_FUNC_HAS_NO_END_S.headId = thisHead;
                e->val.WEED_FUNC_HAS_NO_END_S.lineno = l->declaration->val.functionD.function->tail->lineno;

                return e;
            }
        }

        l = l->next;
    }

    return NULL;
/*
    //We count the functions
    size_t fncCount = 0;
    while (l != NULL) {
        if (l->declaration->kind == declFuncK) {
            fncCount++;
        }

        l = l->next;
    }

    //Then we construct a lookup map
    ConstMap *lm = initMap(fncCount);

    //Now we traverse and insert elements
    l = body->declarationList;
    while (l != NULL) {
        if (l->declaration->kind == declFuncK) {
            size_t firstLine;
            size_t lastLine;
            Body* localBody = l->declaration->val.functionD.function->body;

            // @TODO
            //Clean code up with local variables
            if (localBody->declarationList == NULL) {
                firstLine = localBody->statementList->statement->lineno;
            } else if (localBody->statementList == NULL) {
                firstLine = localBody->declarationList->declaration->lineno;
            } else if(localBody->declarationList->declaration->lineno < localBody->statementList->statement->lineno) {
                firstLine = localBody->declarationList->declaration->lineno;
            } else {
                firstLine = localBody->statementList->statement->lineno;
            }

            //Traverse the lists to the end
            StatementList *localStatementList = body->statementList;
            DeclarationList *localDeclarationList = body->declarationList;

            while (localStatementList != NULL) {
                localStatementList = localStatementList->next;
            }

            while (localDeclarationList != NULL) {
                localDeclarationList = localDeclarationList->next;
            }

            // @TODO
            //Clean code up with local variables and fix lineno
            if (localBody->declarationList == NULL) {
                firstLine = localBody->statementList->lineno;
            } else if (localBody->statementList == NULL) {
                firstLine = localBody->declarationList->lineno;
            } else if(localDeclarationList->lineno > localStatementList->lineno) {
                lastLine = localDeclarationList;
            } else {
                lastLine = localStatementList;
            }

            FunctionInfo *v = NEW(FunctionInfo);
            char *id = l->declaration->val.functionD.function->head->indentifier;

            Key *k = NEW(Key);

            k->keyTypes = eCK;
            k->key.cK = id;

            insert(lm, k, v);

            printf("Inserting %s, with lines %zu first and secondly %zu\n", id, firstLine, lastLine);
        }

        l = l->next;
    }

    //With our constructed map we can look for collisions easily
*/
}

Error *weed_function_return(Body *body) {
    return NULL;
}

