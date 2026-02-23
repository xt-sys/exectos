/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/rtl/sha1.hh
 * DESCRIPTION:     SHA1 computation support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_RTL_SHA1_HH
#define __XTOSKRNL_RTL_SHA1_HH

#include <xtos.hh>


/* Runtime Library */
namespace RTL
{
    class SHA1
    {
        public:
            STATIC XTAPI XTSTATUS ComputeDigest(IN PCUCHAR Buffer,
                                                IN SIZE_T BufferSize,
                                                OUT PUCHAR Digest);

        private:
            STATIC XTAPI VOID ComputeHash(IN OUT PRTL_SHA1_CONTEXT Context,
                                              OUT PUCHAR Digest);
            STATIC XTAPI VOID HashData(IN OUT PRTL_SHA1_CONTEXT Context,
                                       IN PCUCHAR Data,
                                       IN ULONG Length);
            STATIC XTAPI XTSTATUS InitializeContext(OUT PRTL_SHA1_CONTEXT Context);
            STATIC XTAPI VOID TransformData(IN OUT PULONG State,
                                            IN PCUCHAR Buffer);
    };
}

#endif /* __XTOSKRNL_RTL_SHA1_HH */
