.section .data
staticLink:
	.space 24
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
    movq $5, %rdi
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
    movq $4, %rdi
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
    movq $3, %rdi
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
# VAR opt
# VAR boxedInteger
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
	subq $88, %rsp
	popq %rax
	movq %rax, -8(%rbp)
	movq $3, -16(%rbp)
	movq $0, -24(%rbp)
	movq $0, -48(%rbp)
	movq $1, -32(%rbp)
	movq $0, -56(%rbp)
	movq $2, -40(%rbp)
	movq $0, -64(%rbp)
	leaq staticLink, %rax
	movq %rbp, (%rax)
# VAR showwer
# METADATA_DEBUG_INFO
		# 122: {0}
# COMPLEX_SAVE_ALL
		pushq %rcx
		pushq %rdx
		pushq %rbx
		pushq %rsi
		pushq %rdi
		pushq %r8
		pushq %r9
		pushq %r10
		pushq %r11
		pushq %r12
		pushq %r13
		pushq %r14
		pushq %r15
# METADATA_DEBUG_INFO
		# 123: {0}
# INSTRUCTION_FUNCTION_CALL
		call a__0
# METADATA_DEBUG_INFO
		# 124: {0}
# INSTRUCTION_ADD_STACK_PTR
		addq $0, %rsp
# METADATA_DEBUG_INFO
		# 125: {0}
# COMPLEX_RESTORE_ALL
		popq %r15
		popq %r14
		popq %r13
		popq %r12
		popq %r11
		popq %r10
		popq %r9
		popq %r8
		popq %rdi
		popq %rsi
		popq %rbx
		popq %rdx
		popq %rcx
# METADATA_DEBUG_INFO
		# 126: {0, 11}
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rdx
movq %rbp, 0(%rdx)
# METADATA_DEBUG_INFO
		# 127: {0, 12}
# INSTRUCTION_COPY
		mov %rax, %rdx
# METADATA_DEBUG_INFO
		# 128: {0, 12}
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -48(%rbp)
# METADATA_DEBUG_INFO
		# ALLOC
# METADATA_DEBUG_INFO
		# boxedInteger
# METADATA_DEBUG_INFO
		# 129: {0, 13}
# INSTRUCTION_CONST
		mov $1, %rbx
# METADATA_DEBUG_INFO
		# 130: {0, 13, 14}
# COMPLEX_ALLOCATE
		movq $8, %rdx
		imulq %rbx, %rdx
# ALLOC_RECORD_CLASS
		addq $16, %rdx
		pushq %rdx
		pushq %rbp
		call garbageCollectAllocate
		movq %rbx, 0(%rax)
# ALLOC_RECORD_CLASS
		subq $8, %rdx
		movq $0, (%rax, %rdx, 1)
		popq %rdx
		popq %rdx
		addq $8, %rax
# METADATA_DEBUG_INFO
		# 131: {0}
# INSTRUCTION_PUSH
		push %rax
# METADATA_DEBUG_INFO
		# 132: {0}
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -64(%rbp)
# METADATA_DEBUG_INFO
		# 133: {15}
# INSTRUCTION_SET_ZERO
		xorq %rsi, %rsi
# METADATA_DEBUG_INFO
		# 134: {15, 16}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -64(%rbp), %rbx
# METADATA_DEBUG_INFO
		# 135: {15, 16}
# RUNTIME_NULLPTR_CHECK
		pushq %rbx
		call nullPtrCheck
		addq $8, %rsp
# METADATA_DEBUG_INFO
		# 136: {15, 16, 17}
# INSTRUCTION_SET_ZERO
		xorq %rdx, %rdx
# METADATA_DEBUG_INFO
		# 137: {15, 16, 17}
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rsi, (%rbx, %rdx,1)
# METADATA_DEBUG_INFO
		# 138: {0}
# INSTRUCTION_POP
		pop %rax
# METADATA_DEBUG_INFO
		# 139: {0}
# INSTRUCTION_PUSH
		push %rax
# METADATA_DEBUG_INFO
		# 140: {0, 18}
# INSTRUCTION_CONST
		mov $42, %rdi
# METADATA_DEBUG_INFO
		# 141: {0, 18}
# INSTRUCTION_PUSH
		push %rdi
# METADATA_DEBUG_INFO
		# 142: {0}
# INSTRUCTION_PUSH
		push %rax
# METADATA_DEBUG_INFO
		# 143: {0}
# INSTRUCTION_FUNCTION_CALL
		call IntBox_constructor
# METADATA_DEBUG_INFO
		# 144: {}
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# METADATA_DEBUG_INFO
		# 145: {0}
# INSTRUCTION_POP
		pop %rax
# METADATA_DEBUG_INFO
		# ALLOC
# METADATA_DEBUG_INFO
		# opt
# METADATA_DEBUG_INFO
		# 146: {0, 19}
# INSTRUCTION_CONST
		mov $3, %rbx
# METADATA_DEBUG_INFO
		# 147: {0, 19, 20}
# COMPLEX_ALLOCATE
		movq $8, %rdx
		imulq %rbx, %rdx
# ALLOC_RECORD_CLASS
		addq $32, %rdx
		pushq %rdx
		pushq %rbp
		call garbageCollectAllocate
		movq %rbx, 0(%rax)
# ALLOC_RECORD_CLASS
		subq $24, %rdx
		movq $2, (%rax, %rdx, 1)
		movq $0, 8(%rax, %rdx, 1)
		movq $2, 16(%rax, %rdx, 1)
		popq %rdx
		popq %rdx
		addq $8, %rax
# METADATA_DEBUG_INFO
		# 148: {0}
# INSTRUCTION_PUSH
		push %rax
# METADATA_DEBUG_INFO
		# 149: {0}
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -56(%rbp)
# METADATA_DEBUG_INFO
		# 150: {0, 21}
# INSTRUCTION_SET_ZERO
		xorq %rsi, %rsi
# METADATA_DEBUG_INFO
		# 151: {0, 21, 22}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rbx
# METADATA_DEBUG_INFO
		# 152: {0, 21, 22}
# RUNTIME_NULLPTR_CHECK
		pushq %rbx
		call nullPtrCheck
		addq $8, %rsp
# METADATA_DEBUG_INFO
		# 153: {0, 21, 22, 23}
# INSTRUCTION_CONST
		mov $8, %rdx
# METADATA_DEBUG_INFO
		# 154: {0, 21, 22, 23}
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rsi, (%rbx, %rdx,1)
# METADATA_DEBUG_INFO
		# 155: {0, 24}
# COMPLEX_RIP_LAMBDA_LOAD
		leaq lambda_0(%rip), %rsi
# METADATA_DEBUG_INFO
		# 156: {0, 24, 25}
# INSTRUCTION_CONST
		mov $1, %rdx
# METADATA_DEBUG_INFO
		# 157: {0, 24, 25, 26}
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
# METADATA_DEBUG_INFO
		# 158: {0, 24, 27}
# INSTRUCTION_SET_ZERO
		xorq %rdx, %rdx
