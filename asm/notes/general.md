; Move an integer value of 3 to the %rax register
movq $3, %rax

; Move an integer value of 3 to the address stored in the %rax register
movq $3, (%rax)

; Move an integer value of 3
; To the address %rax
; Offset by the value in %rcx * 8
movq $3, (%rax, %rcx, 8)

; Calling a function will need to allocate space
; for the stack. This can be done by subtracting
; from the stack pointer, e.g.
subq  $0x18, $rsp
