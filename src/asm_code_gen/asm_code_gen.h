//
// Created by Mads Grau Kristensen on 2019-03-25.
//

#ifndef HELLO_AMS_CODE_GEN_H
#define HELLO_AMS_CODE_GEN_H

#include "consts.h"
#include "../ast_to_ir/intermediate_representation.h"
#include <stdio.h>

char *getUidLocation(int identifier);

void generateLoadIdentifier(FILE *out, int identifier);

void generateAdd(FILE *out, Add add);

void generateValue(FILE *out, AsmValue *value);

void generate(FILE *file, Instructions* instructions);

#endif //HELLO_AMS_CODE_GEN_H
