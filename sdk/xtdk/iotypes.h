/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/iotypes.h
 * DESCRIPTION:     I/O related type definitions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTDK_IOTYPES_H
#define __XTDK_IOTYPES_H

#include <xttypes.h>
#include <xtstruct.h>
#include <setypes.h>
#include ARCH_HEADER(xtstruct.h)
#include ARCH_HEADER(ketypes.h)


/* Maximum volume label length */
#define IO_MAXIMUM_VOLUME_LABEL_LENGTH          (32 * sizeof(WCHAR))

/* I/O Request Packet function codes */
#define IRP_FC_CREATE                           0x00
#define IRP_FC_CREATE_NAMED_PIPE                0x01
#define IRP_FC_CLOSE                            0x02
#define IRP_FC_READ                             0x03
#define IRP_FC_WRITE                            0x04
#define IRP_FC_QUERY_INFORMATION                0x05
#define IRP_FC_SET_INFORMATION                  0x06
#define IRP_FC_QUERY_EA                         0x07
#define IRP_FC_SET_EA                           0x08
#define IRP_FC_FLUSH_BUFFERS                    0x09
#define IRP_FC_QUERY_VOLUME_INFORMATION         0x0a
#define IRP_FC_SET_VOLUME_INFORMATION           0x0b
#define IRP_FC_DIRECTORY_CONTROL                0x0c
#define IRP_FC_FILE_SYSTEM_CONTROL              0x0d
#define IRP_FC_DEVICE_CONTROL                   0x0e
#define IRP_FC_INTERNAL_DEVICE_CONTROL          0x0f
#define IRP_FC_SCSI                             0x0f
#define IRP_FC_SHUTDOWN                         0x10
#define IRP_FC_LOCK_CONTROL                     0x11
#define IRP_FC_CLEANUP                          0x12
#define IRP_FC_CREATE_MAILSLOT                  0x13
#define IRP_FC_QUERY_SECURITY                   0x14
#define IRP_FC_SET_SECURITY                     0x15
#define IRP_FC_POWER                            0x16
#define IRP_FC_SYSTEM_CONTROL                   0x17
#define IRP_FC_DEVICE_CHANGE                    0x18
#define IRP_FC_QUERY_QUOTA                      0x19
#define IRP_FC_SET_QUOTA                        0x1a
#define IRP_FC_PNP                              0x1b
#define IRP_FC_PNP_POWER                        0x1b
#define IRP_FC_MAXIMUM_FUNCTION                 0x1b

/* Number of PCI base address registers */
#define PCI_TYPE0_ADDRESSES                     6
#define PCI_TYPE1_ADDRESSES                     2
#define PCI_TYPE2_ADDRESSES                     5

/* PCI maximum number of devices */
#define PCI_MAX_BRIDGE_NUMBER                   255
#define PCI_MAX_DEVICES                         32
#define PCI_MAX_FUNCTION                        8

/* Invalid PCI vendor ID */
#define PCI_INVALID_VENDORID                    0xFFFF

/* PCI common config header types */
#define PCI_DEVICE_TYPE                         0x00
#define PCI_BRIDGE_TYPE                         0x01
#define PCI_CARDBUS_BRIDGE_TYPE                 0x02
#define PCI_MULTIFUNCTION                       0x80

/* PCI common config commands */
#define PCI_ENABLE_IO_SPACE                     0x0001
#define PCI_ENABLE_MEMORY_SPACE                 0x0002
#define PCI_ENABLE_BUS_MASTER                   0x0004
#define PCI_ENABLE_SPECIAL_CYCLES               0x0008
#define PCI_ENABLE_WRITE_AND_INVALIDATE         0x0010
#define PCI_ENABLE_VGA_COMPATIBLE_PALETTE       0x0020
#define PCI_ENABLE_PARITY                       0x0040
#define PCI_ENABLE_WAIT_CYCLE                   0x0080
#define PCI_ENABLE_SERR                         0x0100
#define PCI_ENABLE_FAST_BACK_TO_BACK            0x0200
#define PCI_DISABLE_LEVEL_INTERRUPT             0x0400

/* PCI common config statuses */
#define PCI_STATUS_INTERRUPT_PENDING            0x0008
#define PCI_STATUS_CAPABILITIES_LIST            0x0010
#define PCI_STATUS_66MHZ_CAPABLE                0x0020
#define PCI_STATUS_UDF_SUPPORTED                0x0040
#define PCI_STATUS_FAST_BACK_TO_BACK            0x0080
#define PCI_STATUS_DATA_PARITY_DETECTED         0x0100
#define PCI_STATUS_DEVSEL                       0x0600
#define PCI_STATUS_SIGNALED_TARGET_ABORT        0x0800
#define PCI_STATUS_RECEIVED_TARGET_ABORT        0x1000
#define PCI_STATUS_RECEIVED_MASTER_ABORT        0x2000
#define PCI_STATUS_SIGNALED_SYSTEM_ERROR        0x4000
#define PCI_STATUS_DETECTED_PARITY_ERROR        0x8000


/* C/C++ specific code */
#ifndef __XTOS_ASSEMBLER__

