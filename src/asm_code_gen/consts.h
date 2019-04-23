//
// Created by valde on 3/25/19.
//

#ifndef HELLO_CONSTS_H
#define HELLO_CONSTS_H

#include "../utils/map.h"

#define ASM_HEADER ".global main\nmain:\n"
#define ASM_TAIL "mov $60, %%rax\nmov $0, %%rdi\nsyscall\n"
#define REGISTER_COUNT 14


#endif //HELLO_CONSTS_H
