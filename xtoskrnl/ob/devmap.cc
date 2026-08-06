/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ob/devmap.cc
 * DESCRIPTION:     Object Manager Device Map
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Retrieves the Global Devices Directory associated with a given directory's device map.
 *
 * @param Directory
 *        Supplies a pointer to the object directory to query.
 *
 * @return Returns a pointer to the global devices directory, or NULLPTR if none exists.
 *
 * @since XT 1.0
 */
XTAPI
POBJECT_DIRECTORY
OB::DeviceMap::GetGlobalDevicesDirectory(IN POBJECT_DIRECTORY Directory)
{
    POBJECT_DIRECTORY GlobalDevicesDirectory;

    /* Assume no mapping exists */
    GlobalDevicesDirectory = NULLPTR;

    /* Enter guarded region and acquire the device map lock */
    KE::GuardedRegionGuard GuardedRegion;
    KE::PushLockExclusiveGuard PushLock(&DeviceMapLock);

    /* Check if the directory has an associated device map */
    if(Directory->DeviceMap)
    {
        /* Extract the global devices directory */
        GlobalDevicesDirectory = Directory->DeviceMap->GlobalDevicesDirectory;
    }

    /* Return the resolved directory */
    return GlobalDevicesDirectory;
}

/**
 * Retrieves the current state of the unique device maps configuration.
 *
 * @return This routine returns TRUE if unique device maps are enabled, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
OB::DeviceMap::GetUniqueDeviceMaps(VOID)
{
    /* Return the unique device maps flag */
    return UniqueDeviceMaps;
}

/**
 * Initializes the Object Manager's device map subsystem.
 *
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
OB::DeviceMap::InitializeDeviceMap(VOID)
{
    /* Initialize device map lock */
    KE::PushLock::InitializePushLock(&DeviceMapLock);

    /* Temporarily disable unique device maps */
    UniqueDeviceMaps = FALSE;

    /* Return success */
    return STATUS_SUCCESS;
}
