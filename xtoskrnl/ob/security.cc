/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ob/security.cc
 * DESCRIPTION:     Object Manager Security API
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Consumes the audit mask for a handle table entry and triggers a security audit alarm if the requested access
 * rights match the audit requirements.
 *
 * @param Handle
 *        Supplies the handle being accessed.
 *
 * @param ObjectTableEntryInfo
 *        Supplies a pointer to the extended handle table entry information containing the volatile audit mask.
 *
 * @param ObjectTypeName
 *        Supplies a pointer to the Unicode string representing the object's type name.
 *
 * @param AccessMask
 *        Supplies the access mask requested by the caller to be evaluated against the audit mask.
 *
 * @return This routine does not return a value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
OB::Security::AuditObjectAccess(IN HANDLE Handle,
                                IN PHANDLE_TABLE_ENTRY_INFO ObjectTableEntryInfo,
                                IN PUNICODE_STRING ObjectTypeName,
                                IN ACCESS_MASK AccessMask)
{
    ACCESS_MASK BitsToAudit, CurrentAuditMask, PreviousAuditMask, RemainingAuditMask;

    /* Enter a CAS loop */
    while(ObjectTableEntryInfo->AuditMask)
    {
        /* Capture the current state of the audit mask */
        CurrentAuditMask = ObjectTableEntryInfo->AuditMask;

        /* Mask out the access rights */
        RemainingAuditMask = CurrentAuditMask & ~AccessMask;

        /* Check if the requested access intersects with the audit mask */
        if(RemainingAuditMask == CurrentAuditMask)
        {
            /* No overlapping bits, return */
            return;
        }

        /* Attempt to update the audit mask */
        PreviousAuditMask = (ACCESS_MASK)RTL::Atomic::CompareExchange32((VOLATILE PLONG)&ObjectTableEntryInfo->AuditMask,
                                                                        (LONG)RemainingAuditMask,
                                                                        (LONG)CurrentAuditMask);

        /* Verify if the update succeeded */
        if(PreviousAuditMask == CurrentAuditMask)
        {
            /* Extract the access bits */
            BitsToAudit = PreviousAuditMask & AccessMask;

            /* Check if there are active bits */
            if(BitsToAudit)
            {
                /* Dispatch the audit event */
                SE::Audit::OperationAuditAlarm(NULLPTR, Handle, ObjectTypeName, BitsToAudit, NULLPTR);
            }

            /* Auditing completed, return */
            return;
        }
    }
}

/**
 * Provides the default security procedure for handling object security descriptors.
 *
 * @param Object
 *        Supplies a pointer to the object whose security descriptor is being processed.
 *
 * @param OperationCode
 *        Supplies the specific security operation to perform (e.g., Assign, Delete, Query, Set).
 *
 * @param SecurityInformation
 *        Supplies a bitmask indicating which specific components of the security descriptor are involved.
 *
 * @param SecurityDescriptor
 *        Supplies a pointer to a caller-allocated buffer. For a set/assign operation, this contains the new
 *        security data. For a query operation, it receives the requested data.
 *
 * @param Length
 *        Supplies a pointer to a variable that specifies the size of the SecurityDescriptor buffer.
 *        On return for a query operation, it receives the actual number of bytes required.
 *
 * @param OldSecurityDescriptor
 *        Supplies a pointer to a variable that holds the current security descriptor of the object.
 *
 * @param PoolType
 *        Supplies the type of memory pool to use if a new security descriptor must be allocated.
 *
 * @param GenericMapping
 *        Supplies a pointer to the generic access mapping associated with the object's type.
 *
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
OB::Security::ProcessObjectSecurityDescriptor(IN PVOID Object,
                                              IN SECURITY_OPERATION_CODE OperationCode,
                                              IN PSECURITY_INFORMATION SecurityInformation,
                                              IN OUT PSECURITY_DESCRIPTOR SecurityDescriptor,
                                              IN OUT PULONG Length,
                                              IN OUT PSECURITY_DESCRIPTOR *OldSecurityDescriptor,
                                              IN MMPOOL_TYPE PoolType,
                                              IN PGENERIC_MAPPING GenericMapping)
{
    /* Switch on the operation code */
    switch(OperationCode)
    {
        case AssignSecurityDescriptor:
            break;
        case DeleteSecurityDescriptor:
            break;
        case QuerySecurityDescriptor:
            break;
        case SetSecurityDescriptor:
            break;
        default:
            KE::Crash::Panic(0x29, 0, STATUS_INVALID_PARAMETER, 0, 0);
            break;
    }

    UNIMPLEMENTED;

    /* Return success */
    return STATUS_SUCCESS;
}
