/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/krnlinit.c
 * DESCRIPTION:     XT kernel initialization
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * This routine starts up the XT kernel. It is called by boot loader.
 *
 * @param Parameters
 *        Supplies a pointer to memory area containing parameters passed to kernel by bootloader.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KeStartXtSystem(IN PKERNEL_INITIALIZATION_BLOCK Parameters)
{
    /* Check if debugging enabled and if boot loader provided routine for debug printing */
    if(DBG && Parameters->LoaderInformation.DbgPrint)
    {
        /* Use loader's provided DbgPrint() routine for early printing to serial console */
        LdrPrint = Parameters->LoaderInformation.DbgPrint;
    }

    /* Make sure kernel boot stack is initialized */
    if(!Parameters->KernelBootStack)
    {
        /* Initialize kernel boot stack */
        LDRPRINT(L"Initializing kernel boot stack\n");
        Parameters->KernelBootStack = (ULONG_PTR)&KepKernelBootStackData[KERNEL_STACK_SIZE];
    }

    /* Make sure kernel fault stack is initialized */
    if(!Parameters->KernelFaultStack)
    {
        /* Initialize kernel fault stack */
        LDRPRINT(L"Initializing kernel fault stack\n");
        Parameters->KernelFaultStack = (ULONG_PTR)&KepKernelFaultStackData[KERNEL_STACK_SIZE];
    }

    /* Print some message to serial console */
    LDRPRINT(L"Hello world from ExectOS kernel!\n");

    /* Test kernel parameters */
    LDRPRINT(L"\n\n------ Kernel parameters block ------\n"
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
