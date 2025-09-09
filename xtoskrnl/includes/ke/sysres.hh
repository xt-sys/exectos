/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke/sysres.hh
 * DESCRIPTION:     System boot resources management
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KE_SYSRES_HH
#define __XTOSKRNL_KE_SYSRES_HH

#include <xtos.hh>


/* Kernel Library */
namespace KE
{
    class SystemResources
    {
        private:
            STATIC LIST_ENTRY ResourcesListHead;
            STATIC KSPIN_LOCK ResourcesLock;

        public:
            STATIC XTAPI XTSTATUS AcquireResource(IN SYSTEM_RESOURCE_TYPE ResourceType,
                                                  OUT PSYSTEM_RESOURCE_HEADER *ResourceHeader);
            STATIC XTAPI XTSTATUS GetResource(IN SYSTEM_RESOURCE_TYPE ResourceType,
                                              OUT PSYSTEM_RESOURCE_HEADER *ResourceHeader);
            STATIC XTAPI VOID InitializeResources(VOID);
            STATIC XTAPI VOID ReleaseResource(IN PSYSTEM_RESOURCE_HEADER ResourceHeader);

        private:
            STATIC XTAPI XTSTATUS GetSystemResource(IN SYSTEM_RESOURCE_TYPE ResourceType,
                                                    IN BOOLEAN ResourceLock,
                                                    OUT PSYSTEM_RESOURCE_HEADER *ResourceHeader);

    };
}

#endif /* __XTOSKRNL_KE_SYSRES_HH */
