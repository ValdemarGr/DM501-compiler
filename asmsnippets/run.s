.section .data
staticLink:
	.space 16
intprint:
	.asciz "%i\n"
gcHeapOne:
    .space 32
gcHeapTwo:
    .space 32
.section .text
.global main
.extern printf
# METADATA_BEGIN_BODY_BLOCK
# VAR clsB
# VAR clsA
# METADATA_CREATE_MAIN

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
    movq $0, 8(%rsi)
    leaq gcHeapOne, %rax
    movq $0, 8(%rax)

    cmp $0, 0(%rsi)
    je heapTwoIsNew
    jmp heapOneIsNew

    heapOneIsNew:
        leaq gcHeapOne, %rsi
        leaq gcHeapTwo, %rax
    jmp loadHeapEnd
    heapTwoIsNew:
        #already loaded

    loadHeapEnd:

    movq $0, 0(%rax)
    movq $1, 0(%rsi)

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

        #check for already moved
        cmp $-1, %rax
        jne continueTheBFSMove
            # we actually just have to move pointer +8 then we have the ptr
            addq $8, %rcx
            movq (%rcx), %rcx

            mov (%r15, %r13, 1), %rdi #rdi has offset
            #mul by factor
            imulq $8, %rdi
            #negate because of stack access
            movq $0, %r12
            subq %rdi, %r12
            movq %r12, %rdi
            #add the constant offset
            add %r8, %rdi

            #move the value back to stack
            movq %rcx, (%r15, %rdi, 1)

        jmp gcLoopEpilogue
        continueTheBFSMove:

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
# VAR y
# VAR mybool
# VAR modulo
# METADATA_CREATE_MAIN
	main:
	push %rbp
