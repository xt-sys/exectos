/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ar/amd64/traps.c
 * DESCRIPTION:     AMD64 system traps
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Dispatches the trap provided by common trap handler.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common trap handler on the stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpDispatchTrap(IN PKTRAP_FRAME TrapFrame)
{
    /* Check vector and call appropriate handler */
    switch(TrapFrame->Vector)
    {
        case 0x00:
            /* Divide By Zero exception */
            ArpHandleTrap00(TrapFrame);
            break;
        case 0x01:
            /* Debug exception */
            ArpHandleTrap01(TrapFrame);
            break;
        case 0x02:
            /* Non-Maskable Interrupt (NMI) */
            ArpHandleTrap02(TrapFrame);
            break;
        case 0x03:
            /* INT3 instruction executed */
            ArpHandleTrap03(TrapFrame);
            break;
        case 0x04:
            /* Overflow exception */
            ArpHandleTrap04(TrapFrame);
            break;
        case 0x05:
            /* Bound Range Exceeded exception */
            ArpHandleTrap05(TrapFrame);
            break;
        case 0x06:
            /* Invalid Opcode exception */
            ArpHandleTrap06(TrapFrame);
            break;
        case 0x07:
            /* Device Not Available exception */
            ArpHandleTrap07(TrapFrame);
            break;
        case 0x08:
            /* Double Fault exception */
            ArpHandleTrap08(TrapFrame);
            break;
        case 0x09:
            /* Segment Overrun exception */
            ArpHandleTrap09(TrapFrame);
            break;
        case 0x0A:
            /* Invalid TSS exception */
            ArpHandleTrap0A(TrapFrame);
            break;
        case 0x0B:
            /* Segment Not Present exception */
            ArpHandleTrap0B(TrapFrame);
            break;
        case 0x0C:
            /* Stack Segment Fault exception */
            ArpHandleTrap0C(TrapFrame);
            break;
        case 0x0D:
            /* General Protection Fault (GPF) exception*/
            ArpHandleTrap0D(TrapFrame);
            break;
        case 0x0E:
            /* Page Fault exception */
            ArpHandleTrap0E(TrapFrame);
            break;
        case 0x10:
            /* X87 Floating-Point exception */
            ArpHandleTrap10(TrapFrame);
            break;
        case 0x11:
            /* Alignment Check exception */
            ArpHandleTrap11(TrapFrame);
            break;
        case 0x12:
            /* Machine Check exception */
            ArpHandleTrap12(TrapFrame);
            break;
        case 0x13:
            /* SIMD Floating-Point exception */
            ArpHandleTrap13(TrapFrame);
            break;
        case 0x1F:
            /* Software Interrupt at APC level */
            ArpHandleTrap1F(TrapFrame);
            break;
        case 0x2C:
            /* Assertion raised */
            ArpHandleTrap2C(TrapFrame);
            break;
        case 0x2D:
            /* Debug-Service-Request raised */
            ArpHandleTrap2D(TrapFrame);
            break;
        case 0x2F:
            /* Software Interrupt at DISPATCH level */
            ArpHandleTrap2F(TrapFrame);
            break;
        case 0xE1:
            /* InterProcessor Interrupt (IPI) */
            ArpHandleTrapE1(TrapFrame);
            break;
        default:
            /* Unknown/Unexpected trap */
            ArpHandleTrapFF(TrapFrame);
            break;
    }
}

XTCDECL
VOID
ArpHandleSystemCall32(VOID)
{
    DebugPrint(L"Handled 32-bit system call!\n");
}

XTCDECL
VOID
ArpHandleSystemCall64(VOID)
{
    DebugPrint(L"Handled 64-bit system call!\n");
}

