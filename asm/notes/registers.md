https://cs.brown.edu/courses/cs033/docs/guides/x64_cheatsheet.pdf

# Registers

|--------|-------|-------|-------|-------|
| Bits   | 64    | 32    | 16    | 8     |
| Bytes  | 8     | 4     | 2     | 1     |
| Symbol | q     | l     | w     | b     |
|--------|-------|-------|-------|-------|
| 0      | %rax  | %eax  | %ax   | %al   | Function return value
| 1      | %rcx  | %ecx  | %cx   | %cl   |
| 2      | %rdx  | %edx  | %dx   | %dl   |
| 3      | %rbx  | %ebx  | %bx   | %bl   | Callee-save registers
| 4      | %rsi  | %esi  | %si   | %sil  |
| 5      | %rdi  | %edi  | %di   | %dil  |
| 6      | %rsp  | %esp  | %sp   | %spl  | Stack pointer
| 7      | %rbp  | %ebp  | %bp   | %bpl  | Base pointer (Callee-save registers)
| 8      | %r8   | %r8d  | %r8w  | %r8b  | Call save register
| 9      | %r9   | %r9d  | %r9w  | %r9b  | Call save register
| 10     | %r10  | %r10d | %r10w | %r10b | Call save register
| 11     | %r11  | %r11d | %r11w | %r11b | Call save register
| 12     | %r12  | %r12d | %r12w | %r12b | Callee-save registers
| 13     | %r13  | %r13d | %r13w | %r13b | Callee-save registers
| 13     | %r13  | %r13d | %r13w | %r13b | Callee-save registers
| 14     | %r14  | %r14d | %r14w | %r14b | Callee-save registers
| 15     | %r15  | %r15d | %r15w | %r15b | Callee-save registers
|--------|-------|-------|-------|-------|

# Registers 8 byte, 64 bit

%rax
%rcx
%rdx
%rbx
%rsi
%rdi
%rsp - Stack Pointer
%rbp - Frame pointer
%r8
%r9
%r10
%r11
%r12
%r13
%r13
%r14
%r15

# Registers 4 byte, 32 bit

%eax
%ecx
%edx
%ebx
%esi
%edi
%esp
%ebp
%r8d  - D signifies "double word"
%r9d
%r10d
%r11d
%r12d
%r13d
%r13d
%r14d
%r15d

# Registers 2 byte, 16 bits

%ax
%cx
%dx
%bx
%si
%di
%sp
%bp
%r8w - W signifies "word"
%r9w
%r10w
%r11w
%r12w
%r13w
%r13w
%r14w
%r15w

# Registers 1 byte, 8 bits

%al
%cl
%dl
%bl
%sil
%dil
%spl
%bpl
%r8b - B signifies "byte"
%r9b
%r10b
%r11b
%r12b
%r13b
%r13b
%r14b
%r15b
