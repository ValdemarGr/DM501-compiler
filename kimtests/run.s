.section .data
staticLink:
	.space 8
intprint:
	.asciz "%i\n"
charprint:
	.asciz "%c"
nlprint:
	.asciz "\n"
outOfBoundsMsg:
    .asciz "Error, out of bounds\n"
divZeroMsg:
    .asciz "Error, division by zero\n"
negAllocMsg:
    .asciz "Error, negative allocation\n"
nullPtrMsg:
    .asciz "Error, nullpointer dereferencec\n"
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

.type nullPtrCheck, @function
nullPtrCheck:
    push %rbp
    movq %rsp, %rbp

    cmp $0, 16(%rbp)
    je nullPtrCheckKill
    # else
    jmp nullPtrCheckReturn
    nullPtrCheckKill:
    movq $nullPtrMsg, %rdi
    movq $0, %rax
    call printf

    movq $60, %rax
    movq $44, %rdi
    syscall
    nullPtrCheckReturn:

    movq %rbp,%rsp
    pop %rbp
    ret


.type negAllocCheck, @function
negAllocCheck:
    push %rbp
    movq %rsp, %rbp

    cmp $0, 16(%rbp)
    jl negAllocCheckKill
    # else
    jmp negAllocCheckReturn
    negAllocCheckKill:
    movq $negAllocMsg, %rdi
    movq $0, %rax
    call printf

    movq $60, %rax
    movq $66, %rdi
    syscall
    negAllocCheckReturn:

    movq %rbp,%rsp
    pop %rbp
    ret


.type divZeroCheck, @function
divZeroCheck:
    push %rbp
    movq %rsp, %rbp

    cmp $0, 16(%rbp)
    jle divZeroCheckKill
    # else
    jmp divZeroCheckReturn
    divZeroCheckKill:
    movq $divZeroMsg, %rdi
    movq $0, %rax
    call printf

    movq $60, %rax
    movq $42, %rdi
    syscall
    divZeroCheckReturn:

    movq %rbp,%rsp
    pop %rbp
    ret


.type outOfBoundsCheck, @function
outOfBoundsCheck:
    push %rbp
    movq %rsp, %rbp

    pushq %rax

    # 16 has arr
    movq 16(%rbp), %rax
    subq $8, %rax
    movq (%rax), %rax

    # 24 has index
    # index must be larger than -1 and excplicitly less than %rax
    # if index < 0 KILL
    cmp $0, 24(%rbp)
    jl outOfBoundsCheckKill
    # if size <= index KILL
    cmp 24(%rbp), %rax
    jle outOfBoundsCheckKill
    # else
    jmp outOfBoundsCheckReturn
    outOfBoundsCheckKill:
    movq $outOfBoundsMsg, %rdi
    movq $0, %rax
    call printf

    movq $60, %rax
    movq $2, %rdi
    syscall
    outOfBoundsCheckReturn:

    popq %rax

    movq %rbp,%rsp
    pop %rbp
    ret

# METADATA_BEGIN_BODY_BLOCK
# VAR sum
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
    movq $10485760, 16(%r15)
    movq $0, 8(%r15)
    movq $1, 0(%r15)
    addq $10485760, %rax
    push %rax
    movq %rax, %rdi
    mov $12, %rax
    syscall
    pop %rax
    #top should be in rax

    #move heap into one

    leaq gcHeapTwo, %r15
    movq %rax, 24(%r15)
    movq $10485760, 16(%r15)
    movq $0, 8(%r15)
    movq $0, 0(%r15)

    addq $10485760, %rax
    movq %rax, %rdi
    mov $12, %rax
    syscall
	subq $96, %rsp
	popq %rax
	movq %rax, -8(%rbp)
	movq $1, -16(%rbp)
	movq $2, -24(%rbp)
	movq $0, -48(%rbp)
	leaq staticLink, %rax
	movq %rbp, (%rax)
# METADATA_DEBUG_INFO
		# 0: {0, 1}
# INSTRUCTION_CONST
		mov $5, %rbx
# METADATA_DEBUG_INFO
		# 1: {0, 1}
# RUNTIME_NEGATIVE_ALLOC
		pushq %rbx
		call negAllocCheck
		addq $8, %rsp
