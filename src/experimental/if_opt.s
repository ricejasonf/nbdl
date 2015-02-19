	.file	"if_opt.cpp"
	.text
	.type	_Z3log8LogLevelPKc.part.1, @function
_Z3log8LogLevelPKc.part.1:
.LFB1403:
	.cfi_startproc
	pushq	%rax
	.cfi_def_cfa_offset 16
	movq	%rdi, %rsi
	movl	$_ZSt4cout, %edi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	popq	%rdx
	.cfi_def_cfa_offset 8
	movq	%rax, %rdi
	jmp	_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_
	.cfi_endproc
.LFE1403:
	.size	_Z3log8LogLevelPKc.part.1, .-_Z3log8LogLevelPKc.part.1
	.globl	_Z3log8LogLevelPKc
	.type	_Z3log8LogLevelPKc, @function
_Z3log8LogLevelPKc:
.LFB1244:
	.cfi_startproc
	cmpl	$2, %edi
	jg	.L3
	movq	%rsi, %rdi
	jmp	_Z3log8LogLevelPKc.part.1
.L3:
	ret
	.cfi_endproc
.LFE1244:
	.size	_Z3log8LogLevelPKc, .-_Z3log8LogLevelPKc
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"debug message"
	.section	.text.startup,"ax",@progbits
	.globl	main
	.type	main, @function
main:
.LFB1245:
	.cfi_startproc
	pushq	%rcx
	.cfi_def_cfa_offset 16
	movl	$.LC0, %edi
	call	_Z3log8LogLevelPKc.part.1
	xorl	%eax, %eax
	popq	%rsi
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE1245:
	.size	main, .-main
	.type	_GLOBAL__sub_I_logLevel, @function
_GLOBAL__sub_I_logLevel:
.LFB1401:
	.cfi_startproc
	pushq	%rdi
	.cfi_def_cfa_offset 16
	movl	$_ZStL8__ioinit, %edi
	call	_ZNSt8ios_base4InitC1Ev
	popq	%r8
	.cfi_def_cfa_offset 8
	movl	$__dso_handle, %edx
	movl	$_ZStL8__ioinit, %esi
	movl	$_ZNSt8ios_base4InitD1Ev, %edi
	jmp	__cxa_atexit
	.cfi_endproc
.LFE1401:
	.size	_GLOBAL__sub_I_logLevel, .-_GLOBAL__sub_I_logLevel
	.section	.init_array,"aw"
	.align 8
	.quad	_GLOBAL__sub_I_logLevel
	.globl	logLevel
	.section	.rodata
	.align 4
	.type	logLevel, @object
	.size	logLevel, 4
logLevel:
	.long	2
	.local	_ZStL8__ioinit
	.comm	_ZStL8__ioinit,1,1
	.hidden	__dso_handle
	.ident	"GCC: (Ubuntu/Linaro 4.8.1-10ubuntu9) 4.8.1"
	.section	.note.GNU-stack,"",@progbits
