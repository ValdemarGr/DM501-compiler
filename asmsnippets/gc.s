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

	movq 16(%rbp), %r15
	movq %r15, -8(%rbp)

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
    movq 0(%rsi), %r12
    movq $1, %r13
    subq %r12, %r13
    movq %r13, 0(%rsi)
    movq $0, 8(%rsi)
    leaq gcHeapOne, %rsi
    movq 0(%rsi), %r12
    movq $1, %r13
    subq %r12, %r13
    movq %r13, 0(%rsi)
    movq $0, 8(%rsi)

    movq 0(%rsi), %r12
    cmp $1, %r12
    je loadHeapEnd
    leaq gcHeapTwo, %rsi
    loadHeapEnd:

    # we will find the amount of ptrs in -16
    mov -16(%r15), %r14
    mov %r14, %r8
    #add 1 + the size of the array list to begin the fetch phase
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
        #%r12 is now our offset to our first pointer
        movq (%r15, %rdi, 1), %rcx

        #if rcx is 0, then its not allocated
        cmp $0, %rcx
        je gcLoopEpilogue

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

    gcLoopEpilogue:
        #add the moved struct to the new heap

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

    push %r15

    pushq 16(%rbp)
    call garbageCollectBFS
    pop %rax

    leaq gcHeapOne, %r15
    cmp $1, 0(%r15)
    je heapSelectorEnd
    leaq gcHeapTwo, %r15
    heapSelectorEnd:

    pop %r15

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

    #pushq -16(%rbp)
    #call garbageCollect
    #pop %rax

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
    push %r15
    push %r14

    #rax has total offset
    sub $8, %rax

    mov 24(%r15), %r15
    mov -8(%rbp), %r14
    zeroSetterBegin:
    cmp $0, %r14
    je zeroSetterEnd

    movq $0, (%r15, %rax, 1)

    subq $8, %r14
    subq $8, %rax
    jmp zeroSetterBegin
    zeroSetterEnd:

    pop %r14
    pop %r15

    movq 24(%r15), %rax
    addq %r14, %rax

    pop %r14
    pop %r15

    mov %rbp,%rsp
    pop %rbp
    ret

main:
    push %rbp
    movq %rsp, %rbp

    # 20 mb

    mov $0, %rdi
    mov $12, %rax
    syscall
    #move heap into one
    leaq gcHeapOne, %r15
    movq %rax, 24(%r15)
    movq $10000000, 16(%r15)
    movq $0, 8(%r15)
    movq $1, 0(%r15)

    movq $10000000, %rdi
    mov $12, %rax
    syscall

    #top should be in rax

    #move heap into one
    leaq gcHeapTwo, %r15
    movq %rax, 24(%r15)
    movq $10000000, 16(%r15)
    movq $0, 8(%r15)
    movq $0, 0(%r15)

    movq $10000000, %rdi
    mov $12, %rax
    syscall

    push %rbp
    pushq $8
    call garbageCollectAllocate
    movq $42, (%rax)
    mov (%rax), %r14

    mov %rbp,%rsp
    pop %rbp
    ret
    mov $60, %rax
    mov $0, %rdi
    syscall
