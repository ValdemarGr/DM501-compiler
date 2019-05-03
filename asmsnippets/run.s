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
.type hello, @function
hello:
push %rbp
mov %rsp,%rbp
	subq $32, %rsp
	leaq staticLink, %rcx
	movq %rbp, 8(%rcx)
# VAR lam
# COMPLEX_RIP_LAMBDA_LOAD
	leaq lambda_0(%rip), %rbx
# INSTRUCTION_CONST
	mov $2, %rsi
# COMPLEX_ALLOCATE
	push %rdi
	push %r15
	push %r14
	mov %rsi, %r14
	mov $0, %rdi
	mov $12, %rax
	syscall
	push %rax
	mov $8, %r15
	imul %r14, %r15
	add %r15, %rax
	mov %rax, %rdi
	mov $12, %rax
	syscall
	pop %rax
	pop %r14
	pop %r15
	pop %rdi
# INSTRUCTION_CONST
	mov $0, %rdi
# INSTRUCTION_MOVE_TO_OFFSET
	mov %rbx, (%rax, %rdi,1)
# INSTRUCTION_COPY
	mov %rax, %r10
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
	mov %r10, -16(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -16(%rbp), %r11
# INSTRUCTION_RETURN
	mov %r11, %rax
	mov %rbp,%rsp
pop %rbp
ret
# METADATA_END_BODY_BLOCK
# VAR binder
# METADATA_CREATE_MAIN
main:
	push %rbp
	movq %rsp, %rbp
subq $40, %rsp
leaq staticLink, %rax
	movq %rbp, (%rax)
# INSTRUCTION_FUNCTION_CALL
	call hello
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
	mov %rax, -24(%rbp)
# INSTRUCTION_CONST
	mov $5, %rbx
# INSTRUCTION_PUSH
	push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -24(%rbp), %rsi
# INSTRUCTION_CONST
	mov $0, %rdi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
	mov (%rsi, %rdi,1), %rsi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -24(%rbp), %r8
# INSTRUCTION_CONST
	mov $0, %r9
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
	mov (%r8, %r9,1), %r8
# INSTRUCTION_REGISTER_CALL
	call *%r8
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
leaq staticLink, %r8
movq %rbp, 16(%r8)
# METADATA_FUNCTION_ARGUMENT
mov 16(%rbp), %r9
mov %r9, -8(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -8(%rbp), %r10
# INSTRUCTION_CONST
mov $2, %r11
# INSTRUCTION_MUL
imul %r10, %r11
# INSTRUCTION_RETURN
mov %r11, %rax
mov %rbp,%rsp
pop %rbp
ret
# METADATA_END_BODY_BLOCK
