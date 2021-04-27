	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 11, 0	sdk_version 11, 1
	.globl	_main                   ## -- Begin function main
	.p2align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	; Stack unwinding directives are here for some reason is still here:
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp

	; Allocate 16 bytes. This matches the alignment according to .p2align.
	subq	$16, %rsp
	; Store 0 at the beginning.
	movl	$0, -4(%rbp)
	; Move a single byte to the %al register. No idea. This feature does not seem to be
	; excercised.
	movb	l___const.main.getNumber(%rip), %al
	; Now move it to the stack? lol
	movb	%al, -8(%rbp)
	; Move something from the stack to %rdi
	leaq	-8(%rbp), %rdi
	; Now call the function.
	callq	__ZZ4mainENK3$_0clEv
	; %eax has our result.
	; De-allocate the stack stuff.
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4, 0x90         ## -- Begin function _ZZ4mainENK3$_0clEv

; The lambda has a generated label name:
__ZZ4mainENK3$_0clEv:                   ## @"_ZZ4mainENK3$_0clEv"
## %bb.0:
	; It begins like normal.
	pushq	%rbp
	movq	%rsp, %rbp
	; Take the %rdi register, and store it in the stack. This is controlled by the caller.
	; This is not used?
	movq	%rdi, -8(%rbp)
	; Store the 5 on the stack.
	movl	$5, -12(%rbp)
	; Move that stack value to the return value
	movl	-12(%rbp), %eax
	popq	%rbp
	retq
                                        ## -- End function
	.section	__TEXT,__const
l___const.main.getNumber:               ## @__const.main.getNumber
	.space	1

.subsections_via_symbols
