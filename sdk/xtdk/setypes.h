/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/setypes.h
 * DESCRIPTION:     Kernel Security structures and definitions
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTDK_SETYPES_H
#define __XTDK_SETYPES_H

#include <xttypes.h>
#include <xtstruct.h>
#include <xtbase.h>


/* Privilege LUIDs */
#define SE_LUID_MIN_WELL_KNOWN_PRIVILEGE                        (LUID){2, 0}
#define SE_LUID_CREATE_TOKEN_PRIVILEGE                          (LUID){2, 0}
#define SE_LUID_ASSIGNPRIMARYTOKEN_PRIVILEGE                    (LUID){3, 0}
#define SE_LUID_LOCK_MEMORY_PRIVILEGE                           (LUID){4, 0}
#define SE_LUID_INCREASE_QUOTA_PRIVILEGE                        (LUID){5, 0}
#define SE_LUID_MACHINE_ACCOUNT_PRIVILEGE                       (LUID){6, 0}
#define SE_LUID_TCB_PRIVILEGE                                   (LUID){7, 0}
#define SE_LUID_SECURITY_PRIVILEGE                              (LUID){8, 0}
#define SE_LUID_TAKE_OWNERSHIP_PRIVILEGE                        (LUID){9, 0}
#define SE_LUID_LOAD_DRIVER_PRIVILEGE                           (LUID){10, 0}
#define SE_LUID_SYSTEM_PROFILE_PRIVILEGE                        (LUID){11, 0}
#define SE_LUID_SYSTEMTIME_PRIVILEGE                            (LUID){12, 0}
#define SE_LUID_PROF_SINGLE_PROCESS_PRIVILEGE                   (LUID){13, 0}
#define SE_LUID_INC_BASE_PRIORITY_PRIVILEGE                     (LUID){14, 0}
#define SE_LUID_CREATE_PAGEFILE_PRIVILEGE                       (LUID){15, 0}
#define SE_LUID_CREATE_PERMANENT_PRIVILEGE                      (LUID){16, 0}
#define SE_LUID_BACKUP_PRIVILEGE                                (LUID){17, 0}
#define SE_LUID_RESTORE_PRIVILEGE                               (LUID){18, 0}
#define SE_LUID_SHUTDOWN_PRIVILEGE                              (LUID){19, 0}
#define SE_LUID_DEBUG_PRIVILEGE                                 (LUID){20, 0}
#define SE_LUID_AUDIT_PRIVILEGE                                 (LUID){21, 0}
#define SE_LUID_SYSTEM_ENVIRONMENT_PRIVILEGE                    (LUID){22, 0}
#define SE_LUID_CHANGE_NOTIFY_PRIVILEGE                         (LUID){23, 0}
#define SE_LUID_REMOTE_SHUTDOWN_PRIVILEGE                       (LUID){24, 0}
#define SE_LUID_UNDOCK_PRIVILEGE                                (LUID){25, 0}
#define SE_LUID_SYNC_AGENT_PRIVILEGE                            (LUID){26, 0}
#define SE_LUID_ENABLE_DELEGATION_PRIVILEGE                     (LUID){27, 0}
#define SE_LUID_MANAGE_VOLUME_PRIVILEGE                         (LUID){28, 0}
#define SE_LUID_IMPERSONATE_PRIVILEGE                           (LUID){29, 0}
#define SE_LUID_CREATE_GLOBAL_PRIVILEGE                         (LUID){30, 0}
#define SE_LUID_TRUSTED_CREDMAN_ACCESS_PRIVILEGE                (LUID){31, 0}
#define SE_LUID_RELABEL_PRIVILEGE                               (LUID){32, 0}
#define SE_LUID_INC_WORKING_SET_PRIVILEGE                       (LUID){33, 0}
#define SE_LUID_TIME_ZONE_PRIVILEGE                             (LUID){34, 0}
#define SE_LUID_CREATE_SYMBOLIC_LINK_PRIVILEGE                  (LUID){35, 0}

/* Standard Access Rights definitions */
#define SE_DELETE                                               0x00010000L
#define SE_READ_CONTROL                                         0x00020000L
#define SE_WRITE_DAC                                            0x00040000L
#define SE_WRITE_OWNER                                          0x00080000L
#define SE_SYNCHRONIZE                                          0x00100000L
#define SE_STANDARD_RIGHTS_REQUIRED                             0x000F0000L
#define SE_STANDARD_RIGHTS_READ                                 SE_READ_CONTROL
#define SE_STANDARD_RIGHTS_WRITE                                SE_READ_CONTROL
#define SE_STANDARD_RIGHTS_EXECUTE                              SE_READ_CONTROL
#define SE_STANDARD_RIGHTS_ALL                                  0x001F0000L
#define SE_SPECIFIC_RIGHTS_ALL                                  0x0000FFFFL
#define SE_ACCESS_SYSTEM_SECURITY                               0x01000000L
#define SE_MAXIMUM_ALLOWED                                      0x02000000L

