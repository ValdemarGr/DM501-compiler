.section .data
staticLink:
	.space 24
intprint:
	.asciz "%i\n"
gcHeapOne:
    .space 32
gcHeapTwo:
    .space 32
metaHashMap:
    .space 16
metaLambdaList:
    .space 16
.section .text
.global main
.extern printf
# METADATA_BEGIN_BODY_BLOCK
# VAR clsB
# VAR clsA
# METADATA_CREATE_MAIN
.type garbageCollectStructure, @function
garbageCollectStructure:
    push %rbp
    movq %rsp, %rbp
	subq $56, %rsp



    movq %rbp,%rsp
    pop %rbp
    ret

.type garbageCollectBFS, @function
garbageCollectBFS:
    push %rbp
    movq %rsp, %rbp

    push %rax
    push %rcx
    push %rdx
    push %rbx
    push %rsi
    push %rdi
    push %r8
    push %r9
    push %r10
    push %r11
    push %r12
    push %r13
    push %r14
    push %r15

    #reset heap counter for both &
    # swap in use values
    leaq gcHeapTwo, %rsi
    negq 0(%rsi)
    movq $0, 8(%rsi)
    leaq gcHeapOne, %rsi
    negq 0(%rsi)
    movq $0, 8(%rsi)

    movq 0(%rsi), %r12
    cmp $1, %r12
    je loadHeapEnd
    leaq gcHeapTwo, %rsi
    loadHeapEnd:

	mov 16(%rbp), %r15
    # we will find the amount of ptrs in -16
    mov -16(%r15), %r14
    mov %r14, %r8
    #add 1 + the size of the array list to begin the fetch phase
    inc %r8
    #times 8 because of sizes
    imulq $8, %r8

    mov $24, %r13 # offset

    #initial pass moves all ptrs over
    gcLoopBegin:
    cmp $0, %r14
    je gcLoopEnd
        mov (%r15, %r13, 1), %rdi #rdi has offset
        #mul by factor
        imulq $8, %rdi
        #add the constant offset
        add %r8, %rdi
        #%r12 is now our offset to our first pointer
        movq (%r15, %rdi, 1), %rcx

        # move all fields

        # get size
        mov -8(%rcx), %rax
        cmp $-1, %rax
        jne forLambdaEnd
        forLambda:
            mov $24, %rax
            jmp skipRec
        forLambdaEnd:

        add $8, %rax

        #get ptr count
        movq (%rcx, %rax, 1), %rdi
        #add the ptr count slot
        add $8, %rax

        cmpq $-1, %rdi
        je skipRec
            imulq $8, %rdi
            add %rdi, %rax
            #rax now has the structure size
        skipRec:

        # subtract 8 from rcx to find where to move from
        sub $8, %rcx
        # reserve new space on heap

        # new heap struct is in rsi
        # in rcx we have the pointer to the beginning of the struct in old heap
        # in rax we have the length of the struct * 8
        # we have r12 free
        # we have rdi free

        # save new positional ptr
        movq 24(%rsi), %r12
        movq 8(%rsi), %rdi
        add %rdi, %r12
        mov $0, %rbx

        heapMover:
        cmp $0, %rax
        je heapMoverEnd

            # move rcx + indexer(rbx) to top of new heap
            movq (%rcx, %rbx, 1), %rdx

            movq %rdx, (%r12, %rdi, 1)

            add $8, %rdi
            add $8, %rbx
        jmp heapMover
        heapMoverEnd:

        mov %rdi, 8(%rsi)

        #Now we have to restore the stack pointer
        # we hide size ptr
        subq $8, %r12
        #move r12 to stack position working in

        mov (%r15, %r13, 1), %rdi #rdi has offset
        #mul by factor
        imulq $8, %rdi
        #add the constant offset
        add %r8, %rdi
        #r12 is now our offset to our first pointer
        movq %r12, (%r15, %rdi, 1)


        #add the moved struct to the new heap

        add $8, %r13
        dec %r14
        jmp gcLoopBegin
    gcLoopEnd:

    # if we are main, quit
    leaq staticLink, %rcx #used for comparison with main
    mov 0(%rcx), %rcx
    cmp %r15, %rcx
    je gcRet

    #recurse
    pushq -8(%r15)
    call garbageCollect
    pop %r15

    gcRet:

    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rdi
    pop %rsi
    pop %rbx
    pop %rdx
    pop %rcx
    pop %rax

    movq %rbp,%rsp
    pop %rbp
    ret


