.include "print.asm"

.global main
main:
    movq $20, %r8
    movq $4, %r9

    movq %r8, %rax
    cqto
    idiv %r9


    push %rax
    call print_number

	mov $60, %rax
	mov $0, %rdi
 	syscall
