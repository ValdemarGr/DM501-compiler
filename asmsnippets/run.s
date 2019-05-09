.section .data
staticLink:
	.space 40
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
    #inc %r8
    #times 8 because of sizes
    imulq $8, %r8

    mov $-24, %r13 # offset

    #add initial offset
    movq $0, %r12
    subq %r8, %r12
    movq %r12, %r8

    add %r13, %r8

    #initial pass moves all ptrs over
    gcLoopBegin:
    cmp $0, %r14
    je gcLoopEnd
        mov (%r15, %r13, 1), %rdi #rdi has offset
        #mul by factor
        imulq $8, %rdi
        #negate because of stack access
        movq $0, %r12
        subq %rdi, %r12
        movq %r12, %rdi
        #add the constant offset
        add %r8, %rdi
        movq (%r15, %rdi, 1), %rcx

        #nullptr check
        cmp $0, %rcx
        je gcLoopEpilogue

        subq $8, %rcx
        # get size
        mov 0(%rcx), %rax

        #mul by 8 bc pointer size
        imulq $8, %rax

        #skip head
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
        # reserve new space on heap

        # new heap struct is in rsi
        # in rcx we have the pointer to the beginning of the struct in old heap
        # in rax we have the length of the struct * 8
        # we have r12 free
        # we have rdi free

        # save new positional ptr
        movq 24(%rsi), %r12 # r12 has new heap start
        movq 8(%rsi), %rdi # current heap position

        # save old heap position
        pushq %rdi

        mov $0, %rbx # rbx has 0

        heapMover:
        cmp $0, %rax
        je heapMoverEnd

            # move rcx + indexer(rbx) to top of new heap
            movq (%rcx, %rbx, 1), %rdx

            movq %rdx, (%r12, %rdi, 1)

            add $8, %rdi
            add $8, %rbx
            sub $8, %rax
        jmp heapMover
        heapMoverEnd:

        mov %rdi, 8(%rsi)

        #Now we have to restore the stack pointer
        #We have old heap start in r12
        popq %rdi

        movq 24(%rsi), %r12
        addq %rdi, %r12
        addq $8, %r12

        mov (%r15, %r13, 1), %rdi #rdi has offset
        #mul by factor
        imulq $8, %rdi
        #negate because of stack access
        movq $0, %rax
        subq %rdi, %rax
        movq %rax, %rdi
        #add the constant offset
        add %r8, %rdi
        #r12 is now our offset to our first pointer

        # place -1 and newptr in old heap
        movq $-1, 0(%rcx)
        movq %r12, 8(%rcx)

        movq %r12, (%r15, %rdi, 1)

        gcLoopEpilogue:

        subq $8, %r13
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
    call garbageCollectBFS
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
    pop 16(%rbp)

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

    leaq gcHeapOne, %r15
    leaq gcHeapTwo, %r14
    cmp $1, 0(%r15)
    je heapSelectorEnd
    leaq gcHeapTwo, %r15
    leaq gcHeapOne, %r14
    heapSelectorEnd:

    # traverse new heap, if ptr to old heap found, move it to new heap, we know new heap is packed
    movq $0, %rdx # rdx is heap iterator
    movq 24(%r15), %rcx # rcx has actual heap ptr
    movq 8(%r15), %rax # rax has current new heap pos
    newHeapTraverseBegin:
        cmp %rdx, %rax
        je newHeapTraverseEnd

        # now to traversing ptrs
        # for this element get size
        movq (%rcx, %rdx, 1), %rbx # rbx has struct size
        movq %rbx, %r13
        # add 1 to skip size block
        add $1, %rbx
        # mul by 8 for block size
        imul $8, %rbx

        mov %rbx, %r10
        # add heap iter for absolute position
        add %rdx, %rbx

        movq (%rcx, %rbx, 1), %rsi # rbx how has ptr count

        movq %rsi, %r11
        imul $8, %r11
        add %r10, %r11
        addq $8, %r11
        pushq %r11

        # use rbx + 8 has first item
        addq $8, %rbx #now is first ptr
        selfPtrHeapMoveBegin:
            cmp $0, %rsi
            je selfPtrHeapMoveEnd

            # get the struct ptr offset
            movq (%rcx, %rbx, 1), %rdi # rdi has offset
            # copy struct position ptr
            movq %rdx, %r8
            # add 1 for header
            add $8, %r8
            #mul 8 ptr size
            imul $8, %rdi
            #add offset
            add %rdi, %r8

            pushq %r8

            movq (%rcx, %r8, 1), %r8

            # nullptr check
            cmp $0, %r8
            jne skipEndNullptr
            popq %r8
            jmp selfPtrHeapMoveEpilogue
            skipEndNullptr:

            # finally move the ptr to new heap space if it is in old
            # we can check this by seeing if the addr is between old start and old end
            # old is in r14
            movq 24(%r14), %r9 # begin
            movq 16(%r14), %r10 # end
            # add the pointer to the end offset
            addq %r9, %r10

            cmp %r9, %r8 #inverted because of gas xd
            jl beforeEpilogueBegin
            cmp %r8, %r10 #inverted because of gas xd
            jl beforeEpilogueBegin
            # if r8 < r9 || r10 < r8 skip

            # remove from top of stack

            jmp beforeEpilogueEnd
            beforeEpilogueBegin:
            popq %r8
            jmp selfPtrHeapMoveEpilogue
            beforeEpilogueEnd:
            # save offset for when we have to save this field

            # we actually have to move it
            # we can use r9 & r10 again
            # we need to find total size of the struct
            # the header can be used for this
            # grab header size by -8
            subq $8, %r8
            movq (%r8), %r9 # r9 has size

            cmp $-1, %r9 # if we have already moved it
            jne alreadyMovedEnd
                # now we can find the head pointer by adding 8
                movq 8(%r8), %r11

                # Where old ptr offset aboslute in structure we are working in
                popq %r8

                # hide head
                addq $8, %r11

                movq %r11, (%rcx, %r8, 1)

                jmp selfPtrHeapMoveEpilogue

            alreadyMovedEnd:

            # add 1 to size for ptr count index
            addq $1, %r9
            # mul 8 for ptr size
            imul $8, %r9
            # then move ptr count to r10
            movq (%r8, %r9, 1), %r10

            # mul 8 for size
            imul $8, %r10
            # add 8 for ptr count box
            addq $8, %r10

            # add the sizes
            addq %r9, %r10
            movq %r10, %r13

            structSizeEvalEnd:
            # size will be in r13
            # we can use r9 & r10 again

            # save old head
            movq %rax, %r11

            # copy heap iter
            # save old heap tail
            movq $0, %r9
            newHeapMoverBegin:
                cmp %r9, %r13
                je newHeapMoverEnd

                # move this heap block
                movq (%r8, %r9, 1), %r10
                movq %r10, (%rcx, %rax, 1)

                addq $8, %r9
                addq $8, %rax #inc heap top
                jmp newHeapMoverBegin
            newHeapMoverEnd:

            # Where old ptr offset aboslute in structure we are working in
            popq %r13

            # add the heap ptr to rax
            addq %rcx, %r11
            # hide head
            addq $8, %r11

            # update old heap with -1 and new ptr
            movq $-1, 0(%r8)
            movq %r11, 8(%r8)

            # and update the new heap
            movq %r11, (%rcx, %r13, 1)


        selfPtrHeapMoveEpilogue:

            addq $8, %rbx
            dec %rsi
            jmp selfPtrHeapMoveBegin
        selfPtrHeapMoveEnd:


    newHeapTraverseEpilogue:
        popq %r12
        addq %r12, %rdx

        jmp newHeapTraverseBegin
    newHeapTraverseEnd:

    movq 8(%r15), %rax # rax has current new heap pos



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
# VAR ib1
# VAR ib2
# VAR ib3
# VAR ib4
# VAR ib5
# VAR seq
# VAR aoib
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
	subq $144, %rsp
	popq %rax
	movq %rax, -8(%rbp)
	movq $7, -16(%rbp)
	movq $0, -24(%rbp)
	movq $0, -80(%rbp)
	movq $1, -32(%rbp)
	movq $0, -88(%rbp)
	movq $2, -40(%rbp)
	movq $0, -96(%rbp)
	movq $3, -48(%rbp)
	movq $0, -104(%rbp)
	movq $4, -56(%rbp)
	movq $0, -112(%rbp)
	movq $5, -64(%rbp)
	movq $0, -120(%rbp)
	movq $6, -72(%rbp)
	movq $0, -128(%rbp)
	leaq staticLink, %rax
	movq %rbp, (%rax)