# METADATA_DEBUG_INFO
		# 159: {0, 24, 27}
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rsi, (%rax, %rdx,1)
# METADATA_DEBUG_INFO
		# 160: {0, 28}
# INSTRUCTION_COPY
		mov %rax, %rsi
# METADATA_DEBUG_INFO
		# 161: {28, 29}
# INSTRUCTION_CONST
		mov $8, %rdx
# METADATA_DEBUG_INFO
		# 162: {0, 28, 29}
# INSTRUCTION_POP
		pop %rax
# METADATA_DEBUG_INFO
		# 163: {0, 28, 29}
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rax, (%rsi, %rdx,1)
# METADATA_DEBUG_INFO
		# 164: {0, 28}
# INSTRUCTION_PUSH
		push %rax
# METADATA_DEBUG_INFO
		# 165: {28, 30}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rbx
# METADATA_DEBUG_INFO
		# 166: {28, 30}
# RUNTIME_NULLPTR_CHECK
		pushq %rbx
		call nullPtrCheck
		addq $8, %rsp
# METADATA_DEBUG_INFO
		# 167: {28, 30, 31}
# INSTRUCTION_CONST
		mov $16, %rdx
# METADATA_DEBUG_INFO
		# 168: {28, 30, 31}
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rsi, (%rbx, %rdx,1)
# METADATA_DEBUG_INFO
		# 169: {0}
# INSTRUCTION_POP
		pop %rax
# METADATA_DEBUG_INFO
		# 170: {0}
# INSTRUCTION_PUSH
		push %rax
# METADATA_DEBUG_INFO
		# 171: {0, 32}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -64(%rbp), %rdx
# METADATA_DEBUG_INFO
		# 172: {0, 32}
# INSTRUCTION_PUSH
		push %rdx
# METADATA_DEBUG_INFO
		# 173: {0}
# INSTRUCTION_PUSH
		push %rax
# METADATA_DEBUG_INFO
		# 174: {0}
# INSTRUCTION_FUNCTION_CALL
		call Optional_constructor
# METADATA_DEBUG_INFO
		# 175: {}
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# METADATA_DEBUG_INFO
		# 176: {0}
# INSTRUCTION_POP
		pop %rax
# METADATA_DEBUG_INFO
		# 177: {0, 33}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rdi
# METADATA_DEBUG_INFO
		# 178: {0, 33}
# RUNTIME_NULLPTR_CHECK
		pushq %rdi
		call nullPtrCheck
		addq $8, %rsp
# METADATA_DEBUG_INFO
		# 179: {0, 33, 34}
# INSTRUCTION_CONST
		mov $16, %rdx
# METADATA_DEBUG_INFO
		# 180: {0, 33, 34}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %rdx, 1), %rdi
# METADATA_DEBUG_INFO
		# 181: {0, 33}
# RUNTIME_NULLPTR_CHECK
		pushq %rdi
		call nullPtrCheck
		addq $8, %rsp
# METADATA_DEBUG_INFO
		# 182: {0, 33}
# COMPLEX_SAVE_ALL
		pushq %rcx
		pushq %rdx
		pushq %rbx
		pushq %rsi
		pushq %rdi
		pushq %r8
		pushq %r9
		pushq %r10
		pushq %r11
		pushq %r12
		pushq %r13
		pushq %r14
		pushq %r15
# METADATA_DEBUG_INFO
		# 183: {0, 33, 35}
# COMPLEX_RIP_LAMBDA_LOAD
		leaq lambda_1(%rip), %rsi
# METADATA_DEBUG_INFO
		# 184: {0, 33, 35, 36}
# INSTRUCTION_CONST
		mov $1, %rbx
# METADATA_DEBUG_INFO
		# 185: {0, 33, 35, 36, 37}
# COMPLEX_ALLOCATE
		movq $8, %rdx
		imulq %rbx, %rdx
# ALLOC_LAMBDA
		addq $32, %rdx
		pushq %rdx
		pushq %rbp
		call garbageCollectAllocate
		movq $2, 0(%rax)
		movq $1, 24(%rax)
		movq $1, 32(%rax)
		popq %rdx
		popq %rdx
		addq $8, %rax
# METADATA_DEBUG_INFO
		# 186: {0, 33, 35, 38}
# INSTRUCTION_SET_ZERO
		xorq %rdx, %rdx
# METADATA_DEBUG_INFO
		# 187: {0, 33, 35, 38}
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rsi, (%rax, %rdx,1)
# METADATA_DEBUG_INFO
		# 196: {0, 33, 41}
# INSTRUCTION_COPY
		mov %rax, %rbx
# METADATA_DEBUG_INFO
		# 197: {0, 33, 41}
# INSTRUCTION_PUSH
		push %rbx
# METADATA_DEBUG_INFO
		# 198: {0, 33, 42}
# INSTRUCTION_CONST
		mov $8, %rdx
# METADATA_DEBUG_INFO
		# 199: {0, 33, 42}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %rdx, 1), %rdi
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# METADATA_DEBUG_INFO
		# 200: {0, 33}
# INSTRUCTION_PUSH
		push %rdi
# METADATA_DEBUG_INFO
		# 201: {0, 43}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rbx
# METADATA_DEBUG_INFO
		# 202: {0, 43}
# RUNTIME_NULLPTR_CHECK
		pushq %rbx
		call nullPtrCheck
		addq $8, %rsp
# METADATA_DEBUG_INFO
		# 203: {0, 43, 44}
# INSTRUCTION_CONST
		mov $16, %rdx
# METADATA_DEBUG_INFO
		# 204: {0, 43, 44}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rdx, 1), %rbx
# METADATA_DEBUG_INFO
		# 205: {0, 43, 45}
# INSTRUCTION_SET_ZERO
		xorq %rdx, %rdx
# METADATA_DEBUG_INFO
		# 206: {0, 43, 45}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rdx, 1), %rbx
# METADATA_DEBUG_INFO
		# 207: {0, 43}
# INSTRUCTION_REGISTER_CALL
		call *%rbx
# METADATA_DEBUG_INFO
		# 208: {0}
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# METADATA_DEBUG_INFO
		# 209: {0}
# COMPLEX_RESTORE_ALL
		popq %r15
		popq %r14
		popq %r13
		popq %r12
		popq %r11
		popq %r10
		popq %r9
		popq %r8
		popq %rdi
		popq %rsi
		popq %rbx
		popq %rdx
		popq %rcx
# METADATA_DEBUG_INFO
		# 210: {0, 46}
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rdx
movq %rbp, 0(%rdx)
# METADATA_DEBUG_INFO
		# 211: {0}
# INSTRUCTION_COPY
		mov %rax, %rdx
# COMPLEX_GARBAGE_COLLECT
		pushq %rbp
		call garbageCollect
		popq %rbp
# METADATA_DEBUG_INFO
		# 212: {0, 48}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rdi
# METADATA_DEBUG_INFO
		# 213: {0, 48}
# RUNTIME_NULLPTR_CHECK
		pushq %rdi
		call nullPtrCheck
		addq $8, %rsp
# METADATA_DEBUG_INFO
		# 214: {0, 48, 49}
# INSTRUCTION_CONST
		mov $16, %rdx
