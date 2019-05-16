.section .data
staticLink:
	.space 16
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
    movq $2097152000, 16(%r15)
    movq $0, 8(%r15)
    movq $1, 0(%r15)
    addq $2097152000, %rax
    push %rax
    movq %rax, %rdi
    mov $12, %rax
    syscall
    pop %rax
    #top should be in rax

    #move heap into one

    leaq gcHeapTwo, %r15
    movq %rax, 24(%r15)
    movq $2097152000, 16(%r15)
    movq $0, 8(%r15)
    movq $0, 0(%r15)

    addq $2097152000, %rax
    movq %rax, %rdi
    mov $12, %rax
    syscall
	subq $208, %rsp
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
# INSTRUCTION_CONST
		mov $1000, %rdx
# COMPLEX_ALLOCATE
		movq $8, %r8
		imulq %rdx, %r8
# ALLOC_ARR_OF_PRIM
		addq $16, %r8
		pushq %r8
		pushq %rbp
		call garbageCollectAllocate
		movq %rdx, 0(%rax)
		subq $8, %r8
		movq $0, (%rax, %r8, 1)
		popq %r8
		popq %r8
		addq $8, %rax
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -64(%rbp)
# INSTRUCTION_CONST
		mov $1000, %rsi
# COMPLEX_ALLOCATE
		movq $8, %rbx
		imulq %rsi, %rbx
# ALLOC_ARR_OF_PRIM
		addq $16, %rbx
		pushq %rbx
		pushq %rbp
		call garbageCollectAllocate
		movq %rsi, 0(%rax)
		subq $8, %rbx
		movq $0, (%rax, %rbx, 1)
		popq %rbx
		popq %rbx
		addq $8, %rax
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -72(%rbp)
# INSTRUCTION_CONST
		mov $1000, %rdx
# COMPLEX_ALLOCATE
		movq $8, %rbx
		imulq %rdx, %rbx
# ALLOC_ARR_OF_PRIM
		addq $16, %rbx
		pushq %rbx
		pushq %rbp
		call garbageCollectAllocate
		movq %rdx, 0(%rax)
		subq $8, %rbx
		movq $0, (%rax, %rbx, 1)
		popq %rbx
		popq %rbx
		addq $8, %rax
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -80(%rbp)
# INSTRUCTION_CONST
		mov $1000, %rdx
# COMPLEX_ALLOCATE
		movq $8, %rsi
		imulq %rdx, %rsi
# ALLOC_ARR_OF_PRIM
		addq $16, %rsi
		pushq %rsi
		pushq %rbp
		call garbageCollectAllocate
		movq %rdx, 0(%rax)
		subq $8, %rsi
		movq $0, (%rax, %rsi, 1)
		popq %rsi
		popq %rsi
		addq $8, %rax
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rax, -88(%rbp)
# INSTRUCTION_CONST
		mov $1000, %rdi
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdi, -120(%rbp)
# INSTRUCTION_CONST
		mov $600, %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rbx, -112(%rbp)
# INSTRUCTION_CONST
		mov $-1, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -128(%rbp)
# INSTRUCTION_SET_ZERO
		xorq %rsi, %rsi
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rsi, -136(%rbp)
# INSTRUCTION_SET_ZERO
		xorq %r8, %r8
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %r8, -144(%rbp)
# INSTRUCTION_CONST
		mov $1, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -160(%rbp)
# INSTRUCTION_CONST
		mov $40, %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rbx, -168(%rbp)
# INSTRUCTION_CONST
		mov $3641, %rdi
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdi, -176(%rbp)
# INSTRUCTION_CONST
		mov $729, %rsi
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rsi, -184(%rbp)
# INSTRUCTION_SET_ZERO
		xorq %rdx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -104(%rbp)
# INSTRUCTION_LABEL
		while_cnd_4:
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -104(%rbp), %rbx
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -120(%rbp), %r11
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MINUS
		sub %rbx, %r11
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rax
		cmp $0, %r11
		setg %al
		movsx %al, %r11
		pop %rax
# INSTRUCTION_CONST
		mov $1, %r8
# INSTRUCTION_CMP
		cmp %r11, %r8
# INSTRUCTION_JE
		je while_4_begin
# INSTRUCTION_JMP
		jmp while_4_end
