/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/includes/xtldr.hh
 * DESCRIPTION:     Top level header for XTLDR
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_XTLDR_HH
#define __XTLDR_XTLDR_HH

#include <xtblapi.h>
#include <xtver.h>

#include <libxtos.hh>
#include <globals.hh>


class BootUtils
{
    public:
        STATIC XTCDECL BOOLEAN GetBooleanParameter(IN PCWSTR Parameters,
                                                   IN PCWSTR Needle);
};

class Configuration
{
    public:
        STATIC XTCDECL BOOLEAN GetBooleanValue(IN PCWSTR ConfigName);
        STATIC XTCDECL EFI_STATUS GetBootOptionValue(IN PLIST_ENTRY Options,
                                                     IN PCWSTR OptionName,
                                                     OUT PWCHAR *OptionValue);
        STATIC XTCDECL VOID GetEditableOptions(OUT PCWSTR **OptionsArray,
                                               OUT PULONG OptionsCount);
        STATIC XTCDECL EFI_STATUS GetValue(IN PCWSTR ConfigName,
                                           OUT PWCHAR *ConfigValue);
        STATIC XTCDECL EFI_STATUS LoadConfiguration();
        STATIC XTCDECL EFI_STATUS ParseCommandLine();
        STATIC XTCDECL EFI_STATUS SetBootOptionValue(IN PLIST_ENTRY Options,
                                                     IN PCWSTR OptionName,
                                                     IN PCWSTR OptionValue);

    private:
        STATIC XTCDECL EFI_STATUS ParseConfigFile(IN CONST PCHAR RawConfig,
                                                  OUT PLIST_ENTRY Configuration);
        STATIC XTCDECL EFI_STATUS ReadConfigFile(IN PCWSTR ConfigDirectory,
                                                 IN PCWSTR ConfigFile,
                                                 OUT PCHAR *ConfigData);
        STATIC XTCDECL EFI_STATUS SetValue(IN PCWSTR ConfigName,
                                           IN PCWSTR ConfigValue);
        STATIC XTCDECL VOID UpdateConfiguration(IN PLIST_ENTRY NewConfig);
};

class Console
{
    public:
        STATIC XTCDECL VOID ClearLine(IN ULONGLONG LineNo);
        STATIC XTCDECL VOID ClearScreen();
        STATIC XTCDECL VOID DisableCursor();
        STATIC XTCDECL VOID EnableCursor();
        STATIC XTCDECL VOID InitializeConsole();
        STATIC XTCDECL VOID Print(IN PCWSTR Format,
                                  IN ...);
        STATIC XTCDECL XTSTATUS PutChar(IN WCHAR Character);
        STATIC XTCDECL VOID QueryMode(OUT PUINT_PTR ResX,
                                      OUT PUINT_PTR ResY);
        STATIC XTCDECL VOID ReadKeyStroke(OUT PEFI_INPUT_KEY Key);
        STATIC XTCDECL VOID ResetInputBuffer();
        STATIC XTCDECL VOID SetAttributes(IN ULONGLONG Attributes);
        STATIC XTCDECL VOID SetCursorPosition(IN ULONGLONG PosX,
                                              IN ULONGLONG PosY);
        STATIC XTCDECL VOID Write(IN PCWSTR String);

    private:
        STATIC XTCDECL EFI_STATUS SetMode(IN ULONGLONG Mode);
};

class Debug
{
    public:
            STATIC XTCDECL EFI_STATUS InitializeDebugConsole();
            STATIC XTCDECL VOID Print(IN PCWSTR Format,
                                      IN ...);
            STATIC XTCDECL XTSTATUS PutChar(IN WCHAR Character);

        private:
            STATIC XTCDECL EFI_STATUS ActivateSerialIOController();
            STATIC XTCDECL EFI_STATUS InitializeSerialPort(IN ULONG PortNumber,
                                                           IN ULONG PortAddress,
                                                           IN ULONG BaudRate);
};