/* I/O routine callbacks */
typedef XTSTATUS (XTAPI *PDRIVER_ADD_DEVICE)(IN PDRIVER_OBJECT DriverObject, PDEVICE_OBJECT PhysicalDeviceObject);
typedef VOID (XTAPI *PDRIVER_CANCEL)(IN PDEVICE_OBJECT DeviceObject, IN OUT PIRP Irp);
typedef IO_ALLOCATION_ACTION (XTAPI *PDRIVER_CONTROL)(IN PDEVICE_OBJECT DeviceObject, IN OUT PIRP Irp, IN PVOID MapRegisterBase, IN PVOID Context);
typedef XTSTATUS (XTAPI *PDRIVER_DISPATCH)(IN PDEVICE_OBJECT DeviceObject, IN OUT PIRP Irp);
typedef XTSTATUS (XTAPI *PDRIVER_INITIALIZE)(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath);
typedef VOID (XTAPI *PDRIVER_STARTIO)(IN OUT PDEVICE_OBJECT DeviceObject, IN OUT PIRP Irp);
typedef VOID (XTAPI *PDRIVER_UNLOAD)(IN PDRIVER_OBJECT DriverObject);
typedef VOID (XTAPI *PFAST_IO_ACQUIRE_FILE)(IN PFILE_OBJECT FileObject);
typedef XTSTATUS (XTAPI *PFAST_IO_ACQUIRE_FOR_CCFLUSH)(IN PFILE_OBJECT FileObject, IN PDEVICE_OBJECT DeviceObject);
typedef XTSTATUS (XTAPI *PFAST_IO_ACQUIRE_FOR_MOD_WRITE)(IN PFILE_OBJECT FileObject, IN PLARGE_INTEGER EndingOffset, OUT PERESOURCE *ResourceToRelease, IN PDEVICE_OBJECT DeviceObject);
typedef BOOLEAN (XTAPI *PFAST_IO_CHECK_IF_POSSIBLE)(IN PFILE_OBJECT FileObject, IN PLARGE_INTEGER FileOffset, IN ULONG Length, IN BOOLEAN Wait, IN ULONG LockKey, IN BOOLEAN CheckForReadOperation, OUT PIO_STATUS_BLOCK IoStatus, IN PDEVICE_OBJECT DeviceObject);
typedef BOOLEAN (XTAPI *PFAST_IO_DEVICE_CONTROL)(IN PFILE_OBJECT FileObject, IN BOOLEAN Wait, IN PVOID InputBuffer, IN ULONG InputBufferLength, OUT PVOID OutputBuffer, IN ULONG OutputBufferLength, IN ULONG IoControlCode, OUT PIO_STATUS_BLOCK IoStatus, IN PDEVICE_OBJECT DeviceObject);
typedef VOID (XTAPI *PFAST_IO_DETACH_DEVICE)(IN PDEVICE_OBJECT SourceDevice, IN PDEVICE_OBJECT TargetDevice);
typedef BOOLEAN (XTAPI *PFAST_IO_LOCK)(IN PFILE_OBJECT FileObject, IN PLARGE_INTEGER FileOffset, IN PLARGE_INTEGER Length, IN PEPROCESS ProcessId, IN ULONG Key, IN BOOLEAN FailImmediately, IN BOOLEAN ExclusiveLock, OUT PIO_STATUS_BLOCK IoStatus, IN PDEVICE_OBJECT DeviceObject);
typedef BOOLEAN (XTAPI *PFAST_IO_MDL_READ)(IN PFILE_OBJECT FileObject, IN PLARGE_INTEGER FileOffset, IN ULONG Length, IN ULONG LockKey, OUT PMDL *MdlChain, OUT PIO_STATUS_BLOCK IoStatus, IN PDEVICE_OBJECT DeviceObject);
typedef BOOLEAN (XTAPI *PFAST_IO_MDL_READ_COMPLETE)(IN PFILE_OBJECT FileObject, IN PMDL MdlChain, IN PDEVICE_OBJECT DeviceObject);
typedef BOOLEAN (XTAPI *PFAST_IO_MDL_READ_COMPLETE_COMPRESSED)(IN PFILE_OBJECT FileObject, IN PMDL MdlChain, IN PDEVICE_OBJECT DeviceObject);
typedef BOOLEAN (XTAPI *PFAST_IO_MDL_WRITE_COMPLETE)(IN PFILE_OBJECT FileObject, IN PLARGE_INTEGER FileOffset, IN PMDL MdlChain, IN PDEVICE_OBJECT DeviceObject);
typedef BOOLEAN (XTAPI *PFAST_IO_MDL_WRITE_COMPLETE_COMPRESSED)(IN PFILE_OBJECT FileObject, IN PLARGE_INTEGER FileOffset, IN PMDL MdlChain, IN PDEVICE_OBJECT DeviceObject);
typedef BOOLEAN (XTAPI *PFAST_IO_PREPARE_MDL_WRITE)(IN PFILE_OBJECT FileObject, IN PLARGE_INTEGER FileOffset, IN ULONG Length, IN ULONG LockKey, OUT PMDL *MdlChain, OUT PIO_STATUS_BLOCK IoStatus, IN PDEVICE_OBJECT DeviceObject);
typedef BOOLEAN (XTAPI *PFAST_IO_QUERY_BASIC_INFO)(IN PFILE_OBJECT FileObject, IN BOOLEAN Wait, OUT PFILE_BASIC_INFORMATION Buffer, OUT PIO_STATUS_BLOCK IoStatus, IN PDEVICE_OBJECT DeviceObject);
typedef BOOLEAN (XTAPI *PFAST_IO_QUERY_NETWORK_OPEN_INFO)(IN PFILE_OBJECT FileObject, IN BOOLEAN Wait, OUT PFILE_NETWORK_OPEN_INFORMATION Buffer, OUT PIO_STATUS_BLOCK IoStatus, IN PDEVICE_OBJECT DeviceObject);
typedef BOOLEAN (XTAPI *PFAST_IO_QUERY_OPEN)(IN OUT PIRP Irp, OUT PFILE_NETWORK_OPEN_INFORMATION NetworkInformation, IN PDEVICE_OBJECT DeviceObject);
typedef BOOLEAN (XTAPI *PFAST_IO_QUERY_STANDARD_INFO)(IN PFILE_OBJECT FileObject, IN BOOLEAN Wait, OUT PFILE_STANDARD_INFORMATION Buffer, OUT PIO_STATUS_BLOCK IoStatus, IN PDEVICE_OBJECT DeviceObject);
typedef BOOLEAN (XTAPI *PFAST_IO_READ)(IN PFILE_OBJECT FileObject, IN PLARGE_INTEGER FileOffset, IN ULONG Length, IN BOOLEAN Wait, IN ULONG LockKey, OUT PVOID Buffer, OUT PIO_STATUS_BLOCK IoStatus, IN PDEVICE_OBJECT DeviceObject);
typedef BOOLEAN (XTAPI *PFAST_IO_READ_COMPRESSED)(IN PFILE_OBJECT FileObject, IN PLARGE_INTEGER FileOffset, IN ULONG Length, IN ULONG LockKey, OUT PVOID Buffer, OUT PMDL *MdlChain, OUT PIO_STATUS_BLOCK IoStatus, OUT PCOMPRESSED_DATA_INFO CompressedDataInfo, IN ULONG CompressedDataInfoLength, IN PDEVICE_OBJECT DeviceObject);
typedef VOID (XTAPI *PFAST_IO_RELEASE_FILE)(IN PFILE_OBJECT FileObject);
typedef XTSTATUS (XTAPI *PFAST_IO_RELEASE_FOR_CCFLUSH)(IN PFILE_OBJECT FileObject, IN PDEVICE_OBJECT DeviceObject);
typedef XTSTATUS (XTAPI *PFAST_IO_RELEASE_FOR_MOD_WRITE)(IN PFILE_OBJECT FileObject, IN PERESOURCE ResourceToRelease, IN PDEVICE_OBJECT DeviceObject);
typedef BOOLEAN (XTAPI *PFAST_IO_UNLOCK_ALL)(IN PFILE_OBJECT FileObject, IN PEPROCESS ProcessId, OUT PIO_STATUS_BLOCK IoStatus, IN PDEVICE_OBJECT DeviceObject);
typedef BOOLEAN (XTAPI *PFAST_IO_UNLOCK_ALL_BY_KEY)(IN PFILE_OBJECT FileObject, IN PVOID ProcessId, IN ULONG Key, OUT PIO_STATUS_BLOCK IoStatus, IN PDEVICE_OBJECT DeviceObject);
typedef BOOLEAN (XTAPI *PFAST_IO_UNLOCK_SINGLE)(IN PFILE_OBJECT FileObject, IN PLARGE_INTEGER FileOffset, IN PLARGE_INTEGER Length, IN PEPROCESS ProcessId, IN ULONG Key, OUT PIO_STATUS_BLOCK IoStatus, IN PDEVICE_OBJECT DeviceObject);
typedef BOOLEAN (XTAPI *PFAST_IO_WRITE)(IN PFILE_OBJECT FileObject, IN PLARGE_INTEGER FileOffset, IN ULONG Length, IN BOOLEAN Wait, IN ULONG LockKey, IN PVOID Buffer, OUT PIO_STATUS_BLOCK IoStatus, IN PDEVICE_OBJECT DeviceObject);
typedef BOOLEAN (XTAPI *PFAST_IO_WRITE_COMPRESSED)(IN PFILE_OBJECT FileObject, IN PLARGE_INTEGER FileOffset, IN ULONG Length, IN ULONG LockKey, IN PVOID Buffer, OUT PMDL *MdlChain, OUT PIO_STATUS_BLOCK IoStatus, IN PCOMPRESSED_DATA_INFO CompressedDataInfo, IN ULONG CompressedDataInfoLength, IN PDEVICE_OBJECT DeviceObject);
typedef VOID (XTAPI *PINTERFACE_DEREFERENCE)(IN PVOID Context);
typedef VOID (XTAPI *PINTERFACE_REFERENCE)(IN PVOID Context);
typedef VOID (XTAPI *PIO_APC_ROUTINE)(IN PVOID ApcContext, IN PIO_STATUS_BLOCK IoStatusBlock, IN ULONG Reserved);
typedef XTSTATUS (XTAPI *PIO_COMPLETION_ROUTINE)(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp, IN PVOID Context);
typedef VOID (XTAPI *PIO_TIMER_ROUTINE)(IN PDEVICE_OBJECT DeviceObject, IN PVOID Context);

