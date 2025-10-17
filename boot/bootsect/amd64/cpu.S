/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            boot/bootsect/amd64/cpu.S
 * DESCRIPTION:     Low-level support for CPU initialization
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */


BuildPageMap:
    /* Generate page map for first 1GB of memory */
    pushaw
    pushw %es
    cld
    movw $(0x1000 / 16), %ax
    movw %ax, %es
    xorw %di, %di
    movl $(0x2000 | 0x07), %eax
    stosl
    xorl %eax, %eax
    movw $1021, %cx
    rep stosl
    movw $(0x2000 / 16), %ax
    movw %ax, %es
    xorw %di, %di
    movl $(0x3000 | 0x07), %eax
    stosl
    xorl %eax, %eax
    movw $1021, %cx
    rep stosl
    movw $(0x3000 / 16), %ax
    movw %ax, %es
    xorw %di, %di
    movw $512, %cx
    movl $0x00000083, %eax
.BuildPageMapLoop:
    /* Identity map 512 pages of 2MB */
    movl %eax, %es:(%di)
    addl $2097152, %eax
    addw $0x08, %di
    loop .BuildPageMapLoop
    popw %es
    popaw
    ret

InitializeCpu:
    /* Check if CPU supports CPUID, long mode and PAE */
    pushal
    pushfl
    popl %eax
    movl %eax, %ebx
    xorl $0x00200000, %eax
    pushl %eax
    popfl
    pushfl
    popl %eax
    cmpl %ebx, %eax
    je CpuUnsupported
    movl $0x01, %eax
    cpuid
    testl $0x40, %edx
    jz CpuUnsupported
    movl $0x80000000, %eax
    cpuid
    cmpl $0x80000000, %eax
    jbe CpuUnsupported
    movl $0x80000001, %eax
    cpuid
    testl $0x20000000, %edx
    jz CpuUnsupported
    popal
    call LoadGdt
    ret

LoadGdt:
    /* Load Global Descriptor Table */
    lgdt .GdtPointer
    ret

RunStage2:
    /* Switch to long mode and pass control to Stage 2 */
    call BuildPageMap
    call ParseExecutableHeader
    xorl %edx, %edx
    pushl %edx
    pushl %eax
    cli
    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss
    movl %cr4, %eax
    orl $0x00A0, %eax
    movl %eax, %cr4
    movl $0x00001000, %eax
    movl %eax, %cr3
    movl $0xC0000080, %ecx
    rdmsr
    orl $0x00000100, %eax
    wrmsr
    movl %cr0, %eax
    orl $0x80000001, %eax
    movl %eax, %cr0
    ljmp $0x10, $.Stage2LongMode
.code64
.Stage2LongMode:
    /* Set segments and stack, then jump to Stage 2 */
    movw $0x18, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    xorw %ax, %ax
    movw %ax, %fs
    movw %ax, %gs
    popq %rax
    xorq %rbx, %rbx
    xorq %rcx, %rcx
    xorq %rdx, %rdx
    xorq %rsi, %rsi
    xorq %rdi, %rdi
    xorq %rbp, %rbp
    jmp *%rax

.code16
.GdtDescriptor:
    /* Global Descriptor Table */
    .quad 0x0000000000000000
    .quad 0x0000000000000000
    .quad 0x00AF9A000000FFFF
    .quad 0x00CF92000000FFFF
    .quad 0x00009E000000FFFF
    .quad 0x000092000000FFFF
    .quad 0x00CF9B000000FFFF

.GdtPointer:
    /* Pointer to Global Descriptor Table */
    .word .GdtPointer - .GdtDescriptor - 1
    .long .GdtDescriptor

.Stage2FileName:
    /* Name of Stage 2 executable file */
    .ascii "BOOTX64 EFI"
