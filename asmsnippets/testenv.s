o:
        .long   1
l:
        .quad   1
        .quad   2
        .quad   3
        .quad   4
        .quad   5

.global main
main:
    pushq   %rbp
    movq    %rsp, %rbp
    movl    $5, o(%rip)
    movq    $0, l(%rip)
    movq    $0, l+8(%rip)
    movl    $1, %eax
    popq    %rbp
    ret

    #mov $60, %rax
    #mov $0, %rdi
    #syscall
