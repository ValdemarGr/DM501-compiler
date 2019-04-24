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
            fprintf(out, "# INSTRUCTION_ADD\n");
            printIndentation(out);
            fprintf(out, "add %%%s, %%%s\n",
                    getNextRegister(instruction->val.arithmetic2.source),
                    getNextRegister(instruction->val.arithmetic2.dest));
        } break;
        case METADATA_BEGIN_BODY_BLOCK:
            fprintf(out, "# METADATA_BEGIN_BODY_BLOCK\n");
            currentIndendation++;
            //SKIP
            break;
        case METADATA_END_BODY_BLOCK:
            fprintf(out, "# METADATA_END_BODY_BLOCK\n");
            currentIndendation--;
            //SKIP
            break;
        case INSTRUCTION_PROGRAM_BEGIN:
            //SKIP
            break;
        case INSTRUCTION_FUNCTION_LABEL:
            fprintf(out, "# INSTRUCTION_FUNCTION_LABEL\n");
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
            fprintf(out, "# VAR %s\n", instruction->val.var->name);
//SYMBOL *var = instruction->val.var;
            //fprintf(out, "; var #%zu with name %s and stack pos -%zu(%%rbp)\n", var->uniqueIdForScope, var->name, (var->uniqueIdForScope + 1) * POINTER_SIZE);
            //fprintf(out, "mov $0,%s\n", getUidLocation(instruction->val.var));
        } break;
        case INSTRUCTION_FUNCTION_END:
            //SKIP
            break;
        case INSTRUCTION_RETURN: {
            fprintf(out, "# INSTRUCTION_RETURN\n");
            printIndentation(out);
            fprintf(out, "mov %%%s, %%rax\n", getNextRegister(instruction->val.tempToReturn));
            printIndentation(out);
            fprintf(out, "mov %%rbp,%%rsp\npop %%rbp\nret\n");
        } break;
        case METADATA_FUNCTION_ARGUMENT: {
            //fprintf(out, "; arg %zu at stack pos -%zu(%%rbp)\n", instruction->val.argNum, (instruction->val.argNum + 1) * POINTER_SIZE);
        } break;
        case INSTRUCTION_MINUS: {
            fprintf(out, "# INSTRUCTION_MINUS\n");
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
            fprintf(out, "# INSTRUCTION_CONST\n");
            printIndentation(out);
            fprintf(out, "mov $%i, %%%s\n",
                    instruction->val.constant.value,
                    getNextRegister(instruction->val.constant.temp));
        } break;
        case INSTRUCTION_WRITE: {
            fprintf(out, "# INSTRUCTION_WRITE\n");
            /*printIndentation(out);
            fprintf(out, "push %%rsi\n");
            printIndentation(out);
            fprintf(out, "push %%rdi\n");
            printIndentation(out);
            fprintf(out, "push %%rax\n");

            printIndentation(out);
            fprintf(out, "mov %%%s, %%rsi\n",
                    getNextRegister(instruction->val.tempToWrite));
            printIndentation(out);
            fprintf(out, "movl $.LC0, %%edi\n");
            printIndentation(out);
            fprintf(out, "movl $0, %%eax\n");
            printIndentation(out);
            fprintf(out, "call printf\n");

            printIndentation(out);
            fprintf(out, "pop %%rax\n");
            printIndentation(out);
            fprintf(out, "pop %%rdi\n");
            printIndentation(out);
            fprintf(out, "pop %%rsi\n");*/
            printIndentation(out);
            fprintf(out, "push %%%s\n",
                    getNextRegister(instruction->val.tempToWrite));
            printIndentation(out);
            fprintf(out, "#call print_number\n");
            printIndentation(out);
            fprintf(out, "pop %%%s\n",
                    getNextRegister(instruction->val.tempToWrite));
        } break;
        case INSTRUCTION_AND: {
            fprintf(out, "# INSTRUCTION_AND\n");
            printIndentation(out);
            fprintf(out, "and %%%s, %%%s\n",
                    getNextRegister(instruction->val.arithmetic2.source),
            getNextRegister(instruction->val.arithmetic2.dest));
        } break;
        case INSTRUCTION_OR: {
            fprintf(out, "# INSTRUCTION_OR\n");
            printIndentation(out);
            fprintf(out, "or %%%s, %%%s\n",
                    getNextRegister(instruction->val.arithmetic2.source),
                    getNextRegister(instruction->val.arithmetic2.dest));
        } break;
        case INSTRUCTION_PUSH: {
            fprintf(out, "# INSTRUCTION_PUSH\n");
            printIndentation(out);
            fprintf(out, "push %%%s\n",
                    getNextRegister(instruction->val.tempToPush));
        } break;
        case INSTRUCTION_POP: {
            fprintf(out, "# INSTRUCTION_POP\n");
            printIndentation(out);
            fprintf(out, "pop %%%s\n",
                    getNextRegister(instruction->val.tempToPopInto));
        } break;
        case INSTRUCTION_NEGATE: {
            fprintf(out, "# INSTRUCTION_NEGATE\n");
            printIndentation(out);
            fprintf(out, "neg %%%s\n",
                    getNextRegister(instruction->val.tempToNegate));
        } break;
        case INSTRUCTION_ABS: {

        } break;
        case INSTRUCTION_FUNCTION_CALL: {
            fprintf(out, "# INSTRUCTION_FUNCTION_CALL\n");
            printIndentation(out);
            fprintf(out, "call %s\n",
                    instruction->val.function);
        } break;
        case COMPLEX_CONSTRAIN_BOOLEAN: {

        } break;
        case COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK_IN_SCOPE: {
            fprintf(out, "# COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK_IN_SCOPE\n");
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
            fprintf(out, "# COMPLEX_ALLOCATE\n");
            printIndentation(out);
            fprintf(out, "push %%rdi\n");
            printIndentation(out);
            fprintf(out, "push %%rax\n");
            printIndentation(out);
            fprintf(out, "push %%r15\n");
            printIndentation(out);
            fprintf(out, "push %%r14\n");

            printIndentation(out);
            fprintf(out, "mov %%%s, %%r14\n",
                    getNextRegister(instruction->val.allocate.timesTemp));
            printIndentation(out);
            fprintf(out, "mov $0, %%rdi\n");
            printIndentation(out);
            fprintf(out, "mov $12, %%rax\n");
            printIndentation(out);
            fprintf(out, "syscall\n");
            printIndentation(out);
            fprintf(out, "push %%rax\n");
            printIndentation(out);
            fprintf(out, "mov $%zu, %%r15\n",
                    getSizeForType(instruction->val.allocate.tpe));
            printIndentation(out);
            fprintf(out, "imul %%r14, %%r15\n");
            printIndentation(out);
            fprintf(out, "add %%r15, %%rax\n");
            printIndentation(out);
            fprintf(out, "mov %%rax, %%rdi\n");
            printIndentation(out);
            fprintf(out, "mov $12, %%rax\n");
            printIndentation(out);
            fprintf(out, "syscall\n");
            printIndentation(out);
            fprintf(out, "pop %%%s\n",
                    getNextRegister(instruction->val.allocate.ptrTemp));
        } break;
        case COMPLEX_ALLOCATE_END : {
            fprintf(out, "# COMPLEX_ALLOCATE_END\n");
            printIndentation(out);
            fprintf(out, "pop %%r14\n");
            printIndentation(out);
            fprintf(out, "pop %%r15\n");
            printIndentation(out);
            fprintf(out, "pop %%rax\n");
            printIndentation(out);
            fprintf(out, "pop %%rdi\n");
        } break;
        case METADATA_CREATE_MAIN: {
            fprintf(out, "# METADATA_CREATE_MAIN\n");
            printIndentation(out);
            fprintf(out, ASM_HEADER);
            currentIndendation++;
        } break;
        case COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK: {
            fprintf(out, "# COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK\n");
            SYMBOL *var = instruction->val.ptrLoad.var;
            printIndentation(out);
            fprintf(out, "mov -%zu(%%rbp), %%%s\n",
                    (var->uniqueIdForScope + 1) * POINTER_SIZE,
                    getNextRegister(instruction->val.ptrLoad.temporary));
            printIndentation(out);
            fprintf(out, "mov (%%%s), %%%s\n",
                    getNextRegister(instruction->val.ptrLoad.temporary),
                    getNextRegister(instruction->val.ptrLoad.temporary));
        } break;
        case COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER: {
            //Move ptr into intermediate
            fprintf(out, "# COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER\n");
            printIndentation(out);
            fprintf(out, "mov -%zu(%%rbp), %%%s\n",
                    (instruction->val.ptrSave.sym->uniqueIdForScope + 1) * POINTER_SIZE,
                    getNextRegister(instruction->val.ptrSave.intermediate));
            printIndentation(out);
            fprintf(out, "mov %%%s, (%%%s)\n",
                    getNextRegister(instruction->val.ptrSave.tempValue),
                    getNextRegister(instruction->val.ptrSave.intermediate));
        } break;
        case COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER_IN_SCOPE: {
            fprintf(out, "# COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER_IN_SCOPE\n");
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
            fprintf(out, "# INSTRUCTION_RIGHT_SHIFT\n");
            printIndentation(out);
            fprintf(out, "sar $%zu, %%%s\n",
                    instruction->val.rightShift.constant,
                    getNextRegister(instruction->val.rightShift.temp));
        } break;
        case INSTRUCTION_XOR: {
            fprintf(out, "# INSTRUCTION_XOR\n");
            printIndentation(out);
            fprintf(out, "xor %%%s, %%%s\n",
                    getNextRegister(instruction->val.arithmetic2.source),
                    getNextRegister(instruction->val.arithmetic2.dest));
        } break;
        case INSTRUCTION_COPY: {
            fprintf(out, "# INSTRUCTION_COPY\n");
            printIndentation(out);
            fprintf(out, "mov %%%s, %%%s\n",
                    getNextRegister(instruction->val.arithmetic2.source),
                    getNextRegister(instruction->val.arithmetic2.dest));
        } break;
        case COMPLEX_SAVE_STATIC_LINK: {
            fprintf(out, "# COMPLEX_SAVE_STATIC_LINK\n");
            printIndentation(out);
            fprintf(out, "leaq staticLink, %%%s\n",
                    getNextRegister(instruction->val.pushPopStaticLink.temporary));
            printIndentation(out);
            fprintf(out, "push %zu(%%%s)\n",
                    instruction->val.pushPopStaticLink.staticLinkDepth * POINTER_SIZE,
                    getNextRegister(instruction->val.pushPopStaticLink.temporary));
        } break;
        case COMPLEX_RESTORE_STATIC_LINK: {
            fprintf(out, "# COMPLEX_RESTORE_STATIC_LINK\n");
            printIndentation(out);
            fprintf(out, "leaq staticLink, %%%s\n",
                    getNextRegister(instruction->val.pushPopStaticLink.temporary));
            printIndentation(out);
            fprintf(out, "pop %zu(%%%s)\n",
                    instruction->val.pushPopStaticLink.staticLinkDepth * POINTER_SIZE,
                    getNextRegister(instruction->val.pushPopStaticLink.temporary));
        } break;
        case COMPLEX_LOAD_POINTER_TO_STATIC_LINK_FRAME: {
            fprintf(out, "# COMPLEX_LOAD_POINTER_TO_STATIC_LINK_FRAME\n");
            //Fetch static link ptr
            printIndentation(out);
            fprintf(out, "leaq staticLink, %%%s\n",
                    getNextRegister(instruction->val.loadPtrToStaticLink.intermediateTemp));
            printIndentation(out);
            fprintf(out, "movq %zu(%%%s), %%%s\n",
                    instruction->val.loadPtrToStaticLink.scopeToFindFrame * POINTER_SIZE,
                    getNextRegister(instruction->val.loadPtrToStaticLink.intermediateTemp),
                    getNextRegister(instruction->val.loadPtrToStaticLink.intermediateTemp));
            printIndentation(out);
            fprintf(out, "movq %%%s, -%zu(%%%s)\n",
                    getNextRegister(instruction->val.loadPtrToStaticLink.ptrTemp),
                    (instruction->val.loadPtrToStaticLink.linkBaseOffset + 1) * POINTER_SIZE,
                    getNextRegister(instruction->val.loadPtrToStaticLink.intermediateTemp));
        } break;
    }
}

void generateScopeFrames(FILE *file) {
    fprintf(file, ".include \"print.asm\"\n");
    fprintf(file, ".section .data\n");
    fprintf(file, "staticLink:\n");
    fprintf(file, "\t.space %zu\n", (maxDistFromRoot + 1) * POINTER_SIZE);
    fprintf(file, ".LC0:\n\t.string \"%%i\"\n");
    fprintf(file, ".LC1:\n\t.string \"hello!\"\n");
    fprintf(file, ".section .text\n");
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