# METADATA_DEBUG_INFO
		# 215: {0, 48, 49}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %rdx, 1), %rdi
# METADATA_DEBUG_INFO
		# 216: {0, 48}
# RUNTIME_NULLPTR_CHECK
		pushq %rdi
		call nullPtrCheck
		addq $8, %rsp
# METADATA_DEBUG_INFO
		# 217: {0, 48}
# COMPLEX_SAVE_ALL
		pushq %rcx
		pushq %rdx
		pushq %rbx
		pushq %rsi
		pushq %rdi
		pushq %r8
		pushq %r9
		pushq %r10
		pushq %r11
		pushq %r12
		pushq %r13
		pushq %r14
		pushq %r15
# METADATA_DEBUG_INFO
		# 218: {0, 48, 50}
# COMPLEX_RIP_LAMBDA_LOAD
		leaq lambda_2(%rip), %rsi
# METADATA_DEBUG_INFO
		# 219: {0, 48, 50, 51}
# INSTRUCTION_CONST
		mov $1, %rbx
# METADATA_DEBUG_INFO
		# 220: {0, 48, 50, 51, 52}
# COMPLEX_ALLOCATE
		movq $8, %rdx
		imulq %rbx, %rdx
# ALLOC_LAMBDA
		addq $32, %rdx
		pushq %rdx
		pushq %rbp
		call garbageCollectAllocate
		movq $2, 0(%rax)
		movq $1, 24(%rax)
		movq $1, 32(%rax)
		popq %rdx
		popq %rdx
		addq $8, %rax
# METADATA_DEBUG_INFO
		# 221: {0, 48, 50, 53}
# INSTRUCTION_SET_ZERO
		xorq %rdx, %rdx
# METADATA_DEBUG_INFO
		# 222: {0, 48, 50, 53}
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rsi, (%rax, %rdx,1)
# METADATA_DEBUG_INFO
		# 231: {0, 48, 56}
# INSTRUCTION_COPY
		mov %rax, %rbx
# METADATA_DEBUG_INFO
		# 232: {0, 48, 56}
# INSTRUCTION_PUSH
		push %rbx
# METADATA_DEBUG_INFO
		# 233: {0, 48, 57}
# INSTRUCTION_CONST
		mov $8, %rdx
# METADATA_DEBUG_INFO
		# 234: {0, 48, 57}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %rdx, 1), %rdi
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# METADATA_DEBUG_INFO
		# 235: {0, 48}
# INSTRUCTION_PUSH
		push %rdi
# METADATA_DEBUG_INFO
		# 236: {0, 58}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rbx
# METADATA_DEBUG_INFO
		# 237: {0, 58}
# RUNTIME_NULLPTR_CHECK
		pushq %rbx
		call nullPtrCheck
		addq $8, %rsp
# METADATA_DEBUG_INFO
		# 238: {0, 58, 59}
# INSTRUCTION_CONST
		mov $16, %rdx
# METADATA_DEBUG_INFO
		# 239: {0, 58, 59}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rdx, 1), %rbx
# METADATA_DEBUG_INFO
		# 240: {0, 58, 60}
# INSTRUCTION_SET_ZERO
		xorq %rdx, %rdx
# METADATA_DEBUG_INFO
		# 241: {0, 58, 60}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rdx, 1), %rbx
# METADATA_DEBUG_INFO
		# 242: {0, 58}
# INSTRUCTION_REGISTER_CALL
		call *%rbx
# METADATA_DEBUG_INFO
		# 243: {0}
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# METADATA_DEBUG_INFO
		# 244: {0}
# COMPLEX_RESTORE_ALL
		popq %r15
		popq %r14
		popq %r13
		popq %r12
		popq %r11
		popq %r10
		popq %r9
		popq %r8
		popq %rdi
		popq %rsi
		popq %rbx
		popq %rdx
		popq %rcx
# METADATA_DEBUG_INFO
		# 245: {0, 61}
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rdx
movq %rbp, 0(%rdx)
# METADATA_DEBUG_INFO
		# 246: {0}
# INSTRUCTION_COPY
		mov %rax, %rdx
# METADATA_DEBUG_INFO
		# 247: {0, 63}
# INSTRUCTION_CONST
		mov $99, %rsi
# METADATA_DEBUG_INFO
		# 248: {0, 63, 64}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rbx
# METADATA_DEBUG_INFO
		# 249: {0, 63, 64}
# RUNTIME_NULLPTR_CHECK
		pushq %rbx
		call nullPtrCheck
		addq $8, %rsp
# METADATA_DEBUG_INFO
		# 250: {0, 63, 64, 65}
# INSTRUCTION_SET_ZERO
		xorq %rdx, %rdx
# METADATA_DEBUG_INFO
		# 251: {0, 63, 64, 65}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rdx, 1), %rbx
# METADATA_DEBUG_INFO
		# 252: {0, 63, 64}
# RUNTIME_NULLPTR_CHECK
		pushq %rbx
		call nullPtrCheck
		addq $8, %rsp
# METADATA_DEBUG_INFO
		# 253: {0, 63, 64, 66}
# INSTRUCTION_SET_ZERO
		xorq %rdx, %rdx
# METADATA_DEBUG_INFO
		# 254: {0, 63, 64, 66}
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rsi, (%rbx, %rdx,1)
# METADATA_DEBUG_INFO
		# 255: {0, 67}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rdi
# METADATA_DEBUG_INFO
		# 256: {0, 67}
# RUNTIME_NULLPTR_CHECK
		pushq %rdi
		call nullPtrCheck
		addq $8, %rsp
# METADATA_DEBUG_INFO
		# 257: {0, 67, 68}
# INSTRUCTION_CONST
		mov $16, %rdx
# METADATA_DEBUG_INFO
		# 258: {0, 67, 68}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %rdx, 1), %rdi
# METADATA_DEBUG_INFO
		# 259: {0, 67}
# RUNTIME_NULLPTR_CHECK
		pushq %rdi
		call nullPtrCheck
		addq $8, %rsp
# METADATA_DEBUG_INFO
		# 260: {0, 67}
# COMPLEX_SAVE_ALL
		pushq %rcx
		pushq %rdx
		pushq %rbx
		pushq %rsi
		pushq %rdi
		pushq %r8
		pushq %r9
		pushq %r10
		pushq %r11
		pushq %r12
		pushq %r13
		pushq %r14
		pushq %r15
# METADATA_DEBUG_INFO
		# 261: {0, 67, 69}
# COMPLEX_RIP_LAMBDA_LOAD
		leaq lambda_3(%rip), %rsi
# METADATA_DEBUG_INFO
		# 262: {0, 67, 69, 70}
# INSTRUCTION_CONST
		mov $1, %rbx
# METADATA_DEBUG_INFO
		# 263: {0, 67, 69, 70, 71}
# COMPLEX_ALLOCATE
		movq $8, %rdx
		imulq %rbx, %rdx
# ALLOC_LAMBDA
		addq $32, %rdx
		pushq %rdx
		pushq %rbp
		call garbageCollectAllocate
		movq $2, 0(%rax)
		movq $1, 24(%rax)
		movq $1, 32(%rax)
		popq %rdx
		popq %rdx
		addq $8, %rax
