	.file	"printComp.c"
# GNU C17 (GCC) version 8.2.1 20181127 (x86_64-pc-linux-gnu)
#	compiled by GNU C version 8.2.1 20181127, GMP version 6.1.2, MPFR version 4.0.1, MPC version 1.1.0, isl version isl-0.20-GMP

# warning: MPFR header version 4.0.1 differs from library version 4.0.2.
# GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
# options passed:  printComp.c -mtune=generic -march=x86-64
# -auxbase-strip out.s -g -fverbose-asm
# options enabled:  -fPIC -fPIE -faggressive-loop-optimizations
# -fasynchronous-unwind-tables -fauto-inc-dec -fchkp-check-incomplete-type
# -fchkp-check-read -fchkp-check-write -fchkp-instrument-calls
# -fchkp-narrow-bounds -fchkp-optimize -fchkp-store-bounds
# -fchkp-use-static-bounds -fchkp-use-static-const-bounds
# -fchkp-use-wrappers -fcommon -fdelete-null-pointer-checks
# -fdwarf2-cfi-asm -fearly-inlining -feliminate-unused-debug-types
# -ffp-int-builtin-inexact -ffunction-cse -fgcse-lm -fgnu-runtime
# -fgnu-unique -fident -finline-atomics -fira-hoist-pressure
# -fira-share-save-slots -fira-share-spill-slots -fivopts
# -fkeep-static-consts -fleading-underscore -flifetime-dse
# -flto-odr-type-merging -fmath-errno -fmerge-debug-strings -fpeephole
# -fplt -fprefetch-loop-arrays -freg-struct-return
# -fsched-critical-path-heuristic -fsched-dep-count-heuristic
# -fsched-group-heuristic -fsched-interblock -fsched-last-insn-heuristic
# -fsched-rank-heuristic -fsched-spec -fsched-spec-insn-heuristic
# -fsched-stalled-insns-dep -fschedule-fusion -fsemantic-interposition
# -fshow-column -fshrink-wrap-separate -fsigned-zeros
# -fsplit-ivs-in-unroller -fssa-backprop -fstack-protector-strong
# -fstdarg-opt -fstrict-volatile-bitfields -fsync-libcalls -ftrapping-math
# -ftree-cselim -ftree-forwprop -ftree-loop-if-convert -ftree-loop-im
# -ftree-loop-ivcanon -ftree-loop-optimize -ftree-parallelize-loops=
# -ftree-phiprop -ftree-reassoc -ftree-scev-cprop -funit-at-a-time
# -funwind-tables -fverbose-asm -fzero-initialized-in-bss
# -m128bit-long-double -m64 -m80387 -malign-stringops
# -mavx256-split-unaligned-load -mavx256-split-unaligned-store
# -mfancy-math-387 -mfp-ret-in-387 -mfxsr -mglibc -mieee-fp
# -mlong-double-80 -mmmx -mno-sse4 -mpush-args -mred-zone -msse -msse2
# -mstv -mtls-direct-seg-refs -mvzeroupper

	.text
.Ltext0:
	.section	.rodata
.LC0:
	.string	"%i"
	.text
	.globl	doMagicalStuff
	.type	doMagicalStuff, @function
doMagicalStuff:
.LFB6:
	.file 1 "printComp.c"
	.loc 1 8 22
	.cfi_startproc
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$32, %rsp	#,
# printComp.c:9:     int a = 1;
	.loc 1 9 9
	movl	$1, -24(%rbp)	#, a
# printComp.c:10:     int b = 1;
	.loc 1 10 9
	movl	$1, -20(%rbp)	#, b
# printComp.c:11:     int c = 1;
	.loc 1 11 9
	movl	$1, -16(%rbp)	#, c
# printComp.c:12:     int d = 1;
	.loc 1 12 9
	movl	$1, -12(%rbp)	#, d
# printComp.c:13:     int e = 1;
	.loc 1 13 9
	movl	$1, -8(%rbp)	#, e
