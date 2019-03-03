//
// Created by valde on 2/25/19.
//

#include "enum_to_string.h"
#include "../ast/tree.h"

char *typeToString(Type *type) {
    char *name;
    switch (type->kind) {
        case typeIdK:
            return type->val.idType.id;
        case typeLambdaK:
            typeToString(type->val.typeLambdaK.returnType);
            break;
        case typeClassK:
            name = (char*)malloc(sizeof(char) * (strlen(type->val.typeClass.classId) + strlen("class ") + 10));
            strcat(name, "class ");
            strcat(name, type->val.typeClass.classId);
            break;
        case typeGenericK:
            return "generic";
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