# METADATA_DEBUG_INFO
		# 264: {0, 67, 69, 72}
# INSTRUCTION_SET_ZERO
		xorq %rdx, %rdx
# METADATA_DEBUG_INFO
		# 265: {0, 67, 69, 72}
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rsi, (%rax, %rdx,1)
# METADATA_DEBUG_INFO
		# 274: {0, 67, 75}
# INSTRUCTION_COPY
		mov %rax, %rbx
# METADATA_DEBUG_INFO
		# 275: {0, 67, 75}
# INSTRUCTION_PUSH
		push %rbx
# METADATA_DEBUG_INFO
		# 276: {0, 67, 76}
# INSTRUCTION_CONST
		mov $8, %rdx
# METADATA_DEBUG_INFO
		# 277: {0, 67, 76}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rdi, %rdx, 1), %rdi
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# METADATA_DEBUG_INFO
		# 278: {0, 67}
# INSTRUCTION_PUSH
		push %rdi
# METADATA_DEBUG_INFO
		# 279: {0, 77}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -56(%rbp), %rbx
# METADATA_DEBUG_INFO
		# 280: {0, 77}
# RUNTIME_NULLPTR_CHECK
		pushq %rbx
		call nullPtrCheck
		addq $8, %rsp
# METADATA_DEBUG_INFO
		# 281: {0, 77, 78}
# INSTRUCTION_CONST
		mov $16, %rdx
# METADATA_DEBUG_INFO
		# 282: {0, 77, 78}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rdx, 1), %rbx
# METADATA_DEBUG_INFO
		# 283: {0, 77, 79}
# INSTRUCTION_SET_ZERO
		xorq %rdx, %rdx
# METADATA_DEBUG_INFO
		# 284: {0, 77, 79}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rdx, 1), %rbx
# METADATA_DEBUG_INFO
		# 285: {0, 77}
# INSTRUCTION_REGISTER_CALL
		call *%rbx
# METADATA_DEBUG_INFO
		# 286: {0}
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# METADATA_DEBUG_INFO
		# 287: {0}
# COMPLEX_RESTORE_ALL
		popq %r15
		popq %r14
		popq %r13
		popq %r12
		popq %r11
		popq %r10
		popq %r9
		popq %r8
		popq %rdi
		popq %rsi
		popq %rbx
		popq %rdx
		popq %rcx
# METADATA_DEBUG_INFO
		# 288: {0, 80}
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rdx
movq %rbp, 0(%rdx)
# METADATA_DEBUG_INFO
		# 289: {0}
# INSTRUCTION_COPY
		mov %rax, %rdx
# METADATA_DEBUG_INFO
		# 290: {0}
# COMPLEX_SAVE_ALL
		pushq %rcx
		pushq %rdx
		pushq %rbx
		pushq %rsi
		pushq %rdi
		pushq %r8
		pushq %r9
		pushq %r10
		pushq %r11
		pushq %r12
		pushq %r13
		pushq %r14
		pushq %r15
# METADATA_DEBUG_INFO
		# 291: {0, 82}
# COMPLEX_RIP_LAMBDA_LOAD
		leaq lambda_4(%rip), %rsi
# METADATA_DEBUG_INFO
		# 292: {0, 82, 83}
# INSTRUCTION_CONST
		mov $1, %rbx
# METADATA_DEBUG_INFO
		# 293: {0, 82, 83, 84}
# COMPLEX_ALLOCATE
		movq $8, %rdx
		imulq %rbx, %rdx
# ALLOC_LAMBDA
		addq $32, %rdx
		pushq %rdx
		pushq %rbp
		call garbageCollectAllocate
		movq $2, 0(%rax)
		movq $1, 24(%rax)
		movq $1, 32(%rax)
		popq %rdx
		popq %rdx
		addq $8, %rax
# METADATA_DEBUG_INFO
		# 294: {0, 82, 85}
# INSTRUCTION_SET_ZERO
		xorq %rdx, %rdx
# METADATA_DEBUG_INFO
		# 295: {0, 82, 85}
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rsi, (%rax, %rdx,1)
# METADATA_DEBUG_INFO
		# 304: {0, 88}
# INSTRUCTION_COPY
		mov %rax, %rdx
# METADATA_DEBUG_INFO
		# 305: {0, 88}
# INSTRUCTION_PUSH
		push %rdx
# METADATA_DEBUG_INFO
		# 306: {0, 89}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rbx
# METADATA_DEBUG_INFO
		# 307: {0, 89}
# RUNTIME_NULLPTR_CHECK
		pushq %rbx
		call nullPtrCheck
		addq $8, %rsp
# METADATA_DEBUG_INFO
		# 308: {0, 89, 90}
# INSTRUCTION_CONST
		mov $8, %rdx
# METADATA_DEBUG_INFO
		# 309: {0, 89, 90}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rdx, 1), %rbx
# METADATA_DEBUG_INFO
		# CAPTURE PUSH
# METADATA_DEBUG_INFO
		# 310: {0, 89}
# INSTRUCTION_PUSH
		push %rbx
# METADATA_DEBUG_INFO
		# 311: {0, 91}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -48(%rbp), %rbx
# METADATA_DEBUG_INFO
		# 312: {0, 91, 92}
# INSTRUCTION_SET_ZERO
		xorq %rdx, %rdx
# METADATA_DEBUG_INFO
		# 313: {0, 91, 92}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rdx, 1), %rbx
# METADATA_DEBUG_INFO
		# 314: {0, 91}
# INSTRUCTION_REGISTER_CALL
		call *%rbx
# METADATA_DEBUG_INFO
		# 315: {0}
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# METADATA_DEBUG_INFO
		# 316: {0}
# COMPLEX_RESTORE_ALL
		popq %r15
		popq %r14
		popq %r13
		popq %r12
		popq %r11
		popq %r10
		popq %r9
		popq %r8
		popq %rdi
		popq %rsi
		popq %rbx
		popq %rdx
		popq %rcx
# METADATA_DEBUG_INFO
		# 317: {0, 93}
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rdx
movq %rbp, 0(%rdx)
# METADATA_DEBUG_INFO
		# 318: {0}
# INSTRUCTION_COPY
		mov %rax, %rdx
movq $0, %rax
leave
ret
# METADATA_DEBUG_INFO
		# 0: {0, 1}
# INSTRUCTION_FUNCTION_LABEL
.type Optional_constructor, @function
Optional_constructor:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
		subq $48, %rsp
		movq %rax, -8(%rbp)
		movq $0, -16(%rbp)
		leaq staticLink, %rdx
		movq %rbp, 8(%rdx)
# METADATA_DEBUG_INFO
			# 1: {0, 2}
# METADATA_FUNCTION_ARGUMENT
			mov 16(%rbp), %rdx
			mov %rdx, -32(%rbp)
# METADATA_DEBUG_INFO
			# 2: {0, 2}
# METADATA_FUNCTION_ARGUMENT
			mov 24(%rbp), %rdx
			mov %rdx, -24(%rbp)
