//
// Created by valde on 5/13/19.
//

#ifndef HELLO_SANDBOXER_H
#define HELLO_SANDBOXER_H

#include "ast/tree.h"
#include "ast_to_ir/intermediate_representation.h"
#include "asm_code_gen/asm_code_gen.h"
#include <stdio.h>

int sandboxBody(Body *body);


#endif //HELLO_SANDBOXER_H
