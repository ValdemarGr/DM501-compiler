#ifndef HELLO_ABSTRACT_ASM_TREE_H
#define HELLO_ABSTRACT_ASM_TREE_H

typedef struct Value Value;

typedef struct Jump {
    char* label;
} Jump;

typedef struct JumpIfZero {
    char* label_true;
    char* label_false;
} JumpIfZero;

typedef struct Add {
    Value* left;
    Value* right;
} Add;

typedef enum {
    VALUE_UID, VALUE_ADD
} ValueKind;

typedef struct Value {
    ValueKind kind;
    union {
        int identifier;
        Add add;
    } val;
} Value;

typedef enum {
    INSTRUCTION_ADD
} InstructionKind;

typedef struct Instructions {
    struct Instructions* next;
    InstructionKind kind;
    union {
        Add add;
    } val;
} Instructions;

#endif //HELLO_ABSTRACT_ASM_TREE_H
