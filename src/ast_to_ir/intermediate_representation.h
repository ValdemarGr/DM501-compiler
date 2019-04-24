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

typedef struct Jump {
    char* label;
} Jump;

typedef struct JumpIfZero {
    char* label_true;
    char* label_false;
} JumpIfZero;

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
    INSTRUCTION_NEGATE,
    INSTRUCTION_ABS,
    INSTRUCTION_FUNCTION_CALL,
    INSTRUCTION_RIGHT_SHIFT,
    INSTRUCTION_XOR,
    INSTRUCTION_COPY,
    INSTRUCTION_CMP,

    COMPLEX_ALLOCATE,
    COMPLEX_ALLOCATE_END,
    COMPLEX_CONSTRAIN_BOOLEAN,
    COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK_IN_SCOPE,
    COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK,
    COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER,
    COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER_IN_SCOPE,
    COMPLEX_SAVE_STATIC_LINK,
    COMPLEX_RESTORE_STATIC_LINK,
    COMPLEX_LOAD_POINTER_TO_STATIC_LINK_FRAME,

    METADATA_BEGIN_BODY_BLOCK,
    METADATA_END_BODY_BLOCK,
    METADATA_FUNCTION_ARGUMENT,
    METADATA_CREATE_MAIN
} InstructionKind;

typedef struct Instructions {
    struct Instructions* next;
    InstructionKind kind;
    union {
        Arithmetic2 arithmetic2; //INSTRUCTION_ADD..
        Arithmetic3 arithmetic3;
        struct { int value; size_t temp; } constant; //INSTRUCTION_CONST
        SYMBOL *var; //INSTRUCTION_VAR
        struct { size_t ptrTemp; size_t timesTemp; Type* tpe;} allocate;
        struct {char* label; size_t distance; size_t temporary;} functionHead; //INSTRUCTION_FUNCTION_LABEL & INSTRUCTION_FUNCTION_END
        char* function; //INSTRUCTION_CALL
        size_t argNum; //METADATA_FUNCTION_ARGUMENT
        size_t tempToWrite; //INSTRUCTION_WRITE
        size_t tempToReturn; //INSTRUCTION_RETURN
        size_t tempToConstrain; //COMPLEX_CONSTRAIN_BOOLEAN
        size_t tempToPush;
        size_t tempToPopInto;
        size_t tempToNegate;
        size_t tempToAbs;
        struct { size_t constant; size_t temp; } rightShift;
        struct {SYMBOL* var; size_t temporary; } ptrLoad; //COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK
        struct {SYMBOL *sym; size_t tempValue; size_t intermediate; } ptrSave; //COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK
        struct { size_t scopeToFindFrame; size_t uniqueVariableId; size_t outputTemp; } loadTempFromParentScope;
        struct { size_t scopeToFindFrame; size_t uniqueVariableId; size_t intermediateTemp; size_t inputTemp; } saveTempFromParentScope;
        struct {size_t staticLinkDepth; size_t temporary; } pushPopStaticLink;
        struct { size_t ptrTemp; size_t scopeToFindFrame; size_t linkBaseOffset; size_t  intermediateTemp;} loadPtrToStaticLink;
    } val;
} Instructions;

#endif //HELLO_INTERMEDIATE_REPRESENTATION_H
