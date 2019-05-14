.section .data
staticLink:
	.space 24
intprint:
	.asciz "%i\n"
charprint:
	.asciz "%c"
nlprint:
	.asciz "\n"
gconeprint:
    .asciz "Gc one\n"
gctwoprint:
    .asciz "Gc two\n"
gcrueentsize:
    .asciz "Current size: %i\n"
gcinuse:
    .asciz "In use: %i\n"
gcstataddr:
    .asciz "Start address: %i\n"
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
.type gcPrintDebug, @function
gcPrintDebug:
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

    # for gc one
    leaq gcHeapOne, %r15

    movq $gconeprint, %rdi
    movq $0, %rax
    call printf

    movq 0(%r15), %rsi
    movq $gcinuse, %rdi
    movq $0, %rax
    call printf

    movq 8(%r15), %rsi
    movq $gcrueentsize, %rdi
    movq $0, %rax
    call printf

    movq 24(%r15), %rsi
    movq $gcstataddr, %rdi
    movq $0, %rax
    call printf

    # for gc two
    leaq gcHeapTwo, %r15

    movq $gctwoprint, %rdi
    movq $0, %rax
    call printf

    movq 0(%r15), %rsi
    movq $gcinuse, %rdi
    movq $0, %rax
    call printf

    movq 8(%r15), %rsi
    movq $gcrueentsize, %rdi
    movq $0, %rax
    call printf

    movq 24(%r15), %rsi
    movq $gcstataddr, %rdi
    movq $0, %rax
    call printf


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

    movq %rax, 8(%r15) # rax has current new heap pos



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

    push %rsi
    push %r15
    push %r14

    #args
    #rbp
    #desired byte size

    movq 16(%rbp), %rax
    movq %rax, -16(%rbp)
    movq 24(%rbp), %rax
    movq %rax, -8(%rbp)

    # check if we need to gc
    leaq gcHeapOne, %r15
    cmp $1, 0(%r15)
    je heapSelectorEndAlloc1
    leaq gcHeapTwo, %r15
    heapSelectorEndAlloc1:

    # current heap position
    movq 8(%r15), %r14
    # add new size
    movq -8(%rbp), %rax
    addq %r14, %rax
    # buffer
    addq $40, %rax
    cmp 16(%r15), %rax
    #if we are in the safe zone
    jl continueTheAlloc
        pushq -16(%rbp)
        call garbageCollect
        addq $8, %rsp
    continueTheAlloc:


    # do the alloc
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

    movq $0, %rsi
    # set all fields to 0
    zeroSetterCmp:
    cmp %rsi, -8(%rbp)
    je zeroSetterEnd
    movq $0, (%rax, %rsi, 1)
    addq $8, %rsi
    jmp zeroSetterCmp
    zeroSetterEnd:

    pop %r14
    pop %r15
    pop %rsi

    mov %rbp,%rsp
    pop %rbp
    ret
# METADATA_BEGIN_BODY_BLOCK
# VAR s
# VAR s2
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
# METADATA_DEBUG_INFO
		# ALLOC
# METADATA_DEBUG_INFO
		# s
# INSTRUCTION_CONST
		mov $7, %rbx
# COMPLEX_ALLOCATE
		movq $8, %rsi
		imulq %rbx, %rsi
# ALLOC_RECORD_CLASS
		addq $64, %rsi
		pushq %rsi
		pushq %rbp
		call garbageCollectAllocate
		movq %rbx, 0(%rax)
# ALLOC_RECORD_CLASS
		subq $56, %rsi
		movq $6, (%rax, %rsi, 1)
		movq $1, 8(%rax, %rsi, 1)
		movq $2, 16(%rax, %rsi, 1)
		movq $3, 24(%rax, %rsi, 1)
		movq $4, 32(%rax, %rsi, 1)
		movq $5, 40(%rax, %rsi, 1)
		movq $6, 48(%rax, %rsi, 1)
		popq %rsi
		popq %rsi
		addq $8, %rax
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -40(%rbp)
# COMPLEX_RIP_LAMBDA_LOAD
		leaq lambda_0(%rip), %rdi
# INSTRUCTION_CONST
		mov $1, %r8
# COMPLEX_ALLOCATE
		movq $8, %r9
		imulq %r8, %r9
# ALLOC_LAMBDA
		addq $32, %r9
		pushq %r9
		pushq %rbp
		call garbageCollectAllocate
		movq $2, 0(%rax)
		movq $1, 24(%rax)
		movq $1, 32(%rax)
		popq %r9
		popq %r9
		addq $8, %rax
