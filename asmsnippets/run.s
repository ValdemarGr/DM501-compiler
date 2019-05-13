.section .data
staticLink:
	.space 24
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
# VAR d
# VAR c
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
	subq $64, %rsp
	popq %rax
	movq %rax, -8(%rbp)
	movq $2, -16(%rbp)
	movq $0, -24(%rbp)
	movq $0, -40(%rbp)
	movq $1, -32(%rbp)
	movq $0, -48(%rbp)
	leaq staticLink, %rax
	movq %rbp, (%rax)
# INSTRUCTION_CONST
		mov $2, %rcx
# COMPLEX_ALLOCATE
		movq $8, %rdx
		imulq %rcx, %rdx
# ALLOC_RECORD_CLASS
		addq $24, %rdx
		pushq %rdx
		pushq %rbp
		call garbageCollectAllocate
		movq %rcx, 0(%rax)
# ALLOC_RECORD_CLASS
		subq $16, %rdx
		movq $1, (%rax, %rdx, 1)
		movq $0, 8(%rax, %rdx, 1)
		popq %rdx
		popq %rdx
		addq $8, %rax
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -40(%rbp)
# INSTRUCTION_CONST
		mov $40, %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rsi
# INSTRUCTION_CONST
		mov $8, %rdi
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rbx, (%rsi, %rdi,1)
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_CONST
		mov $2, %r8
# COMPLEX_ALLOCATE
		movq $8, %r9
		imulq %r8, %r9
# ALLOC_RECORD_CLASS
		addq $24, %r9
		pushq %r9
		pushq %rbp
		call garbageCollectAllocate
		movq %r8, 0(%rax)
# ALLOC_RECORD_CLASS
		subq $16, %r9
		movq $1, (%rax, %r9, 1)
		movq $0, 8(%rax, %r9, 1)
		popq %r9
		popq %r9
		addq $8, %rax
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -48(%rbp)
# INSTRUCTION_CONST
		mov $30, %r10
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r11
# INSTRUCTION_CONST
		mov $8, %r12
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r10, (%r11, %r12,1)
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_CONST
		mov $2, %r13
# COMPLEX_ALLOCATE
		movq $8, %r14
		imulq %r13, %r14
# ALLOC_RECORD_CLASS
		addq $24, %r14
		pushq %r14
		pushq %rbp
		call garbageCollectAllocate
		movq %r13, 0(%rax)
# ALLOC_RECORD_CLASS
		subq $16, %r14
		movq $1, (%rax, %r14, 1)
		movq $0, 8(%rax, %r14, 1)
		popq %r14
		popq %r14
		addq $8, %rax
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r15
# INSTRUCTION_CONST
		mov $0, %rcx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rax, (%r15, %rcx,1)
# INSTRUCTION_CONST
		mov $20, %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rbx
# INSTRUCTION_CONST
		mov $0, %rsi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rsi,1), %rdi
# INSTRUCTION_CONST
		mov $8, %r8
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rdx, (%rdi, %r8,1)
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_CONST
		mov $3, %r9
# COMPLEX_ALLOCATE
		movq $8, %r10
		imulq %r9, %r10
# ALLOC_RECORD_CLASS
		addq $24, %r10
		pushq %r10
		pushq %rbp
		call garbageCollectAllocate
		movq %r9, 0(%rax)
# ALLOC_RECORD_CLASS
		subq $16, %r10
		movq $1, (%rax, %r10, 1)
		movq $2, 8(%rax, %r10, 1)
		popq %r10
		popq %r10
		addq $8, %rax
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r11
# INSTRUCTION_CONST
		mov $0, %r12
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r11, %r12,1), %r13
# INSTRUCTION_CONST
		mov $0, %r14
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rax, (%r13, %r14,1)
# INSTRUCTION_CONST
		mov $10, %r15
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rcx
# INSTRUCTION_CONST
		mov $0, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rbx