/* Bus Query ID Type */
typedef enum _BUS_QUERY_ID_TYPE
{
    BusQueryDeviceID,
    BusQueryHardwareIDs,
    BusQueryCompatibleIDs,
    BusQueryInstanceID,
    BusQueryDeviceSerialNumber,
    BusQueryContainerID
} BUS_QUERY_ID_TYPE, *PBUS_QUERY_ID_TYPE;

/* Device Relation Type */
typedef enum _DEVICE_RELATION_TYPE
{
    BusRelations,
    EjectionRelations,
    PowerRelations,
    RemovalRelations,
    TargetDeviceRelation,
    SingleBusRelations,
    TransportRelations
} DEVICE_RELATION_TYPE, *PDEVICE_RELATION_TYPE;

/* Device Text Type */
typedef enum _DEVICE_TEXT_TYPE
{
    DeviceTextDescription,
    DeviceTextLocationInformation
} DEVICE_TEXT_TYPE, *PDEVICE_TEXT_TYPE;

/* Device Usage Notification Type */
typedef enum _DEVICE_USAGE_NOTIFICATION_TYPE
{
    DeviceUsageTypeUndefined,
    DeviceUsageTypePaging,
    DeviceUsageTypeHibernation,
    DeviceUsageTypeDumpFile,
    DeviceUsageTypeBoot,
    DeviceUsageTypePostDisplay,
    DeviceUsageTypeGuestAssigned,
} DEVICE_USAGE_NOTIFICATION_TYPE;

/* Directory Notify Information Class */
typedef enum _DIRECTORY_NOTIFY_INFORMATION_CLASS
{
    DirectoryNotifyInformation = 1,
    DirectoryNotifyExtendedInformation
} DIRECTORY_NOTIFY_INFORMATION_CLASS, *PDIRECTORY_NOTIFY_INFORMATION_CLASS;

/* File Information Class */
typedef enum _FILE_INFORMATION_CLASS
{
    FileDirectoryInformation = 1,
    FileFullDirectoryInformation,
    FileBothDirectoryInformation,
    FileBasicInformation,
    FileStandardInformation,
    FileInternalInformation,
    FileEaInformation,
    FileAccessInformation,
    FileNameInformation,
    FileRenameInformation,
    FileLinkInformation,
    FileNamesInformation,
    FileDispositionInformation,
    FilePositionInformation,
    FileFullEaInformation,
    FileModeInformation,
    FileAlignmentInformation,
    FileAllInformation,
    FileAllocationInformation,
    FileEndOfFileInformation,
    FileAlternateNameInformation,
    FileStreamInformation,
    FilePipeInformation,
    FilePipeLocalInformation,
    FilePipeRemoteInformation,
    FileMailslotQueryInformation,
    FileMailslotSetInformation,
    FileCompressionInformation,
    FileObjectIdInformation,
    FileCompletionInformation,
    FileMoveClusterInformation,
    FileQuotaInformation,
    FileReparsePointInformation,
    FileNetworkOpenInformation,
    FileAttributeTagInformation,
    FileTrackingInformation,
    FileIdBothDirectoryInformation,
    FileIdFullDirectoryInformation,
    FileValidDataLengthInformation,
    FileShortNameInformation,
    FileIoCompletionNotificationInformation,
    FileIoStatusBlockRangeInformation,
    FileIoPriorityHintInformation,
    FileSfioReserveInformation,
    FileSfioVolumeInformation,
    FileHardLinkInformation,
    FileProcessIdsUsingFileInformation,
    FileNormalizedNameInformation,
    FileNetworkPhysicalNameInformation,
    FileIdGlobalTxDirectoryInformation,
    FileIsRemoteDeviceInformation,
    FileAttributeCacheInformation,
    FileNumaNodeInformation,
    FileStandardLinkInformation,
    FileRemoteProtocolInformation,
    FileRenameInformationBypassAccessCheck,
    FileLinkInformationBypassAccessCheck,
    FileVolumeNameInformation,
    FileIdInformation,
    FileIdExtdDirectoryInformation,
    FileReplaceCompletionInformation,
    FileHardLinkFullIdInformation,
    FileIdExtdBothDirectoryInformation,
    FileDispositionInformationEx,
    FileRenameInformationEx ,
    FileRenameInformationExBypassAccessCheck,
    FileDesiredStorageClassInformation,
    FileStatInformation,
    FileMemoryPartitionInformation,
    FileStatLxInformation,
    FileCaseSensitiveInformation,
    FileLinkInformationEx,
    FileLinkInformationExBypassAccessCheck,
    FileStorageReserveIdInformation,
    FileCaseSensitiveInformationForceAccessCheck,
    FileKnownFolderInformation,
    FileMaximumInformation
} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