# INSTRUCTION_CONST
		mov $0, %r10
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rdi, (%rax, %r10,1)
# INSTRUCTION_COPY
		mov %rax, %r13
# INSTRUCTION_CONST
		mov $8, %r14
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rax, (%r13, %r14,1)
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r15
# INSTRUCTION_CONST
		mov $16, %rcx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r13, (%r15, %rcx,1)
# COMPLEX_RIP_LAMBDA_LOAD
		leaq lambda_1(%rip), %rdx
# INSTRUCTION_CONST
		mov $1, %rbx
# COMPLEX_ALLOCATE
		movq $8, %rsi
		imulq %rbx, %rsi
# ALLOC_LAMBDA
		addq $32, %rsi
		pushq %rsi
		pushq %rbp
		call garbageCollectAllocate
		movq $2, 0(%rax)
		movq $1, 24(%rax)
		movq $1, 32(%rax)
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
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r12
# INSTRUCTION_CONST
		mov $24, %r13
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r10, (%r12, %r13,1)
# COMPLEX_RIP_LAMBDA_LOAD
		leaq lambda_2(%rip), %r14
# INSTRUCTION_CONST
		mov $1, %r15
# COMPLEX_ALLOCATE
		movq $8, %rcx
		imulq %r15, %rcx
# ALLOC_LAMBDA
		addq $32, %rcx
		pushq %rcx
		pushq %rbp
		call garbageCollectAllocate
		movq $2, 0(%rax)
		movq $1, 24(%rax)
		movq $1, 32(%rax)
		popq %rcx
		popq %rcx
		addq $8, %rax
# INSTRUCTION_CONST
		mov $0, %rdx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r14, (%rax, %rdx,1)
# INSTRUCTION_COPY
		mov %rax, %rdi
# INSTRUCTION_CONST
		mov $8, %r8
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rax, (%rdi, %r8,1)
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r9
# INSTRUCTION_CONST
		mov $32, %r10
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rdi, (%r9, %r10,1)
# COMPLEX_RIP_LAMBDA_LOAD
		leaq lambda_3(%rip), %r11
# INSTRUCTION_CONST
		mov $1, %r12
# COMPLEX_ALLOCATE
		movq $8, %r13
		imulq %r12, %r13
# ALLOC_LAMBDA
		addq $32, %r13
		pushq %r13
		pushq %rbp
		call garbageCollectAllocate
		movq $2, 0(%rax)
		movq $1, 24(%rax)
		movq $1, 32(%rax)
		popq %r13
		popq %r13
		addq $8, %rax
# INSTRUCTION_CONST
		mov $0, %r14
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r11, (%rax, %r14,1)
# INSTRUCTION_COPY
		mov %rax, %rdx
# INSTRUCTION_CONST
		mov $8, %rbx
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rax, (%rdx, %rbx,1)
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rsi
# INSTRUCTION_CONST
		mov $40, %rdi
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rdx, (%rsi, %rdi,1)
# COMPLEX_RIP_LAMBDA_LOAD
		leaq lambda_4(%rip), %r8
# INSTRUCTION_CONST
		mov $1, %r9
# COMPLEX_ALLOCATE
		movq $8, %r10
		imulq %r9, %r10
# ALLOC_LAMBDA
		addq $32, %r10
		pushq %r10
		pushq %rbp
		call garbageCollectAllocate
		movq $2, 0(%rax)
		movq $1, 24(%rax)
		movq $1, 32(%rax)
		popq %r10
		popq %r10
		addq $8, %rax
# INSTRUCTION_CONST
		mov $0, %r11
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r8, (%rax, %r11,1)
# INSTRUCTION_COPY
		mov %rax, %r14
# INSTRUCTION_CONST
		mov $8, %r15
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rax, (%r14, %r15,1)
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rcx
# INSTRUCTION_CONST
		mov $48, %rdx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r14, (%rcx, %rdx,1)
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_PUSH
		push %rax
# INSTRUCTION_CONST
		mov $30, %rbx
# INSTRUCTION_PUSH
		push %rbx
# INSTRUCTION_PUSH
		push %rax
# INSTRUCTION_FUNCTION_CALL
		call String_constructor
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# INSTRUCTION_POP
		pop %rax
# METADATA_DEBUG_INFO
		# ALLOC
# METADATA_DEBUG_INFO
		# s2
# INSTRUCTION_CONST
		mov $7, %rsi
