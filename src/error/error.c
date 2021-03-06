//
// Created by valde on 2/23/19.
//
#include "error.h"

int writeError(Error *e) {
    if (e == NULL) {
        return 0;
    }

    fprintf(stderr, "%s:%d.%d-%d.%d: ", e->location.filename,
            e->location.first_line, e->location.first_column, e->location.last_line,
            e->location.last_column);

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
            fprintf(stderr, "Type error, expected type %s, expression: ",
                    typeEnumToString(e->val.TYPE_EXPRESSION_IS_NOT_AS_EXPECTED_S.expectedType));
            prettyEXP(e->val.TYPE_EXPRESSION_IS_NOT_AS_EXPECTED_S.expThatCausedError, NULL);
            fprintf(stderr, " is of type %s", typeEnumToString(e->val.TYPE_EXPRESSION_IS_NOT_AS_EXPECTED_S.expressionType));
            fprintf(stderr, "\n");

            break;
        case TYPE_TERM_FUNCTION_NOT_FOUND:
            fprintf(stderr, "Function could not be found %s\n",
                    e->val.TYPE_TERM_FUNCTION_NOT_FOUND_S.fid);
            break;
        case TYPE_TERM_IS_NOT_FUNCTION:
            fprintf(stderr, "'%s' is not a function\n",
                   e->val.TYPE_TERM_IS_NOT_FUNCTION_S.fid);
            break;
        case TYPE_TERM_INVALID_FUNCTION_CALL_RETURN_TYPE:
            fprintf(stderr, "Function call %s returns type %s, type %s expected\n",
                   e->val.TYPE_TERM_INVALID_FUNCTION_CALL_RETURN_TYPE_S.fid,
                   typeToString(e->val.TYPE_TERM_INVALID_FUNCTION_CALL_RETURN_TYPE_S.foundType),
                   typeToString(e->val.TYPE_TERM_INVALID_FUNCTION_CALL_RETURN_TYPE_S.expectedType));
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
            fprintf(stderr, "Function call %s does not have the correct number of arguments. Expected %i found %i\n",
                   e->val.TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH_S.fid,
                   e->val.TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH_S.foundCount,
                   e->val.TYPE_TERM_FUNCTION_CALL_ARGUMENT_COUNT_NOT_MATCH_S.expectedCount);
            break;
        case TYPE_TERM_NOT_BOOLEAN:
            fprintf(stderr, "Type error, expected %s got boolean",
                   typeToString(e->val.TYPE_TERM_NOT_BOOLEAN_S.expectedType));
            break;
        case TYPE_TERM_NOT_INTEGER:
            fprintf(stderr, "Type error, expected %s got int",
                    typeToString(e->val.TYPE_TERM_NOT_INTEGER_S.expectedType));
            break;
        case SYMBOL_NOT_FOUND:
            fprintf(stderr, "Symbol not found %s\n",
                   e->val.SYMBOL_NOT_FOUND_S.id);
            break;
        case VARIABLE_UNEXPECTED_TYPE:
            fprintf(stderr, "Variable %s unexpected type. Expected type %s, found type %s\n",
                   e->val.VARIABLE_UNEXPECTED_TYPE_S.id,
                   typeToString(e->val.VARIABLE_UNEXPECTED_TYPE_S.expectedType),
                    typeToString(e->val.VARIABLE_UNEXPECTED_TYPE_S.foundType));
            break;
        case VARIABLE_COULD_NOT_FIND_RECORD_ITEM:
            fprintf(stderr, "Record item could not be found\n");
            break;
        case VARIABLE_UNEXPECTED_CLASS: {
            fprintf(stderr, "Unexpected class\n");
        } break;
        case ILLEGAL_DOWNCAST: {
            fprintf(stderr, "Illegal downcast\n");
        } break;
        case VALUE_IS_NULL: {
            fprintf(stderr, "Expression is null\n");
        } break;
        case NULL_COMPARISON: {
            fprintf(stderr, "Invalid null comparison\n");
        } break;
        case CONST_REASSIGNMENT: {
            fprintf(stderr, "Reassignment of immutable variable %s\n", e->val.CONST_REASSIGNMENT.id);
        } break;
        case INVALID_ASSIGMENT_TO_TYPE: {
            fprintf(stderr, "Invalid assignment of value to type\n");
        } break;
        case INVALID_ASSIGMENT_TO_NULL: {
            fprintf(stderr, "Variable %s of type %s cannot be assigned to null\n",
                    e->val.INVALID_ASSIGMENT_TO_NULL.id,
                    typeEnumToString(e->val.INVALID_ASSIGMENT_TO_NULL.typeKind));
        } break;
        case INVALID_TYPE: {
            fprintf(stderr, "Type %s is invalid\n",
                    e->val.INVALID_TYPE.id);
        } break;
        case NOT_CLASS: {
            fprintf(stderr, "Error: NOT_CLASS\n");
        } break;
        case TOO_MANY_GENERICS: {
            fprintf(stderr, "Error: TOO_MANY_GENERICS\n");
        } break;
        case TOO_FEW_GENERICS: {
            fprintf(stderr, "Error: TOO_FEW_GENERICS\n");
        } break;
        case CLASS_NOT_EXTENDED: {
            fprintf(stderr, "Error: CLASS_NOT_EXTENDED\n");
        } break;
        case NOT_TYPE: {
            fprintf(stderr, "Error: NOT_TYPE\n");
        } break;
        case DECLARATIONS_IN_CLASS: {
            fprintf(stderr, "Error: DECLARATIONS_IN_CLASS\n");
        } break;
        case RETURN_IN_VOID_LAMBDA: {
            fprintf(stderr, "Error: RETURN_IN_VOID_LAMBDA\n");
        } break;
        case NESTED_LAMBDA: {
            fprintf(stderr, "NESTED_LAMBDA\n");
        } break;
        case LAMBDA_CAPTURE_INVALID: {
            fprintf(stderr, "LAMBDA_CAPTURE_INVALID\n");
        } break;
        case INVALID_GENERIC_HAS_TYPE_CONSTRAINT: {
            fprintf(stderr, "INVALID_GENERIC_HAS_TYPE_CONSTRAINT\n");
        } break;
        case INVALID_ALLOCATE_TARGET: {
            fprintf(stderr, "Variable %%s is not an valid allocate target\n");
        } break;
        case NO_PRIMITIVE_GENERICS : {
            fprintf(stderr, "NO_PRIMITIVE_GENERICS\n");
        } break;
        case RECURSIVE_TYPEDEF: {
            fprintf(stderr, "Recursive type definition\n");
        } break;
        case RETURN_IN_MAIN: {
            fprintf(stderr, "Return statement is not allowed in main body\n");
        }
        default:
            break;
    }

    return e->error;
}