# printComp.c:14:     int f = 1;
	.loc 1 14 9
	movl	$1, -4(%rbp)	#, f
# printComp.c:16:     printf("%i", a);
	.loc 1 16 5
	movl	-24(%rbp), %eax	# a, tmp93
	movl	%eax, %esi	# tmp93,
	leaq	.LC0(%rip), %rdi	#,
	movl	$0, %eax	#,
	call	printf@PLT	#
# printComp.c:17:     printf("%i", b);
	.loc 1 17 5
	movl	-20(%rbp), %eax	# b, tmp94
	movl	%eax, %esi	# tmp94,
	leaq	.LC0(%rip), %rdi	#,
	movl	$0, %eax	#,
	call	printf@PLT	#
# printComp.c:18:     printf("%i", c);
	.loc 1 18 5
	movl	-16(%rbp), %eax	# c, tmp95
	movl	%eax, %esi	# tmp95,
	leaq	.LC0(%rip), %rdi	#,
	movl	$0, %eax	#,
	call	printf@PLT	#
# printComp.c:19:     printf("%i", d);
	.loc 1 19 5
	movl	-12(%rbp), %eax	# d, tmp96
	movl	%eax, %esi	# tmp96,
	leaq	.LC0(%rip), %rdi	#,
	movl	$0, %eax	#,
	call	printf@PLT	#
# printComp.c:20:     printf("%i", e);
	.loc 1 20 5
	movl	-8(%rbp), %eax	# e, tmp97
	movl	%eax, %esi	# tmp97,
	leaq	.LC0(%rip), %rdi	#,
	movl	$0, %eax	#,
	call	printf@PLT	#
# printComp.c:21:     printf("%i", f);
	.loc 1 21 5
	movl	-4(%rbp), %eax	# f, tmp98
	movl	%eax, %esi	# tmp98,
	leaq	.LC0(%rip), %rdi	#,
	movl	$0, %eax	#,
	call	printf@PLT	#
# printComp.c:24:     if (a < b || (b >= e || f == b) && e < b) {
	.loc 1 24 8
	movl	-24(%rbp), %eax	# a, tmp99
	cmpl	-20(%rbp), %eax	# b, tmp99
	jl	.L2	#,
# printComp.c:24:     if (a < b || (b >= e || f == b) && e < b) {
	.loc 1 24 15 discriminator 1
	movl	-20(%rbp), %eax	# b, tmp100
	cmpl	-8(%rbp), %eax	# e, tmp100
	jge	.L3	#,
# printComp.c:24:     if (a < b || (b >= e || f == b) && e < b) {
	.loc 1 24 26 discriminator 3
	movl	-4(%rbp), %eax	# f, tmp101
	cmpl	-20(%rbp), %eax	# b, tmp101
	jne	.L4	#,
.L3:
# printComp.c:24:     if (a < b || (b >= e || f == b) && e < b) {
	.loc 1 24 37 discriminator 4
	movl	-8(%rbp), %eax	# e, tmp102
	cmpl	-20(%rbp), %eax	# b, tmp102
	jge	.L4	#,
.L2:
# printComp.c:25:         return 66;
	.loc 1 25 16
	movl	$66, %eax	#, _5
	jmp	.L5	#
.L4:
# printComp.c:29:     return a + b + c + d + e + f;
	.loc 1 29 14
	movl	-24(%rbp), %edx	# a, tmp103
	movl	-20(%rbp), %eax	# b, tmp104
	addl	%eax, %edx	# tmp104, _1
# printComp.c:29:     return a + b + c + d + e + f;
	.loc 1 29 18
	movl	-16(%rbp), %eax	# c, tmp105
	addl	%eax, %edx	# tmp105, _2
# printComp.c:29:     return a + b + c + d + e + f;
	.loc 1 29 22
	movl	-12(%rbp), %eax	# d, tmp106
	addl	%eax, %edx	# tmp106, _3
