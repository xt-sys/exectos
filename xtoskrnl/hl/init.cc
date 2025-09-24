/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/init.cc
 * DESCRIPTION:     Hardware layer initialization code
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * Initializes the hardware layer subsystem
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
HL::Init::InitializeSystem(VOID)
{
    XTSTATUS Status;

    /* Initialize ACPI */
    Status = Acpi::InitializeAcpi();
    if(Status != STATUS_SUCCESS)
    {
        return Status;
    }

    /* Get system information from ACPI */
    Status = Acpi::InitializeAcpiSystemInformation();
    if(Status != STATUS_SUCCESS)
    {
        return Status;
    }

    /* Return success */
    return STATUS_SUCCESS;
}