# METADATA_DEBUG_INFO
		# 2: {0, 1, 2}
# COMPLEX_ALLOCATE
		movq $8, %rdx
		imulq %rbx, %rdx
# ALLOC_ARR_OF_PTR
		addq $8, %rdx
		pushq %rdx
		pushq %rbp
		call garbageCollectAllocate
		movq %rbx, 0(%rax)
# ALLOC_ARR_OF_PTR
		subq $0, %rdx
		movq $-1, (%rax, %rdx, 1)
		popq %rdx
		popq %rdx
		addq $8, %rax
# METADATA_DEBUG_INFO
		# 3: {0}
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -48(%rbp)
# INSTRUCTION_SET_ZERO
		xorq %rdx, %rdx
# METADATA_DEBUG_INFO
		# 5: {0, 3}
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -56(%rbp)
# INSTRUCTION_LABEL
		while_cnd_0:
# METADATA_DEBUG_INFO
		# 6: {0, 4}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rsi
# METADATA_DEBUG_INFO
		# 7: {0, 4, 5}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rbx
# METADATA_DEBUG_INFO
		# 8: {0, 4, 5, 6}
# INSTRUCTION_CONST
		mov $-8, %rdx
# METADATA_DEBUG_INFO
		# 9: {0, 4, 5, 6}
# RUNTIME_NULLPTR_CHECK
		pushq %rbx
		call nullPtrCheck
		addq $8, %rsp
# METADATA_DEBUG_INFO
		# 10: {0, 4, 5, 6}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rdx,1), %rbx
# METADATA_DEBUG_INFO
		# 11: {0, 4, 5}
# INSTRUCTION_MINUS
		sub %rsi, %rbx
# METADATA_DEBUG_INFO
		# 12: {0, 5}
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rax
		cmp $0, %rbx
		setg %al
		movsx %al, %rbx
		pop %rax
# METADATA_DEBUG_INFO
		# 13: {0, 5, 7}
# INSTRUCTION_CONST
		mov $1, %rdx
# METADATA_DEBUG_INFO
		# 14: {0, 5, 7}
# INSTRUCTION_CMP
		cmp %rbx, %rdx
# METADATA_DEBUG_INFO
		# 15: {0}
# INSTRUCTION_JE
		je while_0_begin
# METADATA_DEBUG_INFO
		# 16: {}
# INSTRUCTION_JMP
		jmp while_0_end
# INSTRUCTION_LABEL
		while_0_begin:
# METADATA_DEBUG_INFO
		# 17: {0, 8}
# INSTRUCTION_CONST
		mov $5, %rbx
# METADATA_DEBUG_INFO
		# 18: {0, 8}
# RUNTIME_NEGATIVE_ALLOC
		pushq %rbx
		call negAllocCheck
		addq $8, %rsp
# METADATA_DEBUG_INFO
		# 19: {0, 8, 9}
# COMPLEX_ALLOCATE
		movq $8, %rdx
		imulq %rbx, %rdx
# ALLOC_ARR_OF_PRIM
		addq $16, %rdx
		pushq %rdx
		pushq %rbp
		call garbageCollectAllocate
		movq %rbx, 0(%rax)
		subq $8, %rdx
		movq $0, (%rax, %rdx, 1)
		popq %rdx
		popq %rdx
		addq $8, %rax
# METADATA_DEBUG_INFO
		# 20: {0, 10}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rbx
# METADATA_DEBUG_INFO
		# 21: {0, 10}
# RUNTIME_NULLPTR_CHECK
		pushq %rbx
		call nullPtrCheck
		addq $8, %rsp
# METADATA_DEBUG_INFO
		# 22: {0, 10, 11}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rdx
# METADATA_DEBUG_INFO
		# 23: {0, 10, 11}
# RUNTIME_ARRAY_BOUNDS_CHECK
		pushq %rdx
		pushq %rbx
		call outOfBoundsCheck
		addq $16, %rsp
# INSTRUCTION_MUL_CONST
		imul $8, %rdx
# METADATA_DEBUG_INFO
		# 25: {0, 10, 11}
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rax, (%rbx, %rdx,1)
# INSTRUCTION_SET_ZERO
		xorq %rdx, %rdx
