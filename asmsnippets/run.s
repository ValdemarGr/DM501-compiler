.section .data
staticLink:
	.space 8
intprint:
	.asciz "%i\n"
.section .text
.global main
.extern printf
# METADATA_BEGIN_BODY_BLOCK
# VAR o
# METADATA_CREATE_MAIN
	main:
	push %rbp
	movq %rsp, %rbp
	subq $24, %rsp
	leaq staticLink, %rax
	movq %rbp, (%rax)
# INSTRUCTION_CONST
		mov $0, %rax
# INSTRUCTION_CONST
		mov $4, %rcx
# INSTRUCTION_MINUS
		sub %rcx, %rax
# COMPLEX_MOVE_TEMPORARY_VALUE_TO_STACK
		mov %rax, -8(%rbp)
# INSTRUCTION_CONST
		mov $1, %rbx
# INSTRUCTION_WRITE
		movq %rbx, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# METADATA_END_BODY_BLOCK
mov $60, %rax
mov $0, %rdi
syscall