# METADATA_DEBUG_INFO
			# 3: {0, 3}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rsi
# METADATA_DEBUG_INFO
			# CLASS LOAD
# METADATA_DEBUG_INFO
			# 4: {0, 3, 4}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -32(%rbp), %rbx
# METADATA_DEBUG_INFO
			# 5: {0, 3, 4, 5}
# INSTRUCTION_SET_ZERO
			xorq %rdx, %rdx
# METADATA_DEBUG_INFO
			# 6: {0, 3, 4, 5}
# INSTRUCTION_MOVE_TO_OFFSET
			mov %rsi, (%rbx, %rdx,1)
# METADATA_DEBUG_INFO
			# 7: {0, 6}
# INSTRUCTION_CONST
			mov $1, %rsi
# METADATA_DEBUG_INFO
			# CLASS LOAD
# METADATA_DEBUG_INFO
			# 8: {0, 6, 7}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -32(%rbp), %rbx
# METADATA_DEBUG_INFO
			# 9: {0, 6, 7, 8}
# INSTRUCTION_CONST
			mov $8, %rdx
# METADATA_DEBUG_INFO
			# 10: {0, 6, 7, 8}
# INSTRUCTION_MOVE_TO_OFFSET
			mov %rsi, (%rbx, %rdx,1)
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
# METADATA_DEBUG_INFO
		# 11: {0, 3}
# INSTRUCTION_FUNCTION_LABEL
.type IntBox_constructor, @function
IntBox_constructor:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
		subq $48, %rsp
		movq %rax, -8(%rbp)
		movq $0, -16(%rbp)
		leaq staticLink, %rsi
		movq %rbp, 8(%rsi)
# METADATA_DEBUG_INFO
			# 12: {0, 4}
# METADATA_FUNCTION_ARGUMENT
			mov 16(%rbp), %rbx
			mov %rbx, -32(%rbp)
# METADATA_DEBUG_INFO
			# 13: {0, 4}
# METADATA_FUNCTION_ARGUMENT
			mov 24(%rbp), %rbx
			mov %rbx, -24(%rbp)
# METADATA_DEBUG_INFO
			# 14: {0, 5}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rdx
# METADATA_DEBUG_INFO
			# CLASS LOAD
# METADATA_DEBUG_INFO
			# 15: {0, 5, 6}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -32(%rbp), %rsi
# METADATA_DEBUG_INFO
			# 16: {0, 5, 6, 7}
# INSTRUCTION_SET_ZERO
			xorq %rbx, %rbx
# METADATA_DEBUG_INFO
			# 17: {0, 5, 6, 7}
# INSTRUCTION_MOVE_TO_OFFSET
			mov %rdx, (%rsi, %rbx,1)
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
# METADATA_DEBUG_INFO
		# 18: {0, 5}
# INSTRUCTION_FUNCTION_LABEL
.type c__0, @function
c__0:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
		subq $72, %rsp
		movq %rax, -8(%rbp)
		movq $2, -16(%rbp)
		movq $0, -24(%rbp)
		movq $0, -32(%rbp)
		movq $1, -32(%rbp)
		movq $0, -40(%rbp)
		leaq staticLink, %rdx
		movq %rbp, 8(%rdx)
# VAR internalOpt
# VAR internalIntBox
# METADATA_DEBUG_INFO
			# ALLOC
# METADATA_DEBUG_INFO
			# internalIntBox
# METADATA_DEBUG_INFO
			# 19: {0, 7}
# INSTRUCTION_CONST
			mov $1, %rbx
# METADATA_DEBUG_INFO
			# 20: {0, 7, 8}
# COMPLEX_ALLOCATE
			movq $8, %rdx
			imulq %rbx, %rdx
# ALLOC_RECORD_CLASS
			addq $16, %rdx
			pushq %rdx
			pushq %rbp
			call garbageCollectAllocate
			movq %rbx, 0(%rax)
# ALLOC_RECORD_CLASS
			subq $8, %rdx
			movq $0, (%rax, %rdx, 1)
			popq %rdx
			popq %rdx
			addq $8, %rax
# METADATA_DEBUG_INFO
			# 21: {0}
# INSTRUCTION_PUSH
			push %rax
# METADATA_DEBUG_INFO
			# 22: {0}
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
			mov %rax, -48(%rbp)
# METADATA_DEBUG_INFO
			# 23: {9}
# INSTRUCTION_SET_ZERO
			xorq %rsi, %rsi
# METADATA_DEBUG_INFO
			# 24: {9, 10}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -48(%rbp), %rbx
# METADATA_DEBUG_INFO
			# 25: {9, 10}
# RUNTIME_NULLPTR_CHECK
			pushq %rbx
			call nullPtrCheck
			addq $8, %rsp
# METADATA_DEBUG_INFO
			# 26: {9, 10, 11}
# INSTRUCTION_SET_ZERO
			xorq %rdx, %rdx
# METADATA_DEBUG_INFO
			# 27: {9, 10, 11}
# INSTRUCTION_MOVE_TO_OFFSET
			mov %rsi, (%rbx, %rdx,1)
# METADATA_DEBUG_INFO
			# 28: {0}
# INSTRUCTION_POP
			pop %rax
# METADATA_DEBUG_INFO
			# 29: {0}
# INSTRUCTION_PUSH
			push %rax
# METADATA_DEBUG_INFO
			# 30: {0, 12}
# INSTRUCTION_CONST
			mov $13, %rdx
# METADATA_DEBUG_INFO
			# 31: {0, 12}
# INSTRUCTION_PUSH
			push %rdx
# METADATA_DEBUG_INFO
			# 32: {0}
# INSTRUCTION_PUSH
			push %rax
# METADATA_DEBUG_INFO
			# 33: {0}
# INSTRUCTION_FUNCTION_CALL
			call IntBox_constructor
# METADATA_DEBUG_INFO
			# 34: {}
# INSTRUCTION_ADD_STACK_PTR
			addq $16, %rsp
# METADATA_DEBUG_INFO
			# 35: {0}
# INSTRUCTION_POP
			pop %rax
# METADATA_DEBUG_INFO
			# ALLOC
# METADATA_DEBUG_INFO
			# internalOpt
# METADATA_DEBUG_INFO
			# 36: {0, 13}
# INSTRUCTION_CONST
			mov $3, %rbx
# METADATA_DEBUG_INFO
			# 37: {0, 13, 14}
# COMPLEX_ALLOCATE
			movq $8, %rdx
			imulq %rbx, %rdx
# ALLOC_RECORD_CLASS
			addq $32, %rdx
			pushq %rdx
			pushq %rbp
			call garbageCollectAllocate
			movq %rbx, 0(%rax)
# ALLOC_RECORD_CLASS
			subq $24, %rdx
			movq $2, (%rax, %rdx, 1)
			movq $0, 8(%rax, %rdx, 1)
			movq $2, 16(%rax, %rdx, 1)
			popq %rdx
			popq %rdx
			addq $8, %rax
# METADATA_DEBUG_INFO
			# 38: {0}
# INSTRUCTION_PUSH
			push %rax
# METADATA_DEBUG_INFO
			# 39: {0}
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
			mov %rax, -40(%rbp)