# INSTRUCTION_CONST
		mov $0, %rsi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rsi,1), %rdi
# INSTRUCTION_CONST
		mov $8, %r8
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r15, (%rdi, %r8,1)
# COMPLEX_RIP_LAMBDA_LOAD
		leaq lambda_0(%rip), %r9
# INSTRUCTION_CONST
		mov $1, %r10
# COMPLEX_ALLOCATE
		movq $8, %r11
		imulq %r10, %r11
# ALLOC_LAMBDA
		addq $32, %r11
		pushq %r11
		pushq %rbp
		call garbageCollectAllocate
		movq $2, 0(%rax)
		movq $1, 24(%rax)
		movq $1, 32(%rax)
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
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rdx
# INSTRUCTION_CONST
		mov $0, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdx, %rbx,1), %rsi
# INSTRUCTION_CONST
		mov $0, %rdi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rsi, %rdi,1), %r8
# INSTRUCTION_CONST
		mov $16, %r9
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r15, (%r8, %r9,1)
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_CONST
		mov $42, %r10
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r11
# INSTRUCTION_CONST
		mov $0, %r12
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r11, %r12,1), %r13
# INSTRUCTION_CONST
		mov $0, %r14
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r13, %r14,1), %r15
# INSTRUCTION_CONST
		mov $0, %rcx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r10, (%r15, %rcx,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rdx
# INSTRUCTION_CONST
		mov $0, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdx, %rbx,1), %rsi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rdi
# INSTRUCTION_CONST
		mov $0, %r8
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rsi, (%rdi, %r8,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r9
# INSTRUCTION_CONST
		mov $8, %r10
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r9, %r10,1), %r11
# INSTRUCTION_WRITE
		movq %r11, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r12
# INSTRUCTION_CONST
		mov $8, %r13
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r12, %r13,1), %r14
# INSTRUCTION_WRITE
		movq %r14, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r15
# INSTRUCTION_CONST
		mov $0, %rcx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r15, %rcx,1), %rdx
# INSTRUCTION_CONST
		mov $8, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdx, %rbx,1), %rsi
# INSTRUCTION_WRITE
		movq %rsi, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rdi
# INSTRUCTION_CONST
		mov $0, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %r8,1), %r9
# INSTRUCTION_CONST
		mov $0, %r10
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r9, %r10,1), %r11
# INSTRUCTION_CONST
		mov $8, %r12
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r11, %r12,1), %r13
# INSTRUCTION_WRITE
		movq %r13, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r14
# INSTRUCTION_CONST
		mov $0, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r14, %r15,1), %rcx
# INSTRUCTION_CONST
		mov $0, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rbx
# INSTRUCTION_CONST
		mov $16, %rsi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rsi,1), %rdi
# COMPLEX_SAVE_ALL
# INSTRUCTION_CONST
		mov $8, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %r8,1), %r9
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %r9
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r10
# INSTRUCTION_CONST
		mov $0, %r11
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r10, %r11,1), %r12
# INSTRUCTION_CONST
		mov $0, %r13
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r12, %r13,1), %r14
# INSTRUCTION_CONST
		mov $16, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r14, %r15,1), %rcx
# INSTRUCTION_CONST
		mov $0, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rbx
# INSTRUCTION_REGISTER_CALL
		call *%rbx
# INSTRUCTION_ADD_STACK_PTR
		addq $8, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rsi
movq %rbp, 0(%rsi)
# INSTRUCTION_CONST
		mov $0, %rdi
# INSTRUCTION_WRITE
		movq %rdi, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# INSTRUCTION_CONST
		mov $0, %r8
# INSTRUCTION_WRITE
		movq %r8, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_GARBAGE_COLLECT
		pushq %rbp
		call garbageCollect
		popq %rbp
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r9
# INSTRUCTION_CONST
		mov $8, %r10
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r9, %r10,1), %r11
# INSTRUCTION_WRITE
		movq %r11, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r12