class EfiUtils
{
    public:
        STATIC XTCDECL EFI_STATUS EnterFirmwareSetup();
        STATIC XTCDECL EFI_STATUS ExitBootServices();
        STATIC XTCDECL EFI_STATUS GetConfigurationTable(IN PEFI_GUID TableGuid,
                                                       OUT PVOID *Table);
        STATIC XTCDECL EFI_STATUS GetEfiVariable(IN PEFI_GUID Vendor,
                                                 IN PCWSTR VariableName,
                                                 OUT PVOID *VariableValue);
        STATIC XTCDECL ULONGLONG GetRandomValue(IN OUT PULONGLONG RNGBuffer);
        STATIC XTCDECL INT_PTR GetSecureBootStatus();
        STATIC XTCDECL EFI_STATUS InitializeEntropy(PULONGLONG RNGBuffer);
        STATIC XTCDECL EFI_STATUS LoadEfiImage(IN PEFI_DEVICE_PATH_PROTOCOL DevicePath,
                                               IN PVOID ImageData,
                                               IN SIZE_T ImageSize,
                                               OUT PEFI_HANDLE ImageHandle);
        STATIC XTCDECL EFI_STATUS RebootSystem();
        STATIC XTCDECL EFI_STATUS SetEfiVariable(IN PEFI_GUID Vendor,
                                                 IN PCWSTR VariableName,
                                                 IN PVOID VariableValue,
                                                 IN UINT_PTR Size);
        STATIC XTCDECL EFI_STATUS ShutdownSystem();
        STATIC XTCDECL VOID SleepExecution(IN ULONG_PTR Milliseconds);
        STATIC XTCDECL EFI_STATUS StartEfiImage(IN EFI_HANDLE ImageHandle);
        STATIC XTCDECL EFI_STATUS WaitForEfiEvent(IN UINT_PTR NumberOfEvents,
                                                  IN PEFI_EVENT Event,
                                                  OUT PUINT_PTR Index);
};

class Memory
{
    public:
        STATIC XTCDECL EFI_STATUS AllocatePages(IN EFI_ALLOCATE_TYPE AllocationType,
                                                IN ULONGLONG NumberOfPages,
                                                OUT PEFI_PHYSICAL_ADDRESS Memory);
        STATIC XTCDECL EFI_STATUS AllocatePool(IN UINT_PTR Size,
                                               OUT PVOID *Memory);
        STATIC XTCDECL EFI_STATUS BuildPageMap(IN PXTBL_PAGE_MAPPING PageMap,
                                               IN ULONG_PTR SelfMapAddress);
        STATIC XTCDECL EFI_STATUS FreePages(IN ULONGLONG NumberOfPages,
                                            IN EFI_PHYSICAL_ADDRESS Memory);
        STATIC XTCDECL EFI_STATUS FreePool(IN PVOID Memory);
        STATIC XTCDECL VOID GetMappingsCount(IN PXTBL_PAGE_MAPPING PageMap,
                                             OUT PULONG NumberOfMappings);
        STATIC XTCDECL EFI_STATUS GetMemoryMap(OUT PEFI_MEMORY_MAP MemoryMap);
        STATIC XTCDECL PVOID GetVirtualAddress(IN PXTBL_PAGE_MAPPING PageMap,
                                               IN PVOID PhysicalAddress);
        STATIC XTCDECL VOID InitializePageMap(OUT PXTBL_PAGE_MAPPING PageMap,
                                              IN SHORT PageMapLevel,
                                              IN PAGE_SIZE PageSize);
        STATIC XTCDECL EFI_STATUS MapEfiMemory(IN OUT PXTBL_PAGE_MAPPING PageMap,
                                               IN OUT PVOID *MemoryMapAddress,
                                               IN PBL_GET_MEMTYPE_ROUTINE GetMemoryTypeRoutine);
        STATIC XTCDECL EFI_STATUS MapPage(IN PXTBL_PAGE_MAPPING PageMap,
                                          IN ULONG_PTR VirtualAddress,
                                          IN ULONG_PTR PhysicalAddress,
                                          IN ULONG NumberOfPages);
        STATIC XTCDECL EFI_STATUS MapVirtualMemory(IN OUT PXTBL_PAGE_MAPPING PageMap,
                                                   IN PVOID VirtualAddress,
                                                   IN PVOID PhysicalAddress,
                                                   IN ULONGLONG NumberOfPages,
                                                   IN LOADER_MEMORY_TYPE MemoryType);
        STATIC XTCDECL PVOID PhysicalAddressToVirtual(IN PVOID PhysicalAddress,
                                                      IN PVOID PhysicalBase,
                                                      IN PVOID VirtualBase);
        STATIC XTCDECL EFI_STATUS PhysicalListToVirtual(IN PXTBL_PAGE_MAPPING PageMap,
                                                        IN OUT PLIST_ENTRY ListHead,
                                                        IN PVOID PhysicalBase,
                                                        IN PVOID VirtualBase);