# COMPLEX_ALLOCATE
		movq $8, %rdi
		imulq %rsi, %rdi
# ALLOC_RECORD_CLASS
		addq $64, %rdi
		pushq %rdi
		pushq %rbp
		call garbageCollectAllocate
		movq %rsi, 0(%rax)
# ALLOC_RECORD_CLASS
		subq $56, %rdi
		movq $6, (%rax, %rdi, 1)
		movq $1, 8(%rax, %rdi, 1)
		movq $2, 16(%rax, %rdi, 1)
		movq $3, 24(%rax, %rdi, 1)
		movq $4, 32(%rax, %rdi, 1)
		movq $5, 40(%rax, %rdi, 1)
		movq $6, 48(%rax, %rdi, 1)
		popq %rdi
		popq %rdi
		addq $8, %rax
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -48(%rbp)
# COMPLEX_RIP_LAMBDA_LOAD
		leaq lambda_0(%rip), %r8
# INSTRUCTION_CONST
		mov $1, %r9
# COMPLEX_ALLOCATE
		movq $8, %r10
		imulq %r9, %r10
# ALLOC_LAMBDA
		addq $32, %r10
		pushq %r10
		pushq %rbp
		call garbageCollectAllocate
		movq $2, 0(%rax)
		movq $1, 24(%rax)
		movq $1, 32(%rax)
		popq %r10
		popq %r10
		addq $8, %rax
# INSTRUCTION_CONST
		mov $0, %r11
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r8, (%rax, %r11,1)
# INSTRUCTION_COPY
		mov %rax, %r12
# INSTRUCTION_CONST
		mov $8, %r13
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rax, (%r12, %r13,1)
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r14
# INSTRUCTION_CONST
		mov $16, %r15
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r12, (%r14, %r15,1)
# COMPLEX_RIP_LAMBDA_LOAD
		leaq lambda_1(%rip), %rcx
# INSTRUCTION_CONST
		mov $1, %rdx
# COMPLEX_ALLOCATE
		movq $8, %rbx
		imulq %rdx, %rbx
# ALLOC_LAMBDA
		addq $32, %rbx
		pushq %rbx
		pushq %rbp
		call garbageCollectAllocate
		movq $2, 0(%rax)
		movq $1, 24(%rax)
		movq $1, 32(%rax)
		popq %rbx
		popq %rbx
		addq $8, %rax
# INSTRUCTION_CONST
		mov $0, %rsi
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rcx, (%rax, %rsi,1)
# INSTRUCTION_COPY
		mov %rax, %rdi
# INSTRUCTION_CONST
		mov $8, %r8
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rax, (%rdi, %r8,1)
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r9
# INSTRUCTION_CONST
		mov $24, %r10
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rdi, (%r9, %r10,1)
# COMPLEX_RIP_LAMBDA_LOAD
		leaq lambda_2(%rip), %r11
# INSTRUCTION_CONST
		mov $1, %r12
# COMPLEX_ALLOCATE
		movq $8, %r13
		imulq %r12, %r13
# ALLOC_LAMBDA
		addq $32, %r13
		pushq %r13
		pushq %rbp
		call garbageCollectAllocate
		movq $2, 0(%rax)
		movq $1, 24(%rax)
		movq $1, 32(%rax)
		popq %r13
		popq %r13
		addq $8, %rax
# INSTRUCTION_CONST
		mov $0, %r14
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r11, (%rax, %r14,1)
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
		mov $32, %rbx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r15, (%rdx, %rbx,1)
# COMPLEX_RIP_LAMBDA_LOAD
		leaq lambda_3(%rip), %rsi
# INSTRUCTION_CONST
		mov $1, %rdi
# COMPLEX_ALLOCATE
		movq $8, %r8
		imulq %rdi, %r8
# ALLOC_LAMBDA
		addq $32, %r8
		pushq %r8
		pushq %rbp
		call garbageCollectAllocate
		movq $2, 0(%rax)
		movq $1, 24(%rax)
		movq $1, 32(%rax)
		popq %r8
		popq %r8
		addq $8, %rax
# INSTRUCTION_CONST
		mov $0, %r9
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rsi, (%rax, %r9,1)
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
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r12
# INSTRUCTION_CONST
		mov $40, %r13
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r10, (%r12, %r13,1)
# COMPLEX_RIP_LAMBDA_LOAD
		leaq lambda_4(%rip), %r14
# INSTRUCTION_CONST
		mov $1, %r15
# COMPLEX_ALLOCATE
		movq $8, %rcx
		imulq %r15, %rcx
