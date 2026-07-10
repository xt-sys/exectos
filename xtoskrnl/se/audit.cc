/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/se/audit.cc
 * DESCRIPTION:     Security Auditing
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Generates a security audit alarm for an object access operation.
 *
 * @param SubsystemName
 *        Supplies the name of the subsystem triggering the audit alarm.
 *
 * @param HandleId
 *        Supplies the handle or identifier of the object being accessed.
 *
 * @param ObjectTypeName
 *        Supplies the name of the object type being accessed.
 *
 * @param AccessMask
 *        Supplies the access mask containing the specific rights that triggered the audit.
 *
 * @param UserSid
 *        Optionally supplies the SID of the user initiating the operation.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
SE::Audit::OperationAuditAlarm(IN PUNICODE_STRING SubsystemName,
                               IN PVOID HandleId,
                               IN PUNICODE_STRING ObjectTypeName,
                               IN ACCESS_MASK AccessMask,
                               IN PSID UserSid)
{
    UNIMPLEMENTED;
}
