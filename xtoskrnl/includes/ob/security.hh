/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ob/security.hh
 * DESCRIPTION:     Object Manager Security API
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_OB_SECURITY_HH
#define __XTOSKRNL_OB_SECURITY_HH

#include <xtos.hh>


/* Object Manager */
namespace OB
{
    class Security
    {
        public:
            STATIC XTAPI VOID AuditObjectAccess(IN HANDLE Handle,
                                                IN PHANDLE_TABLE_ENTRY_INFO ObjectTableEntryInfo,
                                                IN PUNICODE_STRING ObjectTypeName,
                                                IN ACCESS_MASK DesiredAccess);
            STATIC XTAPI XTSTATUS ProcessObjectSecurityDescriptor(IN PVOID Object,
                                                                  IN SECURITY_OPERATION_CODE OperationCode,
                                                                  IN PSECURITY_INFORMATION SecurityInformation,
                                                                  IN OUT PSECURITY_DESCRIPTOR SecurityDescriptor,
                                                                  IN OUT PULONG Length,
                                                                  IN OUT PSECURITY_DESCRIPTOR *OldSecurityDescriptor,
                                                                  IN MMPOOL_TYPE PoolType,
                                                                  IN PGENERIC_MAPPING GenericMapping);
    };
}

#endif /* __XTOSKRNL_OB_SECURITY_HH */