/* Generic access rights definitions */
#define SE_GENERIC_ALL                                          0x10000000L
#define SE_GENERIC_EXECUTE                                      0x20000000L
#define SE_GENERIC_WRITE                                        0x40000000L
#define SE_GENERIC_READ                                         0x80000000L

/* Process access rights definitions */
#define SE_PROCESS_TERMINATE                                    0x0001
#define SE_PROCESS_CREATE_THREAD                                0x0002
#define SE_PROCESS_SET_SESSIONID                                0x0004
#define SE_PROCESS_VM_OPERATION                                 0x0008
#define SE_PROCESS_VM_READ                                      0x0010
#define SE_PROCESS_VM_WRITE                                     0x0020
#define SE_PROCESS_DUP_HANDLE                                   0x0040
#define SE_PROCESS_CREATE_PROCESS                               0x0080
#define SE_PROCESS_SET_QUOTA                                    0x0100
#define SE_PROCESS_SET_INFORMATION                              0x0200
#define SE_PROCESS_QUERY_INFORMATION                            0x0400
#define SE_PROCESS_SUSPEND_RESUME                               0x0800
#define SE_PROCESS_QUERY_LIMITED_INFORMATION                    0x1000

/* Process full control access mask */
#define SE_PROCESS_ALL_ACCESS                                   (SE_STANDARD_RIGHTS_REQUIRED | SE_SYNCHRONIZE | 0xFFFF)

/* Thread access rights definitions */
#define SE_THREAD_TERMINATE                                     0x0001
#define SE_THREAD_SUSPEND_RESUME                                0x0002
#define SE_THREAD_ALERT                                         0x0004
#define SE_THREAD_GET_CONTEXT                                   0x0008
#define SE_THREAD_SET_CONTEXT                                   0x0010
#define SE_THREAD_SET_INFORMATION                               0x0020
#define SE_THREAD_QUERY_INFORMATION                             0x0040
#define SE_THREAD_SET_THREAD_TOKEN                              0x0080
#define SE_THREAD_IMPERSONATE                                   0x0100
#define SE_THREAD_DIRECT_IMPERSONATION                          0x0200

/* Thread full control access mask */
#define SE_THREAD_ALL_ACCESS                                    (SE_STANDARD_RIGHTS_REQUIRED | SE_SYNCHRONIZE | 0xFFFF)

/* Object full control access mask */
#define SE_OBJECT_TYPE_ALL_ACCESS                               (SE_STANDARD_RIGHTS_REQUIRED | 0x1)

/* Default security quota */
#define SE_DEFAULT_SECURITY_QUOTA                               2048

#define SE_INITIAL_PRIVILEGE_COUNT                              3

/* Token source length */
#define SE_TOKEN_SOURCE_LENGTH                                  8

/* C/C++ specific code */
#ifndef __XTOS_ASSEMBLER__

/* The classification type for a security proxy token */
typedef enum _PROXY_CLASS
{
    ProxyFull = 0,
    ProxyService,
    ProxyTree,
    ProxyDirectory
} PROXY_CLASS, *PPROXY_CLASS;

/* Security impersonation levels */
typedef enum _SECURITY_IMPERSONATION_LEVEL
{
    SecurityAnonymous,
    SecurityIdentification,
    SecurityImpersonation,
    SecurityDelegation
} SECURITY_IMPERSONATION_LEVEL, *PSECURITY_IMPERSONATION_LEVEL;

/* Security operation codes */
typedef enum _SECURITY_OPERATION_CODE
{
    SetSecurityDescriptor,
    QuerySecurityDescriptor,
    DeleteSecurityDescriptor,
    AssignSecurityDescriptor
} SECURITY_OPERATION_CODE, *PSECURITY_OPERATION_CODE;

/* Access token types enumeration list */
typedef enum _TOKEN_TYPE
{
    TokenPrimary = 1,
    TokenImpersonation
} TOKEN_TYPE, *PTOKEN_TYPE;

/* Access Control List structure definition */
typedef struct _ACL
{
    UCHAR AclRevision;
    UCHAR Reserved1;
    USHORT AclSize;
    USHORT AceCount;
    USHORT Reserved2;
} ACL, *PACL;

