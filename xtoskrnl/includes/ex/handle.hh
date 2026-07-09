/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ex/handle.hh
 * DESCRIPTION:     Handle Table management interface
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_EX_HANDLE_HH
#define __XTOSKRNL_EX_HANDLE_HH

#include <xtos.hh>


/* Kernel Executive */
namespace EX
{
    class Handle
    {
        public:
            STATIC XTFASTCALL PHANDLE_TABLE_ENTRY_INFO GetHandleInformation(IN PHANDLE_TABLE HandleTable,
                                                                            IN HANDLE Handle,
                                                                            IN BOOLEAN Locked);
            STATIC XTAPI BOOLEAN LockHandleTableEntry(IN PHANDLE_TABLE HandleTable,
                                                      IN PHANDLE_TABLE_ENTRY HandleTableEntry);
            STATIC XTAPI PHANDLE_TABLE_ENTRY MapHandleToPointer(IN PHANDLE_TABLE HandleTable,
                                                                IN HANDLE Handle);
            STATIC XTAPI VOID UnlockHandleTableEntry(IN PHANDLE_TABLE HandleTable,
                                                     IN PHANDLE_TABLE_ENTRY HandleTableEntry);

        private:
            STATIC XTAPI PHANDLE_TABLE_ENTRY LookupHandleTableEntry(IN PHANDLE_TABLE HandleTable,
                                                                    IN EXHANDLE Handle);
    };
}

#endif /* __XTOSKRNL_EX_HANDLE_HH */
