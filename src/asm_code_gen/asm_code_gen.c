#include "asm_code_gen.h"

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
    char *uidLocation = getUidLocation(identifier);
    fprintf(out, "mov %s,%s", uidLocation, VALUE_STORE);
}

void generateValue(FILE *out, AsmValue *value) {
    switch (value->kind) {
        case VALUE_UID:
            generateLoadIdentifier(out, value->val.identifier);
            break;
        case VALUE_ADD:
            break;
        case VALUE_CONST:
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
            generateAdd(out, instruction->val.add);
            break;
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
            fprintf(out, ".type %s, @function\n%s:\npush %%rbp\nmov %%rsp,%%rbp\npush %%rax\n", instruction->val.label, instruction->val.label);
            break;
        case INSTRUCTION_VAR:
            fprintf(out, "mov $0,%s\n", getUidLocation(instruction->val.var));
            break;
        case INSTRUCTION_FUNCTION_END:
            fprintf(out, "mov %%rbp,%%rsp\npop %%rbp\nret\n");
            break;
        case INSTRUCTION_RETURN:
            break;
        case METADATA_FUNCTION_ARGUMENT:
            break;
    }
}

void generate(FILE *file, Instructions* instructions) {
    fprintf(file, ASM_HEADER);

    Instructions* current_instruction = instructions;

    while (current_instruction != NULL) {
        generateInstruction(file, current_instruction);
        current_instruction = current_instruction->next;
    }

    fprintf(file, ASM_TAIL);
}