# METADATA_DEBUG_INFO
		# 27: {0, 13}
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -64(%rbp)
# INSTRUCTION_LABEL
		while_cnd_1:
# METADATA_DEBUG_INFO
		# 28: {0, 14}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -64(%rbp), %rsi
# METADATA_DEBUG_INFO
		# 29: {0, 14, 15}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rbx
# METADATA_DEBUG_INFO
		# 30: {0, 14, 15}
# RUNTIME_NULLPTR_CHECK
		pushq %rbx
		call nullPtrCheck
		addq $8, %rsp
# METADATA_DEBUG_INFO
		# 31: {0, 14, 15, 16}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rdx
# METADATA_DEBUG_INFO
		# 32: {0, 14, 15, 16}
# RUNTIME_ARRAY_BOUNDS_CHECK
		pushq %rdx
		pushq %rbx
		call outOfBoundsCheck
		addq $16, %rsp
# INSTRUCTION_MUL_CONST
		imul $8, %rdx
# METADATA_DEBUG_INFO
		# 34: {0, 14, 15, 16}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rdx,1), %rbx
# METADATA_DEBUG_INFO
		# 35: {0, 14, 15, 18}
# INSTRUCTION_CONST
		mov $-8, %rdx
# METADATA_DEBUG_INFO
		# 36: {0, 14, 15, 18}
# RUNTIME_NULLPTR_CHECK
		pushq %rbx
		call nullPtrCheck
		addq $8, %rsp
# METADATA_DEBUG_INFO
		# 37: {0, 14, 15, 18}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rdx,1), %rbx
# METADATA_DEBUG_INFO
		# 38: {0, 14, 15}
# INSTRUCTION_MINUS
		sub %rsi, %rbx
# METADATA_DEBUG_INFO
		# 39: {0, 15}
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rax
		cmp $0, %rbx
		setg %al
		movsx %al, %rbx
		pop %rax
# METADATA_DEBUG_INFO
		# 40: {0, 15, 19}
# INSTRUCTION_CONST
		mov $1, %rdx
# METADATA_DEBUG_INFO
		# 41: {0, 15, 19}
# INSTRUCTION_CMP
		cmp %rbx, %rdx
# METADATA_DEBUG_INFO
		# 42: {0}
# INSTRUCTION_JE
		je while_1_begin
# METADATA_DEBUG_INFO
		# 43: {0}
# INSTRUCTION_JMP
		jmp while_1_end
# INSTRUCTION_LABEL
		while_1_begin:
# METADATA_DEBUG_INFO
		# 44: {0, 20}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rsi
# METADATA_DEBUG_INFO
		# 45: {0, 20, 21}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -64(%rbp), %rdx
# METADATA_DEBUG_INFO
		# 46: {0, 20, 21}
# INSTRUCTION_ADD
		add %rdx, %rsi
# METADATA_DEBUG_INFO
		# 47: {0, 20, 22}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rbx
# METADATA_DEBUG_INFO
		# 48: {0, 20, 22}
# RUNTIME_NULLPTR_CHECK
		pushq %rbx
		call nullPtrCheck
		addq $8, %rsp
# METADATA_DEBUG_INFO
		# 49: {0, 20, 22, 23}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rdx
# METADATA_DEBUG_INFO
		# 50: {0, 20, 22, 23}
# RUNTIME_ARRAY_BOUNDS_CHECK
		pushq %rdx
		pushq %rbx
		call outOfBoundsCheck
		addq $16, %rsp
# INSTRUCTION_MUL_CONST
		imul $8, %rdx
# METADATA_DEBUG_INFO
		# 52: {0, 20, 22, 23}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rdx,1), %rbx
# METADATA_DEBUG_INFO
		# 53: {0, 20, 22}
# RUNTIME_NULLPTR_CHECK
		pushq %rbx
		call nullPtrCheck
		addq $8, %rsp
# METADATA_DEBUG_INFO
		# 54: {0, 20, 22, 25}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -64(%rbp), %rdx
# METADATA_DEBUG_INFO
		# 55: {0, 20, 22, 25}
# RUNTIME_ARRAY_BOUNDS_CHECK
		pushq %rdx
		pushq %rbx
		call outOfBoundsCheck
		addq $16, %rsp
