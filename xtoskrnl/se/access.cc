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
