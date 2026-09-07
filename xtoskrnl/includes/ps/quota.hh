/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ps/quota.hh
 * DESCRIPTION:     Process Quota Management
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_PS_QUOTA_HH
#define __XTOSKRNL_PS_QUOTA_HH

#include <xtos.hh>


/* Process and thread management */
namespace PS
{
    class Quota
    {
        private:
            STATIC EPROCESS_QUOTA_BLOCK DefaultQuotaBlock;
            STATIC KSPIN_LOCK QuotaLock;

        public:
            STATIC XTAPI XTSTATUS ChargeProcessQuota(IN PEPROCESS_QUOTA_BLOCK QuotaBlock,
                                                     IN PEPROCESS Process,
                                                     IN PS_QUOTA_TYPE QuotaType,
                                                     IN SIZE_T Amount);
            STATIC XTAPI PEPROCESS_QUOTA_BLOCK ChargeSharedPoolQuota(IN PEPROCESS Process,
                                                                     IN SIZE_T PagedAmount,
                                                                     IN SIZE_T NonPagedAmount);
            STATIC VOID XTAPI InitializeQuota(VOID);
            STATIC XTFASTCALL VOID ReturnProcessQuota(IN PEPROCESS_QUOTA_BLOCK QuotaBlock,
                                                      IN PEPROCESS Process,
                                                      IN PS_QUOTA_TYPE QuotaType,
                                                      IN SIZE_T Amount);
            STATIC XTAPI VOID ReturnSharedPoolQuota(IN PEPROCESS_QUOTA_BLOCK QuotaBlock,
                                                    IN SIZE_T PagedAmount,
                                                    IN SIZE_T NonPagedAmount);

        private:
            STATIC XTAPI VOID DereferenceQuotaBlock(IN PEPROCESS_QUOTA_BLOCK QuotaBlock);
            STATIC XTINLINE VOID UpdatePeakUsage(IN PSIZE_T TargetQuota,
                                                 IN SIZE_T NewQuota);
    };
}

#endif /* __XTOSKRNL_PS_QUOTA_HH */
