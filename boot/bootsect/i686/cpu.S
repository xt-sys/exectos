/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            boot/bootsect/i686/cpu.S
 * DESCRIPTION:     Low-level support for CPU initialization
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */


BuildPageMap:
    /* Generate page map for first 16MB of memory */
    pushaw
    pushw %es
    cld
    movw $(0x1000 >> 0x04), %ax
    movw %ax, %es
    xorw %di, %di
    movl $(0x2000 | 0x03), %eax
    stosl
    movl $(0x3000 | 0x03), %eax
    stosl
    movl $(0x4000 | 0x03), %eax
    stosl
    movl $(0x5000 | 0x03), %eax
    stosl
    xorl %eax, %eax
    movw $(1024 - 4), %cx
    rep stosl
    movl $0x00000003, %eax
    movl $4, %edx
    movw $(0x2000 >> 0x04), %bx
.BuildPageMapLoop:
    /* Identity map 1024 pages of 4KB */
    movw %bx, %es
    xorw %di, %di
    pushl %edx
    movw $1024, %cx
.FillPageMapTable:
    /* Fill the page table */
    movl %eax, %es:(%di)
    addl $4096, %eax
    addw $0x04, %di
    loop .FillPageMapTable
    popl %edx
    addw $(0x1000 >> 0x04), %bx
    decl %edx
    jnz .BuildPageMapLoop
    popw %es
    popaw
    ret

InitializeCpu:
    /* Check if CPU supports CPUID */
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
    popal
    call LoadGdt
    ret

LoadGdt:
    /* Load Global Descriptor Table */
    lgdt .GdtPointer
    ret

RunStage2:
    /* Switch to protected mode and pass control to Stage 2 */
    call BuildPageMap
    call ParseExecutableHeader
    pushl %eax
    cli
    movl %cr0, %eax
    orl $0x01, %eax
    movl %eax, %cr0
    ljmp $0x08, $.Stage2ProtectedMode
.code32
.Stage2ProtectedMode:
    /* Set segments and stack, then jump to Stage 2 */
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    xorw %ax, %ax
    movw %ax, %fs
    movw %ax, %gs
    popl %eax
    xorl %ebx, %ebx
    xorl %ecx, %ecx
    xorl %edx, %edx
    xorl %esi, %esi
    xorl %edi, %edi
    xorl %ebp, %ebp
    movl $0x1000, %ebx
    movl %ebx, %cr3
    movl %cr0, %ebx
    orl $0x80000000, %ebx
    movl %ebx, %cr0
    jmp *%eax

.code16
.GdtDescriptor:
    /* Global Descriptor Table */
    .quad 0x0000000000000000
    .quad 0x00CF9A000000FFFF
    .quad 0x00CF92000000FFFF
    .quad 0x00009E000000FFFF
    .quad 0x000092000000FFFF

.GdtPointer:
    /* Pointer to Global Descriptor Table */
    .word .GdtPointer - .GdtDescriptor - 1
    .long .GdtDescriptor

.Stage2FileName:
    /* Name of Stage 2 executable file */
    .ascii "BOOTIA32EFI"
