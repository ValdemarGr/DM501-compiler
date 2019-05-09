//
// Created by valde on 3/25/19.
//

#ifndef HELLO_CONSTS_H
#define HELLO_CONSTS_H

#include "../utils/map.h"

#define ASM_HEADER "main:\n\tpush %%rbp\nmov %%rbp, %%rax\n\tmovq %%rsp, %%rbp\npush %%rax\n"
#define ASM_TAIL "mov $60, %%rax\nmov $0, %%rdi\nsyscall\n"
#define MAIN_HEADER "\t# 20 mb\r\n\r\n    mov $0, %%rdi\r\n    mov $12, %%rax\r\n    syscall\r\n    #move heap into one\r\n    leaq gcHeapOne, %%r15\r\n    movq %%rax, 24(%%r15)\r\n    movq $10000, 16(%%r15)\r\n    movq $0, 8(%%r15)\r\n    movq $1, 0(%%r15)\r\n    addq $10000, %%rax\r\n    push %%rax\r\n    movq %%rax, %%rdi\r\n    mov $12, %%rax\r\n    syscall\r\n    pop %%rax\r\n    #top should be in rax\r\n\r\n    #move heap into one\r\n\r\n    leaq gcHeapTwo, %%r15\r\n    movq %%rax, 24(%%r15)\r\n    movq $10000, 16(%%r15)\r\n    movq $0, 8(%%r15)\r\n    movq $0, 0(%%r15)\r\n\r\n    addq $10000, %%rax\r\n    movq %%rax, %%rdi\r\n    mov $12, %%rax\r\n    syscall\n"
#define GARBAGE_COLLECTOR_CHUNK "gcHeapOne:\r\n    .space 32\r\ngcHeapTwo:\r\n    .space 32\r\nmetaHashMap:\r\n    .space 16\r\nmetaLambdaList:\r\n    .space 16\r\n.section .text\r\n.global main\r\n.extern printf\r\n# METADATA_BEGIN_BODY_BLOCK\r\n# VAR clsB\r\n# VAR clsA\r\n# METADATA_CREATE_MAIN\r\n.type garbageCollectStructure, @function\r\ngarbageCollectStructure:\r\n    push %rbp\r\n    movq %rsp, %rbp\r\n\tsubq $56, %rsp\r\n\r\n\r\n\r\n    movq %rbp,%rsp\r\n    pop %rbp\r\n    ret\r\n\r\n.type garbageCollectBFS, @function\r\ngarbageCollectBFS:\r\n    push %rbp\r\n    movq %rsp, %rbp\r\n\r\n    push %rax\r\n    push %rcx\r\n    push %rdx\r\n    push %rbx\r\n    push %rsi\r\n    push %rdi\r\n    push %r8\r\n    push %r9\r\n    push %r10\r\n    push %r11\r\n    push %r12\r\n    push %r13\r\n    push %r14\r\n    push %r15\r\n\r\n    #reset heap counter for both &\r\n    # swap in use values\r\n    leaq gcHeapTwo, %rsi\r\n    negq 0(%rsi)\r\n    movq $0, 8(%rsi)\r\n    leaq gcHeapOne, %rsi\r\n    negq 0(%rsi)\r\n    movq $0, 8(%rsi)\r\n\r\n    movq 0(%rsi), %r12\r\n    cmp $1, %r12\r\n    je loadHeapEnd\r\n    leaq gcHeapTwo, %rsi\r\n    loadHeapEnd:\r\n\r\n\tmov 16(%rbp), %r15\r\n    # we will find the amount of ptrs in -16\r\n    mov -16(%r15), %r14\r\n    mov %r14, %r8\r\n    #add 1 + the size of the array list to begin the fetch phase\r\n    #inc %r8\r\n    #times 8 because of sizes\r\n    imulq $8, %r8\r\n\r\n    mov $-24, %r13 # offset\r\n\r\n    #add initial offset\r\n    movq $0, %r12\r\n    subq %r8, %r12\r\n    movq %r12, %r8\r\n\r\n    add %r13, %r8\r\n\r\n    #initial pass moves all ptrs over\r\n    gcLoopBegin:\r\n    cmp $0, %r14\r\n    je gcLoopEnd\r\n        mov (%r15, %r13, 1), %rdi #rdi has offset\r\n        #mul by factor\r\n        imulq $8, %rdi\r\n        #negate because of stack access\r\n        movq $0, %r12\r\n        subq %rdi, %r12\r\n        movq %r12, %rdi\r\n        #add the constant offset\r\n        add %r8, %rdi\r\n        movq (%r15, %rdi, 1), %rcx\r\n\r\n        #nullptr check\r\n        cmp $0, %rcx\r\n        je gcLoopEpilogue\r\n\r\n        subq $8, %rcx\r\n        # get size\r\n        mov 0(%rcx), %rax\r\n\r\n        #mul by 8 bc pointer size\r\n        imulq $8, %rax\r\n\r\n        #skip head\r\n        add $8, %rax\r\n\r\n        #get ptr count\r\n        movq (%rcx, %rax, 1), %rdi\r\n        #add the ptr count slot\r\n        add $8, %rax\r\n\r\n        cmpq $-1, %rdi\r\n        je skipRec\r\n            imulq $8, %rdi\r\n            add %rdi, %rax\r\n            #rax now has the structure size\r\n        skipRec:\r\n\r\n        # subtract 8 from rcx to find where to move from\r\n        # reserve new space on heap\r\n\r\n        # new heap struct is in rsi\r\n        # in rcx we have the pointer to the beginning of the struct in old heap\r\n        # in rax we have the length of the struct * 8\r\n        # we have r12 free\r\n        # we have rdi free\r\n\r\n        # save new positional ptr\r\n        movq 24(%rsi), %r12 # r12 has new heap start\r\n        movq 8(%rsi), %rdi # current heap position\r\n\r\n        # save old heap position\r\n        pushq %rdi\r\n\r\n        mov $0, %rbx # rbx has 0\r\n\r\n        heapMover:\r\n        cmp $0, %rax\r\n        je heapMoverEnd\r\n\r\n            # move rcx + indexer(rbx) to top of new heap\r\n            movq (%rcx, %rbx, 1), %rdx\r\n\r\n            movq %rdx, (%r12, %rdi, 1)\r\n\r\n            add $8, %rdi\r\n            add $8, %rbx\r\n            sub $8, %rax\r\n        jmp heapMover\r\n        heapMoverEnd:\r\n\r\n        mov %rdi, 8(%rsi)\r\n\r\n        #Now we have to restore the stack pointer\r\n        #We have old heap start in r12\r\n        popq %rdi\r\n\r\n        movq 24(%rsi), %r12\r\n        addq %rdi, %r12\r\n        addq $8, %r12\r\n\r\n        mov (%r15, %r13, 1), %rdi #rdi has offset\r\n        #mul by factor\r\n        imulq $8, %rdi\r\n        #negate because of stack access\r\n        movq $0, %rax\r\n        subq %rdi, %rax\r\n        movq %rax, %rdi\r\n        #add the constant offset\r\n        add %r8, %rdi\r\n        #r12 is now our offset to our first pointer\r\n\r\n        # place -1 and newptr in old heap\r\n        movq $-1, 0(%rcx)\r\n        movq %r12, 8(%rcx)\r\n\r\n        movq %r12, (%r15, %rdi, 1)\r\n\r\n        gcLoopEpilogue:\r\n\r\n        subq $8, %r13\r\n        dec %r14\r\n        jmp gcLoopBegin\r\n    gcLoopEnd:\r\n\r\n    # if we are main, quit\r\n    leaq staticLink, %rcx #used for comparison with main\r\n    mov 0(%rcx), %rcx\r\n    cmp %r15, %rcx\r\n    je gcRet\r\n\r\n    #recurse\r\n    pushq -8(%r15)\r\n    call garbageCollectBFS\r\n    pop %r15\r\n\r\n    gcRet:\r\n\r\n    pop %r15\r\n    pop %r14\r\n    pop %r13\r\n    pop %r12\r\n    pop %r11\r\n    pop %r10\r\n    pop %r9\r\n    pop %r8\r\n    pop %rdi\r\n    pop %rsi\r\n    pop %rbx\r\n    pop %rdx\r\n    pop %rcx\r\n    pop %rax\r\n\r\n    movq %rbp,%rsp\r\n    pop %rbp\r\n    ret\r\n\r\n\r\n.type garbageCollect, @function\r\ngarbageCollect:\r\n    push %rbp\r\n    movq %rsp, %rbp\r\n\r\n    pushq 16(%rbp)\r\n    call garbageCollectBFS\r\n    pop 16(%rbp)\r\n\r\n    push %rax\r\n    push %rcx\r\n    push %rdx\r\n    push %rbx\r\n    push %rsi\r\n    push %rdi\r\n    push %r8\r\n    push %r9\r\n    push %r10\r\n    push %r11\r\n    push %r12\r\n    push %r13\r\n    push %r14\r\n    push %r15\r\n\r\n    leaq gcHeapOne, %r15\r\n    leaq gcHeapTwo, %r14\r\n    cmp $1, 0(%r15)\r\n    je heapSelectorEnd\r\n    leaq gcHeapTwo, %r15\r\n    leaq gcHeapOne, %r14\r\n    heapSelectorEnd:\r\n\r\n    # traverse new heap, if ptr to old heap found, move it to new heap, we know new heap is packed\r\n    movq $0, %rdx # rdx is heap iterator\r\n    movq 24(%r15), %rcx # rcx has actual heap ptr\r\n    movq 8(%r15), %rax # rax has current new heap pos\r\n    newHeapTraverseBegin:\r\n        cmp %rdx, %rax\r\n        je newHeapTraverseEnd\r\n\r\n        # now to traversing ptrs\r\n        # for this element get size\r\n        movq (%rcx, %rdx, 1), %rbx # rbx has struct size\r\n        movq %rbx, %r13\r\n        # add 1 to skip size block\r\n        add $1, %rbx\r\n        # mul by 8 for block size\r\n        imul $8, %rbx\r\n\r\n        mov %rbx, %r10\r\n        # add heap iter for absolute position\r\n        add %rdx, %rbx\r\n\r\n        movq (%rcx, %rbx, 1), %rsi # rbx how has ptr count\r\n\r\n        movq %rsi, %r11\r\n        imul $8, %r11\r\n        add %r10, %r11\r\n        addq $8, %r11\r\n        pushq %r11\r\n\r\n        # use rbx + 8 has first item\r\n        addq $8, %rbx #now is first ptr\r\n        selfPtrHeapMoveBegin:\r\n            cmp $0, %rsi\r\n            je selfPtrHeapMoveEnd\r\n\r\n            # get the struct ptr offset\r\n            movq (%rcx, %rbx, 1), %rdi # rdi has offset\r\n            # copy struct position ptr\r\n            movq %rdx, %r8\r\n            # add 1 for header\r\n            add $8, %r8\r\n            #mul 8 ptr size\r\n            imul $8, %rdi\r\n            #add offset\r\n            add %rdi, %r8\r\n\r\n            pushq %r8\r\n\r\n            movq (%rcx, %r8, 1), %r8\r\n\r\n            # nullptr check\r\n            cmp $0, %r8\r\n            jne skipEndNullptr\r\n            popq %r8\r\n            jmp selfPtrHeapMoveEpilogue\r\n            skipEndNullptr:\r\n\r\n            # finally move the ptr to new heap space if it is in old\r\n            # we can check this by seeing if the addr is between old start and old end\r\n            # old is in r14\r\n            movq 24(%r14), %r9 # begin\r\n            movq 16(%r14), %r10 # end\r\n            # add the pointer to the end offset\r\n            addq %r9, %r10\r\n\r\n            cmp %r9, %r8 #inverted because of gas xd\r\n            jl beforeEpilogueBegin\r\n            cmp %r8, %r10 #inverted because of gas xd\r\n            jl beforeEpilogueBegin\r\n            # if r8 < r9 || r10 < r8 skip\r\n\r\n            # remove from top of stack\r\n\r\n            jmp beforeEpilogueEnd\r\n            beforeEpilogueBegin:\r\n            popq %r8\r\n            jmp selfPtrHeapMoveEpilogue\r\n            beforeEpilogueEnd:\r\n            # save offset for when we have to save this field\r\n\r\n            # we actually have to move it\r\n            # we can use r9 & r10 again\r\n            # we need to find total size of the struct\r\n            # the header can be used for this\r\n            # grab header size by -8\r\n            subq $8, %r8\r\n            movq (%r8), %r9 # r9 has size\r\n\r\n            cmp $-1, %r9 # if we have already moved it\r\n            jne alreadyMovedEnd\r\n                # now we can find the head pointer by adding 8\r\n                movq 8(%r8), %r11\r\n\r\n                # Where old ptr offset aboslute in structure we are working in\r\n                popq %r8\r\n\r\n                # hide head\r\n                addq $8, %r11\r\n\r\n                movq %r11, (%rcx, %r8, 1)\r\n\r\n                jmp selfPtrHeapMoveEpilogue\r\n\r\n            alreadyMovedEnd:\r\n\r\n            # add 1 to size for ptr count index\r\n            addq $1, %r9\r\n            # mul 8 for ptr size\r\n            imul $8, %r9\r\n            # then move ptr count to r10\r\n            movq (%r8, %r9, 1), %r10\r\n\r\n            # mul 8 for size\r\n            imul $8, %r10\r\n            # add 8 for ptr count box\r\n            addq $8, %r10\r\n\r\n            # add the sizes\r\n            addq %r9, %r10\r\n            movq %r10, %r13\r\n\r\n            structSizeEvalEnd:\r\n            # size will be in r13\r\n            # we can use r9 & r10 again\r\n\r\n            # save old head\r\n            movq %rax, %r11\r\n\r\n            # copy heap iter\r\n            # save old heap tail\r\n            movq $0, %r9\r\n            newHeapMoverBegin:\r\n                cmp %r9, %r13\r\n                je newHeapMoverEnd\r\n\r\n                # move this heap block\r\n                movq (%r8, %r9, 1), %r10\r\n                movq %r10, (%rcx, %rax, 1)\r\n\r\n                addq $8, %r9\r\n                addq $8, %rax #inc heap top\r\n                jmp newHeapMoverBegin\r\n            newHeapMoverEnd:\r\n\r\n            # Where old ptr offset aboslute in structure we are working in\r\n            popq %r13\r\n\r\n            # add the heap ptr to rax\r\n            addq %rcx, %r11\r\n            # hide head\r\n            addq $8, %r11\r\n\r\n            # update old heap with -1 and new ptr\r\n            movq $-1, 0(%r8)\r\n            movq %r11, 8(%r8)\r\n\r\n            # and update the new heap\r\n            movq %r11, (%rcx, %r13, 1)\r\n\r\n\r\n        selfPtrHeapMoveEpilogue:\r\n\r\n            addq $8, %rbx\r\n            dec %rsi\r\n            jmp selfPtrHeapMoveBegin\r\n        selfPtrHeapMoveEnd:\r\n\r\n\r\n    newHeapTraverseEpilogue:\r\n        popq %r12\r\n        addq %r12, %rdx\r\n\r\n        jmp newHeapTraverseBegin\r\n    newHeapTraverseEnd:\r\n\r\n    movq 8(%r15), %rax # rax has current new heap pos\r\n\r\n\r\n\r\n    pop %r15\r\n    pop %r14\r\n    pop %r13\r\n    pop %r12\r\n    pop %r11\r\n    pop %r10\r\n    pop %r9\r\n    pop %r8\r\n    pop %rdi\r\n    pop %rsi\r\n    pop %rbx\r\n    pop %rdx\r\n    pop %rcx\r\n    pop %rax\r\n\r\n    mov %rbp,%rsp\r\n    pop %rbp\r\n    ret\r\n\r\n.type garbageCollectAllocate, @function\r\ngarbageCollectAllocate:\r\n    push %rbp\r\n    movq %rsp, %rbp\r\n    subq $32, %rsp\r\n\r\n    push %r15\r\n    push %r14\r\n\r\n    #args\r\n    #rbp\r\n    #desired byte size\r\n\r\n    movq 16(%rbp), %rax\r\n    movq %rax, -16(%rbp)\r\n    movq 24(%rbp), %rax\r\n    movq %rax, -8(%rbp)\r\n\r\n    leaq gcHeapOne, %r15\r\n    cmp $1, 0(%r15)\r\n    je heapSelectorEndAlloc\r\n    leaq gcHeapTwo, %r15\r\n    heapSelectorEndAlloc:\r\n\r\n    # current heap position\r\n    movq 8(%r15), %r14\r\n    # add new size\r\n    movq -8(%rbp), %rax\r\n    addq %r14, %rax\r\n    movq %rax, 8(%r15)\r\n\r\n    # old size is still in r14\r\n    movq 24(%r15), %rax\r\n    addq %r14, %rax\r\n\r\n    pop %r14\r\n    pop %r15\r\n\r\n    mov %rbp,%rsp\r\n    pop %rbp\r\n    ret\r\n"
#endif //HELLO_CONSTS_H
