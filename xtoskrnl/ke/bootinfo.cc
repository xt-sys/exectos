/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/bootinfo.cc
 * DESCRIPTION:     Bootloader-provided system information handling support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/* Kernel Library */
namespace KE
{

XTAPI
PVOID
BootInformation::GetDebugPrint(VOID)
{
    return InitializationBlock->LoaderInformation.DbgPrint;
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
BootInformation::GetFirmwareType(VOID)
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
 *        Supplies a pointer to a variable that receives a pointer to the matching parameter, or NULL if not found.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
BootInformation::GetKernelParameter(IN PCWSTR ParameterName,
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
    ParameterNameLength = RtlWideStringLength(ParameterName, 0);
    if(ParameterNameLength == 0)
    {
        /* Do not allow empty parameter names */
        return STATUS_INVALID_PARAMETER;
    }

    /* Assume the requested parameter is not present in the kernel parameters */
    *Parameter = nullptr;

    /* Start searching from the beginning of the list */
    SearchStart = InitializationBlock->KernelParameters;

    /* Search for the parameter name */
    while((Match = RtlFindWideStringInsensitive(SearchStart, ParameterName)))
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
 *
 *
 *
 *
 */
XTAPI
PLIST_ENTRY
BootInformation::GetSystemResources(VOID)
{
    return &InitializationBlock->SystemResourcesListHead;
}

/**
 *
 *
 *
 *
 */
XTAPI
VOID
BootInformation::SetInitializationBlock(IN PKERNEL_INITIALIZATION_BLOCK Block)
{
    InitializationBlock = Block;
}

} /* namespace */


/* TEMPORARY FOR COMPATIBILITY WITH C CODE */
XTCLINK
XTAPI
XTSTATUS
KeGetKernelParameter(IN PCWSTR ParameterName,
                   OUT PCWSTR *Parameter)
{
    return KE::BootInformation::GetKernelParameter(ParameterName, Parameter);
}

XTCLINK
XTAPI
PKERNEL_INITIALIZATION_BLOCK
KeGetInitializationBlock(VOID)
{
    return KE::BootInformation::GetInitializationBlock();
}