mov %rbp, %rax
	movq %rsp, %rbp
		# 20 mb

    mov $0, %rdi
    mov $12, %rax
    syscall
    #move heap into one
    leaq gcHeapOne, %r15
    movq %rax, 24(%r15)
    movq $1048576000, 16(%r15)
    movq $0, 8(%r15)
    movq $1, 0(%r15)
    addq $1048576000, %rax
    push %rax
    movq %rax, %rdi
    mov $12, %rax
    syscall
    pop %rax
    #top should be in rax

    #move heap into one

    leaq gcHeapTwo, %r15
    movq %rax, 24(%r15)
    movq $1048576000, 16(%r15)
    movq $0, 8(%r15)
    movq $0, 0(%r15)

    addq $1048576000, %rax
    movq %rax, %rdi
    mov $12, %rax
    syscall
	subq $240, %rsp
	popq %rax
	movq %rax, -8(%rbp)
	movq $4, -16(%rbp)
	movq $1, -24(%rbp)
	movq $0, -64(%rbp)
	movq $2, -32(%rbp)
	movq $0, -72(%rbp)
	movq $3, -40(%rbp)
	movq $0, -80(%rbp)
	movq $4, -48(%rbp)
	movq $0, -88(%rbp)
	leaq staticLink, %rax
	movq %rbp, (%rax)
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $30, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -104(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -104(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_ALLOCATE
		movq $8, %rdx
		imulq %rcx, %rdx
# ALLOC_ARR_OF_PRIM
		addq $16, %rdx
		pushq %rdx
		pushq %rbp
		call garbageCollectAllocate
		movq %rcx, 0(%rax)
		subq $8, %rdx
		movq $0, (%rax, %rdx, 1)
		popq %rdx
		popq %rdx
		addq $8, %rax
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -64(%rbp)
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $30, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -144(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -144(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_ALLOCATE
		movq $8, %rdx
		imulq %rcx, %rdx
# ALLOC_ARR_OF_PRIM
		addq $16, %rdx
		pushq %rdx
		pushq %rbp
		call garbageCollectAllocate
		movq %rcx, 0(%rax)
		subq $8, %rdx
		movq $0, (%rax, %rdx, 1)
		popq %rdx
		popq %rdx
		addq $8, %rax
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -72(%rbp)
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $30, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -96(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -96(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_ALLOCATE
		movq $8, %rdx
		imulq %rcx, %rdx
# ALLOC_ARR_OF_PRIM
		addq $16, %rdx
		pushq %rdx
		pushq %rbp
		call garbageCollectAllocate
		movq %rcx, 0(%rax)
		subq $8, %rdx
		movq $0, (%rax, %rdx, 1)
		popq %rdx
		popq %rdx
		addq $8, %rax
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -80(%rbp)
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $30, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -160(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -160(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_ALLOCATE
		movq $8, %rdx
		imulq %rcx, %rdx
# ALLOC_ARR_OF_PRIM
		addq $16, %rdx
		pushq %rdx
		pushq %rbp
		call garbageCollectAllocate
		movq %rcx, 0(%rax)
		subq $8, %rdx
		movq $0, (%rax, %rdx, 1)
		popq %rdx
		popq %rdx
		addq $8, %rax
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -88(%rbp)
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $30, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -168(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -168(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -120(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $600, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -176(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -176(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -112(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $-1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -112(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -112(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -128(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_SET_ZERO
		xorq %rcx, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -88(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -88(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -136(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_SET_ZERO
		xorq %rcx, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -184(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -184(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -144(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -192(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -192(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -160(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $40, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -200(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -200(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -168(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $3641, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -112(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -112(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -176(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $729, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -208(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -208(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -184(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_SET_ZERO
		xorq %rcx, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -216(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -216(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -104(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_LABEL
		while_cnd_4:
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -104(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -192(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -120(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -128(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -128(%rbp), %rdx
# INSTRUCTION_MINUS
		sub %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -128(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -192(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -128(%rbp), %rcx
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rdx
		cmp $0, %rcx
		setg %dl
		movsx %dl, %rcx
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -224(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -224(%rbp), %rdx
# INSTRUCTION_CMP
		cmp %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -224(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -128(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_JE
		je while_4_begin
# INSTRUCTION_JMP
		jmp while_4_end
# INSTRUCTION_LABEL
		while_4_begin:
# COMPLEX_SAVE_ALL
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -160(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -136(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH_STACK
		push 88(%rbp)
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -168(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -232(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH_STACK
		push 184(%rbp)
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -176(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -240(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH_STACK
		push 192(%rbp)
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -184(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -248(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH_STACK
		push 200(%rbp)
# INSTRUCTION_FUNCTION_CALL
		call NextRand__0
# INSTRUCTION_ADD_STACK_PTR
		addq $32, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -200(%rbp)
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rcx
movq %rbp, 0(%rcx)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -200(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -160(%rbp)
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -160(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -144(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $20, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -256(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -256(%rbp), %rdx
# INSTRUCTION_DIV
		movq %rcx, %rax
		pushq %r10
		movq %rdx, %r10
		cqto
		idiv %r10
		popq %r10
		movq %rax, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -256(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -144(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -256(%rbp), %rcx
# INSTRUCTION_ADD_CONST
		add $10, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -256(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -64(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -264(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -104(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -112(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -112(%rbp), %rcx
# INSTRUCTION_MUL_CONST
		imul $8, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -112(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -256(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -112(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -264(%rbp), %rbx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rcx, (%rbx, %rdx,1)
# INSTRUCTION_POP
		pop %rbx
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# COMPLEX_SAVE_ALL
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -160(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -288(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH_STACK
		push 240(%rbp)
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -168(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -120(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH_STACK
		push 72(%rbp)
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -176(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -120(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH_STACK
		push 72(%rbp)
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -184(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -296(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH_STACK
		push 248(%rbp)
# INSTRUCTION_FUNCTION_CALL
		call NextRand__0
# INSTRUCTION_ADD_STACK_PTR
		addq $32, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rbx
movq %rbp, 0(%rbx)
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -160(%rbp)
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -160(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -304(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $90, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -312(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -312(%rbp), %rdx
# INSTRUCTION_DIV
		movq %rcx, %rax
		pushq %r10
		movq %rdx, %r10
		cqto
		idiv %r10
		popq %r10
		movq %rax, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -312(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -304(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -312(%rbp), %rcx
# INSTRUCTION_ADD_CONST
		add $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -312(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -72(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -128(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -104(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -320(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -320(%rbp), %rcx
# INSTRUCTION_MUL_CONST
		imul $8, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -320(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -312(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -320(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -128(%rbp), %rbx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rcx, (%rbx, %rdx,1)
# INSTRUCTION_POP
		pop %rbx
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_SET_ZERO
		xorq %rcx, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -136(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -80(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -208(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -104(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -336(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -336(%rbp), %rcx
# INSTRUCTION_MUL_CONST
		imul $8, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -336(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -136(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -336(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -208(%rbp), %rbx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rcx, (%rbx, %rdx,1)
# INSTRUCTION_POP
		pop %rbx
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_SET_ZERO
		xorq %rcx, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -352(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -88(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -360(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -104(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -368(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -368(%rbp), %rcx
# INSTRUCTION_MUL_CONST
		imul $8, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -368(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -352(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -368(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -360(%rbp), %rbx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rcx, (%rbx, %rdx,1)
# INSTRUCTION_POP
		pop %rbx
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -104(%rbp), %rbx
# INSTRUCTION_ADD_CONST
		add $1, %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rbx, -104(%rbp)
# INSTRUCTION_JMP
		jmp while_cnd_4
# INSTRUCTION_LABEL
		while_4_end:
# COMPLEX_SAVE_ALL
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_SET_ZERO
		xorq %rcx, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -384(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH_STACK
		push 336(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -120(%rbp), %rbx
# INSTRUCTION_CONST
		mov $1, %rdx
# INSTRUCTION_MINUS
		sub %rdx, %rbx
# INSTRUCTION_PUSH
		push %rbx
# INSTRUCTION_FUNCTION_CALL
		call quicksort__0
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rbx
movq %rbp, 0(%rbx)
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -152(%rbp)
# COMPLEX_SAVE_ALL
# INSTRUCTION_SET_ZERO
		xorq %rdx, %rdx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_SET_ZERO
		xorq %rcx, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -392(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH_STACK
		push 344(%rbp)
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_SET_ZERO
		xorq %rcx, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -400(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH_STACK
		push 352(%rbp)
# INSTRUCTION_FUNCTION_CALL
		call BKnap__0
# INSTRUCTION_ADD_STACK_PTR
		addq $24, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rbx
movq %rbp, 0(%rbx)
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -96(%rbp)
# INSTRUCTION_SET_ZERO
		xorq %rdx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -104(%rbp)
# INSTRUCTION_LABEL
		while_cnd_5:
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -104(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -408(%rbp)
# INSTRUCTION_POP
		pop %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -120(%rbp), %rbx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_MINUS
		sub %rcx, %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -408(%rbp)
# INSTRUCTION_POP
		pop %rcx
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rdx
		cmp $0, %rbx
		setg %dl
		movsx %dl, %rbx
		pop %rdx
# INSTRUCTION_CONST
		mov $1, %rdx
# INSTRUCTION_CMP
		cmp %rbx, %rdx
# INSTRUCTION_JE
		je while_5_begin
# INSTRUCTION_JMP
		jmp while_5_end
# INSTRUCTION_LABEL
		while_5_begin:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -80(%rbp), %rsi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -104(%rbp), %rbx
# INSTRUCTION_MUL_CONST
		imul $8, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rsi, %rbx,1), %rdx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -416(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -416(%rbp), %rcx
# INSTRUCTION_MINUS
		sub %rdx, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -416(%rbp)
# INSTRUCTION_POP
		pop %rcx
# COMPLEX_ABS_VALUE
		movq %rsi, %rdi
		sar $63, %rdi
		addq %rdi, %rsi
		xor %rsi, %rdi
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -216(%rbp), %rcx
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rdx
		cmp $0, %rcx
		setg %dl
		movsx %dl, %rcx
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_CONST
		mov $1, %rbx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_MINUS
		sub %rcx, %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -216(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_CONST
		mov $1, %rdx
# INSTRUCTION_CMP
		cmp %rbx, %rdx
# INSTRUCTION_JE
		je if_9_begin
# INSTRUCTION_JMP
		jmp if_9_end
# INSTRUCTION_LABEL
		if_9_begin:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -144(%rbp), %rsi
# INSTRUCTION_ADD_CONST
		add $1, %rsi
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rsi, -144(%rbp)
# INSTRUCTION_LABEL
		if_9_end:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -104(%rbp), %rsi
# INSTRUCTION_ADD_CONST
		add $1, %rsi
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rsi, -104(%rbp)
# INSTRUCTION_JMP
		jmp while_cnd_5
# INSTRUCTION_LABEL
		while_5_end:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -144(%rbp), %rbx
# INSTRUCTION_WRITE
		movq %rbx, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -128(%rbp), %rdx
# INSTRUCTION_WRITE
		movq %rdx, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -136(%rbp), %rdi
# INSTRUCTION_WRITE
		movq %rdi, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
xorq %rax,%rax
ret
mov $60, %rax
mov $0, %rdi
syscall
# INSTRUCTION_FUNCTION_LABEL
.type BKnap__0, @function
BKnap__0:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
	subq $416, %rsp
	movq %rax, -8(%rbp)
	movq $0, -16(%rbp)
	leaq staticLink, %rdx
	movq %rbp, 8(%rdx)
# METADATA_FUNCTION_ARGUMENT
		mov 16(%rbp), %rdx
		mov %rdx, -40(%rbp)
# METADATA_FUNCTION_ARGUMENT
		mov 24(%rbp), %rdx
		mov %rdx, -32(%rbp)
# METADATA_FUNCTION_ARGUMENT
		mov 32(%rbp), %rdx
		mov %rdx, -24(%rbp)
# VAR counter
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -64(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rbx
		mov 0(%rbx), %rbx
		movq -16(%rbx), %rdx
		addq $1, %rdx
		imul $-1, %rdx
		mov -24(%rbx, %rdx, 8), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -72(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -80(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -80(%rbp), %rcx
# INSTRUCTION_MUL_CONST
		imul $8, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -80(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -72(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -80(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rbx, -88(%rbp)
# INSTRUCTION_POP
		pop %rbx
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -64(%rbp), %rdx
# INSTRUCTION_ADD
		add %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -64(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -88(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rcx
		mov 0(%rcx), %rcx
		movq -16(%rcx), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov -72(%rcx, %rbx, 8), %rdx
# INSTRUCTION_POP
		pop %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -96(%rbp)
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -96(%rbp), %rdx
# INSTRUCTION_MINUS
		sub %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -96(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -64(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -96(%rbp), %rcx
# INSTRUCTION_ADD_CONST
		add $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -96(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -96(%rbp), %rcx
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rdx
		cmp $0, %rcx
		setg %dl
		movsx %dl, %rcx
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -104(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -104(%rbp), %rdx
# INSTRUCTION_CMP
		cmp %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -104(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -96(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_JE
		je if_0_begin
# INSTRUCTION_JMP
		jmp if_0_end
# INSTRUCTION_LABEL
		if_0_begin:
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -112(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rcx
		mov 0(%rcx), %rcx
		movq -16(%rcx), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov -48(%rcx, %rbx, 8), %rdx
# INSTRUCTION_POP
		pop %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -120(%rbp)
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -128(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -128(%rbp), %rcx
# INSTRUCTION_MUL_CONST
		imul $8, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -128(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -112(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -128(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -120(%rbp), %rbx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rcx, (%rbx, %rdx,1)
# INSTRUCTION_POP
		pop %rbx
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -136(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rcx
		mov 0(%rcx), %rcx
		movq -16(%rcx), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov -80(%rcx, %rbx, 8), %rdx
# INSTRUCTION_POP
		pop %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -144(%rbp)
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -152(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -144(%rbp), %rdx
# INSTRUCTION_MINUS
		sub %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -144(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -152(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -144(%rbp), %rdx
# INSTRUCTION_MINUS
		sub %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -144(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -136(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -144(%rbp), %rcx
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rdx
		cmp $0, %rcx
		setg %dl
		movsx %dl, %rcx
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -160(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -160(%rbp), %rdx
# INSTRUCTION_CMP
		cmp %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -160(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -144(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_JE
		je if_1_begin
# INSTRUCTION_JMP
		jmp if_1_end
# INSTRUCTION_LABEL
		if_1_begin:
# COMPLEX_SAVE_ALL
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -168(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -168(%rbp), %rcx
# INSTRUCTION_ADD_CONST
		add $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -168(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH_STACK
		push 152(%rbp)
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -176(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rcx
		mov 0(%rcx), %rcx
		movq -16(%rcx), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov -32(%rcx, %rbx, 8), %rdx
# INSTRUCTION_POP
		pop %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -184(%rbp)
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -192(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -192(%rbp), %rcx
# INSTRUCTION_MUL_CONST
		imul $8, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -192(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -184(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -192(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rbx, -200(%rbp)
# INSTRUCTION_POP
		pop %rbx
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -176(%rbp), %rdx
# INSTRUCTION_ADD
		add %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -176(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -200(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH_STACK
		push 160(%rbp)
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -208(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rcx
		mov 0(%rcx), %rcx
		movq -16(%rcx), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov -24(%rcx, %rbx, 8), %rdx
# INSTRUCTION_POP
		pop %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -216(%rbp)
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -224(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -224(%rbp), %rcx
# INSTRUCTION_MUL_CONST
		imul $8, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -224(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -216(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -224(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rbx, -232(%rbp)
# INSTRUCTION_POP
		pop %rbx
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -208(%rbp), %rdx
# INSTRUCTION_ADD
		add %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -208(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -232(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH_STACK
		push 192(%rbp)
# INSTRUCTION_FUNCTION_CALL
		call BKnap__0
# INSTRUCTION_ADD_STACK_PTR
		addq $24, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -240(%rbp)
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rcx
movq %rbp, 8(%rcx)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -240(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -48(%rbp)
# INSTRUCTION_LABEL
		if_1_end:
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -248(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rcx
		mov 0(%rcx), %rcx
		movq -16(%rcx), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov -32(%rcx, %rbx, 8), %rdx
# INSTRUCTION_POP
		pop %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -256(%rbp)
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -264(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -264(%rbp), %rcx
# INSTRUCTION_MUL_CONST
		imul $8, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -264(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -256(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -264(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rbx, -272(%rbp)
# INSTRUCTION_POP
		pop %rbx
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -248(%rbp), %rdx
# INSTRUCTION_ADD
		add %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -248(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -272(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rdx
		mov 0(%rdx), %rdx
		movq -16(%rdx), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov -88(%rdx, %rbx, 8), %rcx
# INSTRUCTION_POP
		pop %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -280(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -248(%rbp), %rdx
# INSTRUCTION_MINUS
		sub %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -248(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -280(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -248(%rbp), %rcx
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rdx
		cmp $0, %rcx
		setg %dl
		movsx %dl, %rcx
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -288(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rcx
		mov 0(%rcx), %rcx
		movq -16(%rcx), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov -80(%rcx, %rbx, 8), %rdx
# INSTRUCTION_POP
		pop %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -296(%rbp)
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -304(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -296(%rbp), %rdx
# INSTRUCTION_MINUS
		sub %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -296(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -304(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -296(%rbp), %rdx
# INSTRUCTION_MINUS
		sub %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -296(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -288(%rbp)
# INSTRUCTION_POP
		pop %rcx
# COMPLEX_ABS_VALUE
		movq %rbx, %r9
		sar $63, %r9
		addq %r9, %rbx
		xor %rbx, %r9
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -312(%rbp), %rcx
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rdx
		cmp $0, %rcx
		setg %dl
		movsx %dl, %rcx
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -320(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -320(%rbp), %rdx
# INSTRUCTION_MINUS
		sub %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -320(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -312(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -320(%rbp), %rdx
# INSTRUCTION_AND
		and %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -320(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -248(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -328(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -328(%rbp), %rdx
# INSTRUCTION_CMP
		cmp %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -328(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -320(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_JE
		je if_2_begin
# INSTRUCTION_JMP
		jmp if_2_end
# INSTRUCTION_LABEL
		if_2_begin:
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -336(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rbx
		mov 0(%rbx), %rbx
		movq -16(%rbx), %rdx
		addq $1, %rdx
		imul $-1, %rdx
		mov -32(%rbx, %rdx, 8), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -344(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -352(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -352(%rbp), %rcx
# INSTRUCTION_MUL_CONST
		imul $8, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -352(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -344(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -352(%rbp), %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rbx,1), %rdx
# INSTRUCTION_POP
		pop %rbx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -336(%rbp), %rcx
# INSTRUCTION_ADD
		add %rdx, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -336(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -336(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE
		leaq staticLink, %rbx
		mov 0(%rbx), %rbx
		movq -16(%rbx), %rdx
		addq $1, %rdx
		imul $-1, %rdx
		mov %rcx, -88(%rbx, %rdx, 8)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -360(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rbx
		mov 0(%rbx), %rbx
		movq -16(%rbx), %rdx
		addq $1, %rdx
		imul $-1, %rdx
		mov -24(%rbx, %rdx, 8), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -368(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -376(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -376(%rbp), %rcx
# INSTRUCTION_MUL_CONST
		imul $8, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -376(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -368(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -376(%rbp), %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rbx,1), %rdx
# INSTRUCTION_POP
		pop %rbx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -360(%rbp), %rcx
# INSTRUCTION_ADD
		add %rdx, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -360(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -360(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE
		leaq staticLink, %rsi
		mov 0(%rsi), %rsi
		movq -16(%rsi), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov %rcx, -96(%rsi, %rbx, 8)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_SET_ZERO
		xorq %rdx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -56(%rbp)
# INSTRUCTION_LABEL
		while_cnd_0:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -384(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -384(%rbp), %rcx
# INSTRUCTION_MINUS
		sub %rdx, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -384(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -384(%rbp), %rcx
# INSTRUCTION_ADD_CONST
		add $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -384(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -384(%rbp), %rcx
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rdx
		cmp $0, %rcx
		setg %dl
		movsx %dl, %rcx
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_CONST
		mov $1, %rbx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CMP
		cmp %rcx, %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -384(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_JE
		je while_0_begin
# INSTRUCTION_JMP
		jmp while_0_end
# INSTRUCTION_LABEL
		while_0_begin:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rsi
		mov 0(%rsi), %rsi
		movq -16(%rsi), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov -48(%rsi, %rbx, 8), %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rsi
# INSTRUCTION_MUL_CONST
		imul $8, %rsi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdx, %rsi,1), %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rdi
		mov 0(%rdi), %rdi
		movq -16(%rdi), %rsi
		addq $1, %rsi
		imul $-1, %rsi
		mov -40(%rdi, %rsi, 8), %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rsi
# INSTRUCTION_MUL_CONST
		imul $8, %rsi
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rbx, (%rdx, %rsi,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rdx
# INSTRUCTION_ADD_CONST
		add $1, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -56(%rbp)
# INSTRUCTION_JMP
		jmp while_cnd_0
# INSTRUCTION_LABEL
		while_0_end:
# INSTRUCTION_LABEL
		if_2_end:
# INSTRUCTION_LABEL
		if_0_end:
# COMPLEX_SAVE_ALL
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_FUNCTION_CALL
		call Bound__0
# INSTRUCTION_ADD_STACK_PTR
		addq $24, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rdx
movq %rbp, 8(%rdx)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rbx
		mov 0(%rbx), %rbx
		movq -16(%rbx), %rdx
		addq $1, %rdx
		imul $-1, %rdx
		mov -88(%rbx, %rdx, 8), %rsi
# INSTRUCTION_MINUS
		sub %rsi, %rax
# INSTRUCTION_ADD_CONST
		add $1, %rax
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rdx
		cmp $0, %rax
		setg %dl
		movsx %dl, %rax
		pop %rdx
# INSTRUCTION_CONST
		mov $1, %rdx
# INSTRUCTION_CMP
		cmp %rax, %rdx
# INSTRUCTION_JE
		je if_3_begin
# INSTRUCTION_JMP
		jmp if_3_end
# INSTRUCTION_LABEL
		if_3_begin:
# INSTRUCTION_SET_ZERO
		xorq %rbx, %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rdi
		mov 0(%rdi), %rdi
		movq -16(%rdi), %rsi
		addq $1, %rsi
		imul $-1, %rsi
		mov -48(%rdi, %rsi, 8), %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rsi
# INSTRUCTION_MUL_CONST
		imul $8, %rsi
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rbx, (%rdx, %rsi,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rdi
		mov 0(%rdi), %rdi
		movq -16(%rdi), %rsi
		addq $1, %rsi
		imul $-1, %rsi
		mov -80(%rdi, %rsi, 8), %rdx
# INSTRUCTION_CONST
		mov $1, %rsi
# INSTRUCTION_MINUS
		sub %rsi, %rdx
# INSTRUCTION_MINUS
		sub %rbx, %rdx
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rdx
		cmp $0, %rdx
		setg %dl
		movsx %dl, %rdx
		pop %rdx
# INSTRUCTION_CONST
		mov $1, %rbx
# INSTRUCTION_CMP
		cmp %rdx, %rbx
# INSTRUCTION_JE
		je if_4_begin
# INSTRUCTION_JMP
		jmp if_4_end
# INSTRUCTION_LABEL
		if_4_begin:
# COMPLEX_SAVE_ALL
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rdx
# INSTRUCTION_ADD_CONST
		add $1, %rdx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_FUNCTION_CALL
		call BKnap__0
# INSTRUCTION_ADD_STACK_PTR
		addq $24, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rdx
movq %rbp, 8(%rdx)
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -48(%rbp)
# INSTRUCTION_LABEL
		if_4_end:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rsi
		mov 0(%rsi), %rsi
		movq -16(%rsi), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov -88(%rsi, %rbx, 8), %rdi
# INSTRUCTION_MINUS
		sub %rdi, %rdx
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rdx
		cmp $0, %rdx
		setg %dl
		movsx %dl, %rdx
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rsi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %r8
		mov 0(%r8), %r8
		movq -16(%r8), %rdi
		addq $1, %rdi
		imul $-1, %rdi
		mov -80(%r8, %rdi, 8), %rbx
# INSTRUCTION_CONST
		mov $1, %rdi
# INSTRUCTION_MINUS
		sub %rdi, %rbx
# INSTRUCTION_MINUS
		sub %rsi, %rbx
# COMPLEX_ABS_VALUE
		movq %r14, %rbx
		sar $63, %rbx
		addq %rbx, %r14
		xor %r14, %rbx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -392(%rbp), %rcx
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rdx
		cmp $0, %rcx
		setg %dl
		movsx %dl, %rcx
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_CONST
		mov $1, %rbx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_MINUS
		sub %rcx, %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -392(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_AND
		and %rdx, %rbx
# INSTRUCTION_CONST
		mov $1, %rdx
# INSTRUCTION_CMP
		cmp %rbx, %rdx
# INSTRUCTION_JE
		je if_5_begin
# INSTRUCTION_JMP
		jmp if_5_end
# INSTRUCTION_LABEL
		if_5_begin:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rsi
# COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE
		leaq staticLink, %rbx
		mov 0(%rbx), %rbx
		movq -16(%rbx), %rdx
		addq $1, %rdx
		imul $-1, %rdx
		mov %rsi, -88(%rbx, %rdx, 8)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rsi
# COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE
		leaq staticLink, %rbx
		mov 0(%rbx), %rbx
		movq -16(%rbx), %rdx
		addq $1, %rdx
		imul $-1, %rdx
		mov %rsi, -96(%rbx, %rdx, 8)
# INSTRUCTION_SET_ZERO
		xorq %rdx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -56(%rbp)
# INSTRUCTION_LABEL
		while_cnd_1:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rbx
# INSTRUCTION_MINUS
		sub %rdx, %rbx
# INSTRUCTION_ADD_CONST
		add $1, %rbx
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rdx
		cmp $0, %rbx
		setg %dl
		movsx %dl, %rbx
		pop %rdx
# INSTRUCTION_CONST
		mov $1, %rdx
# INSTRUCTION_CMP
		cmp %rbx, %rdx
# INSTRUCTION_JE
		je while_1_begin
# INSTRUCTION_JMP
		jmp while_1_end
# INSTRUCTION_LABEL
		while_1_begin:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rbx
		mov 0(%rbx), %rbx
		movq -16(%rbx), %rdx
		addq $1, %rdx
		imul $-1, %rdx
		mov -48(%rbx, %rdx, 8), %rsi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rdx
# INSTRUCTION_MUL_CONST
		imul $8, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rsi, %rdx,1), %rdi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rbx
		mov 0(%rbx), %rbx
		movq -16(%rbx), %rdx
		addq $1, %rdx
		imul $-1, %rdx
		mov -40(%rbx, %rdx, 8), %rsi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rdx
# INSTRUCTION_MUL_CONST
		imul $8, %rdx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rdi, (%rsi, %rdx,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rdx
# INSTRUCTION_ADD_CONST
		add $1, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -56(%rbp)
# INSTRUCTION_JMP
		jmp while_cnd_1
# INSTRUCTION_LABEL
		while_1_end:
# INSTRUCTION_LABEL
		if_5_end:
# INSTRUCTION_LABEL
		if_3_end:
# INSTRUCTION_SET_ZERO
		xorq %rdx, %rdx
# INSTRUCTION_RETURN
		mov %rdx, %rax
		mov %rbp,%rsp
pop %rbp
ret
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
xorq %rax,%rax
ret
# INSTRUCTION_PUSH
	push %rcx
# INSTRUCTION_FUNCTION_LABEL
.type Bound__0, @function
Bound__0:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
	subq $136, %rsp
	movq %rax, -8(%rbp)
	movq $0, -16(%rbp)
	leaq staticLink, %rcx
	movq %rbp, 8(%rcx)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# METADATA_FUNCTION_ARGUMENT
		mov 16(%rbp), %rcx
		mov %rcx, -40(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# METADATA_FUNCTION_ARGUMENT
		mov 24(%rbp), %rcx
		mov %rcx, -32(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# METADATA_FUNCTION_ARGUMENT
		mov 32(%rbp), %rcx
		mov %rcx, -24(%rbp)
# INSTRUCTION_POP
		pop %rcx
# VAR counter
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rbx, -48(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -56(%rbp)
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -80(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -80(%rbp), %rcx
# INSTRUCTION_ADD_CONST
		add $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -80(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -80(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -64(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_LABEL
		while_cnd_2:
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -64(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -88(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rcx
		mov 0(%rcx), %rcx
		movq -16(%rcx), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov -80(%rcx, %rbx, 8), %rdx
# INSTRUCTION_POP
		pop %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -96(%rbp)
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -96(%rbp), %rdx
# INSTRUCTION_MINUS
		sub %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -96(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -88(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -96(%rbp), %rcx
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rdx
		cmp $0, %rcx
		setg %dl
		movsx %dl, %rcx
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -72(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -72(%rbp), %rdx
# INSTRUCTION_CMP
		cmp %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -72(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -96(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_JE
		je while_2_begin
# INSTRUCTION_JMP
		jmp while_2_end
# INSTRUCTION_LABEL
		while_2_begin:
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -104(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rcx
		mov 0(%rcx), %rcx
		movq -16(%rcx), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov -24(%rcx, %rbx, 8), %rdx
# INSTRUCTION_POP
		pop %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -112(%rbp)
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -64(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -80(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -80(%rbp), %rcx
# INSTRUCTION_MUL_CONST
		imul $8, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -80(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -112(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -80(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rbx, -88(%rbp)
# INSTRUCTION_POP
		pop %rbx
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -104(%rbp), %rdx
# INSTRUCTION_ADD
		add %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -104(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -88(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -104(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -56(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -136(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rcx
		mov 0(%rcx), %rcx
		movq -16(%rcx), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov -72(%rcx, %rbx, 8), %rdx
# INSTRUCTION_POP
		pop %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -144(%rbp)
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -144(%rbp), %rdx
# INSTRUCTION_MINUS
		sub %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -144(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -136(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -144(%rbp), %rcx
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rdx
		cmp $0, %rcx
		setg %dl
		movsx %dl, %rcx
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -152(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -152(%rbp), %rdx
# INSTRUCTION_CMP
		cmp %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -152(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -144(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_JE
		je if_6_begin
# INSTRUCTION_JMP
		jmp el_6_begin
# INSTRUCTION_LABEL
		if_6_begin:
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -160(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rcx
		mov 0(%rcx), %rcx
		movq -16(%rcx), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov -32(%rcx, %rbx, 8), %rdx
# INSTRUCTION_POP
		pop %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -168(%rbp)
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -64(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -184(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -184(%rbp), %rcx
# INSTRUCTION_MUL_CONST
		imul $8, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -184(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -168(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -184(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rbx, -96(%rbp)
# INSTRUCTION_POP
		pop %rbx
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -160(%rbp), %rdx
# INSTRUCTION_ADD
		add %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -160(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -96(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -160(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -48(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_JMP
		jmp ifel_6_end
# INSTRUCTION_LABEL
		el_6_begin:
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -192(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -104(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -200(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rcx
		mov 0(%rcx), %rcx
		movq -16(%rcx), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov -72(%rcx, %rbx, 8), %rdx
# INSTRUCTION_POP
		pop %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -208(%rbp)
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -200(%rbp), %rdx
# INSTRUCTION_MINUS
		sub %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -200(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -208(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rcx
		mov 0(%rcx), %rcx
		movq -16(%rcx), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov -24(%rcx, %rbx, 8), %rdx
# INSTRUCTION_POP
		pop %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -112(%rbp)
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -64(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -232(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -232(%rbp), %rcx
# INSTRUCTION_MUL_CONST
		imul $8, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -232(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -112(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -232(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rbx, -248(%rbp)
# INSTRUCTION_POP
		pop %rbx
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -248(%rbp), %rdx
# INSTRUCTION_DIV
		movq %rcx, %rax
		pushq %r10
		movq %rdx, %r10
		cqto
		idiv %r10
		popq %r10
		movq %rax, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -248(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -200(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -104(%rbp), %rdx
# INSTRUCTION_MINUS
		sub %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -104(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -248(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rcx
		mov 0(%rcx), %rcx
		movq -16(%rcx), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov -32(%rcx, %rbx, 8), %rdx
# INSTRUCTION_POP
		pop %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -256(%rbp)
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -64(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -264(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -264(%rbp), %rcx
# INSTRUCTION_MUL_CONST
		imul $8, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -264(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -256(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -264(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rbx, -272(%rbp)
# INSTRUCTION_POP
		pop %rbx
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -104(%rbp), %rdx
# INSTRUCTION_MUL
		imul %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -104(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -272(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -192(%rbp), %rdx
# INSTRUCTION_ADD
		add %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -192(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -104(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -192(%rbp), %rcx
# INSTRUCTION_RETURN
		mov %rcx, %rax
		mov %rbp,%rsp
pop %rbp
ret
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_LABEL
		ifel_6_end:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -64(%rbp), %rdx
# INSTRUCTION_ADD_CONST
		add $1, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -64(%rbp)
# INSTRUCTION_JMP
		jmp while_cnd_2
# INSTRUCTION_LABEL
		while_2_end:
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -288(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -288(%rbp), %rcx
# INSTRUCTION_RETURN
		mov %rcx, %rax
		mov %rbp,%rsp
pop %rbp
ret
# INSTRUCTION_POP
		pop %rcx
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
xorq %rax,%rax
ret
# INSTRUCTION_FUNCTION_LABEL
.type NextRand__0, @function
NextRand__0:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
	subq $96, %rsp
	movq %rax, -8(%rbp)
	movq $0, -16(%rbp)
	leaq staticLink, %rbx
	movq %rbp, 8(%rbx)
# METADATA_FUNCTION_ARGUMENT
		mov 16(%rbp), %rdx
		mov %rdx, -48(%rbp)
# METADATA_FUNCTION_ARGUMENT
		mov 24(%rbp), %rdx
		mov %rdx, -40(%rbp)
# METADATA_FUNCTION_ARGUMENT
		mov 32(%rbp), %rdx
		mov %rdx, -32(%rbp)
# METADATA_FUNCTION_ARGUMENT
		mov 40(%rbp), %rdx
		mov %rdx, -24(%rbp)
# VAR temp
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -80(%rbp)
# INSTRUCTION_POP
		pop %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -80(%rbp), %rcx
# INSTRUCTION_MUL
		imul %rdx, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -80(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -88(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -80(%rbp), %rdx
# INSTRUCTION_ADD
		add %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -80(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -88(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -80(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -56(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -96(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -64(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -72(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -72(%rbp), %rdx
# INSTRUCTION_DIV
		movq %rcx, %rax
		pushq %r10
		movq %rdx, %r10
		cqto
		idiv %r10
		popq %r10
		movq %rax, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -72(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -64(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -72(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -72(%rbp), %rdx
# INSTRUCTION_MUL
		imul %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -72(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -72(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -96(%rbp), %rdx
# INSTRUCTION_MINUS
		sub %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -96(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -72(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -96(%rbp), %rcx
# INSTRUCTION_RETURN
		mov %rcx, %rax
		mov %rbp,%rsp
pop %rbp
ret
# INSTRUCTION_POP
		pop %rcx
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
xorq %rax,%rax
ret
# INSTRUCTION_PUSH
	push %rcx
# INSTRUCTION_FUNCTION_LABEL
.type exchange__0, @function
exchange__0:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
	subq $120, %rsp
	movq %rax, -8(%rbp)
	movq $0, -16(%rbp)
	leaq staticLink, %rcx
	movq %rbp, 8(%rcx)
# INSTRUCTION_POP
		pop %rcx
# METADATA_FUNCTION_ARGUMENT
		mov 16(%rbp), %rdx
		mov %rdx, -32(%rbp)
# METADATA_FUNCTION_ARGUMENT
		mov 24(%rbp), %rdx
		mov %rdx, -24(%rbp)
# VAR w_temp
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rcx
		mov 0(%rcx), %rcx
		movq -16(%rcx), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov -32(%rcx, %rbx, 8), %rdx
# INSTRUCTION_POP
		pop %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -88(%rbp)
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -72(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -72(%rbp), %rcx
# INSTRUCTION_MUL_CONST
		imul $8, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -72(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -88(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -72(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rbx, -104(%rbp)
# INSTRUCTION_POP
		pop %rbx
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -104(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -40(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rcx
		mov 0(%rcx), %rcx
		movq -16(%rcx), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov -32(%rcx, %rbx, 8), %rdx
# INSTRUCTION_POP
		pop %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -112(%rbp)
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -80(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -80(%rbp), %rcx
# INSTRUCTION_MUL_CONST
		imul $8, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -80(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -112(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -80(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rbx, -88(%rbp)
# INSTRUCTION_POP
		pop %rbx
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rcx
		mov 0(%rcx), %rcx
		movq -16(%rcx), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov -32(%rcx, %rbx, 8), %rdx
# INSTRUCTION_POP
		pop %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -136(%rbp)
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -56(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rcx
# INSTRUCTION_MUL_CONST
		imul $8, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -56(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -88(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -136(%rbp), %rbx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rcx, (%rbx, %rdx,1)
# INSTRUCTION_POP
		pop %rbx
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -160(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rcx
		mov 0(%rcx), %rcx
		movq -16(%rcx), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov -32(%rcx, %rbx, 8), %rdx
# INSTRUCTION_POP
		pop %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -168(%rbp)
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -184(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -184(%rbp), %rcx
# INSTRUCTION_MUL_CONST
		imul $8, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -184(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -160(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -184(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -168(%rbp), %rbx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rcx, (%rbx, %rdx,1)
# INSTRUCTION_POP
		pop %rbx
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rcx
		mov 0(%rcx), %rcx
		movq -16(%rcx), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov -24(%rcx, %rbx, 8), %rdx
# INSTRUCTION_POP
		pop %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -96(%rbp)
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -200(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -200(%rbp), %rcx
# INSTRUCTION_MUL_CONST
		imul $8, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -200(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -96(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -200(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rbx, -216(%rbp)
# INSTRUCTION_POP
		pop %rbx
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -216(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -48(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rcx
		mov 0(%rcx), %rcx
		movq -16(%rcx), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov -24(%rcx, %rbx, 8), %rdx
# INSTRUCTION_POP
		pop %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -64(%rbp)
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -72(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -72(%rbp), %rcx
# INSTRUCTION_MUL_CONST
		imul $8, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -72(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -64(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -72(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rbx, -80(%rbp)
# INSTRUCTION_POP
		pop %rbx
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rcx
		mov 0(%rcx), %rcx
		movq -16(%rcx), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov -24(%rcx, %rbx, 8), %rdx
# INSTRUCTION_POP
		pop %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -248(%rbp)
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -88(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -88(%rbp), %rcx
# INSTRUCTION_MUL_CONST
		imul $8, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -88(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -80(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -88(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -248(%rbp), %rbx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rcx, (%rbx, %rdx,1)
# INSTRUCTION_POP
		pop %rbx
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rbx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rcx
		mov 0(%rcx), %rcx
		movq -16(%rcx), %rdx
		addq $1, %rdx
		imul $-1, %rdx
		mov -24(%rcx, %rdx, 8), %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rbx, -272(%rbp)
# INSTRUCTION_POP
		pop %rbx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -96(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -96(%rbp), %rcx
# INSTRUCTION_MUL_CONST
		imul $8, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -96(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -96(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -272(%rbp), %rdx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rbx, (%rdx, %rcx,1)
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -296(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -296(%rbp), %rcx
# INSTRUCTION_RETURN
		mov %rcx, %rax
		mov %rbp,%rsp
pop %rbp
ret
# INSTRUCTION_POP
		pop %rcx
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
xorq %rax,%rax
ret
# INSTRUCTION_PUSH
	push %rcx
# INSTRUCTION_FUNCTION_LABEL
.type quicksort__0, @function
quicksort__0:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
	subq $112, %rsp
	movq %rax, -8(%rbp)
	movq $0, -16(%rbp)
	leaq staticLink, %rcx
	movq %rbp, 8(%rcx)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# METADATA_FUNCTION_ARGUMENT
		mov 16(%rbp), %rcx
		mov %rcx, -32(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# METADATA_FUNCTION_ARGUMENT
		mov 24(%rbp), %rcx
		mov %rcx, -24(%rbp)
# INSTRUCTION_POP
		pop %rcx
# VAR dummy
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -64(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -72(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -72(%rbp), %rdx
# INSTRUCTION_MINUS
		sub %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -72(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -64(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -72(%rbp), %rcx
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rdx
		cmp $0, %rcx
		setg %dl
		movsx %dl, %rcx
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -72(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -72(%rbp), %rdx
# INSTRUCTION_CMP
		cmp %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -72(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -72(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_JE
		je if_7_begin
# INSTRUCTION_JMP
		jmp if_7_end
# INSTRUCTION_LABEL
		if_7_begin:
# COMPLEX_SAVE_ALL
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -104(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH_STACK
		push 88(%rbp)
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -112(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH_STACK
		push 96(%rbp)
# INSTRUCTION_FUNCTION_CALL
		call partition__0
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -56(%rbp)
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rcx
movq %rbp, 8(%rcx)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -40(%rbp)
# COMPLEX_SAVE_ALL
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -64(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH_STACK
		push 48(%rbp)
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -80(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -128(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -80(%rbp), %rdx
# INSTRUCTION_MINUS
		sub %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -80(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -128(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH_STACK
		push 64(%rbp)
# INSTRUCTION_FUNCTION_CALL
		call quicksort__0
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -72(%rbp)
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rcx
movq %rbp, 8(%rcx)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -72(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -48(%rbp)
# COMPLEX_SAVE_ALL
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -136(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -136(%rbp), %rcx
# INSTRUCTION_ADD_CONST
		add $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -136(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH_STACK
		push 120(%rbp)
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -80(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH_STACK
		push 64(%rbp)
# INSTRUCTION_FUNCTION_CALL
		call quicksort__0
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -88(%rbp)
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rcx
movq %rbp, 8(%rcx)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -88(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -48(%rbp)
# INSTRUCTION_LABEL
		if_7_end:
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -152(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -152(%rbp), %rcx
# INSTRUCTION_RETURN
		mov %rcx, %rax
		mov %rbp,%rsp
pop %rbp
ret
# INSTRUCTION_POP
		pop %rcx
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
xorq %rax,%rax
ret
# INSTRUCTION_PUSH
	push %rcx
# INSTRUCTION_FUNCTION_LABEL
.type partition__0, @function
partition__0:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
	subq $136, %rsp
	movq %rax, -8(%rbp)
	movq $0, -16(%rbp)
	leaq staticLink, %rcx
	movq %rbp, 8(%rcx)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# METADATA_FUNCTION_ARGUMENT
		mov 16(%rbp), %rcx
		mov %rcx, -32(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# METADATA_FUNCTION_ARGUMENT
		mov 24(%rbp), %rcx
		mov %rcx, -24(%rbp)
# INSTRUCTION_POP
		pop %rcx
# VAR dummy
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rcx
		mov 0(%rcx), %rcx
		movq -16(%rcx), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov -32(%rcx, %rbx, 8), %rdx
# INSTRUCTION_POP
		pop %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -72(%rbp)
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -120(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -120(%rbp), %rcx
# INSTRUCTION_MUL_CONST
		imul $8, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -120(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -72(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -120(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rbx, -80(%rbp)
# INSTRUCTION_POP
		pop %rbx
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -80(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -40(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rcx
		mov 0(%rcx), %rcx
		movq -16(%rcx), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov -24(%rcx, %rbx, 8), %rdx
# INSTRUCTION_POP
		pop %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -128(%rbp)
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -144(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -144(%rbp), %rcx
# INSTRUCTION_MUL_CONST
		imul $8, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -144(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -128(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -144(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rbx, -56(%rbp)
# INSTRUCTION_POP
		pop %rbx
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -48(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -152(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -160(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -152(%rbp), %rdx
# INSTRUCTION_MINUS
		sub %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -152(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -160(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -152(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -56(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -168(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -168(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -64(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_LABEL
		while_cnd_3:
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -64(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -80(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -176(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -184(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -176(%rbp), %rdx
# INSTRUCTION_MINUS
		sub %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -176(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -184(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -176(%rbp), %rdx
# INSTRUCTION_MINUS
		sub %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -176(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -80(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -176(%rbp), %rcx
# INSTRUCTION_ADD_CONST
		add $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -176(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -176(%rbp), %rcx
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rdx
		cmp $0, %rcx
		setg %dl
		movsx %dl, %rcx
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -96(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -96(%rbp), %rdx
# INSTRUCTION_CMP
		cmp %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -96(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -176(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_JE
		je while_3_begin
# INSTRUCTION_JMP
		jmp while_3_end
# INSTRUCTION_LABEL
		while_3_begin:
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rcx
		mov 0(%rcx), %rcx
		movq -16(%rcx), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov -32(%rcx, %rbx, 8), %rdx
# INSTRUCTION_POP
		pop %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -192(%rbp)
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -64(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -208(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -208(%rbp), %rcx
# INSTRUCTION_MUL_CONST
		imul $8, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -208(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -192(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -208(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rbx, -64(%rbp)
# INSTRUCTION_POP
		pop %rbx
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -112(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -64(%rbp), %rdx
# INSTRUCTION_MUL
		imul %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -64(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -112(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -224(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rcx
		mov 0(%rcx), %rcx
		movq -16(%rcx), %rbx
		addq $1, %rbx
		imul $-1, %rbx
		mov -24(%rcx, %rbx, 8), %rdx
# INSTRUCTION_POP
		pop %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -72(%rbp)
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -64(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -248(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -248(%rbp), %rcx
# INSTRUCTION_MUL_CONST
		imul $8, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -248(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -72(%rbp), %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -248(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rbx, -88(%rbp)
# INSTRUCTION_POP
		pop %rbx
# INSTRUCTION_POP
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -224(%rbp), %rdx
# INSTRUCTION_MUL
		imul %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -224(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -88(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -64(%rbp), %rdx
# INSTRUCTION_MINUS
		sub %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -64(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -224(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -64(%rbp), %rcx
# INSTRUCTION_ADD_CONST
		add $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -64(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -64(%rbp), %rcx
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rdx
		cmp $0, %rcx
		setg %dl
		movsx %dl, %rcx
		pop %rdx
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -264(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -264(%rbp), %rdx
# INSTRUCTION_CMP
		cmp %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -264(%rbp)
# INSTRUCTION_POP
		pop %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -64(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_JE
		je if_8_begin
# INSTRUCTION_JMP
		jmp if_8_end
# INSTRUCTION_LABEL
		if_8_begin:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rbx
# INSTRUCTION_ADD_CONST
		add $1, %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rbx, -56(%rbp)
# COMPLEX_SAVE_ALL
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -280(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH_STACK
		push 264(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -64(%rbp), %rdx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_FUNCTION_CALL
		call exchange__0
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -96(%rbp)
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rcx
movq %rbp, 8(%rcx)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -96(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -72(%rbp)
# INSTRUCTION_LABEL
		if_8_end:
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -64(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -288(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -288(%rbp), %rcx
# INSTRUCTION_ADD_CONST
		add $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -288(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -288(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -64(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_JMP
		jmp while_cnd_3
# INSTRUCTION_LABEL
		while_3_end:
# COMPLEX_SAVE_ALL
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -104(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -104(%rbp), %rcx
# INSTRUCTION_ADD_CONST
		add $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -104(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH_STACK
		push 88(%rbp)
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -304(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH_STACK
		push 288(%rbp)
# INSTRUCTION_FUNCTION_CALL
		call exchange__0
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -112(%rbp)
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rcx
movq %rbp, 8(%rcx)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -112(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -72(%rbp)
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -320(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -320(%rbp), %rcx
# INSTRUCTION_ADD_CONST
		add $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rcx, -320(%rbp)
# INSTRUCTION_POP
		pop %rcx
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -320(%rbp), %rcx
# INSTRUCTION_RETURN
		mov %rcx, %rax
		mov %rbp,%rsp
pop %rbp
ret
# INSTRUCTION_POP
		pop %rcx
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
xorq %rax,%rax
ret
