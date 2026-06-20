/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/quota.cc
 * DESCRIPTION:     Memory Manager Quota support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Attempts to increase the pool quota limit.
 *
 * @param PoolType
 *        Supplies the type of memory pool whose quota limit is being evaluated for an increase.
 *
 * @param CurrentQuotaLimit
 *        Supplies the current quota limit, in bytes, for the requested pool.
 *
 * @param NewQuotaLimit
 *        Receives the newly established quota limit if the increase was successful.
 *
 * @return This routine returns TRUE if the quota limit was successfully raised, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
MM::Quota::RaisePoolQuota(IN MMPOOL_TYPE PoolType,
                          IN SIZE_T CurrentQuotaLimit,
                          OUT PSIZE_T NewQuotaLimit
)
{
    UNIMPLEMENTED;
    return FALSE;
}

/**
 * Returns a specified amount of quota back to the global system pool.
 *
 * @param PoolType
 *        Supplies the type of memory pool receiving the returned quota.
 *
 * @param ReturnedQuota
 *        Supplies the amount of quota, in bytes, being returned to the system.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Quota::ReturnPoolQuota(IN MMPOOL_TYPE PoolType,
                           IN SIZE_T ReturnedQuota)
{
    /* Route the returned quota to the appropriate pool */
    switch(PoolType)
    {
        case PagedPool:
            /* Return quota to the paged pool */
            PagedPoolQuota -= ReturnedQuota;
            break;
        default:
            /* Return quota to the non-paged pool */
            NonPagedPoolQuota -= ReturnedQuota;
            break;
    }
}