# INSTRUCTION_CONST
		mov $8, %r13
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r12, %r13,1), %r14
# INSTRUCTION_WRITE
		movq %r14, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r15
# INSTRUCTION_CONST
		mov $0, %rcx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r15, %rcx,1), %rdx
# INSTRUCTION_CONST
		mov $8, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdx, %rbx,1), %rsi
# INSTRUCTION_WRITE
		movq %rsi, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rdi
# INSTRUCTION_CONST
		mov $0, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %r8,1), %r9
# INSTRUCTION_CONST
		mov $0, %r10
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r9, %r10,1), %r11
# INSTRUCTION_CONST
		mov $8, %r12
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r11, %r12,1), %r13
# INSTRUCTION_WRITE
		movq %r13, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r14
# INSTRUCTION_CONST
		mov $0, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r14, %r15,1), %rcx
# INSTRUCTION_CONST
		mov $0, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rbx
# INSTRUCTION_CONST
		mov $16, %rsi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rsi,1), %rdi
# COMPLEX_SAVE_ALL
# INSTRUCTION_CONST
		mov $8, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %r8,1), %r9
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %r9
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r10
# INSTRUCTION_CONST
		mov $0, %r11
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r10, %r11,1), %r12
# INSTRUCTION_CONST
		mov $0, %r13
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r12, %r13,1), %r14
# INSTRUCTION_CONST
		mov $16, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r14, %r15,1), %rcx
# INSTRUCTION_CONST
		mov $0, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rbx
# INSTRUCTION_REGISTER_CALL
		call *%rbx
# INSTRUCTION_ADD_STACK_PTR
		addq $8, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rsi
movq %rbp, 0(%rsi)
# INSTRUCTION_CONST
		mov $0, %rdi
# INSTRUCTION_WRITE
		movq %rdi, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# INSTRUCTION_CONST
		mov $0, %r8
# INSTRUCTION_WRITE
		movq %r8, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_GARBAGE_COLLECT
		pushq %rbp
		call garbageCollect
		popq %rbp
# INSTRUCTION_CONST
		mov $2, %r9
# COMPLEX_ALLOCATE
		movq $8, %r10
		imulq %r9, %r10
# ALLOC_RECORD_CLASS
		addq $24, %r10
		pushq %r10
		pushq %rbp
		call garbageCollectAllocate
		movq %r9, 0(%rax)
# ALLOC_RECORD_CLASS
		subq $16, %r10
		movq $1, (%rax, %r10, 1)
		movq $0, 8(%rax, %r10, 1)
		popq %r10
		popq %r10
		addq $8, %rax
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r11
# INSTRUCTION_CONST
		mov $0, %r12
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rax, (%r11, %r12,1)
# INSTRUCTION_CONST
		mov $20, %r13
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r14
# INSTRUCTION_CONST
		mov $0, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r14, %r15,1), %rcx
# INSTRUCTION_CONST
		mov $8, %rdx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r13, (%rcx, %rdx,1)
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_CONST
		mov $3, %rbx
# COMPLEX_ALLOCATE
		movq $8, %rsi
		imulq %rbx, %rsi
# ALLOC_RECORD_CLASS
		addq $24, %rsi
		pushq %rsi
		pushq %rbp
		call garbageCollectAllocate
		movq %rbx, 0(%rax)
# ALLOC_RECORD_CLASS
		subq $16, %rsi
		movq $1, (%rax, %rsi, 1)
		movq $2, 8(%rax, %rsi, 1)
		popq %rsi
		popq %rsi
		addq $8, %rax
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rdi
# INSTRUCTION_CONST
		mov $0, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %r8,1), %r9
# INSTRUCTION_CONST
		mov $0, %r10
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rax, (%r9, %r10,1)
# INSTRUCTION_CONST
		mov $10, %r11
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r12
# INSTRUCTION_CONST
		mov $0, %r13
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r12, %r13,1), %r14
# INSTRUCTION_CONST
		mov $0, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r14, %r15,1), %rcx
