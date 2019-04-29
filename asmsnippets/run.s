.section .data
staticLink:
	.space 8
intprint:
	.asciz "%i\n"
.section .text
.global main
.extern printf
# METADATA_BEGIN_BODY_BLOCK
# METADATA_CREATE_MAIN
	main:
	push %rbp
	movq %rsp, %rbp
	leaq staticLink, %rax
	movq %rbp, (%rax)
	subq $32, %rsp
# VAR a
# VAR b
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
# COMPLEX_LOAD_POINTER_TO_STATIC_LINK_FRAME
		leaq staticLink, %rdx
		movq 0(%rdx), %rdx
		movq %rcx, -8(%rdx)
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
# COMPLEX_LOAD_POINTER_TO_STATIC_LINK_FRAME
		leaq staticLink, %rdi
		movq 0(%rdi), %rdi
		movq %rsi, -16(%rdi)
# COMPLEX_ALLOCATE_END
		pop %r14
		pop %r15
		pop %rax
		pop %rdi
# INSTRUCTION_CONST
		mov $5, %r8
# INSTRUCTION_CONST
		mov $2, %r9
# INSTRUCTION_ADD
		add %r8, %r9
# COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER
		mov -16(%rbp), %r10
		mov %r9, (%r10)
# COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK
		mov -16(%rbp), %r11
		mov (%r11), %r11
# INSTRUCTION_WRITE
		movq %r11, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK
		mov -16(%rbp), %r12
		mov (%r12), %r12
# INSTRUCTION_WRITE
		movq %r12, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK
		mov -16(%rbp), %r13
		mov (%r13), %r13
# INSTRUCTION_WRITE
		movq %r13, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# METADATA_END_BODY_BLOCK
mov $60, %rax
mov $0, %rdi
syscall
