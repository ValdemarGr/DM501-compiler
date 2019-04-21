#include "asm_code_gen.h"
#include "../ast_to_ir/intermediate_representation.h"

void generateInstruction(FILE *out, Instructions* instruction) {
    switch (instruction->kind) {
        case INSTRUCTION_ADD:
            //
            break;
        case METADATA_BEGIN_BODY_BLOCK:
            //SKIP
            break;
        case METADATA_END_BODY_BLOCK:
            //SKIP
            break;
        case INSTRUCTION_PROGRAM_BEGIN:
            //SKIP
            break;
        case INSTRUCTION_FUNCTION_LABEL:
            fprintf(out, ".type %s, @function\n%s:\npush %%rbp\nmov %%rsp,%%rbp\npush %%rax\n", instruction->val.label, instruction->val.label);
            break;
        case INSTRUCTION_VAR:{
            SYMBOL *var = instruction->val.var;
            fprintf(out, "; var #%zu with stack pos -%zu(%%rbp)\n", var->uniqueIdForScope, (var->uniqueIdForScope + 1) * POINTER_SIZE);
            //fprintf(out, "mov $0,%s\n", getUidLocation(instruction->val.var));
        } break;
        case INSTRUCTION_FUNCTION_END:
            //SKIP
            break;
        case INSTRUCTION_RETURN: {
            fprintf(out, "mov TEMPORARY%zu, %rax\n", instruction->val.tempToReturn);
            fprintf(out, "mov %%rbp,%%rsp\npop %%rbp\nret\n");
        } break;
        case METADATA_FUNCTION_ARGUMENT:
            break;
        case INSTRUCTION_MINUS: {

        } break;
        case INSTRUCTION_MUL: {

        } break;
        case INSTRUCTION_DIV: {

        } break;
        case INSTRUCTION_CONST: {

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
        case COMPLEX_CONSTRAIN_BOOLEAN: {

        } break;
        case COMPLEX_FETCH_VARIABLE_FROM_PARENT_SCOPE_FRAME: {

        } break;
        case COMPLEX_ALLOCATE: {

        } break;
        case METADATA_CREATE_MAIN: {
            fprintf(out, ASM_HEADER);
        } break;
    }
}

void generate(FILE *file, Instructions* instructions) {
    Instructions* current_instruction = instructions;
    while (current_instruction != NULL && current_instruction->kind) {
        generateInstruction(file, current_instruction);
        current_instruction = current_instruction->next;
    }

    fprintf(file, ASM_TAIL);
}