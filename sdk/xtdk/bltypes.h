/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/bltypes.h
 * DESCRIPTION:     XT Boot Manager structures definitions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_BLTYPES_H
#define __XTDK_BLTYPES_H

#include <xttypes.h>
#include <xtuefi.h>
#include <hltypes.h>


/* Architecture specific definitions */
#if defined(__i386__) || defined(__i686__)
    #define XTBL_ARCH_LOADER_DIRECTORY_PATH                         L"\\EFI\\BOOT\\XTLDR32\\"
    #define XTBL_ARCH_MODULES_DIRECTORY_PATH                        L"\\EFI\\BOOT\\XTLDR32\\MODULES\\"
    #define XTBL_ARCH_THEMES_DIRECTORY_PATH                         L"\\EFI\\BOOT\\XTLDR32\\THEMES\\"
#elif defined(__amd64__) || defined(__x86_64__)
    #define XTBL_ARCH_LOADER_DIRECTORY_PATH                         L"\\EFI\\BOOT\\XTLDR64\\"
    #define XTBL_ARCH_MODULES_DIRECTORY_PATH                        L"\\EFI\\BOOT\\XTLDR64\\MODULES\\"
    #define XTBL_ARCH_THEMES_DIRECTORY_PATH                         L"\\EFI\\BOOT\\XTLDR64\\THEMES\\"
#else
    #error Unknown architecture
#endif

/* XTLDR directories */
#define XTBL_LOADER_DIRECTORY_PATH                                  L"\\EFI\\BOOT\\XTLDR\\"
#define XTBL_MODULES_DIRECTORY_PATH                                 L"\\EFI\\BOOT\\XTLDR\\MODULES\\"
#define XTBL_THEMES_DIRECTORY_PATH                                  L"\\EFI\\BOOT\\XTLDR\\THEMES\\"

/* XTLDR module segment macros */
#define XTBL_MODDEPS                                                SEGMENT(".moddeps") CONST WCHAR XtBlpDeps[][8]
#define XTBL_MODINFO                                                SEGMENT(".modinfo") CONST WCHAR XtBlpInfo[]

/* EFI XT boot devices */
#define XTBL_BOOT_DEVICE_UNKNOWN                                    0x00
#define XTBL_BOOT_DEVICE_CDROM                                      0x01
#define XTBL_BOOT_DEVICE_FLOPPY                                     0x02
#define XTBL_BOOT_DEVICE_HARDDISK                                   0x03
#define XTBL_BOOT_DEVICE_RAMDISK                                    0x04

/* XTLDR Debug Port type definitions */
#define XTBL_DEBUGPORT_SCREEN                                       1
#define XTBL_DEBUGPORT_SERIAL                                       2

/* TUI dialog box attributes */
#define XTBL_TUI_DIALOG_GENERIC_BOX                                 1
#define XTBL_TUI_DIALOG_ERROR_BOX                                   2
#define XTBL_TUI_DIALOG_ACTIVE_BUTTON                               4
#define XTBL_TUI_DIALOG_INACTIVE_BUTTON                             8
#define XTBL_TUI_DIALOG_ACTIVE_INPUT                                16
#define XTBL_TUI_DIALOG_INACTIVE_INPUT                              32
#define XTBL_TUI_DIALOG_PROGRESS_BAR                                64

/* TUI dialog box maximum width */
#define XTBL_TUI_MAX_DIALOG_WIDTH                                   100

