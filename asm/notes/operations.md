# Byte size in operations

e.g with mov

movb - Move a byte (1 byte)
movw - Move a word (2 bytes)
movl - Move a long (4 bytes)
movq - Move a quad (8 bytes)

# Operations
S := source
D := destination
k := value

Example suffixes:

movq %rax, %rbx  ; Move an 8 byte "quadword" from %rax to %rbx registers
movzbl %a1, %ebx ; movz__  Move with zero padding.
                  ; ____bl  Go from a "b" single byte, to a "l" double word.
                  ; %a1 is a single byte register
                  ; %ebx is a "d" double word.

## Data movement

mov  S,D  Move source to destination
push S    Push source onto stack
pop  D    Pop top of stack into destination
cwtl      Convert word in %ax to doubleword in %eax (sign-extended)
cltq      Convert doubleword in %ea to quadword in %rax (sign-extended)
cqto      Convert quadword in %rax to octoword in %rdx:%rax

## Unary Operations

inc  D    increment by 1
dec  D    decrement by 1
neg  D    arithmetic negation
not  D    bitwise complement

## Binary Operations

leaq S,D  Load effective address of source into destination
add  S,D  Add source to destination
sub  S,D  Subtract source from destination
imul S,D  Multiply destination by source
xor  S,D  Bitwise XOR destination by source
or   S,D  Bitwise OR destination by source
and  S,D  Bitwise AND destination by source

## Shift Operations

sal  k,D  Left shift destination by k bits
shl  k,D  "
sar  k,D  Arithmetic right shift destination by k bits
shr  k,D  Logical right shift destination by k bits

## Special Arithmetic Operations

imulq S  Signed full multiply of %rax by S. Result stored in %rdx:%rax
mulq  S  Unsigned full multiply of %rax by S. Result stored in %rdx:%rax
idivq S  Signed divide %rdx:%rax by S
         Quotient stored in %rax
         Remainder stored in %rdx
idivq S  Unsigned divide %rdx:%rax by S
         Quotient stored in %rax
         Remainder stored in %rdx

## Comparison and Test Instructions

cmp  S2,S1  Set condition codes according to S1 - S2
test S2,S1  Set condition codes according to S1 & S2

## Conditional Set Instructions

                                                    CONDITION CODE
sete / setz    D  Set if equal/zero                 ZF
setne / setnz  D  Set if not equal/nonzero          ~ZF
sets           D  Set if negative                   SF
setns          D  Set if nonnegative                ~SF
setg / setnle  D  Set if greater (signed)           ~(SF^0F)&~ZF
setl / setnge  D  Set if greater or equal (signed)  ~(SF ^0F)
setl / setnge  D  Set if less (signed)              SF^0F
setle / setng  D  Set if less or equal              (SF^0F)|ZF
seta / setnbe  D  Set if above (unsigned)           ~CF&~ZF
setae / setnb  D  Set if above or equal (unsigned)  ~CF
setb / setnae  D  Set if below (unsigned)           CF
setbe / setna  D  Set if below or equal (unsigned)  CF|ZF

## Jump Instructions

jmp        Label     Jump if to label
jmp        *Operand  Jump if to specified location
je / jz    Label     Jump if equal/zero
jne / jnz  Label     Jump if not equal/nonzero
js         Label     Jump if negative
jns        Label     Jump if nonnegative
jg / jnle  Label     Jump if greater          (signed)
jge / jnl  Label     Jump if greater or equal (signed)
jl / jnge  Label     Jump if less             (signed)
jle / jng  Label     Jump if less or equal    (signed)
ja / jnbe  Label     Jump if above            (unsigned)
jae / jnb  Label     Jump if above or equal   (unsigned)
jb / jnae  Label     Jump if below            (unsigned)
jbe / jna  Label     Jump if below or equal   (unsigned)

## Conditional Move

cmove / cmovz     S,D  Move if equal/zero
cmovene / cmovnz  S,D  Move if not equal/nonzero
cmovs             S,D  Move if negative
cmovns            S,D  Move if nonnegative
cmovg / cmovnle   S,D  Move if greater          (signed)
cmovge / cmovnl   S,D  Move if greator or equal (signed)
cmovl / cmovnge   S,D  Move if less             (signed)
cmovle / cmovng   S,D  Move if less or equal    (signed)
cmova / cmovnbe   S,D  Move if above            (unsigned)
cmovae / cmovnb   S,D  Move if above or equal   (unsigned)
cmovb / cmovnae   S,D  Move if below            (unsigned)
cmovbe / cmovna   S,D  Move if below or equal   (unsigned)

## Procedure Call Instruction

call   Label     Push return address and jump to label
call   *Operand  Push return address and jump to specified location
leave            Set %rsp to %rbp, then pop to of stack into %rbp
ret              Pop return address from stack and jump there
