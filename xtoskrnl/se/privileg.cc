/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/se/privileg.cc
 * DESCRIPTION:     Privileges Management
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Performs a privilege check against a specific access token.
 *
 * @param Token
 *        Supplies a pointer to the access token being queried.
 *
 * @param Privileges
 *        Supplies a pointer to an array of privileges and their attributes.
 *
 * @param PrivilegeCount
 *        Supplies the number of privileges in the array.
 *
 * @param PrivilegeSet
 *        Supplies the control flags specifying how the privileges are evaluated.
 *
 * @param ProcessorMode
 *        Supplies the processor mode from which the request originated.
 *
 * @return This routine returns TRUE if the required privileges are held, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
SE::Privileges::CheckPrivilege(IN PTOKEN Token,
                               IN OUT PLUID_AND_ATTRIBUTES Privileges,
                               IN ULONG PrivilegeCount,
                               IN ULONG PrivilegeSet,
                               IN KPROCESSOR_MODE ProcessorMode)
{
    UNIMPLEMENTED;

    /* Return TRUE */
    return TRUE;
}

/**
 * Checks whether the specified privileges are held by the security context.
 *
 * @param RequiredPrivileges
 *        Supplies a pointer to the privilege set that specifies the privileges to check.
 *
 * @param SecurityContext
 *        Supplies a pointer to the security subject context representing the caller.
 *
 * @param ProcessorMode
 *        Supplies the processor mode of the caller.
 *
 * @return This routine returns TRUE if the required privileges are held, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
SE::Privileges::CheckPrivilege(IN OUT PPRIVILEGE_SET RequiredPrivileges,
                               IN PSECURITY_SUBJECT_CONTEXT SecurityContext,
                               IN KPROCESSOR_MODE ProcessorMode)
{
    PACCESS_TOKEN Token;

    /* Verify that an existing client token has a sufficient impersonation level */
    if((SecurityContext->ClientToken != NULLPTR) && (SecurityContext->ImpersonationLevel < SecurityImpersonation))
    {
        /* The impersonation level is too low to perform privilege checks, return FALSE */
        return FALSE;
    }

    /* Select the effective access token based on the presence of a client impersonation token */
    Token = SecurityContext->ClientToken ? SecurityContext->ClientToken : SecurityContext->PrimaryToken;

    /* Dispatch the privilege check and return the result */
    return CheckPrivilege((PTOKEN)Token, RequiredPrivileges->Privilege, RequiredPrivileges->PrivilegeCount,
                          RequiredPrivileges->Control, ProcessorMode);
}

/**
* Checks if the current thread's access token holds a specific privilege.
*
* @param PrivilegeValue
*        Supplies the locally unique identifier (LUID) of the privilege to check.
*
* @param ProcessorMode
*        Supplies the processor mode from which the request originated.
*
* @return This routine returns TRUE if the privilege is held, or FALSE otherwise.
*
* @since XT 1.0
*/
XTAPI
BOOLEAN
SE::Privileges::CheckSinglePrivilege(LUID PrivilegeValue,
                                     KPROCESSOR_MODE ProcessorMode)
{
    UNIMPLEMENTED;

    /* Return TRUE */
    return TRUE;
}
