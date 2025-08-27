/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/bltypes.h
 * DESCRIPTION:     XT Boot Loader structures definitions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_BLTYPES_H
#define __XTDK_BLTYPES_H

#include <xttypes.h>
#include <xtstruct.h>
#include <xtuefi.h>
#include <hltypes.h>
#include ARCH_HEADER(xtstruct.h)


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

/* XTLDR Routine pointers */
typedef LONG (*PBL_GET_MEMTYPE_ROUTINE)(IN EFI_MEMORY_TYPE EfiMemoryType);

/* Boot Loader protocol routine pointers */
typedef EFI_STATUS (*PBL_ALLOCATE_PAGES)(IN EFI_ALLOCATE_TYPE AllocationType, IN ULONGLONG Size, OUT PEFI_PHYSICAL_ADDRESS Memory);
typedef EFI_STATUS (*PBL_ALLOCATE_POOL)(IN UINT_PTR Size, OUT PVOID *Memory);
typedef EFI_STATUS (*PBL_BOOTMENU_INITIALIZE_OS_LIST)(IN ULONG MaxNameLength, OUT PXTBL_BOOTMENU_ITEM *MenuEntries, OUT PULONG EntriesCount, OUT PULONG DefaultId);
typedef BOOLEAN (*PBL_BOOTUTIL_GET_BOOLEAN_PARAMETER)(IN CONST PWCHAR Parameters, IN CONST PWCHAR Needle);
typedef EFI_STATUS (*PBL_BUILD_PAGE_MAP)(IN PXTBL_PAGE_MAPPING PageMap, IN ULONG_PTR SelfMapAddress);
typedef EFI_STATUS (*PBL_CLOSE_VOLUME)(IN PEFI_HANDLE VolumeHandle);
typedef VOID (*PBL_CLEAR_CONSOLE_LINE)(IN ULONGLONG LineNo);
typedef EFI_STATUS (*PBL_CLOSE_XT_PROTOCOL)(IN PEFI_HANDLE Handle, IN PEFI_GUID ProtocolGuid);
typedef BOOLEAN (*PBL_CONFIG_GET_BOOLEAN_VALUE)(IN CONST PWCHAR ConfigName);
typedef EFI_STATUS (*PBL_CONFIG_GET_BOOT_OPTION_VALUE)(IN PLIST_ENTRY Options, IN CONST PWCHAR OptionName, OUT PWCHAR *OptionValue);
typedef VOID (*PBL_CONFIG_GET_EDITABLE_OPTIONS)(OUT CONST PWCHAR **OptionsArray, OUT PSIZE_T OptionsCount);
typedef PWCHAR (*PBL_CONFIG_GET_VALUE)(IN CONST PWCHAR ConfigName);
typedef EFI_STATUS (*PBL_CONFIG_SET_BOOT_OPTION_VALUE)(IN PLIST_ENTRY Options, IN CONST PWCHAR OptionName, IN CONST PWCHAR OptionValue);
typedef VOID (*PBL_CONSOLE_CLEAR_SCREEN)();
typedef VOID (*PBL_CONSOLE_DISABLE_CURSOR)();
typedef VOID (*PBL_CONSOLE_ENABLE_CURSOR)();
typedef VOID (*PBL_CONSOLE_PRINT)(IN PUSHORT Format, IN ...);
typedef VOID (*PBL_CONSOLE_QUERY_MODE)(OUT PUINT_PTR ResX, OUT PUINT_PTR ResY);
typedef VOID (*PBL_CONSOLE_READ_KEY_STROKE)(OUT PEFI_INPUT_KEY Key);
typedef VOID (*PBL_CONSOLE_RESET_INPUT_BUFFER)();
typedef VOID (*PBL_CONSOLE_SET_ATTRIBUTES)(IN ULONGLONG Attributes);
typedef VOID (*PBL_CONSOLE_SET_CURSOR_POSITION)(IN ULONGLONG PosX, IN ULONGLONG PosY);
typedef VOID (*PBL_CONSOLE_WRITE)(IN PUSHORT String);
typedef VOID (XTAPI *PBL_COPY_MEMORY)(OUT PVOID Destination, IN PCVOID Source, IN SIZE_T Length);
typedef VOID (*PBL_DEBUG_PRINT)(IN PUSHORT Format, IN ...);
typedef EFI_STATUS (*PBL_ENTER_FIRMWARE_SETUP)();
typedef EFI_STATUS (*PBL_EXIT_BOOT_SERVICES)();
typedef EFI_STATUS (*PBL_FIND_BOOT_PROTOCOL)(IN PWCHAR SystemType, OUT PEFI_GUID BootProtocolGuid);
typedef EFI_STATUS (*PBL_FREE_PAGES)(IN ULONGLONG Size, IN EFI_PHYSICAL_ADDRESS Memory);
typedef EFI_STATUS (*PBL_FREE_POOL)(IN PVOID Memory);
typedef EFI_STATUS (*PBL_GET_CONFIGURATION_TABLE)(IN PEFI_GUID TableGuid, OUT PVOID *Table);
typedef EFI_STATUS (*PBL_GET_EFI_VARIABLE)(IN PEFI_GUID Vendor, IN PWCHAR VariableName, OUT PVOID *VariableValue);
typedef VOID (*PBL_GET_MAPPINGS_COUNT)(IN PXTBL_PAGE_MAPPING PageMap, OUT PULONG NumberOfMappings);
typedef EFI_STATUS (*PBL_GET_MEMORY_MAP)(OUT PEFI_MEMORY_MAP MemoryMap);
typedef PLIST_ENTRY (*PBL_GET_MODULES_LIST)();
typedef ULONGLONG (*PBL_GET_RANDOM_VALUE)(IN OUT PULONGLONG RNGBuffer);
typedef INT_PTR (*PBL_GET_SECURE_BOOT_STATUS)();
typedef PVOID (*PBL_GET_VIRTUAL_ADDRESS)(IN PXTBL_PAGE_MAPPING PageMap, IN PVOID PhysicalAddress);
typedef EFI_STATUS (*PBL_INITIALIZE_ENTROPY)(PULONGLONG RNGBuffer);
typedef VOID (*PBL_INITIALIZE_PAGE_MAP)(OUT PXTBL_PAGE_MAPPING PageMap, IN SHORT PageMapLevel, IN PAGE_SIZE PageSize);
typedef EFI_STATUS (*PBL_INSTALL_XT_PROTOCOL)(IN PVOID Interface, IN PEFI_GUID Guid);
typedef EFI_STATUS (*PBL_INVOKE_BOOT_PROTOCOL)(IN PWCHAR ShortName, IN PLIST_ENTRY OptionsList);
typedef EFI_STATUS (*PBL_LOCATE_PROTOCOL_HANDLES)(OUT PEFI_HANDLE *Handles, OUT PUINT_PTR Count, IN PEFI_GUID ProtocolGuid);
typedef EFI_STATUS (*PBL_LOAD_EFI_IMAGE)(IN PEFI_DEVICE_PATH_PROTOCOL DevicePath, IN PVOID ImageData, IN SIZE_T ImageSize, OUT PEFI_HANDLE ImageHandle);
typedef EFI_STATUS (*PBL_MAP_EFI_MEMORY)(IN OUT PXTBL_PAGE_MAPPING PageMap, IN OUT PVOID *MemoryMapAddress, IN PBL_GET_MEMTYPE_ROUTINE GetMemoryTypeRoutine);
typedef EFI_STATUS (*PBL_MAP_PAGE)(IN PXTBL_PAGE_MAPPING PageMap, IN ULONG_PTR VirtualAddress, IN ULONG_PTR PhysicalAddress, IN ULONG NumberOfPages);
typedef EFI_STATUS (*PBL_MAP_VIRTUAL_MEMORY)(IN OUT PXTBL_PAGE_MAPPING PageMap, IN PVOID VirtualAddress, IN PVOID PhysicalAddress, IN ULONGLONG NumberOfPages, IN LOADER_MEMORY_TYPE MemoryType);
typedef EFI_STATUS (*PBL_OPEN_VOLUME)(IN PEFI_DEVICE_PATH_PROTOCOL DevicePath, OUT PEFI_HANDLE DiskHandle, OUT PEFI_FILE_HANDLE *FsHandle);
typedef EFI_STATUS (*PBL_OPEN_PROTOCOL)(OUT PEFI_HANDLE Handle, OUT PVOID *ProtocolHandler, IN PEFI_GUID ProtocolGuid);
typedef EFI_STATUS (*PBL_OPEN_PROTOCOL_HANDLE)(IN EFI_HANDLE Handle, OUT PVOID *ProtocolHandler, IN PEFI_GUID ProtocolGuid);
typedef PVOID (*PBL_PHYSICAL_ADDRESS_TO_VIRTUAL)(IN PVOID PhysicalAddress, IN PVOID PhysicalBase, IN PVOID VirtualBase);
typedef EFI_STATUS (*PBL_PHYSICAL_LIST_TO_VIRTUAL)(IN PXTBL_PAGE_MAPPING PageMap, IN OUT PLIST_ENTRY ListHead, IN PVOID PhysicalBase, IN PVOID VirtualBase);
typedef EFI_STATUS (*PBL_POWER_SYSTEM)();
typedef EFI_STATUS (*PBL_READ_FILE)(IN PEFI_FILE_HANDLE DirHandle, IN CONST PWCHAR FileName, OUT PVOID *FileData, OUT PSIZE_T FileSize);
typedef EFI_STATUS (*PBL_REGISTER_BOOT_PROTOCOL)(IN PWCHAR SystemType, IN PEFI_GUID BootProtocolGuid);
typedef VOID (*PBL_REGISTER_XT_BOOT_MENU)(PVOID BootMenuRoutine);
typedef EFI_STATUS (*PBL_SET_EFI_VARIABLE)(IN PEFI_GUID Vendor, IN PWCHAR VariableName, IN PVOID VariableValue, IN UINT_PTR Size);
typedef VOID (XTAPI *PBL_SET_MEMORY)(OUT PVOID Destination, IN UCHAR Byte, IN SIZE_T Length);
typedef VOID (*PBL_SLEEP_EXECUTION)(IN ULONG_PTR Milliseconds);
typedef EFI_STATUS (*PBL_START_EFI_IMAGE)(IN EFI_HANDLE ImageHandle);
typedef VOID (*PBL_TUI_DISPLAY_ERROR_DIALOG)(IN PWCHAR Caption, IN PWCHAR Message);
typedef VOID (*PBL_TUI_DISPLAY_INFO_DIALOG)(IN PWCHAR Caption, IN PWCHAR Message);
typedef VOID (*PBL_TUI_DISPLAY_INPUT_DIALOG)(IN PWCHAR Caption, IN PWCHAR Message, IN PWCHAR *InputFieldText);
typedef XTBL_DIALOG_HANDLE (*PBL_TUI_DISPLAY_PROGRESS_DIALOG)(IN PWCHAR Caption, IN PWCHAR Message, IN UCHAR Percentage);
typedef VOID (*PBL_TUI_UPDATE_PROGRESS_BAR)(IN PXTBL_DIALOG_HANDLE Handle, IN PWCHAR Message, IN UCHAR Percentage);
typedef EFI_STATUS (*PBL_WAIT_FOR_EFI_EVENT)(IN UINT_PTR NumberOfEvents, IN PEFI_EVENT Event, OUT PUINT_PTR Index);
typedef VOID (*PBL_XT_BOOT_MENU)();
typedef VOID (XTAPI *PBL_ZERO_MEMORY)(OUT PVOID Destination, IN SIZE_T Length);

