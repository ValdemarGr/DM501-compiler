	.file	"hashmap.c"
	.text
	.p2align 4,,15
	.type	hashFromKey.part.1, @function
hashFromKey.part.1:
.LFB35:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	movq	%rdi, %rbx
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	movq	8(%rsi), %rbp
	movq	%rbp, %rdi
	call	strlen@PLT
	movq	%rax, %rcx
	testq	%rax, %rax
	je	.L1
	movsbl	0(%rbp), %ecx
	cmpq	$1, %rax
	je	.L3
	leaq	1(%rbp), %rdx
	addq	%rbp, %rax
	.p2align 4,,10
	.p2align 3
.L4:
	movsbl	(%rdx), %esi
	addq	$1, %rdx
	leal	(%rsi,%rcx,2), %ecx
	cmpq	%rdx, %rax
	jne	.L4
.L3:
	movslq	%ecx, %rax
	xorl	%edx, %edx
	divq	(%rbx)
	movq	%rdx, %rcx
.L1:
	addq	$8, %rsp
	.cfi_def_cfa_offset 24
	movq	%rcx, %rax
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE35:
	.size	hashFromKey.part.1, .-hashFromKey.part.1
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"Malloc(%d) failed.\n"
	.text
	.p2align 4,,15
	.globl	Malloc
	.type	Malloc, @function
Malloc:
.LFB22:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movl	%edi, %edi
	movq	%rdi, %rbx
	call	malloc@PLT
	testq	%rax, %rax
	je	.L18
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
.L18:
	.cfi_restore_state
	movq	stderr(%rip), %rdi
	movl	%ebx, %edx
	leaq	.LC0(%rip), %rsi
	call	fprintf@PLT
	movq	stderr(%rip), %rdi
	call	fflush@PLT
	call	abort@PLT
	.cfi_endproc
.LFE22:
	.size	Malloc, .-Malloc
	.p2align 4,,15
	.globl	makeIntKey
	.type	makeIntKey, @function
makeIntKey:
.LFB23:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movl	%edi, %ebx
	movl	$16, %edi
	call	malloc@PLT
	movl	$0, (%rax)
	movl	%ebx, 8(%rax)
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE23:
	.size	makeIntKey, .-makeIntKey
	.p2align 4,,15
	.globl	makeCharKey
	.type	makeCharKey, @function
makeCharKey:
.LFB24:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	%rdi, %rbx
	movl	$16, %edi
	call	malloc@PLT
	movl	$1, (%rax)
	movq	%rbx, 8(%rax)
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE24:
	.size	makeCharKey, .-makeCharKey
	.p2align 4,,15
	.globl	maxBy
	.type	maxBy, @function
maxBy:
.LFB25:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	subq	$8, %rsp
	.cfi_def_cfa_offset 64
	cmpq	$0, (%rdi)
	je	.L29
	movq	%rsi, %r12
	movq	%rdi, %r15
	xorl	%r14d, %r14d
	xorl	%ebp, %ebp
	xorl	%r13d, %r13d
	.p2align 4,,10
	.p2align 3
.L28:
	movq	8(%r15), %rax
	movq	(%rax,%r14,8), %rbx
	testq	%rbx, %rbx
	je	.L25
	.p2align 4,,10
	.p2align 3
.L27:
	movq	8(%rbx), %rdi
	call	*%r12
	cmpq	%rbp, %rax
	jbe	.L26
	movq	%rax, %rbp
	movq	%rbx, %r13
.L26:
	movq	16(%rbx), %rbx
	testq	%rbx, %rbx
	jne	.L27
.L25:
	addq	$1, %r14
	cmpq	%r14, (%r15)
	ja	.L28
.L23:
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	movq	%r13, %rax
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	ret
.L29:
	.cfi_restore_state
	xorl	%r13d, %r13d
	jmp	.L23
	.cfi_endproc
.LFE25:
	.size	maxBy, .-maxBy
	.p2align 4,,15
	.globl	minBy
	.type	minBy, @function
minBy:
.LFB26:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	subq	$8, %rsp
	.cfi_def_cfa_offset 64
	cmpq	$0, (%rdi)
	je	.L42
	movq	%rsi, %r12
	movq	%rdi, %r15
	xorl	%r14d, %r14d
	movl	$4294967295, %ebp
	xorl	%r13d, %r13d
	.p2align 4,,10
	.p2align 3
.L41:
	movq	8(%r15), %rax
	movq	(%rax,%r14,8), %rbx
	testq	%rbx, %rbx
	je	.L38
	.p2align 4,,10
	.p2align 3
.L40:
	movq	8(%rbx), %rdi
	call	*%r12
	cmpq	%rbp, %rax
	jnb	.L39
	movq	%rax, %rbp
	movq	%rbx, %r13