# ALLOC_LAMBDA
		addq $32, %rcx
		pushq %rcx
		pushq %rbp
		call garbageCollectAllocate
		movq $2, 0(%rax)
		movq $1, 24(%rax)
		movq $1, 32(%rax)
		popq %rcx
		popq %rcx
		addq $8, %rax
# INSTRUCTION_CONST
		mov $0, %rdx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r14, (%rax, %rdx,1)
# INSTRUCTION_COPY
		mov %rax, %rbx
# INSTRUCTION_CONST
		mov $8, %rsi
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rax, (%rbx, %rsi,1)
# INSTRUCTION_PUSH
		push %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rdi
# INSTRUCTION_CONST
		mov $48, %r8
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rbx, (%rdi, %r8,1)
# INSTRUCTION_POP
		pop %rax
# INSTRUCTION_PUSH
		push %rax
# INSTRUCTION_CONST
		mov $30, %r9
# INSTRUCTION_PUSH
		push %r9
# INSTRUCTION_PUSH
		push %rax
# INSTRUCTION_FUNCTION_CALL
		call String_constructor
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# INSTRUCTION_POP
		pop %rax
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r10
# INSTRUCTION_CONST
		mov $48, %r11
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r10, %r11,1), %r10
# COMPLEX_SAVE_ALL
# INSTRUCTION_CONST
		mov $104, %r12
# INSTRUCTION_PUSH
		push %r12
# INSTRUCTION_CONST
		mov $8, %r13
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r10, %r13,1), %r10
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %r10
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r14
# INSTRUCTION_CONST
		mov $48, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r14, %r15,1), %r14
# INSTRUCTION_CONST
		mov $0, %rcx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r14, %rcx,1), %r14
# INSTRUCTION_REGISTER_CALL
		call *%r14
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rdx
movq %rbp, 0(%rdx)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rbx
# INSTRUCTION_CONST
		mov $48, %rsi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rsi,1), %rbx
# COMPLEX_SAVE_ALL
# INSTRUCTION_CONST
		mov $101, %rdi
# INSTRUCTION_PUSH
		push %rdi
# INSTRUCTION_CONST
		mov $8, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %r8,1), %rbx
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r9
# INSTRUCTION_CONST
		mov $48, %r10
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r9, %r10,1), %r9
# INSTRUCTION_CONST
		mov $0, %r11
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r9, %r11,1), %r9
# INSTRUCTION_REGISTER_CALL
		call *%r9
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %r12
movq %rbp, 0(%r12)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r13
# INSTRUCTION_CONST
		mov $48, %r14
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r13, %r14,1), %r13
# COMPLEX_SAVE_ALL
# INSTRUCTION_CONST
		mov $108, %r15
# INSTRUCTION_PUSH
		push %r15
# INSTRUCTION_CONST
		mov $8, %rcx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r13, %rcx,1), %r13
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %r13
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rdx
# INSTRUCTION_CONST
		mov $48, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdx, %rbx,1), %rdx
# INSTRUCTION_CONST
		mov $0, %rsi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdx, %rsi,1), %rdx
# INSTRUCTION_REGISTER_CALL
		call *%rdx
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rdi
movq %rbp, 0(%rdi)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r8
# INSTRUCTION_CONST
		mov $48, %r9
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r8, %r9,1), %r8
# COMPLEX_SAVE_ALL
# INSTRUCTION_CONST
		mov $108, %r10
# INSTRUCTION_PUSH
		push %r10
# INSTRUCTION_CONST
		mov $8, %r11
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r8, %r11,1), %r8
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %r8
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r12
# INSTRUCTION_CONST
		mov $48, %r13
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r12, %r13,1), %r12
# INSTRUCTION_CONST
		mov $0, %r14
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r12, %r14,1), %r12
# INSTRUCTION_REGISTER_CALL
		call *%r12
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %r15
movq %rbp, 0(%r15)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rcx
# INSTRUCTION_CONST
		mov $48, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rcx
# COMPLEX_SAVE_ALL
# INSTRUCTION_CONST
		mov $111, %rbx
# INSTRUCTION_PUSH
		push %rbx
# INSTRUCTION_CONST
		mov $8, %rsi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rsi,1), %rcx
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rdi
# INSTRUCTION_CONST
		mov $48, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %r8,1), %rdi
