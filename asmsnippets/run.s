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
# VAR bar
# VAR list
# VAR origin
# INSTRUCTION_FUNCTION_LABEL
.type sum, @function
sum:
push %rbp
mov %rsp,%rbp
	subq $32, %rsp
	leaq staticLink, %rax
	movq %rbp, 8(%rax)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov 8(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov 16(%rbp), %rdx
# INSTRUCTION_ADD
	add %rcx, %rdx
# INSTRUCTION_RETURN
	mov %rdx, %rax
	mov %rbp,%rsp
pop %rbp
ret
# METADATA_END_BODY_BLOCK
# METADATA_CREATE_MAIN
main:
	push %rbp
	movq %rsp, %rbp
subq $80, %rsp
leaq staticLink, %rax
	movq %rbp, (%rax)
# INSTRUCTION_CONST
	mov $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
	mov %rcx, 3(%rbp)
# INSTRUCTION_CONST
	mov $1, %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov 3(%rbp), %rbx
# INSTRUCTION_CMP
	cmp %rbx, %rdx
# INSTRUCTION_JE
	je if_0_begin
# INSTRUCTION_JMP
	jmp el_0_begin
# INSTRUCTION_LABEL
	if_0_begin:
# INSTRUCTION_CONST
	mov $4, %rsi
# COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE
	leaq staticLink, %rax
	mov 0(%rax), %rax
	mov %rsi, 8(%rax)
# INSTRUCTION_JMP
	jmp ifel_0_end
# INSTRUCTION_LABEL
	el_0_begin:
# INSTRUCTION_CONST
	mov $5, %rdi
# COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE
	leaq staticLink, %rax
	mov 0(%rax), %rax
	mov %rdi, 8(%rax)
# INSTRUCTION_LABEL
	ifel_0_end:
# INSTRUCTION_CONST
	mov $1, %r8
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov 8(%rbp), %r9
# INSTRUCTION_CONST
	mov $4, %r10
# INSTRUCTION_MINUS
	sub %r9, %r10
# COMPLEX_CONSTRAIN_BOOLEAN
	push %rdx
	cmp $0, %r10
	seta %dl
	movsx %dl, %r10
	pop %rdx
# INSTRUCTION_CMP
	cmp %r10, %r8
# INSTRUCTION_JE
	je if_1_begin
# INSTRUCTION_JMP
	jmp if_1_end
# INSTRUCTION_LABEL
	if_1_begin:
# INSTRUCTION_CONST
	mov $1, %r11
# INSTRUCTION_WRITE
	movq %r11, %rsi
	movq $intprint, %rdi
	movq $0, %rax
	call printf
# INSTRUCTION_LABEL
	if_1_end:
# INSTRUCTION_CONST
	mov $16, %r12
# COMPLEX_ALLOCATE
	push %rdi
	push %rax
	push %r15
	push %r14
	mov %r12, %r14
	mov $0, %rdi
	mov $12, %rax
	syscall
	push %rax
	mov $93971510102607, %r15
	imul %r14, %r15
	add %r15, %rax
	mov %rax, %rdi
	mov $12, %rax
	syscall
	pop %r13
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
	mov %r13, 48(%rbp)
# COMPLEX_ALLOCATE_END
	pop %r14
	pop %r15
	pop %rax
	pop %rdi
# INSTRUCTION_CONST
	mov $1, %r14
# COMPLEX_ALLOCATE
	push %rdi
	push %rax
	push %r15
	push %r14
	mov %r14, %r14
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
	pop %r15
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
	mov %r15, 56(%rbp)
# COMPLEX_ALLOCATE_END
	pop %r14
	pop %r15
	pop %rax
	pop %rdi
# INSTRUCTION_CONST
	mov $2, %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov 56(%rbp), %rcx
# INSTRUCTION_CONST
	mov $0, %rdx
# INSTRUCTION_MOVE_TO_OFFSET
	mov %rax, (%rcx, %rdx,1)
# INSTRUCTION_CONST
	mov $3, %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov 56(%rbp), %rsi
# INSTRUCTION_CONST
	mov $8, %rdi
# INSTRUCTION_MOVE_TO_OFFSET
	mov %rbx, (%rsi, %rdi,1)
# INSTRUCTION_CONST
	mov $0, %r8
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov 48(%rbp), %r9
# INSTRUCTION_CONST
	mov $0, %r10
# INSTRUCTION_CONST
	mov $1, %r11
# INSTRUCTION_MOVE_TO_OFFSET
	mov %r8, (%r9, %r10,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov 8(%rbp), %r12
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov 56(%rbp), %r13
# INSTRUCTION_CONST
	mov $8, %r14
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
	mov (%r13, %r14,1), %r13
# INSTRUCTION_MINUS
	sub %r12, %r13
# COMPLEX_CONSTRAIN_BOOLEAN
	push %rdx
	cmp $0, %r13
	seta %dl
	movsx %dl, %r13
	pop %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov 48(%rbp), %r15
# INSTRUCTION_CONST
	mov $1, %rax
# INSTRUCTION_CONST
	mov $1, %rcx
# INSTRUCTION_MOVE_TO_OFFSET
	mov %r13, (%r15, %rax,1)
# INSTRUCTION_CONST
	mov $0, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
	mov %rdx, 16(%rbp)
# INSTRUCTION_LABEL
	while_cnd_0:
# INSTRUCTION_CONST
	mov $1, %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov 16(%rbp), %rsi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov 8(%rbp), %rdi
# INSTRUCTION_MINUS
	sub %rdi, %rsi
# INSTRUCTION_CONST
	mov $1, %r8
# INSTRUCTION_ADD
	add %r8, %rsi
# COMPLEX_CONSTRAIN_BOOLEAN
	push %rdx
	cmp $0, %rsi
	seta %dl
	movsx %dl, %rsi
	pop %rdx
# INSTRUCTION_CMP
	cmp %rsi, %rbx
# INSTRUCTION_JE
	je while_0_begin
# INSTRUCTION_JMP
	jmp while_0_end
# INSTRUCTION_LABEL
	while_0_begin:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
	leaq staticLink, %rax
	mov 0(%rax), %rax
	mov 16(%rax), %r9
# INSTRUCTION_WRITE
	movq %r9, %rsi
	movq $intprint, %rdi
	movq $0, %rax
	call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
	leaq staticLink, %rax
	mov 0(%rax), %rax
	mov 16(%rax), %r10
# INSTRUCTION_CONST
	mov $1, %r11
# INSTRUCTION_ADD
	add %r10, %r11
# COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE
	leaq staticLink, %rax
	mov 0(%rax), %rax
	mov %r11, 16(%rax)
# INSTRUCTION_JMP
	jmp while_cnd_0
# INSTRUCTION_LABEL
	while_0_end:
# METADATA_END_BODY_BLOCK
mov $60, %rax
mov $0, %rdi
syscall