/* Boot Loader protocol routine pointers */
typedef EFI_STATUS (*PBL_ALLOCATE_PAGES)(IN UINT64 Size, OUT PEFI_PHYSICAL_ADDRESS Memory);
typedef EFI_STATUS (*PBL_ALLOCATE_POOL)(IN UINT_PTR Size, OUT PVOID *Memory);
typedef VOID (*PBL_BOOTMENU_INITIALIZE_OS_LIST)(OUT PXTBL_BOOTMENU_ITEM MenuEntries, OUT PULONG EntriesCount, OUT PULONG DefaultId);
typedef EFI_STATUS (*PBL_BOOTPROTO_BOOT_SYSTEM)(IN PXTBL_BOOT_PARAMETERS Parameters);
typedef EFI_STATUS (*PBL_CLOSE_VOLUME)(IN PEFI_HANDLE VolumeHandle);
typedef VOID (*PBL_CLEAR_CONSOLE_LINE)(IN ULONGLONG LineNo);
typedef EFI_STATUS (*PBL_CLOSE_XT_PROTOCOL)(IN PEFI_HANDLE Handle, IN PEFI_GUID ProtocolGuid);
typedef PWCHAR (*PBL_CONFIG_GET_VALUE)(IN CONST PWCHAR ConfigName);
typedef VOID (*PBL_CONSOLE_CLEAR_SCREEN)();
typedef VOID (*PBL_CONSOLE_DISABLE_CURSOR)();
typedef VOID (*PBL_CONSOLE_ENABLE_CURSOR)();
typedef VOID (*PBL_CONSOLE_PRINT)(IN PUINT16 Format, IN ...);
typedef VOID (*PBL_CONSOLE_QUERY_MODE)(OUT PUINT_PTR ResX, OUT PUINT_PTR ResY);
typedef VOID (*PBL_CONSOLE_READ_KEY_STROKE)(OUT PEFI_INPUT_KEY Key);
typedef VOID (*PBL_CONSOLE_RESET_INPUT_BUFFER)();
typedef VOID (*PBL_CONSOLE_SET_ATTRIBUTES)(IN ULONGLONG Attributes);
typedef VOID (*PBL_CONSOLE_SET_CURSOR_POSITION)(IN ULONGLONG PosX, IN ULONGLONG PosY);
typedef VOID (*PBL_CONSOLE_WRITE)(IN PUSHORT String);
typedef VOID (XTAPI *PBL_COPY_MEMORY)(OUT PVOID Destination, IN PCVOID Source, IN SIZE_T Length);
typedef VOID (*PBL_DEBUG_PRINT)(IN PUINT16 Format, IN ...);
typedef EFI_STATUS (*PBL_EXECIMAGE_GET_ENTRY_POINT)(IN PVOID ImagePointer, OUT PVOID *EntryPoint);
typedef EFI_STATUS (*PBL_EXECIMAGE_GET_MACHINE_TYPE)(IN PVOID ImagePointer, OUT PUSHORT MachineType);
typedef EFI_STATUS (*PBL_EXECIMAGE_GET_SUBSYSTEM)(IN PVOID ImagePointer, OUT PUSHORT SubSystem);
typedef EFI_STATUS (*PBL_EXECIMAGE_LOAD_IMAGE)(IN PEFI_FILE_HANDLE FileHandle, IN LOADER_MEMORY_TYPE MemoryType, IN PVOID VirtualAddress, OUT PVOID *ImagePointer);
typedef EFI_STATUS (*PBL_EXECIMAGE_RELOCATE_IMAGE)(IN PVOID ImagePointer, IN EFI_VIRTUAL_ADDRESS Address);
typedef EFI_STATUS (*PBL_EXIT_BOOT_SERVICES)();
typedef EFI_STATUS (*PBL_FIND_BOOT_PROTOCOL)(IN PWCHAR SystemType, OUT PEFI_GUID BootProtocolGuid);
typedef EFI_STATUS (*PBL_FREE_PAGES)(IN UINT64 Size, IN EFI_PHYSICAL_ADDRESS Memory);
typedef EFI_STATUS (*PBL_FREE_POOL)(IN PVOID Memory);
typedef EFI_STATUS (*PBL_GET_MEMORY_MAP)(OUT PEFI_MEMORY_MAP MemoryMap);
typedef PLIST_ENTRY (*PBL_GET_MODULES_LIST)();
typedef INT_PTR (*PBL_GET_SECURE_BOOT_STATUS)();
typedef EFI_STATUS (*PBL_INSTALL_XT_PROTOCOL)(IN PVOID Interface, IN PEFI_GUID Guid);
typedef EFI_STATUS (*PBL_INVOKE_BOOT_PROTOCOL)(IN PLIST_ENTRY OptionsList);
typedef EFI_STATUS (*PBL_OPEN_VOLUME)(IN PEFI_DEVICE_PATH_PROTOCOL DevicePath, OUT PEFI_HANDLE DiskHandle, OUT PEFI_FILE_HANDLE *FsHandle);
typedef EFI_STATUS (*PBL_OPEN_XT_PROTOCOL)(OUT PEFI_HANDLE Handle, OUT PVOID *ProtocolHandler, IN PEFI_GUID ProtocolGuid);
typedef EFI_STATUS (*PBL_READ_FILE)(IN PEFI_FILE_HANDLE DirHandle, IN CONST PWCHAR FileName, OUT PVOID *FileData, OUT PSIZE_T FileSize);
typedef EFI_STATUS (*PBL_REGISTER_BOOT_PROTOCOL)(IN PWCHAR SystemType, IN PEFI_GUID BootProtocolGuid);
typedef VOID (*PBL_REGISTER_XT_BOOT_MENU)(PVOID BootMenuRoutine);
typedef VOID (XTAPI *PBL_SET_MEMORY)(OUT PVOID Destination, IN UCHAR Byte, IN SIZE_T Length);
typedef VOID (*PBL_SLEEP_EXECUTION)(IN ULONG_PTR Milliseconds);
typedef VOID (*PBL_TUI_DISPLAY_ERROR_DIALOG)(IN PWCHAR Caption, IN PWCHAR Message);
typedef VOID (*PBL_TUI_DISPLAY_INFO_DIALOG)(IN PWCHAR Caption, IN PWCHAR Message);
typedef VOID (*PBL_TUI_DISPLAY_INPUT_DIALOG)(IN PWCHAR Caption, IN PWCHAR Message, IN PWCHAR *InputFieldText);
typedef XTBL_DIALOG_HANDLE (*PBL_TUI_DISPLAY_PROGRESS_DIALOG)(IN PWCHAR Caption, IN PWCHAR Message, IN UCHAR Percentage);
typedef VOID (*PBL_TUI_UPDATE_PROGRESS_BAR)(IN PXTBL_DIALOG_HANDLE Handle, IN PWCHAR Message, IN UCHAR Percentage);
typedef EFI_STATUS (*PBL_WAIT_FOR_EFI_EVENT)(IN UINT_PTR NumberOfEvents, IN PEFI_EVENT Event, OUT PUINT_PTR Index);
typedef VOID (*PBL_XT_BOOT_MENU)();
typedef VOID (XTAPI *PBL_ZERO_MEMORY)(OUT PVOID Destination, IN SIZE_T Length);

