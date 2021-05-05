	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 11, 0	sdk_version 11, 1
	.globl	_main                   ## -- Begin function main
	.p2align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	leaq	-8(%rbp), %rdi
	callq	__ZN6ObjectC1Ev
	leaq	-8(%rbp), %rdi
	callq	__ZN6Object8blackboxEv
	xorl	%eax, %eax
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	__ZN6ObjectC1Ev         ## -- Begin function _ZN6ObjectC1Ev
	.weak_def_can_be_hidden	__ZN6ObjectC1Ev
	.p2align	4, 0x90
__ZN6ObjectC1Ev:                        ## @_ZN6ObjectC1Ev
## %bb.0:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdi
	callq	__ZN6ObjectC2Ev
	addq	$16, %rsp
	popq	%rbp
	retq
                                        ## -- End function
	.globl	__ZN6Object8blackboxEv  ## -- Begin function _ZN6Object8blackboxEv
	.weak_definition	__ZN6Object8blackboxEv
	.p2align	4, 0x90
__ZN6Object8blackboxEv:                 ## @_ZN6Object8blackboxEv
## %bb.0:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	%rdi, -8(%rbp)
	popq	%rbp
	retq
                                        ## -- End function
	.globl	__ZN6ObjectC2Ev         ## -- Begin function _ZN6ObjectC2Ev
	.weak_def_can_be_hidden	__ZN6ObjectC2Ev
	.p2align	4, 0x90
__ZN6ObjectC2Ev:                        ## @_ZN6ObjectC2Ev
## %bb.0:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	$5, (%rax)
	popq	%rbp
	retq
                                        ## -- End function
.subsections_via_symbols
