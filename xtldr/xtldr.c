/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/xtldr.c
 * DESCRIPTION:     XTOS UEFI Boot Loader
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtldr.h>


/**
 * Initializes EFI Boot Loader (XTLDR).
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlInitializeBootLoader()
{
    EFI_GUID LipGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    PEFI_LOADED_IMAGE_PROTOCOL LoadedImage;
    EFI_HANDLE Handle;
    EFI_STATUS Status;

    /* Set current XTLDR's EFI BootServices status */
    BlpStatus.BootServices = TRUE;

    /* Initialize console */
    BlInitializeConsole();

    /* Print XTLDR version */
    BlConsolePrint(L"XTLDR boot loader v%s\n", XTOS_VERSION);

    /* Initialize XTLDR configuration linked lists */
    RtlInitializeListHead(&BlpBootProtocols);
    RtlInitializeListHead(&BlpConfig);
    RtlInitializeListHead(&BlpLoadedModules);

    /* Store SecureBoot status */
    BlpStatus.SecureBoot = BlGetSecureBootStatus();

    /* Attempt to open EFI LoadedImage protocol */
    Status = BlOpenProtocol(&Handle, (PVOID *)&LoadedImage, &LipGuid);
    if(Status == STATUS_EFI_SUCCESS)
    {
        /* Store boot loader image base and size */
        BlpStatus.LoaderBase = LoadedImage->ImageBase;
        BlpStatus.LoaderSize = LoadedImage->ImageSize;

        /* Check if debug is enabled */
        if(DEBUG)
        {
            /* Protocol opened successfully, print useful debug information */
            BlConsolePrint(L"\n---------- BOOTLOADER DEBUG ----------\n"
                           L"Pointer Size      : %d\n"
                           L"Image Base Address: %P\n"
                           L"Image Base Size   : 0x%lX\n"
                           L"Image Revision    : 0x%lX\n"
                           L"--------------------------------------\n",
                           sizeof(PVOID),
                           LoadedImage->ImageBase,
                           LoadedImage->ImageSize,
                           LoadedImage->Revision);
            BlSleepExecution(3000);
        }

        /* Close EFI LoadedImage protocol */
        BlCloseProtocol(&Handle, &LipGuid);
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
BlInitializeBootMenuList(IN ULONG MaxNameLength,
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
    BlGetConfigValue(L"DEFAULT", &DefaultMenuEntry);

    /* Check if configuration allows to use last booted OS */
    if(BlGetConfigBooleanValue(L"KEEPLASTBOOT"))
    {
        /* Attempt to get last booted Operating System from NVRAM */
        Status = BlGetEfiVariable(&VendorGuid, L"XtLdrLastBootOS", (PVOID*)&LastBooted);
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
    Status = BlAllocateMemoryPool(NumberOfEntries * sizeof(XTBL_BOOTMENU_ITEM), (PVOID*)&OsList);
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
        if((RtlWideStringLength(MenuEntrySection->SectionName, 0) == RtlWideStringLength(DefaultMenuEntry, 0)) &&
           (RtlCompareWideStringInsensitive(MenuEntrySection->SectionName, DefaultMenuEntry, 0) == 0))
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
            if(RtlCompareWideStringInsensitive(MenuEntryOption->Name, L"SYSTEMNAME", 0) == 0)
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
        NameLength = RtlWideStringLength(MenuEntryName, 0);
        if(NameLength > MaxNameLength)
        {
            /* Menu entry name is too long, allocate memory for shorter name visible in the boot menu */
            Status = BlAllocateMemoryPool((MaxNameLength + 1) * sizeof(WCHAR), (PVOID*)&VisibleName);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Memory allocation failure */
                return STATUS_EFI_OUT_OF_RESOURCES;
            }

            /* Copy shorter name and append "..." at the end */
            RtlCopyMemory(VisibleName, MenuEntryName, (MaxNameLength - 3) * sizeof(WCHAR));
            RtlCopyMemory(VisibleName + MaxNameLength - 3, L"...", 3 * sizeof(WCHAR));
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
 * Loads all necessary modules and invokes boot protocol.
 *
 * @param ShortName
 *        Supplies a pointer to a short name of the chosen boot menu entry.
 *
 * @param OptionsList
 *        Supplies a pointer to list of options associated with chosen boot menu entry.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlInvokeBootProtocol(IN PWCHAR ShortName,
                     IN PLIST_ENTRY OptionsList)
{
    EFI_GUID VendorGuid = XT_BOOT_LOADER_PROTOCOL_GUID;
    XTBL_BOOT_PARAMETERS BootParameters;
    PXTBL_BOOT_PROTOCOL BootProtocol;
    PLIST_ENTRY OptionsListEntry;
    PXTBL_CONFIG_ENTRY Option;
    EFI_GUID BootProtocolGuid;
    SIZE_T ModuleListLength;
    PWCHAR ModulesList;
    EFI_HANDLE Handle;
    EFI_STATUS Status;

    /* Initialize boot parameters and a list of modules */
    RtlZeroMemory(&BootParameters, sizeof(XTBL_BOOT_PARAMETERS));
    ModulesList = NULLPTR;

    /* Iterate through all options provided by boot menu entry and propagate boot parameters */
    OptionsListEntry = OptionsList->Flink;
    while(OptionsListEntry != OptionsList)
    {
        /* Get option */
        Option = CONTAIN_RECORD(OptionsListEntry, XTBL_CONFIG_ENTRY, Flink);

        /* Look for boot protocol and modules list */
        if(RtlCompareWideStringInsensitive(Option->Name, L"BOOTMODULES", 0) == 0)
        {
            /* Check a length of modules list */
            ModuleListLength = RtlWideStringLength(Option->Value, 0);

            Status = BlAllocateMemoryPool(sizeof(WCHAR) * (ModuleListLength + 1), (PVOID *)&ModulesList);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Failed to allocate memory, print error message and return status code */
                BlDebugPrint(L"ERROR: Memory allocation failure (Status Code: 0x%zX)\n", Status);
                return STATUS_EFI_OUT_OF_RESOURCES;
            }

            /* Make a copy of modules list */
            RtlCopyMemory(ModulesList, Option->Value, sizeof(WCHAR) * (ModuleListLength + 1));
        }
        else if(RtlCompareWideStringInsensitive(Option->Name, L"SYSTEMTYPE", 0) == 0)
        {
            /* Boot protocol found */
            BootParameters.SystemType = Option->Value;
        }
        else if(RtlCompareWideStringInsensitive(Option->Name, L"SYSTEMPATH", 0) == 0)
        {
            /* System path found, get volume device path */
            Status = BlGetVolumeDevicePath(Option->Value, &BootParameters.DevicePath,
                                           &BootParameters.ArcName, &BootParameters.SystemPath);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Failed to find volume */
                BlDebugPrint(L"ERROR: Failed to find volume device path (Status Code: 0x%zX)\n", Status);
                return Status;
            }

            /* Get EFI compatible system path */
            Status = BlGetEfiPath(BootParameters.SystemPath, &BootParameters.EfiPath);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Failed to get EFI path */
                BlDebugPrint(L"ERROR: Failed to get EFI path (Status Code: 0x%zX)\n", Status);
                return Status;
            }
        }
        else if(RtlCompareWideStringInsensitive(Option->Name, L"KERNELFILE", 0) == 0)
        {
            /* Kernel file name found */
            BootParameters.KernelFile = Option->Value;
        }
        else if(RtlCompareWideStringInsensitive(Option->Name, L"INITRDFILE", 0) == 0)
        {
            /* Initrd file name found */
            BootParameters.InitrdFile = Option->Value;
        }
        else if(RtlCompareWideStringInsensitive(Option->Name, L"HALFILE", 0) == 0)
        {
            /* Hal file name found */
            BootParameters.HalFile = Option->Value;
        }
        else if(RtlCompareWideStringInsensitive(Option->Name, L"PARAMETERS", 0) == 0)
        {
            /* Kernel parameters found */
            BootParameters.Parameters = Option->Value;
        }

        /* Move to the next option entry */
        OptionsListEntry = OptionsListEntry->Flink;
    }

    /* Load all necessary modules */
    Status = BlLoadModules(ModulesList);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to load modules, print error message and return status code */
        BlDebugPrint(L"ERROR: Failed to load XTLDR modules (Status Code: 0x%zX)\n", Status);
        return STATUS_EFI_NOT_READY;
    }

    /* Attempt to get boot protocol GUID */
    Status = BlFindBootProtocol(BootParameters.SystemType, &BootProtocolGuid);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to get boot protocol GUID */
        BlDebugPrint(L"ERROR: Unable to find appropriate boot protocol (Status Code: 0x%zX)\n", Status);
        return STATUS_EFI_UNSUPPORTED;
    }

    /* Open boot protocol */
    Status = BlOpenProtocol(&Handle, (PVOID *)&BootProtocol, &BootProtocolGuid);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open boot protocol */
        BlDebugPrint(L"ERROR: Failed to open boot protocol (Status Code: 0x%zX)\n", Status);
        return Status;
    }

    /* Check if chosen operating system should be saved */
    if(BlGetConfigBooleanValue(L"KEEPLASTBOOT"))
    {
        /* Save chosen operating system in NVRAM */
        Status = BlSetEfiVariable(&VendorGuid, L"XtLdrLastBootOS", (PVOID)ShortName, RtlWideStringLength(ShortName, 0) * sizeof(WCHAR));
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Failed to save chosen Operating System */
            BlDebugPrint(L"WARNING: Failed to save chosen Operating System in NVRAM (Status Code: 0x%zX)\n", Status);
        }
    }

    /* Boot Operating System */
    return BootProtocol->BootSystem(&BootParameters);
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
    BlInitializeBootLoader();

    /* Parse configuration options passed from UEFI shell */
    Status = BlpParseCommandLine();
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to parse command line options */
        BlDisplayErrorDialog(L"XTLDR", L"Failed to parse command line parameters.");
    }

    /* Attempt to early initialize debug console */
    if(DEBUG)
    {
        Status = BlpInitializeDebugConsole();
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Initialization failed, notify user on stdout */
            BlDisplayErrorDialog(L"XTLDR", L"Failed to initialize debug console.");
        }
    }

    /* Load XTLDR configuration file */
    Status = BlpLoadConfiguration();
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to load/parse config file */
        BlDisplayErrorDialog(L"XTLDR", L"Failed to load and parse configuration file ");
    }

    /* Reinitialize debug console if it was not initialized earlier */
    if(DEBUG)
    {
        Status = BlpInitializeDebugConsole();
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Initialization failed, notify user on stdout */
            BlDisplayErrorDialog(L"XTLDR", L"Failed to initialize debug console.");
        }
    }

    /* Disable watchdog timer */
    Status = EfiSystemTable->BootServices->SetWatchdogTimer(0, 0x10000, 0, NULLPTR);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to disable the timer, print message */
        BlDebugPrint(L"WARNING: Failed to disable watchdog timer (Status Code: 0x%zX)\n", Status);
    }

    /* Install loader protocol */
    Status = BlpInstallXtLoaderProtocol();
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to register loader protocol */
        BlDebugPrint(L"ERROR: Failed to register XTLDR loader protocol (Status Code: 0x%zX)\n", Status);
        return Status;
    }

    /* Load all necessary modules */
    BlGetConfigValue(L"MODULES", &Modules);
    Status = BlLoadModules(Modules);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to load modules */
        BlDebugPrint(L"ERROR: Failed to load XTLDR modules (Status Code: 0x%zX)\n", Status);
        BlDisplayErrorDialog(L"XTLDR", L"Failed to load some XTLDR modules.");
    }

    /* Discover and enumerate EFI block devices */
    Status = BlEnumerateBlockDevices();
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to enumerate block devices */
        BlDebugPrint(L"ERROR: Failed to discover and enumerate block devices (Status Code: 0x%zX)\n", Status);
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
            BlDisplayBootMenu();
        }

        /* Fallback to shell, if boot menu returned */
        BlStartLoaderShell();
    }

    /* This point should be never reached, if this happen return error code */
    return STATUS_EFI_LOAD_ERROR;
}