# INSTRUCTION_CONST
		mov $5, %rcx
# COMPLEX_ALLOCATE
		movq $8, %rdx
		imulq %rcx, %rdx
# ALLOC_ARR_OF_PTR
		addq $8, %rdx
		pushq %rdx
		pushq %rbp
		call garbageCollectAllocate
		movq %rcx, 0(%rax)
# ALLOC_ARR_OF_PTR
		subq $0, %rdx
		movq $-1, (%rax, %rdx, 1)
		popq %rdx
		popq %rdx
		addq $8, %rax
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -128(%rbp)
# INSTRUCTION_CONST
		mov $1, %rbx
# COMPLEX_ALLOCATE
		movq $8, %rsi
		imulq %rbx, %rsi
# ALLOC_RECORD_CLASS
		addq $16, %rsi
		pushq %rsi
		pushq %rbp
		call garbageCollectAllocate
		movq %rbx, 0(%rax)
# ALLOC_RECORD_CLASS
		subq $8, %rsi
		movq $0, (%rax, %rsi, 1)
		popq %rsi
		popq %rsi
		addq $8, %rax
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -80(%rbp)
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_CONST
		mov $1, %rdi
# COMPLEX_ALLOCATE
		movq $8, %r8
		imulq %rdi, %r8
# ALLOC_RECORD_CLASS
		addq $16, %r8
		pushq %r8
		pushq %rbp
		call garbageCollectAllocate
		movq %rdi, 0(%rax)
