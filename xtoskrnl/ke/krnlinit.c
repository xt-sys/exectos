/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/krnlinit.c
 * DESCRIPTION:     XT kernel initialization
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtkmapi.h>


/**
 * This routine starts up the XT kernel. It is called by boot loader.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KeStartXtSystem(IN PKERNEL_INITIALIZATION_BLOCK Parameters)
{
    /* Use XTLDR provided DbgPrint() routine for early printing to serial console */
    VOID (*DbgPrint)(IN PWCHAR Format, IN ...) = Parameters->LoaderInformation.DbgPrint;

    /* Print some message to serial console */
    DbgPrint(L"Hello world from ExectOS kernel!\n");

    /* Test kernel parameters */
    DbgPrint(L"\n\n------ Kernel parameters block ------\n"
             L"Loader block size: %lu\n"
             L"Loader block version: %lu\n"
             L"EFI Revision: %lu\n"
             L"EFI RunTime Revision: %lu\n",
             Parameters->Size,
             Parameters->Version,
             Parameters->FirmwareInformation.EfiFirmware.EfiVersion,
             ((PEFI_RUNTIME_SERVICES) Parameters->FirmwareInformation.EfiFirmware.EfiRuntimeServices)->Hdr.Revision
            );

    /* Enter infinite kernel thread loop */
    for(;;);
}
