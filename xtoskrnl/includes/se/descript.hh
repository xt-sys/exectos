/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/se/descript.hh
 * DESCRIPTION:     Security Descriptors
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_SE_DESCRIPT_HH
#define __XTOSKRNL_SE_DESCRIPT_HH

#include <xtos.hh>


/* Kernel Security */
namespace SE
{
    class Descriptor
    {
        public:
            STATIC XTAPI XTSTATUS CaptureSecurityDescriptor(IN PSECURITY_DESCRIPTOR InputDescriptor,
                                                            IN KPROCESSOR_MODE ProcessorMode,
                                                            IN MMPOOL_TYPE PoolType,
                                                            IN BOOLEAN ForceCapture,
                                                            OUT PSECURITY_DESCRIPTOR *OutputDescriptor);
            STATIC XTFASTCALL ULONG ComputeSecurityQuota(IN ULONG Size);
            STATIC XTAPI XTSTATUS ComputeQuotaInformationSize(IN PSECURITY_DESCRIPTOR SecurityDescriptor,
                                                              OUT PULONG QuotaSize);
            STATIC XTAPI VOID ReleaseSecurityDescriptor(IN PSECURITY_DESCRIPTOR Descriptor,
                                                        IN KPROCESSOR_MODE ProcessorMode,
                                                        IN BOOLEAN ForceRelease);
    };
}

#endif /* __XTOSKRNL_SE_DESCRIPT_HH */
