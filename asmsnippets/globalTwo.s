.include "print.asm"

.section .data
#8 bytes per pointer
staticLink:
    .space 16

.global main
main:


    movq $42, (staticLink)
    movq (staticLink), %r8
	push %r8
	call print_number	

	mov $60, %rax
	mov $0, %rdi
 	syscall
