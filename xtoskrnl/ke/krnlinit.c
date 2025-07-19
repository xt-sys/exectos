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
    /* Save the kernel initialization block */
    KeInitializationBlock = Parameters;

    /* Verify kernel and boot loader compatibility */
    if(KeInitializationBlock->BlockSize != sizeof(KERNEL_INITIALIZATION_BLOCK) ||
       KeInitializationBlock->BlockVersion != INITIALIZATION_BLOCK_VERSION ||
       KeInitializationBlock->ProtocolVersion != BOOT_PROTOCOL_VERSION)
    {
        /* Kernel and boot loader version mismatch */
        KeHaltSystem();
    }

    /* Check if debugging enabled and if boot loader provided routine for debug printing */
    if(DEBUG && KeInitializationBlock->LoaderInformation.DbgPrint)
    {
        /* Use loader's provided DbgPrint() routine for early printing to serial console */
        KeDbgPrint = KeInitializationBlock->LoaderInformation.DbgPrint;
    }

    /* Announce kernel startup */
    DebugPrint(L"Starting ExectOS v%d.%d for %s (%s-%s-%s-%s / %s %s)\n",
               XTOS_VERSION_MAJOR, XTOS_VERSION_MINOR, _ARCH_NAME, XTOS_VERSION_DATE,
               XTOS_VERSION_BUILD, XTOS_VERSION_ARCH, XTOS_VERSION_HASH,
               XTOS_COMPILER_NAME, XTOS_COMPILER_VERSION);

    /* Initialize boot CPU */
    ArInitializeProcessor(NULL);

    /* Initialize system resources */
    KepInitializeSystemResources();

    /* Architecture specific kernel initialization */
    KepInitializeMachine();

    /* Raise to HIGH runlevel */
    KeRaiseRunLevel(HIGH_LEVEL);

    /* Switch the boot stack, setting the pointer to the top of the buffer and aligning it to a 4-byte boundary */
    KepSwitchBootStack(((ULONG_PTR)&ArKernelBootStack + KERNEL_STACK_SIZE) & ~0x3);
}
