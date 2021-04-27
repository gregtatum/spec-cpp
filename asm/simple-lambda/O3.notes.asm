	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 11, 0	sdk_version 11, 1
	.globl	_main                   ## -- Begin function main
	.p2align	4, 0x90
_main:                                  ## @main
## %bb.0:
	pushq	%rbp
	movq	%rsp, %rbp
	; The lambda is optimized out.
	movl	$5, %eax
	popq	%rbp
	retq
                                        ## -- End function
.subsections_via_symbols
