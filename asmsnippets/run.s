.section .data
staticLink:
	.space 24
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
# VAR b
# INSTRUCTION_CONST
		mov $5, %rax
# COMPLEX_MOVE_TEMPORARY_VALUE_TO_STACK
		mov %rax, -8(%rbp)
# INSTRUCTION_CONST
		mov $1, %rdx
# COMPLEX_LOAD_VARIABLE_VALUE_FROM_STACK
		mov -8(%rbp), %rbx
# INSTRUCTION_CONST
		mov $5, %rsi
# INSTRUCTION_MINUS
		sub %rbx, %rsi
# INSTRUCTION_CONST
		mov $1, %rdi
# INSTRUCTION_MINUS
		sub %rsi, %rdi
# INSTRUCTION_CMP
		cmp %rdi, %rdx
# INSTRUCTION_JE
		je if_0_begin
# INSTRUCTION_JMP
		jmp if_0_end
# INSTRUCTION_LABEL
		if_0_begin:
# COMPLEX_LOAD_VARIABLE_VALUE_FROM_STACK_IN_SCOPE
		leaq staticLink, %r8
		movq 0(%r8), %r8
		movq -8(%r8), %r8
# INSTRUCTION_WRITE
		movq %r8, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# INSTRUCTION_LABEL
		if_0_end:
# INSTRUCTION_CONST
		mov $1, %r9
# COMPLEX_LOAD_VARIABLE_VALUE_FROM_STACK
		mov -8(%rbp), %r10
# INSTRUCTION_CONST
		mov $6, %r11
# INSTRUCTION_MINUS
		sub %r10, %r11
# INSTRUCTION_CMP
		cmp %r11, %r9
# INSTRUCTION_JE
		je if_1_begin
# INSTRUCTION_JMP
		jmp if_1_end
# INSTRUCTION_LABEL
		if_1_begin:
# COMPLEX_LOAD_VARIABLE_VALUE_FROM_STACK_IN_SCOPE
		leaq staticLink, %r12
		movq 0(%r12), %r12
		movq -8(%r12), %r12
# INSTRUCTION_WRITE
		movq %r12, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# INSTRUCTION_LABEL
		if_1_end:
# METADATA_END_BODY_BLOCK
mov $60, %rax
mov $0, %rdi
syscall
