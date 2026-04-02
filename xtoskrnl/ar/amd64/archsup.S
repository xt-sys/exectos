/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ar/amd64/archsup.S
 * DESCRIPTION:     Provides AMD64 architecture features not implementable in C
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#include <xtkmapi.h>
#include <xtadk.h>

.altmacro
.text


/**
 * Creates a trap or interrupt handler for the specified vector.
 *
 * @param Vector
 *        Supplies a trap/interrupt vector number.
 *
 * @param Type
 *        Specifies whether the handler is designed to handle an interrupt or a trap.
 *
 * @return This macro does not return any value.
 *
 * @since XT 1.0
 */
.macro ArCreateHandler Vector Type
.global Ar\Type\Vector
Ar\Type\Vector:
    /* Check handler type */
    .ifc \Type,Trap
        /* Push fake error code for non-error vector traps */
        .if \Vector != 8 && \Vector != 10 && \Vector != 11 && \Vector != 12 && \Vector != 13 && \Vector != 14 && \Vector != 17 && \Vector != 30
            push $0
        .endif
    .else
        /* Push fake error code for interrupts */
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
    sub $(KTRAP_FRAME_SIZE - KTRAP_FRAME_REGISTERS_SIZE), %rsp
    lea (%rsp), %rbp

    /* Store segment selectors */
    mov %gs, KTRAP_FRAME_SegGs(%rbp)
    mov %fs, KTRAP_FRAME_SegFs(%rbp)
    mov %es, KTRAP_FRAME_SegEs(%rbp)
    mov %ds, KTRAP_FRAME_SegDs(%rbp)

    /* Store debug registers */
    mov %dr7, %rax
    mov %rax, KTRAP_FRAME_Dr7(%rbp)
    mov %dr6, %rax
    mov %rax, KTRAP_FRAME_Dr6(%rbp)
    mov %dr3, %rax
    mov %rax, KTRAP_FRAME_Dr3(%rbp)
    mov %dr2, %rax
    mov %rax, KTRAP_FRAME_Dr2(%rbp)
    mov %dr1, %rax
    mov %rax, KTRAP_FRAME_Dr1(%rbp)
    mov %dr0, %rax
    mov %rax, KTRAP_FRAME_Dr0(%rbp)

    /* Store CR2 and CR3 */
    mov %cr3, %rax
    mov %rax, KTRAP_FRAME_Cr3(%rbp)
    mov %cr2, %rax
    mov %rax, KTRAP_FRAME_Cr2(%rbp)

    /* Store MxCsr register */
    stmxcsr KTRAP_FRAME_MxCsr(%rbp)

    /* Store XMM registers */
    movdqa %xmm15, KTRAP_FRAME_Xmm15(%rbp)
    movdqa %xmm14, KTRAP_FRAME_Xmm14(%rbp)
    movdqa %xmm13, KTRAP_FRAME_Xmm13(%rbp)
    movdqa %xmm12, KTRAP_FRAME_Xmm12(%rbp)
    movdqa %xmm11, KTRAP_FRAME_Xmm11(%rbp)
    movdqa %xmm10, KTRAP_FRAME_Xmm10(%rbp)
    movdqa %xmm9, KTRAP_FRAME_Xmm9(%rbp)
    movdqa %xmm8, KTRAP_FRAME_Xmm8(%rbp)
    movdqa %xmm7, KTRAP_FRAME_Xmm7(%rbp)
    movdqa %xmm6, KTRAP_FRAME_Xmm6(%rbp)
    movdqa %xmm5, KTRAP_FRAME_Xmm5(%rbp)
    movdqa %xmm4, KTRAP_FRAME_Xmm4(%rbp)
    movdqa %xmm3, KTRAP_FRAME_Xmm3(%rbp)
    movdqa %xmm2, KTRAP_FRAME_Xmm2(%rbp)
    movdqa %xmm1, KTRAP_FRAME_Xmm1(%rbp)
    movdqa %xmm0, KTRAP_FRAME_Xmm0(%rbp)

    /* Test previous mode and swap GS if needed */
    movl $0, KTRAP_FRAME_PreviousMode(%rbp)
    mov KTRAP_FRAME_SegCs(%rbp), %ax
    and $3, %al
    mov %al, KTRAP_FRAME_PreviousMode(%rbp)

    /* Skip swapgs as the interrupt originated from kernel mode */
    jz UserMode\Type\Vector

    swapgs

UserMode\Type\Vector:
    /* Set up trap frame pointer for the dispatcher and clear the direction flag */
    mov %rsp, %rcx
    cld

    /* Preserve the original stack pointer */
    mov %rsp, %rbx

    /* Force stack alignment */
    and $-16, %rsp

    /* Allocate 32 bytes of shadow space */
    sub $32, %rsp

    .ifc \Type,Trap
        /* Pass to the trap dispatcher */
        call ArDispatchTrap
    .else
        /* Pass to the interrupt dispatcher */
        call ArDispatchTrap
    .endif

    /* Restore the original trap frame stack pointer */
    mov %rbx, %rsp

    /* Test previous mode and swapgs if needed */
    testb $1, KTRAP_FRAME_PreviousMode(%rbp)
    jz KernelModeReturn\Type\Vector
    cli
    swapgs

KernelModeReturn\Type\Vector:
    /* Restore XMM registers */
    movdqa KTRAP_FRAME_Xmm0(%rbp), %xmm0
    movdqa KTRAP_FRAME_Xmm1(%rbp), %xmm1
    movdqa KTRAP_FRAME_Xmm2(%rbp), %xmm2
    movdqa KTRAP_FRAME_Xmm3(%rbp), %xmm3
    movdqa KTRAP_FRAME_Xmm4(%rbp), %xmm4
    movdqa KTRAP_FRAME_Xmm5(%rbp), %xmm5
    movdqa KTRAP_FRAME_Xmm6(%rbp), %xmm6
    movdqa KTRAP_FRAME_Xmm7(%rbp), %xmm7
    movdqa KTRAP_FRAME_Xmm8(%rbp), %xmm8
    movdqa KTRAP_FRAME_Xmm9(%rbp), %xmm9
    movdqa KTRAP_FRAME_Xmm10(%rbp), %xmm10
    movdqa KTRAP_FRAME_Xmm11(%rbp), %xmm11
    movdqa KTRAP_FRAME_Xmm12(%rbp), %xmm12
    movdqa KTRAP_FRAME_Xmm13(%rbp), %xmm13
    movdqa KTRAP_FRAME_Xmm14(%rbp), %xmm14
    movdqa KTRAP_FRAME_Xmm15(%rbp), %xmm15

    /* Load MxCsr register */
    ldmxcsr KTRAP_FRAME_MxCsr(%rbp)

    /* Free stack space */
    add $(KTRAP_FRAME_SIZE - KTRAP_FRAME_REGISTERS_SIZE), %rsp

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

/* Populate common interrupt and trap handlers */
.irp i,0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
    .irp j,0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
        ArCreateHandler 0x\i\j Interrupt
        ArCreateHandler 0x\i\j Trap
    .endr
.endr

/* Define array of pointers to the interrupt handlers */
.global ArInterruptEntry
ArInterruptEntry:
.irp i,0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
    .irp j,0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
        .quad ArInterrupt0x\i\j
    .endr
.endr

/* Define array of pointers to the trap handlers */
.global ArTrapEntry
ArTrapEntry:
.irp i,0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
    .irp j,0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
        .quad ArTrap0x\i\j
    .endr
.endr

/**
 * Handles a spurious interrupt allowing it to end up.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
.global ArHandleSpuriousInterrupt
ArHandleSpuriousInterrupt:
    iretq
