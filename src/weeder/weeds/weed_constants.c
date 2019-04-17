//
// Created by valde on 4/18/19.
//

#include "weed_constants.h"
#include "../../ast/tree.h"

Error *weedConstantsDeclaration(Declaration *declaration) {
    Error *e = NULL;

    switch (declaration->kind) {
        case declVarK: {
            //skip
        } break;
        case declVarsK: {
            //skip
        } break;
        case declTypeK: {
            //skip
        } break;
        case declFuncK: {
            return weedContants(declaration->val.functionD.function->body);
        } break;
        case declValK: {
            //skip
        } break;
        case declClassK: {
            Error *e = NULL;
            DeclarationList *declarationList = declaration->val.classD.declarationList;

            while (declarationList != NULL) {

                e = weedConstantsDeclaration(declarationList->declaration);
                if (e != NULL) return e;

                declarationList = declarationList->next;
            }

            return NULL;
        } break;
    }

    return NULL;
}

Error *weedContants(Body *body) {

    Error *e = NULL;
    DeclarationList *declarationList = body->declarationList;

    while (declarationList != NULL) {

        e = weedConstantsDeclaration(declarationList->declaration);
        if (e != NULL) return e;

        declarationList = declarationList->next;
    }

    return NULL;
}