    private:
        STATIC XTCDECL LOADER_MEMORY_TYPE GetLoaderMemoryType(IN EFI_MEMORY_TYPE EfiMemoryType);
        STATIC XTCDECL EFI_STATUS GetNextPageTable(IN PXTBL_PAGE_MAPPING PageMap,
                                                   IN PVOID PageTable,
                                                   IN SIZE_T Entry,
                                                   OUT PVOID *NextPageTable);
        STATIC XTCDECL EFI_STATUS SelfMapPml(IN PXTBL_PAGE_MAPPING PageMap,
                                             IN ULONG_PTR SelfMapAddress);
};

class Protocol
{
    public:
        STATIC XTCDECL EFI_STATUS CloseProtocol(IN PEFI_HANDLE Handle,
                                                IN PEFI_GUID ProtocolGuid);
        STATIC XTCDECL EFI_STATUS FindBootProtocol(IN PCWSTR SystemType,
                                                   OUT PEFI_GUID BootProtocolGuid);
        STATIC XTCDECL PLIST_ENTRY GetModulesList();
        STATIC XTCDECL EFI_STATUS InstallProtocol(IN PVOID Interface,
                                                 IN PEFI_GUID Guid);
        STATIC XTCDECL EFI_STATUS InvokeBootProtocol(IN PWCHAR ShortName,
                                                     IN PLIST_ENTRY OptionsList);
        STATIC XTCDECL EFI_STATUS LoadModule(IN PWCHAR ModuleName);
        STATIC XTCDECL EFI_STATUS LoadModules(IN PWCHAR ModulesList);
        STATIC XTCDECL EFI_STATUS LocateProtocolHandles(OUT PEFI_HANDLE *Handles,
                                                        OUT PUINT_PTR Count,
                                                        IN PEFI_GUID ProtocolGuid);
        STATIC XTCDECL EFI_STATUS OpenProtocol(OUT PEFI_HANDLE Handle,
                                               OUT PVOID *ProtocolHandler,
                                               IN PEFI_GUID ProtocolGuid);
        STATIC XTCDECL EFI_STATUS OpenProtocolHandle(IN EFI_HANDLE Handle,
                                                     OUT PVOID *ProtocolHandler,
                                                     IN PEFI_GUID ProtocolGuid);
        STATIC XTCDECL VOID RegisterBootMenu(IN PVOID BootMenuRoutine);
        STATIC XTCDECL EFI_STATUS RegisterBootProtocol(IN PCWSTR SystemType,
                                                       IN PEFI_GUID BootProtocolGuid);
        STATIC XTCDECL EFI_STATUS InstallXtLoaderProtocol();

    private:
        STATIC XTCDECL EFI_STATUS GetModuleInformation(IN PWCHAR SectionData,
                                                       IN ULONG SectionSize,
                                                       OUT PXTBL_MODULE_INFO ModuleInfo);
        STATIC XTCDECL EFI_STATUS GetModuleInfoStrings(IN PWCHAR SectionData,
                                                       IN ULONG SectionSize,
                                                       OUT PWCHAR **ModInfo,
                                                       OUT PULONG InfoCount);
};

class Shell
{
    public:
        STATIC XTCDECL VOID StartLoaderShell();

    private:
        STATIC XTCDECL VOID PrintPrompt();
};

class TextUi
{
    public:
        STATIC XTCDECL VOID DisplayBootMenu();
        STATIC XTCDECL VOID DisplayErrorDialog(IN PCWSTR Caption,
                                               IN PCWSTR Message);
        STATIC XTCDECL VOID DisplayInfoDialog(IN PCWSTR Caption,
                                              IN PCWSTR Message);
        STATIC XTCDECL VOID DisplayInputDialog(IN PCWSTR Caption,
                                               IN PCWSTR Message,
                                               IN OUT PWCHAR *InputFieldText);
        STATIC XTCDECL XTBL_DIALOG_HANDLE DisplayProgressDialog(IN PCWSTR Caption,
                                                                IN PCWSTR Message,
                                                                IN UCHAR Percentage);
        STATIC XTCDECL VOID UpdateProgressBar(IN PXTBL_DIALOG_HANDLE Handle,
                                              IN PCWSTR Message,
                                              IN UCHAR Percentage);

