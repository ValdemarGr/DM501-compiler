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
# VAR get
# VAR set
# METADATA_CREATE_MAIN
	main:
	push %rbp
	movq %rsp, %rbp
	subq $48, %rsp
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
		mov $24, %r15
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
# COMPLEX_RIP_LAMBDA_LOAD
		leaq lambda_0(%rip), %rdx
# INSTRUCTION_CONST
		mov $2, %rbx
# COMPLEX_ALLOCATE
		push %rdi
		push %r15
		push %r14
		mov %rbx, %r14
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
		mov $0, %rsi
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rdx, (%rax, %rsi,1)
# INSTRUCTION_COPY
		mov %rax, %r9
# INSTRUCTION_CONST
		mov $8, %r10
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rax, (%r9, %r10,1)
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %r12
		mov 0(%r12), %r12
		mov -16(%r12), %r11
# INSTRUCTION_CONST
		mov $8, %r13
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r9, (%r11, %r13,1)
# COMPLEX_RIP_LAMBDA_LOAD
		leaq lambda_1(%rip), %r14
# INSTRUCTION_CONST
		mov $2, %r15
# COMPLEX_ALLOCATE
		push %rdi
		push %r15
		push %r14
		mov %r15, %r14
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
		mov $0, %rcx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r14, (%rax, %rcx,1)
# INSTRUCTION_COPY
		mov %rax, %rsi
# INSTRUCTION_CONST
		mov $8, %rdi
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rax, (%rsi, %rdi,1)
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %r9
		mov 0(%r9), %r9
		mov -16(%r9), %r8
# INSTRUCTION_CONST
		mov $16, %r10
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rsi, (%r8, %r10,1)
# INSTRUCTION_POP
		pop %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -16(%rbp), %r11
# INSTRUCTION_CONST
		mov $8, %r12
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r11, %r12,1), %r11
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %r11, -24(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -16(%rbp), %r13
# INSTRUCTION_CONST
		mov $16, %r14
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r13, %r14,1), %r13
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %r13, -32(%rbp)
# INSTRUCTION_CONST
		mov $42, %r15
# INSTRUCTION_PUSH
		push %r15
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rcx
# INSTRUCTION_CONST
		mov $8, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rcx
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rbx
# INSTRUCTION_CONST
		mov $0, %rsi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rsi,1), %rbx
# INSTRUCTION_REGISTER_CALL
		call *%rbx
# INSTRUCTION_WRITE
		movq %rax, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rdi
# INSTRUCTION_CONST
		mov $8, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %r8,1), %rdi
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %rdi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %r9
# INSTRUCTION_CONST
		mov $0, %r10
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r9, %r10,1), %r9
# INSTRUCTION_REGISTER_CALL
		call *%r9
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
	leaq staticLink, %rdi
	movq %rbp, 16(%rdi)
# METADATA_FUNCTION_ARGUMENT
	mov 16(%rbp), %r8
	mov %r8, -8(%rbp)
# METADATA_DEBUG_INFO
	# CLASS LOAD
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -8(%rbp), %r9
# INSTRUCTION_CONST
	mov $0, %r10
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
	mov (%r9, %r10,1), %r9
# INSTRUCTION_RETURN
	mov %r9, %rax
	mov %rbp,%rsp
pop %rbp
ret
# METADATA_END_BODY_BLOCK
# INSTRUCTION_FUNCTION_LABEL
.type lambda_1, @function
lambda_1:
push %rbp
mov %rsp,%rbp
subq $32, %rsp
leaq staticLink, %rdx
movq %rbp, 16(%rdx)
# METADATA_FUNCTION_ARGUMENT
mov 16(%rbp), %rbx
mov %rbx, -8(%rbp)
# METADATA_FUNCTION_ARGUMENT
mov 24(%rbp), %rbx
mov %rbx, -16(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -8(%rbp), %rsi
# METADATA_DEBUG_INFO
# CLASS LOAD
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -16(%rbp), %rdi
# INSTRUCTION_CONST
mov $0, %r8
# INSTRUCTION_MOVE_TO_OFFSET
mov %rsi, (%rdi, %r8,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -8(%rbp), %r9
# INSTRUCTION_RETURN
mov %r9, %rax
mov %rbp,%rsp
pop %rbp
ret
# METADATA_END_BODY_BLOCK
