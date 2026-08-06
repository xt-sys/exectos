/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/obtypes.h
 * DESCRIPTION:     Object Manager structures definitions
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTDK_OBTYPES_H
#define __XTDK_OBTYPES_H

#include <xtbase.h>
#include <xttypes.h>
#include <xtstruct.h>
#include <iotypes.h>


/* Object header information flag masks */
#define OBJECT_FLAG_NEW_OBJECT                                  0x01
#define OBJECT_FLAG_KERNEL_MODE                                 0x02
#define OBJECT_FLAG_CREATOR_INFO                                0x04
#define OBJECT_FLAG_EXCLUSIVE                                   0x08
#define OBJECT_FLAG_PERMANENT                                   0x10
#define OBJECT_FLAG_SECURITY_QUOTA                              0x20
#define OBJECT_FLAG_SINGLE_PROCESS                              0x40
#define OBJECT_FLAG_DEFER_DELETE                                0x80

/* Object lock states */
#define OBJECT_LOCK_STATE_WAIT_EXCLUSIVE                        0xAAAA1234
#define OBJECT_LOCK_WAITSHARED_SIGNATURE                        0xBBBB1234
#define OBJECT_LOCK_STATE_OWNED_EXCLUSIVE                       0xCCCC1234
#define OBJECT_LOCK_OWNEDSHARED_SIGNATURE                       0xDDDD1234
#define OBJECT_LOCK_STATE_RELEASED_SIGNATURE                    0xEEEE1234
#define OBJECT_LOCK_STATE_INITIALIZED                           0xFFFF1234

/* Object name attribute flags */
#define OBJECT_PROTECT_CLOSE                                    0x00000001L
#define OBJECT_INHERIT                                          0x00000002L
#define OBJECT_AUDIT_OBJECT_CLOSE                               0x00000004L
#define OBJECT_PERMANENT                                        0x00000010L
#define OBJECT_EXCLUSIVE                                        0x00000020L
#define OBJECT_CASE_INSENSITIVE                                 0x00000040L
#define OBJECT_OPENIF                                           0x00000080L
#define OBJECT_OPENLINK                                         0x00000100L
#define OBJECT_KERNEL_HANDLE                                    0x00000200L
#define OBJECT_FORCE_ACCESS_CHECK                               0x00000400L
#define OBJECT_VALID_ATTRIBUTES                                 0x000007F2L

/* Object handle attributes */
#define OBJECT_HANDLE_ATTRIBUTES                                (OBJECT_PROTECT_CLOSE | \
                                                                 OBJECT_INHERIT | \
                                                                 OBJECT_AUDIT_OBJECT_CLOSE)

/* Defines maximum number of handles */
#define OBJECT_MAX_HANDLES                                      (1<<24)

/* Handle Table Architecture Dimensions */
#define OBJECT_HANDLE_HIGHLEVEL_COUNT                           OBJECT_MAX_HANDLES / (OBJECT_HANDLE_LOWLEVEL_COUNT * OBJECT_HANDLE_MEDIUMLEVEL_COUNT)
#define OBJECT_HANDLE_MEDIUMLEVEL_COUNT                         (MM_PAGE_SIZE / sizeof(PHANDLE_TABLE_ENTRY))
#define OBJECT_HANDLE_LOWLEVEL_COUNT                            (MM_PAGE_SIZE / sizeof(HANDLE_TABLE_ENTRY))
#define OBJECT_HANDLE_HIGHLEVEL_SIZE                            (OBJECT_HANDLE_HIGHLEVEL_COUNT * sizeof(PHANDLE_TABLE_ENTRY))
#define OBJECT_HANDLE_MEDIUMLEVEL_THRESHOLD                     (OBJECT_HANDLE_MEDIUMLEVEL_COUNT * OBJECT_HANDLE_LOWLEVEL_COUNT)

/* Handle Value and Table Routing Constants */
#define OBJECT_HANDLE_LEVEL_CODE_MASK                           3
#define OBJECT_HANDLE_VALUE_INCREMENT                           4

/* Handle free mask */
#define OBJECT_FREE_HANDLE_MASK                                 0xFFFFFFFF

/* Handle table entry lock bit */
#define OBJECT_HANDLE_TABLE_ENTRY_LOCK_BIT                      1

/* Handle additional information signature */
#define OBJECT_HANDLE_ADDITIONAL_INFO_SIGNATURE                 (-2)

