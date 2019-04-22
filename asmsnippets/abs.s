.include "print.asm"

.global main
main:
	mov $-5, %rax
	sar $31, %rax
	mov %rax, %r8
	add $-5, %rax
	xor %rax, %r8

	push %r8
	call print_number	
	
	mov $60, %rax
	mov $0, %rdi
 	syscall
