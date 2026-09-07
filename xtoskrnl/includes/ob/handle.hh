/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ob/handle.hh
 * DESCRIPTION:     Object Manager Handle Management
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_OB_HANDLE_HH
#define __XTOSKRNL_OB_HANDLE_HH

#include <xtos.hh>


/* Object Manager */
namespace OB
{
    class Handle
    {
        public:
            STATIC XTFASTCALL HANDLE DecodeKernelHandle(IN HANDLE Handle);
            STATIC XTFASTCALL HANDLE EncodeKernelHandle(IN HANDLE Handle);
            STATIC XTFASTCALL ULONG GetHandleAttributes(IN PHANDLE_TABLE_ENTRY HandleTableEntry);
    };
}

#endif /* __XTOSKRNL_OB_HANDLE_HH */
