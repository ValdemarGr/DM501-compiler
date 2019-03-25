#ifndef HELLO_ABSTRACT_ASM_TREE_H
#define HELLO_ABSTRACT_ASM_TREE_H

typedef union Value Value;

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

typedef union Value {

    int identifier;
    Add add;
} Code;

#endif //HELLO_ABSTRACT_ASM_TREE_H
