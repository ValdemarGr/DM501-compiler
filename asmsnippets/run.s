.section .data
staticLink:
	.space 16
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
	subq $32, %rsp
	leaq staticLink, %rax
	movq %rbp, (%rax)
# VAR b
# INSTRUCTION_FUNCTION_LABEL
.type lambda_0, @function
lambda_0:
push %rbp
mov %rsp,%rbp
		subq $32, %rsp
		leaq staticLink, %rax
		movq %rbp, 8(%rax)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -8(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -16(%rbp), %rdx
# INSTRUCTION_ADD
		add %rcx, %rdx
# INSTRUCTION_RETURN
		mov %rdx, %rax
		mov %rbp,%rsp
pop %rbp
ret
# METADATA_END_BODY_BLOCK
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
	mov %rsi, -16(%rbp)
# METADATA_END_BODY_BLOCK
mov $60, %rax
mov $0, %rdi
syscall
