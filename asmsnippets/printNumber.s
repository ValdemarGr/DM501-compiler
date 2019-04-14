.include "print.asm"

.global main
main:
	push $4
	call print_number

	mov $60, %rax
	mov $0, %rdi
 	syscall
