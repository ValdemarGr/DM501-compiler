//
// Created by valde on 2/23/19.
//

#ifndef HELLO_ERROR_H
#define HELLO_ERROR_H

#include <stdlib.h>
#include <stdio.h>
#include "../ast/tree.h"
#include "../scan_parse/pretty.h"
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
        SYMBOL_COULD_NOT_SCOPE
    } error;
    union {
        struct { char* headId; int lineno; } WEED_FUNC_HAS_NO_END_S;
        struct { char* fid; } WEED_FUNC_HAS_NO_RETURN_S;
        struct { struct Expression *expThatCausedError; int lineno; TypeKind expectedType; } TYPE_EXPRESSION_IS_NOT_AS_EXPECTED_S;
        struct { char* fid; int lineno;} TYPE_TERM_FUNCTION_NOT_FOUND_S;
        struct { char* fid; int lineno;} TYPE_TERM_IS_NOT_FUNCTION_S;
        struct { char* fid; int lineno;} TYPE_TERM_INVALID_FUNCTION_CALL_RETURN_TYPE_S;
        struct { char* fid; int lineno; TypeKind expectedType; TypeKind foundType; int argNumber;} TYPE_TERM_FUNCTION_CALL_EXPRESSION_NOT_MATCH_SIGNATURE_S;
        struct { char* fid; int lineno; int foundCount; int expectedCount;} TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH_S;
        struct { struct Term *termThatCausedError; int lineno; } TYPE_TERM_NOT_BOOLEAN_S;
        struct { struct Term *termThatCausedError; int lineno; } TYPE_TERM_NOT_INTEGER_S;
        struct { char *id; int lineno; } SYMBOL_NOT_FOUND_S;
        struct { char *id; int lineno; TypeKind expectedType; TypeKind foundType;} VARIABLE_UNEXPECTED_TYPE_S;
    } val;
} Error;

int writeError(Error *e);

#endif //HELLO_ERROR_H