.L39:
	movq	16(%rbx), %rbx
	testq	%rbx, %rbx
	jne	.L40
.L38:
	addq	$1, %r14
	cmpq	%r14, (%r15)
	ja	.L41
.L36:
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	movq	%r13, %rax
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	ret
.L42:
	.cfi_restore_state
	xorl	%r13d, %r13d
	jmp	.L36
	.cfi_endproc
.LFE26:
	.size	minBy, .-minBy
	.p2align 4,,15
	.globl	findAtleastLargerThanNPrime
	.type	findAtleastLargerThanNPrime, @function
findAtleastLargerThanNPrime:
.LFB27:
	.cfi_startproc
	cmpq	$5, %rdi
	jbe	.L55
	.p2align 4,,10
	.p2align 3
.L54:
	testb	$1, %dil
	je	.L51
	movl	$3, %ecx
	jmp	.L52
	.p2align 4,,10
	.p2align 3
.L53:
	movq	%rdi, %rax
	xorl	%edx, %edx
	divq	%rcx
	addq	$1, %rcx
	testq	%rdx, %rdx
	je	.L51
.L52:
	cmpq	%rcx, %rdi
	ja	.L53
	movq	%rdi, %rax
	ret
	.p2align 4,,10
	.p2align 3
.L51:
	addq	$1, %rdi
	cmpq	$2, %rdi
	ja	.L54
	xorl	%edi, %edi
	movq	%rdi, %rax
	ret
.L55:
	movl	$5, %edi
	movq	%rdi, %rax
	ret
	.cfi_endproc
.LFE27:
	.size	findAtleastLargerThanNPrime, .-findAtleastLargerThanNPrime
	.p2align 4,,15
	.globl	initMap
	.type	initMap, @function
initMap:
.LFB28:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	movq	%rdi, %rbx
	movl	$16, %edi
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	call	Malloc
	movq	%rax, %rbp
	cmpq	$5, %rbx
	jbe	.L73
	.p2align 4,,10
	.p2align 3
.L71:
	testb	$1, %bl
	je	.L66
	movl	$3, %ecx
	jmp	.L67
	.p2align 4,,10
	.p2align 3
.L68:
	movq	%rbx, %rax
	xorl	%edx, %edx
	divq	%rcx
	addq	$1, %rcx
	testq	%rdx, %rdx
	je	.L66
.L67:
	cmpq	%rcx, %rbx
	ja	.L68
	leaq	(%rbx,%rbx,2), %rdi
	salq	$3, %rdi
.L65:
	movq	%rbx, 0(%rbp)
	call	malloc@PLT
	xorl	%edx, %edx
	movq	%rax, 8(%rbp)
	.p2align 4,,10
	.p2align 3
.L69:
	movq	$0, (%rax,%rdx,8)
	addq	$1, %rdx
	cmpq	%rdx, %rbx
	ja	.L69
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	movq	%rbp, %rax
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L66:
	.cfi_restore_state
	addq	$1, %rbx
	cmpq	$2, %rbx
	ja	.L71
	xorl	%edi, %edi
	movq	$0, 0(%rbp)
	call	malloc@PLT
	movq	%rax, 8(%rbp)
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	movq	%rbp, %rax
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L73:
	.cfi_restore_state
	movl	$120, %edi
	movl	$5, %ebx
	jmp	.L65
	.cfi_endproc
.LFE28:
	.size	initMap, .-initMap
	.p2align 4,,15
	.globl	hashFromKey
	.type	hashFromKey, @function
hashFromKey:
.LFB29:
	.cfi_startproc
	cmpl	$1, (%rsi)
	jne	.L85
	jmp	hashFromKey.part.1
	.p2align 4,,10
	.p2align 3
.L85:
	movslq	8(%rsi), %rax
	xorl	%edx, %edx
	divq	(%rdi)
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE29:
	.size	hashFromKey, .-hashFromKey
	.p2align 4,,15
	.globl	insert
	.type	insert, @function
insert:
.LFB30:
	.cfi_startproc
	pushq	%r13
	.cfi_def_cfa_offset 16
	.cfi_offset 13, -16
	pushq	%r12
	.cfi_def_cfa_offset 24
	.cfi_offset 12, -24
	movq	%rdx, %r12
	pushq	%rbp
	.cfi_def_cfa_offset 32
	.cfi_offset 6, -32
	movq	%rdi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 40
	.cfi_offset 3, -40
	movq	%rsi, %rbx
	subq	$8, %rsp
	.cfi_def_cfa_offset 48
	cmpl	$1, (%rsi)
	jne	.L87
	call	hashFromKey.part.1
	movq	%rax, %r13