# INSTRUCTION_CONST
		mov $8, %rdx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r11, (%rcx, %rdx,1)
# COMPLEX_RIP_LAMBDA_LOAD
		leaq lambda_0(%rip), %rbx
# INSTRUCTION_CONST
		mov $1, %rsi
# COMPLEX_ALLOCATE
		movq $8, %rdi
		imulq %rsi, %rdi
# ALLOC_LAMBDA
		addq $32, %rdi
		pushq %rdi
		pushq %rbp
		call garbageCollectAllocate
		movq $2, 0(%rax)
		movq $1, 24(%rax)
		movq $1, 32(%rax)
		popq %rdi
		popq %rdi
		addq $8, %rax
# INSTRUCTION_CONST
		mov $0, %r8
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rbx, (%rax, %r8,1)
# INSTRUCTION_COPY
		mov %rax, %r9
# INSTRUCTION_CONST
		mov $8, %r10
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rax, (%r9, %r10,1)
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r11
# INSTRUCTION_CONST
		mov $0, %r12
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r11, %r12,1), %r13
# INSTRUCTION_CONST
		mov $0, %r14
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r13, %r14,1), %r15
# INSTRUCTION_CONST
		mov $16, %rcx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r9, (%r15, %rcx,1)
# INSTRUCTION_POP
		pop %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rdx
# INSTRUCTION_CONST
		mov $0, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdx, %rbx,1), %rsi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rdi
# INSTRUCTION_CONST
		mov $0, %r8
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rsi, (%rdi, %r8,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r9
# INSTRUCTION_CONST
		mov $8, %r10
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r9, %r10,1), %r11
# INSTRUCTION_WRITE
		movq %r11, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r12
# INSTRUCTION_CONST
		mov $8, %r13
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r12, %r13,1), %r14
# INSTRUCTION_WRITE
		movq %r14, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r15
# INSTRUCTION_CONST
		mov $0, %rcx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r15, %rcx,1), %rdx
# INSTRUCTION_CONST
		mov $8, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdx, %rbx,1), %rsi
# INSTRUCTION_WRITE
		movq %rsi, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rdi
# INSTRUCTION_CONST
		mov $0, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %r8,1), %r9
# INSTRUCTION_CONST
		mov $0, %r10
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r9, %r10,1), %r11
# INSTRUCTION_CONST
		mov $8, %r12
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r11, %r12,1), %r13
# INSTRUCTION_WRITE
		movq %r13, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r14
# INSTRUCTION_CONST
		mov $0, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r14, %r15,1), %rcx
# INSTRUCTION_CONST
		mov $0, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rbx
# INSTRUCTION_CONST
		mov $16, %rsi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rsi,1), %rdi
# COMPLEX_SAVE_ALL
# INSTRUCTION_CONST
		mov $8, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %r8,1), %r9
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %r9
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r10
# INSTRUCTION_CONST
		mov $0, %r11
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r10, %r11,1), %r12
# INSTRUCTION_CONST
		mov $0, %r13
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r12, %r13,1), %r14
# INSTRUCTION_CONST
		mov $16, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r14, %r15,1), %rcx
# INSTRUCTION_CONST
		mov $0, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rbx
# INSTRUCTION_REGISTER_CALL
		call *%rbx
# INSTRUCTION_ADD_STACK_PTR
		addq $8, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rsi
movq %rbp, 0(%rsi)
# INSTRUCTION_CONST
		mov $0, %rdi
# INSTRUCTION_WRITE
		movq %rdi, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# INSTRUCTION_CONST
		mov $0, %r8
# INSTRUCTION_WRITE
		movq %r8, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_GARBAGE_COLLECT
		pushq %rbp
		call garbageCollect
		popq %rbp
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r9
# INSTRUCTION_CONST
		mov $8, %r10
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r9, %r10,1), %r11
# INSTRUCTION_WRITE
		movq %r11, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r12
