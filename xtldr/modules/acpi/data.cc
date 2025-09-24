/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/acpi/data.cc
 * DESCRIPTION:     ACPI module global and static data
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <acpi.hh>


/* ACPI Protocol */
XTBL_ACPI_PROTOCOL Acpi::AcpiProtocol;

/* XTLDR protocol handler */
PXTBL_LOADER_PROTOCOL Acpi::XtLdrProtocol;