# INSTRUCTION_CONST
		mov $0, %r9
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %r9,1), %rdi
# INSTRUCTION_REGISTER_CALL
		call *%rdi
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %r10
movq %rbp, 0(%r10)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r11
# INSTRUCTION_CONST
		mov $48, %r12
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r11, %r12,1), %r11
# COMPLEX_SAVE_ALL
# INSTRUCTION_CONST
		mov $32, %r13
# INSTRUCTION_PUSH
		push %r13
# INSTRUCTION_CONST
		mov $8, %r14
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r11, %r14,1), %r11
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %r11
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r15
# INSTRUCTION_CONST
		mov $48, %rcx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r15, %rcx,1), %r15
# INSTRUCTION_CONST
		mov $0, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r15, %rdx,1), %r15
# INSTRUCTION_REGISTER_CALL
		call *%r15
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rbx
movq %rbp, 0(%rbx)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rsi
# INSTRUCTION_CONST
		mov $48, %rdi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rsi, %rdi,1), %rsi
# COMPLEX_SAVE_ALL
# INSTRUCTION_CONST
		mov $119, %r8
# INSTRUCTION_PUSH
		push %r8
# INSTRUCTION_CONST
		mov $8, %r9
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rsi, %r9,1), %rsi
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %rsi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r10
# INSTRUCTION_CONST
		mov $48, %r11
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r10, %r11,1), %r10
# INSTRUCTION_CONST
		mov $0, %r12
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r10, %r12,1), %r10
# INSTRUCTION_REGISTER_CALL
		call *%r10
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %r13
movq %rbp, 0(%r13)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r14
# INSTRUCTION_CONST
		mov $48, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r14, %r15,1), %r14
# COMPLEX_SAVE_ALL
# INSTRUCTION_CONST
		mov $111, %rcx
# INSTRUCTION_PUSH
		push %rcx
# INSTRUCTION_CONST
		mov $8, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r14, %rdx,1), %r14
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %r14
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rbx
# INSTRUCTION_CONST
		mov $48, %rsi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rsi,1), %rbx
# INSTRUCTION_CONST
		mov $0, %rdi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rdi,1), %rbx
# INSTRUCTION_REGISTER_CALL
		call *%rbx
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %r8
movq %rbp, 0(%r8)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r9
# INSTRUCTION_CONST
		mov $48, %r10
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r9, %r10,1), %r9
# COMPLEX_SAVE_ALL
# INSTRUCTION_CONST
		mov $114, %r11
# INSTRUCTION_PUSH
		push %r11
# INSTRUCTION_CONST
		mov $8, %r12
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r9, %r12,1), %r9
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %r9
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r13
# INSTRUCTION_CONST
		mov $48, %r14
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r13, %r14,1), %r13
# INSTRUCTION_CONST
		mov $0, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r13, %r15,1), %r13
# INSTRUCTION_REGISTER_CALL
		call *%r13
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rcx
movq %rbp, 0(%rcx)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rdx
# INSTRUCTION_CONST
		mov $48, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdx, %rbx,1), %rdx
# COMPLEX_SAVE_ALL
# INSTRUCTION_CONST
		mov $108, %rsi
# INSTRUCTION_PUSH
		push %rsi
# INSTRUCTION_CONST
		mov $8, %rdi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdx, %rdi,1), %rdx
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r8
# INSTRUCTION_CONST
		mov $48, %r9
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r8, %r9,1), %r8
# INSTRUCTION_CONST
		mov $0, %r10
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r8, %r10,1), %r8
# INSTRUCTION_REGISTER_CALL
		call *%r8
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %r11
movq %rbp, 0(%r11)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r12
# INSTRUCTION_CONST
		mov $48, %r13
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r12, %r13,1), %r12
# COMPLEX_SAVE_ALL
# INSTRUCTION_CONST
		mov $100, %r14
# INSTRUCTION_PUSH
		push %r14
# INSTRUCTION_CONST
		mov $8, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r12, %r15,1), %r12
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %r12
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rcx
# INSTRUCTION_CONST
		mov $48, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rcx
# INSTRUCTION_CONST
		mov $0, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rbx,1), %rcx
# INSTRUCTION_REGISTER_CALL
		call *%rcx
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rsi
movq %rbp, 0(%rsi)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rdi
# INSTRUCTION_CONST
		mov $48, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %r8,1), %rdi
# COMPLEX_SAVE_ALL
# INSTRUCTION_CONST
		mov $33, %r9
# INSTRUCTION_PUSH
		push %r9
