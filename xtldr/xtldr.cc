/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/xtldr.cc
 * DESCRIPTION:     XTOS UEFI Boot Loader
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtldr.hh>


/**
 * Initializes EFI Boot Loader (XTLDR).
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
XtLoader::InitializeBootLoader()
{
    EFI_GUID LipGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    PEFI_LOADED_IMAGE_PROTOCOL LoadedImage;
    EFI_HANDLE Handle;
    EFI_STATUS Status;

    /* Set current XTLDR's EFI BootServices status */
    BlpStatus.BootServices = TRUE;

    /* Initialize console */
    Console::InitializeConsole();

    /* Print XTLDR version */
    Console::Print(L"XTLDR boot loader v%s\n", XTOS_VERSION);

    /* Initialize XTLDR configuration linked lists */
    RTL::LinkedList::InitializeListHead(&BlpBootProtocols);
    RTL::LinkedList::InitializeListHead(&BlpConfig);
    RTL::LinkedList::InitializeListHead(&BlpLoadedModules);

    /* Store SecureBoot status */
    BlpStatus.SecureBoot = EfiUtils::GetSecureBootStatus();

    /* Attempt to open EFI LoadedImage protocol */
    Status = Protocol::OpenProtocol(&Handle, (PVOID *)&LoadedImage, &LipGuid);
    if(Status == STATUS_EFI_SUCCESS)
    {
        /* Store boot loader image base and size */
        BlpStatus.LoaderBase = LoadedImage->ImageBase;
        BlpStatus.LoaderSize = LoadedImage->ImageSize;

        /* Check if debug is enabled */
        if(DEBUG)
        {
            /* Protocol opened successfully, print useful debug information */
            Console::Print(L"\n---------- BOOTLOADER DEBUG ----------\n"
                           L"Pointer Size       : %d\n"
                           L"Image Base Address : %P\n"
                           L"Image Base Size    : 0x%lX\n"
                           L"Image Revision     : 0x%lX\n"
                           L"Secure Boot Status : %zd\n"
                           L"--------------------------------------\n",
                           sizeof(PVOID),
                           LoadedImage->ImageBase,
                           LoadedImage->ImageSize,
                           LoadedImage->Revision,
                           BlpStatus.SecureBoot);
            EfiUtils::SleepExecution(3000);
        }

        /* Close EFI LoadedImage protocol */
        Protocol::CloseProtocol(&Handle, &LipGuid);
    }
}

