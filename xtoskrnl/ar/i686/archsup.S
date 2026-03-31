/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ar/i686/archsup.S
 * DESCRIPTION:     Provides i686 architecture features not implementable in C.
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#include <ar/i686/asmsup.h>

.altmacro
.text


/**
 * Creates a task, trap or interrupt handler for the specified vector.
 *
 * @param Vector
 *        Supplies a vector number.
 *
 * @param Type
 *        Specifies whether the handler is designed to handle an interrupt, a task or a trap.
 *
 * @return This macro does not return any value.
 *
 * @since XT 1.0
 */
.macro ArCreateHandler Vector Type
.global _Ar\Type\Vector
_Ar\Type\Vector:
     /* Check handler type */
    .ifc \Type,Task
_Ar\Type\Vector\()Start:
        /* Clear the Task Switch flag */
        clts

        /* Allocate the trap frame and inject the hardware vector for the dispatcher */
        sub $TRAP_FRAME_SIZE, %esp
        movl $\Vector, TrapVector(%esp)

        /* Pass the trap frame pointer as an argument and clear the direction flag */
        push %esp
        cld

        /* Pass control to the trap dispatcher */
        call _ArDispatchTrap

        /* Discard the argument and deallocate the trap frame */
        add $4, %esp
        add $TRAP_FRAME_SIZE, %esp

        /* Hardware task return */
        iretl

        /* Spin back to the entry point to rearm the task gate */
        jmp _Ar\Type\Vector\()Start
    .else
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
        mov TrapSegCs(%ebp), %ax
        and $3, %al
        mov %al, TrapPreviousMode(%ebp)
        jz KernelMode\Type\Vector
        swapgs
        jmp UserMode\Type\Vector

KernelMode\Type\Vector:
        /* Save kernel stack pointer (SS:ESP) */
        movl %ss, %eax
        mov %eax, TrapSegSs(%ebp)
        lea TrapEsp(%ebp), %eax
        mov %eax, TrapEsp(%ebp)

UserMode\Type\Vector:
        /* Push Frame Pointer and clear direction flag */
        push %esp
        cld

        .ifc \Type,Trap
            /* Pass to the trap dispatcher */
            call _ArDispatchTrap
        .else
            /* Pass to the interrupt dispatcher */
            call _ArDispatchTrap
        .endif

        /* Clean up the stack */
        add $4, %esp

        /* Test previous mode and swapgs if needed */
        testb $1, TrapPreviousMode(%ebp)
        jz KernelModeReturn\Type\Vector
        cli
        swapgs

KernelModeReturn\Type\Vector:
        /* Restore segment selectors */
        mov TrapSegDs(%ebp), %ds
        mov TrapSegEs(%ebp), %es
        mov TrapSegFs(%ebp), %fs
        mov TrapSegGs(%ebp), %gs

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
    .endif
.endm

/* Populate common interrupt, task and trap handlers */
.irp i,0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
    .irp j,0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
        ArCreateHandler 0x\i\j Interrupt
        .if 0x\i\j == 0x02 || 0x\i\j == 0x08
            ArCreateHandler 0x\i\j Task
        .else
            ArCreateHandler 0x\i\j Trap
        .endif
    .endr
.endr

/* Define array of pointers to the interrupt handlers */
.global _ArInterruptEntry
_ArInterruptEntry:
.irp i,0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
    .irp j,0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
        .long _ArInterrupt0x\i\j
    .endr
.endr

/* Define array of pointers to the trap handlers */
.global _ArTrapEntry
_ArTrapEntry:
.irp i,0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
    .irp j,0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
        .if 0x\i\j == 0x02 || 0x\i\j == 0x08
            .long _ArTask0x\i\j
        .else
            .long _ArTrap0x\i\j
        .endif
    .endr
.endr
