.section .data
staticLink:
	.space 16
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
	subq $32, %rsp
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
		mov $16, %r15
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
		mov %rax, -8(%rbp)
# INSTRUCTION_CONST
		mov $1, %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rsi
		mov 0(%rsi), %rsi
		mov -8(%rsi), %rbx
# INSTRUCTION_CONST
		mov $0, %rdi
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rdx, (%rbx, %rdi,1)
# INSTRUCTION_CONST
		mov $22, %r8
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %r10
		mov 0(%r10), %r10
		mov -8(%r10), %r9
# INSTRUCTION_CONST
		mov $8, %r11
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r8, (%r9, %r11,1)
# INSTRUCTION_POP
		pop %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -8(%rbp), %r12
# INSTRUCTION_CONST
		mov $8, %r13
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r12, %r13,1), %r12
# INSTRUCTION_WRITE
		movq %r12, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -8(%rbp), %r14
# INSTRUCTION_CONST
		mov $0, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r14, %r15,1), %r14
# INSTRUCTION_WRITE
		movq %r14, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -8(%rbp), %rcx
# INSTRUCTION_CONST
		mov $0, %rdx
# INSTRUCTION_ADD
		add %rdx, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -16(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -16(%rbp), %rbx
# INSTRUCTION_CONST
		mov $0, %rsi
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
mov $60, %rax
mov $0, %rdi
syscall