/* File System Information Class */
typedef enum _FS_INFORMATION_CLASS
{
    FileFsVolumeInformation = 1,
    FileFsLabelInformation,
    FileFsSizeInformation,
    FileFsDeviceInformation,
    FileFsAttributeInformation,
    FileFsControlInformation,
    FileFsFullSizeInformation,
    FileFsObjectIdInformation,
    FileFsDriverPathInformation,
    FileFsVolumeFlagsInformation,
    FileFsMaximumInformation
} FS_INFORMATION_CLASS, *PFS_INFORMATION_CLASS;

/* IO allocation actions */
typedef enum _IO_ALLOCATION_ACTION
{
    KeepObject = 1,
    DeallocateObject,
    DeallocateObjectKeepRegisters
} IO_ALLOCATION_ACTION, *PIO_ALLOCATION_ACTION;

/* Interrupt Request Device Policy */
typedef enum _IRQ_DEVICE_POLICY
{
    IrqPolicyMachineDefault = 0,
    IrqPolicyAllCloseProcessors,
    IrqPolicyOneCloseProcessor,
    IrqPolicyAllProcessorsInMachine,
    IrqPolicySpecifiedProcessors,
    IrqPolicySpreadMessagesAcrossAllProcessors
} IRQ_DEVICE_POLICY, *PIRQ_DEVICE_POLICY;

/* Interrupt Request Priority */
typedef enum _IRQ_PRIORITY
{
    IrqPriorityUndefined = 0,
    IrqPriorityLow,
    IrqPriorityNormal,
    IrqPriorityHigh
} IRQ_PRIORITY, *PIRQ_PRIORITY;

/* Wait context block structure definition */
typedef struct _WAIT_CONTEXT_BLOCK
{
    KDEVICE_QUEUE_ENTRY WaitQueueEntry;
    PDRIVER_CONTROL DeviceRoutine;
    PVOID DeviceContext;
    ULONG NumberOfMapRegisters;
    PVOID DeviceObject;
    PVOID CurrentIrp;
    PKDPC BufferChainingDpc;
} WAIT_CONTEXT_BLOCK, *PWAIT_CONTEXT_BLOCK;

/* Client ID structure */
typedef struct _CLIENT_ID
{
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

/* Device Capabilities structure definition */
typedef struct _DEVICE_CAPABILITIES
{
    USHORT Size;
    USHORT Version;
    ULONG DeviceD1:1;
    ULONG DeviceD2:1;
    ULONG LockSupported:1;
    ULONG EjectSupported:1;
    ULONG Removable:1;
    ULONG DockDevice:1;
    ULONG UniqueID:1;
    ULONG SilentInstall:1;
    ULONG RawDeviceOK:1;
    ULONG SurpriseRemovalOK:1;
    ULONG WakeFromD0:1;
    ULONG WakeFromD1:1;
    ULONG WakeFromD2:1;
    ULONG WakeFromD3:1;
    ULONG HardwareDisabled:1;
    ULONG NonDynamic:1;
    ULONG WarmEjectSupported:1;
    ULONG NoDisplayInUI:1;
    ULONG Reserved1:1;
    ULONG WakeFromInterrupt:1;
    ULONG SecureDevice:1;
    ULONG ChildOfVgaEnabledBridge:1;
    ULONG DecodeIoOnBoot:1;
    ULONG Reserved:9;
    ULONG Address;
    ULONG UINumber;
    DEVICE_POWER_STATE DeviceState[PowerSystemMaximum];
    SYSTEM_POWER_STATE SystemWake;
    DEVICE_POWER_STATE DeviceWake;
    ULONG D1Latency;
    ULONG D2Latency;
    ULONG D3Latency;
} DEVICE_CAPABILITIES, *PDEVICE_CAPABILITIES;

/* Device object structure definition */
typedef struct _DEVICE_OBJECT
{
    CSHORT Type;
    USHORT Size;
    LONG ReferenceCount;
    PDRIVER_OBJECT DriverObject;
    PDEVICE_OBJECT NextDevice;
    PDEVICE_OBJECT AttachedDevice;
    PIRP CurrentIrp;
    PIO_TIMER Timer;
    ULONG Flags;
    ULONG Characteristics;
    VOLATILE PVPB Vpb;
    PVOID DeviceExtension;
    DEVICE_TYPE DeviceType;
    CCHAR StackSize;
    union
    {
        LIST_ENTRY ListEntry;
        WAIT_CONTEXT_BLOCK Wcb;
    } Queue;
    ULONG AlignmentRequirement;
    KDEVICE_QUEUE DeviceQueue;
    KDPC Dpc;
    ULONG ActiveThreadCount;
    PSECURITY_DESCRIPTOR SecurityDescriptor;
    KEVENT DeviceLock;
    USHORT SectorSize;
    USHORT Spare1;
    PDEVOBJ_EXTENSION DeviceObjectExtension;
    PVOID Reserved;
} DEVICE_OBJECT, *PDEVICE_OBJECT;

/* Device object extension structure definition */
typedef struct _DEVOBJ_EXTENSION
{
    CSHORT Type;
    USHORT Size;
    PDEVICE_OBJECT DeviceObject;
} DEVOBJ_EXTENSION, *PDEVOBJ_EXTENSION;

/* Driver extension structure definition */
typedef struct _DRIVER_EXTENSION
{
    PDRIVER_OBJECT DriverObject;
    PDRIVER_ADD_DEVICE AddDevice;
    ULONG Count;
    UNICODE_STRING ServiceKeyName;
} DRIVER_EXTENSION, *PDRIVER_EXTENSION;

/* Driver object structure definition */
typedef struct _DRIVER_OBJECT
{
    CSHORT Type;
    CSHORT Size;
    PDEVICE_OBJECT DeviceObject;
    ULONG Flags;
    PVOID DriverStart;
    ULONG DriverSize;
    PVOID DriverSection;
    PDRIVER_EXTENSION DriverExtension;
    UNICODE_STRING DriverName;
    PUNICODE_STRING HardwareDatabase;
    PFAST_IO_DISPATCH FastIoDispatch;
    PDRIVER_INITIALIZE DriverInit;
    PDRIVER_STARTIO DriverStartIo;
    PDRIVER_UNLOAD DriverUnload;
    PDRIVER_DISPATCH MajorFunction[IRP_FC_MAXIMUM_FUNCTION + 1];
} DRIVER_OBJECT, *PDRIVER_OBJECT;

/* Fast I/O dispatch structure definition */
typedef struct _FAST_IO_DISPATCH
{
    ULONG SizeOfFastIoDispatch;
    PFAST_IO_CHECK_IF_POSSIBLE FastIoCheckIfPossible;
    PFAST_IO_READ FastIoRead;
    PFAST_IO_WRITE FastIoWrite;
    PFAST_IO_QUERY_BASIC_INFO FastIoQueryBasicInfo;
    PFAST_IO_QUERY_STANDARD_INFO FastIoQueryStandardInfo;
    PFAST_IO_LOCK FastIoLock;
    PFAST_IO_UNLOCK_SINGLE FastIoUnlockSingle;
    PFAST_IO_UNLOCK_ALL FastIoUnlockAll;
    PFAST_IO_UNLOCK_ALL_BY_KEY FastIoUnlockAllByKey;
    PFAST_IO_DEVICE_CONTROL FastIoDeviceControl;
    PFAST_IO_ACQUIRE_FILE AcquireFileForNtCreateSection;
    PFAST_IO_RELEASE_FILE ReleaseFileForNtCreateSection;
    PFAST_IO_DETACH_DEVICE FastIoDetachDevice;
    PFAST_IO_QUERY_NETWORK_OPEN_INFO FastIoQueryNetworkOpenInfo;
    PFAST_IO_ACQUIRE_FOR_MOD_WRITE AcquireForModWrite;
    PFAST_IO_MDL_READ MdlRead;
    PFAST_IO_MDL_READ_COMPLETE MdlReadComplete;
    PFAST_IO_PREPARE_MDL_WRITE PrepareMdlWrite;
    PFAST_IO_MDL_WRITE_COMPLETE MdlWriteComplete;
    PFAST_IO_READ_COMPRESSED FastIoReadCompressed;
    PFAST_IO_WRITE_COMPRESSED FastIoWriteCompressed;
    PFAST_IO_MDL_READ_COMPLETE_COMPRESSED MdlReadCompleteCompressed;
    PFAST_IO_MDL_WRITE_COMPLETE_COMPRESSED MdlWriteCompleteCompressed;
    PFAST_IO_QUERY_OPEN FastIoQueryOpen;
    PFAST_IO_RELEASE_FOR_MOD_WRITE ReleaseForModWrite;
    PFAST_IO_ACQUIRE_FOR_CCFLUSH AcquireForCcFlush;
    PFAST_IO_RELEASE_FOR_CCFLUSH ReleaseForCcFlush;
} FAST_IO_DISPATCH, *PFAST_IO_DISPATCH;

/* File information structure definition */
typedef struct _FILE_BASIC_INFORMATION
{
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    ULONG FileAttributes;
} FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION;

/* File quota information structure definition */
typedef struct _FILE_GET_QUOTA_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG SidLength;
    SID Sid;
} FILE_GET_QUOTA_INFORMATION, *PFILE_GET_QUOTA_INFORMATION;