# INSTRUCTION_CONST
		mov $8, %r10
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %r10,1), %rdi
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %rdi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r11
# INSTRUCTION_CONST
		mov $48, %r12
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r11, %r12,1), %r11
# INSTRUCTION_CONST
		mov $0, %r13
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r11, %r13,1), %r11
# INSTRUCTION_REGISTER_CALL
		call *%r11
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %r14
movq %rbp, 0(%r14)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r15
# INSTRUCTION_CONST
		mov $40, %rcx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r15, %rcx,1), %r15
# COMPLEX_SAVE_ALL
# INSTRUCTION_CONST
		mov $8, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r15, %rdx,1), %r15
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %r15
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rbx
# INSTRUCTION_CONST
		mov $40, %rsi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rsi,1), %rbx
# INSTRUCTION_CONST
		mov $0, %rdi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rdi,1), %rbx
# INSTRUCTION_REGISTER_CALL
		call *%rbx
# INSTRUCTION_ADD_STACK_PTR
		addq $8, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %r8
movq %rbp, 0(%r8)
# INSTRUCTION_WRITE_NL
		movq $nlprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r9
# INSTRUCTION_CONST
		mov $40, %r10
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r9, %r10,1), %r9
# COMPLEX_SAVE_ALL
# INSTRUCTION_CONST
		mov $8, %r11
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r9, %r11,1), %r9
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %r9
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r12
# INSTRUCTION_CONST
		mov $40, %r13
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r12, %r13,1), %r12
# INSTRUCTION_CONST
		mov $0, %r14
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r12, %r14,1), %r12
# INSTRUCTION_REGISTER_CALL
		call *%r12
# INSTRUCTION_ADD_STACK_PTR
		addq $8, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %r15
movq %rbp, 0(%r15)
# INSTRUCTION_WRITE_NL
		movq $nlprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rcx
# INSTRUCTION_CONST
		mov $32, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rcx
# COMPLEX_SAVE_ALL
# COMPLEX_RIP_LAMBDA_LOAD
		leaq lambda_5(%rip), %rbx
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
		mov %rax, %r11
# INSTRUCTION_PUSH
		push %r11
# INSTRUCTION_CONST
		mov $8, %r12
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %r12,1), %rcx
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %r13
# INSTRUCTION_CONST
		mov $32, %r14
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r13, %r14,1), %r13
# INSTRUCTION_CONST
		mov $0, %r15
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r13, %r15,1), %r13
# INSTRUCTION_REGISTER_CALL
		call *%r13
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rcx
movq %rbp, 0(%rcx)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rdx
# INSTRUCTION_CONST
		mov $40, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdx, %rbx,1), %rdx
# COMPLEX_SAVE_ALL
# INSTRUCTION_CONST
		mov $8, %rsi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdx, %rsi,1), %rdx
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %rdi
# INSTRUCTION_CONST
		mov $40, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %r8,1), %rdi
# INSTRUCTION_CONST
		mov $0, %r9
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %r9,1), %rdi
# INSTRUCTION_REGISTER_CALL
		call *%rdi
# INSTRUCTION_ADD_STACK_PTR
		addq $8, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %r10
movq %rbp, 0(%r10)
# INSTRUCTION_WRITE_NL
		movq $nlprint, %rdi
		movq $0, %rax
		call printf
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
mov $60, %rax
mov $0, %rdi
syscall
# INSTRUCTION_FUNCTION_LABEL
.type String_constructor, @function
String_constructor:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
	subq $40, %rsp
	movq %rax, -8(%rbp)
	movq $0, -16(%rbp)
	leaq staticLink, %rcx
	movq %rbp, 8(%rcx)
# METADATA_FUNCTION_ARGUMENT
		mov 16(%rbp), %rdx
		mov %rdx, -32(%rbp)
# METADATA_FUNCTION_ARGUMENT
		mov 24(%rbp), %rdx
		mov %rdx, -24(%rbp)
# INSTRUCTION_CONST
		mov $0, %rbx
