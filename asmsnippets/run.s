.include "print.asm"
.section .data
staticLink:
	.space 24
.LC0:
	.string "%i"
.LC1:
	.string "hello!"
.section .text
# METADATA_BEGIN_BODY_BLOCK
# VAR a
# METADATA_CREATE_MAIN
	.global main
main:
	push %rbp
	movq %rsp, %rbp
	leaq staticLink, %rax
	movq %rbp, (%rax)
# INSTRUCTION_CONST
		mov $1, %rax
# COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK
		mov -8(%rbp), %rcx
		mov (%rcx), %rcx
# INSTRUCTION_CONST
		mov $5, %rdx
# INSTRUCTION_MINUS
		sub %rcx, %rdx
# INSTRUCTION_CONST
		mov $1, %rbx
# INSTRUCTION_MINUS
		sub %rbx, %rdx
# INSTRUCTION_CMP
		cmp %rdx, %rax
# INSTRUCTION_JE
		je if_0_begin
# INSTRUCTION_JMP
		jmp if_0_end
# INSTRUCTION_LABEL
		if_0_begin:
# COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK_IN_SCOPE
		leaq staticLink, %rsi
		movq 0(%rsi), %rsi
		movq -0(%rsi), %rsi
		movq (%rsi), %rsi
# INSTRUCTION_WRITE
		push %rsi
		#call print_number
		pop %rsi
# INSTRUCTION_LABEL
		if_0_end:
# METADATA_END_BODY_BLOCK
mov $60, %rax
mov $0, %rdi
syscall
