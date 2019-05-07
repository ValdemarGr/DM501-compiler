.section .data
staticLink:
	.space 24
intprint:
	.asciz "%i\n"
.section .text
.global main
.extern printf
# METADATA_BEGIN_BODY_BLOCK
# VAR clsB
# VAR clsA
# METADATA_CREATE_MAIN
	main:
	push %rbp
	movq %rsp, %rbp
	subq $56, %rsp
	movq $2, -8(%rbp)
	movq $0, -16(%rbp)
	movq $1, -24(%rbp)
	leaq staticLink, %rax
	movq %rbp, (%rax)
# INSTRUCTION_CONST
		mov $3, %rcx
# COMPLEX_ALLOCATE
		push %rdi
		push %r15
		push %r14
		mov %rcx, %r14
		mov $0, %rdi
		mov $12, %rax
		syscall
		push %rax
		mov $8, %r15
		push %r14
		imul %r14, %r15
# ALLOC_RECORD_CLASS
		addq $40, %r15
		add %r15, %rax
		mov %rax, %rdi
		mov $12, %rax
		syscall
		pop %r14
		pop %rax
		movq %r14, 0(%rax)
		addq $1, %r14
		movq $3, (%rax, %r14, 8)
		movq $0, 0(%rax, %r14, 8)
		movq $1, 8(%rax, %r14, 8)
		movq $2, 16(%rax, %r14, 8)
		addq $8, %rax
		pop %r14
		pop %r15
		pop %rdi
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -32(%rbp)
# INSTRUCTION_CONST
		mov $22, %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rsi
		mov 0(%rsi), %rsi
		mov -32(%rsi), %rbx
# INSTRUCTION_CONST
		mov $8, %rdi
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
		push %r14
		imul %r14, %r15
# ALLOC_LAMBDA
		addq $8, %r15
		add %r15, %rax
		mov %rax, %rdi
		mov $12, %rax
		syscall
		pop %r14
		pop %rax
		movq $-1, 0(%rax)
		addq $8, %rax
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
		mov -32(%rcx), %r15
# INSTRUCTION_CONST
		mov $16, %rdx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r13, (%r15, %rdx,1)
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_CONST
		mov $69, %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rsi
# INSTRUCTION_CONST
		mov $0, %rdi
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rbx, (%rsi, %rdi,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %r8
# INSTRUCTION_CONST
		mov $0, %r9
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r8, %r9,1), %r8
# INSTRUCTION_WRITE
		movq %r8, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %r10
# INSTRUCTION_CONST
		mov $0, %r11
# INSTRUCTION_ADD
		add %r11, %r10
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %r10, -40(%rbp)
# INSTRUCTION_CONST
		mov $55, %r12
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r13
# INSTRUCTION_CONST
		mov $0, %r14
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r12, (%r13, %r14,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r15
# INSTRUCTION_CONST
		mov $0, %rcx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r15, %rcx,1), %r15
# INSTRUCTION_WRITE
		movq %r15, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# INSTRUCTION_CONST
		mov $42, %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rbx
# INSTRUCTION_CONST
		mov $0, %rsi
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rdx, (%rbx, %rsi,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rdi
# INSTRUCTION_CONST
		mov $0, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %r8,1), %rdi
# INSTRUCTION_WRITE
		movq %rdi, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# INSTRUCTION_CONST
		mov $99, %r9
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r10
# INSTRUCTION_CONST
		mov $0, %r11
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r9, (%r10, %r11,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %r12
# INSTRUCTION_CONST
		mov $16, %r13
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r12, %r13,1), %r12
# COMPLEX_SAVE_STATIC_LINK
		leaq staticLink, %r14
		push 0(%r14)
# INSTRUCTION_CONST
		mov $8, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r12, %r15,1), %r12
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %r12
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rcx
# INSTRUCTION_CONST
		mov $16, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rcx
# INSTRUCTION_CONST
		mov $0, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rbx,1), %rcx
# INSTRUCTION_REGISTER_CALL
		call *%rcx
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rsi
		pop 0(%rsi)
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
mov $60, %rax
mov $0, %rdi
syscall
# INSTRUCTION_FUNCTION_LABEL
.type lambda_0, @function
lambda_0:
push %rbp
mov %rsp,%rbp
	subq $32, %rsp
	movq $0, -8(%rbp)
	leaq staticLink, %r11
	movq %rbp, 16(%r11)
# METADATA_FUNCTION_ARGUMENT
	mov 16(%rbp), %r12
	mov %r12, -16(%rbp)
# METADATA_DEBUG_INFO
	# CLASS LOAD
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -16(%rbp), %r13
# INSTRUCTION_CONST
	mov $0, %r14
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
	mov (%r13, %r14,1), %r13
# INSTRUCTION_WRITE
	movq %r13, %rsi
	movq $intprint, %rdi
	movq $0, %rax
	call printf
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