# ALLOC_RECORD_CLASS
		subq $8, %r8
		movq $0, (%rax, %r8, 1)
		popq %r8
		popq %r8
		addq $8, %rax
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -88(%rbp)
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_CONST
		mov $1, %r9
# COMPLEX_ALLOCATE
		movq $8, %r10
		imulq %r9, %r10
# ALLOC_RECORD_CLASS
		addq $16, %r10
		pushq %r10
		pushq %rbp
		call garbageCollectAllocate
		movq %r9, 0(%rax)
# ALLOC_RECORD_CLASS
		subq $8, %r10
		movq $0, (%rax, %r10, 1)
		popq %r10
		popq %r10
		addq $8, %rax
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -96(%rbp)
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_CONST
		mov $1, %r11
# COMPLEX_ALLOCATE
		movq $8, %r12
		imulq %r11, %r12
# ALLOC_RECORD_CLASS
		addq $16, %r12
		pushq %r12
		pushq %rbp
		call garbageCollectAllocate
		movq %r11, 0(%rax)
# ALLOC_RECORD_CLASS
		subq $8, %r12
		movq $0, (%rax, %r12, 1)
		popq %r12
		popq %r12
		addq $8, %rax
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -104(%rbp)
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_CONST
		mov $1, %r13
# COMPLEX_ALLOCATE
		movq $8, %r14
		imulq %r13, %r14
# ALLOC_RECORD_CLASS
		addq $16, %r14
		pushq %r14
		pushq %rbp
		call garbageCollectAllocate
		movq %r13, 0(%rax)
# ALLOC_RECORD_CLASS
		subq $8, %r14
		movq $0, (%rax, %r14, 1)
		popq %r14
		popq %r14
		addq $8, %rax
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -112(%rbp)
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_CONST
		mov $3, %r15
# COMPLEX_ALLOCATE
		movq $8, %rcx
		imulq %r15, %rcx
# ALLOC_RECORD_CLASS
		addq $24, %rcx
		pushq %rcx
		pushq %rbp
		call garbageCollectAllocate
		movq %r15, 0(%rax)
# ALLOC_RECORD_CLASS
		subq $16, %rcx
		movq $1, (%rax, %rcx, 1)
		movq $0, 8(%rax, %rcx, 1)
		popq %rcx
		popq %rcx
		addq $8, %rax
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -120(%rbp)
# COMPLEX_RIP_LAMBDA_LOAD
		leaq lambda_0(%rip), %rdx