/* Module protocols routine pointers */
typedef EFI_STATUS (*PBL_ACPI_GET_ACPI_DESCRIPTION_POINTER)(OUT PVOID *AcpiTable);
typedef EFI_STATUS (*PBL_ACPI_GET_ACPI_TABLE)(IN CONST UINT Signature, IN PVOID PreviousTable, OUT PVOID *AcpiTable);
typedef EFI_STATUS (*PBL_ACPI_GET_APIC_BASE)(OUT PVOID *ApicBase);
typedef EFI_STATUS (*PBL_ACPI_GET_RSDP_TABLE)(OUT PVOID *AcpiTable);
typedef EFI_STATUS (*PBL_ACPI_GET_SMBIOS_TABLE)(OUT PVOID *SmBiosTable);
typedef EFI_STATUS (*PBL_ACPI_GET_SMBIOS3_TABLE)(OUT PVOID *SmBiosTable);
typedef EFI_STATUS (*PBL_ACPI_GET_XSDP_TABLE)(OUT PVOID *AcpiTable);
typedef EFI_STATUS (*PBL_BOOTPROTO_BOOT_SYSTEM)(IN PXTBL_BOOT_PARAMETERS Parameters);
typedef EFI_STATUS (*PBL_EXECIMAGE_GET_ENTRY_POINT)(IN PVOID ImagePointer, OUT PVOID *EntryPoint);
typedef EFI_STATUS (*PBL_EXECIMAGE_GET_FILE_SIZE)(IN PVOID ImagePointer, OUT PULONGLONG FileSize);
typedef EFI_STATUS (*PBL_EXECIMAGE_GET_IMAGE_SIZE)(IN PVOID ImagePointer, OUT PUINT ImageSize);
typedef EFI_STATUS (*PBL_EXECIMAGE_GET_MACHINE_TYPE)(IN PVOID ImagePointer, OUT PUSHORT MachineType);
typedef EFI_STATUS (*PBL_EXECIMAGE_GET_SECTION)(IN PVOID ImagePointer, IN PCHAR SectionName, OUT PULONG *RawData);
typedef EFI_STATUS (*PBL_EXECIMAGE_GET_SUBSYSTEM)(IN PVOID ImagePointer, OUT PUSHORT SubSystem);
typedef EFI_STATUS (*PBL_EXECIMAGE_GET_VERSION)(IN PVOID ImagePointer, OUT PUSHORT Version);
typedef EFI_STATUS (*PBL_EXECIMAGE_LOAD_IMAGE)(IN PEFI_FILE_HANDLE FileHandle, IN LOADER_MEMORY_TYPE MemoryType, IN PVOID VirtualAddress, OUT PVOID *ImagePointer);
typedef EFI_STATUS (*PBL_EXECIMAGE_RELOCATE_IMAGE)(IN PVOID ImagePointer, IN EFI_VIRTUAL_ADDRESS Address);
typedef EFI_STATUS (*PBL_EXECIMAGE_UNLOAD_IMAGE)(IN PVOID ImagePointer);
typedef EFI_STATUS (*PBL_EXECIMAGE_VERIFY_IMAGE)(IN PVOID ImagePointer);
typedef EFI_STATUS (*PBL_FRAMEBUFFER_GET_DISPLAY_DRIVER)(OUT PEFI_GRAPHICS_PROTOCOL Protocol);
typedef EFI_STATUS (*PBL_FRAMEBUFFER_GET_DISPLAY_INFORMATION)(OUT PEFI_PHYSICAL_ADDRESS FrameBufferBase, OUT PULONG_PTR FrameBufferSize, OUT PXTBL_FRAMEBUFFER_MODE_INFORMATION ModeInfo);
typedef EFI_STATUS (*PBL_FRAMEBUFFER_GET_PREFERRED_SCREEN_RESOLUTION)(OUT PUINT PreferredWidth, OUT PUINT PreferredHeight);
typedef EFI_STATUS (*PBL_FRAMEBUFFER_INITIALIZE)();
typedef EFI_STATUS (*PBL_FRAMEBUFFER_SET_SCREEN_RESOLUTION)(IN UINT Width, IN UINT Height);

