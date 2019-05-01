.section .data
staticLink:
	.space 8
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
# VAR z
# INSTRUCTION_CONST
		mov $5, %rax
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -16(%rbp)
# INSTRUCTION_CONST
		mov $10, %rcx
# INSTRUCTION_CONST
		mov $1, %rdx
# INSTRUCTION_ADD
		add %rdx, %rcx
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
		mov $8, %r15
		imul %r14, %r15
		add %r15, %rax
		mov %rax, %rdi
		mov $12, %rax
		syscall
		pop %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rbx, -8(%rbp)
# INSTRUCTION_CONST
		mov $10, %rsi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -8(%rbp), %rdi
# INSTRUCTION_CONST
		mov $0, %r8
# INSTRUCTION_CONST
		mov $8, %r9
# INSTRUCTION_CONST
		mov $0, %r10
# INSTRUCTION_ADD
		add %r10, %r8
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rsi, (%rdi, %r8,1)
# COMPLEX_ALLOCATE_END
		pop %r14
		pop %r15
		pop %rax
		pop %rdi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -8(%rbp), %r11
# INSTRUCTION_CONST
		mov $0, %r12
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r11, %r12,1), %r11
# INSTRUCTION_WRITE
		movq %r11, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# METADATA_END_BODY_BLOCK
mov $60, %rax
mov $0, %rdi
syscall
