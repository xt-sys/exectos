/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ar/amd64/boot.S
 * DESCRIPTION:     AMD64-specific boot code for setting up the low-level CPU environment
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <ar/amd64/asmsup.h>

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
 * Starts an application processor (AP). This is just a stub.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
.global ArStartApplicationProcessor
ArStartApplicationProcessor:

.global ArStartApplicationProcessorEnd
ArStartApplicationProcessorEnd:
