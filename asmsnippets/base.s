.include "print.asm"

.global main
main:
	mov $1, %rax
	mov $0, %r8

	and %r8, %rax
	push %rax
	call print_number	

	mov $0, %r10
	add %rax, %r10
	sub %r8, %r10
	cmp $0, %r10	
	jge printstuff

	jmp exit


	printstuff:
	push %r10
	call print_number

	exit:
	mov $60, %rax
	mov $0, %rdi
 	syscall
