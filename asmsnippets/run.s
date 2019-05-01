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
		mov $16, %r15
		imul %r14, %r15
		add %r15, %rax
		mov %rax, %rdi
		mov $12, %rax
		syscall
		pop %rcx
# COMPLEX_LOAD_POINTER_TO_STATIC_LINK_FRAME
		leaq staticLink, %rdx
		movq 0(%rdx), %rdx
		movq %rcx, -16(%rdx)
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
# COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK
		mov -16(%rbp), %rdi
		add $8, %rdi
# INSTRUCTION_CONST
		mov $0, %r8
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rsi, (%rdi, %r8,)
# COMPLEX_ALLOCATE_END
		pop %r14
		pop %r15
		pop %rax
		pop %rdi
# INSTRUCTION_CONST
		mov $2, %r9
# COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK
		mov -16(%rbp), %r10
		mov (%r10), %r10
# INSTRUCTION_CONST
		mov $0, %r11
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r10, %r11, ), %r10
# INSTRUCTION_CONST
		mov $0, %r12
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r9, (%r10, %r12,)
# COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK
		mov -16(%rbp), %r13
		mov (%r13), %r13
# INSTRUCTION_CONST
		mov $0, %r14
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r13, %r14, ), %r13
# INSTRUCTION_CONST
		mov $0, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r13, %r15, ), %r13
# INSTRUCTION_WRITE
		movq %r13, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# METADATA_END_BODY_BLOCK
mov $60, %rax
mov $0, %rdi
syscall