.type garbageCollect, @function
garbageCollect:
    push %rbp
    movq %rsp, %rbp

    pushq 16(%rbp)
    call garbageCollectBFS
    pop %rax

    leaq gcHeapOne, %r15
    cmp $1, 0(%r15)
    je heapSelectorEnd
    leaq gcHeapTwo, %r15
    heapSelectorEnd:



    mov %rbp,%rsp
    pop %rbp
    ret

.type garbageCollectAllocate, @function
garbageCollectAllocate:
    push %rbp
    movq %rsp, %rbp
    subq $32, %rsp

    push %r15
    push %r14

    #args
    #rbp
    #desired byte size

    movq 16(%rbp), %rax
    movq %rax, -16(%rbp)
    movq 24(%rbp), %rax
    movq %rax, -8(%rbp)

    leaq gcHeapOne, %r15
    cmp $1, 0(%r15)
    je heapSelectorEndAlloc
    leaq gcHeapTwo, %r15
    heapSelectorEndAlloc:

    # current heap position
    movq 8(%r15), %r14
    # add new size
    movq -8(%rbp), %rax
    addq %r14, %rax
    movq %rax, 8(%r15)

    # old size is still in r14
    movq 24(%r15), %rax
    addq %r14, %rax

    pop %r14
    pop %r15

    mov %rbp,%rsp
    pop %rbp
    ret
# METADATA_BEGIN_BODY_BLOCK
# VAR clsB
# VAR clsA
# METADATA_CREATE_MAIN
	main:
	push %rbp
mov %rbp, %rax
	movq %rsp, %rbp
push %rax
		# 20 mb

    mov $0, %rdi
    mov $12, %rax
    syscall
    #move heap into one
    leaq gcHeapOne, %r15
    movq %rax, 24(%r15)
    movq $10000, 16(%r15)
    movq $0, 8(%r15)
    movq $1, 0(%r15)
    addq $10000, %rax
    push %rax
    movq %rax, %rdi
    mov $12, %rax
    syscall
    pop %rax
    #top should be in rax

    #move heap into one

    leaq gcHeapTwo, %r15
    movq %rax, 24(%r15)
    movq $10000, 16(%r15)
    movq $0, 8(%r15)
    movq $0, 0(%r15)

    addq $10000, %rax
    movq %rax, %rdi
    mov $12, %rax
    syscall
	subq $64, %rsp
	popq %rax
	movq %rax, -8(%rbp)
	movq $2, -16(%rbp)
	movq $0, -24(%rbp)
	movq $1, -32(%rbp)
	leaq staticLink, %rax
	movq %rbp, (%rax)
# INSTRUCTION_CONST
		mov $3, %rcx
# COMPLEX_ALLOCATE
		movq $8, %rdx
		imulq %rcx, %rdx
# ALLOC_RECORD_CLASS
		addq $40, %rdx
		pushq %rdx
		pushq %rbp
		call garbageCollectAllocate
		movq %rcx, 0(%rax)
		addq $8, %rdx
		movq $3, (%rax, %rdx, 1)
		movq $0, 0(%rax, %rdx, 1)
		movq $1, 8(%rax, %rdx, 1)
		movq $2, 16(%rax, %rdx, 1)
		popq %rdx
		popq %rdx
		addq $8, %rax
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -40(%rbp)
# INSTRUCTION_CONST
		mov $22, %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rdi
		mov 0(%rdi), %rdi
		mov -40(%rdi), %rsi
