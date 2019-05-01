//
// Created by valde on 2/23/19.
//
#include "error.h"

int writeError(Error *e) {
    if (e == NULL) {
        return 0;
    }

    switch (e->error) {
        case WEED_FUNC_HAS_NO_END:
            fprintf(stderr, "Function \"%s\" has no end. ", e->val.WEED_FUNC_HAS_NO_END_S.headId);
            fprintf(stderr, "Did you mean %s at line %i?\n",
                   e->val.WEED_FUNC_HAS_NO_END_S.headId,
                   e->val.WEED_FUNC_HAS_NO_END_S.lineno);
            break;
        case WEED_FUNC_HAS_NO_RETURN:
            fprintf(stderr, "Function \"%s\" does not always/never return\n", e->val.WEED_FUNC_HAS_NO_RETURN_S.fid);
            break;
        case SYMBOL_COULD_NOT_SCOPE:
            fprintf(stderr, "Could not scope table\n");
            break;
        case TYPE_EXPRESSION_IS_NOT_AS_EXPECTED:
            fprintf(stderr, "Type error at %i, expected type is %s, expression: ",
                    e->val.TYPE_EXPRESSION_IS_NOT_AS_EXPECTED_S.lineno,
                    typeEnumToString(e->val.TYPE_EXPRESSION_IS_NOT_AS_EXPECTED_S.expectedType));
            prettyEXP(e->val.TYPE_EXPRESSION_IS_NOT_AS_EXPECTED_S.expThatCausedError, NULL);
            fprintf(stderr, " is of type %s", typeEnumToString(e->val.TYPE_EXPRESSION_IS_NOT_AS_EXPECTED_S.expressionType));
            fprintf(stderr, "\n");

            break;
        case TYPE_TERM_FUNCTION_NOT_FOUND:
            fprintf(stderr, "Function could not be found %s, at line number %i\n",
                    e->val.TYPE_TERM_FUNCTION_NOT_FOUND_S.fid,
                   e->val.TYPE_TERM_FUNCTION_NOT_FOUND_S.lineno);
            break;
        case TYPE_TERM_IS_NOT_FUNCTION:
            fprintf(stderr, "Term is not function %s, at line number %i\n",
                   e->val.TYPE_TERM_IS_NOT_FUNCTION_S.fid,
                   e->val.TYPE_TERM_IS_NOT_FUNCTION_S.lineno);
            break;
        case TYPE_TERM_INVALID_FUNCTION_CALL_RETURN_TYPE:
            fprintf(stderr, "Function call %s returns the incorrect type, at line number %i\n",
                   e->val.TYPE_TERM_INVALID_FUNCTION_CALL_RETURN_TYPE_S.fid,
                   e->val.TYPE_TERM_INVALID_FUNCTION_CALL_RETURN_TYPE_S.lineno);
            break;
        case TYPE_TERM_FUNCTION_CALL_EXPRESSION_NOT_MATCH_SIGNATURE:
            fprintf(stderr, "Function call %s does not match its signature, at line number %i\nArgument number %i. Expected type %s, found %s\n",
                   e->val.TYPE_TERM_FUNCTION_CALL_EXPRESSION_NOT_MATCH_SIGNATURE_S.fid,
                   e->val.TYPE_TERM_FUNCTION_CALL_EXPRESSION_NOT_MATCH_SIGNATURE_S.lineno,
                   e->val.TYPE_TERM_FUNCTION_CALL_EXPRESSION_NOT_MATCH_SIGNATURE_S.argNumber,
                   typeEnumToString(e->val.TYPE_TERM_FUNCTION_CALL_EXPRESSION_NOT_MATCH_SIGNATURE_S.expectedType),
                   typeEnumToString(e->val.TYPE_TERM_FUNCTION_CALL_EXPRESSION_NOT_MATCH_SIGNATURE_S.foundType));
            break;
        case TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH:
            fprintf(stderr, "Function call %s does not have the correct number of arguments. Expected %i found %i, at line number %i\n",
                   e->val.TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH_S.fid,
                   e->val.TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH_S.foundCount,
                   e->val.TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH_S.expectedCount,
                   e->val.TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH_S.lineno);
            break;
        case TYPE_TERM_NOT_BOOLEAN:
            fprintf(stderr, "Type error at %i, expected boolean, term: ",
                   e->val.TYPE_TERM_NOT_BOOLEAN_S.lineno);
            prettyTerm(e->val.TYPE_TERM_NOT_BOOLEAN_S.termThatCausedError, NULL);
            fprintf(stderr, "\n");
            break;
        case TYPE_TERM_NOT_INTEGER:
            fprintf(stderr, "Type error at %i, expected int, term: ",
                   e->val.TYPE_TERM_NOT_INTEGER_S.lineno);
            prettyTerm(e->val.TYPE_TERM_NOT_INTEGER_S.termThatCausedError, NULL);
            fprintf(stderr, "\n");
            break;
        case SYMBOL_NOT_FOUND:
            fprintf(stderr, "Symbol not found %s, at line number %i\n",
                   e->val.SYMBOL_NOT_FOUND_S.id,
                   e->val.SYMBOL_NOT_FOUND_S.lineno);
            break;
        case VARIABLE_UNEXPECTED_TYPE:
            fprintf(stderr, "Variable %s unexpected type at line number %i. Expected type %s, found type %s\n",
                   e->val.VARIABLE_UNEXPECTED_TYPE_S.id,
                   e->val.VARIABLE_UNEXPECTED_TYPE_S.lineno,
                   typeEnumToString(e->val.VARIABLE_UNEXPECTED_TYPE_S.expectedType),
                   typeEnumToString(e->val.VARIABLE_UNEXPECTED_TYPE_S.foundType));
            break;
        case VARIABLE_COULD_NOT_FIND_RECORD_ITEM:
            fprintf(stderr, "Record item could not be found at line %i\n",
                   e->val.VARIABLE_COULD_NOT_FIND_RECORD_ITEM_S.lineno);
            break;
        default:
            break;
        case VARIABLE_UNEXPECTED_CLASS:break;
        case ILLEGAL_DOWNCAST:break;
        case VALUE_IS_NULL:break;
        case NULL_COMPARISON:break;
        case CONST_REASSIGNMENT:break;
        case INVALID_ASSIGMENT_TO_TYPE:break;
        case INVALID_ASSIGMENT_TO_NULL:break;
        case INVALID_TYPE:break;
        case NOT_CLASS:break;
        case TOO_MANY_GENERICS:break;
        case TOO_FEW_GENERICS:break;
        case CLASS_NOT_EXTENDED:break;
        case NOT_TYPE:break;
        case DECLARATIONS_IN_CLASS:break;
    }

    return e->error;
}
