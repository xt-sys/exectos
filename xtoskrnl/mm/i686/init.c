/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/i686/init.c
 * DESCRIPTION:     Architecture specific Memory Manager initialization routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Gets the address of the PDE (Page Directory Entry), that maps given address.
 *
 * @param Address
 *        Specifies the address to find the PDE for.
 *
 * @return This routine returns the address of the PDE.
 *
 * @since XT 1.0
 */
XTAPI
PMMPTE
MmpGetPdeAddress(PVOID Address)
{
    ULONG Offset;

    /* Calculate offset and return PTE address */
    Offset = ((((ULONG)(Address)) >> MM_PDI_SHIFT) << MM_PTE_SHIFT);
    return (PMMPTE)(MM_PDE_BASE + Offset);
}

/**
 * Gets the address of the PTE (Page Table Entry), that maps given address.
 *
 * @param Address
 *        Specifies the address to find the PTE for.
 *
 * @return This routine returns the address of the PTE.
 *
 * @since XT 1.0
 */
XTAPI
PMMPTE
MmpGetPteAddress(PVOID Address)
{
    ULONG Offset;

    /* Calculate offset and return PTE address */
    Offset = ((((ULONG)(Address)) >> MM_PTI_SHIFT) << MM_PTE_SHIFT);
    return (PMMPTE)(MM_PTE_BASE + Offset);
}

/**
 * Performs architecture specific initialization of the XTOS Memory Manager.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MmpInitializeArchitecture(VOID)
{
    UNIMPLEMENTED;
}

/**
 * Checks if PAE (Physical Address Extension) is enabled.
 *
 * @return This routine returns TRUE if PAE is enabled, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
MmpMemoryExtensionEnabled(VOID)
{
    /* Check if PAE is enabled */
    return ((ArReadControlRegister(4) & CR4_PAE) != 0) ? TRUE : FALSE;
}
