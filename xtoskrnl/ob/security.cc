/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ob/security.cc
 * DESCRIPTION:     Object Manager Security API
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


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
