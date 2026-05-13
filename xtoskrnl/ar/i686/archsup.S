/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ar/i686/archsup.S
 * DESCRIPTION:     Provides i686 architecture features not implementable in C.
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#include <xtkmapi.h>
#include <xtadk.h>

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
        sub $KTRAP_FRAME_SIZE, %esp
        movl $\Vector, KTRAP_FRAME_Vector(%esp)

        /* Pass the trap frame pointer as an argument and clear the direction flag */
        push %esp
        cld

        /* Pass control to the trap dispatcher */
        call _ArDispatchTrap

        /* Discard the argument and deallocate the trap frame */
        add $4, %esp
        add $KTRAP_FRAME_SIZE, %esp

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
        sub $(KTRAP_FRAME_SIZE - KTRAP_FRAME_REGISTERS_SIZE), %esp
        lea (%esp), %ebp

        /* Store segment selectors */
        mov %gs, KTRAP_FRAME_SegGs(%ebp)
        mov %fs, KTRAP_FRAME_SegFs(%ebp)
        mov %es, KTRAP_FRAME_SegEs(%ebp)
        mov %ds, KTRAP_FRAME_SegDs(%ebp)

        /* Store debug registers */
        mov %dr7, %eax
        mov %eax, KTRAP_FRAME_Dr7(%ebp)
        mov %dr6, %eax
        mov %eax, KTRAP_FRAME_Dr6(%ebp)
        mov %dr3, %eax
        mov %eax, KTRAP_FRAME_Dr3(%ebp)
        mov %dr2, %eax
        mov %eax, KTRAP_FRAME_Dr2(%ebp)
        mov %dr1, %eax
        mov %eax, KTRAP_FRAME_Dr1(%ebp)
        mov %dr0, %eax
        mov %eax, KTRAP_FRAME_Dr0(%ebp)

        /* Store CR2 and CR3 */
        mov %cr3, %eax
        mov %eax, KTRAP_FRAME_Cr3(%ebp)
        mov %cr2, %eax
        mov %eax, KTRAP_FRAME_Cr2(%ebp)

        /* Test previous mode */
        movl $0, KTRAP_FRAME_PreviousMode(%ebp)
        mov KTRAP_FRAME_SegCs(%ebp), %ax
        and $3, %al
        mov %al, KTRAP_FRAME_PreviousMode(%ebp)
        jz Dispatch\Type\Vector

        /* Load Kernel PB selector into FS */
        mov $KGDT_R0_PB, %ax
        mov %ax, %fs

        /* Set sane data segment selectors */
        mov $(KGDT_R3_DATA | RPL_MASK), %ax
        mov %ax, %ds
        mov %ax, %es

Dispatch\Type\Vector:
        /* Push Frame Pointer and clear direction flag */
        push %esp
        cld

        .ifc \Type,Trap
            /* Pass to the trap dispatcher */
            call _ArDispatchTrap
        .else
            /* Pass to the interrupt dispatcher */
            call _ArDispatchInterrupt
        .endif

        /* Clean up the stack */
        add $4, %esp

        /* Test previous mode and disable interrupts before user mode return */
        testb $1, KTRAP_FRAME_PreviousMode(%ebp)
        jz RestoreState\Type\Vector
        cli

RestoreState\Type\Vector:
        /* Restore segment selectors */
        mov KTRAP_FRAME_SegDs(%ebp), %ds
        mov KTRAP_FRAME_SegEs(%ebp), %es
        mov KTRAP_FRAME_SegFs(%ebp), %fs
        mov KTRAP_FRAME_SegGs(%ebp), %gs

        /* Free stack space */
        add $(KTRAP_FRAME_SIZE - KTRAP_FRAME_REGISTERS_SIZE), %esp

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

/**
 * Enables eXtended Physical Addressing (XPA). On i386, this is just a stub.
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

.global ArEnableExtendedPhysicalAddressingEnd
ArEnableExtendedPhysicalAddressingEnd:

/**
 * Handles a spurious interrupt allowing it to end up.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
.global _ArHandleSpuriousInterrupt
_ArHandleSpuriousInterrupt:
    iret

/**
 * Starts an application processor (AP).
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
.global _ArStartApplicationProcessor
_ArStartApplicationProcessor:
    /* Enter 16-bit (real mode) */
    .code16

    /* Disable interrupts and clear direction flag */
    cli
    cld

    /* Establish a flat addressing baseline */
    movw %cs, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movw $0x1000, %sp

    /* Calculate absolute physical base address */
    xorl %ebx, %ebx
    movw %cs, %bx
    shll $4, %ebx

    /* Load the temporary Global Descriptor Table */
    leal (ApTemporaryGdtDesc - _ArStartApplicationProcessor)(%ebx), %eax
    movl %eax, (ApTemporaryGdtBase - _ArStartApplicationProcessor)
    lgdtl (ApTemporaryGdtSize - _ArStartApplicationProcessor)

    /* Enable Protected Mode */
    movl %cr0, %eax
    orl $0x01, %eax
    movl %eax, %cr0

    /* Far return to enter 32-bit protected mode */
    leal (ApEnterProtectedMode - _ArStartApplicationProcessor)(%ebx), %eax
    pushl $0x08
    pushl %eax
    lretl

ApEnterProtectedMode:
    /* Enter 32-bit protected mode */
    .code32

    /* Setup all data segment registers */
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    xorw %ax, %ax
    movw %ax, %fs
    movw %ax, %gs

    /* Locate PROCESSOR_START_BLOCK structure */
    leal (_ArStartApplicationProcessorEnd - _ArStartApplicationProcessor)(%ebx), %edi

    /* Load CR4 from BSP, but mask PCIDE and PGE */
    movl PROCESSOR_START_BLOCK_Cr4(%edi), %eax
    andl $~0x00020080, %eax
    movl %eax, %cr4

    /* Load the Kernel Page Directory Base from BSP */
    movl PROCESSOR_START_BLOCK_Cr3(%edi), %eax
    movl %eax, %cr3

    /* Enable Paging */
    movl %cr0, %eax
    orl $0x80000000, %eax
    movl %eax, %cr0

    /* Load dedicated Stack for AP */
    movl PROCESSOR_START_BLOCK_Stack(%edi), %esp

    /* Save the pointer to PROCESSOR_START_BLOCK */
    movl %edi, %ecx
    pushl %edi

    /* Call the EntryPoint routine */
    movl PROCESSOR_START_BLOCK_EntryPoint(%edi), %eax
    call *%eax

    /* Fire the breakpoint and halt if the entry point returns */
.ApNoReturnPoint:
    int $0x03
    hlt
    jmp .ApNoReturnPoint

/* Data section for temporary GDT */
.align 8
ApTemporaryGdtSize: .short _ArStartApplicationProcessorEnd - ApTemporaryGdtDesc - 1
ApTemporaryGdtBase: .long 0x00000000
ApTemporaryGdtDesc: .quad 0x0000000000000000, 0x00CF9A000000FFFF, 0x00CF92000000FFFF

.global _ArStartApplicationProcessorEnd
_ArStartApplicationProcessorEnd:
