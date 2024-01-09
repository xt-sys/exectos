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
 * @param MapKey
 *        Identifies the current memory map of the system.
 *
 * @return This routine returns status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlExitBootServices(IN UINT_PTR MapKey)
{
    EFI_STATUS Status;

    /* Attempt to exit boot services */
    Status = EfiSystemTable->BootServices->ExitBootServices(EfiImageHandle, MapKey);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Retry as UEFI spec says to do it twice */
        Status = EfiSystemTable->BootServices->ExitBootServices(EfiImageHandle, MapKey);
    }

    /* Make sure boot services were successfully exited */
    if(Status == STATUS_EFI_SUCCESS)
    {
        /* Mark EFI Boot Services as no longer available */
        BlpStatus.BootServices = FALSE;
    }

    /* Return EFI status code */
    return Status;
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