# INSTRUCTION_MUL_CONST
		imul $8, %rdx
# METADATA_DEBUG_INFO
		# 57: {0, 20, 22, 25}
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rsi, (%rbx, %rdx,1)
# METADATA_DEBUG_INFO
		# 58: {0, 27}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -64(%rbp), %rdx
# INSTRUCTION_ADD_CONST
		add $1, %rdx
# METADATA_DEBUG_INFO
		# 60: {0, 27}
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -64(%rbp)
# METADATA_DEBUG_INFO
		# 61: {0}
# INSTRUCTION_JMP
		jmp while_cnd_1
# INSTRUCTION_LABEL
		while_1_end:
# METADATA_DEBUG_INFO
		# 62: {0, 29}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rdx
# INSTRUCTION_ADD_CONST
		add $1, %rdx
# METADATA_DEBUG_INFO
		# 64: {0, 29}
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -56(%rbp)
# METADATA_DEBUG_INFO
		# 65: {0}
# INSTRUCTION_JMP
		jmp while_cnd_0
# INSTRUCTION_LABEL
		while_0_end:
# INSTRUCTION_SET_ZERO
		xorq %rdx, %rdx
# METADATA_DEBUG_INFO
		# 67: {31}
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -72(%rbp)
# INSTRUCTION_SET_ZERO
		xorq %rdx, %rdx
# METADATA_DEBUG_INFO
		# 69: {32}
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -56(%rbp)
# INSTRUCTION_LABEL
		while_cnd_2:
# METADATA_DEBUG_INFO
		# 70: {33}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rsi
# METADATA_DEBUG_INFO
		# 71: {33, 34}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rbx
# METADATA_DEBUG_INFO
		# 72: {33, 34, 35}
# INSTRUCTION_CONST
		mov $-8, %rdx
# METADATA_DEBUG_INFO
		# 73: {33, 34, 35}
# RUNTIME_NULLPTR_CHECK
		pushq %rbx
		call nullPtrCheck
		addq $8, %rsp
# METADATA_DEBUG_INFO
		# 74: {33, 34, 35}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rdx,1), %rbx
# METADATA_DEBUG_INFO
		# 75: {33, 34}
# INSTRUCTION_MINUS
		sub %rsi, %rbx
# METADATA_DEBUG_INFO
		# 76: {34}
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rax
		cmp $0, %rbx
		setg %al
		movsx %al, %rbx
		pop %rax
# METADATA_DEBUG_INFO
		# 77: {34, 36}
# INSTRUCTION_CONST
		mov $1, %rdx
# METADATA_DEBUG_INFO
		# 78: {34, 36}
# INSTRUCTION_CMP
		cmp %rbx, %rdx
# METADATA_DEBUG_INFO
		# 79: {}
# INSTRUCTION_JE
		je while_2_begin
# METADATA_DEBUG_INFO
		# 80: {}
# INSTRUCTION_JMP
		jmp while_2_end
# INSTRUCTION_LABEL
		while_2_begin:
# INSTRUCTION_SET_ZERO
		xorq %rdx, %rdx
# METADATA_DEBUG_INFO
		# 82: {37}
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -64(%rbp)
# INSTRUCTION_LABEL
		while_cnd_3:
# METADATA_DEBUG_INFO
		# 83: {38}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -64(%rbp), %rsi
# METADATA_DEBUG_INFO
		# 84: {38, 39}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rbx
# METADATA_DEBUG_INFO
		# 85: {38, 39}
# RUNTIME_NULLPTR_CHECK
		pushq %rbx
		call nullPtrCheck
		addq $8, %rsp
# METADATA_DEBUG_INFO
		# 86: {38, 39, 40}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rdx
# METADATA_DEBUG_INFO
		# 87: {38, 39, 40}
# RUNTIME_ARRAY_BOUNDS_CHECK
		pushq %rdx
		pushq %rbx
		call outOfBoundsCheck
		addq $16, %rsp
# INSTRUCTION_MUL_CONST
		imul $8, %rdx
# METADATA_DEBUG_INFO
		# 89: {38, 39, 40}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rdx,1), %rbx
# METADATA_DEBUG_INFO
		# 90: {38, 39, 42}
