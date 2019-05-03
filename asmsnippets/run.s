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
.type foo, @function
foo:
push %rbp
mov %rsp,%rbp
	subq $24, %rsp
	leaq staticLink, %rcx
	movq %rbp, 8(%rcx)
# INSTRUCTION_MINUS
	mov 16(%rbp), %rdx
	mov %rdx, -8(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -8(%rbp), %rbx
# INSTRUCTION_CONST
	mov $2, %rsi
# INSTRUCTION_MUL
	imul %rbx, %rsi
# INSTRUCTION_RETURN
	mov %rsi, %rax
	mov %rbp,%rsp
pop %rbp
ret
# METADATA_END_BODY_BLOCK
# VAR a
# VAR binded
# METADATA_CREATE_MAIN
main:
	push %rbp
	movq %rsp, %rbp
subq $48, %rsp
leaq staticLink, %rax
	movq %rbp, (%rax)
# INSTRUCTION_CONST
	mov $1, %rbx
# COMPLEX_ALLOCATE
	push %rdi
	push %r15
	push %r14
	mov %rbx, %r14
	mov $0, %rdi
	mov $12, %rax
	syscall
	push %rax
	mov $16, %r15
	imul %r14, %r15
	add %r15, %rax
	mov %rax, %rdi
	mov $12, %rax
	syscall
	pop %rax
	pop %r14
	pop %r15
	pop %rdi
# INSTRUCTION_PUSH
	push %rax
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
	mov %rax, -24(%rbp)
# INSTRUCTION_CONST
	mov $22, %rsi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
	leaq staticLink, %r8
	mov 0(%r8), %r8
	mov -24(%r8), %rdi
# INSTRUCTION_CONST
	mov $0, %r9
# INSTRUCTION_MOVE_TO_OFFSET
	mov %rsi, (%rdi, %r9,1)
# COMPLEX_RIP_LAMBDA_LOAD
	leaq lambda_0(%rip), %r10
# INSTRUCTION_CONST
	mov $2, %r11
# COMPLEX_ALLOCATE
	push %rdi
	push %r15
	push %r14
	mov %r11, %r14
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
	mov $0, %r12
# INSTRUCTION_MOVE_TO_OFFSET
	mov %r10, (%rax, %r12,1)
# INSTRUCTION_COPY
	mov %rax, %r15
# INSTRUCTION_CONST
	mov $8, %rcx
# INSTRUCTION_POP
	pop %rax
# INSTRUCTION_MOVE_TO_OFFSET
	mov %rax, (%r15, %rcx,1)
# INSTRUCTION_PUSH
	push %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
	leaq staticLink, %rbx
	mov 0(%rbx), %rbx
	mov -24(%rbx), %rdx
# INSTRUCTION_CONST
	mov $8, %rsi
# INSTRUCTION_MOVE_TO_OFFSET
	mov %r15, (%rdx, %rsi,1)
# INSTRUCTION_POP
	pop %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -24(%rbp), %rdi
# INSTRUCTION_CONST
	mov $8, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
	mov (%rdi, %r8,1), %rdi
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
	mov %rdi, -32(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -24(%rbp), %r9
# INSTRUCTION_CONST
	mov $0, %r10
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
	mov (%r9, %r10,1), %r9
# INSTRUCTION_PUSH
	push %r9
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -32(%rbp), %r11
# INSTRUCTION_CONST
	mov $0, %r12
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
	mov (%r11, %r12,1), %r11
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -32(%rbp), %r13
# INSTRUCTION_CONST
	mov $0, %r14
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
	mov (%r13, %r14,1), %r13
# INSTRUCTION_REGISTER_CALL
	call *%r13
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
leaq staticLink, %r13
movq %rbp, 16(%r13)
# INSTRUCTION_MINUS
mov 16(%rbp), %r14
mov %r14, -8(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -8(%rbp), %r15
# INSTRUCTION_CONST
mov $2, %rcx
# INSTRUCTION_MUL
imul %r15, %rcx
# INSTRUCTION_RETURN
mov %rcx, %rax
mov %rbp,%rsp
pop %rbp
ret
# METADATA_END_BODY_BLOCK