/* Boot parameters structure */
typedef struct _XTBL_BOOT_PARAMETERS
{
    PEFI_DEVICE_PATH_PROTOCOL DevicePath;
    PWCHAR ArcName;
    PWCHAR SystemPath;
    PWCHAR SystemType;
    PWCHAR KernelFile;
    PWCHAR InitrdFile;
    PWCHAR HalFile;
    PWCHAR Parameters;
} XTBL_BOOT_PARAMETERS, *PXTBL_BOOT_PARAMETERS;

/* Boot menu list structure */
typedef struct _XTBL_BOOTMENU_ITEM
{
    PWCHAR EntryName;
    PLIST_ENTRY Options;
} XTBL_BOOTMENU_ITEM, *PXTBL_BOOTMENU_ITEM;

/* XTLDR Configuration data */
typedef struct _XTBL_CONFIG_ENTRY
{
    LIST_ENTRY Flink;
    PWCHAR Name;
    PWCHAR Value;
} XTBL_CONFIG_ENTRY, *PXTBL_CONFIG_ENTRY;

/* XTLDR Configuration section */
typedef struct _XTBL_CONFIG_SECTION
{
    LIST_ENTRY Flink;
    LIST_ENTRY Options;
    PWCHAR SectionName;
} XTBL_CONFIG_SECTION, *PXTBL_CONFIG_SECTION;

/* XTLDR Dialog handle data */
typedef struct _XTBL_DIALOG_HANDLE
{
    UCHAR Attributes;
    UCHAR DialogColor;
    UCHAR TextColor;
    UINT_PTR ResX;
    UINT_PTR ResY;
    UINT_PTR PosX;
    UINT_PTR PosY;
    UINT_PTR Width;
    UINT_PTR Height;
} XTBL_DIALOG_HANDLE, *PXTBL_DIALOG_HANDLE;

/* Registered boot protocol structure */
typedef struct _XTBL_KNOWN_BOOT_PROTOCOL
{
    LIST_ENTRY Flink;
    PWCHAR SystemType;
    EFI_GUID Guid;
} XTBL_KNOWN_BOOT_PROTOCOL, *PXTBL_KNOWN_BOOT_PROTOCOL;

/* XTLDR Module dependencies data */
typedef struct _XTBL_MODULE_DEPS
{
    LIST_ENTRY Flink;
    PWCHAR ModuleName;
} XTBL_MODULE_DEPS, *PXTBL_MODULE_DEPS;

/* XTLDR Module information data */
typedef struct _XTBL_MODULE_INFO
{
    LIST_ENTRY Flink;
    PWCHAR ModuleName;
    PWCHAR ModuleDescription;
    LIST_ENTRY Dependencies;
    PVOID ModuleBase;
    UINT64 ModuleSize;
    UINT32 Revision;
    PEFI_IMAGE_UNLOAD UnloadModule;
} XTBL_MODULE_INFO, *PXTBL_MODULE_INFO;

/* XTLDR Status data */
typedef struct _XTBL_STATUS
{
    PBL_XT_BOOT_MENU BootMenu;
    BOOLEAN BootServices;
    ULONG DebugPort;
    INT_PTR SecureBoot;
    CPPORT SerialPort;
} XTBL_STATUS, *PXTBL_STATUS;

/* XTLDR Boot protocol structure */
typedef struct _XTBL_BOOT_PROTOCOL
{
    PBL_BOOTPROTO_BOOT_SYSTEM BootSystem;
} XTBL_BOOT_PROTOCOL, *PXTBL_BOOT_PROTOCOL;

