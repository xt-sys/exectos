/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ar/amd64/archsup.S
 * DESCRIPTION:     Provides AMD64 architecture features not implementable in C
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#include <amd64/asmsup.h>

.altmacro
.text


/**
 * Enables eXtended Physical Addressing (XPA).
 *
 * @param PageMap
 *        Supplies a pointer to the page map to be used.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
.global ArEnableExtendedPhysicalAddressing
ArEnableExtendedPhysicalAddressing:
    /* Save the original CR4 register */
    movq %cr4, %rax

    /* Save the state of stack pointer and non-volatile registers */
    movq %rsp, XpaRegisterSaveArea(%rip)
    movq %rbp, XpaRegisterSaveArea+0x08(%rip)
    movq %rax, XpaRegisterSaveArea+0x10(%rip)
    movq %rbx, XpaRegisterSaveArea+0x18(%rip)

    /* Save the original CR0 register */
    movq %cr0, %rbp

    /* Load temporary GDT required for mode transitions */
    leaq XpaTemporaryGdtDesc(%rip), %rax
    movq %rax, XpaTemporaryGdtBase(%rip)
    lgdtq XpaTemporaryGdtSize(%rip)

    /* Load addresses for entering compatibility mode and re-entering long mode */
    leaq XpaEnterCompatMode(%rip), %rax
    leaq XpaEnterLongMode(%rip), %rbx

    /* Push the 32-bit code segment selector and the target address for a far jump */
    pushq $GDT_R0_CMCODE
    pushq %rax

    /* Perform a far return to switch to 32-bit compatibility mode */
    lretq

XpaEnterCompatMode:
    /* Enter 32-bit compatibility mode */
    .code32

    /* Store the PageMap pointer on the stack for future use */
    pushl %ecx

    /* Set the stack segment to the 32-bit data segment selector */
    movl $GDT_R0_DATA, %eax
    movl %eax, %ss

    /* Disable PGE and PCIDE to ensure all TLB entries will be flushed */
    movl %cr4, %eax
    andl $~(CR4_PGE | CR4_PCIDE), %eax
    movl %eax, %cr4

    /* Temporarily disable paging */
    movl %ebp, %eax
    andl $~CR0_PG, %eax
    movl %eax, %cr0

    /* Disable Long Mode as prerequisite for enabling 5-level paging */
    movl $X86_MSR_EFER, %ecx
    rdmsr
    andl $~X86_MSR_EFER_LME, %eax
    wrmsr

    /* Transition to 5-level paging (PML5/LA57) */
    movl %cr4, %eax
    orl $CR4_LA57, %eax
    movl %eax, %cr4

    /* Restore the PageMap pointer from the stack and load it into CR3 */
    popl %ecx
    movl %ecx, %cr3

    /* Re-enable Long Mode */
    movl $X86_MSR_EFER, %ecx
    rdmsr
    orl $X86_MSR_EFER_LME, %eax
    wrmsr

    /* Restore CR0 with paging enabled and flush the instruction pipeline */
    movl %ebp, %cr0
    call XpaFlushInstructions

XpaFlushInstructions:
    /* Push the 64-bit code segment selector and the target address for a far jump */
    pushl $GDT_R0_CODE
    pushl %ebx

    /* Perform a far return to switch to 64-bit long mode */
    lretl

XpaEnterLongMode:
    /* Enter 64-bit long mode */
    .code64

    /* Restore the stack pointer and non-volatile registers */
    movq XpaRegisterSaveArea(%rip), %rsp
    movq XpaRegisterSaveArea+8(%rip), %rbp
    movq XpaRegisterSaveArea+0x10(%rip), %rax
    movq XpaRegisterSaveArea+0x18(%rip), %rbx

    /* Restore the original CR4 register with LA57 bit set */
    orq $CR4_LA57, %rax
    movq %rax, %cr4

    /* Return to the caller */
    retq

/* Data section for saving registers and temporary GDT */
XpaRegisterSaveArea: .quad 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000
XpaTemporaryGdtSize: .short ArEnableExtendedPhysicalAddressingEnd - XpaTemporaryGdtDesc - 1
XpaTemporaryGdtBase: .quad 0x0000000000000000
XpaTemporaryGdtDesc: .quad 0x0000000000000000, 0x00CF9A000000FFFF, 0x00AF9A000000FFFF, 0x00CF92000000FFFF

.global ArEnableExtendedPhysicalAddressingEnd
ArEnableExtendedPhysicalAddressingEnd:

/**
 * Creates a trap handler for the specified vector.
 *
 * @param Vector
 *        Supplies a trap vector number.
 *
 * @return This macro does not return any value.
 *
 * @since XT 1.0
 */
