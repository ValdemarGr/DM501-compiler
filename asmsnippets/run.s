.section .data
staticLink:
	.space 24
intprint:
	.asciz "%i\n"
.section .text
.global main
.extern printf
# METADATA_BEGIN_BODY_BLOCK
# INSTRUCTION_FUNCTION_LABEL
.type f, @function
f:
push %rbp
mov %rsp,%rbp
	subq $40, %rsp
	leaq staticLink, %rax
	movq %rbp, 8(%rax)
# INSTRUCTION_MINUS
	mov 16(%rbp), %rcx
	mov %rcx, -8(%rbp)
# VAR l
# COMPLEX_RIP_LAMBDA_LOAD
	leaq lambda_0(%rip), %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
	mov %rdx, -16(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -16(%rbp), %rdi
# INSTRUCTION_RETURN
	mov %rdi, %rax
	mov %rbp,%rsp
pop %rbp
ret
# METADATA_END_BODY_BLOCK
# METADATA_CREATE_MAIN
main:
	push %rbp
	movq %rsp, %rbp
subq $32, %rsp
leaq staticLink, %rax
	movq %rbp, (%rax)
# VAR b
# INSTRUCTION_CONST
	mov $5, %rdx
# INSTRUCTION_PUSH
	push %rdx
# INSTRUCTION_FUNCTION_CALL
	call f
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
	mov %rax, -16(%rbp)
# INSTRUCTION_CONST
	mov $42, %rbx
# INSTRUCTION_PUSH
	push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -16(%rbp), %rsi
# INSTRUCTION_REGISTER_CALL
	call *%rsi
# INSTRUCTION_WRITE
	movq %rax, %rsi
	movq $intprint, %rdi
	movq $0, %rax
	call printf
# METADATA_END_BODY_BLOCK
mov $60, %rax
mov $0, %rdi
syscall
# INSTRUCTION_FUNCTION_LABEL
.type lambda_0, @function
lambda_0:
push %rbp
mov %rsp,%rbp
subq $24, %rsp
leaq staticLink, %rbx
movq %rbp, 16(%rbx)
# INSTRUCTION_MINUS
mov 16(%rbp), %rsi
mov %rsi, -8(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -8(%rbp), %rdi
# INSTRUCTION_CONST
mov $2, %r8
# INSTRUCTION_MUL
imul %rdi, %r8
# INSTRUCTION_RETURN
mov %r8, %rax
mov %rbp,%rsp
pop %rbp
ret
# METADATA_END_BODY_BLOCK
