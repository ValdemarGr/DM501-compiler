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
		mov $1, %rcx
# COMPLEX_ALLOCATE
		push %rdi
		push %r15
		push %r14
		mov %rcx, %r14
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
		mov %rax, -16(%rbp)
# INSTRUCTION_CONST
		mov $22, %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rsi
		mov 0(%rsi), %rsi
		mov -16(%rsi), %rbx
# INSTRUCTION_CONST
		mov $0, %rdi
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rdx, (%rbx, %rdi,1)
# COMPLEX_RIP_LAMBDA_LOAD
		leaq lambda_0(%rip), %r8
# INSTRUCTION_CONST
		mov $2, %r9
# COMPLEX_ALLOCATE
		push %rdi
		push %r15
		push %r14
		mov %r9, %r14
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
		mov $0, %r10
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r8, (%rax, %r10,1)
# INSTRUCTION_COPY
		mov %rax, %r13
# INSTRUCTION_CONST
		mov $8, %r14
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rax, (%r13, %r14,1)
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rcx
		mov 0(%rcx), %rcx
		mov -16(%rcx), %r15
# INSTRUCTION_CONST
		mov $8, %rdx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r13, (%r15, %rdx,1)
# INSTRUCTION_POP
		pop %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -16(%rbp), %rbx
# INSTRUCTION_CONST
		mov $8, %rsi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rsi,1), %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rbx, -24(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -16(%rbp), %rdi
# INSTRUCTION_CONST
		mov $0, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %r8,1), %rdi
# INSTRUCTION_PUSH
		push %rdi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %r9
# INSTRUCTION_CONST
		mov $0, %r10
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r9, %r10,1), %r9
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %r11
# INSTRUCTION_CONST
		mov $0, %r12
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r11, %r12,1), %r11
# INSTRUCTION_REGISTER_CALL
		call *%r11

		mov -24(%rbp), %r8
		mov $8, %r10
		mov (%r8, %r10, 1), %r8

		mov -16(%rbp), %r9
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
	leaq staticLink, %r11
	movq %rbp, 16(%r11)
# INSTRUCTION_MINUS
	mov 16(%rbp), %r12
	mov %r12, -8(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -8(%rbp), %r13
# INSTRUCTION_CONST
	mov $2, %r14
# INSTRUCTION_MUL
	imul %r13, %r14
# INSTRUCTION_RETURN
	mov %r14, %rax
	mov %rbp,%rsp
pop %rbp
ret
# METADATA_END_BODY_BLOCK