# INSTRUCTION_CONST
		mov $8, %r13
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r12, %r13,1), %r14
# INSTRUCTION_WRITE
		movq %r14, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r15
# INSTRUCTION_CONST
		mov $0, %rcx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r15, %rcx,1), %rdx
# INSTRUCTION_CONST
		mov $8, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdx, %rbx,1), %rsi
# INSTRUCTION_WRITE
		movq %rsi, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rdi
# INSTRUCTION_CONST
		mov $0, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %r8,1), %r9
# INSTRUCTION_CONST
		mov $0, %r10
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r9, %r10,1), %r11
# INSTRUCTION_CONST
		mov $8, %r12
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r11, %r12,1), %r13
# INSTRUCTION_WRITE
		movq %r13, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r14
# INSTRUCTION_CONST
		mov $0, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r14, %r15,1), %rcx
# INSTRUCTION_CONST
		mov $0, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rbx
# INSTRUCTION_CONST
		mov $16, %rsi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rsi,1), %rdi
# COMPLEX_SAVE_ALL
# INSTRUCTION_CONST
		mov $8, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %r8,1), %r9
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %r9
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r10
# INSTRUCTION_CONST
		mov $0, %r11
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r10, %r11,1), %r12
# INSTRUCTION_CONST
		mov $0, %r13
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r12, %r13,1), %r14
# INSTRUCTION_CONST
		mov $16, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r14, %r15,1), %rcx
# INSTRUCTION_CONST
		mov $0, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rbx
# INSTRUCTION_REGISTER_CALL
		call *%rbx
# INSTRUCTION_ADD_STACK_PTR
		addq $8, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rsi
movq %rbp, 0(%rsi)
# INSTRUCTION_CONST
		mov $0, %rdi
# INSTRUCTION_WRITE
		movq %rdi, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# INSTRUCTION_CONST
		mov $0, %r8
# INSTRUCTION_WRITE
		movq %r8, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_GARBAGE_COLLECT
		pushq %rbp
		call garbageCollect
		popq %rbp
# INSTRUCTION_CONST
		mov $2, %r9
# COMPLEX_ALLOCATE
		movq $8, %r10
		imulq %r9, %r10
# ALLOC_RECORD_CLASS
		addq $24, %r10
		pushq %r10
		pushq %rbp
		call garbageCollectAllocate
		movq %r9, 0(%rax)
# ALLOC_RECORD_CLASS
		subq $16, %r10
		movq $1, (%rax, %r10, 1)
		movq $0, 8(%rax, %r10, 1)
		popq %r10
		popq %r10
		addq $8, %rax
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r11
# INSTRUCTION_CONST
		mov $0, %r12
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rax, (%r11, %r12,1)
# INSTRUCTION_CONST
		mov $20, %r13
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r14
# INSTRUCTION_CONST
		mov $0, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r14, %r15,1), %rcx
# INSTRUCTION_CONST
		mov $8, %rdx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r13, (%rcx, %rdx,1)
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_CONST
		mov $3, %rbx
# COMPLEX_ALLOCATE
		movq $8, %rsi
		imulq %rbx, %rsi
# ALLOC_RECORD_CLASS
		addq $24, %rsi
		pushq %rsi
		pushq %rbp
		call garbageCollectAllocate
		movq %rbx, 0(%rax)
# ALLOC_RECORD_CLASS
		subq $16, %rsi
		movq $1, (%rax, %rsi, 1)
		movq $2, 8(%rax, %rsi, 1)
		popq %rsi
		popq %rsi
		addq $8, %rax
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rdi
# INSTRUCTION_CONST
		mov $0, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %r8,1), %r9
# INSTRUCTION_CONST
		mov $0, %r10
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rax, (%r9, %r10,1)
# INSTRUCTION_CONST
		mov $10, %r11
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r12
# INSTRUCTION_CONST
		mov $0, %r13
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r12, %r13,1), %r14
# INSTRUCTION_CONST
		mov $0, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r14, %r15,1), %rcx