/* File network open information structure definition */
typedef struct _FILE_NETWORK_OPEN_INFORMATION
{
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER AllocationSize;
    LARGE_INTEGER EndOfFile;
    ULONG FileAttributes;
} FILE_NETWORK_OPEN_INFORMATION, *PFILE_NETWORK_OPEN_INFORMATION;

/* File Object structure definition */
typedef struct _FILE_OBJECT
{
    CSHORT Type;
    CSHORT Size;
    PDEVICE_OBJECT DeviceObject;
    PVPB Vpb;
    PVOID FsContext;
    PVOID FsContext2;
    PSECTION_OBJECT_POINTERS SectionObjectPointer;
    PVOID PrivateCacheMap;
    XTSTATUS FinalStatus;
    PFILE_OBJECT RelatedFileObject;
    BOOLEAN LockOperation;
    BOOLEAN DeletePending;
    BOOLEAN ReadAccess;
    BOOLEAN WriteAccess;
    BOOLEAN DeleteAccess;
    BOOLEAN SharedRead;
    BOOLEAN SharedWrite;
    BOOLEAN SharedDelete;
    ULONG Flags;
    UNICODE_STRING FileName;
    LARGE_INTEGER CurrentByteOffset;
    VOLATILE ULONG Waiters;
    VOLATILE ULONG Busy;
    PVOID LastLock;
    KEVENT Lock;
    KEVENT Event;
    VOLATILE PIO_COMPLETION_CONTEXT CompletionContext;
    KSPIN_LOCK IrpListLock;
    LIST_ENTRY IrpList;
    VOLATILE PVOID FileObjectExtension;
} FILE_OBJECT, *PFILE_OBJECT;

/* File standard information structure definition */
typedef struct _FILE_STANDARD_INFORMATION
{
    LARGE_INTEGER AllocationSize;
    LARGE_INTEGER EndOfFile;
    ULONG NumberOfLinks;
    BOOLEAN DeletePending;
    BOOLEAN Directory;
} FILE_STANDARD_INFORMATION, *PFILE_STANDARD_INFORMATION;

/* Interface structure definition */
typedef struct _INTERFACE
{
    USHORT Size;
    USHORT Version;
    PVOID Context;
    PINTERFACE_REFERENCE InterfaceReference;
    PINTERFACE_DEREFERENCE InterfaceDereference;
} INTERFACE, *PINTERFACE;

/* IO completion context structure definition */
typedef struct _IO_COMPLETION_CONTEXT
{
    PVOID Port;
    PVOID Key;
} IO_COMPLETION_CONTEXT, *PIO_COMPLETION_CONTEXT;

