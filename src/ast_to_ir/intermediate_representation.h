//
// Created by valde on 4/12/19.
//

#ifndef HELLO_INTERMEDIATE_REPRESENTATION_H
#define HELLO_INTERMEDIATE_REPRESENTATION_H

#include <stdio.h>
#include "../ast/tree.h"
#include "../utils/memory.h"
#include "../symbol/symbol.h"
#include "abstract_asm_tree.h"
#include "intermediate_representation.h"
#include "../ast/tree.h"
#include "../utils/sortedset.h"

typedef struct Arithmetic2 {
    size_t source;
    size_t dest;
} Arithmetic2;

typedef struct Arithmetic3 {
    size_t source1;
    size_t source2;
    size_t dest;
} Arithmetic3;

typedef enum {
    INSTRUCTION_ADD,
    INSTRUCTION_MINUS,
    INSTRUCTION_MUL,
    INSTRUCTION_DIV,
    INSTRUCTION_CONST,
    INSTRUCTION_PROGRAM_BEGIN,
    INSTRUCTION_FUNCTION_LABEL,
    INSTRUCTION_VAR,
    INSTRUCTION_FUNCTION_END,
    INSTRUCTION_RETURN,
    INSTRUCTION_WRITE,
    INSTRUCTION_AND,
    INSTRUCTION_OR,
    INSTRUCTION_PUSH,
    INSTRUCTION_POP,
    INSTRUCTION_PUSH_STACK,
    INSTRUCTION_POP_STACK,
    INSTRUCTION_NEGATE,
    INSTRUCTION_ABS,
    INSTRUCTION_FUNCTION_CALL,
    INSTRUCTION_RIGHT_SHIFT,
    INSTRUCTION_XOR,
    INSTRUCTION_COPY,
    INSTRUCTION_CMP,
    INSTRUCTION_LABEL,
    INSTRUCTION_JE,
    INSTRUCTION_JMP,
    INSTRUCTION_MOVE,
    INSTRUCTION_ADD_CONST,
    INSTRUCTION_MUL_CONST,
    INSTRUCTION_MOVE_TO_OFFSET,
    INSTRUCTION_LEA_TO_OFFSET,
    INSTRUCTION_REGISTER_CALL,

    COMPLEX_ALLOCATE,
    COMPLEX_ALLOCATE_END,
    COMPLEX_CONSTRAIN_BOOLEAN,
    COMPLEX_MOVE_TEMPORARY_INTO_STACK,
    COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE,
    COMPLEX_MOVE_TEMPORARY_FROM_STACK,
    COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE,
    COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET,
    COMPLEX_SAVE_STATIC_LINK,
    COMPLEX_RESTORE_STATIC_LINK,
    COMPLEX_LOAD_POINTER_TO_STATIC_LINK_FRAME,
    COMPLEX_RIP_LAMBDA_LOAD,
    COMPLEX_GARBAGE_COLLECT,

    METADATA_BEGIN_BODY_BLOCK,
    METADATA_END_BODY_BLOCK,
    METADATA_FUNCTION_ARGUMENT,
    METADATA_CREATE_MAIN,
    METADATA_BEGIN_ARITHMETIC_EVALUATION,
    METADATA_END_ARITHMETIC_EVALUATION,
    METADATA_BEGIN_GLOBAL_BLOCK,
    METADATA_END_GLOBAL_BLOCK,
    METADATA_DEBUG_INFO
} InstructionKind;

typedef enum {
    ALLOC_RECORD_CLASS,
    ALLOC_ARR_OF_PTR,
    ALLOC_ARR_OF_PRIM,
    ALLOC_LAMBDA
} AllocationType;

typedef struct Instructions {
    struct Instructions* next;
    struct Instructions* previous;
    InstructionKind kind;
    union {
        Arithmetic2 arithmetic2; //INSTRUCTION_ADD..
        Arithmetic3 arithmetic3;
        struct { int value; size_t temp; } constant; //INSTRUCTION_CONST
        SYMBOL *var; //INSTRUCTION_VAR
        struct { size_t timesTemp; size_t eleSize; SortedSet *pointerSet; AllocationType allocationType; size_t intermediate; } allocate;
        struct {char* label; size_t distance; size_t temporary; SymbolTable *tableForFunction; SortedSet *pointerSet; } functionHead; //INSTRUCTION_FUNCTION_LABEL & INSTRUCTION_FUNCTION_END
        char* function; //INSTRUCTION_CALL
        size_t callRegister;
        struct {size_t temporary; char* lambdaGlobalName; } lambdaLoad;
        struct {size_t argNum; size_t moveReg; size_t stackNum; } args; //METADATA_FUNCTION_ARGUMENT
        size_t tempToWrite; //INSTRUCTION_WRITE
        size_t tempToReturn; //INSTRUCTION_RETURN
        size_t tempToConstrain; //COMPLEX_CONSTRAIN_BOOLEAN
        size_t tempToPush;
        size_t tempToPopInto;
        size_t tempToNegate;
        size_t tempToAbs;
        struct {SymbolTable *tableForFunction; SortedSet *pointerSet;} mainHeader;
        char* label;
        char* debugInfo;
        struct { size_t offset; } popPushStack;
        struct { size_t offset; size_t tempToMove; } tempIntoStack;
        struct { size_t offset; size_t tempToMove; size_t scopeToFindFrame; size_t intermediate; size_t intermediate2; } tempIntoStackScope;
        struct { size_t offset; size_t inputTemp; } tempFromStack;
        struct { size_t offset; size_t inputTemp; size_t scopeToFindFrame; size_t intermediate; size_t intermediate2; } tempFromStackScope;
        struct { size_t constant; size_t temp; } rightShift;
        struct { size_t offsetTemp; size_t ptrTemp; size_t tempToMove; } moveToOffset;
        struct { size_t offsetTemp; size_t ptrTemp; size_t tempToLea; } leaToOffset;
        struct { int constant; size_t temp; } art2const;
        struct { size_t offsetTemp; size_t ptrTemp;  } dereferenceOffset;
        struct {size_t staticLinkDepth; size_t temporary; } pushPopStaticLink;
    } val;
} Instructions;

#endif //HELLO_INTERMEDIATE_REPRESENTATION_H