# printComp.c:29:     return a + b + c + d + e + f;
	.loc 1 29 26
	movl	-8(%rbp), %eax	# e, tmp107
	addl	%eax, %edx	# tmp107, _4
# printComp.c:29:     return a + b + c + d + e + f;
	.loc 1 29 30
	movl	-4(%rbp), %eax	# f, tmp108
	addl	%edx, %eax	# _4, _5
.L5:
# printComp.c:30: }
	.loc 1 30 1
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE6:
	.size	doMagicalStuff, .-doMagicalStuff
	.globl	main
	.type	main, @function
main:
.LFB7:
	.loc 1 32 12
	.cfi_startproc
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$32, %rsp	#,
# printComp.c:34:     int a = 1;
	.loc 1 34 9
	movl	$1, -24(%rbp)	#, a
# printComp.c:35:     int b = 1;
	.loc 1 35 9
	movl	$1, -20(%rbp)	#, b
# printComp.c:36:     int c = 1;
	.loc 1 36 9
	movl	$1, -16(%rbp)	#, c
# printComp.c:37:     int d = 1;
	.loc 1 37 9
	movl	$1, -12(%rbp)	#, d
# printComp.c:38:     int e = 1;
	.loc 1 38 9
	movl	$1, -8(%rbp)	#, e
# printComp.c:39:     int f = 1;
	.loc 1 39 9
	movl	$1, -4(%rbp)	#, f
# printComp.c:41:     printf("%i", a);
	.loc 1 41 5
	movl	-24(%rbp), %eax	# a, tmp89
	movl	%eax, %esi	# tmp89,
	leaq	.LC0(%rip), %rdi	#,
	movl	$0, %eax	#,
	call	printf@PLT	#
# printComp.c:42:     printf("%i", b);
	.loc 1 42 5
	movl	-20(%rbp), %eax	# b, tmp90
	movl	%eax, %esi	# tmp90,
	leaq	.LC0(%rip), %rdi	#,
	movl	$0, %eax	#,
	call	printf@PLT	#
# printComp.c:43:     printf("%i", c);
	.loc 1 43 5
	movl	-16(%rbp), %eax	# c, tmp91
	movl	%eax, %esi	# tmp91,
	leaq	.LC0(%rip), %rdi	#,
	movl	$0, %eax	#,
	call	printf@PLT	#
# printComp.c:44:     printf("%i", d);
	.loc 1 44 5
	movl	-12(%rbp), %eax	# d, tmp92
	movl	%eax, %esi	# tmp92,
	leaq	.LC0(%rip), %rdi	#,
	movl	$0, %eax	#,
	call	printf@PLT	#
# printComp.c:45:     printf("%i", e);
	.loc 1 45 5
	movl	-8(%rbp), %eax	# e, tmp93
	movl	%eax, %esi	# tmp93,
	leaq	.LC0(%rip), %rdi	#,
	movl	$0, %eax	#,
	call	printf@PLT	#
# printComp.c:46:     printf("%i", f);
	.loc 1 46 5
	movl	-4(%rbp), %eax	# f, tmp94
	movl	%eax, %esi	# tmp94,
	leaq	.LC0(%rip), %rdi	#,
	movl	$0, %eax	#,
	call	printf@PLT	#
# printComp.c:48:     a++;
	.loc 1 48 6
	addl	$1, -24(%rbp)	#, a
# printComp.c:49:     b = 5 + c;
	.loc 1 49 7
	movl	-16(%rbp), %eax	# c, tmp98
	addl	$5, %eax	#, tmp97
	movl	%eax, -20(%rbp)	# tmp97, b
# printComp.c:51:     printf("%i", b);
	.loc 1 51 5
	movl	-20(%rbp), %eax	# b, tmp99
	movl	%eax, %esi	# tmp99,
	leaq	.LC0(%rip), %rdi	#,
	movl	$0, %eax	#,
	call	printf@PLT	#
# printComp.c:53:     return 0;
	.loc 1 53 12
	movl	$0, %eax	#, _17
