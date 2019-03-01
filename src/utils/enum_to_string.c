//
// Created by valde on 2/25/19.
//

#include "enum_to_string.h"
#include "../ast/tree.h"

char *typeToString(Type *type) {
    switch (type->kind) {
        case typeIdK:
            return type->val.idType.id;
        case typeLambdaK:
            typeToString(type->val.typeLambdaK.returnType);
            break;
        default:
            return typeEnumToString(type->kind);
            break;
    }
}

char *typeEnumToString(TypeKind typeKind) {
    switch (typeKind) {
        case typeIdK:
            return "custom";
            break;
        case typeIntK:
            return "int";
            break;
        case typeBoolK:
            return "bool";
            break;
        case typeArrayK:
            return "array";
            break;
        case typeRecordK:
            return "record";
            break;
        default:
            break;
    }

    return "unknown";
}
