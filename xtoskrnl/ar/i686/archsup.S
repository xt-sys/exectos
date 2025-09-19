/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ar/i686/archsup.S
 * DESCRIPTION:     Provides i686 architecture features not implementable in C.
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <ar/i686/asmsup.h>

.altmacro
.text


/**
 * This macro creates a trap handler for the specified vector.
 *
 * @param Vector
 *        Supplies a trap vector number.
 *
 * @return This macro does not return any value.
 *
 * @since XT 1.0
 */
.macro ArCreateTrapHandler Vector
.global _ArTrap\Vector
_ArTrap\Vector:
    /* Push fake error code for non-error vectors */
    .if \Vector != 8 && \Vector != 10 && \Vector != 11 && \Vector != 12 && \Vector != 13 && \Vector != 14 && \Vector != 17 && \Vector != 30
        push $0
    .endif

    /* Push vector number */
    push $\Vector

    /* Push General Purpose Registers */
    push %ebp
    push %edi
    push %esi
    push %edx
    push %ecx
    push %ebx
    push %eax

    /* Reserve space for other registers and point RBP to the trap frame */
    sub $(TRAP_FRAME_SIZE - TRAP_REGISTERS_SIZE), %esp
    lea (%esp), %ebp

    /* Store segment selectors */
    mov %gs, TrapSegGs(%ebp)
    mov %fs, TrapSegFs(%ebp)
    mov %es, TrapSegEs(%ebp)
    mov %ds, TrapSegDs(%ebp)

    /* Store debug registers */
    mov %dr7, %eax
    mov %eax, TrapDr7(%ebp)
    mov %dr6, %eax
    mov %eax, TrapDr6(%ebp)
    mov %dr3, %eax
    mov %eax, TrapDr3(%ebp)
    mov %dr2, %eax
    mov %eax, TrapDr2(%ebp)
    mov %dr1, %eax
    mov %eax, TrapDr1(%ebp)
    mov %dr0, %eax
    mov %eax, TrapDr0(%ebp)

    /* Store CR2 and CR3 */
    mov %cr3, %eax
    mov %eax, TrapCr3(%ebp)
    mov %cr2, %eax
    mov %eax, TrapCr2(%ebp)

    /* Test previous mode and swap GS if needed */
    movl $0, TrapPreviousMode(%ebp)
    mov %cs, %ax
    and $1, %al
    mov %al, TrapPreviousMode(%ebp)
    jz KernelMode$\Vector
    swapgs

KernelMode$\Vector:
    /* Push Frame Pointer, clear direction flag and pass to trap dispatcher */
    push %esp
    cld
    call _ArDispatchTrap

    /* Clean up the stack */
    add $4, %esp

    /* Test previous mode and swapgs if needed */
    testb $1, TrapPreviousMode(%ebp)
    jz KernelModeReturn$\Vector
    cli
    swapgs

KernelModeReturn$\Vector:
    /* Restore segment selectors */
    mov TrapSegDs(%ebp), %ds
    mov TrapSegEs(%ebp), %es
    mov TrapSegFs(%ebp), %fs

    /* Free stack space */
    add $(TRAP_FRAME_SIZE - TRAP_REGISTERS_SIZE), %esp

    /* Pop General Purpose Registers */
    pop %eax
    pop %ebx
    pop %ecx
    pop %edx
    pop %esi
    pop %edi
    pop %ebp

    /* Skip error code and vector number, then return */
    add $(2 * 4), %esp
    iretl
.endm

/* Populate common trap handlers */
.irp i,0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
    .irp j,0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
        ArCreateTrapHandler 0x\i\j
    .endr
.endr