# INSTRUCTION_CONST
		mov $2, %rbx
# COMPLEX_ALLOCATE
		movq $8, %rsi
		imulq %rbx, %rsi
# ALLOC_LAMBDA
		addq $24, %rsi
		pushq %rsi
		pushq %rbp
		call garbageCollectAllocate
		movq $2, 0(%rax)
		subq $8, %rsi
		movq $0, (%rax, %rsi, 1)
		movq $0, 8(%rax, %rsi, 1)
		popq %rsi
		popq %rsi
		addq $8, %rax
# INSTRUCTION_CONST
		mov $0, %rdi
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rdx, (%rax, %rdi,1)
# INSTRUCTION_COPY
		mov %rax, %r10
# INSTRUCTION_CONST
		mov $8, %r11
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rax, (%r10, %r11,1)
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %r13
		mov 0(%r13), %r13
		mov -120(%r13), %r12
# INSTRUCTION_CONST
		mov $8, %r14
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r10, (%r12, %r14,1)
# COMPLEX_RIP_LAMBDA_LOAD
		leaq lambda_1(%rip), %r15
# INSTRUCTION_CONST
		mov $2, %rcx
# COMPLEX_ALLOCATE
		movq $8, %rdx
		imulq %rcx, %rdx
# ALLOC_LAMBDA
		addq $24, %rdx
		pushq %rdx
		pushq %rbp
		call garbageCollectAllocate
		movq $2, 0(%rax)
		subq $8, %rdx
		movq $0, (%rax, %rdx, 1)
		movq $0, 8(%rax, %rdx, 1)
		popq %rdx
		popq %rdx
		addq $8, %rax
# INSTRUCTION_CONST
		mov $0, %rbx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r15, (%rax, %rbx,1)
# INSTRUCTION_COPY
		mov %rax, %r8
# INSTRUCTION_CONST
		mov $8, %r9
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rax, (%r8, %r9,1)
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %r11
		mov 0(%r11), %r11
		mov -120(%r11), %r10
# INSTRUCTION_CONST
		mov $16, %r12
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r8, (%r10, %r12,1)
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_CONST
		mov $1, %r13
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -80(%rbp), %r14
# INSTRUCTION_CONST
		mov $0, %r15
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r13, (%r14, %r15,1)
# INSTRUCTION_CONST
		mov $2, %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -88(%rbp), %rdx
# INSTRUCTION_CONST
		mov $0, %rbx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rcx, (%rdx, %rbx,1)
# INSTRUCTION_CONST
		mov $3, %rsi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -96(%rbp), %rdi
