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
		push %rax
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
		pop %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -16(%rbp)
# INSTRUCTION_CONST
		mov $22, %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rax
		mov 0(%rax), %rax
		mov -16(%rax), %rbx
# INSTRUCTION_CONST
		mov $0, %rsi
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rdx, (%rbx, %rsi,1)
# COMPLEX_RIP_LAMBDA_LOAD
		leaq lambda_0(%rip), %r15
# COMPLEX_ALLOCATE
		push %rdi
		push %rax
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
		pop %r10
# INSTRUCTION_CONST
		mov $0, %r11
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r15, (%r10, %r11,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rax
		mov 0(%rax), %rax
		mov -16(%rax), %r12
# INSTRUCTION_CONST
		mov $8, %r13
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r10, (%r12, %r13,1)
# COMPLEX_ALLOCATE_END
		pop %r14
		pop %r15
		pop %rax
		pop %rdi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -16(%rbp), %r14
# INSTRUCTION_CONST
		mov $8, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r14, %r15,1), %r14
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %r14, -24(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -16(%rbp), %rax
# INSTRUCTION_CONST
		mov $0, %rcx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rax, %rcx,1), %rax
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rdx
# INSTRUCTION_CONST
		mov $0, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdx, %rbx,1), %rdx
# INSTRUCTION_REGISTER_CALL
		call *%rdx
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
# INSTRUCTION_MINUS
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
