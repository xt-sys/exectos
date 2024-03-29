/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/efiutils.c
 * DESCRIPTION:     EFI related routines for XT Boot Loader
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtldr.h>


/**
 * Reboots into UEFI firmware setup interface.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlEnterFirmwareSetup()
{
    EFI_GUID Guid = EFI_GLOBAL_VARIABLE_GUID;
    PULONGLONG SetupSupport;
    ULONGLONG Indications;
    EFI_STATUS Status;

    /* Check if booting into firmware interface is supported */
    Status = BlGetEfiVariable(&Guid, L"OsIndicationsSupported", (PVOID*)&SetupSupport);
    if(Status != STATUS_EFI_SUCCESS || !(*SetupSupport & EFI_OS_INDICATIONS_BOOT_TO_FW_UI))
    {
        /* Reboot into firmware setup is not supported */
        BlDebugPrint(L"WARNING: Reboot into firmware setup interface not supported\n");
        return STATUS_EFI_UNSUPPORTED;
    }

    /* Get the value of OsIndications variable */
    Indications = 0;
    Status = BlGetEfiVariable(&Guid, L"OsIndications", (PVOID)&Indications);

    /* Enable FW setup on next boot */
    Indications |= EFI_OS_INDICATIONS_BOOT_TO_FW_UI;
    Status = BlSetEfiVariable(&Guid, L"OsIndications", (PVOID)&Indications, sizeof(Indications));
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to update OsIndications variable */
        return Status;
    }

    /* Reboot into firmware setup */
    BlRebootSystem();

    /* Must not reach this point, just make the compiler happy */
    return STATUS_EFI_SUCCESS;
}

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
        BlDebugPrint(L"ERROR: Memory allocation failure (Status Code: 0x%zX)\n", Status);
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
 * Gets the value of the EFI variable.
 *
 * @param Vendor
 *        Supplies a pointer to the unique vendor GUID.
 *
 * @param VariableName
 *        Supplies a pointer to tge NULL-terminated string containing the variable name.
 *
 * @param VariableValue
 *        Supplies a pointer to the buffer, where the variable value will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlGetEfiVariable(IN PEFI_GUID Vendor,
                 IN PWCHAR VariableName,
                 OUT PVOID *VariableValue)
{
    EFI_STATUS Status;
    PVOID Buffer;
    UINT_PTR Size;

    /* Allocate a buffer for storing a variable's value */
    Size = EFI_MAXIMUM_VARIABLE_SIZE * sizeof(PWCHAR);
    Status = BlAllocateMemoryPool(Size, (PVOID*)&Buffer);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure */
        return Status;
    }

    /* Attempt to get variable value */
    Status = EfiSystemTable->RuntimeServices->GetVariable(VariableName, Vendor, NULL, &Size, Buffer);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to get variable, probably not found such one */
        return Status;
    }

    /* Get variable value and return success */
    *VariableValue = Buffer;
    return STATUS_EFI_SUCCESS;
}

/**
 * Returns a random value based on the initialized RNG buffer.
 *
 * @param RNGBuffer
 *        Supplies a pointer to the RNG buffer.
 *
 * @return This routine returns a random value.
 *
 * @since XT 1.0
 *
 * @see https://en.wikipedia.org/wiki/Xorshift
 */
XTCDECL
ULONGLONG
BlGetRandomValue(IN OUT PULONGLONG RNGBuffer)
{
    /* Recalculate RNG buffer with XORSHIFT */
    *RNGBuffer ^= *RNGBuffer >> 12;
    *RNGBuffer ^= *RNGBuffer << 25;
    *RNGBuffer ^= *RNGBuffer >> 27;

    /* Return random value */
    return *RNGBuffer * 0x2545F4914F6CDD1D;
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
 * Initializes the RNG buffer with random bytes from the default EFI RNG algorithm.
 *
 * @param RNGBuffer
 *        Supplies a pointer to the RNG buffer.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlInitializeEntropy(PULONGLONG RNGBuffer)
{
    EFI_GUID RngGuid = EFI_RNG_PROTOCOL_GUID;
    PEFI_RNG_PROTOCOL Rng;
    EFI_STATUS Status;
    ULONGLONG Seed;

    /* Initialize variables */
    Rng = NULL;
    Seed = 0;

    /* Locate RNG protocol */
    Status = EfiSystemTable->BootServices->LocateProtocol(&RngGuid, NULL, (PVOID *)&Rng);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to locate RNG protocol, return status code */
        return Status;
    }

    /* Get RNG value using the default algorithm */
    Status = Rng->GetRNG(Rng, NULL, 8, (PUCHAR)&Seed);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to get RNG value, return status code */
        return Status;
    }

    /* Initialize RNG state and return success */
    *RNGBuffer = Seed ? Seed : 1;
    return STATUS_EFI_SUCCESS;
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
 * Sets a value of an EFI variable.
 *
 * @param Vendor
 *        Supplies a pointer to the unique vendor GUID.
 *
 * @param VariableName
 *        Supplies a pointer to tge NULL-terminated string containing the variable name.
 *
 * @param VariableValue
 *        Supplies the contents of the variable.
 *
 * @param Size
 *        Supplies the size of the variable data buffer.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlSetEfiVariable(IN PEFI_GUID Vendor,
                 IN PWCHAR VariableName,
                 IN PVOID VariableValue,
                 IN UINT_PTR Size)
{
    ULONG Attributes;

    /* Set EFI variable */
    Attributes = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;
    return EfiSystemTable->RuntimeServices->SetVariable(VariableName, Vendor, Attributes, Size, VariableValue);
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
