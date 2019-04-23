scopeFrames:
.quad 0
.quad 0
.type horse, @function
horse:
push %rbp
mov %rsp,%rbp
mov $0, %rax
mov $5, %rcx
sub %rcx, %rax
mov %rax, %rdx
sar $31, %rdx
add %rdx, %rax
xor %rax, %rdx
mov %rdx, -24(%rbp)
mov $5, %rbx
mov %rbx, -24(%rbp)
mov $2, %rsi
movq scopeFrames+0(%rip), %rdi
movq -0(%rdi), %rdi
mov %rsi, (%rdi)
movq -8(%rbp), %r8
mov %r8, %rax
mov %rbp,%rsp
pop %rbp
ret
.global main
main:
mov $5, %rax
mov %rcx, -8(%rbp)
mov $60, %rax
mov $0, %rdi
syscall
