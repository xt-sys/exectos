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
    if(DEBUG && Parameters->LoaderInformation.DbgPrint)
    {
        /* Use loader's provided DbgPrint() routine for early printing to serial console */
        KeDbgPrint = Parameters->LoaderInformation.DbgPrint;
    }


    /* Print some message to serial console and test kernel parameters */
    DebugPrint(L"Hello world from ExectOS kernel!\n");
    DebugPrint(L"\n\n------ Kernel parameters block ------\n"
             L"Loader block size: %lu\n"
             L"Loader block version: %lu\n"
             L"EFI Revision: %lu\n",
             Parameters->Size,
             Parameters->Version,
             Parameters->FirmwareInformation.EfiFirmware.EfiVersion
            );


    /* Save the kernel initialization block */
    KeInitializationBlock = Parameters;

    /* Initialize boot CPU */
    ArInitializeProcessor();

    /* Architecture specific kernel initialization */
    KepInitializeMachine();

    /* Switch boot stack aligning it to 4 byte boundary */
    KepSwitchBootStack((ULONG_PTR)&ArKernelBootStack & ~0x3);
}