# INSTRUCTION_CONST
		mov $-8, %rdx
# METADATA_DEBUG_INFO
		# 91: {38, 39, 42}
# RUNTIME_NULLPTR_CHECK
		pushq %rbx
		call nullPtrCheck
		addq $8, %rsp
# METADATA_DEBUG_INFO
		# 92: {38, 39, 42}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rdx,1), %rbx
# METADATA_DEBUG_INFO
		# 93: {38, 39}
# INSTRUCTION_MINUS
		sub %rsi, %rbx
# METADATA_DEBUG_INFO
		# 94: {39}
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rax
		cmp $0, %rbx
		setg %al
		movsx %al, %rbx
		pop %rax
# METADATA_DEBUG_INFO
		# 95: {39, 43}
# INSTRUCTION_CONST
		mov $1, %rdx
# METADATA_DEBUG_INFO
		# 96: {39, 43}
# INSTRUCTION_CMP
		cmp %rbx, %rdx
# METADATA_DEBUG_INFO
		# 97: {}
# INSTRUCTION_JE
		je while_3_begin
# METADATA_DEBUG_INFO
		# 98: {}
# INSTRUCTION_JMP
		jmp while_3_end
# INSTRUCTION_LABEL
		while_3_begin:
# METADATA_DEBUG_INFO
		# 99: {44}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -72(%rbp), %rsi
# METADATA_DEBUG_INFO
		# 100: {44, 45}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rbx
# METADATA_DEBUG_INFO
		# 101: {44, 45}
# RUNTIME_NULLPTR_CHECK
		pushq %rbx
		call nullPtrCheck
		addq $8, %rsp
# METADATA_DEBUG_INFO
		# 102: {44, 45, 46}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rdx
# METADATA_DEBUG_INFO
		# 103: {44, 45, 46}
# RUNTIME_ARRAY_BOUNDS_CHECK
		pushq %rdx
		pushq %rbx
		call outOfBoundsCheck
		addq $16, %rsp
# INSTRUCTION_MUL_CONST
		imul $8, %rdx
# METADATA_DEBUG_INFO
		# 105: {44, 45, 46}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rdx,1), %rbx
# METADATA_DEBUG_INFO
		# 106: {44, 45}
# RUNTIME_NULLPTR_CHECK
		pushq %rbx
		call nullPtrCheck
		addq $8, %rsp
# METADATA_DEBUG_INFO
		# 107: {44, 45, 48}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -64(%rbp), %rdx
# METADATA_DEBUG_INFO
		# 108: {44, 45, 48}
# RUNTIME_ARRAY_BOUNDS_CHECK
		pushq %rdx
		pushq %rbx
		call outOfBoundsCheck
		addq $16, %rsp
# INSTRUCTION_MUL_CONST
		imul $8, %rdx
# METADATA_DEBUG_INFO
		# 110: {44, 45, 48}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rdx,1), %rbx
# METADATA_DEBUG_INFO
		# 111: {44, 45}
# INSTRUCTION_ADD
		add %rbx, %rsi
# METADATA_DEBUG_INFO
		# 112: {44}
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rsi, -72(%rbp)
# METADATA_DEBUG_INFO
		# 113: {50}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -64(%rbp), %rdx
# INSTRUCTION_ADD_CONST
		add $1, %rdx
# METADATA_DEBUG_INFO
		# 115: {50}
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -64(%rbp)
# METADATA_DEBUG_INFO
		# 116: {}
# INSTRUCTION_JMP
		jmp while_cnd_3
# INSTRUCTION_LABEL
		while_3_end:
# METADATA_DEBUG_INFO
		# 117: {52}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rdx
# INSTRUCTION_ADD_CONST
		add $1, %rdx
# METADATA_DEBUG_INFO
		# 119: {52}
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -56(%rbp)
# METADATA_DEBUG_INFO
		# 120: {}
# INSTRUCTION_JMP
		jmp while_cnd_2
# INSTRUCTION_LABEL
		while_2_end:
# METADATA_DEBUG_INFO
		# 121: {54}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -72(%rbp), %rdx
# METADATA_DEBUG_INFO
		# 122: {54}
# INSTRUCTION_WRITE
		movq %rdx, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
movq $0, %rax
leave
ret