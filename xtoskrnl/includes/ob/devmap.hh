/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ob/devmap.hh
 * DESCRIPTION:     Object Manager Device Map
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_OB_DEVMAP_HH
#define __XTOSKRNL_OB_DEVMAP_HH

#include <xtos.hh>


/* Object Manager */
namespace OB
{
    class DeviceMap
    {
        private:
            STATIC KPUSH_LOCK DeviceMapLock;
            STATIC BOOLEAN UniqueDeviceMaps;

        public:
            STATIC XTAPI POBJECT_DIRECTORY GetGlobalDevicesDirectory(IN POBJECT_DIRECTORY Directory);
            STATIC XTAPI BOOLEAN GetUniqueDeviceMaps(VOID);
            STATIC XTAPI XTSTATUS InitializeDeviceMap(VOID);
    };
}

#endif /* __XTOSKRNL_OB_DEVMAP_HH */
