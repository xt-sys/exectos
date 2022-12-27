/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/system.c
 * DESCRIPTION:     EFI system information
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtbl.h>


/**
 * This routine checks whether SecureBoot is enabled or not.
 *
 * @return Numeric representation of SecureBoot status (0 = Disabled, >0 = Enabled, <0 SetupMode).
 *
 * @since XT 1.0
 */
XTCDECL
INT_PTR
BlEfiGetSecureBootStatus()
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
 * Read system configuration from a specified table.
 *
 * @param TableGuid
 *        Supplies a pointer to the GUID to search for.
 *
 * @param Table
 *        Supplies a pointer that will point to the configuration table.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlEfiGetSystemConfigurationTable(IN PEFI_GUID TableGuid,
                                 OUT PVOID *Table)
{
    SIZE_T Size = sizeof(EFI_GUID);
    UINT_PTR Index;

    for(Index = 0; Index < EfiSystemTable->NumberOfTableEntries; Index++)
    {
        if(RtlCompareMemory((PVOID)&EfiSystemTable->ConfigurationTable[Index].VendorGuid, TableGuid, Size) == Size)
        {
            *Table = EfiSystemTable->ConfigurationTable[Index].VendorTable;
            return STATUS_EFI_SUCCESS;
        }
    }

    /* Table not found */
    return STATUS_EFI_NOT_FOUND;
}
