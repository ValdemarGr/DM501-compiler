#include "asm_code_gen.h"
#include "../abstract_asm_code_gen/abstract_asm_tree.h"
#include <stdio.h>

void generate_value(FILE* out, Value value) {
    switch (value->kind) {
        case VALUE_UID:
            generate_load_uid(out, value->val->uid);
            break;
    }
}

void generate_add(FILE* out, Add add) {
    fprintf(out, "")
}

char *generateAsm() {
    char* ret = (char*)malloc(sizeof(char) * strlen(ASM_HEADER) + strlen(ASM_TAIL));
    strcat(ret, ASM_HEADER);
    strcat(ret, ASM_TAIL);
    return ret;
}