/* Handle tracing constants */
#define OBJECT_HANDLE_TRACE_DB_STACK_SIZE                       8

/* Object protect-on-close access bit */
#define OBJECT_ACCESS_PROTECT_CLOSE_BIT                         0x02000000L

/* Maximum number of defined object types */
#define OBJECT_MAX_DEFINED_OBJECT_TYPES                         48

/* Object name buffer size */
#define OBJECT_NAME_BUFFER_SIZE                                 248

/* Object name lock flags */
#define OBJECT_NAME_LOCKED                                      ((LONG)0x80000000)
#define OBJECT_REMOVE_QUEUE_LOCKED                              (PVOID)1

/* Object name path separator */
#define OBJECT_NAME_PATH_SEPARATOR                              ((WCHAR)L'\\')

/* Kernel mode object handle mask */
#define OBJECT_HANDLE_KERNEL_MASK                               ((ULONG_PTR)((LONG)0x80000000))

/* Number of hash buckets */
#define OBJECT_NUMBER_HASH_BUCKETS                              37


/* C/C++ specific code */
#ifndef __XTOS_ASSEMBLER__

/* Object Manager routine callbacks */
typedef VOID (XTAPI *POB_DUMP_METHOD)(IN PVOID Object, IN POBJECT_DUMP_CONTROL Control);
typedef XTSTATUS (XTAPI *POB_OPEN_METHOD)(IN OBJECT_OPEN_REASON Reason, IN PEPROCESS Process, IN PVOID ObjectBody, IN ACCESS_MASK GrantedAccess, IN ULONG HandleCount);
typedef VOID (XTAPI *POB_CLOSE_METHOD)(IN PEPROCESS Process, IN PVOID Object, IN ACCESS_MASK GrantedAccess, IN ULONG ProcessHandleCount, IN ULONG SystemHandleCount);
typedef VOID (XTAPI *POB_DELETE_METHOD)(IN PVOID Object);
typedef XTSTATUS (XTAPI *POB_PARSE_METHOD)(IN PVOID ParseObject, IN PVOID ObjectType, IN OUT PACCESS_STATE AccessState, IN KPROCESSOR_MODE ProcessorMode, IN ULONG Attributes, IN OUT PUNICODE_STRING CompleteName, IN OUT PUNICODE_STRING RemainingName, IN OUT PVOID Context, IN PSECURITY_QUALITY_OF_SERVICE SecurityQos, OUT PVOID *Object);
typedef XTSTATUS (XTAPI *POB_SECURITY_METHOD)(IN PVOID Object, IN SECURITY_OPERATION_CODE OperationType, IN PSECURITY_INFORMATION SecurityInformation, IN PSECURITY_DESCRIPTOR SecurityDescriptor, IN OUT PULONG CapturedLength, IN OUT PSECURITY_DESCRIPTOR *ObjectSecurityDescriptor, IN MMPOOL_TYPE PoolType, IN PGENERIC_MAPPING GenericMapping);
typedef XTSTATUS (XTAPI *POB_QUERYNAME_METHOD)(IN PVOID Object, IN BOOLEAN HasObjectName, OUT POBJECT_NAME_INFORMATION ObjectNameInfo, IN ULONG Length, OUT PULONG ReturnLength, IN KPROCESSOR_MODE ProcessorMode);
typedef BOOLEAN (XTAPI *POB_OKAYTOCLOSE_METHOD)(IN PEPROCESS Process, IN PVOID Object, IN HANDLE Handle, IN KPROCESSOR_MODE ProcessorMode);

/* Bitmasks used to identify the presence of optional object headers in memory */
typedef enum _OBJECT_HEADER_INFO
{
    ObjectHeaderInfoNone = 0,
    ObjectHeaderInfoCreatorInfo = 0x01,
    ObjectHeaderInfoNameInfo = 0x02,
    ObjectHeaderInfoHandleInfo = 0x04,
    ObjectHeaderInfoQuotaInfo = 0x08,
    ObjectHeaderInfoProcessInfo = 0x10
} OBJECT_HEADER_INFO, *POBJECT_HEADER_INFO;

/* List of information classes used when querying or setting object attributes */
typedef enum _OBJECT_INFORMATION_CLASS
{
    ObjectBasicInfo,
    ObjectNameInfo,
    ObjectTypeInfo,
    ObjectAllInfo,
    ObjectHandleFlagInfo
} OBJECT_INFORMATION_CLASS, *POBJECT_INFORMATION_CLASS;

