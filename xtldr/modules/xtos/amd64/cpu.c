/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/xtos/amd64/cpu.c
 * DESCRIPTION:     XTOS AMD64 processor context
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Loads previously prepared CPU context and performs architecture-specific tasks needed to boot XTOS.
 *
 * @param Gdt
 *        Supplies a pointer to memory area containing GDT to load.
 *
 * @param Idt
 *        Supplies a pointer to memory area containing IDT to load.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
XtpLoadProcessorContext(IN PKGDTENTRY Gdt,
                        IN PKIDTENTRY Idt)
{
}

/**
 * Sets architecture specific processor context.
 *
 * @param MemoryMappings
 *        Supplies a pointer to linked list containing all memory mappings.
 *
 * @param VirtualAddress
 *        Supplies a pointer to the next valid, free and available virtual address.
 *
 * @param Gdt
 *        Supplies a pointer to memory area containing GDT to fill in.
 *
 * @param Idt
 *        Supplies a pointer to memory area containing IDT to fill in.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
XtpSetProcessorContext(IN PLIST_ENTRY MemoryMappings,
                       IN PVOID *VirtualAddress,
                       OUT PKGDTENTRY *Gdt,
                       OUT PKIDTENTRY *Idt)
{
    /* Return success */
    return STATUS_EFI_SUCCESS;
}
