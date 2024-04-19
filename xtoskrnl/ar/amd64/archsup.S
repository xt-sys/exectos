/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ar/amd64/archsup.S
 * DESCRIPTION:     Provides AMD64 architecture features not implementable in C.
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
.global ArpTrap\Vector
ArpTrap\Vector:
    /* Push fake error code for non-error vectors */
    .if \Vector != 8 && \Vector != 10 && \Vector != 11 && \Vector != 12 && \Vector != 13 && \Vector != 14 && \Vector != 17 && \Vector != 30
        push $0
    .endif

    /* Push vector number */
    push $\Vector

    /* Push General Purpose Registers */
    push %r15
    push %r14
    push %r13
    push %r12
    push %r11
    push %r10
    push %r9
    push %r8
    push %rbp
    push %rdi
    push %rsi
    push %rdx
    push %rcx
    push %rbx
    push %rax

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
    mov %cr3, %rax
    push %rax
    mov %cr2, %rax
    push %rax

    /* Push Debug Registers */
    mov %dr7, %rax
    push %rax
    mov %dr6, %rax
    push %rax
    mov %dr3, %rax
    push %rax
    mov %dr2, %rax
    push %rax
    mov %dr1, %rax
    push %rax
    mov %dr0, %rax
    push %rax

    /* Push Frame Pointer, clear direction flag and pass to trap dispatcher */
    mov %rsp, %rcx
    cld
    call ArpDispatchTrap

    /* Skip space occupied by Debug Registers */
    add $(6 * 8), %rsp

    /* Skip space occupied by CR2 and CR3 */
    add $(2 * 8), %rsp

    /* Skip space occupied by Segments */
    add $(4 * 2), %rsp

    /* Pop General Purpose Registers */
    pop %rax
    pop %rbx
    pop %rcx
    pop %rdx
    pop %rsi
    pop %rdi
    pop %rbp
    pop %r8
    pop %r9
    pop %r10
    pop %r11
    pop %r12
    pop %r13
    pop %r14
    pop %r15

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