# INSTRUCTION_CONST
		mov $0, %r8
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rsi, (%rdi, %r8,1)
# INSTRUCTION_CONST
		mov $4, %r9
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -104(%rbp), %r10
# INSTRUCTION_CONST
		mov $0, %r11
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r9, (%r10, %r11,1)
# INSTRUCTION_CONST
		mov $5, %r12
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -112(%rbp), %r13
# INSTRUCTION_CONST
		mov $0, %r14
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r12, (%r13, %r14,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -80(%rbp), %r15
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -128(%rbp), %rcx
# INSTRUCTION_CONST
		mov $0, %rdx
# INSTRUCTION_CONST
		mov $8, %rbx
# INSTRUCTION_MUL
		imul %rbx, %rdx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r15, (%rcx, %rdx,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -88(%rbp), %rsi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -128(%rbp), %rdi
# INSTRUCTION_CONST
		mov $1, %r8
# INSTRUCTION_CONST
		mov $8, %r9
# INSTRUCTION_MUL
		imul %r9, %r8
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rsi, (%rdi, %r8,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -96(%rbp), %r10
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -128(%rbp), %r11
# INSTRUCTION_CONST
		mov $2, %r12
# INSTRUCTION_CONST
		mov $8, %r13
# INSTRUCTION_MUL
		imul %r13, %r12
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r10, (%r11, %r12,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -104(%rbp), %r14
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -128(%rbp), %r15
# INSTRUCTION_CONST
		mov $3, %rcx
# INSTRUCTION_CONST
		mov $8, %rdx
# INSTRUCTION_MUL
		imul %rdx, %rcx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r14, (%r15, %rcx,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -112(%rbp), %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -128(%rbp), %rsi
# INSTRUCTION_CONST
		mov $4, %rdi
# INSTRUCTION_CONST
		mov $8, %r8
# INSTRUCTION_MUL
		imul %r8, %rdi
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rbx, (%rsi, %rdi,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -120(%rbp), %r9
# INSTRUCTION_CONST
		mov $8, %r10
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r9, %r10,1), %r9
# COMPLEX_SAVE_STATIC_LINK
		leaq staticLink, %r11
		push 0(%r11)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -128(%rbp), %r12
# INSTRUCTION_PUSH
		push %r12
# INSTRUCTION_CONST
		mov $8, %r13
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r9, %r13,1), %r9
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %r9
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -120(%rbp), %r14
# INSTRUCTION_CONST
		mov $8, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r14, %r15,1), %r14
# INSTRUCTION_CONST
		mov $0, %rcx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r14, %rcx,1), %r14
# INSTRUCTION_REGISTER_CALL
		call *%r14
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rdx
		pop 0(%rdx)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -120(%rbp), %rbx
# INSTRUCTION_CONST
		mov $16, %rsi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rsi,1), %rbx
# COMPLEX_SAVE_STATIC_LINK
		leaq staticLink, %rdi
		push 0(%rdi)
# COMPLEX_RIP_LAMBDA_LOAD
		leaq lambda_2(%rip), %r8
# INSTRUCTION_CONST
		mov $2, %r9
# COMPLEX_ALLOCATE
		movq $8, %r10
		imulq %r9, %r10
# ALLOC_LAMBDA
		addq $24, %r10
		pushq %r10
		pushq %rbp
		call garbageCollectAllocate
		movq $2, 0(%rax)
		subq $8, %r10
		movq $0, (%rax, %r10, 1)
		movq $0, 8(%rax, %r10, 1)
		popq %r10
		popq %r10
		addq $8, %rax
# INSTRUCTION_CONST
		mov $0, %r11
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r8, (%rax, %r11,1)
# INSTRUCTION_COPY
		mov %rax, %r14
# INSTRUCTION_PUSH
		push %r14
# INSTRUCTION_CONST
		mov $8, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %r15,1), %rbx
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -120(%rbp), %rcx
# INSTRUCTION_CONST
		mov $16, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rcx
# INSTRUCTION_CONST
		mov $0, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rbx,1), %rcx
# INSTRUCTION_REGISTER_CALL
		call *%rcx
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rsi
		pop 0(%rsi)
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
	subq $48, %rsp
	popq %rax
	movq %rax, -8(%rbp)
	movq $0, -16(%rbp)
	leaq staticLink, %r8
	movq %rbp, 16(%r8)
# METADATA_FUNCTION_ARGUMENT
	mov 16(%rbp), %r9
	mov %r9, -32(%rbp)
