.include "print.asm"

.global main
main:
    movq $0, %r8

    cmp $0, %r8
    setg %dl
    movsx %dl, %r8 

    push %r8
    call print_number

	mov $60, %rax
	mov $0, %rdi
 	syscall
