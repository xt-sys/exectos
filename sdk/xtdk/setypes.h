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

/* Default security quota */
#define SE_DEFAULT_SECURITY_QUOTA                               2048

#define SE_INITIAL_PRIVILEGE_COUNT                              3

/* Token source length */
#define SE_TOKEN_SOURCE_LENGTH                                  8

/* C/C++ specific code */
#ifndef __XTOS_ASSEMBLER__

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

#endif /* __XTOS_ASSEMBLER__ */
#endif /* __XTDK_SETYPES_H */
