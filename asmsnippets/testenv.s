.section .data
staticLink:
	.space 16
.type horse, @function
horse:
    push %rbp
    mov %rsp,%rbp
	leaq staticLink, %rax
	movq %rbp, 8(%rax)
	mov $0, %rcx
	mov $5, %rdx
	sub %rdx, %rcx
	mov %rcx, %rbx
	sar $31, %rbx
	add %rbx, %rcx
	xor %rcx, %rbx
	mov %rbx, -24(%rbp)
	mov $5, %rsi
	mov %rsi, -24(%rbp)
	mov $2, %rdi
	leaq staticLink, %r8
	movq 0(%r8), %r8
	movq -0(%r8), %r8
	movq %rdi, (%r8)
	movq -8(%rbp), %r9
	mov %r9, %rax
	mov %rbp,%rsp
    pop %rbp
    ret
.global main
main:
    leaq staticLink, %rax
    movq %rbp, (%rax)
	mov $5, %rcx
	push %rcx
	call horse
	pop %rdx
	mov %rdx, -8(%rbp)
    mov $60, %rax
    mov $0, %rdi
    syscall
