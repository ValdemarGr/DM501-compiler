.section .data
staticLink:
	.space 16
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
	subq $32, %rsp
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
		mov $8, %r15
		imul %r14, %r15
		add %r15, %rax
		mov %rax, %rdi
		mov $12, %rax
		syscall
		pop %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -16(%rbp)
# COMPLEX_ALLOCATE_END
		pop %r14
		pop %r15
		pop %rax
		pop %rdi
# INSTRUCTION_CONST
		mov $77, %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -16(%rbp), %rbx
# INSTRUCTION_CONST
		mov $8, %rsi
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rdx, (%rbx, %rsi,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -16(%rbp), %rdi
# INSTRUCTION_CONST
		mov $8, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %r8,1), %rdi
# INSTRUCTION_WRITE
		movq %rdi, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# METADATA_END_BODY_BLOCK
mov $60, %rax
mov $0, %rdi
syscall
