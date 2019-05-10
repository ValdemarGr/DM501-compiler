.section .data
staticLink:
	.space 16
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
# INSTRUCTION_FUNCTION_LABEL
.type BKnap, @function
BKnap:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
push %rax
	subq $72, %rsp
	popq %rax
	movq %rax, -8(%rbp)
	movq $0, -16(%rbp)
	leaq staticLink, %rcx
	movq %rbp, 8(%rcx)
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
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -40(%rbp), %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
	leaq staticLink, %rdi
	mov 0(%rdi), %rdi
	movq -16(%rdi), %r8
	addq $1, %r8
	imul $-1, %r8
	mov -24(%rdi, %r8, 8), %rsi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -24(%rbp), %r9
# INSTRUCTION_CONST
	mov $8, %r10
# INSTRUCTION_MUL
	imul %r10, %r9
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
	mov (%rsi, %r9,1), %rsi
# INSTRUCTION_ADD
	add %rbx, %rsi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
	leaq staticLink, %r12
	mov 0(%r12), %r12
	movq -16(%r12), %r13
	addq $1, %r13
	imul $-1, %r13
	mov -72(%r12, %r13, 8), %r11
# INSTRUCTION_MINUS
	sub %r11, %rsi
# INSTRUCTION_CONST
	mov $-1, %r14
# INSTRUCTION_ADD
	add %r14, %rsi
# COMPLEX_CONSTRAIN_BOOLEAN
	push %rdx
	cmp $0, %rsi
	seta %dl
	movsx %dl, %rsi
	pop %rdx
# INSTRUCTION_CONST
	mov $1, %r15
# INSTRUCTION_CMP
	cmp %rsi, %r15
# INSTRUCTION_JE
	je if_0_begin
# INSTRUCTION_JMP
	jmp if_0_end
# INSTRUCTION_LABEL
	if_0_begin:
# INSTRUCTION_CONST
	mov $1, %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
	leaq staticLink, %rbx
	mov 0(%rbx), %rbx
	movq -16(%rbx), %rsi
	addq $1, %rsi
	imul $-1, %rsi
	mov -48(%rbx, %rsi, 8), %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -24(%rbp), %rdi
# INSTRUCTION_CONST
	mov $8, %r8
# INSTRUCTION_MUL
	imul %r8, %rdi