# METADATA_DEBUG_INFO
		# CLASS LOAD
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rsi
# INSTRUCTION_CONST
		mov $0, %rdi
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rbx, (%rsi, %rdi,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %r8
# COMPLEX_ALLOCATE
		movq $8, %r9
		imulq %r8, %r9
# ALLOC_ARR_OF_PRIM
		addq $16, %r9
		pushq %r9
		pushq %rbp
		call garbageCollectAllocate
		movq %r8, 0(%rax)
		subq $8, %r9
		movq $0, (%rax, %r9, 1)
		popq %r9
		popq %r9
		addq $8, %rax
# METADATA_DEBUG_INFO
		# CLASS LOAD
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %r10
# INSTRUCTION_CONST
		mov $8, %r11
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rax, (%r10, %r11,1)
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
# INSTRUCTION_FUNCTION_LABEL
.type lambda_0, @function
lambda_0:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
	subq $40, %rsp
	movq %rax, -8(%rbp)
	movq $0, -16(%rbp)
	leaq staticLink, %r11
	movq %rbp, 16(%r11)
# METADATA_FUNCTION_ARGUMENT
		mov 16(%rbp), %r12
		mov %r12, -24(%rbp)
# METADATA_DEBUG_INFO
		# CLASS LOAD
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %r13
# INSTRUCTION_CONST
		mov $0, %r14
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r13, %r14,1), %r13
# INSTRUCTION_RETURN
		mov %r13, %rax
		mov %rbp,%rsp
pop %rbp
ret
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
	subq $48, %rsp
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
# METADATA_DEBUG_INFO
		# CLASS LOAD
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %r10
# INSTRUCTION_CONST
		mov $8, %r11
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r10, %r11,1), %r10
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %r12
# INSTRUCTION_CONST
		mov $8, %r13
# INSTRUCTION_MUL
		imul %r13, %r12
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r10, %r12,1), %r10
# INSTRUCTION_RETURN
		mov %r10, %rax
		mov %rbp,%rsp
pop %rbp
ret
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
	subq $56, %rsp
	movq %rax, -8(%rbp)
	movq $0, -16(%rbp)
	leaq staticLink, %rbx
	movq %rbp, 16(%rbx)
# METADATA_FUNCTION_ARGUMENT
		mov 16(%rbp), %rsi
		mov %rsi, -32(%rbp)
# METADATA_FUNCTION_ARGUMENT
		mov 24(%rbp), %rsi
		mov %rsi, -24(%rbp)
# VAR i
# INSTRUCTION_CONST
		mov $0, %rdi
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdi, -40(%rbp)
# INSTRUCTION_LABEL
		while_cnd_0:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r8
# METADATA_DEBUG_INFO
		# CLASS LOAD
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %r9
# INSTRUCTION_CONST
		mov $0, %r10
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r9, %r10,1), %r9
# INSTRUCTION_MINUS
		sub %r8, %r9
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rdx
		cmp $0, %r9
		setg %dl
		movsx %dl, %r9
		pop %rdx
# INSTRUCTION_CONST
		mov $1, %r11
# INSTRUCTION_CMP
		cmp %r9, %r11
# INSTRUCTION_JE
		je while_0_begin
# INSTRUCTION_JMP
		jmp while_0_end
# INSTRUCTION_LABEL
		while_0_begin:
# COMPLEX_SAVE_ALL
# METADATA_DEBUG_INFO
		# CLASS LOAD
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %r12
# INSTRUCTION_CONST
		mov $8, %r13
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r12, %r13,1), %r12
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r14
# INSTRUCTION_CONST
		mov $8, %r15
# INSTRUCTION_MUL
		imul %r15, %r14
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r12, %r14,1), %r12
# INSTRUCTION_PUSH
		push %r12
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rcx
# INSTRUCTION_CONST
		mov $8, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rcx, %rdx,1), %rcx
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rbx
# INSTRUCTION_CONST
		mov $0, %rsi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rsi,1), %rbx
# INSTRUCTION_REGISTER_CALL
		call *%rbx
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rdi
movq %rbp, 16(%rdi)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -40(%rbp), %r8
# INSTRUCTION_CONST
		mov $1, %r9
# INSTRUCTION_ADD
		add %r9, %r8
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %r8, -40(%rbp)
# INSTRUCTION_JMP
		jmp while_cnd_0
# INSTRUCTION_LABEL
		while_0_end:
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
# INSTRUCTION_FUNCTION_LABEL
.type lambda_3, @function
lambda_3:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
	subq $48, %rsp
	movq %rax, -8(%rbp)
	movq $0, -16(%rbp)
	leaq staticLink, %r15
	movq %rbp, 16(%r15)
# METADATA_FUNCTION_ARGUMENT
		mov 16(%rbp), %rcx
		mov %rcx, -24(%rbp)
# VAR i
# INSTRUCTION_CONST
		mov $0, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -32(%rbp)
# INSTRUCTION_LABEL
		while_cnd_1:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rbx
# METADATA_DEBUG_INFO
		# CLASS LOAD
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %rsi
# INSTRUCTION_CONST
		mov $0, %rdi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rsi, %rdi,1), %rsi
