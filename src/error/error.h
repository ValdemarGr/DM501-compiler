//
// Created by valde on 2/23/19.
//

#ifndef HELLO_ERROR_H
#define HELLO_ERROR_H

#include <stdlib.h>
#include <stdio.h>
#include "../ast/tree.h"
#include "../pretty_print/pretty.h"
#include "../utils/enum_to_string.h"

typedef struct Error {
    enum {
        WEED_FUNC_HAS_NO_END,
        WEED_FUNC_HAS_NO_RETURN,
        TYPE_EXPRESSION_IS_NOT_AS_EXPECTED,
        TYPE_TERM_FUNCTION_NOT_FOUND,
        TYPE_TERM_IS_NOT_FUNCTION,
        TYPE_TERM_INVALID_FUNCTION_CALL_RETURN_TYPE,
        TYPE_TERM_FUNCTION_CALL_EXPRESSION_NOT_MATCH_SIGNATURE,
        TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH,
        TYPE_TERM_NOT_BOOLEAN,
        TYPE_TERM_NOT_INTEGER,
        SYMBOL_NOT_FOUND,
        VARIABLE_UNEXPECTED_TYPE,
        VARIABLE_UNEXPECTED_CLASS,
        VARIABLE_COULD_NOT_FIND_RECORD_ITEM,
        SYMBOL_COULD_NOT_SCOPE,
        ILLEGAL_DOWNCAST,
        VALUE_IS_NULL,
        NULL_COMPARISON,
        CONST_REASSIGNMENT,
        INVALID_ASSIGMENT_TO_TYPE,
        INVALID_ASSIGMENT_TO_NULL,
        INVALID_TYPE,
        NOT_CLASS,
        TOO_MANY_GENERICS,
        TOO_FEW_GENERICS,
        CLASS_NOT_EXTENDED,
        NOT_TYPE,
        DECLARATIONS_IN_CLASS,
        RETURN_IN_VOID_LAMBDA
    } error;
    union {
        struct { char* headId; int lineno; } WEED_FUNC_HAS_NO_END_S;
        struct { char* fid; } WEED_FUNC_HAS_NO_RETURN_S;
        struct { struct Expression *expThatCausedError; int lineno; TypeKind expectedType; TypeKind expressionType; } TYPE_EXPRESSION_IS_NOT_AS_EXPECTED_S;
        struct { char* fid; int lineno;} TYPE_TERM_FUNCTION_NOT_FOUND_S;
        struct { char* fid; int lineno;} TYPE_TERM_IS_NOT_FUNCTION_S;
        struct { char* fid; int lineno;} TYPE_TERM_INVALID_FUNCTION_CALL_RETURN_TYPE_S;
        struct { char* fid; int lineno; TypeKind expectedType; TypeKind foundType; int argNumber;} TYPE_TERM_FUNCTION_CALL_EXPRESSION_NOT_MATCH_SIGNATURE_S;
        struct { char* fid; int lineno; int foundCount; int expectedCount;} TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH_S;
        struct { struct Term *termThatCausedError; int lineno; } TYPE_TERM_NOT_BOOLEAN_S;
        struct { struct Term *termThatCausedError; int lineno; } TYPE_TERM_NOT_INTEGER_S;
        struct { char *id; int lineno; } SYMBOL_NOT_FOUND_S;
        struct { char *id; int lineno; TypeKind expectedType; TypeKind foundType;} VARIABLE_UNEXPECTED_TYPE_S;
        struct { char *id; int lineno; char *expectedClass; char *foundClass;} VARIABLE_UNEXPECTED_CLASS_S;
        struct { int lineno; } VARIABLE_COULD_NOT_FIND_RECORD_ITEM_S;
        struct { char *idTo; char *idFrom; int lineno; } ILLEGAL_DOWNCAST;
        struct { char *idTo; int lineno; } NULL_POINTER_EXCEPTION;
        struct { int lineno; } VALUE_IS_NULL;
        struct { int lineno; } NULL_COMPARISON;
        struct { char *id; int lineno; } CONST_REASSIGNMENT;
        struct { char *id; int lineno; } INVALID_ASSIGMENT_TO_TYPE;
        struct { char *id; int lineno; } INVALID_ASSIGMENT_TO_NULL;
        struct { char *id; int lineno; } INVALID_TYPE;
        struct { char *id; int lineno; } NOT_CLASS;
        struct { char *id; int lineno; } TOO_MANY_GENERICS;
        struct { char *id; int lineno; } TOO_FEW_GENERICS;
        struct { char *id; int lineno; } CLASS_NOT_EXTENDED;
        struct { char *id; int lineno; } NOT_TYPE;
        struct { char *classId; int lineno; } DECLARATIONS_IN_CLASS;
        struct { TypeKind foundReturnType; int lineno; } RETURN_IN_VOID_LAMBDA;
    } val;
} Error;

int writeError(Error *e);

#endif //HELLO_ERROR_H