.macro ArpCreateTrapHandler Vector
.global ArpTrap\Vector
ArpTrap\Vector:
    /* Push fake error code for non-error vectors */
    .if \Vector != 8 && \Vector != 10 && \Vector != 11 && \Vector != 12 && \Vector != 13 && \Vector != 14 && \Vector != 17 && \Vector != 30
        push $0
    .endif

    /* Push vector number */
    push $\Vector

    /* Push General Purpose Registers */
    push %rbp
    push %rdi
    push %rsi
    push %r15
    push %r14
    push %r13
    push %r12
    push %r11
    push %r10
    push %r9
    push %r8
    push %rdx
    push %rcx
    push %rbx
    push %rax

    /* Reserve space for other registers and point RBP to the trap frame */
    sub $(TRAP_FRAME_SIZE - TRAP_REGISTERS_SIZE), %rsp
    lea (%rsp), %rbp

    /* Store segment selectors */
    mov %gs, TrapSegGs(%rbp)
    mov %fs, TrapSegFs(%rbp)
    mov %es, TrapSegEs(%rbp)
    mov %ds, TrapSegDs(%rbp)

    /* Store debug registers */
    mov %dr7, %rax
    mov %rax, TrapDr7(%rbp)
    mov %dr6, %rax
    mov %rax, TrapDr6(%rbp)
    mov %dr3, %rax
    mov %rax, TrapDr3(%rbp)
    mov %dr2, %rax
    mov %rax, TrapDr2(%rbp)
    mov %dr1, %rax
    mov %rax, TrapDr1(%rbp)
    mov %dr0, %rax
    mov %rax, TrapDr0(%rbp)

    /* Store CR2 and CR3 */
    mov %cr3, %rax
    mov %rax, TrapCr3(%rbp)
    mov %cr2, %rax
    mov %rax, TrapCr2(%rbp)

    /* Store MxCsr register */
    stmxcsr TrapMxCsr(%rbp)

    /* Store XMM registers */
    movdqa %xmm15, TrapXmm15(%rbp)
    movdqa %xmm14, TrapXmm14(%rbp)
    movdqa %xmm13, TrapXmm13(%rbp)
    movdqa %xmm12, TrapXmm12(%rbp)
    movdqa %xmm11, TrapXmm11(%rbp)
    movdqa %xmm10, TrapXmm10(%rbp)
    movdqa %xmm9, TrapXmm9(%rbp)
    movdqa %xmm8, TrapXmm8(%rbp)
    movdqa %xmm7, TrapXmm7(%rbp)
    movdqa %xmm6, TrapXmm6(%rbp)
    movdqa %xmm5, TrapXmm5(%rbp)
    movdqa %xmm4, TrapXmm4(%rbp)
    movdqa %xmm3, TrapXmm3(%rbp)
    movdqa %xmm2, TrapXmm2(%rbp)
    movdqa %xmm1, TrapXmm1(%rbp)
    movdqa %xmm0, TrapXmm0(%rbp)

    /* Test previous mode and swap GS if needed */
    movl $0, TrapPreviousMode(%rbp)
    mov %cs, %ax
    and $1, %al
    mov %al, TrapPreviousMode(%rbp)
    jz KernelMode$\Vector
    swapgs

KernelMode$\Vector:
    /* Push Frame Pointer, clear direction flag and pass to trap dispatcher */
    mov %rsp, %rcx
    cld
    call ArpDispatchTrap

    /* Test previous mode and swapgs if needed */
    testb $1, TrapPreviousMode(%rbp)
    jz KernelModeReturn$\Vector
    cli
    swapgs

KernelModeReturn$\Vector:
    /* Restore XMM registers */
    movdqa TrapXmm0(%rbp), %xmm0
    movdqa TrapXmm1(%rbp), %xmm1
    movdqa TrapXmm2(%rbp), %xmm2
    movdqa TrapXmm3(%rbp), %xmm3
    movdqa TrapXmm4(%rbp), %xmm4
    movdqa TrapXmm5(%rbp), %xmm5
    movdqa TrapXmm6(%rbp), %xmm6
    movdqa TrapXmm7(%rbp), %xmm7
    movdqa TrapXmm8(%rbp), %xmm8
    movdqa TrapXmm9(%rbp), %xmm9
    movdqa TrapXmm10(%rbp), %xmm10
    movdqa TrapXmm11(%rbp), %xmm11
    movdqa TrapXmm12(%rbp), %xmm12
    movdqa TrapXmm13(%rbp), %xmm13
    movdqa TrapXmm14(%rbp), %xmm14
    movdqa TrapXmm15(%rbp), %xmm15

    /* Load MxCsr register */
    ldmxcsr TrapMxCsr(%rbp)

    /* Restore segment selectors */
    mov TrapSegDs(%rbp), %ds
    mov TrapSegEs(%rbp), %es
    mov TrapSegFs(%rbp), %fs

    /* Free stack space */
    add $(TRAP_FRAME_SIZE - TRAP_REGISTERS_SIZE), %rsp

    /* Pop General Purpose Registers */
    pop %rax
    pop %rbx
    pop %rcx
    pop %rdx
    pop %r8
    pop %r9
    pop %r10
    pop %r11
    pop %r12
    pop %r13
    pop %r14
    pop %r15
    pop %rsi
    pop %rdi
    pop %rbp

    /* Skip error code and vector number, then return */
    add $(2 * 8), %rsp
    iretq
.endm

/* Populate common trap handlers */
.irp i,0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
    .irp j,0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
        ArpCreateTrapHandler 0x\i\j
    .endr
.endr
