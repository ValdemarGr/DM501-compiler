#include "asm_code_gen.h"
#include "../ast_to_ir/intermediate_representation.h"

int currentIndendation = 0;
extern int initialGcSizeMB;
extern bool TURBOMODE;
ConstMap *placedBlocks = NULL;

void printIndentation(FILE *file) {
    for (int i = 0; i < currentIndendation; i++) {
        fprintf(file, "\t");
    }
}

char *getNextRegister(size_t reg) {
/*
    char *str = malloc(sizeof(char) * 12);
    sprintf(str, "%d", (int)reg);
    return str;
*/
    if (reg == 0) {
        return "rax";
    }

    switch (reg % 13) {
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
        case 0: {
            return "r15";
        } break;
        default:
            return "INVALID_REGISTER";
            break;
    }
}

int offsetForFunction;

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
            fprintf(out, "mov %%rbp,%%rsp\npop %%rbp\nret\n");
            currentIndendation--;
            //SKIP
            break;
        case INSTRUCTION_PROGRAM_BEGIN:
            //SKIP
            break;
        case INSTRUCTION_FUNCTION_LABEL:
            fprintf(out, "# INSTRUCTION_FUNCTION_LABEL\n");
            fprintf(out, ".type %s, @function\n%s:\npush %%rbp\nmov %%rbp, %%rax\nmov %%rsp,%%rbp\n", instruction->val.functionHead.label, instruction->val.functionHead.label);
            printIndentation(out);
            offsetForFunction = (int)length(instruction->val.functionHead.pointerSet) + 1 + 1;
            fprintf(out, "subq $%i, %%rsp\n",
                    16 + (
                                 (int)instruction->val.functionHead.tableForFunction->nextSymbolId +
                                 offsetForFunction) * POINTER_SIZE);

            printIndentation(out);
            fprintf(out, "movq %%rax, -8(%%rbp)\n");

            SortedSet *gcSet = instruction->val.functionHead.pointerSet;
            int len = (int)length(gcSet);
            printIndentation(out);
            fprintf(out, "movq $%i, -16(%%rbp)\n", len);

            SortedSet *iter = first(gcSet);
            int counter = 3;
            while (iter != NULL) {
                printIndentation(out);
                fprintf(out, "movq $%i, -%i(%%rbp)\n", iter->data, counter * POINTER_SIZE);
                printIndentation(out);
                fprintf(out, "movq $0, -%i(%%rbp)\n", (iter->data + offsetForFunction) * POINTER_SIZE);counter++;
                iter = iter->_next;
            }

            printIndentation(out);
            fprintf(out, "pushq %%%s\n",
                    getNextRegister(instruction->val.functionHead.temporary));
            printIndentation(out);
            fprintf(out, "leaq staticLink, %%%s\n",
                    getNextRegister(instruction->val.functionHead.temporary));
            printIndentation(out);
            fprintf(out, "movq %%rbp, %zu(%%%s)\n",
                    instruction->val.functionHead.distance * POINTER_SIZE,
                    getNextRegister(instruction->val.functionHead.temporary));

            printIndentation(out);
            fprintf(out, "popq %%%s\n",
                    getNextRegister(instruction->val.functionHead.temporary));

            printIndentation(out);
            fprintf(out, "pushq %%rcx\n");
            printIndentation(out);
            fprintf(out, "pushq %%rdx\n");
            printIndentation(out);
            fprintf(out, "pushq %%rbx\n");
            printIndentation(out);
            fprintf(out, "pushq %%rsi\n");
            printIndentation(out);
            fprintf(out, "pushq %%rdi\n");
            printIndentation(out);
            fprintf(out, "pushq %%r8\n");
            printIndentation(out);
            fprintf(out, "pushq %%r9\n");
            printIndentation(out);
            fprintf(out, "pushq %%r10\n");
            printIndentation(out);
            fprintf(out, "pushq %%r11\n");
            printIndentation(out);
            fprintf(out, "pushq %%r12\n");
            printIndentation(out);
            fprintf(out, "pushq %%r13\n");
            printIndentation(out);
            fprintf(out, "pushq %%r14\n");
            printIndentation(out);
            fprintf(out, "pushq %%r15\n");

            currentIndendation++;
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
            fprintf(out, "mov %%%s, %%rax\n",
                    getNextRegister(instruction->val.tempToReturn));

            printIndentation(out);
            fprintf(out, "popq %%r15\n");
            printIndentation(out);
            fprintf(out, "popq %%r14\n");
            printIndentation(out);
            fprintf(out, "popq %%r13\n");
            printIndentation(out);
            fprintf(out, "popq %%r12\n");
            printIndentation(out);
            fprintf(out, "popq %%r11\n");
            printIndentation(out);
            fprintf(out, "popq %%r10\n");
            printIndentation(out);
            fprintf(out, "popq %%r9\n");
            printIndentation(out);
            fprintf(out, "popq %%r8\n");
            printIndentation(out);
            fprintf(out, "popq %%rdi\n");
            printIndentation(out);
            fprintf(out, "popq %%rsi\n");
            printIndentation(out);
            fprintf(out, "popq %%rbx\n");
            printIndentation(out);
            fprintf(out, "popq %%rdx\n");
            printIndentation(out);
            fprintf(out, "popq %%rcx\n");

            printIndentation(out);
            fprintf(out, "mov %%rbp,%%rsp\npop %%rbp\nret\n");
        } break;
        case METADATA_FUNCTION_ARGUMENT: {
            fprintf(out, "# METADATA_FUNCTION_ARGUMENT\n");
            printIndentation(out);
            fprintf(out, "mov %zu(%%rbp), %%%s\n",
                    instruction->val.args.argNum * POINTER_SIZE + 16,
                    getNextRegister(instruction->val.args.moveReg));
            printIndentation(out);
            fprintf(out, "mov %%%s, -%zu(%%rbp)\n",
                    getNextRegister(instruction->val.args.moveReg),
                    (instruction->val.args.stackNum + 1 + offsetForFunction) * POINTER_SIZE);
        } break;
        case INSTRUCTION_MINUS: {
            fprintf(out, "# INSTRUCTION_MINUS\n");
            printIndentation(out);
            fprintf(out, "sub %%%s, %%%s\n",
                    getNextRegister(instruction->val.arithmetic2.source),
                    getNextRegister(instruction->val.arithmetic2.dest));
        } break;
        case INSTRUCTION_MUL: {
            fprintf(out, "# INSTRUCTION_MUL\n");
            printIndentation(out);
            fprintf(out, "imul %%%s, %%%s\n",
                    getNextRegister(instruction->val.arithmetic2.source),
                    getNextRegister(instruction->val.arithmetic2.dest));
        } break;
        case INSTRUCTION_DIV: {
            fprintf(out, "# INSTRUCTION_DIV\n");
            printIndentation(out);
            fprintf(out, "movq %%%s, %%rax\n",
                    getNextRegister(instruction->val.arithmetic2.source));

            printIndentation(out);
            fprintf(out, "pushq %%r10\n");

            printIndentation(out);
            fprintf(out, "movq %%%s, %%r10\n",
                    getNextRegister(instruction->val.arithmetic2.dest));
            printIndentation(out);
            fprintf(out, "pushq %%rdx\n");
            printIndentation(out);
            fprintf(out, "cqto\n");
            printIndentation(out);
            fprintf(out, "idiv %%r10\n");

            printIndentation(out);
            fprintf(out, "popq %%rdx\n");
            printIndentation(out);
            fprintf(out, "popq %%r10\n");
            printIndentation(out);

            fprintf(out, "movq %%rax, %%%s\n",
                    getNextRegister(instruction->val.arithmetic2.dest));
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

            printIndentation(out);
            fprintf(out, "pushq %%rsi\n");
            printIndentation(out);
            fprintf(out, "pushq %%rdi\n");

            printIndentation(out);
            fprintf(out, "movq %%%s, %%rsi\n",
                    getNextRegister(instruction->val.tempToWrite));
            printIndentation(out);
            fprintf(out, "movq $intprint, %%rdi\n");
            printIndentation(out);
            fprintf(out, "movq $0, %%rax\n");
            printIndentation(out);
            fprintf(out, "call printf\n");


            printIndentation(out);
            fprintf(out, "popq %%rdi\n");
            printIndentation(out);
            fprintf(out, "popq %%rsi\n");
        } break;
        case INSTRUCTION_WRITE_CHAR: {
            fprintf(out, "# INSTRUCTION_WRITE_CHAR\n");

            printIndentation(out);
            fprintf(out, "pushq %%rsi\n");
            printIndentation(out);
            fprintf(out, "pushq %%rdi\n");
            printIndentation(out);
            fprintf(out, "movq %%%s, %%rsi\n",
                    getNextRegister(instruction->val.tempToWrite));
            printIndentation(out);
            fprintf(out, "movq $charprint, %%rdi\n");
            printIndentation(out);
            fprintf(out, "movq $0, %%rax\n");
            printIndentation(out);
            fprintf(out, "call printf\n");
            printIndentation(out);
            fprintf(out, "popq %%rdi\n");
            printIndentation(out);
            fprintf(out, "popq %%rsi\n");
        } break;
        case INSTRUCTION_WRITE_NL: {
            fprintf(out, "# INSTRUCTION_WRITE_NL\n");

            printIndentation(out);
            fprintf(out, "pushq %%rsi\n");
            printIndentation(out);
            fprintf(out, "pushq %%rdi\n");
            printIndentation(out);
            fprintf(out, "movq $nlprint, %%rdi\n");
            printIndentation(out);
            fprintf(out, "movq $0, %%rax\n");
            printIndentation(out);
            fprintf(out, "call printf\n");
            printIndentation(out);
            fprintf(out, "popq %%rdi\n");
            printIndentation(out);
            fprintf(out, "popq %%rsi\n");
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
        case INSTRUCTION_PUSH_STACK: {
            fprintf(out, "# INSTRUCTION_PUSH_STACK\n");
            printIndentation(out);
            fprintf(out, "push -%zu(%%rbp)\n",
                    instruction->val.popPushStack.offset + offsetForFunction * POINTER_SIZE);
        } break;
        case INSTRUCTION_POP_STACK: {
            fprintf(out, "# INSTRUCTION_POP_STACK\n");
            printIndentation(out);
            fprintf(out, "pop -%zu(%%rbp)\n",
                    instruction->val.popPushStack.offset + offsetForFunction * POINTER_SIZE);
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
                    instruction->val.functionCall.function);
        } break;
        case COMPLEX_CONSTRAIN_BOOLEAN: {
            fprintf(out, "# COMPLEX_CONSTRAIN_BOOLEAN\n");
            printIndentation(out);
            fprintf(out, "push %%rax\n");
            printIndentation(out);
            fprintf(out, "cmp $0, %%%s\n",
                    getNextRegister(instruction->val.tempToConstrain));
            printIndentation(out);
            fprintf(out, "setg %%al\n");
            printIndentation(out);
            fprintf(out, "movsx %%al, %%%s\n",
                    getNextRegister(instruction->val.tempToConstrain));

            printIndentation(out);
            fprintf(out, "pop %%rax\n");
        } break;
            /*case COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK_IN_SCOPE: {
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
                        (instruction->val.loadTempFromParentScope.uniqueVariableId + 1) * POINTER_SIZE,
                        getNextRegister(instruction->val.loadTempFromParentScope.outputTemp),
                        getNextRegister(instruction->val.loadTempFromParentScope.outputTemp));
                //The pointer to the value is now in the temp
                printIndentation(out);
                fprintf(out, "movq (%%%s), %%%s\n",
                        getNextRegister(instruction->val.loadTempFromParentScope.outputTemp),
                        getNextRegister(instruction->val.loadTempFromParentScope.outputTemp));
            } break;*/
        case COMPLEX_ALLOCATE: {
            fprintf(out, "# COMPLEX_ALLOCATE\n");
            printIndentation(out);
            fprintf(out, "movq $%zu, %%%s\n",
                    instruction->val.allocate.eleSize,
                    getNextRegister(instruction->val.allocate.intermediate));
            printIndentation(out);
            fprintf(out, "imulq %%%s, %%%s\n",
                    getNextRegister(instruction->val.allocate.timesTemp),
                    getNextRegister(instruction->val.allocate.intermediate));
            /*

            printIndentation(out);
            printIndentation(out);
            fprintf(out, "mov $%zu, %%r15\n",
                    instruction->val.allocate.eleSize);

            printIndentation(out);
            fprintf(out, "push %%rdi\n");
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
                    instruction->val.allocate.eleSize);
            //We save the the amount of allocations for later
            printIndentation(out);
            fprintf(out, "push %%r14\n");
            printIndentation(out);
            fprintf(out, "imul %%r14, %%r15\n");*/

            SortedSet *gcSet = instruction->val.allocate.pointerSet;
            int extraSpace = 1 + 1 + (int)length(gcSet);
            switch (instruction->val.allocate.allocationType) {
                case ALLOC_RECORD_CLASS: {
                    fprintf(out, "# ALLOC_RECORD_CLASS\n");
                    //1 slot for data size, 1 for ptr count and |ptrCount|

                    printIndentation(out);
                    fprintf(out, "addq $%i, %%%s\n", extraSpace * POINTER_SIZE, getNextRegister(instruction->val.allocate.intermediate));
                } break;
                case ALLOC_ARR_OF_PTR: {
                    fprintf(out, "# ALLOC_ARR_OF_PTR\n");
                    //1 for head 1 for tail
                    printIndentation(out);
                    fprintf(out, "addq $%i, %%%s\n", extraSpace * POINTER_SIZE, getNextRegister(instruction->val.allocate.intermediate));
                } break;
                case ALLOC_ARR_OF_PRIM: {
                    fprintf(out, "# ALLOC_ARR_OF_PRIM\n");
                    //1 for head 1 for tail
                    printIndentation(out);
                    fprintf(out, "addq $%i, %%%s\n", 2 * POINTER_SIZE, getNextRegister(instruction->val.allocate.intermediate));
                } break;
                case ALLOC_LAMBDA: {
                    fprintf(out, "# ALLOC_LAMBDA\n");
                    //1 for head 1 for class ptr 1 for self indexer and 1 for index position
                    printIndentation(out);
                    fprintf(out, "addq $%i, %%%s\n", 4 * POINTER_SIZE, getNextRegister(instruction->val.allocate.intermediate));
                } break;
            }
/*
            printIndentation(out);
            fprintf(out, "add %%r15, %%rax\n");
            printIndentation(out);
            fprintf(out, "mov %%rax, %%rdi\n");
            printIndentation(out);
            fprintf(out, "mov $12, %%rax\n");
            printIndentation(out);
            fprintf(out, "syscall\n");

            printIndentation(out);
            fprintf(out, "pop %%r14\n");
            printIndentation(out);
            fprintf(out, "pop %%rax\n");*/
            printIndentation(out);
            fprintf(out, "pushq %%%s\n", getNextRegister(instruction->val.allocate.intermediate));
            printIndentation(out);
            fprintf(out, "pushq %%rbp\n");
            printIndentation(out);
            fprintf(out, "call garbageCollectAllocate\n");

            //Insert metadata into rax
            switch (instruction->val.allocate.allocationType) {
                case ALLOC_RECORD_CLASS: {
                    SortedSet *gcSet = instruction->val.allocate.pointerSet;
                    int len = (int)length(gcSet);

                    //move size into head of new block
                    printIndentation(out);
                    fprintf(out, "movq %%%s, 0(%%rax)\n", getNextRegister(instruction->val.allocate.timesTemp));
                    //fix register
                    fprintf(out, "# ALLOC_RECORD_CLASS\n");
                    //1 slot for data size, 1 for ptr count and |ptrCount|
                    int extraSpace = len + 1;
                    printIndentation(out);
                    fprintf(out, "subq $%i, %%%s\n", extraSpace * POINTER_SIZE, getNextRegister(instruction->val.allocate.intermediate));
                    //Move the ptr index array size into new displacement
                    printIndentation(out);
                    fprintf(out, "movq $%i, (%%rax, %%%s, 1)\n", len, getNextRegister(instruction->val.allocate.intermediate));
                    //Now move the individual values into the last slots

                    SortedSet *gcIter = first(gcSet);

                    int localOffset = 1;
                    while (gcIter != NULL) {
                        printIndentation(out);
                        fprintf(out, "movq $%i, %i(%%rax, %%%s, 1)\n", gcIter->data, localOffset * POINTER_SIZE, getNextRegister(instruction->val.allocate.intermediate));
                        localOffset++;
                        gcIter = gcIter->_next;
                    }
                } break;
                case ALLOC_ARR_OF_PTR: {
                    SortedSet *gcSet = instruction->val.allocate.pointerSet;
                    int len = (int)length(gcSet);

                    //move size into head of new block
                    printIndentation(out);
                    fprintf(out, "movq %%%s, 0(%%rax)\n", getNextRegister(instruction->val.allocate.timesTemp));
                    //fix register
                    fprintf(out, "# ALLOC_ARR_OF_PTR\n");
                    //1 slot for data size, 1 for ptr count and |ptrCount|
                    int extraSpace = len + 1;
                    printIndentation(out);
                    fprintf(out, "subq $%i, %%%s\n", extraSpace * POINTER_SIZE, getNextRegister(instruction->val.allocate.intermediate));
                    //Move the ptr index array size into new displacement
                    printIndentation(out);
                    fprintf(out, "movq $%i, (%%rax, %%%s, 1)\n", len, getNextRegister(instruction->val.allocate.intermediate));
                    //Now move the individual values into the last slots

                    SortedSet *gcIter = first(gcSet);

                    int localOffset = 1;
                    while (gcIter != NULL) {
                        printIndentation(out);
                        fprintf(out, "movq $%i, %i(%%rax, %%%s, 1)\n", gcIter->data, localOffset * POINTER_SIZE, getNextRegister(instruction->val.allocate.intermediate));
                        localOffset++;
                        gcIter = gcIter->_next;
                    }
                } break;
                case ALLOC_ARR_OF_PRIM: {
                    //head is size tail is 0
                    //move size into head of new block
                    printIndentation(out);
                    fprintf(out, "movq %%%s, 0(%%rax)\n", getNextRegister(instruction->val.allocate.timesTemp));
                    //move displacer by 1 block
                    printIndentation(out);
                    fprintf(out, "subq $8, %%%s\n", getNextRegister(instruction->val.allocate.intermediate));
                    //Move the ptr index array size into new displacement
                    printIndentation(out);
                    fprintf(out, "movq $0, (%%rax, %%%s, 1)\n", getNextRegister(instruction->val.allocate.intermediate));
                } break;
                case ALLOC_LAMBDA: {
                    //-1 head for indicator
                    printIndentation(out);
                    fprintf(out, "movq $2, 0(%%rax)\n");
                    printIndentation(out);
                    fprintf(out, "movq $1, 24(%%rax)\n");
                    printIndentation(out);
                    fprintf(out, "movq $1, 32(%%rax)\n");
                } break;
            }

            //Clear stack
            printIndentation(out);
            fprintf(out, "popq %%%s\n", getNextRegister(instruction->val.allocate.intermediate));
            printIndentation(out);
            fprintf(out, "popq %%%s\n", getNextRegister(instruction->val.allocate.intermediate));

            //Make header invisible
            printIndentation(out);
            fprintf(out, "addq $8, %%rax\n");

/*
            printIndentation(out);
            fprintf(out, "pop %%r14\n");
            printIndentation(out);
            fprintf(out, "pop %%r15\n");
            printIndentation(out);
            fprintf(out, "pop %%rdi\n");*/
        } break;
            /*case COMPLEX_ALLOCATE_END : {
                fprintf(out, "# COMPLEX_ALLOCATE_END\n");
                printIndentation(out);
                fprintf(out, "pop %%r14\n");
                printIndentation(out);
                fprintf(out, "pop %%r15\n");
                printIndentation(out);
                fprintf(out, "pop %%rax\n");
                printIndentation(out);
                fprintf(out, "pop %%rdi\n");
            } break;*/
        case METADATA_CREATE_MAIN: {
            fprintf(out, "# METADATA_CREATE_MAIN\n");
            printIndentation(out);
            fprintf(out, ASM_HEADER);
            unsigned long long asBytes = ((unsigned long long)initialGcSizeMB) * 1048576;
            printIndentation(out);
            fprintf(out, MAIN_HEADER, asBytes, asBytes, asBytes, asBytes);
            printIndentation(out);
            offsetForFunction = (int)length(instruction->val.mainHeader.pointerSet) + 1 + 1;
            fprintf(out, "subq $%i, %%rsp\n",
                    16 +
                    ((int)instruction->val.mainHeader.tableForFunction->nextSymbolId +
                     offsetForFunction) * POINTER_SIZE);

            printIndentation(out);
            fprintf(out, "popq %%rax\n");
            printIndentation(out);
            fprintf(out, "movq %%rax, -8(%%rbp)\n");

            SortedSet *gcSet = instruction->val.mainHeader.pointerSet;
            int len = (int)length(gcSet);
            printIndentation(out);
            fprintf(out, "movq $%i, -16(%%rbp)\n", len);

            SortedSet *iter = first(gcSet);
            int counter = 3;
            while (iter != NULL) {
                printIndentation(out);
                fprintf(out, "movq $%i, -%i(%%rbp)\n", iter->data, counter * POINTER_SIZE);
                printIndentation(out);
                fprintf(out, "movq $0, -%i(%%rbp)\n", (iter->data + 1 + offsetForFunction) * POINTER_SIZE);
                counter++;
                iter = iter->_next;
            }

            printIndentation(out);
            fprintf(out, "leaq staticLink, %%rax\n\tmovq %%rbp, (%%rax)\n");
            currentIndendation++;
        } break;
            /*case COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK: {
                fprintf(out, "# COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK\n");
                SYMBOL *var = instruction->val.currentScopeLoad.var;
                printIndentation(out);
                fprintf(out, "mov -%zu(%%rbp), %%%s\n",
                        (var->uniqueIdForScope + 1) * POINTER_SIZE,
                        getNextRegister(instruction->val.currentScopeLoad.temporary));
                printIndentation(out);
                fprintf(out, "mov (%%%s), %%%s\n",
                        getNextRegister(instruction->val.currentScopeLoad.temporary),
                        getNextRegister(instruction->val.currentScopeLoad.temporary));
            } break;
            case COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER: {
                //Move ptr into intermediate
                fprintf(out, "# COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER\n");
                printIndentation(out);
                fprintf(out, "mov -%zu(%%rbp), %%%s\n",
                        (instruction->val.currentScopeSave.sym->uniqueIdForScope + 1) * POINTER_SIZE,
                        getNextRegister(instruction->val.currentScopeSave.intermediate));
                printIndentation(out);
                fprintf(out, "mov %%%s, (%%%s)\n",
                        getNextRegister(instruction->val.currentScopeSave.tempValue),
                        getNextRegister(instruction->val.currentScopeSave.intermediate));
            } break;
            case COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER_IN_SCOPE: {
                fprintf(out, "# COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER_IN_SCOPE\n");
                printIndentation(out);
                fprintf(out, "leaq staticLink, %%%s\n",
                        getNextRegister(instruction->val.saveTempToParentScope.intermediateTemp));
                printIndentation(out);
                fprintf(out, "movq %zu(%%%s), %%%s\n",
                        instruction->val.saveTempToParentScope.scopeToFindFrame * POINTER_SIZE,
                        getNextRegister(instruction->val.saveTempToParentScope.intermediateTemp),
                        getNextRegister(instruction->val.saveTempToParentScope.intermediateTemp));
                //We now have a pointer to the stack frame at temporary
                //We want to move the stack frame + offset into temporary
                printIndentation(out);
                fprintf(out, "movq -%zu(%%%s), %%%s\n",
                        (instruction->val.saveTempToParentScope.uniqueVariableId + 1) * POINTER_SIZE,
                        getNextRegister(instruction->val.saveTempToParentScope.intermediateTemp),
                        getNextRegister(instruction->val.saveTempToParentScope.intermediateTemp));
                //The pointer to the value is now in the temp
                printIndentation(out);
                fprintf(out, "movq %%%s, (%%%s)\n",
                        getNextRegister(instruction->val.saveTempToParentScope.inputTemp),
                        getNextRegister(instruction->val.saveTempToParentScope.intermediateTemp));
            } break;*/
            /*case COMPLEX_LOAD_VARIABLE_VALUE_FROM_STACK: {
                fprintf(out, "# COMPLEX_LOAD_VARIABLE_VALUE_FROM_STACK\n");
                SYMBOL *var = instruction->val.currentScopeLoad.var;
                printIndentation(out);
                fprintf(out, "mov -%zu(%%rbp), %%%s\n",
                        (var->uniqueIdForScope + 1) * POINTER_SIZE,
                        getNextRegister(instruction->val.currentScopeLoad.temporary));
            } break;
            case COMPLEX_LOAD_VARIABLE_VALUE_FROM_STACK_IN_SCOPE: {
                fprintf(out, "# COMPLEX_LOAD_VARIABLE_VALUE_FROM_STACK_IN_SCOPE\n");
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
                        (instruction->val.loadTempFromParentScope.uniqueVariableId + 1) * POINTER_SIZE,
                        getNextRegister(instruction->val.loadTempFromParentScope.outputTemp),
                        getNextRegister(instruction->val.loadTempFromParentScope.outputTemp));
            } break;
            case COMPLEX_MOVE_TEMPORARY_VALUE_TO_STACK: {
                fprintf(out, "# COMPLEX_MOVE_TEMPORARY_VALUE_TO_STACK\n");
                printIndentation(out);
                fprintf(out, "mov %%%s, -%zu(%%rbp)\n",
                        getNextRegister(instruction->val.currentScopeSave.tempValue),
                        (instruction->val.currentScopeSave.sym->uniqueIdForScope + 1) * POINTER_SIZE);
            } break;
            case COMPLEX_MOVE_TEMPORARY_VALUE_TO_STACK_IN_SCOPE: {
                fprintf(out, "# COMPLEX_MOVE_TEMPORARY_VALUE_TO_STACK_IN_SCOPE\n");
                printIndentation(out);
                fprintf(out, "leaq staticLink, %%%s\n",
                        getNextRegister(instruction->val.saveTempToParentScope.intermediateTemp));
                printIndentation(out);
                fprintf(out, "movq %zu(%%%s), %%%s\n",
                        instruction->val.saveTempToParentScope.scopeToFindFrame * POINTER_SIZE,
                        getNextRegister(instruction->val.saveTempToParentScope.intermediateTemp),
                        getNextRegister(instruction->val.saveTempToParentScope.intermediateTemp));
                //We now have a pointer to the stack frame at temporary
                //We want to move the stack frame + offset into temporary
                printIndentation(out);
                fprintf(out, "movq %%%s, -%zu(%%%s)\n",
                        getNextRegister(instruction->val.saveTempToParentScope.inputTemp),
                        (instruction->val.saveTempToParentScope.uniqueVariableId + 1) * POINTER_SIZE,
                        getNextRegister(instruction->val.saveTempToParentScope.intermediateTemp));
            } break;*/
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
        case INSTRUCTION_SET_ZERO: {
            fprintf(out, "# INSTRUCTION_SET_ZERO\n");
            printIndentation(out);
            fprintf(out, "xorq %%%s, %%%s\n",
                    getNextRegister(instruction->val.tempToSetZero),
                    getNextRegister(instruction->val.tempToSetZero));

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
            /*printIndentation(out);
            fprintf(out, "leaq staticLink, %%%s\n",
                    getNextRegister(instruction->val.pushPopStaticLink.temporary));
            printIndentation(out);
            fprintf(out, "pop %zu(%%%s)\n",
                    instruction->val.pushPopStaticLink.staticLinkDepth * POINTER_SIZE,
                    getNextRegister(instruction->val.pushPopStaticLink.temporary));*/
            printIndentation(out);
            fprintf(out, "leaq staticLink, %%%s\n",
                    getNextRegister(instruction->val.pushPopStaticLink.temporary));
            fprintf(out, "movq %%rbp, %zu(%%%s)\n",
                    instruction->val.pushPopStaticLink.staticLinkDepth * POINTER_SIZE,
                    getNextRegister(instruction->val.pushPopStaticLink.temporary));
        } break;

        case INSTRUCTION_CMP: {
            fprintf(out, "# INSTRUCTION_CMP\n");
            printIndentation(out);
            fprintf(out, "cmp %%%s, %%%s\n",
                    getNextRegister(instruction->val.arithmetic2.source),
                    getNextRegister(instruction->val.arithmetic2.dest));
        } break;
        case INSTRUCTION_LABEL: {
            fprintf(out, "# INSTRUCTION_LABEL\n");
            printIndentation(out);
            fprintf(out, "%s:\n",
                    instruction->val.label);
        } break;
        case INSTRUCTION_JE: {
            fprintf(out, "# INSTRUCTION_JE\n");
            printIndentation(out);
            fprintf(out, "je %s\n",
                    instruction->val.label);
        } break;
        case INSTRUCTION_JMP: {
            fprintf(out, "# INSTRUCTION_JMP\n");
            printIndentation(out);
            fprintf(out, "jmp %s\n",
                    instruction->val.label);
        } break;
        case INSTRUCTION_MOVE: {
            fprintf(out, "# INSTRUCTION_MOVE\n");
            printIndentation(out);
            fprintf(out, "mov %%%s, %%%s\n",
                    getNextRegister(instruction->val.arithmetic2.source),
                    getNextRegister(instruction->val.arithmetic2.dest));
        } break;
        case INSTRUCTION_ADD_CONST: {
            fprintf(out, "# INSTRUCTION_ADD_CONST\n");
            printIndentation(out);
            fprintf(out, "add $%i, %%%s\n",
                    instruction->val.art2const.constant,
                    getNextRegister(instruction->val.art2const.temp));
        } break;
        case INSTRUCTION_MUL_CONST: {
            fprintf(out, "# INSTRUCTION_MUL_CONST\n");
            printIndentation(out);
            fprintf(out, "imul $%i, %%%s\n",
                    instruction->val.art2const.constant,
                    getNextRegister(instruction->val.art2const.temp));
        } break;
        case INSTRUCTION_SUB_CONST: {
            fprintf(out, "# INSTRUCTION_SUB_CONST\n");
            printIndentation(out);
            fprintf(out, "sub $%i, %%%s\n",
                    instruction->val.art2const.constant,
                    getNextRegister(instruction->val.art2const.temp));
        } break;
        case METADATA_BEGIN_ARITHMETIC_EVALUATION: {

        } break;
        case METADATA_END_ARITHMETIC_EVALUATION: {

        } break;
        case COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET: {
            fprintf(out, "# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET\n");
            printIndentation(out);
            fprintf(out, "mov (%%%s, %%%s, %zu), %%%s\n",
                    getNextRegister(instruction->val.dereferenceOffset.ptrTemp),
                    getNextRegister(instruction->val.dereferenceOffset.offsetTemp),
                    instruction->val.dereferenceOffset.size,
                    getNextRegister(instruction->val.dereferenceOffset.ptrTemp));
        } break;
        case INSTRUCTION_MOVE_TO_OFFSET: {
            fprintf(out, "# INSTRUCTION_MOVE_TO_OFFSET\n");
            printIndentation(out);
            fprintf(out, "mov %%%s, (%%%s, %%%s,1)\n",
                    getNextRegister(instruction->val.moveToOffset.tempToMove),
                    getNextRegister(instruction->val.moveToOffset.ptrTemp),
                    getNextRegister(instruction->val.moveToOffset.offsetTemp));
        } break;
        case INSTRUCTION_LEA_TO_OFFSET: {

        } break;
        case COMPLEX_MOVE_TEMPORARY_INTO_STACK: {
            fprintf(out, "# COMPLEX_MOVE_TEMPORARY_INTO_STACK\n");
            printIndentation(out);
            fprintf(out, "mov %%%s, -%zu(%%rbp)\n",
                    getNextRegister(instruction->val.tempIntoStack.tempToMove),
                    instruction->val.tempIntoStack.offset + offsetForFunction * POINTER_SIZE);
        } break;
        case COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE: {
            fprintf(out, "# COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE\n");
            printIndentation(out);
            fprintf(out, "leaq staticLink, %%%s\n",
                    getNextRegister(instruction->val.tempIntoStackScope.intermediate));
            printIndentation(out);
            fprintf(out, "mov %zu(%%%s), %%%s\n",
                    instruction->val.tempIntoStackScope.scopeToFindFrame * POINTER_SIZE,
                    getNextRegister(instruction->val.tempIntoStackScope.intermediate),
                    getNextRegister(instruction->val.tempIntoStackScope.intermediate));

            printIndentation(out);
            //Grab offset
            fprintf(out, "movq -16(%%%s), %%%s\n",
                    getNextRegister(instruction->val.tempFromStackScope.intermediate),
                    getNextRegister(instruction->val.tempFromStackScope.intermediate2));

            //Skip header
            printIndentation(out);
            fprintf(out, "addq $1, %%%s\n",
                    getNextRegister(instruction->val.tempFromStackScope.intermediate2));

            printIndentation(out);
            fprintf(out, "imul $-1, %%%s\n",
                    getNextRegister(instruction->val.tempFromStackScope.intermediate2));

            printIndentation(out);
            fprintf(out, "mov %%%s, -%zu(%%%s, %%%s, 8)\n",
                    getNextRegister(instruction->val.tempIntoStack.tempToMove),
                    instruction->val.tempIntoStackScope.offset + POINTER_SIZE,
                    getNextRegister(instruction->val.tempIntoStackScope.intermediate),
                    getNextRegister(instruction->val.tempIntoStackScope.intermediate2));
        } break;
        case COMPLEX_MOVE_TEMPORARY_FROM_STACK: {
            fprintf(out, "# COMPLEX_MOVE_TEMPORARY_FROM_STACK\n");
            printIndentation(out);
            fprintf(out, "mov -%zu(%%rbp), %%%s\n",
                    instruction->val.tempFromStack.offset + offsetForFunction * POINTER_SIZE,
                    getNextRegister(instruction->val.tempFromStack.inputTemp));
        } break;
        case COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE: {
            fprintf(out, "# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE\n");
            printIndentation(out);
            fprintf(out, "leaq staticLink, %%%s\n",
                    getNextRegister(instruction->val.tempFromStackScope.intermediate));
            printIndentation(out);
            fprintf(out, "mov %zu(%%%s), %%%s\n",
                    instruction->val.tempFromStackScope.scopeToFindFrame * POINTER_SIZE,
                    getNextRegister(instruction->val.tempFromStackScope.intermediate),
                    getNextRegister(instruction->val.tempFromStackScope.intermediate));
            printIndentation(out);
            //Grab offset
            fprintf(out, "movq -16(%%%s), %%%s\n",
                    getNextRegister(instruction->val.tempFromStackScope.intermediate),
                    getNextRegister(instruction->val.tempFromStackScope.intermediate2));

            //Skip header
            printIndentation(out);
            fprintf(out, "addq $1, %%%s\n",
                    getNextRegister(instruction->val.tempFromStackScope.intermediate2));

            printIndentation(out);
            fprintf(out, "imul $-1, %%%s\n",
                    getNextRegister(instruction->val.tempFromStackScope.intermediate2));

            printIndentation(out);
            fprintf(out, "mov -%zu(%%%s, %%%s, 8), %%%s\n",
                    instruction->val.tempFromStackScope.offset + POINTER_SIZE,
                    getNextRegister(instruction->val.tempFromStackScope.intermediate),
                    getNextRegister(instruction->val.tempFromStackScope.intermediate2),
                    getNextRegister(instruction->val.tempFromStackScope.inputTemp));
        } break;
        case INSTRUCTION_REGISTER_CALL:{
            fprintf(out, "# INSTRUCTION_REGISTER_CALL\n");
            printIndentation(out);
            fprintf(out, "call *%%%s\n",
                    getNextRegister(instruction->val.callRegister));
        } break;
        case METADATA_BEGIN_GLOBAL_BLOCK:break;
        case METADATA_END_GLOBAL_BLOCK:break;
        case COMPLEX_RIP_LAMBDA_LOAD: {
            fprintf(out, "# COMPLEX_RIP_LAMBDA_LOAD\n");
            printIndentation(out);
            fprintf(out, "leaq %s(%%rip), %%%s\n",
                    instruction->val.lambdaLoad.lambdaGlobalName,
                    getNextRegister(instruction->val.lambdaLoad.temporary));
        } break;
        case COMPLEX_ALLOCATE_END:break;
        case METADATA_DEBUG_INFO: {
            fprintf(out, "# METADATA_DEBUG_INFO\n");
            printIndentation(out);
            fprintf(out, "# %s\n", instruction->val.debugInfo);
        }break;
        case COMPLEX_GARBAGE_COLLECT: {
            fprintf(out, "# COMPLEX_GARBAGE_COLLECT\n");
            printIndentation(out);
            fprintf(out, "pushq %%rbp\n");

            printIndentation(out);
            fprintf(out, "call garbageCollect\n");

            printIndentation(out);
            fprintf(out, "popq %%rbp\n");
        } break;
        case COMPLEX_ABS_VALUE: {
            fprintf(out, "# COMPLEX_ABS_VALUE\n");
            int bitsofChar = 8;
            int sizeInt = INTEGER_SIZE;
            int maskSize = bitsofChar * sizeInt - 1;

            printIndentation(out);
            fprintf(out, "movq %%%s, %%%s\n",
                    getNextRegister(instruction->val.arithmetic2.source),
                    getNextRegister(instruction->val.arithmetic2.dest));
            printIndentation(out);
            fprintf(out, "sar $%zu, %%%s\n",
                    (size_t)maskSize,
                    getNextRegister(instruction->val.arithmetic2.dest));
            printIndentation(out);
            fprintf(out, "addq %%%s, %%%s\n",
                    getNextRegister(instruction->val.arithmetic2.dest),
                    getNextRegister(instruction->val.arithmetic2.source));
            printIndentation(out);
            fprintf(out, "xor %%%s, %%%s\n",
                    getNextRegister(instruction->val.arithmetic2.source),
                    getNextRegister(instruction->val.arithmetic2.dest));
        } break;
        case COMPLEX_SAVE_ALL: {
            fprintf(out, "# COMPLEX_SAVE_ALL\n");

/*            SortedSet *ss = instruction->val.restoreSave;
            for (int i = 0; i < 16; i++) {
                if (exists(ss, i)) {
                    printIndentation(out);
                    fprintf(out, "pushq %%%s\n",
                            getNextRegister(i));
                }
            }*/



            printIndentation(out);
            fprintf(out, "pushq %%rcx\n");
            printIndentation(out);
            fprintf(out, "pushq %%rdx\n");
            printIndentation(out);
            fprintf(out, "pushq %%rbx\n");
            printIndentation(out);
            fprintf(out, "pushq %%rsi\n");
            printIndentation(out);
            fprintf(out, "pushq %%rdi\n");
            printIndentation(out);
            fprintf(out, "pushq %%r8\n");
            printIndentation(out);
            fprintf(out, "pushq %%r9\n");
            printIndentation(out);
            fprintf(out, "pushq %%r10\n");
            printIndentation(out);
            fprintf(out, "pushq %%r11\n");
            printIndentation(out);
            fprintf(out, "pushq %%r12\n");
            printIndentation(out);
            fprintf(out, "pushq %%r13\n");
            printIndentation(out);
            fprintf(out, "pushq %%r14\n");
            printIndentation(out);
            fprintf(out, "pushq %%r15\n");

        } break;
        case COMPLEX_RESTORE_ALL: {
            fprintf(out, "# COMPLEX_RESTORE_ALL\n");

/*            SortedSet *ss = instruction->val.restoreSave;
            for (int i = 16; i < 0; i++) {
                if (exists(ss, i)) {
                    printIndentation(out);
                    fprintf(out, "pushq %%%s\n",
                            getNextRegister(i));
                }
            }*/




        } break;
        case INSTRUCTION_ADD_STACK_PTR: {
            fprintf(out, "# INSTRUCTION_ADD_STACK_PTR\n");
            printIndentation(out);
            fprintf(out, "addq $%zu, %%rsp\n", instruction->val.toAddStackPtr);
        } break;

        case RUNTIME_ARRAY_BOUNDS_CHECK: {
            if (!TURBOMODE) {
                fprintf(out, "# RUNTIME_ARRAY_BOUNDS_CHECK\n");
                printIndentation(out);
                fprintf(out, "pushq %%%s\n", getNextRegister(instruction->val.arrayBounds.exprTemp));
                printIndentation(out);
                fprintf(out, "pushq %%%s\n", getNextRegister(instruction->val.arrayBounds.arrPtr));

                printIndentation(out);
                fprintf(out, "call outOfBoundsCheck\n");

                printIndentation(out);
                fprintf(out, "addq $16, %%rsp\n");
            }
        } break;
        case RUNTIME_DIV_ZERO: {
            if (!TURBOMODE) {
                fprintf(out, "# RUNTIME_DIV_ZERO\n");
                printIndentation(out);
                fprintf(out, "pushq %%%s\n", getNextRegister(instruction->val.divZeroTemp));

                printIndentation(out);
                fprintf(out, "call divZeroCheck\n");

                printIndentation(out);
                fprintf(out, "addq $8, %%rsp\n");
            }
        } break;
        case RUNTIME_NEGATIVE_ALLOC: {
            if (!TURBOMODE) {
                fprintf(out, "# RUNTIME_NEGATIVE_ALLOC\n");
                printIndentation(out);
                fprintf(out, "pushq %%%s\n", getNextRegister(instruction->val.negLenTemp));

                printIndentation(out);
                fprintf(out, "call negAllocCheck\n");

                printIndentation(out);
                fprintf(out, "addq $8, %%rsp\n");
            }
        } break;
        case RUNTIME_NULLPTR_CHECK: {
            if (!TURBOMODE) {
                fprintf(out, "# RUNTIME_NULLPTR_CHECK\n");
                printIndentation(out);
                fprintf(out, "pushq %%%s\n", getNextRegister(instruction->val.nullPtrCheck));

                printIndentation(out);
                fprintf(out, "call nullPtrCheck\n");

                printIndentation(out);
                fprintf(out, "addq $8, %%rsp\n");
            }
        } break;
        case INSTRUCTION_LEA_ADD: {
            fprintf(out, "# INSTRUCTION_LEA_ADD\n");
            printIndentation(out);
            fprintf(out, "leaq (%%%s, %%%s, 1), %%%s\n",
                    getNextRegister(instruction->val.arithmetic2.dest),
                    getNextRegister(instruction->val.arithmetic2.source),
                    getNextRegister(instruction->val.arithmetic2.dest));
        } break;
        case INSTRUCTION_LEA_ADD_CONST: {
            fprintf(out, "# INSTRUCTION_LEA_ADD_CONST\n");
            printIndentation(out);
            fprintf(out, "leaq %i(%%%s), %%%s\n",
                    instruction->val.art2const.constant,
                    getNextRegister(instruction->val.art2const.temp),
                    getNextRegister(instruction->val.art2const.temp));
        } break;
        case INSTRUCTION_LOAD_STACK_PTR: {
            fprintf(out, "# INSTRUCTION_LOAD_STACK_PTR\n");
            printIndentation(out);
            fprintf(out, "movq %i(%%rsp), %%%s\n",
                    instruction->val.loadStackPtr.offset,
                    getNextRegister(instruction->val.loadStackPtr.temp));
        } break;
        case COMPLEX_LOAD_POINTER_TO_STATIC_LINK_FRAME:break;
        case NOOP:break;
        case METADATA_ACCESS_VARIABLE_START: {
            fprintf(out, "# METADATA_ACCESS_VARIABLE_START\n");
        } break;
        case METADATA_ACCESS_VARIABLE_END: {
            fprintf(out, "# METADATA_ACCESS_VARIABLE_END\n");
        } break;
    }
}

