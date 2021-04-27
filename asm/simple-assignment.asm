	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 11, 0	sdk_version 11, 1
	.globl	_main                   ## -- Begin function main
	; Align bytes to 2^4 = 16 bytes. 0x90 is a NOP instruction
	.p2align	4, 0x90
_main:                                  ## @main
## %bb.0:
  ; Call a function by pushing onto the base pointer
	pushq	%rbp
	; Move the current stack pointer to the base pointer
	movq	%rsp, %rbp
	; Store the "int" $0 on the stack, taking up 4 bytes.
	movl	$0, -4(%rbp)
	; int a = 5;
	;
	; Store the value of 5 onto the stack, aligning to 8 bytes.
	movl	$5, -8(%rbp)
	; `return a;`
	;
	; Return the value of `int a` by putting it into the %rax register. Only specify
	; The %rax register.
	movl	-8(%rbp), %eax
	; Restore the base pointer
	popq	%rbp
	; Return the quad value
	retq
                                        ## -- End function
.subsections_via_symbols
