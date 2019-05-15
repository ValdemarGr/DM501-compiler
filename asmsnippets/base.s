.include "print.asm"

.global main
main:

    movq $22, %rax
    movq $11, %rdi

    leaq (%rax, %rdi, 1), %rax

    push %rax
    call print_number

	mov $60, %rax
	mov $0, %rdi
 	syscall