/**
 * Initializes a list of operating systems for XTLDR boot menu.
 *
 * @param MenuEntries
 *        Supplies a pointer to memory area where operating systems list will be stored.
 *
 * @param EntriesCount
 *        Supplies a pointer to memory area where number of menu entries will be stored.
 *
 * @param DefaultId
 *        Supplies a pointer to memory area where ID of default menu entry will be stored.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
XtLoader::InitializeBootMenuList(IN ULONG MaxNameLength,
                                 OUT PXTBL_BOOTMENU_ITEM *MenuEntries,
                                 OUT PULONG EntriesCount,
                                 OUT PULONG DefaultId)
{
    EFI_GUID VendorGuid = XT_BOOT_LOADER_PROTOCOL_GUID;
    PWCHAR DefaultMenuEntry, LastBooted, MenuEntryName, VisibleName;
    PLIST_ENTRY MenuEntrySectionList, MenuEntryList;
    PXTBL_CONFIG_SECTION MenuEntrySection;
    PXTBL_CONFIG_ENTRY MenuEntryOption;
    ULONG DefaultOS, NameLength,NumberOfEntries;
    PXTBL_BOOTMENU_ITEM OsList;
    EFI_STATUS Status;

    /* Set default values */
    DefaultOS = 0;
    NumberOfEntries = 0;

    /* Get default menu entry from configuration */
    Configuration::GetValue(L"DEFAULT", &DefaultMenuEntry);

    /* Check if configuration allows to use last booted OS */
    if(Configuration::GetBooleanValue(L"KEEPLASTBOOT"))
    {
        /* Attempt to get last booted Operating System from NVRAM */
        Status = EfiUtils::GetEfiVariable(&VendorGuid, L"XtLdrLastBootOS", (PVOID*)&LastBooted);
        if(Status == STATUS_EFI_SUCCESS)
        {
            /* Set default menu entry to last booted OS */
            DefaultMenuEntry = LastBooted;
        }
    }

    /* Iterate through menu items to get a total number of entries */
    MenuEntrySectionList = BlpMenuList->Flink;
    while(MenuEntrySectionList != BlpMenuList)
    {
        /* Increase number of menu entries, and simply get next item */
        NumberOfEntries++;
        MenuEntrySectionList = MenuEntrySectionList->Flink;
    }

    /* Allocate memory for the OS list depending on the item count */
    Status = Memory::AllocatePool(NumberOfEntries * sizeof(XTBL_BOOTMENU_ITEM), (PVOID*)&OsList);
    if(Status != STATUS_EFI_SUCCESS || !OsList)
    {
        /* Memory allocation failure */
        return STATUS_EFI_OUT_OF_RESOURCES;
    }

    /* Reset counter and iterate through all menu items once again */
    NumberOfEntries = 0;
    MenuEntrySectionList = BlpMenuList->Flink;
    while(MenuEntrySectionList != BlpMenuList)
    {
        /* NULLify menu entry name */
        MenuEntryName = NULLPTR;

        /* Get menu section */
        MenuEntrySection = CONTAIN_RECORD(MenuEntrySectionList, XTBL_CONFIG_SECTION, Flink);

        /* Check if this is the default menu entry */
        if((RTL::WideString::WideStringLength(MenuEntrySection->SectionName, 0) == RTL::WideString::WideStringLength(DefaultMenuEntry, 0)) &&
           (RTL::WideString::CompareWideStringInsensitive(MenuEntrySection->SectionName, DefaultMenuEntry, 0) == 0))
        {
            /* Set default OS ID */
            DefaultOS = NumberOfEntries;
        }

        /* Iterate through all entry parameters */
        MenuEntryList = MenuEntrySection->Options.Flink;
        while(MenuEntryList != &MenuEntrySection->Options)
        {
            /* Get menu entry parameter */
            MenuEntryOption = CONTAIN_RECORD(MenuEntryList, XTBL_CONFIG_ENTRY, Flink);

            /* Check if this is the menu entry display name */
            if(RTL::WideString::CompareWideStringInsensitive(MenuEntryOption->Name, L"SYSTEMNAME", 0) == 0)
            {
                /* Set menu entry display name */
                MenuEntryName = MenuEntryOption->Value;
            }

            /* Get next parameter for this menu entry */
            MenuEntryList = MenuEntryList->Flink;
        }

        /* Add OS to the boot menu list */
        OsList[NumberOfEntries].FullName = MenuEntryName;
        OsList[NumberOfEntries].ShortName = MenuEntrySection->SectionName;
        OsList[NumberOfEntries].Options = &MenuEntrySection->Options;

        /* Check if the menu entry name fits the maximum length */
        NameLength = RTL::WideString::WideStringLength(MenuEntryName, 0);
        if(NameLength > MaxNameLength)
        {
            /* Menu entry name is too long, allocate memory for shorter name visible in the boot menu */
            Status = Memory::AllocatePool((MaxNameLength + 1) * sizeof(WCHAR), (PVOID*)&VisibleName);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Memory allocation failure */
                return STATUS_EFI_OUT_OF_RESOURCES;
            }

            /* Copy shorter name and append "..." at the end */
            RTL::Memory::CopyMemory(VisibleName, MenuEntryName, (MaxNameLength - 3) * sizeof(WCHAR));
            RTL::Memory::CopyMemory(VisibleName + MaxNameLength - 3, L"...", 3 * sizeof(WCHAR));
            VisibleName[MaxNameLength] = L'\0';

            /* Set visible menu entry name */
            OsList[NumberOfEntries].EntryName = VisibleName;
        }
        else
        {
            /* Menu entry name fits the maximum length, use it as is */
            OsList[NumberOfEntries].EntryName = MenuEntryName;
        }

        /* Get next menu entry */
        MenuEntrySectionList = MenuEntrySectionList->Flink;
        NumberOfEntries++;
    }

    /* Set return values */
    *DefaultId = DefaultOS;
    *EntriesCount = NumberOfEntries;
    *MenuEntries = OsList;

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * This routine is the entry point of the XT EFI boot loader.
 *
 * @param ImageHandle
 *        Firmware-allocated handle that identifies the image.
 *
 * @param SystemTable
 *        Provides the EFI system table.
 *
 * @return This routine returns status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlStartXtLoader(IN EFI_HANDLE ImageHandle,
                IN PEFI_SYSTEM_TABLE SystemTable)
{
    PWCHAR Modules;
    EFI_STATUS Status;

    /* Set the system table and image handle */
    EfiImageHandle = ImageHandle;
    EfiSystemTable = SystemTable;

    /* Initialize XTLDR and */
    XtLoader::InitializeBootLoader();

    /* Parse configuration options passed from UEFI shell */
    Status = Configuration::ParseCommandLine();
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to parse command line options */
        TextUi::DisplayErrorDialog(L"XTLDR", L"Failed to parse command line parameters.");
    }

    /* Attempt to early initialize debug console */
    if(DEBUG)
    {
        Status = Debug::InitializeDebugConsole();
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Initialization failed, notify user on stdout */
            TextUi::DisplayErrorDialog(L"XTLDR", L"Failed to initialize debug console.");
        }
    }

    /* Load XTLDR configuration file */
    Status = Configuration::LoadConfiguration();
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to load/parse config file */
        TextUi::DisplayErrorDialog(L"XTLDR", L"Failed to load and parse configuration file ");
    }

    /* Reinitialize debug console if it was not initialized earlier */
    if(DEBUG)
    {
        Status = Debug::InitializeDebugConsole();
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Initialization failed, notify user on stdout */
            TextUi::DisplayErrorDialog(L"XTLDR", L"Failed to initialize debug console.");
        }
    }

    /* Disable watchdog timer */
    Status = EfiSystemTable->BootServices->SetWatchdogTimer(0, 0x10000, 0, NULLPTR);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to disable the timer, print message */
        Debug::Print(L"WARNING: Failed to disable watchdog timer (Status Code: 0x%zX)\n", Status);
    }

    /* Install loader protocol */
    Status = Protocol::InstallXtLoaderProtocol();
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to register loader protocol */
        Debug::Print(L"ERROR: Failed to register XTLDR loader protocol (Status Code: 0x%zX)\n", Status);
        return Status;
    }

    /* Load all necessary modules */
    Configuration::GetValue(L"MODULES", &Modules);
    Status = Protocol::LoadModules(Modules);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to load modules */
        Debug::Print(L"ERROR: Failed to load XTLDR modules (Status Code: 0x%zX)\n", Status);
        TextUi::DisplayErrorDialog(L"XTLDR", L"Failed to load some XTLDR modules.");
    }

    /* Discover and enumerate EFI block devices */
    Status = Volume::EnumerateBlockDevices();
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to enumerate block devices */
        Debug::Print(L"ERROR: Failed to discover and enumerate block devices (Status Code: 0x%zX)\n", Status);
        return Status;
    }

    /* Main boot loader loop */
    while(TRUE)
    {
        /* Check if custom boot menu registered */
        if(BlpStatus.BootMenu != NULLPTR)
        {
            /* Display alternative boot menu */
            BlpStatus.BootMenu();
        }
        else
        {
            /* Display default boot menu */
            TextUi::DisplayBootMenu();
        }

        /* Fallback to shell, if boot menu returned */
        Shell::StartLoaderShell();
    }

    /* This point should be never reached, if this happen return error code */
    return STATUS_EFI_LOAD_ERROR;
}
