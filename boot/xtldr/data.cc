/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/data.cc
 * DESCRIPTION:     XT Boot Loader global and static data
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#include <xtldr.hh>


/* Legacy BIOS cursor X position */
USHORT BiosUtils::CursorX = 0;

/* Legacy BIOS cursor Y position */
USHORT BiosUtils::CursorY = 0;

/* Legacy BIOS screen height */
CONST USHORT BiosUtils::VgaHeight = 25;

/* Legacy BIOS screen width */
CONST USHORT BiosUtils::VgaWidth = 80;

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

/* A list of enabled debug ports */
ULONG Debug::EnabledDebugPorts;

/* XT Boot Loader serial port handle */
CPPORT Debug::SerialPort;

/* XT Boot Loader registered boot protocol list */
LIST_ENTRY Protocol::BootProtocols;

/* XT Boot Loader protocol */
XTBL_LOADER_PROTOCOL Protocol::LoaderProtocol;

/* XT Boot Loader loaded modules list */
LIST_ENTRY Protocol::LoadedModules;

/* List of available block devices */
LIST_ENTRY Volume::EfiBlockDevices;

/* Pointer to the boot menu callback routine */
PBL_XT_BOOT_MENU XtLoader::BootMenu = NULLPTR;

/* EFI Image Handle */
EFI_HANDLE XtLoader::EfiImageHandle;

/* EFI System Table */
PEFI_SYSTEM_TABLE XtLoader::EfiSystemTable;

/* XT Boot Loader status data */
XTBL_STATUS XtLoader::LoaderStatus = {0};
