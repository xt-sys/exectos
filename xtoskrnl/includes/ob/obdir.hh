/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ob/obdir.hh
 * DESCRIPTION:     Object Manager Security API
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_OB_OBDIR_HH
#define __XTOSKRNL_OB_OBDIR_HH

#include <xtos.hh>


/* Object Manager */
namespace OB
{
    class Directory
    {
        public:
            STATIC XTAPI BOOLEAN DeleteDirectoryEntry(IN POBJECT_LOOKUP_CONTEXT LookupContext);
            STATIC XTFASTCALL VOID InitializeLookupContext(OUT POBJECT_LOOKUP_CONTEXT LookupContext);
            STATIC XTAPI BOOLEAN InsertDirectoryEntry(IN OUT POBJECT_DIRECTORY Directory,
                                                      IN POBJECT_LOOKUP_CONTEXT LookupContext,
                                                      IN OUT POBJECT_HEADER ObjectHeader);
            STATIC XTFASTCALL VOID LockLookupContext(IN POBJECT_DIRECTORY Directory,
                                                     IN POBJECT_LOOKUP_CONTEXT LookupContext);

            STATIC XTAPI PVOID LookupDirectoryEntry(IN POBJECT_DIRECTORY Directory,
                                                    IN PUNICODE_STRING Name,
                                                    IN ULONG Attributes,
                                                    IN BOOLEAN SearchGlobalDirectory,
                                                    OUT POBJECT_LOOKUP_CONTEXT LookupContext);
            STATIC XTFASTCALL VOID ReleaseLookupContext(IN OUT POBJECT_LOOKUP_CONTEXT LookupContext);

        private:
            STATIC XTFASTCALL VOID AcquireExclusiveDirectoryLock(IN OUT POBJECT_DIRECTORY Directory,
                                                                 IN OUT POBJECT_LOOKUP_CONTEXT LookupContext);
            STATIC XTFASTCALL VOID AcquireSharedDirectoryLock(IN POBJECT_DIRECTORY Directory,
                                                              IN POBJECT_LOOKUP_CONTEXT LookupContext);
            STATIC XTFASTCALL ULONG ComputeObjectNameHash(IN PCUNICODE_STRING Name);
            STATIC XTFASTCALL VOID ReleaseDirectoryLock(IN OUT POBJECT_DIRECTORY Directory,
                                                        IN OUT POBJECT_LOOKUP_CONTEXT LookupContext);
            STATIC XTFASTCALL VOID ReleaseLookupContextObject(IN OUT POBJECT_LOOKUP_CONTEXT LookupContext);
            STATIC XTFASTCALL PVOID SearchDirectory(IN POBJECT_DIRECTORY Directory,
                                                    IN PCUNICODE_STRING Name,
                                                    IN BOOLEAN CaseInsensitive,
                                                    IN ULONG HashIndex,
                                                    IN POBJECT_LOOKUP_CONTEXT LookupContext);
    };
}

#endif /* __XTOSKRNL_OB_OBDIR_HH */
