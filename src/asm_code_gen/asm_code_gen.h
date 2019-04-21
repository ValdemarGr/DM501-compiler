//
// Created by Mads Grau Kristensen on 2019-03-25.
//

#ifndef HELLO_AMS_CODE_GEN_H
#define HELLO_AMS_CODE_GEN_H

#include "consts.h"
#include "../ast_to_ir/intermediate_representation.h"
#include <stdio.h>
#include "sizes.h"

void generate(FILE *file, Instructions* instructions);

#endif //HELLO_AMS_CODE_GEN_H
