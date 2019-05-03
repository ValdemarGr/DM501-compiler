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
# INSTRUCTION_CONST
		mov $5, %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rax
		mov 0(%rax), %rax
		mov -16(%rax), %rbx
# INSTRUCTION_CONST
		mov $0, %rsi
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rdx, (%rbx, %rsi,1)
# COMPLEX_ALLOCATE_END
		pop %r14
		pop %r15
		pop %rax
		pop %rdi
# INSTRUCTION_CONST
		mov $77, %rdi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -16(%rbp), %r8
# INSTRUCTION_CONST
		mov $0, %r9
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rdi, (%r8, %r9,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -16(%rbp), %r10
# INSTRUCTION_CONST
		mov $0, %r11
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r10, %r11,1), %r10
# INSTRUCTION_WRITE
		movq %r10, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# METADATA_END_BODY_BLOCK
mov $60, %rax
mov $0, %rdi
syscall