/* Bitfields for granular security audit policy categories */
typedef struct _AUDIT_POLICY_CATEGORIES
{
    UCHAR System:4;
    UCHAR Logon:4;
    UCHAR ObjectAccess:4;
    UCHAR PrivilegeUse:4;
    UCHAR DetailedTracking:4;
    UCHAR PolicyChange:4;
    UCHAR AccountManagement:4;
    UCHAR DirectoryServiceAccess:4;
    UCHAR AccountLogon:4;
} AUDIT_POLICY_CATEGORIES, *PAUDIT_POLICY_CATEGORIES;

/* Overlay structure for manipulating audit policy bit masks */
typedef struct _AUDIT_POLICY_OVERLAY
{
    ULONGLONG PolicyBits:36;
    ULONGLONG SetBit:1;
} AUDIT_POLICY_OVERLAY, *PAUDIT_POLICY_OVERLAY;

/* Audit policy structure definition */
typedef union _AUDIT_POLICY
{
    AUDIT_POLICY_CATEGORIES PolicyElements;
    AUDIT_POLICY_OVERLAY PolicyOverlay;
    ULONGLONG Overlay;
} AUDIT_POLICY, *PAUDIT_POLICY;

/* Generic security mapping structure definition */
typedef struct _GENERIC_MAPPING
{
    ULONG GenericRead;
    ULONG GenericWrite;
    ULONG GenericExecute;
    ULONG GenericAll;
} GENERIC_MAPPING, *PGENERIC_MAPPING;

/* LUID and attributes structure definition */
typedef struct _LUID_AND_ATTRIBUTES
{
    LUID Luid;
    ULONG Attributes;
} LUID_AND_ATTRIBUTES, *PLUID_AND_ATTRIBUTES;

/* Initial privilege set structure definition */
typedef struct _INITIAL_PRIVILEGE_SET
{
    ULONG PrivilegeCount;
    ULONG Control;
    LUID_AND_ATTRIBUTES Privilege[SE_INITIAL_PRIVILEGE_COUNT];
} INITIAL_PRIVILEGE_SET, * PINITIAL_PRIVILEGE_SET;

/* Privilege set structure definition */
typedef struct _PRIVILEGE_SET
{
    ULONG PrivilegeCount;
    ULONG Control;
    LUID_AND_ATTRIBUTES Privilege[1];
} PRIVILEGE_SET, *PPRIVILEGE_SET;

/* Process audit information structure definition */
typedef struct _SECURITY_PROCESS_AUDIT_INFO
{
    PEPROCESS Process;
    PEPROCESS ParentProcess;
} SECURITY_PROCESS_AUDIT_INFO, *PSECURITY_PROCESS_AUDIT_INFO;

/* Security audit process creation information structure definition */
typedef struct _SECURITY_AUDIT_PROCESS_CREATION_INFO
{
    POBJECT_NAME_INFORMATION ImageFileName;
} SECURITY_AUDIT_PROCESS_CREATION_INFO, *PSECURITY_AUDIT_PROCESS_CREATION_INFO;

/* Identifier authority structure definition */
typedef struct _SID_IDENTIFIER_AUTHORITY
{
    UCHAR Value[6];
} SID_IDENTIFIER_AUTHORITY,*PSID_IDENTIFIER_AUTHORITY;

/* Security identifier structure definition */
typedef struct _SID
{
    UCHAR Revision;
    UCHAR SubAuthorityCount;
    SID_IDENTIFIER_AUTHORITY IdentifierAuthority;
    ULONG SubAuthority[1];
} SID, *PSID;

/* Security Identifier with its corresponding state attributes structure definition */
typedef struct _SID_ATTRIBUTES
{
    PSID Sid;
    ULONG Attributes;
} SID_ATTRIBUTES, *PSID_ATTRIBUTES;

/* Token source structure definition */
typedef struct _TOKEN_SOURCE
{
    CHAR SourceName[SE_TOKEN_SOURCE_LENGTH];
    LUID SourceIdentifier;
} TOKEN_SOURCE, *PTOKEN_SOURCE;

/* Token control structure definition */
typedef struct _TOKEN_CONTROL
{
    LUID TokenId;
    LUID AuthenticationId;
    LUID ModifiedId;
    TOKEN_SOURCE TokenSource;
} TOKEN_CONTROL, *PTOKEN_CONTROL;

/* Security quality of service structure definition */
typedef struct _SECURITY_QUALITY_OF_SERVICE
{
    ULONG Length;
    SECURITY_IMPERSONATION_LEVEL ImpersonationLevel;
    SECURITY_CONTEXT_TRACKING_MODE ContextTrackingMode;
    BOOLEAN EffectiveOnly;
} SECURITY_QUALITY_OF_SERVICE, *PSECURITY_QUALITY_OF_SERVICE;

