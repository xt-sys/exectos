/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ar/i686/archsup.S
 * DESCRIPTION:     Provides i686 architecture features not implementable in C.
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

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
.macro ArpCreateTrapHandler Vector
.global _ArpTrap\Vector
_ArpTrap\Vector:
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

    /* Push Segments */
    mov %gs, %ax
    push %ax
    mov %fs, %ax
    push %ax
    mov %es, %ax
    push %ax
    mov %ds, %ax
    push %ax

    /* Push Control Registers */
    mov %cr3, %eax
    push %eax
    mov %cr2, %eax
    push %eax

    /* Push Debug Registers */
    mov %dr7, %eax
    push %eax
    mov %dr6, %eax
    push %eax
    mov %dr3, %eax
    push %eax
    mov %dr2, %eax
    push %eax
    mov %dr1, %eax
    push %eax
    mov %dr0, %eax
    push %eax

    /* Push Frame Pointer, clear direction flag and pass to trap dispatcher */
    push %esp
    cld
    call _ArpDispatchTrap

    /* Clean up the stack */
    add $4, %esp

    /* Skip space occupied by Debug Registers */
    add $(6 * 4), %esp

    /* Skip space occupied by CR2 and CR3 */
    add $(2 * 4), %esp

    /* Skip space occupied by Segments */
    add $(4 * 2), %esp

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
        ArpCreateTrapHandler 0x\i\j
    .endr
.endr
