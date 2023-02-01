/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ar/i686/traps.c
 * DESCRIPTION:     I686 system traps
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Handles the trap 0x02 (Non Maskable Interrupt) gracefully.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap02()
{
    LdrPrint(L"Handled Non-Maskable Interrupt (0x02)!\n");
    for(;;);
}

/**
 * Handles the trap 0x08 (Double Fault) gracefully.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap08()
{
    LdrPrint(L"Handled Double Fault exception (0x08)!\n");
    for(;;);
}

/**
 * Handles the trap 0x0E (Page Fault) gracefully.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap0D()
{
    LdrPrint(L"Handled General Protection Fault (0x0D)!\n");
    for(;;);
}

/**
 * Handles the trap 0x0E (Page Fault) gracefully.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrap0E()
{
    LdrPrint(L"Handled Page Fault exception (0x0E)!\n");
    for(;;);
}

/**
 * Handles the trap 0xFF (Unexpected Interrupt) gracefully.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
ArpHandleTrapFF()
{
    LdrPrint(L"Handled Unexpected Interrupt (0xFF)!\n");
    for(;;);
}
