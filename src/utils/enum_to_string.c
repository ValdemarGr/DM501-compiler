//
// Created by valde on 2/25/19.
//

#include "enum_to_string.h"

char *typeEnumToString(TypeKind typeKind) {
    switch (typeKind) {
        case typeIdK:
            return "id typed";
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
