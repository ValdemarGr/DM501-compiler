//
// Created by valde on 4/18/19.
//

#include "sizes.h"
#include "../ast/tree.h"

size_t getSizeForType(Type *tpe) {
    switch (tpe->kind) {
        case typeIdK: {
            //:( TODO
        } break;
        case typeIntK: {
            return INTEGER_SIZE;
        } break;
        case typeBoolK: {
            return 1;
        } break;
        case typeArrayK: {
            //:( TODO
        } break;
        case typeRecordK: {
            size_t accumulator = 0;

            VarDelList *varDelList = tpe->val.recordType.types;

            while (varDelList != NULL) {

                accumulator = accumulator + getSizeForType(varDelList->type);

                varDelList = varDelList->next;
            }

            return accumulator;
        } break;
        case typeLambdaK: {
            //:( TODO
        } break;
        case typeClassK: {
            //:( TODO
        } break;
        case typeGenericK: {
            //:( TODO
        } break;
    }
}