/* Boot parameters structure */
typedef struct _XTBL_BOOT_PARAMETERS
{
    PEFI_DEVICE_PATH_PROTOCOL DevicePath;
    PWCHAR ArcName;
    PWCHAR EfiPath;
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
    PWCHAR FullName;
    PWCHAR ShortName;
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

/* Boot Loader memory mapping information */
typedef struct _XTBL_MEMORY_MAPPING
{
    LIST_ENTRY ListEntry;
    PVOID VirtualAddress;
    PVOID PhysicalAddress;
    ULONGLONG NumberOfPages;
    LOADER_MEMORY_TYPE MemoryType;
} XTBL_MEMORY_MAPPING, *PXTBL_MEMORY_MAPPING;

/* XTLDR Module dependencies data */
typedef struct _XTBL_MODULE_AUTHORS
{
    LIST_ENTRY Flink;
    PWCHAR AuthorName;
} XTBL_MODULE_AUTHORS, *PXTBL_MODULE_AUTHORS;

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
    PWCHAR License;
    PWCHAR Version;
    LIST_ENTRY Authors;
    LIST_ENTRY Dependencies;
    PVOID ModuleBase;
    ULONGLONG ModuleSize;
    UINT Revision;
    PEFI_IMAGE_UNLOAD UnloadModule;
} XTBL_MODULE_INFO, *PXTBL_MODULE_INFO;

