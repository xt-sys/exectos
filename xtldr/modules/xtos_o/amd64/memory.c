/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/amd64/memory.c
 * DESCRIPTION:     EFI memory management for AMD64 target
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Builds the actual memory mapping page table and enables paging. This routine exits EFI boot services as well.
 *
 * @param MemoryMappings
 *        Supplies a pointer to linked list containing all memory mappings.
 *
 * @param VirtualAddress
 *        Supplies a pointer to the next valid, free and available virtual address.
 *
 * @param ImageProtocol
 *        A pointer to the EFI loaded image protocol with information about where in memory the loader code was placed.
 *
 * @param PtePointer
 *        Supplies a pointer to memory area containing a Page Table Entries (PTE).
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
XtEnablePaging(IN PXTBL_PAGE_MAPPING PageMap,
               IN PVOID VirtualAddress,
               IN PEFI_LOADED_IMAGE_PROTOCOL ImageProtocol)
{
    PLOADER_MEMORY_MAPPING Mapping;
    EFI_PHYSICAL_ADDRESS Address;
    PLIST_ENTRY ListEntry, ModulesList, ModulesListEntry;
    PXTBL_MODULE_INFO ModuleInfo;
    EFI_STATUS Status;

    /* Allocate pages for PML4 */
    Status = XtLdrProtocol->Memory.AllocatePages(1, &Address);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure */
        return Status;
    }

    /* Assign and zero-fill memory used by page mappings */
    PageMap->PtePointer = (PVOID)(UINT_PTR)Address;
    RtlZeroMemory(PageMap->PtePointer, EFI_PAGE_SIZE);

    /* Get list of XTLDR modules */
    ModulesList = XtLdrProtocol->Protocol.GetModulesList();
    ModulesListEntry = ModulesList->Flink;
    while(ModulesListEntry != ModulesList)
    {
        /* Get module info */
        ModuleInfo = CONTAIN_RECORD(ModulesListEntry, XTBL_MODULE_INFO, Flink);

        /* Map module code */
        Status = XtLdrProtocol->Memory.MapVirtualMemory(PageMap, ModuleInfo->ModuleBase, ModuleInfo->ModuleBase,
                                  EFI_SIZE_TO_PAGES(ModuleInfo->ModuleSize), LoaderFirmwareTemporary);

        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Mapping module code failed */
            return Status;
        }

        /* Get next module */
        ModulesListEntry = ModulesListEntry->Flink;
    }

    /* Map XTLDR code */
    XtLdrProtocol->Memory.MapVirtualMemory(PageMap, ImageProtocol->ImageBase, ImageProtocol->ImageBase,
                                       EFI_SIZE_TO_PAGES(ImageProtocol->ImageSize), LoaderFirmwareTemporary);

    /* Add page mapping itself to memory mapping */
    Status = XtLdrProtocol->Memory.MapVirtualMemory(PageMap, NULL, PageMap->PtePointer, 1, LoaderMemoryData);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Mapping PML4 failed */
        return Status;
    }

    /* Iterate through and map all the mappings*/
    XtLdrProtocol->Debug.Print(L"Mapping and dumping EFI memory:\n");
    ListEntry = PageMap->MemoryMap.Flink;
    while(ListEntry != &PageMap->MemoryMap)
    {
        /* Take mapping from the list */
        Mapping = CONTAIN_RECORD(ListEntry, LOADER_MEMORY_MAPPING, ListEntry);

        /* Check if virtual address is set */
        if(Mapping->VirtualAddress)
        {
            /* Dump memory mapping */
            XtLdrProtocol->Debug.Print(L"   Type=%02lu, PhysicalBase=0x%016lx, VirtualBase=0x%016lx, Pages=%lu\n", Mapping->MemoryType,
                       Mapping->PhysicalAddress, Mapping->VirtualAddress, Mapping->NumberOfPages);

            /* Map memory */
            Status = XtLdrProtocol->Memory.MapPage(PageMap, (UINT_PTR)Mapping->VirtualAddress,
                                        (UINT_PTR)Mapping->PhysicalAddress, Mapping->NumberOfPages);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Memory mapping failed */
                return Status;
            }
        }

        /* Take next element */
        ListEntry = ListEntry->Flink;
    }

    /* Map zero page as well */
    XtLdrProtocol->Memory.MapPage(PageMap, 0, 0, 1);

    /* Exit EFI Boot Services */
    XtLdrProtocol->Debug.Print(L"Exiting EFI boot services\n");
    Status = XtLdrProtocol->Util.ExitBootServices();
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to exit boot services */
        XtLdrProtocol->Console.Print(L"Failed to exit boot services (Status code: %lx)\n", Status);
        return STATUS_EFI_ABORTED;
    }

    /* Write PML4 to CR3 */
    ArWriteControlRegister(3, (UINT_PTR)PageMap->PtePointer);

    /* Return success */
    return STATUS_EFI_SUCCESS;
}
