	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 11, 0	sdk_version 11, 1
	.globl	__Z9getNumberv          ## -- Begin function _Z9getNumberv
	.p2align	4, 0x90
__Z9getNumberv:                         ## @_Z9getNumberv
## %bb.0:
  ; Remember the old base pointer.
	pushq	%rbp
	; Store the stack to the base pointer.
	movq	%rsp, %rbp
	; Store 5 in the base of the stack.
	movl	$5, -4(%rbp)
	; Move the stack value into the return register.
	movl	-4(%rbp), %eax
	; Pop off the stored base pointer.
	popq	%rbp
	; Return the q value.
	retq
                                        ## -- End function
	.globl	_main                   ## -- Begin function main
	.p2align	4, 0x90
_main:                                  ## @main
## %bb.0:
  ; Start the main function call.
	pushq	%rbp
	movq	%rsp, %rbp
	; Allocate 16 bytes to the stack.
	subq	$16, %rsp
	; Set the first 4 bytes to 0.
	movl	$0, -4(%rbp)
	; Call out to the getNumber.
	callq	__Z9getNumberv
	; De-allocate the stack by 16 bytes.
	addq	$16, %rsp
	; The return value is already in the %eax, no other action is needed.
	popq	%rbp
	retq
                                        ## -- End function
.subsections_via_symbols