# METADATA_FUNCTION_ARGUMENT
	mov 24(%rbp), %r9
	mov %r9, -24(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -24(%rbp), %r10
# METADATA_DEBUG_INFO
	# CLASS LOAD
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -32(%rbp), %r11
# INSTRUCTION_CONST
	mov $0, %r12
# INSTRUCTION_MOVE_TO_OFFSET
	mov %r10, (%r11, %r12,1)
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
# INSTRUCTION_FUNCTION_LABEL
.type lambda_1, @function
lambda_1:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
push %rax
subq $56, %rsp
popq %rax
movq %rax, -8(%rbp)
movq $0, -16(%rbp)
leaq staticLink, %rsi
movq %rbp, 16(%rsi)
# METADATA_FUNCTION_ARGUMENT
mov 16(%rbp), %rdi
mov %rdi, -32(%rbp)
# METADATA_FUNCTION_ARGUMENT
mov 24(%rbp), %rdi
mov %rdi, -24(%rbp)
# VAR iterator
# INSTRUCTION_CONST
mov $0, %r8
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %r8, -40(%rbp)
# INSTRUCTION_LABEL
while_cnd_0:
# INSTRUCTION_CONST
mov $1, %r9
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -40(%rbp), %r10
# METADATA_DEBUG_INFO
# CLASS LOAD
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -32(%rbp), %r11
# INSTRUCTION_CONST
mov $0, %r12
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
mov (%r11, %r12,1), %r11
# INSTRUCTION_CONST
mov $-8, %r13
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
mov (%r11, %r13,1), %r11
# INSTRUCTION_MINUS
sub %r11, %r10
# COMPLEX_CONSTRAIN_BOOLEAN
push %rdx
cmp $0, %r10
seta %dl
movsx %dl, %r10
pop %rdx
# INSTRUCTION_CMP
cmp %r10, %r9
# INSTRUCTION_JE
je while_0_begin
# INSTRUCTION_JMP
jmp while_0_end
# INSTRUCTION_LABEL
while_0_begin:
# METADATA_DEBUG_INFO
# CLASS LOAD
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -32(%rbp), %r14
# INSTRUCTION_CONST
mov $0, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
mov (%r14, %r15,1), %r14
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
leaq staticLink, %rdx
mov 16(%rdx), %rdx
mov -40(%rdx), %rcx
# INSTRUCTION_CONST
mov $8, %rbx
# INSTRUCTION_MUL
imul %rbx, %rcx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
mov (%r14, %rcx,1), %r14
# INSTRUCTION_PUSH
push %r14
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
leaq staticLink, %rdi
mov 16(%rdi), %rdi
mov -24(%rdi), %rsi
# INSTRUCTION_CONST
mov $8, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
mov (%rsi, %r8,1), %rsi
# METADATA_DEBUG_INFO
# CAPTURE PUSH
# INSTRUCTION_PUSH
push %rsi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
leaq staticLink, %r10
mov 16(%r10), %r10
mov -24(%r10), %r9
# INSTRUCTION_CONST
mov $0, %r11
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
mov (%r9, %r11,1), %r9
# INSTRUCTION_REGISTER_CALL
call *%r9
# METADATA_DEBUG_INFO
# CLASS LOAD
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -32(%rbp), %r12
# INSTRUCTION_CONST
mov $0, %r13
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
mov (%r12, %r13,1), %r12
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
leaq staticLink, %r15
mov 16(%r15), %r15
mov -40(%r15), %r14
# INSTRUCTION_CONST
mov $8, %rcx
# INSTRUCTION_MUL
imul %rcx, %r14
# INSTRUCTION_MOVE_TO_OFFSET
mov %rax, (%r12, %r14,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
leaq staticLink, %rbx
mov 16(%rbx), %rbx
mov -40(%rbx), %rdx
# INSTRUCTION_CONST
mov $1, %rsi
# INSTRUCTION_ADD
add %rdx, %rsi
# COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE
leaq staticLink, %rax
mov 16(%rax), %rax
mov %rsi, -40(%rax)
# INSTRUCTION_JMP
jmp while_cnd_0
# INSTRUCTION_LABEL
while_0_end:
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
# INSTRUCTION_FUNCTION_LABEL
.type lambda_2, @function
lambda_2:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
push %rax
subq $48, %rsp
popq %rax
movq %rax, -8(%rbp)
movq $0, -16(%rbp)
leaq staticLink, %r12
movq %rbp, 8(%r12)
# METADATA_FUNCTION_ARGUMENT
mov 16(%rbp), %r13
mov %r13, -32(%rbp)
# METADATA_FUNCTION_ARGUMENT
mov 24(%rbp), %r13
mov %r13, -24(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -24(%rbp), %r14
# INSTRUCTION_CONST
mov $0, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
mov (%r14, %r15,1), %r14
# INSTRUCTION_WRITE
movq %r14, %rsi
movq $intprint, %rdi
movq $0, %rax
call printf
# INSTRUCTION_CONST
mov $2, %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -24(%rbp), %rdx
# INSTRUCTION_CONST
mov $0, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
mov (%rdx, %rbx,1), %rdx
# INSTRUCTION_MUL
imul %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -24(%rbp), %rsi
# INSTRUCTION_CONST
mov $0, %rdi
# INSTRUCTION_MOVE_TO_OFFSET
mov %rdx, (%rsi, %rdi,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -24(%rbp), %r8
# INSTRUCTION_RETURN
mov %r8, %rax
mov %rbp,%rsp
pop %rbp
ret
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
