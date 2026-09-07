/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/mm/quota.hh
 * DESCRIPTION:     Memory Manager Quota support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_MM_QUOTA_HH
#define __XTOSKRNL_MM_QUOTA_HH

#include <xtos.hh>


/* Memory Manager */
namespace MM
{
    class Quota
    {
        private:
            STATIC SIZE_T PagedPoolQuota;
            STATIC SIZE_T NonPagedPoolQuota;

        public:
            STATIC XTAPI BOOLEAN RaisePoolQuota(IN MMPOOL_TYPE PoolType,
                                                IN SIZE_T CurrentQuotaLimit,
                                                OUT PSIZE_T NewQuotaLimit);
            STATIC XTAPI VOID ReturnPoolQuota(IN MMPOOL_TYPE PoolType,
                                              IN SIZE_T ReturnedQuota);
    };
}

#endif /* __XTOSKRNL_MM_QUOTA_HH */
