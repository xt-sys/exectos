/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/mm/pool.hh
 * DESCRIPTION:     Memory Manager pool manager
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_MM_POOL_HH
#define __XTOSKRNL_MM_POOL_HH

#include <xtos.hh>


/* Memory Manager */
namespace MM
{
    class Pool
    {
        protected:
            STATIC PFN_NUMBER NonPagedPoolFrameEnd;
            STATIC PFN_NUMBER NonPagedPoolFrameStart;
            STATIC LIST_ENTRY NonPagedPoolFreeList[MM_MAX_FREE_PAGE_LIST_HEADS];

        public:
            STATIC XTAPI VOID InitializeNonPagedPool(VOID);
            STATIC XTAPI VOID InitializePagedPool(VOID);

        protected:
            STATIC XTAPI VOID VerifyRunLevel(IN MMPOOL_TYPE PoolType,
                                             IN SIZE_T Bytes,
                                             IN PVOID Entry);

        private:
            STATIC XTAPI VOID MapNonPagedPool(VOID);
    };
}

#endif /* __XTOSKRNL_MM_POOL_HH */
