/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/se/access.cc
 * DESCRIPTION:     Security access state check routines
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Computes the access rights that are requested but not granted.
 *
 * @param GrantedAccessMask
 *        Supplies the mask of access rights currently granted to the subject.
 *
 * @param DesiredAccessMask
 *        Supplies the mask of access rights requested by the subject.
 *
 * @return This routine returns an access mask with the bits in DesiredAccess that are not present in GrantedAccess.
 *
 * @since XT 1.0
 */
XTFASTCALL
ACCESS_MASK
SE::Access::ComputeDeniedAccesses(IN ACCESS_MASK GrantedAccessMask,
                                  IN ACCESS_MASK DesiredAccessMask)
{
    /* Return the desired denied accesses */
    return ~GrantedAccessMask & DesiredAccessMask;
}

/**
 * Computes the access rights that are both requested and granted.
 *
 * @param GrantedAccessMask
 *        Supplies the mask of access rights currently granted to the subject.
 *
 * @param DesiredAccessMask
 *        Supplies the mask of access rights requested by the subject.
 *
 * @return This routine returns an access mask containing the intersection of DesiredAccess and GrantedAccess bits.
 *
 * @since XT 1.0
 */
XTFASTCALL
ACCESS_MASK
SE::Access::ComputeGrantedAccesses(IN ACCESS_MASK GrantedAccessMask,
                                   IN ACCESS_MASK DesiredAccessMask)
{
    /* Return the desired granted accesses */
    return GrantedAccessMask & DesiredAccessMask;
}

/**
 * Maps all generic accesses in the provided access mask to specific and standard accesses.
 *
 * @param AccessMask
 *        Supplies a pointer to the access mask to be mapped.
 *
 * @param Mapping
 *        Supplies the mapping of generic to specific and standard access types.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
SE::Access::MapGenericMask(IN OUT PACCESS_MASK AccessMask,
                           IN PGENERIC_MAPPING Mapping)
{
    /* Check if the read right is present in the access mask */
    if(*AccessMask & SE_GENERIC_READ)
    {
        /* Map the read flag to its specific access rights */
        *AccessMask |= Mapping->GenericRead;
    }

    /* Check if the write right is present in the access mask */
    if(*AccessMask & SE_GENERIC_WRITE)
    {
        /* Map the write flag to its specific access rights */
        *AccessMask |= Mapping->GenericWrite;
    }

    /* Check if the execute right is present in the access mask */
    if(*AccessMask & SE_GENERIC_EXECUTE)
    {
        /* Map the execute flag to its specific access rights */
        *AccessMask |= Mapping->GenericExecute;
    }

    /* Check if the all right is present in the access mask */
    if(*AccessMask & SE_GENERIC_ALL)
    {
        /* Map the all flag to its specific access rights */
        *AccessMask |= Mapping->GenericAll;
    }

    /* Clear the generic flags from the final access mask */
    *AccessMask &= ~(SE_GENERIC_READ | SE_GENERIC_WRITE | SE_GENERIC_EXECUTE | SE_GENERIC_ALL);
}