# INSTRUCTION_CONST
		mov $8, %rdx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r11, (%rcx, %rdx,1)
# COMPLEX_RIP_LAMBDA_LOAD
		leaq lambda_0(%rip), %rbx
# INSTRUCTION_CONST
		mov $1, %rsi
# COMPLEX_ALLOCATE
		movq $8, %rdi
		imulq %rsi, %rdi
# ALLOC_LAMBDA
		addq $32, %rdi
		pushq %rdi
		pushq %rbp
		call garbageCollectAllocate
		movq $2, 0(%rax)
		movq $1, 24(%rax)
		movq $1, 32(%rax)
		popq %rdi
		popq %rdi
		addq $8, %rax
# INSTRUCTION_CONST
		mov $0, %r8
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rbx, (%rax, %r8,1)
# INSTRUCTION_COPY
		mov %rax, %r9
# INSTRUCTION_CONST
		mov $8, %r10
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rax, (%r9, %r10,1)
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r11
# INSTRUCTION_CONST
		mov $0, %r12
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r11, %r12,1), %r13
# INSTRUCTION_CONST
		mov $0, %r14
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r13, %r14,1), %r15
# INSTRUCTION_CONST
		mov $16, %rcx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r9, (%r15, %rcx,1)
# INSTRUCTION_POP
		pop %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rdx
# INSTRUCTION_CONST
		mov $0, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdx, %rbx,1), %rsi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rdi
# INSTRUCTION_CONST
		mov $0, %r8
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rsi, (%rdi, %r8,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r9
# INSTRUCTION_CONST
		mov $8, %r10
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r9, %r10,1), %r11
# INSTRUCTION_WRITE
		movq %r11, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r12
# INSTRUCTION_CONST
		mov $8, %r13
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r12, %r13,1), %r14
# INSTRUCTION_WRITE
		movq %r14, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r15
# INSTRUCTION_CONST
		mov $0, %rcx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r15, %rcx,1), %rdx
# INSTRUCTION_CONST
		mov $8, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdx, %rbx,1), %rsi
# INSTRUCTION_WRITE
		movq %rsi, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rdi
# INSTRUCTION_CONST
		mov $0, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %r8,1), %r9
# INSTRUCTION_CONST
		mov $0, %r10
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r9, %r10,1), %r11
# INSTRUCTION_CONST
		mov $8, %r12
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r11, %r12,1), %r13
# INSTRUCTION_WRITE
		movq %r13, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r14
# INSTRUCTION_CONST
		mov $0, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r14, %r15,1), %rcx
# INSTRUCTION_CONST
		mov $0, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rbx
# INSTRUCTION_CONST
		mov $16, %rsi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rsi,1), %rdi
# COMPLEX_SAVE_ALL
# INSTRUCTION_CONST
		mov $8, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %r8,1), %r9
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %r9
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r10
# INSTRUCTION_CONST
		mov $0, %r11
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r10, %r11,1), %r12
# INSTRUCTION_CONST
		mov $0, %r13
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r12, %r13,1), %r14
# INSTRUCTION_CONST
		mov $16, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r14, %r15,1), %rcx
# INSTRUCTION_CONST
		mov $0, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rbx
# INSTRUCTION_REGISTER_CALL
		call *%rbx
# INSTRUCTION_ADD_STACK_PTR
		addq $8, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rsi
movq %rbp, 0(%rsi)
# INSTRUCTION_CONST
		mov $0, %rdi
# INSTRUCTION_WRITE
		movq %rdi, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# INSTRUCTION_CONST
		mov $0, %r8
# INSTRUCTION_WRITE
		movq %r8, %rsi
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
.type lambda_0, @function
lambda_0:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
	subq $40, %rsp
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
		mov (%r15, %rcx,1), %rdx
# INSTRUCTION_WRITE
		movq %rdx, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
xorq %rax,%rax
ret