/**
 * Handles the trap 0x00 when a Divide By Zero exception occurs.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common trap handler on the stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap00(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Handled Division-By-Zero Error (0x00)!\n");
    for(;;);
}

/**
 * Handles the trap 0x01 when Debug exception occurs.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common trap handler on the stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap01(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Handled Debug exception (0x01)!\n");
    for(;;);
}

/**
 * Handles the trap 0x02 when Non-Maskable Interrupt (NMI) occurs.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common trap handler on the stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap02(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Handled Non-Maskable-Interrupt (0x02)!\n");
    for(;;);
}

/**
 * Handles the trap 0x03 when the INT3 instruction is executed.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common trap handler on the stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap03(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Handled INT3 (0x03)!\n");
    for(;;);
}

/**
 * Handles the trap 0x04 when the INTO instruction is executed and overflow bit is set.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common trap handler on the stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap04(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Handled Overflow exception (0x04)!\n");
    for(;;);
}

/**
 * Handles the trap 0x05 when the Bound Range Exceeded exception occurs.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common trap handler on the stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap05(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Handled Bound-Range-Exceeded exception (0x05)!\n");
    for(;;);
}

/**
 * Handles the trap 0x06 when the Invalid Opcode exception occurs.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common trap handler on the stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap06(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Handled Invalid Opcode exception (0x06)!\n");
    for(;;);
}

/**
 * Handles the trap 0x07 when the Device Not Available exception occurs.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common trap handler on the stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap07(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Handled Device Not Available exception (0x07)!\n");
    for(;;);
}

/**
 * Handles the trap 0x08 when Double Fault exception occurs.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common trap handler on the stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap08(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Handled Double-Fault exception (0x08)!\n");
    for(;;);
}

/**
 * Handles the trap 0x09 when the Segment Overrun exception occurs.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common trap handler on the stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap09(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Handled Segment-Overrun exception (0x09)!\n");
    for(;;);
}

/**
 * Handles the trap 0x0A when the Invalid TSS exception occurs.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common trap handler on the stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap0A(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Handled Invalid-TSS exception (0x0A)!\n");
    for(;;);
}

/**
 * Handles the trap 0x0B when the Segment Not Present exception occurs.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common trap handler on the stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap0B(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Handled Segment-Not-Present exception (0x0B)!\n");
    for(;;);
}

/**
 * Handles the trap 0x0C when the Stack Segment Fault exception occurs.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common trap handler on the stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap0C(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Handled Stack-Segment-Fault exception (0x0C)!\n");
    for(;;);
}

/**
 * Handles the trap 0x0D when General Protection Fault (GPF) exception occurs.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common trap handler on the stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap0D(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Handled General-Protection-Fault (0x0D)!\n");
    for(;;);
}

/**
 * Handles the trap 0x0E when the Page Fault (PF) exception occurs.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common trap handler on the stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap0E(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Handled Page-Fault exception (0x0E)!\n");
    for(;;);
}

/**
 * Handles the trap 0x10 when the X87 Floating-Point exception occurs.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common trap handler on the stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap10(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Handled x87 Floating-Point exception (0x10)!\n");
    for(;;);
}

/**
 * Handles the trap 0x11 when the Alignment Check exception occurs.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common trap handler on the stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap11(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Handled Alignment-Check exception (0x11)!\n");
    for(;;);
}

/**
 * Handles the trap 0x12 when the Machine Check exception occurs.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common trap handler on the stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap12(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Handled Machine-Check exception (0x12)!\n");
    for(;;);
}

/**
 * Handles the trap 0x13 when the SIMD Floating-Point exception occurs.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common trap handler on the stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap13(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Handled SIMD Floating-Point exception (0x13)!\n");
    for(;;);
}

/**
 * Handles the trap 0x1F when software interrupt gets generated at APC_LEVEL.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common trap handler on the stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap1F(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Unhandled software interrupt at APC level (0x1F)!\n");
}

/**
 * Handles the trap 0x2C when an assertion is raised.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common trap handler on the stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap2C(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Handled Assertion (0x2C)!\n");
    for(;;);
}

/**
 * Handles the trap 0x2D when a debug service is being requested.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common trap handler on the stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap2D(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Handled Debug-Service-Request (0x2D)!\n");
    for(;;);
}

/**
 * Handles the trap 0x2F when a software interrupt gets generated at DISPATCH_LEVEL.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common trap handler on the stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap2F(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Unhandled software interrupt at DISPATCH level (0x2F)!\n");
}

/**
 * Handles the trap 0xE1 when InterProcessor Interrupt (IPI) occurs.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common trap handler on the stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrapE1(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Unhandled IPI interrupt (0xE1)!\n");
}

/**
 * Handles the trap 0xFF when Unexpected Interrupt occurs.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common trap handler on the stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrapFF(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Handled Unexpected-Interrupt (0xFF)!\n");
    for(;;);
}