/* I/O resource descriptor structure definition */
typedef struct _IO_RESOURCE_DESCRIPTOR
{
    UCHAR Option;
    UCHAR Type;
    UCHAR ShareDisposition;
    UCHAR Spare1;
    USHORT Flags;
    USHORT Spare2;
    union
    {
        struct
        {
            ULONG Length;
            ULONG Alignment;
            PHYSICAL_ADDRESS MinimumAddress;
            PHYSICAL_ADDRESS MaximumAddress;
        } Port;
        struct
        {
            ULONG Length;
            ULONG Alignment;
            PHYSICAL_ADDRESS MinimumAddress;
            PHYSICAL_ADDRESS MaximumAddress;
        } Memory;
        struct
        {
            ULONG MinimumVector;
            ULONG MaximumVector;
            IRQ_DEVICE_POLICY AffinityPolicy;
            USHORT Group;
            IRQ_PRIORITY PriorityPolicy;
            PKAFFINITY_MAP TargetedProcessors;
        } Interrupt;
        struct
        {
            ULONG MinimumChannel;
            ULONG MaximumChannel;
        } Dma;
        struct
        {
            ULONG Length;
            ULONG Alignment;
            PHYSICAL_ADDRESS MinimumAddress;
            PHYSICAL_ADDRESS MaximumAddress;
        } Generic;
        struct
        {
            ULONG Data[3];
        } DevicePrivate;
        struct
        {
            ULONG Length;
            ULONG MinBusNumber;
            ULONG MaxBusNumber;
            ULONG Reserved;
        } BusNumber;
        struct
        {
            ULONG Priority;
            ULONG Reserved1;
            ULONG Reserved2;
        } ConfigData;
    } u;
} IO_RESOURCE_DESCRIPTOR, *PIO_RESOURCE_DESCRIPTOR;

/* I/O resource list structure definition */
typedef struct _IO_RESOURCE_LIST
{
    USHORT Version;
    USHORT Revision;
    ULONG Count;
    IO_RESOURCE_DESCRIPTOR Descriptors[1];
} IO_RESOURCE_LIST, *PIO_RESOURCE_LIST;

/* I/O resource requirements list structure definition */
typedef struct _IO_RESOURCE_REQUIREMENTS_LIST
{
    ULONG ListSize;
    INTERFACE_TYPE InterfaceType;
    ULONG BusNumber;
    ULONG SlotNumber;
    ULONG Reserved[3];
    ULONG AlternativeLists;
    IO_RESOURCE_LIST List[1];
} IO_RESOURCE_REQUIREMENTS_LIST, *PIO_RESOURCE_REQUIREMENTS_LIST;

/* I/O security context structure definition */
typedef struct _IO_SECURITY_CONTEXT
{
    PSECURITY_QUALITY_OF_SERVICE SecurityQos;
    PACCESS_STATE AccessState;
    ACCESS_MASK DesiredAccess;
    ULONG FullCreateOptions;
} IO_SECURITY_CONTEXT, *PIO_SECURITY_CONTEXT;

/* I/O stack location structure definition */
typedef struct _IO_STACK_LOCATION
{
    UCHAR MajorFunction;
    UCHAR MinorFunction;
    UCHAR Flags;
    UCHAR Control;
    union
    {
        struct
        {
            PIO_SECURITY_CONTEXT SecurityContext;
            ULONG Options;
            USHORT FileAttributes;
            USHORT ShareAccess;
            ULONG EaLength;
        } Create;
        struct
        {
            PIO_SECURITY_CONTEXT SecurityContext;
            ULONG Options;
            USHORT Reserved;
            USHORT ShareAccess;
            PNAMED_PIPE_CREATE_PARAMETERS Parameters;
        } CreatePipe;
        struct
        {
            PIO_SECURITY_CONTEXT SecurityContext;
            ULONG Options;
            USHORT Reserved;
            USHORT ShareAccess;
            PMAILSLOT_CREATE_PARAMETERS Parameters;
        } CreateMailslot;
        struct
        {
            ULONG Length;
            ULONG Key;
            LARGE_INTEGER ByteOffset;
        } Read;
        struct
        {
            ULONG Length;
            ULONG Key;
            LARGE_INTEGER ByteOffset;
        } Write;
        struct
        {
            ULONG Length;
            PUNICODE_STRING FileName;
            FILE_INFORMATION_CLASS FileInformationClass;
            ULONG FileIndex;
        } QueryDirectory;
        struct
        {
            ULONG Length;
            ULONG CompletionFilter;
        } NotifyDirectory;
        struct
        {
            ULONG Length;
            ULONG CompletionFilter;
            DIRECTORY_NOTIFY_INFORMATION_CLASS DirectoryNotifyInformationClass;
        } NotifyDirectoryEx;
        struct
        {
            ULONG Length;
            FILE_INFORMATION_CLASS FileInformationClass;
        } QueryFile;
        struct
        {
            ULONG Length;
            FILE_INFORMATION_CLASS FileInformationClass;
            PFILE_OBJECT FileObject;
            union
            {
                struct
                {
                    BOOLEAN ReplaceIfExists;
                    BOOLEAN AdvanceOnly;
                };
                ULONG ClusterCount;
                HANDLE DeleteHandle;
            };
        } SetFile;
        struct
        {
            ULONG Length;
            PVOID EaList;
            ULONG EaListLength;
            ULONG EaIndex;
        } QueryEa;
        struct
        {
            ULONG Length;
        } SetEa;
        struct
        {
            ULONG Length;
            FS_INFORMATION_CLASS FsInformationClass;
        } QueryVolume;
        struct
        {
            ULONG Length;
            FS_INFORMATION_CLASS FsInformationClass;
        } SetVolume;
        struct
        {
            ULONG OutputBufferLength;
            ULONG InputBufferLength;
            ULONG FsControlCode;
            PVOID Type3InputBuffer;
        } FileSystemControl;
        struct
        {
            PLARGE_INTEGER Length;
            ULONG Key;
            LARGE_INTEGER ByteOffset;
        } LockControl;
        struct
        {
            ULONG OutputBufferLength;
            ULONG InputBufferLength;
            ULONG IoControlCode;
            PVOID Type3InputBuffer;
        } DeviceIoControl;
        struct
        {
            SECURITY_INFORMATION SecurityInformation;
            ULONG Length;
        } QuerySecurity;
        struct
        {
            SECURITY_INFORMATION SecurityInformation;
            PSECURITY_DESCRIPTOR SecurityDescriptor;
        } SetSecurity;
        struct
        {
            PVPB Vpb;
            PDEVICE_OBJECT DeviceObject;
        } MountVolume;
        struct
        {
            PVPB Vpb;
            PDEVICE_OBJECT DeviceObject;
        } VerifyVolume;
        struct
        {
            PSCSI_REQUEST_BLOCK Srb;
        } Scsi;
        struct
        {
            ULONG Length;
            PSID StartSid;
            PFILE_GET_QUOTA_INFORMATION SidList;
            ULONG SidListLength;
        } QueryQuota;
        struct
        {
            ULONG Length;
        } SetQuota;
        struct
        {
            DEVICE_RELATION_TYPE Type;
        } QueryDeviceRelations;
        struct
        {
            CONST GUID *InterfaceType;
            USHORT Size;
            USHORT Version;
            PINTERFACE Interface;
            PVOID InterfaceSpecificData;
        } QueryInterface;
        struct
        {
            PDEVICE_CAPABILITIES Capabilities;
        } DeviceCapabilities;
        struct
        {
            PIO_RESOURCE_REQUIREMENTS_LIST IoResourceRequirementList;
        } FilterResourceRequirements;
        struct
        {
            ULONG WhichSpace;
            PVOID Buffer;
            ULONG Offset;
            ULONG Length;
        } ReadWriteConfig;
        struct
        {
            BOOLEAN Lock;
        } SetLock;
        struct
        {
            BUS_QUERY_ID_TYPE IdType;
        } QueryId;
        struct
        {
            DEVICE_TEXT_TYPE DeviceTextType;
            LCID LocaleId;
        } QueryDeviceText;
        struct
        {
            BOOLEAN InPath;
            BOOLEAN Reserved[3];
            DEVICE_USAGE_NOTIFICATION_TYPE Type;
        } UsageNotification;
        struct
        {
            SYSTEM_POWER_STATE PowerState;
        } WaitWake;
        struct
        {
            PPOWER_SEQUENCE PowerSequence;
        } PowerSequence;
        struct
        {
            union
            {
                ULONG SystemContext;
                SYSTEM_POWER_STATE_CONTEXT SystemPowerStateContext;
            };
            POWER_STATE_TYPE Type;
            POWER_STATE State;
            POWER_ACTION ShutdownType;
        } Power;
        struct
        {
            PCM_RESOURCE_LIST AllocatedResources;
            PCM_RESOURCE_LIST AllocatedResourcesTranslated;
        } StartDevice;
        struct
        {
            ULONG_PTR ProviderId;
            PVOID DataPath;
            ULONG BufferSize;
            PVOID Buffer;
        } WMI;
        struct
        {
            PVOID Argument1;
            PVOID Argument2;
            PVOID Argument3;
            PVOID Argument4;
        } Others;
    } Parameters;
    PDEVICE_OBJECT DeviceObject;
    PFILE_OBJECT FileObject;
    PIO_COMPLETION_ROUTINE CompletionRoutine;
    PVOID Context;
} IO_STACK_LOCATION, *PIO_STACK_LOCATION;

