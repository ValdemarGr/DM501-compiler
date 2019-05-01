.section .data
staticLink:
	.space 8
intprint:
	.asciz "%i\n"
.section .text
.global main
.extern printf
# METADATA_BEGIN_BODY_BLOCK
# VAR r
# METADATA_CREATE_MAIN
	main:
	push %rbp
	movq %rsp, %rbp
	subq $48, %rsp
	leaq staticLink, %rax
	movq %rbp, (%rax)
# VAR o
# INSTRUCTION_CONST
		mov $69, %rax
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, 24(%rbp)
# INSTRUCTION_CONST
		mov $1, %rcx
# COMPLEX_ALLOCATE
		push %rdi
		push %rax
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
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, 16(%rbp)
# COMPLEX_ALLOCATE_END
		pop %r14
		pop %r15
		pop %rax
		pop %rdi
# INSTRUCTION_CONST
		mov $1, %rbx
# COMPLEX_ALLOCATE
		push %rdi
		push %rax
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
		pop %rsi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov 16(%rbp), %rdi
# INSTRUCTION_CONST
		mov $0, %r8
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rsi, (%rdi, %r8,1)
# COMPLEX_ALLOCATE_END
		pop %r14
		pop %r15
		pop %rax
		pop %rdi
# INSTRUCTION_CONST
		mov $2, %r9
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov 16(%rbp), %r10
# INSTRUCTION_CONST
		mov $0, %r11
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r10, %r11,1), %r10
# INSTRUCTION_CONST
		mov $0, %r12
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r9, (%r10, %r12,1)
# INSTRUCTION_CONST
		mov $77, %r13
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %r13, 32(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov 16(%rbp), %r14
# INSTRUCTION_CONST
		mov $0, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r14, %r15,1), %r14
# INSTRUCTION_CONST
		mov $0, %rax
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r14, %rax,1), %r14
# INSTRUCTION_WRITE
		movq %r14, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov 24(%rbp), %rcx
# INSTRUCTION_WRITE
		movq %rcx, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov 32(%rbp), %rdx
# INSTRUCTION_WRITE
		movq %rdx, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# METADATA_END_BODY_BLOCK
mov $60, %rax
mov $0, %rdi
syscall
