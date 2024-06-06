/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/init.c
 * DESCRIPTION:     Hardware layer initialization code
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Initializes the hardware layer subsystem
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
HlInitializeSystem(VOID)
{
    XTSTATUS Status;

    /* Initialize ACPI */
    Status = HlpInitializeAcpi();
    if(Status != STATUS_SUCCESS)
    {
        return Status;
    }

    /* Get system information from ACPI */
    Status = HlpInitializeAcpiSystemInformation();
    if(Status != STATUS_SUCCESS)
    {
        return Status;
    }

    /* Return success */
    return STATUS_SUCCESS;
}