# printComp.c:54: }
	.loc 1 54 1
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE7:
	.size	main, .-main
.Letext0:
	.file 2 "/usr/lib/gcc/x86_64-pc-linux-gnu/8.2.1/include/stddef.h"
	.file 3 "/usr/include/bits/types.h"
	.file 4 "/usr/include/bits/types/struct_FILE.h"
	.file 5 "/usr/include/bits/types/FILE.h"
	.file 6 "/usr/include/stdio.h"
	.file 7 "/usr/include/bits/sys_errlist.h"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.long	0x3cb
	.value	0x4
	.long	.Ldebug_abbrev0
	.byte	0x8
	.uleb128 0x1
	.long	.LASF51
	.byte	0xc
	.long	.LASF52
	.long	.LASF53
	.quad	.Ltext0
	.quad	.Letext0-.Ltext0
	.long	.Ldebug_line0
	.uleb128 0x2
	.long	.LASF8
	.byte	0x2
	.byte	0xd8
	.byte	0x17
	.long	0x39
	.uleb128 0x3
	.byte	0x8
	.byte	0x7
	.long	.LASF0
	.uleb128 0x4
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x3
	.byte	0x8
	.byte	0x5
	.long	.LASF1
	.uleb128 0x3
	.byte	0x8
	.byte	0x5
	.long	.LASF2
	.uleb128 0x3
	.byte	0x1
	.byte	0x8
	.long	.LASF3
	.uleb128 0x3
	.byte	0x2
	.byte	0x7
	.long	.LASF4
	.uleb128 0x3
	.byte	0x4
	.byte	0x7
	.long	.LASF5
	.uleb128 0x3
	.byte	0x1
	.byte	0x6
	.long	.LASF6
	.uleb128 0x3
	.byte	0x2
	.byte	0x5
	.long	.LASF7
	.uleb128 0x2
	.long	.LASF9
	.byte	0x3
	.byte	0x96
	.byte	0x19
	.long	0x47
	.uleb128 0x2
	.long	.LASF10
	.byte	0x3
	.byte	0x97
	.byte	0x1b
	.long	0x47
	.uleb128 0x5
	.byte	0x8
	.uleb128 0x6
	.byte	0x8
	.long	0x98
	.uleb128 0x3
	.byte	0x1
	.byte	0x6
	.long	.LASF11
	.uleb128 0x7
	.long	0x98
	.uleb128 0x3
	.byte	0x8
	.byte	0x7
	.long	.LASF12
	.uleb128 0x8
	.long	.LASF54
	.byte	0xd8
	.byte	0x4
	.byte	0x31
	.byte	0x8
	.long	0x232
	.uleb128 0x9
	.long	.LASF13
	.byte	0x4
	.byte	0x33
	.byte	0x7
	.long	0x40
	.byte	0
	.uleb128 0x9
	.long	.LASF14
	.byte	0x4
	.byte	0x36
	.byte	0x9
	.long	0x92
	.byte	0x8
	.uleb128 0x9
	.long	.LASF15
	.byte	0x4
	.byte	0x37
	.byte	0x9
	.long	0x92
	.byte	0x10
	.uleb128 0x9
	.long	.LASF16
	.byte	0x4
	.byte	0x38
	.byte	0x9
	.long	0x92
	.byte	0x18
	.uleb128 0x9
	.long	.LASF17
	.byte	0x4
	.byte	0x39
	.byte	0x9
	.long	0x92
	.byte	0x20
	.uleb128 0x9
	.long	.LASF18
	.byte	0x4
	.byte	0x3a
	.byte	0x9
	.long	0x92
	.byte	0x28
	.uleb128 0x9
	.long	.LASF19
	.byte	0x4
	.byte	0x3b
	.byte	0x9
	.long	0x92
	.byte	0x30
	.uleb128 0x9
	.long	.LASF20
	.byte	0x4
	.byte	0x3c
	.byte	0x9
	.long	0x92
	.byte	0x38
	.uleb128 0x9
	.long	.LASF21
	.byte	0x4
	.byte	0x3d
	.byte	0x9
	.long	0x92
	.byte	0x40
	.uleb128 0x9
	.long	.LASF22
	.byte	0x4
	.byte	0x40
	.byte	0x9
	.long	0x92
	.byte	0x48
	.uleb128 0x9
	.long	.LASF23
	.byte	0x4
	.byte	0x41
	.byte	0x9
	.long	0x92
	.byte	0x50
	.uleb128 0x9
	.long	.LASF24
	.byte	0x4
	.byte	0x42
	.byte	0x9
	.long	0x92
	.byte	0x58
	.uleb128 0x9
	.long	.LASF25
	.byte	0x4
	.byte	0x44
	.byte	0x16
	.long	0x24b
	.byte	0x60
	.uleb128 0x9
	.long	.LASF26
	.byte	0x4
	.byte	0x46
	.byte	0x14
	.long	0x251
	.byte	0x68
	.uleb128 0x9
	.long	.LASF27
	.byte	0x4
	.byte	0x48
	.byte	0x7
	.long	0x40
	.byte	0x70
	.uleb128 0x9
	.long	.LASF28
	.byte	0x4
	.byte	0x49
	.byte	0x7
	.long	0x40
	.byte	0x74
	.uleb128 0x9
	.long	.LASF29
	.byte	0x4
	.byte	0x4a
	.byte	0xb
	.long	0x78
	.byte	0x78
	.uleb128 0x9
	.long	.LASF30
	.byte	0x4
	.byte	0x4d
	.byte	0x12
	.long	0x5c
	.byte	0x80
	.uleb128 0x9
	.long	.LASF31
	.byte	0x4
	.byte	0x4e
	.byte	0xf
	.long	0x6a
	.byte	0x82
	.uleb128 0x9
	.long	.LASF32
	.byte	0x4
	.byte	0x4f
	.byte	0x8
	.long	0x257
	.byte	0x83
	.uleb128 0x9
	.long	.LASF33
	.byte	0x4
	.byte	0x51
	.byte	0xf
	.long	0x267
	.byte	0x88
	.uleb128 0x9
	.long	.LASF34
	.byte	0x4
	.byte	0x59
	.byte	0xd
	.long	0x84
	.byte	0x90
	.uleb128 0x9
	.long	.LASF35
	.byte	0x4
	.byte	0x5b
	.byte	0x17
	.long	0x272
	.byte	0x98
	.uleb128 0x9
	.long	.LASF36
	.byte	0x4
	.byte	0x5c
	.byte	0x19
	.long	0x27d
	.byte	0xa0
	.uleb128 0x9
	.long	.LASF37
	.byte	0x4
	.byte	0x5d
	.byte	0x14
	.long	0x251
	.byte	0xa8
	.uleb128 0x9
	.long	.LASF38
	.byte	0x4
	.byte	0x5e
	.byte	0x9
	.long	0x90
	.byte	0xb0
	.uleb128 0x9
	.long	.LASF39
	.byte	0x4
	.byte	0x5f
	.byte	0xa
	.long	0x2d
	.byte	0xb8
	.uleb128 0x9
	.long	.LASF40
	.byte	0x4
	.byte	0x60
	.byte	0x7
	.long	0x40
	.byte	0xc0
	.uleb128 0x9
	.long	.LASF41
	.byte	0x4
	.byte	0x62
	.byte	0x8
	.long	0x283
	.byte	0xc4
	.byte	0
	.uleb128 0x2
	.long	.LASF42
	.byte	0x5
	.byte	0x7
	.byte	0x19
	.long	0xab
	.uleb128 0xa
	.long	.LASF55
	.byte	0x4
	.byte	0x2b
	.byte	0xe
	.uleb128 0xb
	.long	.LASF43
	.uleb128 0x6
	.byte	0x8
	.long	0x246
	.uleb128 0x6
	.byte	0x8
	.long	0xab
	.uleb128 0xc
	.long	0x98
	.long	0x267
	.uleb128 0xd
	.long	0x39
	.byte	0
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x23e
	.uleb128 0xb
	.long	.LASF44
	.uleb128 0x6
	.byte	0x8
	.long	0x26d
	.uleb128 0xb
	.long	.LASF45
	.uleb128 0x6
	.byte	0x8
	.long	0x278
	.uleb128 0xc
	.long	0x98
	.long	0x293
	.uleb128 0xd
	.long	0x39
	.byte	0x13
	.byte	0
	.uleb128 0xe
	.long	.LASF46
	.byte	0x6
	.byte	0x89
	.byte	0xe
	.long	0x29f
	.uleb128 0x6
	.byte	0x8
	.long	0x232
	.uleb128 0xe
	.long	.LASF47
	.byte	0x6
	.byte	0x8a
	.byte	0xe
	.long	0x29f
	.uleb128 0xe
	.long	.LASF48
	.byte	0x6
	.byte	0x8b
	.byte	0xe
	.long	0x29f
	.uleb128 0xe
	.long	.LASF49
	.byte	0x7
	.byte	0x1a
	.byte	0xc
	.long	0x40
	.uleb128 0xc
	.long	0x2df
	.long	0x2d4
	.uleb128 0xf
	.byte	0
	.uleb128 0x7
	.long	0x2c9
	.uleb128 0x6
	.byte	0x8
	.long	0x9f
	.uleb128 0x7
	.long	0x2d9
	.uleb128 0xe
	.long	.LASF50
	.byte	0x7
	.byte	0x1b
	.byte	0x1a
	.long	0x2d4
	.uleb128 0x10
	.long	.LASF56
	.byte	0x1
	.byte	0x20
	.byte	0x5
	.long	0x40
	.quad	.LFB7
	.quad	.LFE7-.LFB7
	.uleb128 0x1
	.byte	0x9c
	.long	0x361
	.uleb128 0x11
	.string	"a"
	.byte	0x1
	.byte	0x22
	.byte	0x9
	.long	0x40
	.uleb128 0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0x11
	.string	"b"
	.byte	0x1
	.byte	0x23
	.byte	0x9
	.long	0x40
	.uleb128 0x2
	.byte	0x91
	.sleb128 -36
	.uleb128 0x11
	.string	"c"
	.byte	0x1
	.byte	0x24
	.byte	0x9
	.long	0x40
	.uleb128 0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x11
	.string	"d"
	.byte	0x1
	.byte	0x25
	.byte	0x9
	.long	0x40
	.uleb128 0x2
	.byte	0x91
	.sleb128 -28
	.uleb128 0x11
	.string	"e"
	.byte	0x1
	.byte	0x26
	.byte	0x9
	.long	0x40
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x11
	.string	"f"
	.byte	0x1
	.byte	0x27
	.byte	0x9
	.long	0x40
	.uleb128 0x2
	.byte	0x91
	.sleb128 -20
	.byte	0
	.uleb128 0x12
	.long	.LASF57
	.byte	0x1
	.byte	0x8
	.byte	0x5
	.long	0x40
	.quad	.LFB6
	.quad	.LFE6-.LFB6
	.uleb128 0x1
	.byte	0x9c
	.uleb128 0x11
	.string	"a"
	.byte	0x1
	.byte	0x9
	.byte	0x9
	.long	0x40
	.uleb128 0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0x11
	.string	"b"
	.byte	0x1
	.byte	0xa
	.byte	0x9
	.long	0x40
	.uleb128 0x2
	.byte	0x91
	.sleb128 -36
	.uleb128 0x11
	.string	"c"
	.byte	0x1
	.byte	0xb
	.byte	0x9
	.long	0x40
	.uleb128 0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x11
	.string	"d"
	.byte	0x1
	.byte	0xc
	.byte	0x9
	.long	0x40
	.uleb128 0x2
	.byte	0x91
	.sleb128 -28
	.uleb128 0x11
	.string	"e"
	.byte	0x1
	.byte	0xd
	.byte	0x9
	.long	0x40
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x11
	.string	"f"
	.byte	0x1
	.byte	0xe
	.byte	0x9
	.long	0x40
	.uleb128 0x2
	.byte	0x91
	.sleb128 -20
	.byte	0
	.byte	0
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x10
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0x26
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0x13
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3c
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0xf
	.uleb128 0x21
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x10
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2116
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x11
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x12
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2116
	.uleb128 0x19
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_aranges,"",@progbits
	.long	0x2c
	.value	0x2
	.long	.Ldebug_info0
	.byte	0x8
	.byte	0
	.value	0
	.value	0
	.quad	.Ltext0
	.quad	.Letext0-.Ltext0
	.quad	0
	.quad	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF9:
	.string	"__off_t"
