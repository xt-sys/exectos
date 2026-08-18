/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/se/audit.cc
 * DESCRIPTION:     Security Auditing
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Generates a security audit event for an object handle duplication.
 *
 * @param SourceHandle
 *        Supplies the original handle identifier as it exists in the source process.
 *
 * @param TargetHandle
 *        Supplies the newly created handle identifier as it exists in the target process.
 *
 * @param SourceProcess
 *        Supplies a pointer to the executive process originating the duplication.
 *
 * @param TargetProcess
 *        Supplies a pointer to the executive process receiving the duplicated handle.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
SE::Audit::AuditHandleDuplication(PVOID SourceHandle,
                                  PVOID TargetHandle,
                                  PEPROCESS SourceProcess,
                                  PEPROCESS TargetProcess)
{
    UNIMPLEMENTED;
}

/**
 * Determines whether detailed security auditing is enabled for a specific token.
 *
 * @param AccessToken
 *        Supplies an optional pointer to the access token to be evaluated.
 *
 * @return This routine returns TRUE if auditing is required, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTFASTCALL
BOOLEAN
SE::Audit::AuditWithToken(IN PACCESS_TOKEN AccessToken)
{
    UNIMPLEMENTED;

    /* Bypass audit generation */
    return FALSE;
}

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
