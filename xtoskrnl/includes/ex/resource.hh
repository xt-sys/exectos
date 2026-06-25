/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ex/resource.hh
 * DESCRIPTION:     Exclusive resource synchronization support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_EX_RESOURCE_HH
#define __XTOSKRNL_EX_RESOURCE_HH

#include <xtos.hh>


/* Kernel Executive */
namespace EX
{
    class Resources
    {
        private:
            STATIC LARGE_INTEGER ResourcesTimeOut;
            STATIC LIST_ENTRY SystemResourcesList;
            STATIC KSPIN_LOCK SystemResourcesLock;

        public:
            STATIC XTAPI XTSTATUS DeleteResource(IN PERESOURCE Resource);
            STATIC XTAPI XTSTATUS InitializeResource(IN PERESOURCE Resource);
            STATIC XTAPI VOID InitializeSystemResources(VOID);
            STATIC XTAPI XTSTATUS ReinitializeResource(IN PERESOURCE Resource);
    };
}

#endif /* __XTOSKRNL_EX_RESOURCES_HH */