/* Boot Loader page mapping information */
typedef struct _XTBL_PAGE_MAPPING
{
    LIST_ENTRY MemoryMap;
    ULONG MapSize;
    PVOID PtePointer;
    SHORT PageMapLevel;
    PAGE_SIZE PageSize;
} XTBL_PAGE_MAPPING, *PXTBL_PAGE_MAPPING;

/* XTLDR Status data */
typedef struct _XTBL_STATUS
{
    PBL_XT_BOOT_MENU BootMenu;
    BOOLEAN BootServices;
    ULONG DebugPort;
    PVOID LoaderBase;
    ULONGLONG LoaderSize;
    INT_PTR SecureBoot;
    CPPORT SerialPort;
} XTBL_STATUS, *PXTBL_STATUS;

/* XT framebuffer video mode information structure definition */
typedef struct _XTBL_FRAMEBUFFER_MODE_INFORMATION
{
    UINT Width;
    UINT Height;
    UINT Depth;
    UINT RefreshRate;
    UINT BitsPerPixel;
    UINT BytesPerPixel;
    UINT PixelsPerScanLine;
    UINT Pitch;
    EFI_GRAPHICS_PIXEL_FORMAT PixelFormat;
    struct
    {
        USHORT BlueShift;
        USHORT BlueSize;
        USHORT GreenShift;
        USHORT GreenSize;
        USHORT RedShift;
        USHORT RedSize;
        USHORT ReservedShift;
        USHORT ReservedSize;
    } PixelInformation;
} XTBL_FRAMEBUFFER_MODE_INFORMATION, *PXTBL_FRAMEBUFFER_MODE_INFORMATION;

