/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/se/descript.cc
 * DESCRIPTION:     Security Descriptors
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
* Creates a safe kernel-space copy of a security descriptor to prevent TOC/TOU vulnerabilities.
*
* @param InputDescriptor
*        Supplies a pointer to the original security descriptor to be captured.
*
* @param ProcessorMode
*        Supplies the processor mode from which the request originated.
*
* @param PoolType
*        Supplies the type of memory pool to allocate the captured copy from.
*
* @param ForceCapture
*        Supplies a boolean value indicating whether to force the capture in kernel mode.
*
* @param OutputDescriptor
*        Supplies a pointer to a variable that receives the safely captured security descriptor.
*
* @return This routine returns a status code indicating the success or failure of the operation.
*
* @since XT 1.0
*/
XTAPI
XTSTATUS
SE::Descriptor::CaptureSecurityDescriptor(IN PSECURITY_DESCRIPTOR InputDescriptor,
                                          IN KPROCESSOR_MODE ProcessorMode,
                                          IN MMPOOL_TYPE PoolType,
                                          IN BOOLEAN ForceCapture,
                                          OUT PSECURITY_DESCRIPTOR *OutputDescriptor)
{
    UNIMPLEMENTED;

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Computes the memory quota required for a security descriptor.
 *
 * @param Size
 *        Supplies the base size of the security information in bytes.
 *
 * @return This routine returns the computed security quota in bytes.
 *
 * @since XT 1.0
 */
XTFASTCALL
ULONG
SE::Descriptor::ComputeSecurityQuota(IN ULONG Size)
{
    ULONG ComputedSize;

    /* Calculate double the base size */
    ComputedSize = Size * 2;

    /* Ensure the allocated quota meets the minimum default system threshold */
    return (ComputedSize > SE_DEFAULT_SECURITY_QUOTA) ? ComputedSize : SE_DEFAULT_SECURITY_QUOTA;
}

/**
* Calculates the exact memory footprint of a security descriptor for quota accounting.
*
* @param SecurityDescriptor
*        Supplies a pointer to a valid, captured security descriptor.
*
* @param QuotaSize
*        Supplies a pointer to a variable that receives the calculated quota size in bytes.
*
* @return This routine returns a status code indicating the success or failure of the operation.
*
* @since XT 1.0
*/
XTAPI
XTSTATUS
SE::Descriptor::ComputeQuotaInformationSize(IN PSECURITY_DESCRIPTOR SecurityDescriptor,
                                            OUT PULONG QuotaSize)
{
    UNIMPLEMENTED;

    /* Return success */
    return STATUS_SUCCESS;
}

/**
* Frees the memory of a captured security descriptor.
*
* @param Descriptor
*        Supplies a pointer to the captured security descriptor to be released.
*
* @param ProcessorMode
*        Supplies the processor mode that was originally used to capture the descriptor.
*
* @param ForceRelease
*        Supplies a boolean value indicating whether to force the deallocation.
*
* @return This routine does not return any value.
*
* @since XT 1.0
*/
XTAPI
VOID
SE::Descriptor::ReleaseSecurityDescriptor(IN PSECURITY_DESCRIPTOR Descriptor,
                                          IN KPROCESSOR_MODE ProcessorMode,
                                          IN BOOLEAN ForceRelease)
{
    /* Check if the descriptor should be released */
    if(((ProcessorMode == KernelMode) && (ForceRelease == TRUE)) || (ProcessorMode == UserMode))
    {
        /* Ensure the descriptor is valid */
        if(Descriptor)
        {
            /* Free the descriptor */
            MM::Allocator::FreePool(Descriptor, TAG_SE_DESCRIPTOR);
        }
    }
}
