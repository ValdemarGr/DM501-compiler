.section .data
staticLink:
	.space 24
intprint:
	.asciz "%i\n"
.section .text
.global main
.extern printf
# METADATA_BEGIN_BODY_BLOCK
# VAR a
# VAR binded
# METADATA_CREATE_MAIN
	main:
	push %rbp
	movq %rsp, %rbp
	subq $40, %rsp
	leaq staticLink, %rax
	movq %rbp, (%rax)
# INSTRUCTION_CONST
		mov $1, %rax
# COMPLEX_ALLOCATE
		push %rdi
		push %r15
		push %r14
		mov %rax, %r14
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
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -16(%rbp)
# INSTRUCTION_CONST
		mov $22, %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rbx
		mov 0(%rbx), %rbx
		mov -16(%rbx), %rdx
# INSTRUCTION_CONST
		mov $0, %rsi
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rcx, (%rdx, %rsi,1)
# COMPLEX_RIP_LAMBDA_LOAD
		leaq lambda_0(%rip), %rdi
# INSTRUCTION_CONST
		mov $2, %r8
# COMPLEX_ALLOCATE
		push %rdi
		push %r15
		push %r14
		mov %r8, %r14
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
		mov $0, %r9
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rdi, (%rax, %r9,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %r13
		mov 0(%r13), %r13
		mov -16(%r13), %r12
# INSTRUCTION_CONST
		mov $8, %r14
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rax, (%r12, %r14,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -16(%rbp), %r15
# INSTRUCTION_CONST
		mov $8, %rcx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r15, %rcx,1), %r15
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %r15, -24(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -16(%rbp), %rdx
# INSTRUCTION_CONST
		mov $0, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdx, %rbx,1), %rdx
# INSTRUCTION_PUSH
		push %rdx
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
		mov -24(%rbp), %rax

		mov $8, %r8
		mov (%rax,%r8,1), %rax

		mov
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
	leaq staticLink, %r10
	movq %rbp, 16(%r10)
# INSTRUCTION_MINUS
	mov 16(%rbp), %r11
	mov %r11, -8(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -8(%rbp), %r12
# INSTRUCTION_CONST
	mov $2, %r13
# INSTRUCTION_MUL
	imul %r12, %r13
# INSTRUCTION_RETURN
	mov %r13, %rax
	mov %rbp,%rsp
pop %rbp
ret
# METADATA_END_BODY_BLOCK
