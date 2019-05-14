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
            return type->val.typeClass.classId;
            break;
        case typeGenericK:
            return type->val.typeGeneric.genericName;
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

char *variableToString(Variable *variable) {
    switch (variable->kind) {
        case varIdK: {
            return variable->val.idD.id;
        } break;
        case arrayIndexK: {
            char *basename = variableToString(variable->val.arrayIndexD.var);

            char *newName = malloc(sizeof(char) * (4 + strlen(basename)));

            sprintf(newName, "::%s::", basename);

            return newName;
        } break;
        case recordLookupK: {
            char *basename = variableToString(variable->val.recordLookupD.var);

            char *newName = malloc(sizeof(char) * (1 + strlen(basename) + strlen(variable->val.recordLookupD.id)));

            sprintf(newName, "%s.%s", basename, variable->val.recordLookupD.id);

            return newName;
        } break;
    }
}