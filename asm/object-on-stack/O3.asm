	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 11, 0	sdk_version 11, 1
	.globl	_main                   ## -- Begin function main
	.p2align	4, 0x90
_main:                                  ## @main
## %bb.0:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	$5, -8(%rbp)
	leaq	-8(%rbp), %rdi
	callq	__ZN6Object8blackboxEv
	xorl	%eax, %eax
	addq	$16, %rsp
	popq	%rbp
	retq
                                        ## -- End function
	.globl	__ZN6Object8blackboxEv  ## -- Begin function _ZN6Object8blackboxEv
	.weak_def_can_be_hidden	__ZN6Object8blackboxEv
	.p2align	4, 0x90
__ZN6Object8blackboxEv:                 ## @_ZN6Object8blackboxEv
## %bb.0:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	%rdi, -8(%rbp)
	popq	%rbp
	retq
                                        ## -- End function
.subsections_via_symbols
