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
	leaq staticLink, %rax
	movq %rbp, (%rax)
	subq $24, %rsp
# VAR b
# INSTRUCTION_CONST
		mov $5, %rax
# INSTRUCTION_CONST
		mov $2, %rcx
# INSTRUCTION_ADD
		add %rax, %rcx
# COMPLEX_MOVE_TEMPORARY_VALUE_TO_STACK
		mov %rcx, -8(%rbp)
# COMPLEX_LOAD_VARIABLE_VALUE_FROM_STACK
		mov -8(%rbp), %rbx
# INSTRUCTION_WRITE
		movq %rbx, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_LOAD_VARIABLE_VALUE_FROM_STACK
		mov -8(%rbp), %rsi
# INSTRUCTION_WRITE
		movq %rsi, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_LOAD_VARIABLE_VALUE_FROM_STACK
		mov -8(%rbp), %rdi
# INSTRUCTION_WRITE
		movq %rdi, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# METADATA_END_BODY_BLOCK
mov $60, %rax
mov $0, %rdi
syscall