/* IO status block structure definition */
typedef struct _IO_STATUS_BLOCK
{
    union
    {
      XTSTATUS Status;
      PVOID Pointer;
    };
    ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

/* IO timer structure definition */
typedef struct _IO_TIMER
{
    USHORT Type;
    USHORT TimerEnabled;
    LIST_ENTRY IoTimerList;
    PIO_TIMER_ROUTINE TimerRoutine;
    PVOID Context;
    PDEVICE_OBJECT DeviceObject;
} IO_TIMER, *PIO_TIMER;

/* IRP, czyli I/O Request Packet structure definition */
typedef struct _IRP
{
    CSHORT Type;
    USHORT Size;
    PMDL MdlAddress;
    ULONG Flags;
    union
    {
        PIRP MasterIrp;
        VOLATILE LONG IrpCount;
        PVOID SystemBuffer;
    } AssociatedIrp;
    LIST_ENTRY ThreadListEntry;
    IO_STATUS_BLOCK IoStatus;
    KPROCESSOR_MODE RequestorMode;
    BOOLEAN PendingReturned;
    CHAR StackCount;
    CHAR CurrentLocation;
    BOOLEAN Cancel;
    KRUNLEVEL CancelIrql;
    CCHAR ApcEnvironment;
    UCHAR AllocationFlags;
    PIO_STATUS_BLOCK UserIosb;
    PKEVENT UserEvent;
    union
    {
        struct
        {
            union
            {
                PIO_APC_ROUTINE UserApcRoutine;
                PVOID IssuingProcess;
            };
            PVOID UserApcContext;
        } AsynchronousParameters;
        LARGE_INTEGER AllocationSize;
    } Overlay;
    VOLATILE PDRIVER_CANCEL CancelRoutine;
    PVOID UserBuffer;
    union
    {
        struct
        {
            union
            {
                KDEVICE_QUEUE_ENTRY DeviceQueueEntry;
                struct
                {
                    PVOID DriverContext[4];
                };
            };
            PETHREAD Thread;
            PCHAR AuxiliaryBuffer;
            struct
            {
                LIST_ENTRY ListEntry;
                union
                {
                    PIO_STACK_LOCATION CurrentStackLocation;
                    ULONG PacketType;
                };
            };
            PFILE_OBJECT OriginalFileObject;
        } Overlay;
        KAPC Apc;
        PVOID CompletionKey;
    } Tail;
} IRP, *PIRP;

/* Mailslot create parameters structure definition */
typedef struct _MAILSLOT_CREATE_PARAMETERS
{
    ULONG MailslotQuota;
    ULONG MaximumMessageSize;
    LARGE_INTEGER ReadTimeout;
    BOOLEAN TimeoutSpecified;
} MAILSLOT_CREATE_PARAMETERS, *PMAILSLOT_CREATE_PARAMETERS;

/* Named pipe create parameters structure definition */
typedef struct _NAMED_PIPE_CREATE_PARAMETERS
{
    ULONG NamedPipeType;
    ULONG ReadMode;
    ULONG CompletionMode;
    ULONG MaximumInstances;
    ULONG InboundQuota;
    ULONG OutboundQuota;
    LARGE_INTEGER DefaultTimeout;
    BOOLEAN TimeoutSpecified;
} NAMED_PIPE_CREATE_PARAMETERS, *PNAMED_PIPE_CREATE_PARAMETERS;

/* PCI bridge control registers */
typedef struct _PCI_BRIDGE_CONTROL_REGISTER
{
    UINT Bar[2];
    UCHAR PrimaryBus;
    UCHAR SecondaryBus;
    UCHAR SubordinateBus;
    UCHAR SecondaryLatencyTimer;
    UCHAR IoBase;
    UCHAR IoLimit;
    USHORT SecondaryStatus;
    USHORT MemoryBase;
    USHORT MemoryLimit;
    USHORT PrefetchableMemoryBase;
    USHORT PrefetchableMemoryLimit;
    UINT PrefetchableBaseUpper32;
    UINT PrefetchableLimitUpper32;
    USHORT IoBaseUpper16;
    USHORT IoLimitUpper16;
    UINT Reserved;
    UINT ExpansionRomBAR;
    UCHAR InterruptLine;
    UCHAR InterruptPin;
    USHORT BridgeControl;
} PCI_BRIDGE_CONTROL_REGISTER, *PPCI_BRIDGE_CONTROL_REGISTER;

/* PCI and PCI-E common header structure */
typedef struct _PCI_COMMON_HEADER
{
    USHORT VendorId;
    USHORT DeviceId;
    USHORT Command;
    USHORT Status;
    UCHAR RevisionId;
    UCHAR ProgIf;
    UCHAR SubClass;
    UCHAR BaseClass;
    UCHAR CacheLineSize;
    UCHAR LatencyTimer;
    UCHAR HeaderType;
    UCHAR BIST;
    union
    {
        struct
        {
            ULONG BaseAddresses[PCI_TYPE0_ADDRESSES];
            ULONG CIS;
            USHORT SubVendorId;
            USHORT SubSystemId;
            ULONG ROMBaseAddress;
            UCHAR CapabilitiesPtr;
            UCHAR Reserved1[3];
            ULONG Reserved2;
            UCHAR InterruptLine;
            UCHAR InterruptPin;
            UCHAR MinimumGrant;
            UCHAR MaximumLatency;
        } type0;
        struct
        {
            ULONG BaseAddresses[PCI_TYPE1_ADDRESSES];
            UCHAR PrimaryBus;
            UCHAR SecondaryBus;
            UCHAR SubordinateBus;
            UCHAR SecondaryLatency;
            UCHAR IOBase;
            UCHAR IOLimit;
            USHORT SecondaryStatus;
            USHORT MemoryBase;
            USHORT MemoryLimit;
            USHORT PrefetchBase;
            USHORT PrefetchLimit;
            ULONG PrefetchBaseUpper32;
            ULONG PrefetchLimitUpper32;
            USHORT IOBaseUpper16;
            USHORT IOLimitUpper16;
            UCHAR CapabilitiesPtr;
            UCHAR Reserved1[3];
            ULONG ROMBaseAddress;
            UCHAR InterruptLine;
            UCHAR InterruptPin;
            USHORT BridgeControl;
        } type1;
        struct
        {
            ULONG SocketRegistersBaseAddress;
            UCHAR CapabilitiesPtr;
            UCHAR Reserved;
            USHORT SecondaryStatus;
            UCHAR PrimaryBus;
            UCHAR SecondaryBus;
            UCHAR SubordinateBus;
            UCHAR SecondaryLatency;
            struct
            {
                ULONG Base;
                ULONG Limit;
            } Range[PCI_TYPE2_ADDRESSES - 1];
            UCHAR InterruptLine;
            UCHAR InterruptPin;
            USHORT BridgeControl;
        } type2;
    } u;
} PCI_COMMON_HEADER, *PPCI_COMMON_HEADER;

/* PCI and PCI-E common config structure */
typedef struct _PCI_COMMON_CONFIG
{
    PCI_COMMON_HEADER PciHeader;
    UCHAR DeviceSpecific[192];
} PCI_COMMON_CONFIG, *PPCI_COMMON_CONFIG;

/* PCI device header type region structure */
typedef struct _PCI_DEVICE_HEADER_TYPE_REGION
{
    UINT Bar[6];
    UINT CISPtr;
    USHORT SubsystemVendorID;
    USHORT SubsystemID;
    UINT ExpansionRomBar;
    UINT Reserved[2];
    UCHAR InterruptLine;
    UCHAR InterruptPin;
    UCHAR MinGnt;
    UCHAR MaxLat;
} PCI_DEVICE_HEADER_TYPE_REGION, *PPCI_DEVICE_HEADER_TYPE_REGION;

/* PCI device independent region structure */
typedef struct _PCI_DEVICE_INDEPENDENT_REGION
{
    USHORT VendorId;
    USHORT DeviceId;
    USHORT Command;
    USHORT Status;
    UCHAR RevisionID;
    UCHAR ClassCode[3];
    UCHAR CacheLineSize;
    UCHAR LaytencyTimer;
    UCHAR HeaderType;
    UCHAR BIST;
} PCI_DEVICE_INDEPENDENT_REGION, *PPCI_DEVICE_INDEPENDENT_REGION;

/* PCI device type 0 structure */
typedef struct _PCI_TYPE0_DEVICE
{
    PCI_DEVICE_INDEPENDENT_REGION Hdr;
    PCI_DEVICE_HEADER_TYPE_REGION Device;
} PCI_TYPE0_DEVICE, *PPCI_TYPE0_DEVICE;

/* PCI device type 1 structure */
typedef struct _PCI_TYPE1_DEVICE
{
    PCI_DEVICE_INDEPENDENT_REGION Hdr;
    PCI_BRIDGE_CONTROL_REGISTER Bridge;
} PCI_TYPE1_DEVICE, *PPCI_TYPE1_DEVICE;

/* Power sequence structure definition */
typedef struct _POWER_SEQUENCE
{
    ULONG SequenceD1;
    ULONG SequenceD2;
    ULONG SequenceD3;
} POWER_SEQUENCE, *PPOWER_SEQUENCE;

/* SCSI request block structure definition */
typedef struct _SCSI_REQUEST_BLOCK
{
    USHORT Length;
    UCHAR Function;
    UCHAR SrbStatus;
    UCHAR ScsiStatus;
    UCHAR PathId;
    UCHAR TargetId;
    UCHAR Lun;
    UCHAR QueueTag;
    UCHAR QueueAction;
    UCHAR CdbLength;
    UCHAR SenseInfoBufferLength;
    ULONG SrbFlags;
    ULONG DataTransferLength;
    ULONG TimeOutValue;
    PVOID DataBuffer;
    PVOID SenseInfoBuffer;
    PSCSI_REQUEST_BLOCK NextSrb;
    PVOID OriginalRequest;
    PVOID SrbExtension;
    union
    {
        ULONG InternalStatus;
        ULONG QueueSortKey;
        ULONG LinkTimeoutValue;
    };
    UCHAR Cdb[16];
} SCSI_REQUEST_BLOCK, *PSCSI_REQUEST_BLOCK;

/* Section object pointers structure definition */
typedef struct _SECTION_OBJECT_POINTERS
{
    PVOID DataSectionObject;
    PVOID SharedCacheMap;
    PVOID ImageSectionObject;
} SECTION_OBJECT_POINTERS, *PSECTION_OBJECT_POINTERS;

/* Volume Parameter Block structure definition */
typedef struct _VPB
{
    CSHORT Type;
    CSHORT Size;
    USHORT Flags;
    USHORT VolumeLabelLength;
    PDEVICE_OBJECT DeviceObject;
    PDEVICE_OBJECT RealDevice;
    ULONG SerialNumber;
    ULONG ReferenceCount;
    WCHAR VolumeLabel[IO_MAXIMUM_VOLUME_LABEL_LENGTH / sizeof(WCHAR)];
} VPB, *PVPB;

#endif /* __XTOS_ASSEMBLER__ */
#endif /* __XTDK_IOTYPES_H */