/* List of valid reasons for creating, opening, or duplicating an object handle */
typedef enum _OBJECT_OPEN_REASON
{
    ObCreateHandle,
    ObOpenHandle,
    ObDuplicateHandle,
    ObInheritHandle,
    ObMaxOpenReason
} OBJECT_OPEN_REASON, *POBJECT_OPEN_REASON;

/* Mapping of device names to object manager devices */
typedef struct _DEVICE_MAP
{
    POBJECT_DIRECTORY DevicesDirectory;
    POBJECT_DIRECTORY GlobalDevicesDirectory;
    ULONG ReferenceCount;
    ULONG DriveMap;
    UCHAR DriveType[32];
} DEVICE_MAP, *PDEVICE_MAP;

/* Handle table entry structure definition */
typedef struct _HANDLE_TABLE_ENTRY
{
    union
    {
        PVOID Object;
        ULONG_PTR ObAttributes;
        PHANDLE_TABLE_ENTRY_INFO InfoTable;
        ULONG_PTR Value;
    };
    union
    {
        ULONG GrantedAccess;
        struct
        {
            USHORT GrantedAccessIndex;
            USHORT CreatorBackTraceIndex;
        };
        LONG NextFreeTableEntry;
    };
} HANDLE_TABLE_ENTRY, *PHANDLE_TABLE_ENTRY;

/* Handle table entry info structure definition */
typedef struct _HANDLE_TABLE_ENTRY_INFO
{
    ULONG AuditMask;
} HANDLE_TABLE_ENTRY_INFO, *PHANDLE_TABLE_ENTRY_INFO;

/* Handle table structure definition */
typedef struct _HANDLE_TABLE
{
    ULONG_PTR TableCode;
    PEPROCESS QuotaProcess;
    PVOID UniqueProcessId;
    KPUSH_LOCK HandleTableLock[4];
    LIST_ENTRY HandleTableList;
    KPUSH_LOCK HandleContentionEvent;
    PVOID Reserved;
    LONG ExtraInfoPages;
    union
    {
        ULONG Flags;
        UCHAR StrictFIFO:1;
    };
    PHANDLE_TABLE_ENTRY FirstFreeHandle;
    PHANDLE_TABLE_ENTRY LastFreeHandle;
    LONG HandleCount;
    ULONG NextHandleNeedingPool;
} HANDLE_TABLE, *PHANDLE_TABLE;