.L88:
	movl	$24, %edi
	call	Malloc
	movq	8(%rbp), %rdx
	movq	%r12, 8(%rax)
	leaq	(%rdx,%r13,8), %rdx
	movq	$0, 16(%rax)
	movq	(%rdx), %rcx
	movq	%rbx, (%rax)
	testq	%rcx, %rcx
	jne	.L90
	jmp	.L94
	.p2align 4,,10
	.p2align 3
.L92:
	movq	%rdx, %rcx
.L90:
	movq	16(%rcx), %rdx
	testq	%rdx, %rdx
	jne	.L92
	movq	%rax, 16(%rcx)
.L86:
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	popq	%rbx
	.cfi_def_cfa_offset 32
	popq	%rbp
	.cfi_def_cfa_offset 24
	popq	%r12
	.cfi_def_cfa_offset 16
	popq	%r13
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L87:
	.cfi_restore_state
	movslq	8(%rsi), %rax
	xorl	%edx, %edx
	divq	(%rdi)
	movq	%rdx, %r13
	jmp	.L88
.L94:
	movq	%rax, (%rdx)
	jmp	.L86
	.cfi_endproc
.LFE30:
	.size	insert, .-insert
	.p2align 4,,15
	.globl	getTail
	.type	getTail, @function
getTail:
.LFB31:
	.cfi_startproc
	movq	%rdi, %rax
	testq	%rdi, %rdi
	jne	.L97
	jmp	.L102
	.p2align 4,,10
	.p2align 3
.L98:
	movq	%rdx, %rax
.L97:
	movq	16(%rax), %rdx
	testq	%rdx, %rdx
	jne	.L98
	ret
.L102:
	ret
	.cfi_endproc
.LFE31:
	.size	getTail, .-getTail
	.p2align 4,,15
	.globl	get
	.type	get, @function
get:
.LFB32:
	.cfi_startproc
	pushq	%r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	movq	%rsi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	movl	(%rsi), %r12d
	movq	%rdi, %rbx
	cmpl	$1, %r12d
	jne	.L104
	call	hashFromKey.part.1
	movq	%rax, %rdx
.L105:
	movq	8(%rbx), %rcx
	movq	(%rcx,%rdx,8), %rbx
	testq	%rbx, %rbx
	je	.L103
	movq	(%rbx), %rax
	movl	(%rax), %edx
	testl	%edx, %edx
	je	.L122
	cmpl	$1, %r12d
	jne	.L110
	cmpl	$1, %edx
	jne	.L110
	movq	8(%rax), %rsi
	movq	8(%rbp), %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	movl	$0, %eax
	cmovne	%rax, %rbx
.L103:
	movq	%rbx, %rax
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L122:
	.cfi_restore_state
	testl	%r12d, %r12d
	jne	.L110
	movl	8(%rax), %eax
	cmpl	%eax, 8(%rbp)
	movl	$0, %eax
	cmovne	%rax, %rbx
	movq	%rbx, %rax
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L104:
	.cfi_restore_state
	movslq	8(%rsi), %rax
	xorl	%edx, %edx
	divq	(%rdi)
	jmp	.L105
	.p2align 4,,10
	.p2align 3
.L110:
	xorl	%ebx, %ebx
	movq	%rbx, %rax
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE32:
	.size	get, .-get
	.section	.rodata.str1.1
.LC1:
	.string	"%i\n"
	.section	.text.startup,"ax",@progbits
	.p2align 4,,15
	.globl	main
	.type	main, @function
main:
.LFB33:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movl	$40, %edi
	subq	$16, %rsp
	.cfi_def_cfa_offset 32
	movq	%fs:40, %rax
	movq	%rax, 8(%rsp)
	xorl	%eax, %eax
	call	initMap
	movl	$16, %edi
	movl	$5, 4(%rsp)
	movq	%rax, %rbx
	call	malloc@PLT
	leaq	4(%rsp), %rdx
	movq	%rbx, %rdi
	movl	$0, (%rax)
	movq	%rax, %rsi
	movl	$50, 8(%rax)
	call	insert
	movl	$16, %edi
	call	malloc@PLT
	movq	%rbx, %rdi
	movl	$0, (%rax)
	movq	%rax, %rsi
	movl	$50, 8(%rax)
	call	get
	leaq	.LC1(%rip), %rdi
	movq	8(%rax), %rax
	movl	(%rax), %esi
	xorl	%eax, %eax
	call	printf@PLT
	movq	8(%rsp), %rcx
	xorq	%fs:40, %rcx
	jne	.L126
	addq	$16, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 16
	xorl	%eax, %eax
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
.L126:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE33:
	.size	main, .-main
	.ident	"GCC: (GNU) 8.3.0"
	.section	.note.GNU-stack,"",@progbits
