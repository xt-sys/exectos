/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ex/exports.cc
 * DESCRIPTION:     C-compatible API wrappers for exported kernel functions
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Acquires the rundown protection for given descriptor.
 *
 * @param Descriptor
 *        Supplies a pointer to the rundown block descriptor.
 *
 * @return This routine returns TRUE if protection acquired successfully, or FALSE otherwise.
 *
 * @since NT 5.1
 */
XTFASTCALL
BOOLEAN
ExAcquireRundownProtection(IN PEX_RUNDOWN_REFERENCE Descriptor)
{
    return EX::Rundown::AcquireProtection(Descriptor);
}

/**
 * Marks the rundown descriptor as completed.
 *
 * @param Descriptor
 *        Supplies a pointer to the descriptor to be completed.
 *
 * @return This routine does not return any value.
 *
 * @since NT 5.1
 */
XTFASTCALL
VOID
ExCompleteRundownProtection(IN PEX_RUNDOWN_REFERENCE Descriptor)
{
    EX::Rundown::CompleteProtection(Descriptor);
}

/**
 * Initializes the rundown protection descriptor.
 *
 * @param Descriptor
 *        Supplies a pointer to the descriptor to be initialized.
 *
 * @return This routine does not return any value.
 *
 * @since NT 5.1
 */
XTFASTCALL
VOID
ExInitializeRundownProtection(IN PEX_RUNDOWN_REFERENCE Descriptor)
{
    EX::Rundown::InitializeProtection(Descriptor);
}

/**
 * Reinitializes the rundown protection structure after it has been completed.
 *
 * @param Descriptor
 *        Supplies a pointer to the descriptor to be reinitialized.
 *
 * @return This routine does not return any value.
 *
 * @since NT 5.1
 */
XTFASTCALL
VOID
ExReInitializeRundownProtection(IN PEX_RUNDOWN_REFERENCE Descriptor)
{
    EX::Rundown::ReInitializeProtection(Descriptor);
}

/**
 * Releases the rundown protection for given descriptor.
 *
 * @param Descriptor
 *        Supplies a pointer to the descriptor to be initialized.
 *
 * @return This routine does not return any value.
 *
 * @since NT 5.1
 */
XTFASTCALL
VOID
ExReleaseRundownProtection(IN PEX_RUNDOWN_REFERENCE Descriptor)
{
    EX::Rundown::ReleaseProtection(Descriptor);
}

/**
 * Waits until rundown protection calls are completed.
 *
 * @param Descriptor
 *        Supplies a pointer to the rundown block descriptor.
 *
 * @return This routine does not return any value.
 *
 * @since NT 5.1
 */
XTFASTCALL
VOID
ExWaitForRundownProtectionRelease(IN PEX_RUNDOWN_REFERENCE Descriptor)
{
    EX::Rundown::WaitForProtectionRelease(Descriptor);
}
