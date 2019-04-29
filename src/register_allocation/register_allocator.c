//
// Created by valde on 4/21/19.
//

#include "register_allocator.h"
#include "../ast_to_ir/intermediate_representation.h"

ConstMap *currentRegisterContext = NULL;
size_t timestamp = 0;

SortedSet *evaluateRequiredRegistersForExpression(Instructions *instructions) {

    Instructions *iter = instructions;
    size_t registerDepth = 0;
    SortedSet *temporariesForExp = initHeadedSortedSet();

    while (iter != NULL) {
        switch (iter->kind) {
            case INSTRUCTION_ADD: {
                insertSortedSet(temporariesForExp, (int)iter->val.arithmetic2.dest);
            } break;
            case INSTRUCTION_MINUS: {
                insertSortedSet(temporariesForExp, (int)iter->val.arithmetic2.dest);
            } break;
            case INSTRUCTION_MUL: {
                insertSortedSet(temporariesForExp, (int)iter->val.arithmetic2.dest);
            } break;
            case INSTRUCTION_DIV: {
                insertSortedSet(temporariesForExp, (int)iter->val.arithmetic2.dest);
            } break;
            case INSTRUCTION_CONST: {
                insertSortedSet(temporariesForExp, (int)iter->val.constant.temp);
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
                insertSortedSet(temporariesForExp, (int)iter->val.arithmetic2.dest);
            } break;
            case INSTRUCTION_OR: {
                insertSortedSet(temporariesForExp, (int)iter->val.arithmetic2.dest);
            } break;
            case INSTRUCTION_PUSH: {

            } break;
            case INSTRUCTION_POP: {
                insertSortedSet(temporariesForExp, (int)iter->val.arithmetic2.dest);
            } break;
            case INSTRUCTION_NEGATE: {

            } break;
            case INSTRUCTION_ABS: {

            } break;
            case INSTRUCTION_FUNCTION_CALL: {

            } break;
            case INSTRUCTION_RIGHT_SHIFT: {
                insertSortedSet(temporariesForExp, (int)iter->val.arithmetic2.dest);
            } break;
            case INSTRUCTION_XOR: {
                insertSortedSet(temporariesForExp, (int)iter->val.arithmetic2.dest);
            } break;
            case INSTRUCTION_COPY: {
                insertSortedSet(temporariesForExp, (int)iter->val.arithmetic2.dest);
            } break;
            case INSTRUCTION_CMP: {
            } break;
            case INSTRUCTION_LABEL: {

            } break;
            case INSTRUCTION_JE: {

            } break;
            case INSTRUCTION_JMP: {

            } break;
            case COMPLEX_ALLOCATE: {
                insertSortedSet(temporariesForExp, (int)iter->val.allocate.ptrTemp);
            } break;
            case COMPLEX_ALLOCATE_END: {

            } break;
            case COMPLEX_CONSTRAIN_BOOLEAN: {

            } break;
            case COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK_IN_SCOPE: {
                insertSortedSet(temporariesForExp, (int)iter->val.loadTempFromParentScope.outputTemp);
            } break;
            case COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK: {
                insertSortedSet(temporariesForExp, (int)iter->val.ptrLoad.temporary);
            } break;
            case COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER: {
                insertSortedSet(temporariesForExp, (int)iter->val.ptrSave.intermediate);
            } break;
            case COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER_IN_SCOPE: {
                insertSortedSet(temporariesForExp, (int)iter->val.saveTempFromParentScope.intermediateTemp);
            } break;
            case COMPLEX_SAVE_STATIC_LINK: {
                insertSortedSet(temporariesForExp, (int)iter->val.pushPopStaticLink.temporary);
            } break;
            case COMPLEX_RESTORE_STATIC_LINK: {
                insertSortedSet(temporariesForExp, (int)iter->val.pushPopStaticLink.temporary);
            } break;
            case COMPLEX_LOAD_POINTER_TO_STATIC_LINK_FRAME: {
                insertSortedSet(temporariesForExp, (int)iter->val.loadPtrToStaticLink.intermediateTemp);
            } break;
            case METADATA_BEGIN_BODY_BLOCK: {

            } break;
            case METADATA_END_BODY_BLOCK: {

            } break;
            case METADATA_FUNCTION_ARGUMENT: {

            } break;
            case METADATA_CREATE_MAIN: {

            } break;
            case METADATA_BEGIN_ARITHMETIC_EVALUATION: {
                registerDepth = registerDepth + 1;
            } break;
            case METADATA_END_ARITHMETIC_EVALUATION: {
                if (registerDepth == 0) {
                    return temporariesForExp;
                }
            } break;
            case INSTRUCTION_MOVE:break;
            case INSTRUCTION_ADD_CONST:break;
            case INSTRUCTION_MUL_CONST:break;
        }

        iter = iter->next;
    }

    return temporariesForExp;
}

AbstractRegister *initRegister(char *name) {
    AbstractRegister *abstractRegister = (AbstractRegister*)malloc(sizeof(AbstractRegister));

    abstractRegister->useTimestamp = timestamp;
    abstractRegister->registerName = name;
    abstractRegister->boundInstruction = NULL;

    return abstractRegister;
}

ConstMap *registerMap() {
    ConstMap *registers = initMap(14);

    insert(registers, makeIntKey(0), initRegister("rax"));
    insert(registers, makeIntKey(1), initRegister("rcx"));
    insert(registers, makeIntKey(2), initRegister("rdx"));
    insert(registers, makeIntKey(3), initRegister("rbx"));
    insert(registers, makeIntKey(4), initRegister("rsi"));
    insert(registers, makeIntKey(5), initRegister("rdi"));
    insert(registers, makeIntKey(6), initRegister("r8"));
    insert(registers, makeIntKey(7), initRegister("r9"));
    insert(registers, makeIntKey(8), initRegister("r10"));
    insert(registers, makeIntKey(9), initRegister("r11"));
    insert(registers, makeIntKey(10), initRegister("r12"));
    insert(registers, makeIntKey(11), initRegister("r13"));
    insert(registers, makeIntKey(12), initRegister("r14"));
    insert(registers, makeIntKey(13), initRegister("r15"));

    return registers;
}

/*
 * When we spill we put intermediate products on the stack
 * The position will be the context pointer: rbp + sizeof(var) * varsInContext
 */
void simpleRegisterAllocation(Instructions *head) {
    if (currentRegisterContext == NULL) {
        currentRegisterContext = NEW(ConstMap);
    }

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
            case INSTRUCTION_CMP: {

            } break;
            case INSTRUCTION_LABEL: {

            } break;
            case INSTRUCTION_JE: {

            } break;
            case INSTRUCTION_JMP: {

            } break;
            case COMPLEX_ALLOCATE_END: {

            } break;
            case COMPLEX_SAVE_STATIC_LINK: {

            } break;
            case COMPLEX_RESTORE_STATIC_LINK: {

            } break;
            case COMPLEX_LOAD_POINTER_TO_STATIC_LINK_FRAME: {

            } break;
            case METADATA_BEGIN_ARITHMETIC_EVALUATION: {
                SortedSet *regs = evaluateRequiredRegistersForExpression(iter);
                size_t size = length(regs);
                printf("HELLO\n");
            } break;
            case METADATA_END_ARITHMETIC_EVALUATION: {

            } break;
        }

        iter = iter->next;
    }
}
