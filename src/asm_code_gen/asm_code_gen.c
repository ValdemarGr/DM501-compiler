#include "asm_code_gen.h"
#include "../ast_to_ir/intermediate_representation.h"

int currentIndendation = 0;

void printIndentation(FILE *file) {
    for (int i = 0; i < currentIndendation; i++) {
        fprintf(file, "\t");
    }
}

char *getNextRegister(size_t reg) {
    switch (reg) {
        case 0: {
            return "rax";
        } break;
        case 1: {
            return "rcx";
        } break;
        case 2: {
            return "rdx";
        } break;
        case 3: {
            return "rbx";
        } break;
        case 4: {
            return "rsi";
        } break;
        case 5: {
            return "rdi";
        } break;
        case 6: {
            return "r8";
        } break;
        case 7: {
            return "r9";
        } break;
        case 8: {
            return "r10";
        } break;
        case 9: {
            return "r11";
        } break;
        case 10: {
            return "r12";
        } break;
        case 11: {
            return "r13";
        } break;
        case 12: {
            return "r14";
        } break;
        case 13: {
            return "r15";
        } break;
        default:
            return "INVALID_REGISTER";
            break;
    }
}

void generateInstruction(FILE *out, Instructions* instruction) {
    switch (instruction->kind) {
        case INSTRUCTION_ADD: {
            printIndentation(out);
            fprintf(out, "add %%%s, %%%s\n",
                    getNextRegister(instruction->val.arithmetic2.source),
                    getNextRegister(instruction->val.arithmetic2.dest));
        } break;
        case METADATA_BEGIN_BODY_BLOCK:
            currentIndendation++;
            //SKIP
            break;
        case METADATA_END_BODY_BLOCK:
            currentIndendation--;
            //SKIP
            break;
        case INSTRUCTION_PROGRAM_BEGIN:
            //SKIP
            break;
        case INSTRUCTION_FUNCTION_LABEL:
            fprintf(out, ".type %s, @function\n%s:\npush %%rbp\nmov %%rsp,%%rbp\n", instruction->val.functionHead.label, instruction->val.functionHead.label);
            printIndentation(out);
            fprintf(out, "leaq staticLink, %%%s\n",
                    getNextRegister(instruction->val.functionHead.temporary));
            printIndentation(out);
            fprintf(out, "movq %%rbp, %zu(%%%s)\n",
                    instruction->val.functionHead.distance * POINTER_SIZE,
                    getNextRegister(instruction->val.functionHead.temporary));
            break;
        case INSTRUCTION_VAR:{
            //SYMBOL *var = instruction->val.var;
            //fprintf(out, "; var #%zu with name %s and stack pos -%zu(%%rbp)\n", var->uniqueIdForScope, var->name, (var->uniqueIdForScope + 1) * POINTER_SIZE);
            //fprintf(out, "mov $0,%s\n", getUidLocation(instruction->val.var));
        } break;
        case INSTRUCTION_FUNCTION_END:
            //SKIP
            break;
        case INSTRUCTION_RETURN: {
            printIndentation(out);
            fprintf(out, "mov %%%s, %%rax\n", getNextRegister(instruction->val.tempToReturn));
            printIndentation(out);
            fprintf(out, "mov %%rbp,%%rsp\npop %%rbp\nret\n");
        } break;
        case METADATA_FUNCTION_ARGUMENT: {
            //fprintf(out, "; arg %zu at stack pos -%zu(%%rbp)\n", instruction->val.argNum, (instruction->val.argNum + 1) * POINTER_SIZE);
        } break;
        case INSTRUCTION_MINUS: {
            printIndentation(out);
            fprintf(out, "sub %%%s, %%%s\n",
                    getNextRegister(instruction->val.arithmetic2.source),
                    getNextRegister(instruction->val.arithmetic2.dest));
        } break;
        case INSTRUCTION_MUL: {

        } break;
        case INSTRUCTION_DIV: {

        } break;
        case INSTRUCTION_CONST: {
            printIndentation(out);
            fprintf(out, "mov $%i, %%%s\n",
                    instruction->val.constant.value,
                    getNextRegister(instruction->val.constant.temp));
        } break;
        case INSTRUCTION_WRITE: {

        } break;
        case INSTRUCTION_AND: {
            printIndentation(out);
            fprintf(out, "and %%%s, %%%s\n",
                    getNextRegister(instruction->val.arithmetic2.source),
            getNextRegister(instruction->val.arithmetic2.dest));
        } break;
        case INSTRUCTION_OR: {
            printIndentation(out);
            fprintf(out, "or %%%s, %%%s\n",
                    getNextRegister(instruction->val.arithmetic2.source),
                    getNextRegister(instruction->val.arithmetic2.dest));
        } break;
        case INSTRUCTION_PUSH: {
            printIndentation(out);
            fprintf(out, "push %%%s\n",
                    getNextRegister(instruction->val.tempToPush));
        } break;
        case INSTRUCTION_POP: {
            printIndentation(out);
            fprintf(out, "pop %%%s\n",
                    getNextRegister(instruction->val.tempToPopInto));
        } break;
        case INSTRUCTION_NEGATE: {
            printIndentation(out);
            fprintf(out, "neg %%%s\n",
                    getNextRegister(instruction->val.tempToNegate));
        } break;
        case INSTRUCTION_ABS: {

        } break;
        case INSTRUCTION_FUNCTION_CALL: {
            printIndentation(out);
            fprintf(out, "call %s\n",
                    instruction->val.function);
        } break;
        case COMPLEX_CONSTRAIN_BOOLEAN: {

        } break;
        case COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK_IN_SCOPE: {
            printIndentation(out);
            fprintf(out, "leaq staticLink, %%%s\n",
                    getNextRegister(instruction->val.loadTempFromParentScope.outputTemp));
            printIndentation(out);
            fprintf(out, "movq %zu(%%%s), %%%s\n",
                    instruction->val.loadTempFromParentScope.scopeToFindFrame * POINTER_SIZE,
                    getNextRegister(instruction->val.loadTempFromParentScope.outputTemp),
                    getNextRegister(instruction->val.loadTempFromParentScope.outputTemp));
            //We now have a pointer to the stack frame at temporary
            //We want to move the stack frame + offset into temporary
            printIndentation(out);
            fprintf(out, "movq -%zu(%%%s), %%%s\n",
                    instruction->val.loadTempFromParentScope.uniqueVariableId * POINTER_SIZE,
                    getNextRegister(instruction->val.loadTempFromParentScope.outputTemp),
                    getNextRegister(instruction->val.loadTempFromParentScope.outputTemp));
            //The pointer to the value is now in the temp
            printIndentation(out);
            fprintf(out, "movq (%%%s), %%%s\n",
                    getNextRegister(instruction->val.loadTempFromParentScope.outputTemp),
                    getNextRegister(instruction->val.loadTempFromParentScope.outputTemp));
        } break;
        case COMPLEX_ALLOCATE: {

        } break;
        case METADATA_CREATE_MAIN: {
            printIndentation(out);
            fprintf(out, ASM_HEADER);
            currentIndendation++;
        } break;
        case COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK: {
            SYMBOL *var = instruction->val.ptrLoad.var;
            printIndentation(out);
            fprintf(out, "movq -%zu(%%rbp), %%%s\n",
                    (var->uniqueIdForScope + 1) * POINTER_SIZE,
                    getNextRegister(instruction->val.ptrLoad.temporary));
        } break;
        case COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER: {
            printIndentation(out);
            fprintf(out, "mov %%%s, -%zu(%%rbp)\n",
                    getNextRegister(instruction->val.ptrSave.tempValue),
                    (instruction->val.ptrSave.sym->uniqueIdForScope + 1) * POINTER_SIZE);
        } break;
        case COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER_IN_SCOPE: {
            printIndentation(out);
            fprintf(out, "leaq staticLink, %%%s\n",
                    getNextRegister(instruction->val.saveTempFromParentScope.intermediateTemp));
            printIndentation(out);
            fprintf(out, "movq %zu(%%%s), %%%s\n",
                    instruction->val.saveTempFromParentScope.scopeToFindFrame * POINTER_SIZE,
                    getNextRegister(instruction->val.saveTempFromParentScope.intermediateTemp),
                    getNextRegister(instruction->val.saveTempFromParentScope.intermediateTemp));
            //We now have a pointer to the stack frame at temporary
            //We want to move the stack frame + offset into temporary
            printIndentation(out);
            fprintf(out, "movq -%zu(%%%s), %%%s\n",
                    instruction->val.saveTempFromParentScope.uniqueVariableId * POINTER_SIZE,
                    getNextRegister(instruction->val.saveTempFromParentScope.intermediateTemp),
                    getNextRegister(instruction->val.saveTempFromParentScope.intermediateTemp));
            //The pointer to the value is now in the temp
            printIndentation(out);
            fprintf(out, "movq %%%s, (%%%s)\n",
                    getNextRegister(instruction->val.saveTempFromParentScope.inputTemp),
                    getNextRegister(instruction->val.saveTempFromParentScope.intermediateTemp));
        } break;
        case INSTRUCTION_RIGHT_SHIFT: {
            printIndentation(out);
            fprintf(out, "sar $%zu, %%%s\n",
                    instruction->val.rightShift.constant,
                    getNextRegister(instruction->val.rightShift.temp));
        } break;
        case INSTRUCTION_XOR: {
            printIndentation(out);
            fprintf(out, "xor %%%s, %%%s\n",
                    getNextRegister(instruction->val.arithmetic2.source),
                    getNextRegister(instruction->val.arithmetic2.dest));
        } break;
        case INSTRUCTION_COPY: {
            printIndentation(out);
            fprintf(out, "mov %%%s, %%%s\n",
                    getNextRegister(instruction->val.arithmetic2.source),
                    getNextRegister(instruction->val.arithmetic2.dest));
        } break;
        case COMPLEX_SAVE_STATIC_LINK: {
            printIndentation(out);
            fprintf(out, "leaq staticLink, %%%s\n",
                    getNextRegister(instruction->val.pushPopStaticLink.temporary));
            printIndentation(out);
            fprintf(out, "push %zu(%%%s)\n",
                    instruction->val.pushPopStaticLink.staticLinkDepth * POINTER_SIZE,
                    getNextRegister(instruction->val.pushPopStaticLink.temporary));
        } break;
        case COMPLEX_RESTORE_STATIC_LINK: {
            printIndentation(out);
            fprintf(out, "leaq staticLink, %%%s\n",
                    getNextRegister(instruction->val.pushPopStaticLink.temporary));
            printIndentation(out);
            fprintf(out, "pop %zu(%%%s)\n",
                    instruction->val.pushPopStaticLink.staticLinkDepth * POINTER_SIZE,
                    getNextRegister(instruction->val.pushPopStaticLink.temporary));
        } break;
    }
}

void generateScopeFrames(FILE *file) {
    fprintf(file, ".section .data\n");
    fprintf(file, "staticLink:\n");
    fprintf(file, "\t.space %zu\n", (maxDistFromRoot + 1) * POINTER_SIZE);
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