    private:
        STATIC XTCDECL VOID DetermineDialogBoxSize(IN OUT PXTBL_DIALOG_HANDLE Handle,
                                                   IN PCWSTR Message);
        STATIC XTCDECL VOID DisplayEditMenu(IN PXTBL_BOOTMENU_ITEM MenuEntry);
        STATIC XTCDECL VOID DrawBootMenu(OUT PXTBL_DIALOG_HANDLE Handle);
        STATIC XTCDECL VOID DrawBootMenuEntry(IN PXTBL_DIALOG_HANDLE Handle,
                                              IN PWCHAR MenuEntry,
                                              IN UINT Position,
                                              IN BOOLEAN Highlighted);
        STATIC XTCDECL VOID DrawDialogBox(IN OUT PXTBL_DIALOG_HANDLE Handle,
                                          IN PCWSTR Caption,
                                          IN PCWSTR Message);
        STATIC XTCDECL VOID DrawButton(IN PXTBL_DIALOG_HANDLE Handle);
        STATIC XTCDECL VOID DrawInputField(IN PXTBL_DIALOG_HANDLE Handle,
                                           IN PWCHAR InputFieldText);
        STATIC XTCDECL VOID DrawMessage(IN PXTBL_DIALOG_HANDLE Handle,
                                        IN PCWSTR Message);
        STATIC XTCDECL VOID DrawProgressBar(IN PXTBL_DIALOG_HANDLE Handle,
                                            IN UCHAR Percentage);
        STATIC XTCDECL VOID DrawEditMenu(OUT PXTBL_DIALOG_HANDLE Handle);
        STATIC XTCDECL EFI_STATUS DrawEditMenuEntry(IN PXTBL_DIALOG_HANDLE Handle,
                                                    IN PCWSTR OptionName,
                                                    IN PCWSTR OptionValue,
                                                    IN UINT Position,
                                                    IN BOOLEAN Highlighted);
};

class Volume
{
    public:
        STATIC XTCDECL EFI_STATUS CloseVolume(IN PEFI_HANDLE VolumeHandle);
        STATIC XTCDECL EFI_STATUS EnumerateBlockDevices();
        STATIC XTCDECL EFI_STATUS FindDevicePath(IN PEFI_DEVICE_PATH_PROTOCOL FsHandle,
                                                       IN CONST PWCHAR FileSystemPath,
                                                       OUT PEFI_DEVICE_PATH_PROTOCOL* DevicePath);
        STATIC XTCDECL EFI_STATUS GetEfiPath(IN PWCHAR SystemPath,
                                             OUT PWCHAR *EfiPath);
        STATIC XTCDECL EFI_STATUS GetDevicePath(IN PWCHAR SystemPath,
                                                OUT PEFI_DEVICE_PATH_PROTOCOL *DevicePath,
                                                OUT PWCHAR *ArcName,
                                                OUT PWCHAR *Path);
        STATIC XTCDECL EFI_STATUS OpenVolume(IN PEFI_DEVICE_PATH_PROTOCOL DevicePath,
                                             OUT PEFI_HANDLE DiskHandle,
                                             OUT PEFI_FILE_HANDLE *FsHandle);
        STATIC XTCDECL EFI_STATUS ReadFile(IN PEFI_FILE_HANDLE DirHandle,
                                           IN PCWSTR FileName,
                                           OUT PVOID *FileData,
                                           OUT PSIZE_T FileSize);


    private:
        STATIC XTCDECL EFI_STATUS DiscoverEfiBlockDevices(OUT PLIST_ENTRY BlockDevices);
        STATIC XTCDECL EFI_STATUS DissectArcPath(IN PWCHAR SystemPath,
                                                 OUT PWCHAR *ArcName,
                                                 OUT PWCHAR *Path,
                                                 OUT PUSHORT DriveType,
                                                 OUT PULONG DriveNumber,
                                                 OUT PULONG PartNumber);
        STATIC XTCDECL PEFI_DEVICE_PATH_PROTOCOL DuplicateDevicePath(IN PEFI_DEVICE_PATH_PROTOCOL DevicePath);
        STATIC XTCDECL EFI_STATUS FindLastBlockDeviceNode(IN PEFI_DEVICE_PATH_PROTOCOL DevicePath,
                                                          OUT PEFI_DEVICE_PATH_PROTOCOL *LastNode);
        STATIC XTCDECL BOOLEAN FindParentBlockDevice(IN PLIST_ENTRY BlockDevices,
                                                     IN PEFI_BLOCK_DEVICE_DATA ChildNode,
                                                     OUT PEFI_BLOCK_DEVICE_DATA *ParentNode);
};

