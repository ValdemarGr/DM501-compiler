#include "asm_code_gen.h"
#include "../abstract_asm_code_gen/abstract_asm_tree.h"
#include <stdio.h>

#define ADD_RESULT_STORE "%rax"
#define VALUE_STORE "%rdx"

char *getUidLocation(int identifier) {
    switch (identifier) {
        case 1:
            return "%r8";
        case 2:
            return "%r9";
        case 3:
            return "%r10";
        case 4:
            return "%r11";
        default:
            return "%r12";
    }
}


void generateLoadIdentifier(FILE *out, int identifier) {
    char *uidLocation = getUidLocation(identifer);
    fprintf(out, "mov %s,%s", uidLocation, VALUE_STORE);
}

void generateValue(FILE *out, Value *value) {
    switch (value->kind) {
        case VALUE_UID:
            generateLoadIdentifier(out, value->identifier);
            break;
    }
}


void generateAdd(FILE *out, Add add) {
    generateValue(out, add.left);
    fprintf(out, "add %s,%s", VALUE_STORE, ADD_RESULT_STORE);
    generateValue(out, add.right);
    fprintf(out, "add %s,%s", VALUE_STORE, ADD_RESULT_STORE);
}

void generateInstruction(FILE *out, Instructions* instruction) {
    switch (instruction->kind) {
        case INSTRUCTION_ADD:
            generateAdd(out, generateAdd(out, instruction->val.add))
            break;
    }
}

void generate(FILE *file, Instructions* instructions) {
    fprintf(file, ASM_HEADER);

    Instructions* current_instruction = instructions;

    while (current_instruction != NULL) {
        generateInstruction(current_instruction);
        current_instruction = current_instruction->next;
    }

    fprintf(file, ASM_TAIL);
}