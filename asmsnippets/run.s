.section .data
staticLink:
	.space 24
intprint:
	.asciz "%i\n"
.section .text
.global main
.extern printf
# METADATA_BEGIN_BODY_BLOCK
# VAR arr
# VAR i
# METADATA_CREATE_MAIN
	main:
	push %rbp
	movq %rsp, %rbp
	subq $32, %rsp
	leaq staticLink, %rax
	movq %rbp, (%rax)
# INSTRUCTION_CONST
		mov $10, %rcx
# INSTRUCTION_CONST
		mov $1, %rdx
# INSTRUCTION_ADD
		add %rdx, %rcx
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
		imul %r14, %r15
		add %r15, %rax
		mov %rax, %rdi
		mov $12, %rax
		syscall
		pop %rax
		pop %r14
		pop %r15
		pop %rdi
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -8(%rbp)
# INSTRUCTION_CONST
		mov $10, %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -8(%rbp), %rsi
# INSTRUCTION_CONST
		mov $0, %rdi
# INSTRUCTION_CONST
		mov $0, %r8
# INSTRUCTION_ADD
		add %r8, %rdi
# INSTRUCTION_CONST
		mov $8, %r9
# INSTRUCTION_MUL
		imul %r9, %rdi
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rbx, (%rsi, %rdi,1)
# INSTRUCTION_CONST
		mov $0, %r10
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %r10, -16(%rbp)
# INSTRUCTION_LABEL
		while_cnd_0:
# INSTRUCTION_CONST
		mov $1, %r11
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -16(%rbp), %r12
# INSTRUCTION_CONST
		mov $10, %r13
# INSTRUCTION_MINUS
		sub %r13, %r12
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rdx
		cmp $0, %r12
		seta %dl
		movsx %dl, %r12
		pop %rdx
# INSTRUCTION_CMP
		cmp %r12, %r11
# INSTRUCTION_JE
		je while_0_begin
# INSTRUCTION_JMP
		jmp while_0_end
# INSTRUCTION_LABEL
		while_0_begin:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %r15
		mov 0(%r15), %r15
		mov -16(%r15), %r14
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rdx
		mov 0(%rdx), %rdx
		mov -8(%rdx), %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rsi
		mov 0(%rsi), %rsi
		mov -16(%rsi), %rbx
# INSTRUCTION_CONST
		mov $1, %rdi
# INSTRUCTION_ADD
		add %rdi, %rbx
# INSTRUCTION_CONST
		mov $8, %r8
# INSTRUCTION_MUL
		imul %r8, %rbx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r14, (%rcx, %rbx,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %r10
		mov 0(%r10), %r10
		mov -16(%r10), %r9
# INSTRUCTION_CONST
		mov $1, %r11
# INSTRUCTION_ADD
		add %r9, %r11
# COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE
		leaq staticLink, %rax
		mov 0(%rax), %rax
		mov %r11, -16(%rax)
# INSTRUCTION_JMP
		jmp while_cnd_0
# INSTRUCTION_LABEL
		while_0_end:
# INSTRUCTION_CONST
		mov $0, %r12
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %r12, -16(%rbp)
# INSTRUCTION_LABEL
		while_cnd_1:
# INSTRUCTION_CONST
		mov $1, %r13
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -16(%rbp), %r14
# INSTRUCTION_CONST
		mov $10, %r15
# INSTRUCTION_MINUS
		sub %r15, %r14
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rdx
		cmp $0, %r14
		seta %dl
		movsx %dl, %r14
		pop %rdx
# INSTRUCTION_CMP
		cmp %r14, %r13
# INSTRUCTION_JE
		je while_1_begin
# INSTRUCTION_JMP
		jmp while_1_end
# INSTRUCTION_LABEL
		while_1_begin:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rdx
		mov 0(%rdx), %rdx
		mov -8(%rdx), %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rsi
		mov 0(%rsi), %rsi
		mov -16(%rsi), %rbx
# INSTRUCTION_CONST
		mov $8, %rdi
# INSTRUCTION_CONST
		mov $1, %r8
# INSTRUCTION_ADD
		add %r8, %rbx
# INSTRUCTION_MUL
		imul %rdi, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rbx,1), %rcx
# INSTRUCTION_WRITE
		movq %rcx, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %r10
		mov 0(%r10), %r10
		mov -16(%r10), %r9
# INSTRUCTION_CONST
		mov $1, %r11
# INSTRUCTION_ADD
		add %r9, %r11
# COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE
		leaq staticLink, %rax
		mov 0(%rax), %rax
		mov %r11, -16(%rax)
# INSTRUCTION_JMP
		jmp while_cnd_1
# INSTRUCTION_LABEL
		while_1_end:
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
mov $60, %rax
mov $0, %rdi
syscall
