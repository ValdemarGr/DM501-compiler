.section .data
staticLink:
	.space 24
intprint:
	.asciz "%i\n"
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