/* XTLDR Executable image protocol structure */
typedef struct _XTBL_EXECUTABLE_IMAGE_PROTOCOL
{
    PBL_EXECIMAGE_GET_ENTRY_POINT GetEntryPoint;
    PBL_EXECIMAGE_GET_MACHINE_TYPE GetMachineType;
    // PBL_EXECIMAGE_GET_SECTION GetSection;
    PBL_EXECIMAGE_GET_SUBSYSTEM GetSubSystem;
    // PBL_EXECIMAGE_GET_VERSION GetVersion;
    PBL_EXECIMAGE_LOAD_IMAGE LoadImage;
    // PBL_EXECIMAGE_PRINT_INFO PrintDebugInfo;
    PBL_EXECIMAGE_RELOCATE_IMAGE RelocateImage;
    // PBL_EXECUTABLE_VERIFY_IMAGE VerifyImage;
} XTBL_EXECUTABLE_IMAGE_PROTOCOL, *PXTBL_EXECUTABLE_IMAGE_PROTOCOL;

/* XTLDR Loader protocol */
typedef struct _XTBL_LOADER_PROTOCOL
{
    struct
    {
        PBL_FIND_BOOT_PROTOCOL FindProtocol;
        PBL_BOOTMENU_INITIALIZE_OS_LIST InitializeMenuList;
        PBL_INVOKE_BOOT_PROTOCOL InvokeProtocol;
        PBL_REGISTER_XT_BOOT_MENU RegisterMenu;
        PBL_REGISTER_BOOT_PROTOCOL RegisterProtocol;
    } Boot;
    struct
    {
        PBL_CONFIG_GET_VALUE GetValue;
    } Config;
    struct
    {
        PBL_CLEAR_CONSOLE_LINE ClearLine;
        PBL_CONSOLE_CLEAR_SCREEN ClearScreen;
        PBL_CONSOLE_DISABLE_CURSOR DisableCursor;
        PBL_CONSOLE_ENABLE_CURSOR EnableCursor;
        PBL_CONSOLE_PRINT Print;
        PBL_CONSOLE_QUERY_MODE QueryMode;
        PBL_CONSOLE_READ_KEY_STROKE ReadKeyStroke;
        PBL_CONSOLE_RESET_INPUT_BUFFER ResetInputBuffer;
        PBL_CONSOLE_SET_ATTRIBUTES SetAttributes;
        PBL_CONSOLE_SET_CURSOR_POSITION SetCursorPosition;
        PBL_CONSOLE_WRITE Write;
    } Console;
    struct
    {
        PBL_DEBUG_PRINT Print;
    } Debug;
    struct
    {
        PBL_CLOSE_VOLUME CloseVolume;
        PBL_OPEN_VOLUME OpenVolume;
        PBL_READ_FILE ReadFile;
    } Disk;
    struct
    {
        PBL_ALLOCATE_PAGES AllocatePages;
        PBL_ALLOCATE_POOL AllocatePool;
        PBL_COPY_MEMORY CopyMemory;
        PBL_FREE_PAGES FreePages;
        PBL_FREE_POOL FreePool;
        PBL_GET_MEMORY_MAP GetMemoryMap;
        PBL_SET_MEMORY SetMemory;
        PBL_ZERO_MEMORY ZeroMemory;
    } Memory;
    struct
    {
        PBL_CLOSE_XT_PROTOCOL Close;
        PBL_GET_MODULES_LIST GetModulesList;
        PBL_INSTALL_XT_PROTOCOL Install;
        PBL_OPEN_XT_PROTOCOL Open;
    } Protocol;
    struct
    {
        PBL_TUI_DISPLAY_ERROR_DIALOG DisplayErrorDialog;
        PBL_TUI_DISPLAY_INFO_DIALOG DisplayInfoDialog;
        PBL_TUI_DISPLAY_INPUT_DIALOG DisplayInputDialog;
        PBL_TUI_DISPLAY_PROGRESS_DIALOG DisplayProgressDialog;
        PBL_TUI_UPDATE_PROGRESS_BAR UpdateProgressBar;
    } Tui;
    struct
    {
        PBL_EXIT_BOOT_SERVICES ExitBootServices;
        PBL_GET_SECURE_BOOT_STATUS GetSecureBootStatus;
        PBL_SLEEP_EXECUTION SleepExecution;
        PBL_WAIT_FOR_EFI_EVENT WaitForEfiEvent;
    } Util;
} XTBL_LOADER_PROTOCOL, *PXTBL_LOADER_PROTOCOL;

#endif /* __XTDK_BLTYPES_H */
