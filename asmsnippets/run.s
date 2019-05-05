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
		mov $32, %r15
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
		mov $54, %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rsi
		mov 0(%rsi), %rsi
		mov -24(%rsi), %rbx
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
		mov -24(%rcx), %r15
# INSTRUCTION_CONST
		mov $16, %rdx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r13, (%r15, %rdx,1)
# INSTRUCTION_CONST
		mov $58, %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rdi
		mov 0(%rdi), %rdi
		mov -24(%rdi), %rsi
# INSTRUCTION_CONST
		mov $24, %r8
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rbx, (%rsi, %r8,1)
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_CONST
		mov $1, %r9
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
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %r10
# INSTRUCTION_CONST
		mov $0, %r11
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rax, (%r10, %r11,1)
# INSTRUCTION_CONST
		mov $66, %r12
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %r14
		mov 0(%r14), %r14
		mov -24(%r14), %r13
# INSTRUCTION_CONST
		mov $0, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r13, %r15,1), %r13
# INSTRUCTION_CONST
		mov $0, %rcx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r12, (%r13, %rcx,1)
# INSTRUCTION_POP
		pop %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rdx
# INSTRUCTION_CONST
		mov $16, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdx, %rbx,1), %rdx
# COMPLEX_SAVE_STATIC_LINK
		leaq staticLink, %rsi
		push 0(%rsi)
# INSTRUCTION_CONST
		mov $8, %rdi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdx, %rdi,1), %rdx
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %r8
# INSTRUCTION_CONST
		mov $16, %r9
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r8, %r9,1), %r8
# INSTRUCTION_CONST
		mov $0, %r10
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r8, %r10,1), %r8
# INSTRUCTION_REGISTER_CALL
		call *%r8
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %r11
		pop 0(%r11)
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
	subq $24, %rsp
	leaq staticLink, %r11
	movq %rbp, 16(%r11)
# METADATA_FUNCTION_ARGUMENT
	mov 16(%rbp), %r12
	mov %r12, -8(%rbp)
# METADATA_DEBUG_INFO
	# CLASS LOAD
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -8(%rbp), %r13
# INSTRUCTION_CONST
	mov $8, %r14
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
	mov (%r13, %r14,1), %r13
# INSTRUCTION_WRITE
	movq %r13, %rsi
	movq $intprint, %rdi
	movq $0, %rax
	call printf
# METADATA_DEBUG_INFO
	# CLASS LOAD
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -8(%rbp), %r15
# INSTRUCTION_CONST
	mov $0, %rcx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
	mov (%r15, %rcx,1), %r15
# INSTRUCTION_CONST
	mov $0, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
	mov (%r15, %rdx,1), %r15
# INSTRUCTION_WRITE
	movq %r15, %rsi
	movq $intprint, %rdi
	movq $0, %rax
	call printf
# METADATA_DEBUG_INFO
	# CLASS LOAD
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -8(%rbp), %rbx
# INSTRUCTION_CONST
	mov $24, %rsi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
	mov (%rbx, %rsi,1), %rbx
# INSTRUCTION_WRITE
	movq %rbx, %rsi
	movq $intprint, %rdi
	movq $0, %rax
	call printf
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
