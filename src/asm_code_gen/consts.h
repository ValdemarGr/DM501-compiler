//
// Created by valde on 3/25/19.
//

#ifndef HELLO_CONSTS_H
#define HELLO_CONSTS_H

#define ASM_HEADER "global main\nmain:\n"
#define ASM_TAIL " \tmov $60, %rax\ntmov $0, %rdi\nsyscall"

#endif //HELLO_CONSTS_H
