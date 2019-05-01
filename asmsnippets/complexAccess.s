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
	subq $64, %rsp
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
		mov $32, %r15
		imul %r14, %r15
		add %r15, %rax
		mov %rax, %rdi
		mov $12, %rax
		syscall
		pop %rcx
# COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK
        mov %rcx, -16(%rbp)
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
		mov $32, %r15
		imul %r14, %r15
		add %r15, %rax
		mov %rax, %rdi
		mov $12, %rax
		syscall
		pop %rsi
# COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK
        mov -16(%rbp), %rdi
        mov %rsi, 8(%rdi)
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
		mov $32, %r15
		imul %r14, %r15
		add %r15, %rax
		mov %rax, %rdi
		mov $12, %rax
		syscall
		pop %rsi
# COMPLEX_LOAD_VARIABLE_POINTER_FROM_STACK
        mov -16(%rbp), %rdi
        mov 8(%rdi), %rdi
        mov %rsi, 16(%rdi)
# COMPLEX_ALLOCATE_END
		pop %r14
		pop %r15
		pop %rax
		pop %rdi

		mov $69, %rax

        mov -16(%rbp), %rdi
        mov 8(%rdi), %rdi
        mov 16(%rdi), %rdi
        mov %rax, 8(%rdi)

        mov -16(%rbp), %rdi
        mov 8(%rdi), %rdi
        mov 16(%rdi), %rdi
        mov 8(%rdi), %rax

        movq %rax, %rsi
        movq $intprint, %rdi
        movq $0, %rax
        call printf
        
# METADATA_END_BODY_BLOCK
mov $60, %rax
mov $0, %rdi
syscall
