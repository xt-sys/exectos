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

/* Generic security mapping structure definition */
typedef struct _GENERIC_MAPPING
{
    ULONG GenericRead;
    ULONG GenericWrite;
    ULONG GenericExecute;
    ULONG GenericAll;
} GENERIC_MAPPING, *PGENERIC_MAPPING;

/* Security quality of service structure definition */
typedef struct _SECURITY_QUALITY_OF_SERVICE
{
    ULONG Length;
    SECURITY_IMPERSONATION_LEVEL ImpersonationLevel;
    SECURITY_CONTEXT_TRACKING_MODE ContextTrackingMode;
    BOOLEAN EffectiveOnly;
} SECURITY_QUALITY_OF_SERVICE, *PSECURITY_QUALITY_OF_SERVICE;

#endif /* __XTOS_ASSEMBLER__ */
#endif /* __XTDK_SETYPES_H */