class XtLoader
{
    public:
        STATIC XTCDECL VOID InitializeBootLoader();
        STATIC XTCDECL EFI_STATUS InitializeBootMenuList(IN ULONG MaxNameLength,
                                                         OUT PXTBL_BOOTMENU_ITEM *MenuEntries,
                                                         OUT PULONG EntriesCount,
                                                         OUT PULONG DefaultId);
};







/* XTLDR routines forward references */
XTCDECL
EFI_STATUS
BlAllocateMemoryPages(IN EFI_ALLOCATE_TYPE AllocationType,
                      IN ULONGLONG NumberOfPages,
                      OUT PEFI_PHYSICAL_ADDRESS Memory);

XTCDECL
EFI_STATUS
BlAllocateMemoryPool(IN UINT_PTR Size,
                     OUT PVOID *Memory);

XTCDECL
EFI_STATUS
BlBuildPageMap(IN PXTBL_PAGE_MAPPING PageMap,
               IN ULONG_PTR SelfMapAddress);

XTCDECL
VOID
BlClearConsoleLine(IN ULONGLONG LineNo);

XTCDECL
VOID
BlClearConsoleScreen();

XTCDECL
EFI_STATUS
BlCloseProtocol(IN PEFI_HANDLE Handle,
                IN PEFI_GUID ProtocolGuid);

XTCDECL
EFI_STATUS
BlCloseVolume(IN PEFI_HANDLE VolumeHandle);

XTCDECL
VOID
BlConsolePrint(IN PCWSTR Format,
               IN ...);

XTCDECL
VOID
BlConsoleWrite(IN PCWSTR String);

XTCDECL
VOID
BlDebugPrint(IN PCWSTR Format,
             IN ...);

XTCDECL
VOID
BlDisableConsoleCursor();

XTCDECL
VOID
BlDisplayBootMenu();

XTCDECL
VOID
BlDisplayEditMenu(IN PXTBL_BOOTMENU_ITEM MenuEntry);

XTCDECL
VOID
BlDisplayErrorDialog(IN PCWSTR Caption,
                     IN PCWSTR Message);

XTCDECL
VOID
BlDisplayInfoDialog(IN PCWSTR Caption,
                    IN PCWSTR Message);

XTCDECL
VOID
BlDisplayInputDialog(IN PCWSTR Caption,
                     IN PCWSTR Message,
                     IN OUT PWCHAR *InputFieldText);

XTCDECL
XTBL_DIALOG_HANDLE
BlDisplayProgressDialog(IN PCWSTR Caption,
                        IN PCWSTR Message,
                        IN UCHAR Percentage);

XTCDECL
VOID
BlEnableConsoleCursor();

XTCDECL
EFI_STATUS
BlEnterFirmwareSetup();

XTCDECL
EFI_STATUS
BlEnumerateBlockDevices();

XTCDECL
EFI_STATUS
BlExitBootServices();

XTCDECL
EFI_STATUS
BlFindBootProtocol(IN PCWSTR SystemType,
                   OUT PEFI_GUID BootProtocolGuid);

XTCDECL
EFI_STATUS
BlFindVolumeDevicePath(IN PEFI_DEVICE_PATH_PROTOCOL FsHandle,
                       IN CONST PWCHAR FileSystemPath,
                       OUT PEFI_DEVICE_PATH_PROTOCOL* DevicePath);

XTCDECL
EFI_STATUS
BlFreeMemoryPages(IN ULONGLONG NumberOfPages,
                  IN EFI_PHYSICAL_ADDRESS Memory);

XTCDECL
EFI_STATUS
BlFreeMemoryPool(IN PVOID Memory);

XTCDECL
BOOLEAN
BlGetBooleanParameter(IN PCWSTR Parameters,
                      IN PCWSTR Needle);

XTCDECL
EFI_STATUS
BlGetBootOptionValue(IN PLIST_ENTRY Options,
                     IN PCWSTR OptionName,
                     OUT PWCHAR *OptionValue);

XTCDECL
BOOLEAN
BlGetConfigBooleanValue(IN PCWSTR ConfigName);

XTCDECL
EFI_STATUS
BlGetConfigValue(IN PCWSTR ConfigName,
                 OUT PWCHAR *ConfigValue);

XTCDECL
EFI_STATUS
BlGetConfigurationTable(IN PEFI_GUID TableGuid,
                        OUT PVOID *Table);

