/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/xtldr.cc
 * DESCRIPTION:     XTOS UEFI Boot Loader
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtldr.hh>


/**
 * Disables access to EFI Boot Services.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
XtLoader::DisableBootServices()
{
    LoaderStatus.BootServices = FALSE;

}

/**
 * Queries the availability of EFI Boot Services.
 *
 * @return This routine returns TRUE if EFI Boot Services are available, FALSE otherwise.
 *
 * @since XT 1.0
 */
XTCDECL
BOOLEAN
XtLoader::GetBootServicesStatus()
{
    return LoaderStatus.BootServices;
}

/**
 * Retrieves the EFI image handle.
 *
 * @return This routine returns a handle to the EFI-loaded image.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_HANDLE
XtLoader::GetEfiImageHandle()
{
    return XtLoader::EfiImageHandle;
}

/**
 * Retrieves the EFI system table pointer.
 *
 * @return This routine returns a pointer to the EFI system table.
 *
 * @since XT 1.0
 */
XTCDECL
PEFI_SYSTEM_TABLE
XtLoader::GetEfiSystemTable()
{
    return XtLoader::EfiSystemTable;
}

/**
 * Provides base address and size of the XTLDR image.
 *
 * @param LoaderBase
 *        Supplies a pointer to a variable that receives the base address of the XTLDR image.
 *
 * @param LoaderSize
 *        Supplies a pointer to a variable that receives the size of the XTLDR image.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
XtLoader::GetLoaderImageInformation(PVOID *LoaderBase,
                                    PULONGLONG LoaderSize)
{
    *LoaderBase = XtLoader::LoaderStatus.LoaderBase;
    *LoaderSize = XtLoader::LoaderStatus.LoaderSize;
}

/**
 * Retrieves the Secure Boot status.
 *
 * @return This routine returns SecureBoot status.
 *
 * @since XT 1.0
 */
XTCDECL
INT_PTR
XtLoader::GetSecureBootStatus()
{
    return LoaderStatus.SecureBoot;
}

/**
 * Initializes EFI Boot Loader (XTLDR).
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
XtLoader::InitializeBootLoader(IN EFI_HANDLE ImageHandle,
                               IN PEFI_SYSTEM_TABLE SystemTable)
{
    EFI_GUID LipGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    PEFI_LOADED_IMAGE_PROTOCOL LoadedImage;
    EFI_HANDLE Handle;
    EFI_STATUS Status;

    /* Set the system table and image handle */
    EfiImageHandle = ImageHandle;
    EfiSystemTable = SystemTable;

    /* Set current XTLDR's EFI BootServices status */
    LoaderStatus.BootServices = TRUE;

    /* Initialize console */
    Console::InitializeConsole();

    /* Print XTLDR version */
    Console::Print(L"XTLDR boot loader v%s\n", XTOS_VERSION);

    /* Initialize XTLDR protocol */
    Protocol::InitializeProtocol();

    /* Initialize XTLDR configuration */
    Configuration::InitializeConfiguration();

    /* Store SecureBoot status */
    LoaderStatus.SecureBoot = EfiUtils::GetSecureBootStatus();

    /* Attempt to open EFI LoadedImage protocol */
    Status = Protocol::OpenProtocol(&Handle, (PVOID *)&LoadedImage, &LipGuid);
    if(Status == STATUS_EFI_SUCCESS)
    {
        /* Store boot loader image base and size */
        LoaderStatus.LoaderBase = LoadedImage->ImageBase;
        LoaderStatus.LoaderSize = LoadedImage->ImageSize;

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
                           LoaderStatus.SecureBoot);
            EfiUtils::SleepExecution(3000);
        }

        /* Close EFI LoadedImage protocol */
        Protocol::CloseProtocol(&Handle, &LipGuid);
    }
}

/**
 * Registers a boot menu callback routine, that will be used to display alternative boot menu.
 *
 * @param BootMenuRoutine
 *        Supplies a pointer to the boot menu callback routine.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
XtLoader::RegisterBootMenu(IN PVOID BootMenuRoutine)
{
    /* Set boot menu routine */
    BootMenu = (PBL_XT_BOOT_MENU)BootMenuRoutine;
}

/**
 * Invokes either a custom boot menu handler, if one has been registered, or displays the default boot menu.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
XtLoader::ShowBootMenu()
{
    /* Check if custom boot menu registered */
    if(BootMenu != NULLPTR)
    {
        /* Display alternative boot menu */
        BootMenu();
    }
    else
    {
        /* Display default boot menu */
        TextUi::DisplayBootMenu();
    }
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

    /* Check if system is EFI-based and provided parameters are valid */
    if(ImageHandle == NULLPTR || SystemTable == NULLPTR)
    {
        /* Invalid parameters, print error message using BIOS calls and hang */
        BiosUtils::ClearScreen();
        BiosUtils::Print(L"XTLDR requires EFI-based system!");
        for(;;);
    }

    /* Initialize XTLDR and */
    XtLoader::InitializeBootLoader(ImageHandle, SystemTable);

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
    Status = XtLoader::GetEfiSystemTable()->BootServices->SetWatchdogTimer(0, 0x10000, 0, NULLPTR);
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
        /* Show boot menu */
        XtLoader::ShowBootMenu();

        /* Fallback to shell, if boot menu returned */
        Shell::StartLoaderShell();
    }

    /* This point should be never reached, if this happen return error code */
    return STATUS_EFI_LOAD_ERROR;
}
