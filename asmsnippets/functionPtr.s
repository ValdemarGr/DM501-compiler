	.file	"functionPtr.c"
	.text
.Ltext0:
	.section	.rodata
.LC0:
	.string	"%i\n"
	.text
	.globl	doMagicalStuff
	.type	doMagicalStuff, @function
doMagicalStuff:
.LFB6:
	.file 1 "functionPtr.c"
	.loc 1 8 22
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	.loc 1 9 9
	movl	$22, -4(%rbp)
	.loc 1 11 5
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	.loc 1 13 12
	movl	-4(%rbp), %eax
	.loc 1 14 1
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
	.loc 1 16 12
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	.loc 1 18 9
	movl	$1, -16(%rbp)
	.loc 1 20 5
	movl	-16(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	.loc 1 22 6
	addl	$1, -16(%rbp)
	.loc 1 24 11
	leaq	doMagicalStuff(%rip), %rax
	movq	%rax, -8(%rbp)
	.loc 1 26 13
	movq	-8(%rbp), %rdx
	movl	$0, %eax
	call	*%rdx
.LVL0:
	movl	%eax, -12(%rbp)
	.loc 1 28 5
	movl	-12(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	.loc 1 30 12
	movl	$0, %eax
	.loc 1 31 1
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	main, .-main
.Letext0:
	.file 2 "/usr/lib/gcc/x86_64-pc-linux-gnu/8.3.0/include/stddef.h"
	.file 3 "/usr/include/bits/types.h"
	.file 4 "/usr/include/bits/types/struct_FILE.h"
	.file 5 "/usr/include/bits/types/FILE.h"
	.file 6 "/usr/include/stdio.h"
	.file 7 "/usr/include/bits/sys_errlist.h"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.long	0x375
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
	.byte	0x10
	.byte	0x5
	.long	0x40
	.quad	.LFB7
	.quad	.LFE7-.LFB7
	.uleb128 0x1
	.byte	0x9c
	.long	0x33b
	.uleb128 0x11
	.string	"a"
	.byte	0x1
	.byte	0x12
	.byte	0x9
	.long	0x40
	.uleb128 0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x11
	.string	"fp"
	.byte	0x1
	.byte	0x18
	.byte	0xb
	.long	0x346
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x11
	.string	"b"
	.byte	0x1
	.byte	0x1a
	.byte	0x9
	.long	0x40
	.uleb128 0x2
	.byte	0x91
	.sleb128 -28
	.byte	0
	.uleb128 0x12
	.long	0x40
	.long	0x346
	.uleb128 0x13
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x33b
	.uleb128 0x14
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
	.uleb128 0x15
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x13
	.uleb128 0x18
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x14
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
.LASF15:
	.string	"_IO_read_end"
.LASF8:
	.string	"size_t"
.LASF54:
	.string	"_IO_FILE"
.LASF48:
	.string	"stderr"
.LASF17:
	.string	"_IO_write_base"
.LASF21:
	.string	"_IO_buf_end"
.LASF14:
	.string	"_IO_read_ptr"
.LASF39:
	.string	"__pad5"
.LASF5:
	.string	"unsigned int"
.LASF51:
	.string	"GNU C17 8.3.0 -m64 -mtune=generic -march=x86-64 -g"
.LASF34:
	.string	"_offset"
.LASF40:
	.string	"_mode"
.LASF52:
	.string	"functionPtr.c"
.LASF26:
	.string	"_chain"
.LASF22:
	.string	"_IO_save_base"
.LASF3:
	.string	"unsigned char"
.LASF49:
	.string	"sys_nerr"
.LASF0:
	.string	"long unsigned int"
.LASF4:
	.string	"short unsigned int"
.LASF46:
	.string	"stdin"
.LASF24:
	.string	"_IO_save_end"
.LASF55:
	.string	"_IO_lock_t"
.LASF50:
	.string	"sys_errlist"
.LASF25:
	.string	"_markers"
.LASF10:
	.string	"__off64_t"
.LASF56:
	.string	"main"
.LASF42:
	.string	"FILE"
.LASF28:
	.string	"_flags2"
.LASF57:
	.string	"doMagicalStuff"
.LASF29:
	.string	"_old_offset"
.LASF33:
	.string	"_lock"
.LASF43:
	.string	"_IO_marker"
.LASF30:
	.string	"_cur_column"
.LASF12:
	.string	"long long unsigned int"
.LASF44:
	.string	"_IO_codecvt"
.LASF27:
	.string	"_fileno"
.LASF20:
	.string	"_IO_buf_base"
.LASF31:
	.string	"_vtable_offset"
.LASF35:
	.string	"_codecvt"
.LASF37:
	.string	"_freeres_list"
.LASF9:
	.string	"__off_t"
.LASF41:
	.string	"_unused2"
.LASF2:
	.string	"long long int"
.LASF47:
	.string	"stdout"
.LASF11:
	.string	"char"
.LASF53:
	.string	"/home/valde/Git/DM501-compiler/asmsnippets"
.LASF19:
	.string	"_IO_write_end"
.LASF7:
	.string	"short int"
.LASF23:
	.string	"_IO_backup_base"
.LASF13:
	.string	"_flags"
.LASF36:
	.string	"_wide_data"
.LASF1:
	.string	"long int"
.LASF18:
	.string	"_IO_write_ptr"
.LASF45:
	.string	"_IO_wide_data"
.LASF6:
	.string	"signed char"
.LASF38:
	.string	"_freeres_buf"
.LASF16:
	.string	"_IO_read_base"
.LASF32:
	.string	"_shortbuf"
	.ident	"GCC: (GNU) 8.3.0"
	.section	.note.GNU-stack,"",@progbits
