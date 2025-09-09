/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/krnlinit.cc
 * DESCRIPTION:     XT kernel initialization
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/* Use routines from Kernel Library */
using namespace KE;

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
    /* Verify kernel and boot loader compatibility */
    if(Parameters->BlockSize != sizeof(KERNEL_INITIALIZATION_BLOCK) ||
       Parameters->BlockVersion != INITIALIZATION_BLOCK_VERSION ||
       Parameters->ProtocolVersion != BOOT_PROTOCOL_VERSION)
    {
        /* Kernel and boot loader version mismatch */
        Crash::HaltSystem();
    }

    /* Save the kernel initialization block */
    BootInformation::SetInitializationBlock(Parameters);

    /* Check if debugging enabled and if boot loader provided routine for debug printing */
    if(DEBUG && BootInformation::GetDebugPrint())
    {
        /* Use loader's provided DbgPrint() routine for early printing to serial console */
        KdSetPrintRoutine(BootInformation::GetDebugPrint());
        DebugPrint(L"Initializing ExectOS v%d.%d for %s\n", XTOS_VERSION_MAJOR, XTOS_VERSION_MINOR, _ARCH_NAME);
    }

    /* Initialize boot CPU */
    AR::ProcSup::InitializeProcessor(NULL);

    /* Initialize system resources */
    SystemResources::InitializeResources();

    /* Check if debugging enabled */
    if(DEBUG)
    {
        /* Initialize debug I/O */
        KdInitializeDebugIoProviders();
    }

    /* Announce kernel startup */
    DebugPrint(L"Starting ExectOS v%d.%d for %s (%s-%s-%s-%s / %s %s)\n",
               XTOS_VERSION_MAJOR, XTOS_VERSION_MINOR, _ARCH_NAME, XTOS_VERSION_DATE,
               XTOS_VERSION_BUILD, XTOS_VERSION_ARCH, XTOS_VERSION_HASH,
               XTOS_COMPILER_NAME, XTOS_COMPILER_VERSION);

    /* Architecture specific kernel initialization */
    KernelInit::InitializeMachine();

    /* Raise to HIGH runlevel */
    RunLevel::RaiseRunLevel(HIGH_LEVEL);

    /* Switch the boot stack and transfer control to the KepStartKernel() routine */
    KernelInit::SwitchBootStack();
}
