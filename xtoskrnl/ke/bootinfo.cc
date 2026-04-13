/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/bootinfo.cc
 * DESCRIPTION:     Bootloader-provided system information handling support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Retrieves a pointer to the DebugPrint routine provided by the bootloader.
 *
 * @return This routine returns a pointer to the DebugPrint routine.
 *
 * @since XT 1.0
 */
XTAPI
PKD_PRINT_ROUTINE
KE::BootInformation::GetDebugPrint(VOID)
{
    return (PKD_PRINT_ROUTINE)InitializationBlock->LoaderInformation.DbgPrint;
}

/**
 * Retrieves the system firmware type (BIOS or UEFI).
 *
 * @return This routine returns the type of the system firmware.
 *
 * @since XT 1.0
 */
XTAPI
SYSTEM_FIRMWARE_TYPE
KE::BootInformation::GetFirmwareType(VOID)
{
    return InitializationBlock->FirmwareInformation.FirmwareType;
}

/**
 * Retrieves a pointer to the specified kernel parameter within the kernel parameters list.
 *
 * @param ParameterName
 *        Supplies a pointer to a null-terminated wide string specifying the name of the parameter to search for.
 *
 * @param Parameter
 *        Supplies a pointer to a variable that receives a pointer to the matching parameter, or NULLPTR if not found.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
KE::BootInformation::GetKernelParameter(IN PCWSTR ParameterName,
                                        OUT PCWSTR *Parameter)
{
    PCWSTR Match, SearchStart;
    SIZE_T ParameterNameLength;

    /* Validate input parameters */
    if(!ParameterName || !Parameter)
    {
        /* Invalid input parameters, return error */
        return STATUS_INVALID_PARAMETER;
    }

    /* Get the length of the parameter name we are looking for */
    ParameterNameLength = RTL::WideString::WideStringLength(ParameterName, 0);
    if(ParameterNameLength == 0)
    {
        /* Do not allow empty parameter names */
        return STATUS_INVALID_PARAMETER;
    }

    /* Assume the requested parameter is not present in the kernel parameters */
    *Parameter = NULLPTR;

    /* Start searching from the beginning of the list */
    SearchStart = InitializationBlock->KernelParameters;

    /* Search for the parameter name */
    while((Match = RTL::WideString::FindWideStringInsensitive(SearchStart, ParameterName)))
    {
        /* Check if the match is at the start of the string or preceded by a space */
        if(Match == InitializationBlock->KernelParameters || *(Match - 1) == L' ')
        {
            /* Check the character after the match to avoid matching prefixes */
            if(Match[ParameterNameLength] == L'\0' ||
               Match[ParameterNameLength] == L' ' ||
               Match[ParameterNameLength] == L'=')
            {
                /* A valid parameter was found, return a pointer to it */
                *Parameter = Match;
                return STATUS_SUCCESS;
            }
        }

        /* The match was a substring of a larger token, continue searching */
        SearchStart = Match + 1;
    }

    /* Parameter not found */
    return STATUS_NOT_FOUND;
}

/**
 * Retrieves the value of a specified kernel parameter and copies it into a buffer.
 *
 * @param ParameterName
 *        Supplies a pointer to a null-terminated wide string specifying the name of the parameter to search for.
 *
 * @param ValueBuffer
 *        Supplies a pointer to a variable that receives the null-terminated value of the matching parameter.
 *
 * @param BufferSize
 *        Supplies the size of the value buffer, in wide characters.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
KE::BootInformation::GetKernelParameterValue(IN PCWSTR ParameterName,
                                             OUT PWSTR ValueBuffer,
                                             IN SIZE_T BufferSize)
{
    PCWSTR Match;
    SIZE_T NameLength, Index;
    XTSTATUS Status;

    /* Validate input parameters */
    if(!ParameterName || !ValueBuffer || BufferSize == 0)
    {
        /* Invalid input parameters, return error */
        return STATUS_INVALID_PARAMETER;
    }

    /* Initialize the output buffer to an empty string */
    ValueBuffer[0] = L'\0';

    /* Find the parameter in the list using the base function */
    Status = GetKernelParameter(ParameterName, &Match);
    if(Status != STATUS_SUCCESS)
    {
        /* Parameter not found, return error */
        return Status;
    }

    /* Move pointer past the parameter name */
    NameLength = RTL::WideString::WideStringLength(ParameterName, 0);
    Match += NameLength;

    /* If the parameter has a value (indicated by '='), copy it */
    if(*Match == L'=')
    {
        /* Skip the assignment operator */
        Match++;

        /* Copy the value to the caller's buffer until a space or end of string is reached */
        Index = 0;
        while(*Match != L'\0' && *Match != L' ' && Index < (BufferSize - 1))
        {
            /* Copy the character */
            ValueBuffer[Index] = *Match;
            Index++;
            Match++;
        }

        /* Null-terminate the isolated value string */
        ValueBuffer[Index] = L'\0';
    }

    /* Value successfully retrieved (or parameter exists without value) */
    return STATUS_SUCCESS;
}

/**
 * Retrieves a pointer to the list of memory descriptors.
 *
 * @return This routine returns a pointer to the list of memory descriptors.
 *
 * @since XT 1.0
 */
XTAPI
PLIST_ENTRY
KE::BootInformation::GetMemoryDescriptors(VOID)
{
    return &InitializationBlock->MemoryDescriptorListHead;
}

/**
 * Retrieves a pointer to the list of system resources.
 *
 * @return This routine returns a pointer to the list of system resources.
 *
 * @since XT 1.0
 */
XTAPI
PLIST_ENTRY
KE::BootInformation::GetSystemResources(VOID)
{
    return &InitializationBlock->SystemResourcesListHead;
}

/**
 * Initializes the bootloader-provided system information.
 *
 * @param Block
 *        Supplies a pointer to the kernel initialization block.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::BootInformation::InitializeInitializationBlock(IN PKERNEL_INITIALIZATION_BLOCK Block)
{
    /* Check if the initialization block is already initialized */
    if(!InitializationBlock)
    {
        /* Save the kernel initialization block */
        InitializationBlock = Block;
    }
}