# INSTRUCTION_LABEL
		while_4_begin:
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -160(%rbp), %rsi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_PUSH
		push %rsi
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -168(%rbp), %r10
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_PUSH
		push %r10
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -176(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_PUSH
		push %rdx
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -184(%rbp), %r9
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_PUSH
		push %r9
# INSTRUCTION_FUNCTION_CALL
		call NextRand__0
# INSTRUCTION_ADD_STACK_PTR
		addq $32, %rsp
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rdi
movq %rbp, 0(%rdi)
# INSTRUCTION_COPY
		mov %rax, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -160(%rbp)
# INSTRUCTION_COPY
		mov %rdx, %rsi
# INSTRUCTION_CONST
		mov $20, %r8
# INSTRUCTION_DIV
		movq %rsi, %rax
		pushq %r10
		movq %r8, %r10
		pushq %rdx
		cqto
		idiv %r10
		popq %rdx
		popq %r10
		movq %rax, %r8
# INSTRUCTION_LEA_ADD_CONST
		leaq 10(%r8), %r8
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -64(%rbp), %rdi
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -104(%rbp), %rsi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
		imul $8, %rsi
# INSTRUCTION_MOVE_TO_OFFSET
		mov %r8, (%rdi, %rsi,1)
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -160(%rbp), %rbx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_PUSH
		push %rbx
# INSTRUCTION_COPY
		mov %r10, %rdx
# INSTRUCTION_PUSH
		push %rdx
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -176(%rbp), %rdi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_PUSH
		push %rdi
# INSTRUCTION_COPY
		mov %r9, %rsi
# INSTRUCTION_PUSH
		push %rsi
# INSTRUCTION_FUNCTION_CALL
		call NextRand__0
# INSTRUCTION_ADD_STACK_PTR
		addq $32, %rsp
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rbx
movq %rbp, 0(%rbx)
# INSTRUCTION_COPY
		mov %rax, %rsi
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rsi, -160(%rbp)
# INSTRUCTION_COPY
		mov %rsi, %r8
# INSTRUCTION_CONST
		mov $90, %rdi
# INSTRUCTION_DIV
		movq %r8, %rax
		pushq %r10
		movq %rdi, %r10
		pushq %rdx
		cqto
		idiv %r10
		popq %rdx
		popq %r10
		movq %rax, %rdi
# INSTRUCTION_LEA_ADD_CONST
		leaq 1(%rdi), %rdi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -72(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -104(%rbp), %rsi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
		imul $8, %rsi
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rdi, (%rdx, %rsi,1)
# INSTRUCTION_SET_ZERO
		xorq %rdx, %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -80(%rbp), %r8
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -104(%rbp), %rdi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
		imul $8, %rdi
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rdx, (%r8, %rdi,1)
# INSTRUCTION_SET_ZERO
		xorq %rsi, %rsi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -88(%rbp), %rbx
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -104(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
		imul $8, %rdx
# INSTRUCTION_MOVE_TO_OFFSET
		mov %rsi, (%rbx, %rdx,1)
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -104(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_LEA_ADD_CONST
		leaq 1(%rdx), %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -104(%rbp)
# INSTRUCTION_JMP
		jmp while_cnd_4
# INSTRUCTION_LABEL
		while_4_end:
# INSTRUCTION_SET_ZERO
		xorq %rsi, %rsi
# INSTRUCTION_PUSH
		push %rsi
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -120(%rbp), %rbx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_SUB_CONST
		sub $1, %rbx
# INSTRUCTION_PUSH
		push %rbx
# INSTRUCTION_FUNCTION_CALL
		call quicksort__0
# INSTRUCTION_ADD_STACK_PTR
		addq $16, %rsp
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rdx
movq %rbp, 0(%rdx)
# INSTRUCTION_COPY
		mov %rax, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -152(%rbp)
# INSTRUCTION_SET_ZERO
		xorq %rbx, %rbx
# INSTRUCTION_PUSH
		push %rbx
# INSTRUCTION_SET_ZERO
		xorq %rdx, %rdx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_SET_ZERO
		xorq %rdx, %rdx
# INSTRUCTION_PUSH
		push %rdx
# INSTRUCTION_FUNCTION_CALL
		call BKnap__0
# INSTRUCTION_ADD_STACK_PTR
		addq $24, %rsp
# COMPLEX_RESTORE_STATIC_LINK
		leaq staticLink, %rdx
movq %rbp, 0(%rdx)
# INSTRUCTION_COPY
		mov %rax, %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rbx, -96(%rbp)
# INSTRUCTION_SET_ZERO
		xorq %rdx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -104(%rbp)
# INSTRUCTION_LABEL
		while_cnd_5:
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -104(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -120(%rbp), %rdi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MINUS
		sub %rdx, %rdi
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rax
		cmp $0, %rdi
		setg %al
		movsx %al, %rdi
		pop %rax
# INSTRUCTION_CONST
		mov $1, %rdx
# INSTRUCTION_CMP
		cmp %rdi, %rdx
# INSTRUCTION_JE
		je while_5_begin
# INSTRUCTION_JMP
		jmp while_5_end
# INSTRUCTION_LABEL
		while_5_begin:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -80(%rbp), %rbx
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -104(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
		imul $8, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
		mov (%rbx, %rdx, 1), %rbx
# INSTRUCTION_CONST
		mov $1, %rsi
# INSTRUCTION_MINUS
		sub %rbx, %rsi
# COMPLEX_ABS_VALUE
		movq %rsi, %rdx
		sar $63, %rdx
		addq %rdx, %rsi
		xor %rsi, %rdx
# COMPLEX_CONSTRAIN_BOOLEAN
		push %rax
		cmp $0, %rdx
		setg %al
		movsx %al, %rdx
		pop %rax
# INSTRUCTION_CONST
		mov $1, %rbx
# INSTRUCTION_MINUS
		sub %rdx, %rbx
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
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -144(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_LEA_ADD_CONST
		leaq 1(%rdx), %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -144(%rbp)
# INSTRUCTION_LABEL
		if_9_end:
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -104(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_LEA_ADD_CONST
		leaq 1(%rdx), %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
		mov %rdx, -104(%rbp)
# INSTRUCTION_JMP
		jmp while_cnd_5
# INSTRUCTION_LABEL
		while_5_end:
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -144(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_WRITE
		pushq %rsi
		pushq %rdi
		movq %rdx, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
		popq %rdi
		popq %rsi
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -128(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_WRITE
		pushq %rsi
		pushq %rdi
		movq %rdx, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
		popq %rdi
		popq %rsi
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
		mov -136(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_WRITE
		pushq %rsi
		pushq %rdi
		movq %rdx, %rsi
		movq $intprint, %rdi
		movq $0, %rax
		call printf
		popq %rdi
		popq %rsi
movq $0, %rax
leave
ret
# INSTRUCTION_FUNCTION_LABEL
.type BKnap__0, @function
BKnap__0:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
		subq $80, %rsp
		movq %rax, -8(%rbp)
		movq $0, -16(%rbp)
		pushq %rdx
		leaq staticLink, %rdx
		movq %rbp, 8(%rdx)
		popq %rdx
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
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -40(%rbp), %r8
# METADATA_ACCESS_VARIABLE_END
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rbx
			mov 0(%rbx), %rbx
			movq -16(%rbx), %rdx
			addq $1, %rdx
			imul $-1, %rdx
			mov -24(%rbx, %rdx, 8), %rsi
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rbx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
			imul $8, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%rsi, %rbx, 1), %rsi
# INSTRUCTION_LEA_ADD
			leaq (%r8, %rsi, 1), %r8
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rsi
			mov 0(%rsi), %rsi
			movq -16(%rsi), %rdx
			addq $1, %rdx
			imul $-1, %rdx
			mov -72(%rsi, %rdx, 8), %rdi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MINUS
			sub %r8, %rdi
# INSTRUCTION_LEA_ADD_CONST
			leaq 1(%rdi), %rdi
# COMPLEX_CONSTRAIN_BOOLEAN
			push %rax
			cmp $0, %rdi
			setg %al
			movsx %al, %rdi
			pop %rax
# INSTRUCTION_CONST
			mov $1, %rdx
# INSTRUCTION_CMP
			cmp %rdi, %rdx
# INSTRUCTION_JE
			je if_0_begin
# INSTRUCTION_JMP
			jmp if_0_end
# INSTRUCTION_LABEL
			if_0_begin:
# INSTRUCTION_CONST
			mov $1, %r8
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rbx
			mov 0(%rbx), %rbx
			movq -16(%rbx), %rdx
			addq $1, %rdx
			imul $-1, %rdx
			mov -48(%rbx, %rdx, 8), %rsi
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rbx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
			imul $8, %rbx
# INSTRUCTION_MOVE_TO_OFFSET
			mov %r8, (%rsi, %rbx,1)
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rsi
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rbx
			mov 0(%rbx), %rbx
			movq -16(%rbx), %rdx
			addq $1, %rdx
			imul $-1, %rdx
			mov -80(%rbx, %rdx, 8), %rdi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_SUB_CONST
			sub $1, %rdi
# INSTRUCTION_MINUS
			sub %rsi, %rdi
# COMPLEX_CONSTRAIN_BOOLEAN
			push %rax
			cmp $0, %rdi
			setg %al
			movsx %al, %rdi
			pop %rax
# INSTRUCTION_CONST
			mov $1, %r8
# INSTRUCTION_CMP
			cmp %rdi, %r8
# INSTRUCTION_JE
			je if_1_begin
# INSTRUCTION_JMP
			jmp if_1_end
# INSTRUCTION_LABEL
			if_1_begin:
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_LEA_ADD_CONST
			leaq 1(%rdx), %rdx
# INSTRUCTION_PUSH
			push %rdx
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -32(%rbp), %rdi
# METADATA_ACCESS_VARIABLE_END
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rdx
			mov 0(%rdx), %rdx
			movq -16(%rdx), %rbx
			addq $1, %rbx
			imul $-1, %rbx
			mov -32(%rdx, %rbx, 8), %rsi
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %r11
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
			imul $8, %r11
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%rsi, %r11, 1), %rsi
# INSTRUCTION_LEA_ADD
			leaq (%rdi, %rsi, 1), %rdi
# INSTRUCTION_PUSH
			push %rdi
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -40(%rbp), %rsi
# METADATA_ACCESS_VARIABLE_END
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rdx
			mov 0(%rdx), %rdx
			movq -16(%rdx), %r9
			addq $1, %r9
			imul $-1, %r9
			mov -24(%rdx, %r9, 8), %r10
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rdi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
			imul $8, %rdi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%r10, %rdi, 1), %r10
# INSTRUCTION_LEA_ADD
			leaq (%rsi, %r10, 1), %rsi
# INSTRUCTION_PUSH
			push %rsi
# INSTRUCTION_FUNCTION_CALL
			call BKnap__0
# INSTRUCTION_ADD_STACK_PTR
			addq $24, %rsp
# COMPLEX_RESTORE_STATIC_LINK
			leaq staticLink, %rsi
movq %rbp, 8(%rsi)
# INSTRUCTION_COPY
			mov %rax, %r8
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
			mov %r8, -48(%rbp)
# INSTRUCTION_LABEL
			if_1_end:
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -32(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rsi
			mov 0(%rsi), %rsi
			movq -16(%rsi), %r8
			addq $1, %r8
			imul $-1, %r8
			mov -32(%rsi, %r8, 8), %rdi
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rbx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
			imul $8, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%rdi, %rbx, 1), %rdi
# INSTRUCTION_LEA_ADD
			leaq (%rdx, %rdi, 1), %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rsi
			mov 0(%rsi), %rsi
			movq -16(%rsi), %rbx
			addq $1, %rbx
			imul $-1, %rbx
			mov -88(%rsi, %rbx, 8), %rdi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MINUS
			sub %rdi, %rdx
# COMPLEX_CONSTRAIN_BOOLEAN
			push %rax
			cmp $0, %rdx
			setg %al
			movsx %al, %rdx
			pop %rax
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rsi
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rdi
			mov 0(%rdi), %rdi
			movq -16(%rdi), %rbx
			addq $1, %rbx
			imul $-1, %rbx
			mov -80(%rdi, %rbx, 8), %r8
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_SUB_CONST
			sub $1, %r8
# INSTRUCTION_MINUS
			sub %rsi, %r8
# COMPLEX_ABS_VALUE
			movq %r8, %rsi
			sar $63, %rsi
			addq %rsi, %r8
			xor %r8, %rsi
# COMPLEX_CONSTRAIN_BOOLEAN
			push %rax
			cmp $0, %rsi
			setg %al
			movsx %al, %rsi
			pop %rax
# INSTRUCTION_CONST
			mov $1, %rbx
# INSTRUCTION_MINUS
			sub %rsi, %rbx
# INSTRUCTION_AND
			and %rdx, %rbx
# INSTRUCTION_CONST
			mov $1, %rdx
# INSTRUCTION_CMP
			cmp %rbx, %rdx
# INSTRUCTION_JE
			je if_2_begin
# INSTRUCTION_JMP
			jmp if_2_end
# INSTRUCTION_LABEL
			if_2_begin:
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -32(%rbp), %r8
# METADATA_ACCESS_VARIABLE_END
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rdx
			mov 0(%rdx), %rdx
			movq -16(%rdx), %rsi
			addq $1, %rsi
			imul $-1, %rsi
			mov -32(%rdx, %rsi, 8), %rdi
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rbx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
			imul $8, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%rdi, %rbx, 1), %rdi
# INSTRUCTION_LEA_ADD
			leaq (%r8, %rdi, 1), %r8
# COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE
			leaq staticLink, %rbx
			mov 0(%rbx), %rbx
			movq -16(%rbx), %rdx
			addq $1, %rdx
			imul $-1, %rdx
			mov %r8, -88(%rbx, %rdx, 8)
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -40(%rbp), %rdi
# METADATA_ACCESS_VARIABLE_END
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rbx
			mov 0(%rbx), %rbx
			movq -16(%rbx), %rdx
			addq $1, %rdx
			imul $-1, %rdx
			mov -24(%rbx, %rdx, 8), %rsi
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
			imul $8, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%rsi, %rdx, 1), %rsi
# INSTRUCTION_LEA_ADD
			leaq (%rdi, %rsi, 1), %rdi
# COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE
			leaq staticLink, %rbx
			mov 0(%rbx), %rbx
			movq -16(%rbx), %rdx
			addq $1, %rdx
			imul $-1, %rdx
			mov %rdi, -96(%rbx, %rdx, 8)
# INSTRUCTION_SET_ZERO
			xorq %rdx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
			mov %rdx, -56(%rbp)
# INSTRUCTION_LABEL
			while_cnd_0:
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -56(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rbx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MINUS
			sub %rdx, %rbx
# INSTRUCTION_LEA_ADD_CONST
			leaq 1(%rbx), %rbx
# COMPLEX_CONSTRAIN_BOOLEAN
			push %rax
			cmp $0, %rbx
			setg %al
			movsx %al, %rbx
			pop %rax
# INSTRUCTION_CONST
			mov $1, %rdx
# INSTRUCTION_CMP
			cmp %rbx, %rdx
# INSTRUCTION_JE
			je while_0_begin
# INSTRUCTION_JMP
			jmp while_0_end
# INSTRUCTION_LABEL
			while_0_begin:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rdx
			mov 0(%rdx), %rdx
			movq -16(%rdx), %rbx
			addq $1, %rbx
			imul $-1, %rbx
			mov -48(%rdx, %rbx, 8), %rdi
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -56(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
			imul $8, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%rdi, %rdx, 1), %rdi
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rdx
			mov 0(%rdx), %rdx
			movq -16(%rdx), %rbx
			addq $1, %rbx
			imul $-1, %rbx
			mov -40(%rdx, %rbx, 8), %rsi
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -56(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
			imul $8, %rdx
# INSTRUCTION_MOVE_TO_OFFSET
			mov %rdi, (%rsi, %rdx,1)
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -56(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_LEA_ADD_CONST
			leaq 1(%rdx), %rdx
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
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -32(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_PUSH
			push %rdx
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -40(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_PUSH
			push %rdx
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_PUSH
			push %rdx
# INSTRUCTION_FUNCTION_CALL
			call Bound__0
# INSTRUCTION_ADD_STACK_PTR
			addq $24, %rsp
# COMPLEX_RESTORE_STATIC_LINK
			leaq staticLink, %rdx
movq %rbp, 8(%rdx)
# INSTRUCTION_COPY
			mov %rax, %rdi
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rbx
			mov 0(%rbx), %rbx
			movq -16(%rbx), %rdx
			addq $1, %rdx
			imul $-1, %rdx
			mov -88(%rbx, %rdx, 8), %rsi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MINUS
			sub %rsi, %rdi
# INSTRUCTION_LEA_ADD_CONST
			leaq 1(%rdi), %rdi
# COMPLEX_CONSTRAIN_BOOLEAN
			push %rax
			cmp $0, %rdi
			setg %al
			movsx %al, %rdi
			pop %rax
# INSTRUCTION_CONST
			mov $1, %rdx
# INSTRUCTION_CMP
			cmp %rdi, %rdx
# INSTRUCTION_JE
			je if_3_begin
# INSTRUCTION_JMP
			jmp if_3_end
# INSTRUCTION_LABEL
			if_3_begin:
# INSTRUCTION_SET_ZERO
			xorq %rdi, %rdi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rbx
			mov 0(%rbx), %rbx
			movq -16(%rbx), %rdx
			addq $1, %rdx
			imul $-1, %rdx
			mov -48(%rbx, %rdx, 8), %rsi
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
			imul $8, %rdx
# INSTRUCTION_MOVE_TO_OFFSET
			mov %rdi, (%rsi, %rdx,1)
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rdi
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rbx
			mov 0(%rbx), %rbx
			movq -16(%rbx), %rdx
			addq $1, %rdx
			imul $-1, %rdx
			mov -80(%rbx, %rdx, 8), %rsi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_SUB_CONST
			sub $1, %rsi
# INSTRUCTION_MINUS
			sub %rdi, %rsi
# COMPLEX_CONSTRAIN_BOOLEAN
			push %rax
			cmp $0, %rsi
			setg %al
			movsx %al, %rsi
			pop %rax
# INSTRUCTION_CONST
			mov $1, %rdx
# INSTRUCTION_CMP
			cmp %rsi, %rdx
# INSTRUCTION_JE
			je if_4_begin
# INSTRUCTION_JMP
			jmp if_4_end
# INSTRUCTION_LABEL
			if_4_begin:
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_LEA_ADD_CONST
			leaq 1(%rdx), %rdx
# INSTRUCTION_PUSH
			push %rdx
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -32(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_PUSH
			push %rdx
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -40(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_PUSH
			push %rdx
# INSTRUCTION_FUNCTION_CALL
			call BKnap__0
# INSTRUCTION_ADD_STACK_PTR
			addq $24, %rsp
# COMPLEX_RESTORE_STATIC_LINK
			leaq staticLink, %rdx
movq %rbp, 8(%rdx)
# INSTRUCTION_COPY
			mov %rax, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
			mov %rdx, -48(%rbp)
# INSTRUCTION_LABEL
			if_4_end:
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -32(%rbp), %r8
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rbx
			mov 0(%rbx), %rbx
			movq -16(%rbx), %rdx
			addq $1, %rdx
			imul $-1, %rdx
			mov -88(%rbx, %rdx, 8), %rsi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MINUS
			sub %rsi, %r8
# COMPLEX_CONSTRAIN_BOOLEAN
			push %rax
			cmp $0, %r8
			setg %al
			movsx %al, %r8
			pop %rax
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rdi
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rbx
			mov 0(%rbx), %rbx
			movq -16(%rbx), %rdx
			addq $1, %rdx
			imul $-1, %rdx
			mov -80(%rbx, %rdx, 8), %rsi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_SUB_CONST
			sub $1, %rsi
# INSTRUCTION_MINUS
			sub %rdi, %rsi
# COMPLEX_ABS_VALUE
			movq %rsi, %rdx
			sar $63, %rdx
			addq %rdx, %rsi
			xor %rsi, %rdx
# COMPLEX_CONSTRAIN_BOOLEAN
			push %rax
			cmp $0, %rdx
			setg %al
			movsx %al, %rdx
			pop %rax
# INSTRUCTION_CONST
			mov $1, %rbx
# INSTRUCTION_MINUS
			sub %rdx, %rbx
# INSTRUCTION_AND
			and %r8, %rbx
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
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -32(%rbp), %rsi
# METADATA_ACCESS_VARIABLE_END
# COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE
			leaq staticLink, %rbx
			mov 0(%rbx), %rbx
			movq -16(%rbx), %rdx
			addq $1, %rdx
			imul $-1, %rdx
			mov %rsi, -88(%rbx, %rdx, 8)
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -40(%rbp), %rsi
# METADATA_ACCESS_VARIABLE_END
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
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -56(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rbx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MINUS
			sub %rdx, %rbx
# INSTRUCTION_LEA_ADD_CONST
			leaq 1(%rbx), %rbx
# COMPLEX_CONSTRAIN_BOOLEAN
			push %rax
			cmp $0, %rbx
			setg %al
			movsx %al, %rbx
			pop %rax
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
			mov -48(%rbx, %rdx, 8), %rdi
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -56(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
			imul $8, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%rdi, %rdx, 1), %rdi
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rbx
			mov 0(%rbx), %rbx
			movq -16(%rbx), %rdx
			addq $1, %rdx
			imul $-1, %rdx
			mov -40(%rbx, %rdx, 8), %rsi
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -56(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
			imul $8, %rdx
# INSTRUCTION_MOVE_TO_OFFSET
			mov %rdi, (%rsi, %rdx,1)
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -56(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_LEA_ADD_CONST
			leaq 1(%rdx), %rdx
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
			mov %rbp,%rsp
pop %rbp
ret
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
# INSTRUCTION_FUNCTION_LABEL
.type Bound__0, @function
Bound__0:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
		subq $88, %rsp
		movq %rax, -8(%rbp)
		movq $0, -16(%rbp)
		pushq %r8
		leaq staticLink, %r8
		movq %rbp, 8(%r8)
		popq %r8
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
# METADATA_FUNCTION_ARGUMENT
			mov 16(%rbp), %rsi
			mov %rsi, -40(%rbp)
# METADATA_FUNCTION_ARGUMENT
			mov 24(%rbp), %rsi
			mov %rsi, -32(%rbp)
# METADATA_FUNCTION_ARGUMENT
			mov 32(%rbp), %rsi
			mov %rsi, -24(%rbp)
# VAR counter
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rbx
# METADATA_ACCESS_VARIABLE_END
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
			mov %rbx, -48(%rbp)
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -32(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
			mov %rdx, -56(%rbp)
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -40(%rbp), %rbx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_LEA_ADD_CONST
			leaq 1(%rbx), %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
			mov %rbx, -64(%rbp)
# INSTRUCTION_LABEL
			while_cnd_2:
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -64(%rbp), %rdi
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rdx
			mov 0(%rdx), %rdx
			movq -16(%rdx), %rbx
			addq $1, %rbx
			imul $-1, %rbx
			mov -80(%rdx, %rbx, 8), %rsi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MINUS
			sub %rdi, %rsi
# COMPLEX_CONSTRAIN_BOOLEAN
			push %rax
			cmp $0, %rsi
			setg %al
			movsx %al, %rsi
			pop %rax
# INSTRUCTION_CONST
			mov $1, %rdx
# INSTRUCTION_CMP
			cmp %rsi, %rdx
# INSTRUCTION_JE
			je while_2_begin
# INSTRUCTION_JMP
			jmp while_2_end
# INSTRUCTION_LABEL
			while_2_begin:
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -56(%rbp), %r8
# METADATA_ACCESS_VARIABLE_END
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rbx
			mov 0(%rbx), %rbx
			movq -16(%rbx), %rdx
			addq $1, %rdx
			imul $-1, %rdx
			mov -24(%rbx, %rdx, 8), %rsi
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -64(%rbp), %rbx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
			imul $8, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%rsi, %rbx, 1), %rsi
# INSTRUCTION_LEA_ADD
			leaq (%r8, %rsi, 1), %r8
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
			mov %r8, -56(%rbp)
# INSTRUCTION_COPY
			mov %r8, %rsi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rbx
			mov 0(%rbx), %rbx
			movq -16(%rbx), %rdx
			addq $1, %rdx
			imul $-1, %rdx
			mov -72(%rbx, %rdx, 8), %rdi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MINUS
			sub %rsi, %rdi
# COMPLEX_CONSTRAIN_BOOLEAN
			push %rax
			cmp $0, %rdi
			setg %al
			movsx %al, %rdi
			pop %rax
# INSTRUCTION_CONST
			mov $1, %rsi
# INSTRUCTION_CMP
			cmp %rdi, %rsi
# INSTRUCTION_JE
			je if_6_begin
# INSTRUCTION_JMP
			jmp el_6_begin
# INSTRUCTION_LABEL
			if_6_begin:
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -48(%rbp), %r8
# METADATA_ACCESS_VARIABLE_END
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rbx
			mov 0(%rbx), %rbx
			movq -16(%rbx), %rdi
			addq $1, %rdi
			imul $-1, %rdi
			mov -32(%rbx, %rdi, 8), %rdx
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -64(%rbp), %rsi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
			imul $8, %rsi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%rdx, %rsi, 1), %rdx
# INSTRUCTION_LEA_ADD
			leaq (%r8, %rdx, 1), %r8
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
			mov %r8, -48(%rbp)
# INSTRUCTION_JMP
			jmp ifel_6_end
# INSTRUCTION_LABEL
			el_6_begin:
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -48(%rbp), %rbx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_CONST
			mov $1, %r11
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -56(%rbp), %r8
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %r10
			mov 0(%r10), %r10
			movq -16(%r10), %rdx
			addq $1, %rdx
			imul $-1, %rdx
			mov -72(%r10, %rdx, 8), %rsi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MINUS
			sub %rsi, %r8
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rdi
			mov 0(%rdi), %rdi
			movq -16(%rdi), %rdx
			addq $1, %rdx
			imul $-1, %rdx
			mov -24(%rdi, %rdx, 8), %r9
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -64(%rbp), %rsi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
			imul $8, %rsi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%r9, %rsi, 1), %r9
# INSTRUCTION_DIV
			movq %r8, %rax
			pushq %r10
			movq %r9, %r10
			pushq %rdx
			cqto
			idiv %r10
			popq %rdx
			popq %r10
			movq %rax, %r9
# INSTRUCTION_MINUS
			sub %r9, %r11
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rdi
			mov 0(%rdi), %rdi
			movq -16(%rdi), %rsi
			addq $1, %rsi
			imul $-1, %rsi
			mov -32(%rdi, %rsi, 8), %rdx
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -64(%rbp), %r8
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
			imul $8, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%rdx, %r8, 1), %rdx
# INSTRUCTION_MUL
			imul %rdx, %r11
# INSTRUCTION_LEA_ADD
			leaq (%rbx, %r11, 1), %rbx
# INSTRUCTION_RETURN
			mov %rbx, %rax
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
			mov %rbp,%rsp
pop %rbp
ret
# INSTRUCTION_LABEL
			ifel_6_end:
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -64(%rbp), %rdi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_LEA_ADD_CONST
			leaq 1(%rdi), %rdi
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
			mov %rdi, -64(%rbp)
# INSTRUCTION_JMP
			jmp while_cnd_2
# INSTRUCTION_LABEL
			while_2_end:
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -48(%rbp), %rbx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_RETURN
			mov %rbx, %rax
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
			mov %rbp,%rsp
pop %rbp
ret
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
# INSTRUCTION_FUNCTION_LABEL
.type NextRand__0, @function
NextRand__0:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
		subq $80, %rsp
		movq %rax, -8(%rbp)
		movq $0, -16(%rbp)
		pushq %rbx
		leaq staticLink, %rbx
		movq %rbp, 8(%rbx)
		popq %rbx
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
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rbx
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -32(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL
			imul %rdx, %rbx
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -40(%rbp), %rdi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_LEA_ADD
			leaq (%rbx, %rdi, 1), %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
			mov %rbx, -56(%rbp)
# INSTRUCTION_COPY
			mov %rbx, %rsi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -56(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -48(%rbp), %rbx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_DIV
			movq %rdx, %rax
			pushq %r10
			movq %rbx, %r10
			pushq %rdx
			cqto
			idiv %r10
			popq %rdx
			popq %r10
			movq %rax, %rbx
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -48(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL
			imul %rdx, %rbx
# INSTRUCTION_MINUS
			sub %rbx, %rsi
# INSTRUCTION_RETURN
			mov %rsi, %rax
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
			mov %rbp,%rsp
pop %rbp
ret
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
# INSTRUCTION_FUNCTION_LABEL
.type exchange__0, @function
exchange__0:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
		subq $72, %rsp
		movq %rax, -8(%rbp)
		movq $0, -16(%rbp)
		pushq %rbx
		leaq staticLink, %rbx
		movq %rbp, 8(%rbx)
		popq %rbx
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
# METADATA_FUNCTION_ARGUMENT
			mov 16(%rbp), %rdx
			mov %rdx, -32(%rbp)
# METADATA_FUNCTION_ARGUMENT
			mov 24(%rbp), %rdx
			mov %rdx, -24(%rbp)
# VAR w_temp
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rsi
			mov 0(%rsi), %rsi
			movq -16(%rsi), %rdx
			addq $1, %rdx
			imul $-1, %rdx
			mov -32(%rsi, %rdx, 8), %rdi
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rbx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
			imul $8, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%rdi, %rbx, 1), %rdi
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
			mov %rdi, -40(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rsi
			mov 0(%rsi), %rsi
			movq -16(%rsi), %rbx
			addq $1, %rbx
			imul $-1, %rbx
			mov -32(%rsi, %rbx, 8), %r8
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -32(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
			imul $8, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%r8, %rdx, 1), %r8
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rsi
			mov 0(%rsi), %rsi
			movq -16(%rsi), %rdi
			addq $1, %rdi
			imul $-1, %rdi
			mov -32(%rsi, %rdi, 8), %rdx
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rbx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
			imul $8, %rbx
# INSTRUCTION_MOVE_TO_OFFSET
			mov %r8, (%rdx, %rbx,1)
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -40(%rbp), %rsi
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rdx
			mov 0(%rdx), %rdx
			movq -16(%rdx), %rbx
			addq $1, %rbx
			imul $-1, %rbx
			mov -32(%rdx, %rbx, 8), %r8
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -32(%rbp), %rdi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
			imul $8, %rdi
# INSTRUCTION_MOVE_TO_OFFSET
			mov %rsi, (%r8, %rdi,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rbx
			mov 0(%rbx), %rbx
			movq -16(%rbx), %r11
			addq $1, %r11
			imul $-1, %r11
			mov -24(%rbx, %r11, 8), %rdx
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %r8
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
			imul $8, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%rdx, %r8, 1), %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
			mov %rdx, -48(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rdx
			mov 0(%rdx), %rdx
			movq -16(%rdx), %r9
			addq $1, %r9
			imul $-1, %r9
			mov -24(%rdx, %r9, 8), %r10
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -32(%rbp), %rdi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
			imul $8, %rdi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%r10, %rdi, 1), %r10
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %r8
			mov 0(%r8), %r8
			movq -16(%r8), %rdx
			addq $1, %rdx
			imul $-1, %rdx
			mov -24(%r8, %rdx, 8), %rsi
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rdi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
			imul $8, %rdi
# INSTRUCTION_MOVE_TO_OFFSET
			mov %r10, (%rsi, %rdi,1)
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -48(%rbp), %r8
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rdx
			mov 0(%rdx), %rdx
			movq -16(%rdx), %rdi
			addq $1, %rdi
			imul $-1, %rdi
			mov -24(%rdx, %rdi, 8), %rbx
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -32(%rbp), %rsi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
			imul $8, %rsi
# INSTRUCTION_MOVE_TO_OFFSET
			mov %r8, (%rbx, %rsi,1)
# INSTRUCTION_CONST
			mov $1, %rsi
# INSTRUCTION_RETURN
			mov %rsi, %rax
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
			mov %rbp,%rsp
pop %rbp
ret
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
# INSTRUCTION_FUNCTION_LABEL
.type quicksort__0, @function
quicksort__0:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
		subq $72, %rsp
		movq %rax, -8(%rbp)
		movq $0, -16(%rbp)
		pushq %rdi
		leaq staticLink, %rdi
		movq %rbp, 8(%rdi)
		popq %rdi
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
# METADATA_FUNCTION_ARGUMENT
			mov 16(%rbp), %rsi
			mov %rsi, -32(%rbp)
# METADATA_FUNCTION_ARGUMENT
			mov 24(%rbp), %rsi
			mov %rsi, -24(%rbp)
# VAR dummy
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -32(%rbp), %rbx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MINUS
			sub %rdx, %rbx
# COMPLEX_CONSTRAIN_BOOLEAN
			push %rax
			cmp $0, %rbx
			setg %al
			movsx %al, %rbx
			pop %rax
# INSTRUCTION_CONST
			mov $1, %rdx
# INSTRUCTION_CMP
			cmp %rbx, %rdx
# INSTRUCTION_JE
			je if_7_begin
# INSTRUCTION_JMP
			jmp if_7_end
# INSTRUCTION_LABEL
			if_7_begin:
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %r8
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_PUSH
			push %r8
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -32(%rbp), %rsi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_PUSH
			push %rsi
# INSTRUCTION_FUNCTION_CALL
			call partition__0
# INSTRUCTION_ADD_STACK_PTR
			addq $16, %rsp
# COMPLEX_RESTORE_STATIC_LINK
			leaq staticLink, %rbx
movq %rbp, 8(%rbx)
# INSTRUCTION_COPY
			mov %rax, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
			mov %rdx, -40(%rbp)
# INSTRUCTION_COPY
			mov %r8, %rbx
# INSTRUCTION_PUSH
			push %rbx
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -40(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_SUB_CONST
			sub $1, %rdx
# INSTRUCTION_PUSH
			push %rdx
# INSTRUCTION_FUNCTION_CALL
			call quicksort__0
# INSTRUCTION_ADD_STACK_PTR
			addq $16, %rsp
# COMPLEX_RESTORE_STATIC_LINK
			leaq staticLink, %rdi
movq %rbp, 8(%rdi)
# INSTRUCTION_COPY
			mov %rax, %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
			mov %rbx, -48(%rbp)
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -40(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_LEA_ADD_CONST
			leaq 1(%rdx), %rdx
# INSTRUCTION_PUSH
			push %rdx
# INSTRUCTION_COPY
			mov %rsi, %r8
# INSTRUCTION_PUSH
			push %r8
# INSTRUCTION_FUNCTION_CALL
			call quicksort__0
# INSTRUCTION_ADD_STACK_PTR
			addq $16, %rsp
# COMPLEX_RESTORE_STATIC_LINK
			leaq staticLink, %rdx
movq %rbp, 8(%rdx)
# INSTRUCTION_COPY
			mov %rax, %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
			mov %rbx, -48(%rbp)
# INSTRUCTION_LABEL
			if_7_end:
# INSTRUCTION_CONST
			mov $1, %rdi
# INSTRUCTION_RETURN
			mov %rdi, %rax
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
			mov %rbp,%rsp
pop %rbp
ret
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
# INSTRUCTION_FUNCTION_LABEL
.type partition__0, @function
partition__0:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
		subq $96, %rsp
		movq %rax, -8(%rbp)
		movq $0, -16(%rbp)
		pushq %rdx
		leaq staticLink, %rdx
		movq %rbp, 8(%rdx)
		popq %rdx
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
# METADATA_FUNCTION_ARGUMENT
			mov 16(%rbp), %rbx
			mov %rbx, -32(%rbp)
# METADATA_FUNCTION_ARGUMENT
			mov 24(%rbp), %rbx
			mov %rbx, -24(%rbp)
# VAR dummy
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %r8
			mov 0(%r8), %r8
			movq -16(%r8), %rsi
			addq $1, %rsi
			imul $-1, %rsi
			mov -32(%r8, %rsi, 8), %rdx
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -32(%rbp), %rbx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
			imul $8, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%rdx, %rbx, 1), %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
			mov %rdx, -40(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rdx
			mov 0(%rdx), %rdx
			movq -16(%rdx), %rsi
			addq $1, %rsi
			imul $-1, %rsi
			mov -24(%rdx, %rsi, 8), %rbx
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -32(%rbp), %rdi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
			imul $8, %rdi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%rbx, %rdi, 1), %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
			mov %rbx, -48(%rbp)
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_SUB_CONST
			sub $1, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
			mov %rdx, -56(%rbp)
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -24(%rbp), %r8
# METADATA_ACCESS_VARIABLE_END
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
			mov %r8, -64(%rbp)
# INSTRUCTION_LABEL
			while_cnd_3:
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -64(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -32(%rbp), %rbx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_SUB_CONST
			sub $1, %rbx
# INSTRUCTION_MINUS
			sub %rdx, %rbx
# INSTRUCTION_LEA_ADD_CONST
			leaq 1(%rbx), %rbx
# COMPLEX_CONSTRAIN_BOOLEAN
			push %rax
			cmp $0, %rbx
			setg %al
			movsx %al, %rbx
			pop %rax
# INSTRUCTION_CONST
			mov $1, %rdx
# INSTRUCTION_CMP
			cmp %rbx, %rdx
# INSTRUCTION_JE
			je while_3_begin
# INSTRUCTION_JMP
			jmp while_3_end
# INSTRUCTION_LABEL
			while_3_begin:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %r11
			mov 0(%r11), %r11
			movq -16(%r11), %r8
			addq $1, %r8
			imul $-1, %r8
			mov -32(%r11, %r8, 8), %rbx
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -64(%rbp), %rsi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
			imul $8, %rsi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%rbx, %rsi, 1), %rbx
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -48(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL
			imul %rdx, %rbx
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -40(%rbp), %r9
# METADATA_ACCESS_VARIABLE_END
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
			leaq staticLink, %rdx
			mov 0(%rdx), %rdx
			movq -16(%rdx), %rsi
			addq $1, %rsi
			imul $-1, %rsi
			mov -24(%rdx, %rsi, 8), %rdi
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -64(%rbp), %r8
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_MUL_CONST
			imul $8, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
			mov (%rdi, %r8, 1), %rdi
# INSTRUCTION_MUL
			imul %rdi, %r9
# INSTRUCTION_MINUS
			sub %r9, %rbx
# INSTRUCTION_LEA_ADD_CONST
			leaq 1(%rbx), %rbx
# COMPLEX_CONSTRAIN_BOOLEAN
			push %rax
			cmp $0, %rbx
			setg %al
			movsx %al, %rbx
			pop %rax
# INSTRUCTION_CONST
			mov $1, %rsi
# INSTRUCTION_CMP
			cmp %rbx, %rsi
# INSTRUCTION_JE
			je if_8_begin
# INSTRUCTION_JMP
			jmp if_8_end
# INSTRUCTION_LABEL
			if_8_begin:
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -56(%rbp), %r8
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_LEA_ADD_CONST
			leaq 1(%r8), %r8
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
			mov %r8, -56(%rbp)
# INSTRUCTION_COPY
			mov %r8, %rdx
# INSTRUCTION_PUSH
			push %rdx
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -64(%rbp), %rdi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_PUSH
			push %rdi
# INSTRUCTION_FUNCTION_CALL
			call exchange__0
# INSTRUCTION_ADD_STACK_PTR
			addq $16, %rsp
# COMPLEX_RESTORE_STATIC_LINK
			leaq staticLink, %rsi
movq %rbp, 8(%rsi)
# INSTRUCTION_COPY
			mov %rax, %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
			mov %rbx, -72(%rbp)
# INSTRUCTION_LABEL
			if_8_end:
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -64(%rbp), %rsi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_LEA_ADD_CONST
			leaq 1(%rsi), %rsi
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
			mov %rsi, -64(%rbp)
# INSTRUCTION_JMP
			jmp while_cnd_3
# INSTRUCTION_LABEL
			while_3_end:
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -56(%rbp), %rdi
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_LEA_ADD_CONST
			leaq 1(%rdi), %rdi
# INSTRUCTION_PUSH
			push %rdi
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -32(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_PUSH
			push %rdx
# INSTRUCTION_FUNCTION_CALL
			call exchange__0
# INSTRUCTION_ADD_STACK_PTR
			addq $16, %rsp
# COMPLEX_RESTORE_STATIC_LINK
			leaq staticLink, %rsi
movq %rbp, 8(%rsi)
# INSTRUCTION_COPY
			mov %rax, %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
			mov %rbx, -72(%rbp)
# METADATA_ACCESS_VARIABLE_START
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
			mov -56(%rbp), %rdx
# METADATA_ACCESS_VARIABLE_END
# INSTRUCTION_LEA_ADD_CONST
			leaq 1(%rdx), %rdx
# INSTRUCTION_RETURN
			mov %rdx, %rax
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
			mov %rbp,%rsp
pop %rbp
ret
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