XTCDECL
VOID
BlGetEditableOptions(OUT PCWSTR **OptionsArray,
                     OUT PULONG OptionsCount);

XTCDECL
EFI_STATUS
BlGetEfiPath(IN PWCHAR SystemPath,
             OUT PWCHAR *EfiPath);

XTCDECL
EFI_STATUS
BlGetEfiVariable(IN PEFI_GUID Vendor,
                 IN PCWSTR VariableName,
                 OUT PVOID *VariableValue);

XTCDECL
VOID
BlGetMappingsCount(IN PXTBL_PAGE_MAPPING PageMap,
                   OUT PULONG NumberOfMappings);

XTCDECL
EFI_STATUS
BlGetMemoryMap(OUT PEFI_MEMORY_MAP MemoryMap);

XTCDECL
PLIST_ENTRY
BlGetModulesList();

XTCDECL
ULONGLONG
BlGetRandomValue(IN OUT PULONGLONG RNGBuffer);

XTCDECL
INT_PTR
BlGetSecureBootStatus();

XTCDECL
PVOID
BlGetVirtualAddress(IN PXTBL_PAGE_MAPPING PageMap,
                    IN PVOID PhysicalAddress);

XTCDECL
EFI_STATUS
BlGetVolumeDevicePath(IN PWCHAR SystemPath,
                      OUT PEFI_DEVICE_PATH_PROTOCOL *DevicePath,
                      OUT PWCHAR *ArcName,
                      OUT PWCHAR *Path);

XTCDECL
VOID
BlInitializeBootLoader();

XTCDECL
EFI_STATUS
BlInitializeBootMenuList(IN ULONG MaxNameLength,
                         OUT PXTBL_BOOTMENU_ITEM *MenuEntries,
                         OUT PULONG EntriesCount,
                         OUT PULONG DefaultId);

XTCDECL
VOID
BlInitializeConsole();

XTCDECL
EFI_STATUS
BlInitializeEntropy(PULONGLONG RNGBuffer);

XTCDECL
VOID
BlInitializePageMap(OUT PXTBL_PAGE_MAPPING PageMap,
                    IN SHORT PageMapLevel,
                    IN PAGE_SIZE PageSize);

XTCDECL
EFI_STATUS
BlInstallProtocol(IN PVOID Interface,
                  IN PEFI_GUID Guid);

XTCDECL
EFI_STATUS
BlInvokeBootProtocol(IN PWCHAR ShortName,
                     IN PLIST_ENTRY OptionsList);

XTCDECL
EFI_STATUS
BlLoadEfiImage(IN PEFI_DEVICE_PATH_PROTOCOL DevicePath,
               IN PVOID ImageData,
               IN SIZE_T ImageSize,
               OUT PEFI_HANDLE ImageHandle);

XTCDECL
EFI_STATUS
BlLoadModule(IN PWCHAR ModuleName);

XTCDECL
EFI_STATUS
BlLoadModules(IN PWCHAR ModulesList);

XTCDECL
EFI_STATUS
BlLocateProtocolHandles(OUT PEFI_HANDLE *Handles,
                        OUT PUINT_PTR Count,
                        IN PEFI_GUID ProtocolGuid);

XTCDECL
EFI_STATUS
BlMapEfiMemory(IN OUT PXTBL_PAGE_MAPPING PageMap,
               IN OUT PVOID *DesiredVirtualAddress,
               IN PBL_GET_MEMTYPE_ROUTINE GetMemoryTypeRoutine);

XTCDECL
EFI_STATUS
BlMapPage(IN PXTBL_PAGE_MAPPING PageMap,
          IN ULONG_PTR VirtualAddress,
          IN ULONG_PTR PhysicalAddress,
          IN ULONG NumberOfPages);

XTCDECL
EFI_STATUS
BlMapVirtualMemory(IN OUT PXTBL_PAGE_MAPPING PageMap,
                   IN PVOID VirtualAddress,
                   IN PVOID PhysicalAddress,
                   IN ULONGLONG NumberOfPages,
                   IN LOADER_MEMORY_TYPE MemoryType);

XTCDECL
EFI_STATUS
BlOpenVolume(IN PEFI_DEVICE_PATH_PROTOCOL DevicePath,
             OUT PEFI_HANDLE DiskHandle,
             OUT PEFI_FILE_HANDLE *FsHandle);

XTCDECL
EFI_STATUS
BlOpenProtocol(OUT PEFI_HANDLE Handle,
               OUT PVOID *ProtocolHandler,
               IN PEFI_GUID ProtocolGuid);

