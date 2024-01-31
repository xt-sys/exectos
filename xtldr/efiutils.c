/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/efiutils.c
 * DESCRIPTION:     EFI related routines for XT Boot Loader
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtldr.h>


/**
 * Exits EFI boot services.
 *
 * @return This routine returns status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlExitBootServices()
{
    PEFI_MEMORY_MAP MemoryMap;
    EFI_STATUS Status;
    ULONG Counter;

    /* Boot Services might be partially shutdown, so mark them as unavailable */
    BlpStatus.BootServices = FALSE;

    /* Allocate buffer for EFI memory map */
    Status = BlAllocateMemoryPool(sizeof(EFI_MEMORY_MAP), (PVOID*)&MemoryMap);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure */
        BlDebugPrint(L"ERROR: Memory allocation failure (Status Code: 0x%lx)\n", Status);
        return Status;
    }

    /* Zero fill the buffer and initialize counter */
    RtlZeroMemory(MemoryMap, sizeof(EFI_MEMORY_MAP));
    Counter = 0xFF;

    /* Attempt to exit boot services */
    while(Counter > 0)
    {
        /* Get memory map each time as it can change between two calls */
        Status = BlGetMemoryMap(MemoryMap);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Failed to get new memory map */
            return Status;
        }

        /* Exit boot services */
        Status = EfiSystemTable->BootServices->ExitBootServices(EfiImageHandle, MemoryMap->MapKey);
        if(Status == STATUS_EFI_SUCCESS)
        {
            break;
        }

        /* Decrement counter */
        Counter--;
    }

    /* Return EFI status code */
    return Status;
}

/**
 * Gets the address of a reqested system configuration table.
 *
 * @param TableGuid
 *        Supplies a GUID of the configuration table.
 *
 * @param Table
 *        Supplies a pointer to the memory area where the table address will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlGetConfigurationTable(IN PEFI_GUID TableGuid,
                        OUT PVOID *Table)
{
    SIZE_T Index;

    /* Iterate through all system configuration tables */
    for(Index = 0; Index < EfiSystemTable->NumberOfTableEntries; Index++)
    {
        /* Check if this table matches requested table */
        if(RtlCompareGuids((PGUID)&(EfiSystemTable->ConfigurationTable[Index].VendorGuid), (PGUID)TableGuid))
        {
            /* Found requested table, return success */
            *Table = EfiSystemTable->ConfigurationTable[Index].VendorTable;
            return STATUS_EFI_SUCCESS;
        }
    }

    /* Table not found */
    *Table = NULL;
    return STATUS_EFI_NOT_FOUND;
}

/**
 * Checks whether SecureBoot is enabled or not.
 *
 * @return Numeric representation of SecureBoot status (0 = Disabled, >0 = Enabled, <0 SetupMode).
 *
 * @since XT 1.0
 */
XTCDECL
INT_PTR
BlGetSecureBootStatus()
{
    EFI_GUID VarGuid = EFI_GLOBAL_VARIABLE_GUID;
    INT_PTR SecureBootStatus = 0;
    UCHAR VarValue = 0;
    UINT_PTR Size;

    Size = sizeof(VarValue);
    if(EfiSystemTable->RuntimeServices->GetVariable(L"SecureBoot", &VarGuid,
       NULL, &Size, &VarValue) == STATUS_EFI_SUCCESS)
    {
        SecureBootStatus = (INT_PTR)VarValue;

        if((EfiSystemTable->RuntimeServices->GetVariable(L"SetupMode", &VarGuid,
           NULL, &Size, &VarValue) == STATUS_EFI_SUCCESS) && VarValue != 0)
        {
            SecureBootStatus = -1;
        }
    }

    /* Return SecureBoot status */
    return SecureBootStatus;
}

/**
 * Loads an EFI image into memory.
 *
 * @param DevicePath
 *        Specifies a device path from which the image is loaded.
 *
 * @param ImageData
 *        Supplies a pointer to the memory are containing a copy of the EFI image.
 *
 * @param ImageSize
 *        Supplies the size (in bytes) of the EFI image.
 *
 * @param ImageHandle
 *        Supplies a pointer to the memory area, where an EFI_image handle will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlLoadEfiImage(IN PEFI_DEVICE_PATH_PROTOCOL DevicePath,
               IN PVOID ImageData,
               IN SIZE_T ImageSize,
               OUT PEFI_HANDLE ImageHandle)
{
    /* Load EFI image */
    return EfiSystemTable->BootServices->LoadImage(FALSE, EfiImageHandle, DevicePath, ImageData, ImageSize, ImageHandle);
}

/**
 * Reboots the machine.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlRebootSystem()
{
    /* Reboot machine */
    return EfiSystemTable->RuntimeServices->ResetSystem(EfiResetCold, STATUS_EFI_SUCCESS, 0, NULL);
}

/**
 * Shuts down the machine.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlShutdownSystem()
{
    /* Shutdown machine */
    return EfiSystemTable->RuntimeServices->ResetSystem(EfiResetShutdown, STATUS_EFI_SUCCESS, 0, NULL);
}

/**
 * Puts the system to sleep for the specified number of milliseconds.
 *
 * @param Milliseconds
 *        Supplies the number of milliseconds to sleep.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlSleepExecution(IN ULONG_PTR Milliseconds)
{
    EfiSystemTable->BootServices->Stall(Milliseconds * 1000);
}

/**
 * Executes a loaded EFI image entry point.
 *
 * @param ImageHandle
 *        Provides a handle of loaded image, that will be started.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlStartEfiImage(IN EFI_HANDLE ImageHandle)
{
    return EfiSystemTable->BootServices->StartImage(ImageHandle, NULL, NULL);
}

/**
 * Waits for one or more EFI events.
 *
 * @param NumberOfEvents
 *        Supplies the number of events to wait for.
 *
 * @param Event
 *        Supplies the array of events to wait for.
 *
 * @param Index
 *        Receives the index of the event that was signaled.
 *
 * @return This routine returns status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlWaitForEfiEvent(IN UINT_PTR NumberOfEvents,
                  IN PEFI_EVENT Event,
                  OUT PUINT_PTR Index)
{
    return EfiSystemTable->BootServices->WaitForEvent(NumberOfEvents, Event, Index);
}
