/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ar/i686/traps.cc
 * DESCRIPTION:     I686 system traps
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


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
AR::Traps::DispatchTrap(IN PKTRAP_FRAME TrapFrame)
{
    /* Check vector and call appropriate handler */
    switch(TrapFrame->Vector)
    {
        case 0x00:
            /* Divide By Zero exception */
            HandleTrap00(TrapFrame);
            break;
        case 0x01:
            /* Debug exception */
            HandleTrap01(TrapFrame);
            break;
        case 0x02:
            /* Non-Maskable Interrupt (NMI) */
            HandleTrap02(TrapFrame);
            break;
        case 0x03:
            /* INT3 instruction executed */
            HandleTrap03(TrapFrame);
            break;
        case 0x04:
            /* Overflow exception */
            HandleTrap04(TrapFrame);
            break;
        case 0x05:
            /* Bound Range Exceeded exception */
            HandleTrap05(TrapFrame);
            break;
        case 0x06:
            /* Invalid Opcode exception */
            HandleTrap06(TrapFrame);
            break;
        case 0x07:
            /* Device Not Available exception */
            HandleTrap07(TrapFrame);
            break;
        case 0x08:
            /* Double Fault exception */
            HandleTrap08(TrapFrame);
            break;
        case 0x09:
            /* Segment Overrun exception */
            HandleTrap09(TrapFrame);
            break;
        case 0x0A:
            /* Invalid TSS exception */
            HandleTrap0A(TrapFrame);
            break;
        case 0x0B:
            /* Segment Not Present exception */
            HandleTrap0B(TrapFrame);
            break;
        case 0x0C:
            /* Stack Segment Fault exception */
            HandleTrap0C(TrapFrame);
            break;
        case 0x0D:
            /* General Protection Fault (GPF) exception*/
            HandleTrap0D(TrapFrame);
            break;
        case 0x0E:
            /* Page Fault exception */
            HandleTrap0E(TrapFrame);
            break;
        case 0x10:
            /* X87 Floating-Point exception */
            HandleTrap10(TrapFrame);
            break;
        case 0x11:
            /* Alignment Check exception */
            HandleTrap11(TrapFrame);
            break;
        case 0x12:
            /* Machine Check exception */
            HandleTrap12(TrapFrame);
            break;
        case 0x13:
            /* SIMD Floating-Point exception */
            HandleTrap13(TrapFrame);
            break;
        case 0x2A:
            /* Tick Count service request */
            HandleTrap2A(TrapFrame);
            break;
        case 0x2B:
            /* User-mode callback return */
            HandleTrap2B(TrapFrame);
            break;
        case 0x2C:
            /* Assertion raised */
            HandleTrap2C(TrapFrame);
            break;
        case 0x2D:
            /* Debug-Service-Request raised */
            HandleTrap2D(TrapFrame);
            break;
        case 0x2E:
            /* System call service request */
            HandleTrap2E(TrapFrame);
            break;
        default:
            /* Unknown/Unexpected trap */
            HandleTrapFF(TrapFrame);
            break;
    }
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
AR::Traps::HandleTrap00(IN PKTRAP_FRAME TrapFrame)
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
AR::Traps::HandleTrap01(IN PKTRAP_FRAME TrapFrame)
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
AR::Traps::HandleTrap02(IN PKTRAP_FRAME TrapFrame)
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
AR::Traps::HandleTrap03(IN PKTRAP_FRAME TrapFrame)
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
AR::Traps::HandleTrap04(IN PKTRAP_FRAME TrapFrame)
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
AR::Traps::HandleTrap05(IN PKTRAP_FRAME TrapFrame)
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
AR::Traps::HandleTrap06(IN PKTRAP_FRAME TrapFrame)
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
AR::Traps::HandleTrap07(IN PKTRAP_FRAME TrapFrame)
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
AR::Traps::HandleTrap08(IN PKTRAP_FRAME TrapFrame)
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
AR::Traps::HandleTrap09(IN PKTRAP_FRAME TrapFrame)
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
AR::Traps::HandleTrap0A(IN PKTRAP_FRAME TrapFrame)
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
AR::Traps::HandleTrap0B(IN PKTRAP_FRAME TrapFrame)
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
AR::Traps::HandleTrap0C(IN PKTRAP_FRAME TrapFrame)
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
AR::Traps::HandleTrap0D(IN PKTRAP_FRAME TrapFrame)
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
AR::Traps::HandleTrap0E(IN PKTRAP_FRAME TrapFrame)
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
AR::Traps::HandleTrap10(IN PKTRAP_FRAME TrapFrame)
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
AR::Traps::HandleTrap11(IN PKTRAP_FRAME TrapFrame)
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
AR::Traps::HandleTrap12(IN PKTRAP_FRAME TrapFrame)
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
AR::Traps::HandleTrap13(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Handled SIMD Floating-Point exception (0x13)!\n");
    for(;;);
}

/**
 * Handles the trap 0x2A when Tick Count service request occurs.
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
AR::Traps::HandleTrap2A(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Unhandled Tick Count service request (0x2A)!\n");
}

/**
 * Handles the trap 0x2B when Callback return service request occurs.
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
AR::Traps::HandleTrap2B(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Unhandled Callback return service request (0x2B)!\n");
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
AR::Traps::HandleTrap2C(IN PKTRAP_FRAME TrapFrame)
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
AR::Traps::HandleTrap2D(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Handled Debug-Service-Request (0x2D)!\n");
    for(;;);
}

/**
 * Handles the trap 0x2E when a system call is being requested.
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
AR::Traps::HandleTrap2E(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Unhandled system call (0x2E)!\n");
}

/**
 * Handles the trap 0xFF then Unexpected Interrupt occurs.
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
AR::Traps::HandleTrapFF(IN PKTRAP_FRAME TrapFrame)
{
    DebugPrint(L"Handled Unexpected-Interrupt (0xFF)!\n");
    for(;;);
}

/**
 * C-linkage wrapper for dispatching the trap provided by common trap handler.
 *
 * @param TrapFrame
 *        Supplies a kernel trap frame pushed by common trap handler on the stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTCDECL
VOID
ArDispatchTrap(IN PKTRAP_FRAME TrapFrame)
{
    AR::Traps::DispatchTrap(TrapFrame);
}
