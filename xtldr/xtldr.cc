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

    /* Initialize XTLDR protocol */
    Protocol::InitializeProtocol();

    /* Initialize XTLDR configuration */
    Configuration::InitializeConfiguration();

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