# METADATA_DEBUG_INFO
			# 40: {0, 15}
# INSTRUCTION_SET_ZERO
			xorq %rsi, %rsi
# METADATA_DEBUG_INFO
			# 41: {0, 15, 16}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -40(%rbp), %rbx
# METADATA_DEBUG_INFO
			# 42: {0, 15, 16}
# RUNTIME_NULLPTR_CHECK
			pushq %rbx
			call nullPtrCheck
			addq $8, %rsp
# METADATA_DEBUG_INFO
			# 43: {0, 15, 16, 17}
# INSTRUCTION_CONST
			mov $8, %rdx
# METADATA_DEBUG_INFO
			# 44: {0, 15, 16, 17}
# INSTRUCTION_MOVE_TO_OFFSET
			mov %rsi, (%rbx, %rdx,1)
# METADATA_DEBUG_INFO
			# 45: {0, 18}
# COMPLEX_RIP_LAMBDA_LOAD
			leaq lambda_0(%rip), %rdi
# METADATA_DEBUG_INFO
			# 46: {0, 18, 19}
# INSTRUCTION_CONST
			mov $1, %rbx
# METADATA_DEBUG_INFO
			# 47: {0, 18, 19, 20}
# COMPLEX_ALLOCATE
			movq $8, %rdx
			imulq %rbx, %rdx
# ALLOC_LAMBDA
			addq $32, %rdx
			pushq %rdx
			pushq %rbp
			call garbageCollectAllocate
			movq $2, 0(%rax)
			movq $1, 24(%rax)
			movq $1, 32(%rax)
			popq %rdx
			popq %rdx
			addq $8, %rax
# METADATA_DEBUG_INFO
			# 48: {0, 18, 21}
# INSTRUCTION_SET_ZERO
			xorq %rsi, %rsi
# METADATA_DEBUG_INFO
			# 49: {0, 18, 21}
# INSTRUCTION_MOVE_TO_OFFSET
			mov %rdi, (%rax, %rsi,1)
# METADATA_DEBUG_INFO
			# 84: {0, 24}
# INSTRUCTION_COPY
			mov %rax, %rsi
# METADATA_DEBUG_INFO
			# 85: {24, 25}
# INSTRUCTION_CONST
			mov $8, %rdx
# METADATA_DEBUG_INFO
			# 86: {0, 24, 25}
# INSTRUCTION_POP
			pop %rax
# METADATA_DEBUG_INFO
			# 87: {0, 24, 25}
# INSTRUCTION_MOVE_TO_OFFSET
			mov %rax, (%rsi, %rdx,1)
# METADATA_DEBUG_INFO
			# 88: {0, 24}
# INSTRUCTION_PUSH
			push %rax
# METADATA_DEBUG_INFO
			# 89: {24, 26}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -40(%rbp), %rbx
# METADATA_DEBUG_INFO
			# 90: {24, 26}
# RUNTIME_NULLPTR_CHECK
			pushq %rbx
			call nullPtrCheck
			addq $8, %rsp
# METADATA_DEBUG_INFO
			# 91: {24, 26, 27}
# INSTRUCTION_CONST
			mov $16, %rdx
# METADATA_DEBUG_INFO
			# 92: {24, 26, 27}
# INSTRUCTION_MOVE_TO_OFFSET
			mov %rsi, (%rbx, %rdx,1)
# METADATA_DEBUG_INFO
			# 93: {0}
# INSTRUCTION_POP
			pop %rax
# METADATA_DEBUG_INFO
			# 94: {0}
# INSTRUCTION_PUSH
			push %rax
# METADATA_DEBUG_INFO
			# 95: {0, 28}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -48(%rbp), %rsi
# METADATA_DEBUG_INFO
			# 96: {0, 28}
# INSTRUCTION_PUSH
			push %rsi
# METADATA_DEBUG_INFO
			# 97: {0}
# INSTRUCTION_PUSH
			push %rax
# METADATA_DEBUG_INFO
			# 98: {0}
# INSTRUCTION_FUNCTION_CALL
			call Optional_constructor
# METADATA_DEBUG_INFO
			# 99: {}
# INSTRUCTION_ADD_STACK_PTR
			addq $16, %rsp
# METADATA_DEBUG_INFO
			# 100: {}
# INSTRUCTION_POP
			pop %rax
# METADATA_DEBUG_INFO
			# 101: {29}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -40(%rbp), %rdx
# METADATA_DEBUG_INFO
			# 102: {29}
# RUNTIME_NULLPTR_CHECK
			pushq %rdx
			call nullPtrCheck
			addq $8, %rsp
# METADATA_DEBUG_INFO
			# 103: {29, 30}
# INSTRUCTION_CONST
			mov $16, %rbx
# METADATA_DEBUG_INFO
			# 104: {29, 30}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%rdx, %rbx, 1), %rdx
# METADATA_DEBUG_INFO
			# 105: {29}
# INSTRUCTION_RETURN
			mov %rdx, %rax
			mov %rbp,%rsp
pop %rbp
ret
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
# METADATA_DEBUG_INFO
		# 106: {0, 7}
# INSTRUCTION_FUNCTION_LABEL
.type b__0, @function
b__0:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
		subq $40, %rsp
		movq %rax, -8(%rbp)
		movq $0, -16(%rbp)
		leaq staticLink, %rbx
		movq %rbp, 8(%rbx)
# METADATA_DEBUG_INFO
			# 107: {0}
# COMPLEX_SAVE_ALL
			pushq %rcx
			pushq %rdx
			pushq %rbx
			pushq %rsi
			pushq %rdi
			pushq %r8
			pushq %r9
			pushq %r10
			pushq %r11
			pushq %r12
			pushq %r13
			pushq %r14
			pushq %r15
# METADATA_DEBUG_INFO
			# 108: {0}
# INSTRUCTION_FUNCTION_CALL
			call c__0
# METADATA_DEBUG_INFO
			# 109: {0}
# INSTRUCTION_ADD_STACK_PTR
			addq $0, %rsp
# METADATA_DEBUG_INFO
			# 110: {0}
# COMPLEX_RESTORE_ALL
			popq %r15
			popq %r14
			popq %r13
			popq %r12
			popq %r11
			popq %r10
			popq %r9
			popq %r8
			popq %rdi
			popq %rsi
			popq %rbx
			popq %rdx
			popq %rcx
# METADATA_DEBUG_INFO
			# 111: {0, 9}
# COMPLEX_RESTORE_STATIC_LINK
			leaq staticLink, %rsi
movq %rbp, 8(%rsi)
# METADATA_DEBUG_INFO
			# 112: {0, 10}
# INSTRUCTION_COPY
			mov %rax, %rbx
# METADATA_DEBUG_INFO
			# 113: {10}
# INSTRUCTION_RETURN
			mov %rbx, %rax
			mov %rbp,%rsp
pop %rbp
ret
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
# METADATA_DEBUG_INFO
		# 114: {0, 9}
# INSTRUCTION_FUNCTION_LABEL
.type a__0, @function
a__0:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
		subq $40, %rsp
		movq %rax, -8(%rbp)
		movq $0, -16(%rbp)
		leaq staticLink, %rsi
		movq %rbp, 8(%rsi)
