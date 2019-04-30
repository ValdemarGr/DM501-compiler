.include "print.asm"
.section .data
staticLink:
	.space 8
.LC0:
	.string "%i"
.LC1:
	.string "hello!"
.section .text
mov $60, %rax
mov $0, %rdi
syscall
[1;34mvar [0ma : [0;36mint[0m, ;
[1;34mvar [0mb : [0;36mint[0m, ;
[1;34mallocate [0ma;
[1;34mallocate [0mb;
a[1;34m = [0m5;
b[1;34m = [0m(9 + a);
[1;34mwrite [0ma;
[1;34mwrite [0mb;
.include "print.asm"
.section .data
staticLink:
	.space 8
.LC0:
	.string "%i"
.LC1:
	.string "hello!"
.section .text
# METADATA_BEGIN_BODY_BLOCK
# VAR a
# VAR b
# METADATA_CREATE_MAIN
	.global main
main:
	push %rbp
	movq %rsp, %rbp
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
# COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER
		mov -8(%rbp), %r9
		mov %r8, (%r9)
# INSTRUCTION_CONST
		mov $9, %r10
# COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK
		mov -8(%rbp), %r11
		mov (%r11), %r11
# INSTRUCTION_ADD
		add %r10, %r11
# COMPLEX_MOVE_TEMPORARY_VALUE_INTO_POINTER
		mov -16(%rbp), %r12
		mov %r11, (%r12)
# COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK
		mov -8(%rbp), %r13
		mov (%r13), %r13
# INSTRUCTION_WRITE
		push %r13
		#call print_number
		pop %r13
# COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK
		mov -16(%rbp), %r14
		mov (%r14), %r14
# INSTRUCTION_WRITE
		push %r14
		#call print_number
		pop %r14
# METADATA_END_BODY_BLOCK
mov $60, %rax
mov $0, %rdi
syscall
