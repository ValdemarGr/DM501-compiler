//
// Created by valde on 4/21/19.
//

#include "register_allocator.h"
#include "../ast_to_ir/intermediate_representation.h"

void simpleRegisterAllocation(Instructions *head) {
    Instructions *iter = head;

    int i = 0;

    while (iter != NULL) {

        switch (iter->kind) {
            case INSTRUCTION_ADD: {

            } break;
            case INSTRUCTION_MINUS: {

            } break;
            case INSTRUCTION_MUL: {

            } break;
            case INSTRUCTION_DIV: {

            } break;
            case INSTRUCTION_CONST: {

            } break;
            case INSTRUCTION_PROGRAM_BEGIN: {

            } break;
            case INSTRUCTION_FUNCTION_LABEL: {

            } break;
            case INSTRUCTION_VAR: {

            } break;
            case INSTRUCTION_FUNCTION_END: {

            } break;
            case INSTRUCTION_RETURN: {

            } break;
            case INSTRUCTION_WRITE: {

            } break;
            case INSTRUCTION_AND: {

            } break;
            case INSTRUCTION_OR: {

            } break;
            case INSTRUCTION_PUSH: {

            } break;
            case INSTRUCTION_POP: {

            } break;
            case INSTRUCTION_NEGATE: {

            } break;
            case INSTRUCTION_ABS: {

            } break;
            case INSTRUCTION_FUNCTION_CALL: {

            } break;
            case INSTRUCTION_RIGHT_SHIFT: {

            } break;
            case INSTRUCTION_XOR: {

            } break;
            case INSTRUCTION_COPY: {

            } break;
            case COMPLEX_CONSTRAIN_BOOLEAN: {

            } break;
            case COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK_IN_SCOPE: {

            } break;
            case COMPLEX_ALLOCATE: {

            } break;
            case COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK: {

            } break;
            case COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER: {

            } break;
            case COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER_IN_SCOPE: {

            } break;
            case METADATA_BEGIN_BODY_BLOCK: {

            } break;
            case METADATA_END_BODY_BLOCK: {

            } break;
            case METADATA_FUNCTION_ARGUMENT: {

            } break;
            case METADATA_CREATE_MAIN: {

            } break;
        }

        iter = iter->next;
    }
}
