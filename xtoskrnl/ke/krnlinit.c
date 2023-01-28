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
        LdrDbgPrint = Parameters->LoaderInformation.DbgPrint;
    }


    /* Print some message to serial console and test kernel parameters */
    LdrPrint(L"Hello world from ExectOS kernel!\n");
    LdrPrint(L"\n\n------ Kernel parameters block ------\n"
             L"Loader block size: %lu\n"
             L"Loader block version: %lu\n"
             L"EFI Revision: %lu\n",
             Parameters->Size,
             Parameters->Version,
             Parameters->FirmwareInformation.EfiFirmware.EfiVersion
            );


    /* Save the kernel initialization block */
    KeInitializationBlock = Parameters;

    /* Initialize kernel stacks */
    KepInitializeStack(Parameters);

    /* Architecture specific initialization */
    KepArchInitialize();

    /* Switch boot stack alligning it to 4 byte boundary */
    KepSwitchBootStack(KeInitializationBlock->KernelBootStack & ~0x3);
}

/**
 * Initializes a stack needed by the kernel.
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
KepInitializeStack(IN PKERNEL_INITIALIZATION_BLOCK Parameters)
{
    /* Make sure kernel boot stack is initialized */
    if(!Parameters->KernelBootStack)
    {
        /* Initialize kernel boot stack */
        LdrPrint(L"Initializing kernel boot stack\n");
        Parameters->KernelBootStack = (ULONG_PTR)&KepKernelBootStackData[KERNEL_STACK_SIZE];
    }

    /* Make sure kernel fault stack is initialized */
    if(!Parameters->KernelFaultStack)
    {
        /* Initialize kernel fault stack */
        LdrPrint(L"Initializing kernel fault stack\n");
        Parameters->KernelFaultStack = (ULONG_PTR)&KepKernelFaultStackData[KERNEL_STACK_SIZE];
    }
}