XTCDECL
EFI_STATUS
BlOpenProtocolHandle(IN EFI_HANDLE Handle,
                     OUT PVOID *ProtocolHandler,
                     IN PEFI_GUID ProtocolGuid);

XTCDECL
PVOID
BlPhysicalAddressToVirtual(IN PVOID PhysicalAddress,
                           IN PVOID PhysicalBase,
                           IN PVOID VirtualBase);

XTCDECL
EFI_STATUS
BlPhysicalListToVirtual(IN PXTBL_PAGE_MAPPING PageMap,
                        IN OUT PLIST_ENTRY ListHead,
                        IN PVOID PhysicalBase,
                        IN PVOID VirtualBase);

XTCDECL
VOID
BlQueryConsoleMode(OUT PUINT_PTR ResX,
                   OUT PUINT_PTR ResY);

XTCDECL
EFI_STATUS
BlReadFile(IN PEFI_FILE_HANDLE DirHandle,
           IN PCWSTR FileName,
           OUT PVOID *FileData,
           OUT PSIZE_T FileSize);

XTCDECL
VOID
BlReadKeyStroke(OUT PEFI_INPUT_KEY Key);

XTCDECL
EFI_STATUS
BlRebootSystem();

XTCDECL
VOID
BlRegisterBootMenu(PVOID BootMenuRoutine);

XTCDECL
EFI_STATUS
BlRegisterBootProtocol(IN PCWSTR SystemType,
                       IN PEFI_GUID BootProtocolGuid);

XTCDECL
VOID
BlResetConsoleInputBuffer();

XTCDECL
EFI_STATUS
BlSetBootOptionValue(IN PLIST_ENTRY Options,
                     IN PCWSTR OptionName,
                     IN PCWSTR OptionValue);

XTCDECL
EFI_STATUS
BlSetConfigValue(IN PCWSTR ConfigName,
                 IN PCWSTR ConfigValue);

XTCDECL
VOID
BlSetConsoleAttributes(IN ULONGLONG Attributes);

XTCDECL
EFI_STATUS
BlSetConsoleMode(IN ULONGLONG Mode);

XTCDECL
VOID
BlSetCursorPosition(IN ULONGLONG PosX,
                    IN ULONGLONG PosY);

XTCDECL
EFI_STATUS
BlSetEfiVariable(IN PEFI_GUID Vendor,
                 IN PCWSTR VariableName,
                 IN PVOID VariableValue,
                 IN UINT_PTR Size);

XTCDECL
EFI_STATUS
BlShutdownSystem();

XTCDECL
VOID
BlSleepExecution(IN ULONG_PTR Milliseconds);

XTCDECL
EFI_STATUS
BlStartEfiImage(IN EFI_HANDLE ImageHandle);

XTCDECL
VOID
BlStartLoaderShell();

XTCDECL
EFI_STATUS
BlStartXtLoader(IN EFI_HANDLE ImageHandle,
                IN PEFI_SYSTEM_TABLE SystemTable);

XTCDECL
VOID
BlUpdateProgressBar(IN PXTBL_DIALOG_HANDLE Handle,
                    IN PCWSTR Message,
                    IN UCHAR Percentage);

XTCDECL
EFI_STATUS
BlWaitForEfiEvent(IN UINT_PTR NumberOfEvents,
                  IN PEFI_EVENT Event,
                  OUT PUINT_PTR Index);

XTCDECL
EFI_STATUS
BlpActivateSerialIOController();

XTCDECL
XTSTATUS
BlpConsolePutChar(IN WCHAR Character);

XTCDECL
XTSTATUS
BlpDebugPutChar(IN WCHAR Character);

XTCDECL
VOID
BlpDetermineDialogBoxSize(IN OUT PXTBL_DIALOG_HANDLE Handle,
                          IN PCWSTR Message);

XTCDECL
EFI_STATUS
BlpDiscoverEfiBlockDevices(OUT PLIST_ENTRY BlockDevices);

XTCDECL
EFI_STATUS
BlpDissectVolumeArcPath(IN PWCHAR SystemPath,
                        OUT PWCHAR *ArcName,
                        OUT PWCHAR *Path,
                        OUT PUSHORT DriveType,
                        OUT PULONG DriveNumber,
                        OUT PULONG PartNumber);

XTCDECL
VOID
BlpDrawBootMenu(OUT PXTBL_DIALOG_HANDLE Handle);