.LASF14:
	.string	"_IO_read_ptr"
.LASF26:
	.string	"_chain"
.LASF8:
	.string	"size_t"
.LASF32:
	.string	"_shortbuf"
.LASF20:
	.string	"_IO_buf_base"
.LASF12:
	.string	"long long unsigned int"
.LASF57:
	.string	"doMagicalStuff"
.LASF53:
	.string	"/home/valde/Git/DM501-compiler/asmsnippets"
.LASF2:
	.string	"long long int"
.LASF6:
	.string	"signed char"
.LASF27:
	.string	"_fileno"
.LASF15:
	.string	"_IO_read_end"
.LASF1:
	.string	"long int"
.LASF13:
	.string	"_flags"
.LASF21:
	.string	"_IO_buf_end"
.LASF30:
	.string	"_cur_column"
.LASF44:
	.string	"_IO_codecvt"
.LASF29:
	.string	"_old_offset"
.LASF34:
	.string	"_offset"
.LASF43:
	.string	"_IO_marker"
.LASF46:
	.string	"stdin"
.LASF5:
	.string	"unsigned int"
.LASF38:
	.string	"_freeres_buf"
.LASF0:
	.string	"long unsigned int"
.LASF18:
	.string	"_IO_write_ptr"
.LASF49:
	.string	"sys_nerr"
