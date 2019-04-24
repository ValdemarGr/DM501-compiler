.section .data
.LC1:
	.string "hello!"

.section .text
.global main
main:
    movl $.LC1, %edi
    movl $0, %eax
    call printf

    mov $60, %rax
    mov $0, %rdi
    syscall