XTCDECL
VOID
BlpDrawBootMenuEntry(IN PXTBL_DIALOG_HANDLE Handle,
                     IN PWCHAR MenuEntry,
                     IN UINT Position,
                     IN BOOLEAN Highlighted);

XTCDECL
VOID
BlpDrawDialogBox(IN OUT PXTBL_DIALOG_HANDLE Handle,
                 IN PCWSTR Caption,
                 IN PCWSTR Message);

XTCDECL
VOID
BlpDrawDialogButton(IN PXTBL_DIALOG_HANDLE Handle);

XTCDECL
VOID
BlpDrawDialogInputField(IN PXTBL_DIALOG_HANDLE Handle,
                        IN PWCHAR InputFieldText);

XTCDECL
VOID
BlpDrawDialogMessage(IN PXTBL_DIALOG_HANDLE Handle,
                     IN PCWSTR Message);

XTCDECL
VOID
BlpDrawDialogProgressBar(IN PXTBL_DIALOG_HANDLE Handle,
                         IN UCHAR Percentage);

XTCDECL
VOID
BlpDrawEditMenu(OUT PXTBL_DIALOG_HANDLE Handle);

XTCDECL
EFI_STATUS
BlpDrawEditMenuEntry(IN PXTBL_DIALOG_HANDLE Handle,
                     IN PCWSTR OptionName,
                     IN PCWSTR OptionValue,
                     IN UINT Position,
                     IN BOOLEAN Highlighted);

XTCDECL
PEFI_DEVICE_PATH_PROTOCOL
BlpDuplicateDevicePath(IN PEFI_DEVICE_PATH_PROTOCOL DevicePath);

XTCDECL
EFI_STATUS
BlpFindLastBlockDeviceNode(IN PEFI_DEVICE_PATH_PROTOCOL DevicePath,
                           OUT PEFI_DEVICE_PATH_PROTOCOL *LastNode);

XTCDECL
BOOLEAN
BlpFindParentBlockDevice(IN PLIST_ENTRY BlockDevices,
                         IN PEFI_BLOCK_DEVICE_DATA ChildNode,
                         OUT PEFI_BLOCK_DEVICE_DATA *ParentNode);

XTCDECL
LOADER_MEMORY_TYPE
BlpGetLoaderMemoryType(IN EFI_MEMORY_TYPE EfiMemoryType);

XTCDECL
EFI_STATUS
BlpGetModuleInformation(IN PWCHAR SectionData,
                        IN ULONG SectionSize,
                        OUT PXTBL_MODULE_INFO ModuleInfo);

XTCDECL
EFI_STATUS
BlpGetModuleInfoStrings(IN PWCHAR SectionData,
                        IN ULONG SectionSize,
                        OUT PWCHAR **ModInfo,
                        OUT PULONG InfoCount);

XTCDECL
EFI_STATUS
BlpGetNextPageTable(IN PXTBL_PAGE_MAPPING PageMap,
                    IN PVOID PageTable,
                    IN SIZE_T Entry,
                    OUT PVOID *NextPageTable);

XTCDECL
EFI_STATUS
BlpInitializeDebugConsole();

XTCDECL
EFI_STATUS
BlpInitializeSerialPort(IN ULONG PortNumber,
                        IN ULONG PortAddress,
                        IN ULONG BaudRate);

XTCDECL
EFI_STATUS
BlpInstallXtLoaderProtocol();

XTCDECL
EFI_STATUS
BlpLoadConfiguration();

XTCDECL
EFI_STATUS
BlpParseCommandLine(VOID);

XTCDECL
EFI_STATUS
BlpParseConfigFile(IN CONST PCHAR RawConfig,
                   OUT PLIST_ENTRY Configuration);

XTCDECL
VOID
BlpPrintShellPrompt();

XTCDECL
EFI_STATUS
BlpReadConfigFile(IN PCWSTR ConfigDirectory,
                  IN PCWSTR ConfigFile,
                  OUT PCHAR *ConfigData);

XTCDECL
EFI_STATUS
BlpSelfMapPml(IN PXTBL_PAGE_MAPPING PageMap,
              IN ULONG_PTR SelfMapAddress);

XTCDECL
ULONGLONG
BlpStringReadPadding(IN PUSHORT *Format);

XTCDECL
VOID
BlpUpdateConfiguration(IN PLIST_ENTRY NewConfig);

#endif /* __XTLDR_XTLDR_HH */