.LASF4:
	.string	"short unsigned int"
.LASF22:
	.string	"_IO_save_base"
.LASF51:
	.string	"GNU C17 8.2.1 20181127 -mtune=generic -march=x86-64 -g"
.LASF33:
	.string	"_lock"
.LASF28:
	.string	"_flags2"
.LASF40:
	.string	"_mode"
.LASF47:
	.string	"stdout"
.LASF35:
	.string	"_codecvt"
.LASF19:
	.string	"_IO_write_end"
.LASF55:
	.string	"_IO_lock_t"
.LASF54:
	.string	"_IO_FILE"
.LASF50:
	.string	"sys_errlist"
.LASF25:
	.string	"_markers"
.LASF3:
	.string	"unsigned char"
.LASF7:
	.string	"short int"
.LASF45:
	.string	"_IO_wide_data"
.LASF31:
	.string	"_vtable_offset"
.LASF42:
	.string	"FILE"
.LASF11:
	.string	"char"
.LASF10:
	.string	"__off64_t"
.LASF16:
	.string	"_IO_read_base"
.LASF24:
	.string	"_IO_save_end"
.LASF39:
	.string	"__pad5"
.LASF41:
	.string	"_unused2"
.LASF48:
	.string	"stderr"
.LASF52:
	.string	"printComp.c"
.LASF23:
	.string	"_IO_backup_base"
.LASF37:
	.string	"_freeres_list"
.LASF36:
	.string	"_wide_data"
.LASF56:
	.string	"main"
.LASF17:
	.string	"_IO_write_base"
	.ident	"GCC: (GNU) 8.2.1 20181127"
	.section	.note.GNU-stack,"",@progbits
