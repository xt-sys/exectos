/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/se/audit.hh
 * DESCRIPTION:     Security Auditing
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_SE_AUDIT_HH
#define __XTOSKRNL_SE_AUDIT_HH

#include <xtos.hh>


/* Kernel Security */
namespace SE
{
    class Audit
    {
        public:
            STATIC XTAPI VOID OperationAuditAlarm(IN PUNICODE_STRING SubsystemName,
                                                  IN PVOID HandleId,
                                                  IN PUNICODE_STRING ObjectTypeName,
                                                  IN ACCESS_MASK AccessMask,
                                                  IN PSID UserSid);
    };
}

#endif /* __XTOSKRNL_SE_AUDIT_HH */
