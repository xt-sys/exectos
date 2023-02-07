/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ar/amd64/traps.c
 * DESCRIPTION:     AMD64 system traps
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


XTCDECL
VOID
ArpHandleSystemCall32()
{
    DebugPrint(L"Handled 32-bit system call!\n");
}

XTCDECL
VOID
ArpHandleSystemCall64()
{
    DebugPrint(L"Handled 64-bit system call!\n");
}

/**
 * Handles the trap 0x00 when a Divide By Zero exception occurs.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap00()
{
    DebugPrint(L"Handled Division-By-Zero Error (0x00)!\n");
    for(;;);
}

/**
 * Handles the trap 0x01 when Debug exception occurs.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap01()
{
    DebugPrint(L"Handled Debug exception (0x01)!\n");
    for(;;);
}

/**
 * Handles the trap 0x02 when Non-Maskable Interrupt (NMI) occurs.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap02()
{
    DebugPrint(L"Handled Non-Maskable-Interrupt (0x02)!\n");
    for(;;);
}

/**
 * Handles the trap 0x03 when the INT3 instruction is executed.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap03()
{
    DebugPrint(L"Handled INT3 (0x03)!\n");
    for(;;);
}

/**
 * Handles the trap 0x04 when the INTO instruction is executed and overflow bit is set.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap04()
{
    DebugPrint(L"Handled Overflow exception (0x04)!\n");
    for(;;);
}

/**
 * Handles the trap 0x05 when the Bound Range Exceeded exception occurs.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap05()
{
    DebugPrint(L"Handled Bound-Range-Exceeded exception (0x05)!\n");
    for(;;);
}

/**
 * Handles the trap 0x06 when the Invalid Opcode exception occurs.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap06()
{
    DebugPrint(L"Handled Invalid Opcode exception (0x06)!\n");
    for(;;);
}

/**
 * Handles the trap 0x07 when the Device Not Available exception occurs.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap07()
{
    DebugPrint(L"Handled Device Not Available exception (0x07)!\n");
    for(;;);
}

/**
 * Handles the trap 0x08 when Double Fault exception occurs.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap08()
{
    DebugPrint(L"Handled Double-Fault exception (0x08)!\n");
    for(;;);
}

/**
 * Handles the trap 0x09 when the Segment Overrun exception occurs.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap09()
{
    DebugPrint(L"Handled Segment-Overrun exception (0x09)!\n");
    for(;;);
}

/**
 * Handles the trap 0x0A when the Invalid TSS exception occurs.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap0A()
{
    DebugPrint(L"Handled Invalid-TSS exception (0x0A)!\n");
    for(;;);
}

/**
 * Handles the trap 0x0B when the Segment Not Present exception occurs.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap0B()
{
    DebugPrint(L"Handled Segment-Not-Present exception (0x0B)!\n");
    for(;;);
}

/**
 * Handles the trap 0x0C when the Stack Segment Fault exception occurs.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap0C()
{
    DebugPrint(L"Handled Stack-Segment-Fault exception (0x0C)!\n");
    for(;;);
}

/**
 * Handles the trap 0x0D when General Protection Fault (GPF) exception occurs.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap0D()
{
    DebugPrint(L"Handled General-Protection-Fault (0x0D)!\n");
    for(;;);
}

/**
 * Handles the trap 0x0E when the Page Fault (PF) exception occurs.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap0E()
{
    DebugPrint(L"Handled Page-Fault exception (0x0E)!\n");
    for(;;);
}

/**
 * Handles the trap 0x10 when the X87 Floating-Point exception occurs.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap10()
{
    DebugPrint(L"Handled x87 Floating-Point exception (0x10)!\n");
    for(;;);
}

/**
 * Handles the trap 0x11 when the Alignment Check exception occurs.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap11()
{
    DebugPrint(L"Handled Alignment-Check exception (0x11)!\n");
    for(;;);
}

/**
 * Handles the trap 0x12 when the Machine Check exception occurs.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap12()
{
    DebugPrint(L"Handled Machine-Check exception (0x12)!\n");
    for(;;);
}

/**
 * Handles the trap 0x13 when the SIMD Floating-Point exception occurs.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap13()
{
    DebugPrint(L"Handled SIMD Floating-Point exception (0x13)!\n");
    for(;;);
}

/**
 * Handles the trap 0x2C when an assertion is raised.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap2C()
{
    DebugPrint(L"Handled Assertion (0x2C)!\n");
    for(;;);
}

/**
 * Handles the trap 0x2D when a debug service is being requested.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap2D()
{
    DebugPrint(L"Handled Debug-Service-Request (0x2D)!\n");
    for(;;);
}

/**
 * Handles the trap 0xFF then Unexpected Interrupt occurs.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrapFF()
{
    DebugPrint(L"Handled Unexpected-Interrupt (0xFF)!\n");
    for(;;);
}