/* Parameters provided by a caller when creating or opening an object */
typedef struct _OBJECT_ATTRIBUTES
{
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PSECURITY_DESCRIPTOR SecurityDescriptor;
    PSECURITY_QUALITY_OF_SERVICE SecurityQualityOfService;
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

/* Internal data stored during object creation */
typedef struct _OBJECT_CREATE_INFORMATION
{
    ULONG Attributes;
    HANDLE RootDirectory;
    PVOID ParseContext;
    KPROCESSOR_MODE ProbeMode;
    ULONG PagedPoolCharge;
    ULONG NonPagedPoolCharge;
    ULONG SecurityDescriptorCharge;
    PSECURITY_DESCRIPTOR SecurityDescriptor;
    PSECURITY_QUALITY_OF_SERVICE SecurityQos;
    SECURITY_QUALITY_OF_SERVICE SecurityQualityOfService;
} OBJECT_CREATE_INFORMATION, *POBJECT_CREATE_INFORMATION;

/* Optional header storing information about the object's creator */
typedef struct _OBJECT_CREATOR_INFO
{
    LIST_ENTRY TypeList;
    HANDLE CreatorProcessId;
} OBJECT_CREATOR_INFO, *POBJECT_CREATOR_INFO;

/* Directory object used to structure the namespace hierarchy */
typedef struct _OBJECT_DIRECTORY
{
    POBJECT_DIRECTORY_ENTRY HashBuckets[OBJECT_NUMBER_HASH_BUCKETS];
    KPUSH_LOCK Lock;
    PDEVICE_MAP DeviceMap;
    ULONG SessionId;
} OBJECT_DIRECTORY, *POBJECT_DIRECTORY;

/* Linked list entry representing an object within a directory bucket */
typedef struct _OBJECT_DIRECTORY_ENTRY
{
    POBJECT_DIRECTORY_ENTRY ChainLink;
    PVOID Object;
    ULONG HashValue;
} OBJECT_DIRECTORY_ENTRY, *POBJECT_DIRECTORY_ENTRY;

/* Control structure used during object diagnostic dumps */
typedef struct _OBJECT_DUMP_CONTROL
{
    PVOID Stream;
    ULONG Detail;
} OBJECT_DUMP_CONTROL, *POBJECT_DUMP_CONTROL;

/* Handle table interface union definition */
typedef union _OBJECT_HANDLE
{
    struct
    {
        ULONG TagBits:2;
        ULONG Index:30;
    };
    HANDLE GenericHandleOverlay;
    ULONG_PTR Value;
} OBJECT_HANDLE, *POBJECT_HANDLE;

/* Entry tracking the number of open handles a specific process holds */
typedef struct _OBJECT_HANDLE_COUNT_ENTRY
{
    PEPROCESS OwningProcess;
    ULONG HandleTableIndex;
    ULONG HandleCount;
} OBJECT_HANDLE_COUNT_ENTRY, *POBJECT_HANDLE_COUNT_ENTRY;

/* Database array tracking handle counts for objects opened by processes */
typedef struct _OBJECT_HANDLE_COUNT_DATABASE
{
    ULONG CountEntries;
    OBJECT_HANDLE_COUNT_ENTRY HandleCountEntries[1];
} OBJECT_HANDLE_COUNT_DATABASE, *POBJECT_HANDLE_COUNT_DATABASE;

/* Database tracking all handle counts across different processes */
typedef struct _OBJECT_HANDLE_COUNT_INFORMATION
{
    ULONG TotalHandleCount;
    ULONG EntryCount;
    POBJECT_HANDLE_COUNT_ENTRY Entries;
} OBJECT_HANDLE_COUNT_INFORMATION, *POBJECT_HANDLE_COUNT_INFORMATION;

/* Optional header containing the handle count database */
typedef struct _OBJECT_HANDLE_INFO
{
    OBJECT_HANDLE_COUNT_INFORMATION HandleCounts;
} OBJECT_HANDLE_INFO, *POBJECT_HANDLE_INFO;

/* Object Handle Information */
typedef struct _OBJECT_HANDLE_INFORMATION
{
    ULONG HandleAttributes;
    ACCESS_MASK GrantedAccess;
} OBJECT_HANDLE_INFORMATION, *POBJECT_HANDLE_INFORMATION;

/* Core object header */
typedef struct _OBJECT_HEADER
{
    LONG_PTR PointerCount;
    union
    {
        LONG_PTR HandleCount;
        VOLATILE PVOID NextToFree;
    };
    POBJECT_TYPE Type;
    UCHAR NameInfoOffset;
    UCHAR HandleInfoOffset;
    UCHAR QuotaInfoOffset;
    UCHAR Flags;
    union
    {
        POBJECT_CREATE_INFORMATION ObjectCreateInfo;
        PVOID QuotaBlockCharged;
    };
    PSECURITY_DESCRIPTOR SecurityDescriptor;
    QUAD Body;
} OBJECT_HEADER, *POBJECT_HEADER;

/* Creator tracking information */
typedef struct _OBJECT_HEADER_CREATOR_INFO
{
    LIST_ENTRY TypeList;
    PVOID CreatorUniqueProcess;
    USHORT CreatorBackTraceIndex;
    USHORT Reserved;
} OBJECT_HEADER_CREATOR_INFO, *POBJECT_HEADER_CREATOR_INFO;

/* Union representing either a single handle count entry or a full database */
typedef union _OBJECT_HEADER_HANDLE_INFO
{
    POBJECT_HANDLE_COUNT_DATABASE HandleCountDatabase;
    OBJECT_HANDLE_COUNT_ENTRY SingleEntry;
} OBJECT_HEADER_HANDLE_INFO, *POBJECT_HEADER_HANDLE_INFO;

/* Name information stored in the object header */
typedef struct _OBJECT_HEADER_NAME_INFO
{
    POBJECT_DIRECTORY Directory;
    UNICODE_STRING Name;
    ULONG QueryReferences;
    ULONG Reserved;
    ULONG DbgReferenceCount;
} OBJECT_HEADER_NAME_INFO, *POBJECT_HEADER_NAME_INFO;

/* Active quota information stored in the object header */
typedef struct _OBJECT_HEADER_QUOTA_INFO
{
    ULONG PagedPoolCharge;
    ULONG NonPagedPoolCharge;
    ULONG SecurityDescriptorCharge;
    PEPROCESS ExclusiveProcess;
} OBJECT_HEADER_QUOTA_INFO, *POBJECT_HEADER_QUOTA_INFO;

/* Context maintained during namespace traversal and directory lookups */
typedef struct _OBJECT_LOOKUP_CONTEXT
{
    POBJECT_DIRECTORY Directory;
    PVOID Object;
    USHORT HashIndex;
    BOOLEAN DirectoryLocked;
    VOLATILE ULONG LockStateSignature;
} OBJECT_LOOKUP_CONTEXT, *POBJECT_LOOKUP_CONTEXT;

/* Optional header storing the object's hierarchical name and directory links */
typedef struct _OBJECT_NAME_INFO
{
    LIST_ENTRY ObjectLinks;
    UNICODE_STRING ObjectName;
    PWSTR ObjectDirectoryPath;
} OBJECT_NAME_INFO, *POBJECT_NAME_INFO;

/* Structure used to query an object's name */
typedef struct _OBJECT_NAME_INFORMATION
{
    UNICODE_STRING Name;
} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;

/* Memory layout sizes for the object's optional headers */
typedef struct _OBJECT_OPTIONAL_HEADER_LAYOUT
{
    ULONG QuotaInfoSize;
    ULONG HandleInfoSize;
    ULONG NameInfoSize;
    ULONG CreatorInfoSize;
    ULONG TotalSize;
} OBJECT_OPTIONAL_HEADER_LAYOUT, *POBJECT_OPTIONAL_HEADER_LAYOUT;

/* Information block linking an object to a specific process */
typedef struct _OBJECT_PROCESS_INFO
{
    LIST_ENTRY Entry;
    PEPROCESS Process;
    ULONG ReferenceCount;
} OBJECT_PROCESS_INFO, *POBJECT_PROCESS_INFO;

/* Optional header tracking memory pool charges */
typedef struct _OBJECT_QUOTA_INFO
{
    ULONG PagedPoolCharge;
    ULONG NonPagedPoolCharge;
    ULONG SecurityDescriptorCharge;
    PEPROCESS ExclusiveProcess;
} OBJECT_QUOTA_INFO, *POBJECT_QUOTA_INFO;

/* Configuration block defining the lifecycle and behavior of a specific object type */
typedef struct _OBJECT_TYPE_INITIALIZER
{
    USHORT Length;
    BOOLEAN UseDefaultObject;
    BOOLEAN CaseInsensitive;
    ULONG InvalidAttributes;
    GENERIC_MAPPING GenericMapping;
    ULONG ValidAccessMask;
    BOOLEAN SecurityRequired;
    BOOLEAN MaintainHandleCount;
    BOOLEAN MaintainTypeList;
    MMPOOL_TYPE PoolType;
    ULONG DefaultPagedPoolCharge;
    ULONG DefaultNonPagedPoolCharge;
    POB_DUMP_METHOD DumpProcedure;
    POB_OPEN_METHOD OpenProcedure;
    POB_CLOSE_METHOD CloseProcedure;
    POB_DELETE_METHOD DeleteProcedure;
    POB_PARSE_METHOD ParseProcedure;
    POB_SECURITY_METHOD SecurityProcedure;
    POB_QUERYNAME_METHOD QueryNameProcedure;
    POB_OKAYTOCLOSE_METHOD OkayToCloseProcedure;
} OBJECT_TYPE_INITIALIZER, *POBJECT_TYPE_INITIALIZER;


/* Descriptor representing a registered object type in the system */
typedef struct _OBJECT_TYPE
{
    LIST_ENTRY TypeList;
    UNICODE_STRING Name;
    PVOID DefaultObject;
    ULONG Index;
    ULONG TotalNumberOfObjects;
    ULONG TotalNumberOfHandles;
    ULONG HighWaterNumberOfObjects;
    ULONG HighWaterNumberOfHandles;
    OBJECT_TYPE_INITIALIZER TypeInfo;
    KPUSH_LOCK TypeLock;
    ULONG Key;
    LIST_ENTRY CallbackList;
} OBJECT_TYPE;

#endif /* __XTOS_ASSEMBLER__ */
#endif /* __XTDK_OBTYPES_H */
