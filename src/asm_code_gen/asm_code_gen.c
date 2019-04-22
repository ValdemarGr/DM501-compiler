#include "asm_code_gen.h"
#include "../ast_to_ir/intermediate_representation.h"

void generateInstruction(FILE *out, Instructions* instruction) {
    switch (instruction->kind) {
        case INSTRUCTION_ADD: {
            fprintf(out, "add TEMPORARY#%zu, TEMPORARY#%zu\n", instruction->val.arithmetic2.source, instruction->val.arithmetic2.dest);
        } break;
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
            fprintf(out, ".type %s, @function\n%s:\npush %%rbp\nmov %%rsp,%%rbp\n", instruction->val.label, instruction->val.label);
            break;
        case INSTRUCTION_VAR:{
            SYMBOL *var = instruction->val.var;
            fprintf(out, "; var #%zu with name %s and stack pos -%zu(%%rbp)\n", var->uniqueIdForScope, var->name, (var->uniqueIdForScope + 1) * POINTER_SIZE);
            //fprintf(out, "mov $0,%s\n", getUidLocation(instruction->val.var));
        } break;
        case INSTRUCTION_FUNCTION_END:
            //SKIP
            break;
        case INSTRUCTION_RETURN: {
            fprintf(out, "mov TEMPORARY#%zu, %%rax\n", instruction->val.tempToReturn);
            fprintf(out, "mov %%rbp,%%rsp\npop %%rbp\nret\n");
        } break;
        case METADATA_FUNCTION_ARGUMENT: {
            fprintf(out, "; arg %zu at stack pos -%zu(%%rbp)\n", instruction->val.argNum, (instruction->val.argNum + 1) * POINTER_SIZE);
        } break;
        case INSTRUCTION_MINUS: {
            fprintf(out, "sub TEMPORARY#%zu, TEMPORARY#%zu\n", instruction->val.arithmetic2.source, instruction->val.arithmetic2.dest);
        } break;
        case INSTRUCTION_MUL: {

        } break;
        case INSTRUCTION_DIV: {

        } break;
        case INSTRUCTION_CONST: {
            fprintf(out, "mov $%i, TEMPORARY#%zu\n", instruction->val.constant.value, instruction->val.constant.temp);
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
        case COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK_IN_SCOPE: {
            fprintf(out, "movq scopeFrames+%zu(%%rip), TEMPORARY#%zu\n",
                    instruction->val.loadTempFromParentScope.scopeToFindFrame * POINTER_SIZE,
                    instruction->val.loadTempFromParentScope.outputTemp);
            //We now have a pointer to the stack frame at temporary
            //We want to move the stack frame + offset into temporary
            fprintf(out, "movq -%zu(TEMPORARY#%zu), TEMPORARY#%zu\n",
                    instruction->val.loadTempFromParentScope.uniqueVariableId * POINTER_SIZE,
                    instruction->val.loadTempFromParentScope.outputTemp,
                    instruction->val.loadTempFromParentScope.outputTemp);
            //The pointer to the value is now in the temp
            fprintf(out, "mov (TEMPORARY#%zu), TEMPORARY#%zu\n",
                    instruction->val.loadTempFromParentScope.outputTemp,
                    instruction->val.loadTempFromParentScope.outputTemp);
        } break;
        case COMPLEX_ALLOCATE: {

        } break;
        case METADATA_CREATE_MAIN: {
            fprintf(out, ASM_HEADER);
        } break;
        case COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK: {
            SYMBOL *var = instruction->val.ptrLoad.var;
            fprintf(out, "movq -%zu(%%rbp), TEMPORARY#%zu\n", (var->uniqueIdForScope + 1) * POINTER_SIZE, instruction->val.ptrLoad.temporary);
        } break;
        case COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER: {
            fprintf(out, "mov TEMPORARY#%zu, -%zu(%%rbp)\n", instruction->val.ptrSave.tempValue, (instruction->val.ptrSave.sym->uniqueIdForScope + 1) * POINTER_SIZE);
        } break;
        case COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER_IN_SCOPE: {
            fprintf(out, "movq scopeFrames+%zu(%%rip), TEMPORARY#%zu\n",
                    instruction->val.saveTempFromParentScope.scopeToFindFrame * POINTER_SIZE,
                    instruction->val.saveTempFromParentScope.intermediateTemp);
            //We now have a pointer to the stack frame at temporary
            //We want to move the stack frame + offset into temporary
            fprintf(out, "movq -%zu(TEMPORARY#%zu), TEMPORARY#%zu\n",
                    instruction->val.saveTempFromParentScope.uniqueVariableId * POINTER_SIZE,
                    instruction->val.saveTempFromParentScope.intermediateTemp,
                    instruction->val.saveTempFromParentScope.intermediateTemp);
            //The pointer to the value is now in the temp
            fprintf(out, "mov TEMPORARY#%zu, (TEMPORARY#%zu)\n",
                    instruction->val.saveTempFromParentScope.inputTemp,
                    instruction->val.saveTempFromParentScope.intermediateTemp);
        } break;
        case INSTRUCTION_RIGHT_SHIFT: {
            fprintf(out, "sar TEMPORARY#%zu, TEMPORARY#%zu\n", instruction->val.arithmetic2.source, instruction->val.arithmetic2.dest);
        } break;
        case INSTRUCTION_XOR: {
            fprintf(out, "xor TEMPORARY#%zu, TEMPORARY#%zu\n", instruction->val.arithmetic2.source, instruction->val.arithmetic2.dest);
        } break;
        case INSTRUCTION_COPY: {
            fprintf(out, "mov TEMPORARY#%zu, TEMPORARY#%zu\n", instruction->val.arithmetic2.source, instruction->val.arithmetic2.dest);
        } break;
    }
}

void generateScopeFrames(FILE *file) {
    fprintf(file, "scopeFrames:\n");
    for (int i = 0; i < maxDistFromRoot + 1; ++i) {
        fprintf(file, ".quad 0\n");
    }
}

void generate(FILE *file, Instructions* instructions) {
    generateScopeFrames(file);

    Instructions* current_instruction = instructions;
    while (current_instruction != NULL) {
        generateInstruction(file, current_instruction);
        current_instruction = current_instruction->next;
    }

    fprintf(file, ASM_TAIL);
}