/* Security client context structure definition */
typedef struct _SECURITY_CLIENT_CONTEXT
{
    SECURITY_QUALITY_OF_SERVICE SecurityQos;
    PACCESS_TOKEN ClientToken;
    BOOLEAN DirectlyAccessClientToken;
    BOOLEAN DirectAccessEffectiveOnly;
    BOOLEAN ServerIsRemote;
    TOKEN_CONTROL ClientTokenControl;
} SECURITY_CLIENT_CONTEXT, *PSECURITY_CLIENT_CONTEXT;

/* Security subject context structure definition */
typedef struct _SECURITY_SUBJECT_CONTEXT
{
    PACCESS_TOKEN ClientToken;
    SECURITY_IMPERSONATION_LEVEL ImpersonationLevel;
    PACCESS_TOKEN PrimaryToken;
    PVOID ProcessAuditId;
} SECURITY_SUBJECT_CONTEXT, *PSECURITY_SUBJECT_CONTEXT;

/* Per-token audit generation information and access masks structure definition */
typedef struct _SECURITY_TOKEN_AUDIT_DATA
{
    ULONG Length;
    ULONG GrantMask;
    ULONG DenyMask;
} SECURITY_TOKEN_AUDIT_DATA, *PSECURITY_TOKEN_AUDIT_DATA;

/* Security proxy configuration and constraint data structure definition */
typedef struct _SECURITY_TOKEN_PROXY_DATA
{
    ULONG Length;
    PROXY_CLASS ProxyClass;
    UNICODE_STRING PathInfo;
    ULONG ContainerMask;
    ULONG ObjectMask;
} SECURITY_TOKEN_PROXY_DATA, *PSECURITY_TOKEN_PROXY_DATA;

/* Active references, device mappings, and tokens tracking data structure definition */
typedef struct _SESSION_REFERENCES
{
    PSESSION_REFERENCES Next;
    LUID LogonId;
    ULONG ReferenceCount;
    ULONG Flags;
    PDEVICE_MAP DeviceMap;
    LIST_ENTRY TokenList;
} SESSION_REFERENCES, *PSESSION_REFERENCES;

/* Access state structure definition */
typedef struct _ACCESS_STATE
{
    LUID OperationID;
    BOOLEAN SecurityEvaluated;
    BOOLEAN GenerateAudit;
    BOOLEAN GenerateOnClose;
    BOOLEAN PrivilegesAllocated;
    ULONG Flags;
    ACCESS_MASK RemainingDesiredAccess;
    ACCESS_MASK PreviouslyGrantedAccess;
    ACCESS_MASK OriginalDesiredAccess;
    SECURITY_SUBJECT_CONTEXT SubjectSecurityContext;
    PSECURITY_DESCRIPTOR SecurityDescriptor;
    PVOID AuxData;
    union
    {
        INITIAL_PRIVILEGE_SET InitialPrivilegeSet;
        PRIVILEGE_SET PrivilegeSet;
    } Privileges;
    BOOLEAN AuditPrivileges;
    UNICODE_STRING ObjectName;
    UNICODE_STRING ObjectTypeName;
} ACCESS_STATE, *PACCESS_STATE;

/* The core access token structure definition */
typedef struct _TOKEN
{
    TOKEN_SOURCE TokenSource;
    LUID TokenId;
    LUID AuthenticationId;
    LUID ParentTokenId;
    LARGE_INTEGER ExpirationTime;
    PERESOURCE TokenLock;
    AUDIT_POLICY AuditPolicy;
    LUID ModifiedId;
    ULONG SessionId;
    ULONG UserAndGroupCount;
    ULONG RestrictedSidCount;
    ULONG PrivilegeCount;
    ULONG VariableLength;
    ULONG DynamicCharged;
    ULONG DynamicAvailable;
    ULONG DefaultOwnerIndex;
    PSID_ATTRIBUTES UserAndGroups;
    PSID_ATTRIBUTES RestrictedSids;
    PSID PrimaryGroup;
    PLUID_AND_ATTRIBUTES Privileges;
    PULONG DynamicPart;
    PACL DefaultDacl;
    TOKEN_TYPE TokenType;
    SECURITY_IMPERSONATION_LEVEL ImpersonationLevel;
    ULONG TokenFlags;
    BOOLEAN TokenInUse;
    PSECURITY_TOKEN_PROXY_DATA ProxyData;
    PSECURITY_TOKEN_AUDIT_DATA AuditData;
    PSESSION_REFERENCES LogonSession;
    LUID OriginatingLogonSession;
    ULONG VariablePart;
} TOKEN, *PTOKEN;

#endif /* __XTOS_ASSEMBLER__ */
#endif /* __XTDK_SETYPES_H */