/* XT framebuffer information structure definition */
typedef struct _XTBL_FRAMEBUFFER_INFORMATION
{
    BOOLEAN Initialized;
    EFI_GRAPHICS_PROTOCOL Protocol;
    EFI_PHYSICAL_ADDRESS FrameBufferBase;
    ULONG_PTR FrameBufferSize;
    UINT DefaultMode;
    EFI_HANDLE Handle;
    union
    {
        PEFI_GRAPHICS_OUTPUT_PROTOCOL Gop;
        PEFI_UNIVERSAL_GRAPHICS_ADAPTER_PROTOCOL Uga;
    } Driver;
    XTBL_FRAMEBUFFER_MODE_INFORMATION ModeInfo;
} XTBL_FRAMEBUFFER_INFORMATION, *PXTBL_FRAMEBUFFER_INFORMATION;

/* XTLDR ACPI protocol structure */
typedef struct _XTBL_ACPI_PROTOCOL
{
    PBL_ACPI_GET_ACPI_DESCRIPTION_POINTER GetAcpiDescriptionPointer;
    PBL_ACPI_GET_ACPI_TABLE GetAcpiTable;
    PBL_ACPI_GET_APIC_BASE GetApicBase;
    PBL_ACPI_GET_RSDP_TABLE GetRsdpTable;
    PBL_ACPI_GET_SMBIOS_TABLE GetSMBiosTable;
    PBL_ACPI_GET_SMBIOS3_TABLE GetSMBios3Table;
    PBL_ACPI_GET_XSDP_TABLE GetXsdpTable;
} XTBL_ACPI_PROTOCOL, *PXTBL_ACPI_PROTOCOL;

/* XTLDR Boot protocol structure */
typedef struct _XTBL_BOOT_PROTOCOL
{
    PBL_BOOTPROTO_BOOT_SYSTEM BootSystem;
} XTBL_BOOT_PROTOCOL, *PXTBL_BOOT_PROTOCOL;

/* XTLDR Executable image protocol structure */
typedef struct _XTBL_EXECUTABLE_IMAGE_PROTOCOL
{
    PBL_EXECIMAGE_GET_ENTRY_POINT GetEntryPoint;
    PBL_EXECIMAGE_GET_FILE_SIZE GetFileSize;
    PBL_EXECIMAGE_GET_IMAGE_SIZE GetImageSize;
    PBL_EXECIMAGE_GET_MACHINE_TYPE GetMachineType;
    PBL_EXECIMAGE_GET_SECTION GetSection;
    PBL_EXECIMAGE_GET_SUBSYSTEM GetSubSystem;
    PBL_EXECIMAGE_GET_VERSION GetVersion;
    PBL_EXECIMAGE_LOAD_IMAGE LoadImage;
    PBL_EXECIMAGE_RELOCATE_IMAGE RelocateImage;
    PBL_EXECIMAGE_UNLOAD_IMAGE UnloadImage;
    PBL_EXECIMAGE_VERIFY_IMAGE VerifyImage;
} XTBL_EXECUTABLE_IMAGE_PROTOCOL, *PXTBL_EXECUTABLE_IMAGE_PROTOCOL;