# METADATA_DEBUG_INFO
			# 115: {0}
# COMPLEX_SAVE_ALL
			pushq %rcx
			pushq %rdx
			pushq %rbx
			pushq %rsi
			pushq %rdi
			pushq %r8
			pushq %r9
			pushq %r10
			pushq %r11
			pushq %r12
			pushq %r13
			pushq %r14
			pushq %r15
# METADATA_DEBUG_INFO
			# 116: {0}
# INSTRUCTION_FUNCTION_CALL
			call b__0
# METADATA_DEBUG_INFO
			# 117: {0}
# INSTRUCTION_ADD_STACK_PTR
			addq $0, %rsp
# METADATA_DEBUG_INFO
			# 118: {0}
# COMPLEX_RESTORE_ALL
			popq %r15
			popq %r14
			popq %r13
			popq %r12
			popq %r11
			popq %r10
			popq %r9
			popq %r8
			popq %rdi
			popq %rsi
			popq %rbx
			popq %rdx
			popq %rcx
# METADATA_DEBUG_INFO
			# 119: {0, 11}
# COMPLEX_RESTORE_STATIC_LINK
			leaq staticLink, %rdx
movq %rbp, 8(%rdx)
# METADATA_DEBUG_INFO
			# 120: {0, 12}
# INSTRUCTION_COPY
			mov %rax, %rdx
# METADATA_DEBUG_INFO
			# 121: {12}
# INSTRUCTION_RETURN
			mov %rdx, %rax
			mov %rbp,%rsp
pop %rbp
ret
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
# METADATA_DEBUG_INFO
		# 188: {0, 33, 39}
# INSTRUCTION_FUNCTION_LABEL
.type lambda_1, @function
lambda_1:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
		subq $56, %rsp
		movq %rax, -8(%rbp)
		movq $0, -16(%rbp)
		leaq staticLink, %rdx
		movq %rbp, 8(%rdx)
# METADATA_DEBUG_INFO
			# 189: {0, 33, 40}
# METADATA_FUNCTION_ARGUMENT
			mov 16(%rbp), %rdx
			mov %rdx, -32(%rbp)
# METADATA_DEBUG_INFO
			# 190: {0, 33, 40}
# METADATA_FUNCTION_ARGUMENT
			mov 24(%rbp), %rdx
			mov %rdx, -24(%rbp)
# METADATA_DEBUG_INFO
			# 191: {0, 33, 41}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rbx
# METADATA_DEBUG_INFO
			# 192: {0, 33, 41}
# RUNTIME_NULLPTR_CHECK
			pushq %rbx
			call nullPtrCheck
			addq $8, %rsp
# METADATA_DEBUG_INFO
			# 193: {0, 33, 41, 42}
# INSTRUCTION_SET_ZERO
			xorq %rdx, %rdx
# METADATA_DEBUG_INFO
			# 194: {0, 33, 41, 42}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%rbx, %rdx, 1), %rbx
# METADATA_DEBUG_INFO
			# 195: {0, 33, 41}
# INSTRUCTION_WRITE
			pushq %rsi
			pushq %rdi
			movq %rbx, %rsi
			movq $intprint, %rdi
			movq $0, %rax
			call printf
			popq %rdi
			popq %rsi
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
# METADATA_DEBUG_INFO
		# 223: {0, 48, 54}
# INSTRUCTION_FUNCTION_LABEL
.type lambda_2, @function
lambda_2:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
		subq $56, %rsp
		movq %rax, -8(%rbp)
		movq $0, -16(%rbp)
		leaq staticLink, %rdx
		movq %rbp, 8(%rdx)
# METADATA_DEBUG_INFO
			# 224: {0, 48, 55}
# METADATA_FUNCTION_ARGUMENT
			mov 16(%rbp), %rdx
			mov %rdx, -32(%rbp)
# METADATA_DEBUG_INFO
			# 225: {0, 48, 55}
# METADATA_FUNCTION_ARGUMENT
			mov 24(%rbp), %rdx
			mov %rdx, -24(%rbp)
# METADATA_DEBUG_INFO
			# 226: {0, 48, 56}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rbx
# METADATA_DEBUG_INFO
			# 227: {0, 48, 56}
# RUNTIME_NULLPTR_CHECK
			pushq %rbx
			call nullPtrCheck
			addq $8, %rsp
# METADATA_DEBUG_INFO
			# 228: {0, 48, 56, 57}
# INSTRUCTION_SET_ZERO
			xorq %rdx, %rdx
# METADATA_DEBUG_INFO
			# 229: {0, 48, 56, 57}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%rbx, %rdx, 1), %rbx
# METADATA_DEBUG_INFO
			# 230: {0, 48, 56}
# INSTRUCTION_WRITE
			pushq %rsi
			pushq %rdi
			movq %rbx, %rsi
			movq $intprint, %rdi
			movq $0, %rax
			call printf
			popq %rdi
			popq %rsi
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
# METADATA_DEBUG_INFO
		# 266: {0, 67, 73}
# INSTRUCTION_FUNCTION_LABEL
.type lambda_3, @function
lambda_3:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
		subq $56, %rsp
		movq %rax, -8(%rbp)
		movq $0, -16(%rbp)
		leaq staticLink, %rdx
		movq %rbp, 8(%rdx)
# METADATA_DEBUG_INFO
			# 267: {0, 67, 74}
# METADATA_FUNCTION_ARGUMENT
			mov 16(%rbp), %rdx
			mov %rdx, -32(%rbp)
# METADATA_DEBUG_INFO
			# 268: {0, 67, 74}
# METADATA_FUNCTION_ARGUMENT
			mov 24(%rbp), %rdx
			mov %rdx, -24(%rbp)
# METADATA_DEBUG_INFO
			# 269: {0, 67, 75}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rbx
# METADATA_DEBUG_INFO
			# 270: {0, 67, 75}
# RUNTIME_NULLPTR_CHECK
			pushq %rbx
			call nullPtrCheck
			addq $8, %rsp
# METADATA_DEBUG_INFO
			# 271: {0, 67, 75, 76}
# INSTRUCTION_SET_ZERO
			xorq %rdx, %rdx
# METADATA_DEBUG_INFO
			# 272: {0, 67, 75, 76}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%rbx, %rdx, 1), %rbx
# METADATA_DEBUG_INFO
			# 273: {0, 67, 75}
# INSTRUCTION_WRITE
			pushq %rsi
			pushq %rdi
			movq %rbx, %rsi
			movq $intprint, %rdi
			movq $0, %rax
			call printf
			popq %rdi
			popq %rsi
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
# METADATA_DEBUG_INFO
		# 296: {0, 86}
# INSTRUCTION_FUNCTION_LABEL
.type lambda_4, @function
lambda_4:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
		subq $56, %rsp
		movq %rax, -8(%rbp)
		movq $0, -16(%rbp)
		leaq staticLink, %rdx
		movq %rbp, 8(%rdx)
# METADATA_DEBUG_INFO
			# 297: {0, 87}