# INSTRUCTION_CONST
		mov $8, %r8
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rbx, (%rsi, %r8,1)
# COMPLEX_RIP_LAMBDA_LOAD
		leaq lambda_0(%rip), %r9
# INSTRUCTION_CONST
		mov $2, %r10
# COMPLEX_ALLOCATE
		movq $8, %r11
		imulq %r10, %r11
# ALLOC_LAMBDA
		addq $8, %r11
		pushq %r11
		pushq %rbp
		call garbageCollectAllocate
		movq $-1, 0(%rax)
		popq %r11
		popq %r11
		addq $8, %rax
# INSTRUCTION_CONST
		mov $0, %r12
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r9, (%rax, %r12,1)
# INSTRUCTION_COPY
		mov %rax, %r15
# INSTRUCTION_CONST
		mov $8, %rcx
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rax, (%r15, %rcx,1)
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rbx
		mov 0(%rbx), %rbx
		mov -40(%rbx), %rdx
# INSTRUCTION_CONST
		mov $16, %rsi
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r15, (%rdx, %rsi,1)
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_CONST
		mov $69, %rdi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r8
# INSTRUCTION_CONST
		mov $0, %r9
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rdi, (%r8, %r9,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r10
# INSTRUCTION_CONST
		mov $0, %r11
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r10, %r11,1), %r10
# INSTRUCTION_WRITE
		movq %r10, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r12
# INSTRUCTION_CONST
		mov $0, %r13
# INSTRUCTION_ADD
		add %r13, %r12
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %r12, -48(%rbp)
# INSTRUCTION_CONST
		mov $55, %r14
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r15
# INSTRUCTION_CONST
		mov $0, %rcx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r14, (%r15, %rcx,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rdx
# INSTRUCTION_CONST
		mov $0, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdx, %rbx,1), %rdx
# INSTRUCTION_WRITE
		movq %rdx, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# INSTRUCTION_CONST
		mov $42, %rsi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rdi
# INSTRUCTION_CONST
		mov $0, %r8
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rsi, (%rdi, %r8,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r9
# INSTRUCTION_CONST
		mov $0, %r10
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r9, %r10,1), %r9
# INSTRUCTION_WRITE
		movq %r9, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# INSTRUCTION_CONST
		mov $99, %r11
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r12
# INSTRUCTION_CONST
		mov $0, %r13
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r11, (%r12, %r13,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r14
# INSTRUCTION_CONST
		mov $16, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r14, %r15,1), %r14
# COMPLEX_SAVE_STATIC_LINK
		leaq staticLink, %rcx
		push 0(%rcx)
# INSTRUCTION_CONST
		mov $8, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r14, %rdx,1), %r14
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %r14
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rbx
# INSTRUCTION_CONST
		mov $16, %rsi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rsi,1), %rbx
# INSTRUCTION_CONST
		mov $0, %rdi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rdi,1), %rbx
# INSTRUCTION_REGISTER_CALL
		call *%rbx
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %r8
		pop 0(%r8)
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
mov $60, %rax
mov $0, %rdi
syscall
# INSTRUCTION_FUNCTION_LABEL
.type lambda_0, @function
lambda_0:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
push %rax
	subq $40, %rsp
	popq %rax
	movq %rax, -8(%rbp)
	movq $0, -16(%rbp)
	leaq staticLink, %r13
	movq %rbp, 16(%r13)
# METADATA_FUNCTION_ARGUMENT
	mov 16(%rbp), %r14
	mov %r14, -24(%rbp)
# METADATA_DEBUG_INFO
	# CLASS LOAD
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -24(%rbp), %r15
# INSTRUCTION_CONST
	mov $0, %rcx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
	mov (%r15, %rcx,1), %r15
# INSTRUCTION_WRITE
	movq %r15, %rsi
	movq $intprint, %rdi
	movq $0, %rax
	call printf
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
