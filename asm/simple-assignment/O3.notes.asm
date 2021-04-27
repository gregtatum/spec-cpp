	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 11, 0	sdk_version 11, 1
	.globl	_main                   ## -- Begin function main
	.p2align	4, 0x90
_main:                                  ## @main
## %bb.0:
	; Start a function call
	pushq	%rbp
	movq	%rsp, %rbp
	; Move the value 5 directly to the return. This is optimized compared to the original
	; code of int a = 5; This is equivalent to `return 5`
	movl	$5, %eax
	popq	%rbp
	retq
                                        ## -- End function
.subsections_via_symbols
