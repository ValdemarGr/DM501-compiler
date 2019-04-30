.section .data
hello: .asciz "Hello\n"
intprint: .asciz "%i\n"

.section .text
.global main
.extern printf
main:
	movq $intprint, %rdi
	movq $69, %rsi
	movq $0, %rax
	call printf
	ret
