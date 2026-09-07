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
 * Inherits the device map from a parent process or assigns the system default.
 *
 * @param NewProcess
 *        Supplies the newly created process that will inherit the device map.
 *
 * @param ParentProcess
 *        Supplies the optional parent process to inherit the device map from.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
VOID
OB::DeviceMap::InheritDeviceMap(IN PEPROCESS NewProcess,
                                IN PEPROCESS ParentProcess)
{
    PDEVICE_MAP TargetDeviceMap;

    /* Acquire the device map lock */
    KE::PushLockExclusiveGuard PushLock(&DeviceMapLock);

    /* Determine the source of the device map */
    if(ParentProcess)
    {
        /* Inherit the device map from the provided parent process */
        TargetDeviceMap = (PDEVICE_MAP)ParentProcess->DeviceMap;
    }
    else
    {
        /* No parent process provided, use system device map */
        TargetDeviceMap = SystemDeviceMap;
    }

    /* Check if a valid device map was resolved */
    if(TargetDeviceMap)
    {
        /* Increment the reference count of the target device map */
        TargetDeviceMap->ReferenceCount++;

        /* Assign the device map to the process */
        NewProcess->DeviceMap = TargetDeviceMap;
    }
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
