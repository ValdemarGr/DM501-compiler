.section .data
staticLink:
	.space 8
intprint:
	.asciz "%i\n"
.section .text
.global main
.extern printf
# METADATA_BEGIN_BODY_BLOCK
# METADATA_CREATE_MAIN
	main:
	push %rbp
	movq %rsp, %rbp
	subq $24, %rsp
	leaq staticLink, %rax
	movq %rbp, (%rax)
# VAR a
# INSTRUCTION_CONST
		mov $5, %rax
# COMPLEX_MOVE_TEMPORARY_VALUE_TO_STACK
		mov %rax, -8(%rbp)
# COMPLEX_LOAD_VARIABLE_VALUE_FROM_STACK
		mov -8(%rbp), %rdx
# INSTRUCTION_WRITE
		movq %rdx, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# METADATA_END_BODY_BLOCK
mov $60, %rax
mov $0, %rdi
syscall
