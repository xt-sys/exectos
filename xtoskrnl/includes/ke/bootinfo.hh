/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke/bootinfo.hh
 * DESCRIPTION:     Bootloader-provided system information handling support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KE_BOOTINFO_HH
#define __XTOSKRNL_KE_BOOTINFO_HH

#include <xtos.hh>


/* Kernel Library */
namespace KE
{
    class BootInformation
    {
        private:
            STATIC PKERNEL_INITIALIZATION_BLOCK InitializationBlock;

        public:
            STATIC XTAPI PKD_PRINT_ROUTINE GetDebugPrint(VOID);
            STATIC XTAPI SYSTEM_FIRMWARE_TYPE GetFirmwareType(VOID);
            STATIC XTAPI XTSTATUS GetKernelParameter(IN PCWSTR ParameterName,
                                                     OUT PCWSTR *Parameter);
            STATIC XTAPI PLIST_ENTRY GetSystemResources(VOID);
            STATIC XTAPI VOID InitializeInitializationBlock(IN PKERNEL_INITIALIZATION_BLOCK Block);

            /* TEMPORARY FOR COMPATIBILITY WITH C CODE */
            STATIC XTAPI PKERNEL_INITIALIZATION_BLOCK GetInitializationBlock(VOID)
            {
                return InitializationBlock;
            }
    };
}

#endif /* __XTOSKRNL_KE_BOOTINFO_HH */