# INSTRUCTION_MOVE_TO_OFFSET
	mov %rcx, (%rdx, %rdi,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -24(%rbp), %r9
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
	leaq staticLink, %r11
	mov 0(%r11), %r11
	movq -16(%r11), %r12
	addq $1, %r12
	imul $-1, %r12
	mov -80(%r11, %r12, 8), %r10
# INSTRUCTION_CONST
	mov $1, %r13
# INSTRUCTION_MINUS
	sub %r13, %r10
# INSTRUCTION_MINUS
	sub %r10, %r9
# COMPLEX_CONSTRAIN_BOOLEAN
	push %rdx
	cmp $0, %r9
	seta %dl
	movsx %dl, %r9
	pop %rdx
# INSTRUCTION_CONST
	mov $1, %r14
# INSTRUCTION_CMP
	cmp %r9, %r14
# INSTRUCTION_JE
	je if_1_begin
# INSTRUCTION_JMP
	jmp if_1_end
# INSTRUCTION_LABEL
	if_1_begin:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -24(%rbp), %r15
# INSTRUCTION_CONST
	mov $1, %rcx
# INSTRUCTION_ADD
	add %r15, %rcx
# INSTRUCTION_PUSH
	push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -32(%rbp), %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
	leaq staticLink, %rsi
	mov 0(%rsi), %rsi
	movq -16(%rsi), %rdi
	addq $1, %rdi
	imul $-1, %rdi
	mov -32(%rsi, %rdi, 8), %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -24(%rbp), %r8
# INSTRUCTION_CONST
	mov $8, %r9
# INSTRUCTION_MUL
	imul %r9, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
	mov (%rbx, %r8,1), %rbx
# INSTRUCTION_ADD
	add %rdx, %rbx
# INSTRUCTION_PUSH
	push %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -40(%rbp), %r10
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
	leaq staticLink, %r12
	mov 0(%r12), %r12
	movq -16(%r12), %r13
	addq $1, %r13
	imul $-1, %r13
	mov -24(%r12, %r13, 8), %r11
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -24(%rbp), %r14
# INSTRUCTION_CONST
	mov $8, %r15
# INSTRUCTION_MUL
	imul %r15, %r14
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
	mov (%r11, %r14,1), %r11
# INSTRUCTION_ADD
	add %r10, %r11
# INSTRUCTION_PUSH
	push %r11
# INSTRUCTION_FUNCTION_CALL
	call BKnap
# COMPLEX_RESTORE_STATIC_LINK
	leaq staticLink, %rcx
movq %rbp, 8(%rcx)
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
	mov %rax, -48(%rbp)
# INSTRUCTION_LABEL
	if_1_end:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -32(%rbp), %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
	leaq staticLink, %rsi
	mov 0(%rsi), %rsi
	movq -16(%rsi), %rdi
	addq $1, %rdi
	imul $-1, %rdi
	mov -32(%rsi, %rdi, 8), %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -24(%rbp), %r8
# INSTRUCTION_CONST
	mov $8, %r9
# INSTRUCTION_MUL
	imul %r9, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
	mov (%rbx, %r8,1), %rbx
# INSTRUCTION_ADD
	add %rdx, %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
	leaq staticLink, %r11
	mov 0(%r11), %r11
	movq -16(%r11), %r12
	addq $1, %r12
	imul $-1, %r12
	mov -88(%r11, %r12, 8), %r10
# INSTRUCTION_MINUS
	sub %rbx, %r10
# COMPLEX_CONSTRAIN_BOOLEAN
	push %rdx
	cmp $0, %r10
	seta %dl
	movsx %dl, %r10
	pop %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -24(%rbp), %r13
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
	leaq staticLink, %r15
	mov 0(%r15), %r15
	movq -16(%r15), %rcx
	addq $1, %rcx
	imul $-1, %rcx
	mov -80(%r15, %rcx, 8), %r14
# INSTRUCTION_CONST
	mov $1, %rdx
# INSTRUCTION_MINUS
	sub %rdx, %r14
# INSTRUCTION_MINUS
	sub %r13, %r14
# INSTRUCTION_CONST
	mov $1, %rbx
# INSTRUCTION_MINUS
	sub %r14, %rbx
# INSTRUCTION_AND
	and %r10, %rbx
# INSTRUCTION_CONST
	mov $1, %rsi
# INSTRUCTION_CMP
	cmp %rbx, %rsi
# INSTRUCTION_JE
	je if_2_begin
# INSTRUCTION_JMP
	jmp if_2_end
# INSTRUCTION_LABEL
	if_2_begin:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -32(%rbp), %rdi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
	leaq staticLink, %r9
	mov 0(%r9), %r9
	movq -16(%r9), %r10
	addq $1, %r10
	imul $-1, %r10
	mov -32(%r9, %r10, 8), %r8
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -24(%rbp), %r11
# INSTRUCTION_CONST
	mov $8, %r12
# INSTRUCTION_MUL
	imul %r12, %r11
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
	mov (%r8, %r11,1), %r8
# INSTRUCTION_ADD
	add %rdi, %r8
# COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE
	leaq staticLink, %r13
	mov 0(%r13), %r13
	movq -16(%r13), %r14
	addq $1, %r14
	imul $-1, %r14
	mov %r8, -88(%r13, %r14, 8)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -40(%rbp), %r15
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
	leaq staticLink, %rdx
	mov 0(%rdx), %rdx
	movq -16(%rdx), %rbx
	addq $1, %rbx
	imul $-1, %rbx
	mov -24(%rdx, %rbx, 8), %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -24(%rbp), %rsi
# INSTRUCTION_CONST
	mov $8, %rdi
# INSTRUCTION_MUL
	imul %rdi, %rsi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
	mov (%rcx, %rsi,1), %rcx
# INSTRUCTION_ADD
	add %r15, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE
	leaq staticLink, %r8
	mov 0(%r8), %r8
	movq -16(%r8), %r9
	addq $1, %r9
	imul $-1, %r9
	mov %rcx, -96(%r8, %r9, 8)
# INSTRUCTION_CONST
	mov $0, %r10
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
	mov %r10, -56(%rbp)
# INSTRUCTION_LABEL
	while_cnd_0:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -56(%rbp), %r11
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -24(%rbp), %r12
# INSTRUCTION_MINUS
	sub %r12, %r11
# INSTRUCTION_CONST
	mov $-1, %r13
# INSTRUCTION_ADD
	add %r13, %r11
# COMPLEX_CONSTRAIN_BOOLEAN
	push %rdx
	cmp $0, %r11
	seta %dl
	movsx %dl, %r11
	pop %rdx
# INSTRUCTION_CONST
	mov $1, %r14
# INSTRUCTION_CMP
	cmp %r11, %r14
# INSTRUCTION_JE
	je while_0_begin
# INSTRUCTION_JMP
	jmp while_0_end
# INSTRUCTION_LABEL
	while_0_begin:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
	leaq staticLink, %rcx
	mov 0(%rcx), %rcx
	movq -16(%rcx), %rdx
	addq $1, %rdx
	imul $-1, %rdx
	mov -48(%rcx, %rdx, 8), %r15
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -56(%rbp), %rbx
# INSTRUCTION_CONST
	mov $8, %rsi
# INSTRUCTION_MUL
	imul %rsi, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
	mov (%r15, %rbx,1), %r15
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
	leaq staticLink, %r8
	mov 0(%r8), %r8
	movq -16(%r8), %r9
	addq $1, %r9
	imul $-1, %r9
	mov -40(%r8, %r9, 8), %rdi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -56(%rbp), %r10
# INSTRUCTION_CONST
	mov $8, %r11
# INSTRUCTION_MUL
	imul %r11, %r10
# INSTRUCTION_MOVE_TO_OFFSET
	mov %r15, (%rdi, %r10,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -56(%rbp), %r12
# INSTRUCTION_CONST
	mov $1, %r13
# INSTRUCTION_ADD
	add %r12, %r13
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
	mov %r13, -56(%rbp)
# INSTRUCTION_JMP
	jmp while_cnd_0
# INSTRUCTION_LABEL
	while_0_end:
# INSTRUCTION_LABEL
	if_2_end:
# INSTRUCTION_LABEL
	if_0_end:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -32(%rbp), %r14
# INSTRUCTION_PUSH
	push %r14
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -40(%rbp), %r15
# INSTRUCTION_PUSH
	push %r15
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -24(%rbp), %rcx
# INSTRUCTION_PUSH
	push %rcx
# INSTRUCTION_FUNCTION_CALL
	call Bound
# COMPLEX_RESTORE_STATIC_LINK
	leaq staticLink, %rdx
movq %rbp, 8(%rdx)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
	leaq staticLink, %rsi
	mov 0(%rsi), %rsi
	movq -16(%rsi), %rdi
	addq $1, %rdi
	imul $-1, %rdi
	mov -88(%rsi, %rdi, 8), %rbx
# INSTRUCTION_MINUS
	sub %rax, %rbx
# INSTRUCTION_CONST
	mov $-1, %r8
# INSTRUCTION_ADD
	add %r8, %rbx
# COMPLEX_CONSTRAIN_BOOLEAN
	push %rdx
	cmp $0, %rbx
	seta %dl
	movsx %dl, %rbx
	pop %rdx
# INSTRUCTION_CONST
	mov $1, %r9
# INSTRUCTION_CMP
	cmp %rbx, %r9
# INSTRUCTION_JE
	je if_3_begin
# INSTRUCTION_JMP
	jmp if_3_end
# INSTRUCTION_LABEL
	if_3_begin:
# INSTRUCTION_CONST
	mov $0, %r10
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
	leaq staticLink, %r12
	mov 0(%r12), %r12
	movq -16(%r12), %r13
	addq $1, %r13
	imul $-1, %r13
	mov -48(%r12, %r13, 8), %r11
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -24(%rbp), %r14
# INSTRUCTION_CONST
	mov $8, %r15
# INSTRUCTION_MUL
	imul %r15, %r14
# INSTRUCTION_MOVE_TO_OFFSET
	mov %r10, (%r11, %r14,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -24(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
	leaq staticLink, %rbx
	mov 0(%rbx), %rbx
	movq -16(%rbx), %rsi
	addq $1, %rsi
	imul $-1, %rsi
	mov -80(%rbx, %rsi, 8), %rdx
# INSTRUCTION_CONST
	mov $1, %rdi
# INSTRUCTION_MINUS
	sub %rdi, %rdx
# INSTRUCTION_MINUS
	sub %rdx, %rcx
# COMPLEX_CONSTRAIN_BOOLEAN
	push %rdx
	cmp $0, %rcx
	seta %dl
	movsx %dl, %rcx
	pop %rdx
# INSTRUCTION_CONST
	mov $1, %r8
# INSTRUCTION_CMP
	cmp %rcx, %r8
# INSTRUCTION_JE
	je if_4_begin
# INSTRUCTION_JMP
	jmp if_4_end
# INSTRUCTION_LABEL
	if_4_begin:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -24(%rbp), %r9
# INSTRUCTION_CONST
	mov $1, %r10
# INSTRUCTION_ADD
	add %r9, %r10
# INSTRUCTION_PUSH
	push %r10
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -32(%rbp), %r11
# INSTRUCTION_PUSH
	push %r11
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -40(%rbp), %r12
# INSTRUCTION_PUSH
	push %r12
# INSTRUCTION_FUNCTION_CALL
	call BKnap
# COMPLEX_RESTORE_STATIC_LINK
	leaq staticLink, %r13
movq %rbp, 8(%r13)
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
	mov %rax, -48(%rbp)
# INSTRUCTION_LABEL
	if_4_end:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -32(%rbp), %r14
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
	leaq staticLink, %rcx
	mov 0(%rcx), %rcx
	movq -16(%rcx), %rdx
	addq $1, %rdx
	imul $-1, %rdx
	mov -88(%rcx, %rdx, 8), %r15
# INSTRUCTION_MINUS
	sub %r14, %r15
# COMPLEX_CONSTRAIN_BOOLEAN
	push %rdx
	cmp $0, %r15
	seta %dl
	movsx %dl, %r15
	pop %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -24(%rbp), %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
	leaq staticLink, %rdi
	mov 0(%rdi), %rdi
	movq -16(%rdi), %r8
	addq $1, %r8
	imul $-1, %r8
	mov -80(%rdi, %r8, 8), %rsi
# INSTRUCTION_CONST
	mov $1, %r9
# INSTRUCTION_MINUS
	sub %r9, %rsi
# INSTRUCTION_MINUS
	sub %rbx, %rsi
# INSTRUCTION_CONST
	mov $1, %r10
# INSTRUCTION_MINUS
	sub %rsi, %r10
# INSTRUCTION_AND
	and %r15, %r10
# INSTRUCTION_CONST
	mov $1, %r11
# INSTRUCTION_CMP
	cmp %r10, %r11
# INSTRUCTION_JE
	je if_5_begin
# INSTRUCTION_JMP
	jmp if_5_end
# INSTRUCTION_LABEL
	if_5_begin:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -32(%rbp), %r12
# COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE
	leaq staticLink, %r13
	mov 0(%r13), %r13
	movq -16(%r13), %r14
	addq $1, %r14
	imul $-1, %r14
	mov %r12, -88(%r13, %r14, 8)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -40(%rbp), %r15
# COMPLEX_MOVE_TEMPORARY_INTO_STACK_IN_SCOPE
	leaq staticLink, %rcx
	mov 0(%rcx), %rcx
	movq -16(%rcx), %rdx
	addq $1, %rdx
	imul $-1, %rdx
	mov %r15, -96(%rcx, %rdx, 8)
# INSTRUCTION_CONST
	mov $0, %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
	mov %rbx, -56(%rbp)
# INSTRUCTION_LABEL
	while_cnd_1:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -56(%rbp), %rsi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -24(%rbp), %rdi
# INSTRUCTION_MINUS
	sub %rdi, %rsi
# INSTRUCTION_CONST
	mov $-1, %r8
# INSTRUCTION_ADD
	add %r8, %rsi
# COMPLEX_CONSTRAIN_BOOLEAN
	push %rdx
	cmp $0, %rsi
	seta %dl
	movsx %dl, %rsi
	pop %rdx
# INSTRUCTION_CONST
	mov $1, %r9
# INSTRUCTION_CMP
	cmp %rsi, %r9
# INSTRUCTION_JE
	je while_1_begin
# INSTRUCTION_JMP
	jmp while_1_end
# INSTRUCTION_LABEL
	while_1_begin:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
	leaq staticLink, %r11
	mov 0(%r11), %r11
	movq -16(%r11), %r12
	addq $1, %r12
	imul $-1, %r12
	mov -48(%r11, %r12, 8), %r10
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -56(%rbp), %r13
# INSTRUCTION_CONST
	mov $8, %r14
# INSTRUCTION_MUL
	imul %r14, %r13
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
	mov (%r10, %r13,1), %r10
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
	leaq staticLink, %rcx
	mov 0(%rcx), %rcx
	movq -16(%rcx), %rdx
	addq $1, %rdx
	imul $-1, %rdx
	mov -40(%rcx, %rdx, 8), %r15
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -56(%rbp), %rbx
# INSTRUCTION_CONST
	mov $8, %rsi
# INSTRUCTION_MUL
	imul %rsi, %rbx
# INSTRUCTION_MOVE_TO_OFFSET
	mov %r10, (%r15, %rbx,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
	mov -56(%rbp), %rdi
# INSTRUCTION_CONST
	mov $1, %r8
# INSTRUCTION_ADD
	add %rdi, %r8
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
	mov %r8, -56(%rbp)
# INSTRUCTION_JMP
	jmp while_cnd_1
# INSTRUCTION_LABEL
	while_1_end:
# INSTRUCTION_LABEL
	if_5_end:
# INSTRUCTION_LABEL
	if_3_end:
# INSTRUCTION_CONST
	mov $0, %r9
# INSTRUCTION_RETURN
	mov %r9, %rax
	mov %rbp,%rsp
pop %rbp
ret
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
# INSTRUCTION_FUNCTION_LABEL
.type Bound, @function
Bound:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
push %rax
subq $80, %rsp
popq %rax
movq %rax, -8(%rbp)
movq $0, -16(%rbp)
leaq staticLink, %rbx
movq %rbp, 8(%rbx)
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
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -24(%rbp), %rdi
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %rdi, -48(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -32(%rbp), %r8
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %r8, -56(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -40(%rbp), %r9
# INSTRUCTION_CONST
mov $1, %r10
# INSTRUCTION_ADD
add %r9, %r10
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %r10, -64(%rbp)
# INSTRUCTION_LABEL
while_cnd_2:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -64(%rbp), %r11
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
leaq staticLink, %r13
mov 0(%r13), %r13
movq -16(%r13), %r14
addq $1, %r14
imul $-1, %r14
mov -80(%r13, %r14, 8), %r12
# INSTRUCTION_MINUS
sub %r12, %r11
# COMPLEX_CONSTRAIN_BOOLEAN
push %rdx
cmp $0, %r11
seta %dl
movsx %dl, %r11
pop %rdx
# INSTRUCTION_CONST
mov $1, %r15
# INSTRUCTION_CMP
cmp %r11, %r15
# INSTRUCTION_JE
je while_2_begin
# INSTRUCTION_JMP
jmp while_2_end
# INSTRUCTION_LABEL
while_2_begin:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -56(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
leaq staticLink, %rbx
mov 0(%rbx), %rbx
movq -16(%rbx), %rsi
addq $1, %rsi
imul $-1, %rsi
mov -24(%rbx, %rsi, 8), %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -64(%rbp), %rdi
# INSTRUCTION_CONST
mov $8, %r8
# INSTRUCTION_MUL
imul %r8, %rdi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
mov (%rdx, %rdi,1), %rdx
# INSTRUCTION_ADD
add %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %rdx, -56(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -56(%rbp), %r9
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
leaq staticLink, %r11
mov 0(%r11), %r11
movq -16(%r11), %r12
addq $1, %r12
imul $-1, %r12
mov -72(%r11, %r12, 8), %r10
# INSTRUCTION_MINUS
sub %r10, %r9
# COMPLEX_CONSTRAIN_BOOLEAN
push %rdx
cmp $0, %r9
seta %dl
movsx %dl, %r9
pop %rdx
# INSTRUCTION_CONST
mov $1, %r13
# INSTRUCTION_CMP
cmp %r9, %r13
# INSTRUCTION_JE
je if_6_begin
# INSTRUCTION_JMP
jmp el_6_begin
# INSTRUCTION_LABEL
if_6_begin:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -48(%rbp), %r14
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
leaq staticLink, %rcx
mov 0(%rcx), %rcx
movq -16(%rcx), %rdx
addq $1, %rdx
imul $-1, %rdx
mov -32(%rcx, %rdx, 8), %r15
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -64(%rbp), %rbx
# INSTRUCTION_CONST
mov $8, %rsi
# INSTRUCTION_MUL
imul %rsi, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
mov (%r15, %rbx,1), %r15
# INSTRUCTION_ADD
add %r14, %r15
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %r15, -48(%rbp)
# INSTRUCTION_JMP
jmp ifel_6_end
# INSTRUCTION_LABEL
el_6_begin:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -48(%rbp), %rdi
# INSTRUCTION_CONST
mov $1, %r8
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -56(%rbp), %r9
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
leaq staticLink, %r11
mov 0(%r11), %r11
movq -16(%r11), %r12
addq $1, %r12
imul $-1, %r12
mov -72(%r11, %r12, 8), %r10
# INSTRUCTION_MINUS
sub %r10, %r9
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
leaq staticLink, %r14
mov 0(%r14), %r14
movq -16(%r14), %r15
addq $1, %r15
imul $-1, %r15
mov -24(%r14, %r15, 8), %r13
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -64(%rbp), %rcx
# INSTRUCTION_CONST
mov $8, %rdx
# INSTRUCTION_MUL
imul %rdx, %rcx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
mov (%r13, %rcx,1), %r13
# INSTRUCTION_DIV
movq %r9, %rax
pushq %r10
movq %r13, %r10
cqto
idiv %r10
popq %r10
movq %rax, %r13
# INSTRUCTION_MINUS
sub %r13, %r8
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
leaq staticLink, %rsi
mov 0(%rsi), %rsi
movq -16(%rsi), %rdi
addq $1, %rdi
imul $-1, %rdi
mov -32(%rsi, %rdi, 8), %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -64(%rbp), %r8
# INSTRUCTION_CONST
mov $8, %r9
# INSTRUCTION_MUL
imul %r9, %r8
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
mov (%rbx, %r8,1), %rbx
# INSTRUCTION_MUL
imul %r8, %rbx
# INSTRUCTION_ADD
add %rdi, %rbx
# INSTRUCTION_RETURN
mov %rbx, %rax
mov %rbp,%rsp
pop %rbp
ret
# INSTRUCTION_LABEL
ifel_6_end:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -64(%rbp), %r11
# INSTRUCTION_CONST
mov $1, %r12
# INSTRUCTION_ADD
add %r11, %r12
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %r12, -64(%rbp)
# INSTRUCTION_JMP
jmp while_cnd_2
# INSTRUCTION_LABEL
while_2_end:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -48(%rbp), %r13
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
.type NextRand, @function
NextRand:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
push %rax
subq $72, %rsp
popq %rax
movq %rax, -8(%rbp)
movq $0, -16(%rbp)
leaq staticLink, %rdi
movq %rbp, 8(%rdi)
# METADATA_FUNCTION_ARGUMENT
mov 16(%rbp), %r8
mov %r8, -48(%rbp)
# METADATA_FUNCTION_ARGUMENT
mov 24(%rbp), %r8
mov %r8, -40(%rbp)
# METADATA_FUNCTION_ARGUMENT
mov 32(%rbp), %r8
mov %r8, -32(%rbp)
# METADATA_FUNCTION_ARGUMENT
mov 40(%rbp), %r8
mov %r8, -24(%rbp)
# VAR temp
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -24(%rbp), %r9
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -32(%rbp), %r10
# INSTRUCTION_MUL
imul %r9, %r10
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -40(%rbp), %r11
# INSTRUCTION_ADD
add %r10, %r11
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %r11, -56(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -56(%rbp), %r12
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -56(%rbp), %r13
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -48(%rbp), %r14
# INSTRUCTION_DIV
movq %r13, %rax
pushq %r10
movq %r14, %r10
cqto
idiv %r10
popq %r10
movq %rax, %r14
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -48(%rbp), %r15
# INSTRUCTION_MUL
imul %r14, %r15
# INSTRUCTION_MINUS
sub %r15, %r12
# INSTRUCTION_RETURN
mov %r12, %rax
mov %rbp,%rsp
pop %rbp
ret
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
# INSTRUCTION_FUNCTION_LABEL
.type exchange, @function
exchange:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
push %rax
subq $64, %rsp
popq %rax
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
# VAR w_temp
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
leaq staticLink, %r12
mov 0(%r12), %r12
movq -16(%r12), %r13
addq $1, %r13
imul $-1, %r13
mov -32(%r12, %r13, 8), %r11
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -24(%rbp), %r14
# INSTRUCTION_CONST
mov $8, %r15
# INSTRUCTION_MUL
imul %r15, %r14
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
mov (%r11, %r14,1), %r11
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %r11, -40(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
leaq staticLink, %rdx
mov 0(%rdx), %rdx
movq -16(%rdx), %rbx
addq $1, %rbx
imul $-1, %rbx
mov -32(%rdx, %rbx, 8), %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -32(%rbp), %rsi
# INSTRUCTION_CONST
mov $8, %rdi
# INSTRUCTION_MUL
imul %rdi, %rsi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
mov (%rcx, %rsi,1), %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
leaq staticLink, %r9
mov 0(%r9), %r9
movq -16(%r9), %r10
addq $1, %r10
imul $-1, %r10
mov -32(%r9, %r10, 8), %r8
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -24(%rbp), %r11
# INSTRUCTION_CONST
mov $8, %r12
# INSTRUCTION_MUL
imul %r12, %r11
# INSTRUCTION_MOVE_TO_OFFSET
mov %rcx, (%r8, %r11,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -40(%rbp), %r13
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
leaq staticLink, %r15
mov 0(%r15), %r15
movq -16(%r15), %rcx
addq $1, %rcx
imul $-1, %rcx
mov -32(%r15, %rcx, 8), %r14
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -32(%rbp), %rdx
# INSTRUCTION_CONST
mov $8, %rbx
# INSTRUCTION_MUL
imul %rbx, %rdx
# INSTRUCTION_MOVE_TO_OFFSET
mov %r13, (%r14, %rdx,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
leaq staticLink, %rdi
mov 0(%rdi), %rdi
movq -16(%rdi), %r8
addq $1, %r8
imul $-1, %r8
mov -24(%rdi, %r8, 8), %rsi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -24(%rbp), %r9
# INSTRUCTION_CONST
mov $8, %r10
# INSTRUCTION_MUL
imul %r10, %r9
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
mov (%rsi, %r9,1), %rsi
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %rsi, -48(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
leaq staticLink, %r12
mov 0(%r12), %r12
movq -16(%r12), %r13
addq $1, %r13
imul $-1, %r13
mov -24(%r12, %r13, 8), %r11
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -32(%rbp), %r14
# INSTRUCTION_CONST
mov $8, %r15
# INSTRUCTION_MUL
imul %r15, %r14
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
mov (%r11, %r14,1), %r11
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
leaq staticLink, %rdx
mov 0(%rdx), %rdx
movq -16(%rdx), %rbx
addq $1, %rbx
imul $-1, %rbx
mov -24(%rdx, %rbx, 8), %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -24(%rbp), %rsi
# INSTRUCTION_CONST
mov $8, %rdi
# INSTRUCTION_MUL
imul %rdi, %rsi
# INSTRUCTION_MOVE_TO_OFFSET
mov %r11, (%rcx, %rsi,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -48(%rbp), %r8
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
leaq staticLink, %r10
mov 0(%r10), %r10
movq -16(%r10), %r11
addq $1, %r11
imul $-1, %r11
mov -24(%r10, %r11, 8), %r9
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -32(%rbp), %r12
# INSTRUCTION_CONST
mov $8, %r13
# INSTRUCTION_MUL
imul %r13, %r12
# INSTRUCTION_MOVE_TO_OFFSET
mov %r8, (%r9, %r12,1)
# INSTRUCTION_CONST
mov $1, %r14
# INSTRUCTION_RETURN
mov %r14, %rax
mov %rbp,%rsp
pop %rbp
ret
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
# INSTRUCTION_FUNCTION_LABEL
.type quicksort, @function
quicksort:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
push %rax
subq $64, %rsp
popq %rax
movq %rax, -8(%rbp)
movq $0, -16(%rbp)
leaq staticLink, %r11
movq %rbp, 8(%r11)
# METADATA_FUNCTION_ARGUMENT
mov 16(%rbp), %r12
mov %r12, -32(%rbp)
# METADATA_FUNCTION_ARGUMENT
mov 24(%rbp), %r12
mov %r12, -24(%rbp)
# VAR dummy
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -24(%rbp), %r13
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -32(%rbp), %r14
# INSTRUCTION_MINUS
sub %r14, %r13
# COMPLEX_CONSTRAIN_BOOLEAN
push %rdx
cmp $0, %r13
seta %dl
movsx %dl, %r13
pop %rdx
# INSTRUCTION_CONST
mov $1, %r15
# INSTRUCTION_CMP
cmp %r13, %r15
# INSTRUCTION_JE
je if_7_begin
# INSTRUCTION_JMP
jmp if_7_end
# INSTRUCTION_LABEL
if_7_begin:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -24(%rbp), %rcx
# INSTRUCTION_PUSH
push %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -32(%rbp), %rdx
# INSTRUCTION_PUSH
push %rdx
# INSTRUCTION_FUNCTION_CALL
call partition
# COMPLEX_RESTORE_STATIC_LINK
leaq staticLink, %rbx
movq %rbp, 8(%rbx)
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %rax, -40(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -24(%rbp), %rsi
# INSTRUCTION_PUSH
push %rsi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -40(%rbp), %rdi
# INSTRUCTION_CONST
mov $1, %r8
# INSTRUCTION_MINUS
sub %r8, %rdi
# INSTRUCTION_PUSH
push %rdi
# INSTRUCTION_FUNCTION_CALL
call quicksort
# COMPLEX_RESTORE_STATIC_LINK
leaq staticLink, %r9
movq %rbp, 8(%r9)
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %rax, -48(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -40(%rbp), %r10
# INSTRUCTION_CONST
mov $1, %r11
# INSTRUCTION_ADD
add %r10, %r11
# INSTRUCTION_PUSH
push %r11
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -32(%rbp), %r12
# INSTRUCTION_PUSH
push %r12
# INSTRUCTION_FUNCTION_CALL
call quicksort
# COMPLEX_RESTORE_STATIC_LINK
leaq staticLink, %r13
movq %rbp, 8(%r13)
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %rax, -48(%rbp)
# INSTRUCTION_LABEL
if_7_end:
# INSTRUCTION_CONST
mov $1, %r14
# INSTRUCTION_RETURN
mov %r14, %rax
mov %rbp,%rsp
pop %rbp
ret
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
# INSTRUCTION_FUNCTION_LABEL
.type partition, @function
partition:
push %rbp
mov %rbp, %rax
mov %rsp,%rbp
push %rax
subq $88, %rsp
popq %rax
movq %rax, -8(%rbp)
movq $0, -16(%rbp)
leaq staticLink, %r13
movq %rbp, 8(%r13)
# METADATA_FUNCTION_ARGUMENT
mov 16(%rbp), %r14
mov %r14, -32(%rbp)
# METADATA_FUNCTION_ARGUMENT
mov 24(%rbp), %r14
mov %r14, -24(%rbp)
# VAR dummy
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
leaq staticLink, %rcx
mov 0(%rcx), %rcx
movq -16(%rcx), %rdx
addq $1, %rdx
imul $-1, %rdx
mov -32(%rcx, %rdx, 8), %r15
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -32(%rbp), %rbx
# INSTRUCTION_CONST
mov $8, %rsi
# INSTRUCTION_MUL
imul %rsi, %rbx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
mov (%r15, %rbx,1), %r15
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %r15, -40(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
leaq staticLink, %r8
mov 0(%r8), %r8
movq -16(%r8), %r9
addq $1, %r9
imul $-1, %r9
mov -24(%r8, %r9, 8), %rdi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -32(%rbp), %r10
# INSTRUCTION_CONST
mov $8, %r11
# INSTRUCTION_MUL
imul %r11, %r10
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
mov (%rdi, %r10,1), %rdi
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %rdi, -48(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -24(%rbp), %r12
# INSTRUCTION_CONST
mov $1, %r13
# INSTRUCTION_MINUS
sub %r13, %r12
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %r12, -56(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -24(%rbp), %r14
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %r14, -64(%rbp)
# INSTRUCTION_LABEL
while_cnd_3:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -64(%rbp), %r15
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -32(%rbp), %rcx
# INSTRUCTION_CONST
mov $1, %rdx
# INSTRUCTION_MINUS
sub %rdx, %rcx
# INSTRUCTION_MINUS
sub %rcx, %r15
# INSTRUCTION_CONST
mov $-1, %rbx
# INSTRUCTION_ADD
add %rbx, %r15
# COMPLEX_CONSTRAIN_BOOLEAN
push %rdx
cmp $0, %r15
seta %dl
movsx %dl, %r15
pop %rdx
# INSTRUCTION_CONST
mov $1, %rsi
# INSTRUCTION_CMP
cmp %r15, %rsi
# INSTRUCTION_JE
je while_3_begin
# INSTRUCTION_JMP
jmp while_3_end
# INSTRUCTION_LABEL
while_3_begin:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -64(%rbp), %rdi
# INSTRUCTION_WRITE
movq %rdi, %rsi
movq $intprint, %rdi
movq $0, %rax
call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -32(%rbp), %r8
# INSTRUCTION_CONST
mov $1, %r9
# INSTRUCTION_MINUS
sub %r9, %r8
# INSTRUCTION_WRITE
movq %r8, %rsi
movq $intprint, %rdi
movq $0, %rax
call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
leaq staticLink, %r11
mov 0(%r11), %r11
movq -16(%r11), %r12
addq $1, %r12
imul $-1, %r12
mov -32(%r11, %r12, 8), %r10
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -64(%rbp), %r13
# INSTRUCTION_CONST
mov $8, %r14
# INSTRUCTION_MUL
imul %r14, %r13
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
mov (%r10, %r13,1), %r10
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -48(%rbp), %r15
# INSTRUCTION_MUL
imul %r10, %r15
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -40(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK_IN_SCOPE
leaq staticLink, %rbx
mov 0(%rbx), %rbx
movq -16(%rbx), %rsi
addq $1, %rsi
imul $-1, %rsi
mov -24(%rbx, %rsi, 8), %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -64(%rbp), %rdi
# INSTRUCTION_CONST
mov $8, %r8
# INSTRUCTION_MUL
imul %r8, %rdi
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
mov (%rdx, %rdi,1), %rdx
# INSTRUCTION_MUL
imul %rcx, %rdx
# INSTRUCTION_MINUS
sub %r15, %rdx
# INSTRUCTION_CONST
mov $-1, %r9
# INSTRUCTION_ADD
add %r9, %rdx
# COMPLEX_CONSTRAIN_BOOLEAN
push %rdx
cmp $0, %rdx
seta %dl
movsx %dl, %rdx
pop %rdx
# INSTRUCTION_CONST
mov $1, %r10
# INSTRUCTION_CMP
cmp %rdx, %r10
# INSTRUCTION_JE
je if_8_begin
# INSTRUCTION_JMP
jmp if_8_end
# INSTRUCTION_LABEL
if_8_begin:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -56(%rbp), %r11
# INSTRUCTION_CONST
mov $1, %r12
# INSTRUCTION_ADD
add %r11, %r12
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %r12, -56(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -56(%rbp), %r13
# INSTRUCTION_PUSH
push %r13
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -64(%rbp), %r14
# INSTRUCTION_PUSH
push %r14
# INSTRUCTION_FUNCTION_CALL
call exchange
# COMPLEX_RESTORE_STATIC_LINK
leaq staticLink, %r15
movq %rbp, 8(%r15)
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %rax, -72(%rbp)
# INSTRUCTION_LABEL
if_8_end:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -64(%rbp), %rcx
# INSTRUCTION_CONST
mov $1, %rdx
# INSTRUCTION_ADD
add %rcx, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %rdx, -64(%rbp)
# INSTRUCTION_JMP
jmp while_cnd_3
# INSTRUCTION_LABEL
while_3_end:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -56(%rbp), %rbx
# INSTRUCTION_CONST
mov $1, %rsi
# INSTRUCTION_ADD
add %rbx, %rsi
# INSTRUCTION_PUSH
push %rsi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -32(%rbp), %rdi
# INSTRUCTION_PUSH
push %rdi
# INSTRUCTION_FUNCTION_CALL
call exchange
# COMPLEX_RESTORE_STATIC_LINK
leaq staticLink, %r8
movq %rbp, 8(%r8)
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %rax, -72(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -56(%rbp), %r9
# INSTRUCTION_CONST
mov $1, %r10
# INSTRUCTION_ADD
add %r9, %r10
# INSTRUCTION_RETURN
mov %r10, %rax
mov %rbp,%rsp
pop %rbp
ret
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
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
    movq $1000000, 16(%r15)
    movq $0, 8(%r15)
    movq $1, 0(%r15)
    addq $1000000, %rax
    push %rax
    movq %rax, %rdi
    mov $12, %rax
    syscall
    pop %rax
    #top should be in rax

    #move heap into one

    leaq gcHeapTwo, %r15
    movq %rax, 24(%r15)
    movq $1000000, 16(%r15)
    movq $0, 8(%r15)
    movq $0, 0(%r15)

    addq $1000000, %rax
    movq %rax, %rdi
    mov $12, %rax
    syscall
subq $200, %rsp
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
mov $10, %r15
# COMPLEX_ALLOCATE
movq $8, %rcx
imulq %r15, %rcx
# ALLOC_ARR_OF_PRIM
addq $16, %rcx
pushq %rcx
pushq %rbp
call garbageCollectAllocate
movq %r15, 0(%rax)
subq $8, %rcx
movq $0, (%rax, %rcx, 1)
popq %rcx
popq %rcx
addq $8, %rax
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %rax, -64(%rbp)
# INSTRUCTION_CONST
mov $10, %rdx
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
mov %rax, -72(%rbp)
# INSTRUCTION_CONST
mov $10, %rsi
# COMPLEX_ALLOCATE
movq $8, %rdi
imulq %rsi, %rdi
# ALLOC_ARR_OF_PRIM
addq $16, %rdi
pushq %rdi
pushq %rbp
call garbageCollectAllocate
movq %rsi, 0(%rax)
subq $8, %rdi
movq $0, (%rax, %rdi, 1)
popq %rdi
popq %rdi
addq $8, %rax
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %rax, -80(%rbp)
# INSTRUCTION_CONST
mov $10, %r8
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
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %rax, -88(%rbp)
# INSTRUCTION_CONST
mov $10, %r10
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %r10, -120(%rbp)
# INSTRUCTION_CONST
mov $600, %r11
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %r11, -112(%rbp)
# INSTRUCTION_CONST
mov $0, %r12
# INSTRUCTION_CONST
mov $1, %r13
# INSTRUCTION_MINUS
sub %r13, %r12
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %r12, -128(%rbp)
# INSTRUCTION_CONST
mov $0, %r14
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %r14, -136(%rbp)
# INSTRUCTION_CONST
mov $0, %r15
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %r15, -144(%rbp)
# INSTRUCTION_CONST
mov $1, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %rcx, -160(%rbp)
# INSTRUCTION_CONST
mov $40, %rdx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %rdx, -168(%rbp)
# INSTRUCTION_CONST
mov $3641, %rbx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %rbx, -176(%rbp)
# INSTRUCTION_CONST
mov $729, %rsi
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %rsi, -184(%rbp)
# INSTRUCTION_CONST
mov $0, %rdi
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %rdi, -104(%rbp)
# INSTRUCTION_LABEL
while_cnd_4:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -104(%rbp), %r8
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -120(%rbp), %r9
# INSTRUCTION_MINUS
sub %r9, %r8
# COMPLEX_CONSTRAIN_BOOLEAN
push %rdx
cmp $0, %r8
seta %dl
movsx %dl, %r8
pop %rdx
# INSTRUCTION_CONST
mov $1, %r10
# INSTRUCTION_CMP
cmp %r8, %r10
# INSTRUCTION_JE
je while_4_begin
# INSTRUCTION_JMP
jmp while_4_end
# INSTRUCTION_LABEL
while_4_begin:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -160(%rbp), %r11
# INSTRUCTION_PUSH
push %r11
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -168(%rbp), %r12
# INSTRUCTION_PUSH
push %r12
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -176(%rbp), %r13
# INSTRUCTION_PUSH
push %r13
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -184(%rbp), %r14
# INSTRUCTION_PUSH
push %r14
# INSTRUCTION_FUNCTION_CALL
call NextRand
# COMPLEX_RESTORE_STATIC_LINK
leaq staticLink, %r15
movq %rbp, 0(%r15)
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %rax, -160(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -160(%rbp), %rcx
# INSTRUCTION_CONST
mov $20, %rdx
# INSTRUCTION_DIV
movq %rcx, %rax
pushq %r10
movq %rdx, %r10
cqto
idiv %r10
popq %r10
movq %rax, %rdx
# INSTRUCTION_CONST
mov $10, %rbx
# INSTRUCTION_ADD
add %rdx, %rbx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -64(%rbp), %rsi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -104(%rbp), %rdi
# INSTRUCTION_CONST
mov $8, %r8
# INSTRUCTION_MUL
imul %r8, %rdi
# INSTRUCTION_MOVE_TO_OFFSET
mov %rbx, (%rsi, %rdi,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -160(%rbp), %r9
# INSTRUCTION_PUSH
push %r9
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -168(%rbp), %r10
# INSTRUCTION_PUSH
push %r10
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -176(%rbp), %r11
# INSTRUCTION_PUSH
push %r11
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -184(%rbp), %r12
# INSTRUCTION_PUSH
push %r12
# INSTRUCTION_FUNCTION_CALL
call NextRand
# COMPLEX_RESTORE_STATIC_LINK
leaq staticLink, %r13
movq %rbp, 0(%r13)
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %rax, -160(%rbp)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -160(%rbp), %r14
# INSTRUCTION_CONST
mov $90, %r15
# INSTRUCTION_DIV
movq %r14, %rax
pushq %r10
movq %r15, %r10
cqto
idiv %r10
popq %r10
movq %rax, %r15
# INSTRUCTION_CONST
mov $1, %rcx
# INSTRUCTION_ADD
add %r15, %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -72(%rbp), %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -104(%rbp), %rbx
# INSTRUCTION_CONST
mov $8, %rsi
# INSTRUCTION_MUL
imul %rsi, %rbx
# INSTRUCTION_MOVE_TO_OFFSET
mov %rcx, (%rdx, %rbx,1)
# INSTRUCTION_CONST
mov $0, %rdi
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -80(%rbp), %r8
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -104(%rbp), %r9
# INSTRUCTION_CONST
mov $8, %r10
# INSTRUCTION_MUL
imul %r10, %r9
# INSTRUCTION_MOVE_TO_OFFSET
mov %rdi, (%r8, %r9,1)
# INSTRUCTION_CONST
mov $0, %r11
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -88(%rbp), %r12
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -104(%rbp), %r13
# INSTRUCTION_CONST
mov $8, %r14
# INSTRUCTION_MUL
imul %r14, %r13
# INSTRUCTION_MOVE_TO_OFFSET
mov %r11, (%r12, %r13,1)
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -104(%rbp), %r15
# INSTRUCTION_CONST
mov $1, %rcx
# INSTRUCTION_ADD
add %r15, %rcx
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %rcx, -104(%rbp)
# INSTRUCTION_JMP
jmp while_cnd_4
# INSTRUCTION_LABEL
while_4_end:
# INSTRUCTION_CONST
mov $0, %rdx
# INSTRUCTION_PUSH
push %rdx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -120(%rbp), %rbx
# INSTRUCTION_CONST
mov $1, %rsi
# INSTRUCTION_MINUS
sub %rsi, %rbx
# INSTRUCTION_PUSH
push %rbx
# INSTRUCTION_FUNCTION_CALL
call quicksort
# COMPLEX_RESTORE_STATIC_LINK
leaq staticLink, %rdi
movq %rbp, 0(%rdi)
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %rax, -152(%rbp)
# INSTRUCTION_CONST
mov $0, %r8
# INSTRUCTION_PUSH
push %r8
# INSTRUCTION_CONST
mov $0, %r9
# INSTRUCTION_PUSH
push %r9
# INSTRUCTION_CONST
mov $0, %r10
# INSTRUCTION_PUSH
push %r10
# INSTRUCTION_FUNCTION_CALL
call BKnap
# COMPLEX_RESTORE_STATIC_LINK
leaq staticLink, %r11
movq %rbp, 0(%r11)
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %rax, -96(%rbp)
# INSTRUCTION_CONST
mov $0, %r12
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %r12, -104(%rbp)
# INSTRUCTION_LABEL
while_cnd_5:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -104(%rbp), %r13
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -120(%rbp), %r14
# INSTRUCTION_MINUS
sub %r14, %r13
# COMPLEX_CONSTRAIN_BOOLEAN
push %rdx
cmp $0, %r13
seta %dl
movsx %dl, %r13
pop %rdx
# INSTRUCTION_CONST
mov $1, %r15
# INSTRUCTION_CMP
cmp %r13, %r15
# INSTRUCTION_JE
je while_5_begin
# INSTRUCTION_JMP
jmp while_5_end
# INSTRUCTION_LABEL
while_5_begin:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -80(%rbp), %rcx
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -104(%rbp), %rdx
# INSTRUCTION_CONST
mov $8, %rbx
# INSTRUCTION_MUL
imul %rbx, %rdx
# COMPLEX_DEREFERENCE_POINTER_WITH_OFFSET
mov (%rcx, %rdx,1), %rcx
# INSTRUCTION_CONST
mov $1, %rsi
# INSTRUCTION_MINUS
sub %rcx, %rsi
# INSTRUCTION_CONST
mov $1, %rdi
# INSTRUCTION_MINUS
sub %rsi, %rdi
# INSTRUCTION_CONST
mov $1, %r8
# INSTRUCTION_CMP
cmp %rdi, %r8
# INSTRUCTION_JE
je if_9_begin
# INSTRUCTION_JMP
jmp if_9_end
# INSTRUCTION_LABEL
if_9_begin:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -144(%rbp), %r9
# INSTRUCTION_CONST
mov $1, %r10
# INSTRUCTION_ADD
add %r9, %r10
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %r10, -144(%rbp)
# INSTRUCTION_LABEL
if_9_end:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -104(%rbp), %r11
# INSTRUCTION_CONST
mov $1, %r12
# INSTRUCTION_ADD
add %r11, %r12
# COMPLEX_MOVE_TEMPORARY_INTO_STACK
mov %r12, -104(%rbp)
# INSTRUCTION_JMP
jmp while_cnd_5
# INSTRUCTION_LABEL
while_5_end:
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -144(%rbp), %r13
# INSTRUCTION_WRITE
movq %r13, %rsi
movq $intprint, %rdi
movq $0, %rax
call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -128(%rbp), %r14
# INSTRUCTION_WRITE
movq %r14, %rsi
movq $intprint, %rdi
movq $0, %rax
call printf
# COMPLEX_MOVE_TEMPORARY_FROM_STACK
mov -136(%rbp), %r15
# INSTRUCTION_WRITE
movq %r15, %rsi
movq $intprint, %rdi
movq $0, %rax
call printf
# METADATA_END_BODY_BLOCK
mov %rbp,%rsp
pop %rbp
ret
mov $60, %rax
mov $0, %rdi
syscall