extern bool debugMode;

void generateScopeFrames(FILE *file) {
    fprintf(file, ".section .data\n");
    fprintf(file, "staticLink:\n");
    fprintf(file, "\t.space %zu\n", (maxDistFromRoot + 1) * POINTER_SIZE);
    fprintf(file, "intprint:\n\t.asciz \"%%i\\n\"\n");
    fprintf(file, "charprint:\n\t.asciz \"%%c\"\n");
    fprintf(file, "nlprint:\n\t.asciz \"\\n\"\n");
    fprintf(file, "outOfBoundsMsg:\n"
                  "    .asciz \"Error, out of bounds\\n\"\n");
    fprintf(file, "divZeroMsg:\n"
                  "    .asciz \"Error, division by zero\\n\"\n");
    fprintf(file, "negAllocMsg:\n"
                  "    .asciz \"Error, negative allocation\\n\"\n");
    fprintf(file, "nullPtrMsg:\n"
                  "    .asciz \"Error, nullpointer dereferencec\\n\"\n");
    fprintf(file, GARBAGE_COLLECTOR_CHUNK);
    fprintf(file, RUNTIME_ERROR_FUNCS);
}

void generateForNestedGlobalBlocks(FILE *file, Instructions *iter) {
    if (iter == NULL) {
        return;
    }

    int level = -1;
    Instructions *toContinueFrom = NULL;

    while (iter != NULL) {
        if (iter->kind == METADATA_BEGIN_GLOBAL_BLOCK) {
            level++;
            if (toContinueFrom == NULL && level > 0) {
                toContinueFrom = iter;
            }
        }

        if (level == 0) {
            generateInstruction(file, iter);
        }

        if (iter->kind == METADATA_END_GLOBAL_BLOCK) {
            level--;
        }

        iter = iter->next;
    }

    generateForNestedGlobalBlocks(file, toContinueFrom);
}

void generate(FILE *file, Instructions* instructions) {
    if (placedBlocks == NULL) {
        placedBlocks = initMap(50);
    }

    generateScopeFrames(file);

    Instructions* current_instruction = instructions;
    while (current_instruction != NULL) {
        //Skip until global ends
        int globalCounter = 0;

        if (current_instruction->kind == METADATA_BEGIN_GLOBAL_BLOCK) {
            globalCounter++;
        }

        while (globalCounter != 0) {
            current_instruction = current_instruction->next;

            if (current_instruction->kind == METADATA_BEGIN_GLOBAL_BLOCK) {
                globalCounter++;
            }

            if (current_instruction->kind == METADATA_END_GLOBAL_BLOCK) {
                globalCounter--;
            }
        }

        generateInstruction(file, current_instruction);
        current_instruction = current_instruction->next;
    }

    fprintf(file, ASM_TAIL);

    current_instruction = instructions;
    generateForNestedGlobalBlocks(file, current_instruction);

}