# INSTRUCTION_MINUS
		sub %rbx, %rsi
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rdx
		cmp $0, %rsi
		setg %dl
		movsx %dl, %rsi
		pop %rdx
# INSTRUCTION_CONST
		mov $1, %r8
# INSTRUCTION_CMP
		cmp %rsi, %r8
# INSTRUCTION_JE
		je while_1_begin
# INSTRUCTION_JMP
		jmp while_1_end
# INSTRUCTION_LABEL
		while_1_begin:
# METADATA_DEBUG_INFO
		# CLASS LOAD
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %r9
# INSTRUCTION_CONST
		mov $8, %r10
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r9, %r10,1), %r9
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %r11
# INSTRUCTION_CONST
		mov $8, %r12
# INSTRUCTION_MUL
		imul %r12, %r11
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r9, %r11,1), %r9
# INSTRUCTION_WRITE_CHAR
		movq %r9, %rsi
		movq $charprint, %rdi
		movq $0, %rax
		call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %r13
# INSTRUCTION_CONST
		mov $1, %r14
# INSTRUCTION_ADD
		add %r14, %r13
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %r13, -32(%rbp)
# INSTRUCTION_JMP
		jmp while_cnd_1
# INSTRUCTION_LABEL
		while_1_end:
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
# INSTRUCTION_FUNCTION_LABEL
.type lambda_4, @function
lambda_4:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
	subq $48, %rsp
	movq %rax, -8(%rbp)
	movq $0, -16(%rbp)
	leaq staticLink, %r12
	movq %rbp, 16(%r12)
# METADATA_FUNCTION_ARGUMENT
		mov 16(%rbp), %r13
		mov %r13, -32(%rbp)
# METADATA_FUNCTION_ARGUMENT
		mov 24(%rbp), %r13
		mov %r13, -24(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %r14
# METADATA_DEBUG_INFO
		# CLASS LOAD
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %r15
# INSTRUCTION_CONST
		mov $8, %rcx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r15, %rcx,1), %r15
# METADATA_DEBUG_INFO
		# CLASS LOAD
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rdx
# INSTRUCTION_CONST
		mov $0, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdx, %rbx,1), %rdx
# INSTRUCTION_CONST
		mov $8, %rsi
# INSTRUCTION_MUL
		imul %rsi, %rdx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r14, (%r15, %rdx,1)
# METADATA_DEBUG_INFO
		# CLASS LOAD
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %rdi
# INSTRUCTION_CONST
		mov $0, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %r8,1), %rdi
# INSTRUCTION_CONST
		mov $1, %r9
# INSTRUCTION_ADD
		add %r9, %rdi
# METADATA_DEBUG_INFO
		# CLASS LOAD
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -32(%rbp), %r10
# INSTRUCTION_CONST
		mov $0, %r11
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rdi, (%r10, %r11,1)
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
# INSTRUCTION_FUNCTION_LABEL
.type lambda_5, @function
lambda_5:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
	subq $48, %rsp
	movq %rax, -8(%rbp)
	movq $0, -16(%rbp)
	leaq staticLink, %r9
	movq %rbp, 8(%r9)
# METADATA_FUNCTION_ARGUMENT
		mov 16(%rbp), %r10
		mov %r10, -32(%rbp)
# METADATA_FUNCTION_ARGUMENT
		mov 24(%rbp), %r10
		mov %r10, -24(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %r12
		mov 0(%r12), %r12
		movq -16(%r12), %r13
		addq $1, %r13
		imul $-1, %r13
		mov -16(%r12, %r13, 8), %r11
# INSTRUCTION_CONST
		mov $48, %r14
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r11, %r14,1), %r11
# COMPLEX_SAVE_ALL
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -24(%rbp), %r15
# INSTRUCTION_PUSH
		push %r15
# INSTRUCTION_CONST
		mov $8, %rcx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%r11, %rcx,1), %r11
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# INSTRUCTION_PUSH
		push %r11
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
		leaq staticLink, %rbx
		mov 0(%rbx), %rbx
		movq -16(%rbx), %rsi
		addq $1, %rsi
		imul $-1, %rsi
		mov -16(%rbx, %rsi, 8), %rdx
# INSTRUCTION_CONST
		mov $48, %rdi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdx, %rdi,1), %rdx
# INSTRUCTION_CONST
		mov $0, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdx, %r8,1), %rdx
# INSTRUCTION_REGISTER_CALL
		call *%rdx
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# COMPLEX_RESTORE_ALL
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %r9
movq %rbp, 8(%r9)
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
