	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 11, 0	sdk_version 11, 1
	.globl	__Z9getNumberv          ## -- Begin function _Z9getNumberv
	.p2align	4, 0x90
__Z9getNumberv:                         ## @_Z9getNumberv
## %bb.0:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	$5, %eax
	popq	%rbp
	retq
                                        ## -- End function
	.globl	_main                   ## -- Begin function main
	.p2align	4, 0x90
_main:                                  ## @main
## %bb.0:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	$5, %eax
	popq	%rbp
	retq
                                        ## -- End function
.subsections_via_symbols
