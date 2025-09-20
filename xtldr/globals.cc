/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/globals.cc
 * DESCRIPTION:     XT Boot Loader global variables
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtldr.hh>


/* XT Boot Loader menu list */
PLIST_ENTRY Configuration::BootMenuList = NULLPTR;

/* XT Boot Loader configuration list */
LIST_ENTRY Configuration::Config;

/* XT Boot Loader loaded configuration */
LIST_ENTRY Configuration::ConfigSections;

/* List of user-editable boot options */
PCWSTR Configuration::EditableConfigOptions[] = {
    L"BootModules", L"SystemType", L"SystemPath",
    L"KernelFile",  L"InitrdFile", L"HalFile",
    L"Parameters", NULLPTR
};

/* XT Boot Loader serial ports list */
ULONG Debug::ComPortList[COMPORT_COUNT] = COMPORT_ADDRESS;

/* XT Boot Loader registered boot protocol list */
LIST_ENTRY Protocol::BootProtocols;

/* XT Boot Loader protocol */
XTBL_LOADER_PROTOCOL Protocol::LoaderProtocol;

/* XT Boot Loader loaded modules list */
LIST_ENTRY Protocol::LoadedModules;

/* List of available block devices */
LIST_ENTRY Volume::EfiBlockDevices;



/* XT Boot Loader status data */
XTBL_STATUS BlpStatus = {0};

/* EFI Image Handle */
EFI_HANDLE EfiImageHandle;

/* EFI System Table */
PEFI_SYSTEM_TABLE EfiSystemTable;