/* XT framebuffer support protocol structure */
typedef struct _XTBL_FRAMEBUFFER_PROTOCOL
{
    PBL_FRAMEBUFFER_GET_DISPLAY_DRIVER GetDisplayDriver;
    PBL_FRAMEBUFFER_GET_DISPLAY_INFORMATION GetDisplayInformation;
    PBL_FRAMEBUFFER_GET_PREFERRED_SCREEN_RESOLUTION GetPreferredScreenResolution;
    PBL_FRAMEBUFFER_INITIALIZE Initialize;
    PBL_FRAMEBUFFER_SET_SCREEN_RESOLUTION SetScreenResolution;
} XTBL_FRAMEBUFFER_PROTOCOL, *PXTBL_FRAMEBUFFER_PROTOCOL;

/* XTLDR Loader protocol structure */
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
        PBL_BOOTUTIL_GET_BOOLEAN_PARAMETER GetBooleanParameter;
    } BootUtil;
    struct
    {
        PBL_CONFIG_GET_BOOLEAN_VALUE GetBooleanValue;
        PBL_CONFIG_GET_BOOT_OPTION_VALUE GetBootOptionValue;
        PBL_CONFIG_GET_EDITABLE_OPTIONS GetEditableOptions;
        PBL_CONFIG_GET_VALUE GetValue;
        PBL_CONFIG_SET_BOOT_OPTION_VALUE SetBootOptionValue;
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
        PBL_BUILD_PAGE_MAP BuildPageMap;
        PBL_COPY_MEMORY CopyMemory;
        PBL_FREE_PAGES FreePages;
        PBL_FREE_POOL FreePool;
        PBL_GET_MAPPINGS_COUNT GetMappingsCount;
        PBL_GET_MEMORY_MAP GetMemoryMap;
        PBL_GET_VIRTUAL_ADDRESS GetVirtualAddress;
        PBL_INITIALIZE_PAGE_MAP InitializePageMap;
        PBL_MAP_EFI_MEMORY MapEfiMemory;
        PBL_MAP_PAGE MapPage;
        PBL_MAP_VIRTUAL_MEMORY MapVirtualMemory;
        PBL_PHYSICAL_ADDRESS_TO_VIRTUAL PhysicalAddressToVirtual;
        PBL_PHYSICAL_LIST_TO_VIRTUAL PhysicalListToVirtual;
        PBL_SET_MEMORY SetMemory;
        PBL_ZERO_MEMORY ZeroMemory;
    } Memory;
    struct
    {
        PBL_CLOSE_XT_PROTOCOL Close;
        PBL_GET_MODULES_LIST GetModulesList;
        PBL_INSTALL_XT_PROTOCOL Install;
        PBL_LOCATE_PROTOCOL_HANDLES LocateHandles;
        PBL_OPEN_PROTOCOL Open;
        PBL_OPEN_PROTOCOL_HANDLE OpenHandle;
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
        PBL_ENTER_FIRMWARE_SETUP EnterFirmwareSetup;
        PBL_EXIT_BOOT_SERVICES ExitBootServices;
        PBL_GET_CONFIGURATION_TABLE GetConfigurationTable;
        PBL_GET_EFI_VARIABLE GetEfiVariable;
        PBL_GET_RANDOM_VALUE GetRandomValue;
        PBL_GET_SECURE_BOOT_STATUS GetSecureBootStatus;
        PBL_INITIALIZE_ENTROPY InitializeEntropy;
        PBL_LOAD_EFI_IMAGE LoadEfiImage;
        PBL_POWER_SYSTEM RebootSystem;
        PBL_SET_EFI_VARIABLE SetEfiVariable;
        PBL_POWER_SYSTEM ShutdownSystem;
        PBL_SLEEP_EXECUTION SleepExecution;
        PBL_START_EFI_IMAGE StartEfiImage;
        PBL_WAIT_FOR_EFI_EVENT WaitForEfiEvent;
    } Util;
} XTBL_LOADER_PROTOCOL, *PXTBL_LOADER_PROTOCOL;

#endif /* __XTDK_BLTYPES_H */