# METADATA_FUNCTION_ARGUMENT
			mov 16(%rbp), %rdx
			mov %rdx, -32(%rbp)
# METADATA_DEBUG_INFO
			# 298: {0, 87}
# METADATA_FUNCTION_ARGUMENT
			mov 24(%rbp), %rdx
			mov %rdx, -24(%rbp)
# METADATA_DEBUG_INFO
			# 299: {0, 88}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rdx
# METADATA_DEBUG_INFO
			# 300: {0, 88}
# RUNTIME_NULLPTR_CHECK
			pushq %rdx
			call nullPtrCheck
			addq $8, %rsp
# METADATA_DEBUG_INFO
			# 301: {0, 88, 89}
# INSTRUCTION_SET_ZERO
			xorq %rbx, %rbx
# METADATA_DEBUG_INFO
			# 302: {0, 88, 89}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%rdx, %rbx, 1), %rdx
# METADATA_DEBUG_INFO
			# 303: {0, 88}
# INSTRUCTION_WRITE
			pushq %rsi
			pushq %rdi
			movq %rdx, %rsi
			movq $intprint, %rdi
			movq $0, %rax
			call printf
			popq %rdi
			popq %rsi
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
# METADATA_DEBUG_INFO
		# 50: {0, 22}
# INSTRUCTION_FUNCTION_LABEL
.type lambda_0, @function
lambda_0:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
		subq $56, %rsp
		movq %rax, -8(%rbp)
		movq $0, -16(%rbp)
		leaq staticLink, %rbx
		movq %rbp, 16(%rbx)
# METADATA_DEBUG_INFO
			# 51: {0, 23}
# METADATA_FUNCTION_ARGUMENT
			mov 16(%rbp), %rdx
			mov %rdx, -32(%rbp)
# METADATA_DEBUG_INFO
			# 52: {0, 23}
# METADATA_FUNCTION_ARGUMENT
			mov 24(%rbp), %rdx
			mov %rdx, -24(%rbp)
# METADATA_DEBUG_INFO
			# CLASS LOAD
# METADATA_DEBUG_INFO
			# 53: {0, 24}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -32(%rbp), %rsi
# METADATA_DEBUG_INFO
			# 54: {0, 24, 25}
# INSTRUCTION_CONST
			mov $8, %rdx
# METADATA_DEBUG_INFO
			# 55: {0, 24, 25}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%rsi, %rdx, 1), %rsi
# METADATA_DEBUG_INFO
			# 56: {0, 24, 26}
# INSTRUCTION_CONST
			mov $1, %rbx
# METADATA_DEBUG_INFO
			# 57: {0, 24, 26}
# INSTRUCTION_MINUS
			sub %rsi, %rbx
# METADATA_DEBUG_INFO
			# 58: {0, 26, 27}
# COMPLEX_ABS_VALUE
			movq %rbx, %rdx
			sar $63, %rdx
			addq %rdx, %rbx
			xor %rbx, %rdx
# METADATA_DEBUG_INFO
			# 59: {0, 27}
# COMPLEX_CONSTRAIN_BOOLEAN
			push %rax
			cmp $0, %rdx
			setg %al
			movsx %al, %rdx
			pop %rax
# METADATA_DEBUG_INFO
			# 60: {0, 27, 28}
# INSTRUCTION_CONST
			mov $1, %rsi
# METADATA_DEBUG_INFO
			# 61: {0, 27, 28}
# INSTRUCTION_MINUS
			sub %rdx, %rsi
# METADATA_DEBUG_INFO
			# 62: {0, 28, 29}
# INSTRUCTION_CONST
			mov $1, %rdx
# METADATA_DEBUG_INFO
			# 63: {0, 28, 29}
# INSTRUCTION_CMP
			cmp %rsi, %rdx
# METADATA_DEBUG_INFO
			# 64: {0}
# INSTRUCTION_JE
			je if_0_begin
# METADATA_DEBUG_INFO
			# 65: {0}
# INSTRUCTION_JMP
			jmp if_0_end
# INSTRUCTION_LABEL
			if_0_begin:
# METADATA_DEBUG_INFO
			# 66: {0}
# COMPLEX_SAVE_ALL
			pushq %rcx
			pushq %rdx
			pushq %rbx
			pushq %rsi
			pushq %rdi
			pushq %r8
			pushq %r9
			pushq %r10
			pushq %r11
			pushq %r12
			pushq %r13
			pushq %r14
			pushq %r15
# METADATA_DEBUG_INFO
			# CLASS LOAD
# METADATA_DEBUG_INFO
			# 67: {0, 30}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -32(%rbp), %rbx
# METADATA_DEBUG_INFO
			# 68: {0, 30, 31}
# INSTRUCTION_SET_ZERO
			xorq %rdx, %rdx
# METADATA_DEBUG_INFO
			# 69: {0, 30, 31}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%rbx, %rdx, 1), %rbx
# METADATA_DEBUG_INFO
			# 70: {0, 30}
# INSTRUCTION_PUSH
			push %rbx
# METADATA_DEBUG_INFO
			# 71: {0, 32}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rdx
# METADATA_DEBUG_INFO
			# 72: {0, 32}
# RUNTIME_NULLPTR_CHECK
			pushq %rdx
			call nullPtrCheck
			addq $8, %rsp
# METADATA_DEBUG_INFO
			# 73: {0, 32, 33}
# INSTRUCTION_CONST
			mov $8, %rdi
# METADATA_DEBUG_INFO
			# 74: {0, 32, 33}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%rdx, %rdi, 1), %rdx
# METADATA_DEBUG_INFO
			# CAPTURE PUSH
# METADATA_DEBUG_INFO
			# 75: {0, 32}
# INSTRUCTION_PUSH
			push %rdx
# METADATA_DEBUG_INFO
			# 76: {0, 34}
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rdx
# METADATA_DEBUG_INFO
			# 77: {0, 34, 35}
# INSTRUCTION_SET_ZERO
			xorq %rsi, %rsi
# METADATA_DEBUG_INFO
			# 78: {0, 34, 35}
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%rdx, %rsi, 1), %rdx
# METADATA_DEBUG_INFO
			# 79: {0, 34}
# INSTRUCTION_REGISTER_CALL
			call *%rdx
# METADATA_DEBUG_INFO
			# 80: {0}
# INSTRUCTION_ADD_STACK_PTR
			addq $16, %rsp
# METADATA_DEBUG_INFO
			# 81: {0}
# COMPLEX_RESTORE_ALL
			popq %r15
			popq %r14
			popq %r13
			popq %r12
			popq %r11
			popq %r10
			popq %r9
			popq %r8
			popq %rdi
			popq %rsi
			popq %rbx
			popq %rdx
			popq %rcx
# METADATA_DEBUG_INFO
			# 82: {0, 36}
# COMPLEX_RESTORE_STATIC_LINK
			leaq staticLink, %rbx
movq %rbp, 16(%rbx)
# METADATA_DEBUG_INFO
			# 83: {0}
# INSTRUCTION_COPY
			mov %rax, %rdx
# INSTRUCTION_LABEL
			if_0_end:
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
