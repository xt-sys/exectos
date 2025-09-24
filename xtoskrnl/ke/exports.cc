/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/exports.cc
 * DESCRIPTION:     C-compatible API wrappers for exported kernel functions
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Acquires a specified queued spinlock.
 *
 * @param LockLevel
 *        Supplies the queued spinlock level.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTFASTCALL
VOID
KeAcquireQueuedSpinLock(IN KSPIN_LOCK_QUEUE_LEVEL LockLevel)
{
    KE::SpinLock::AcquireQueuedSpinLock(LockLevel);
}

/**
 * Acquires a kernel spin lock.
 *
 * @param SpinLock
 *        Supplies a pointer to the kernel spin lock.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTFASTCALL
VOID
KeAcquireSpinLock(IN OUT PKSPIN_LOCK SpinLock)
{
    KE::SpinLock::AcquireSpinLock(SpinLock);
}

/**
 * Looks for an unacquired system resource of the specified type and acquires it.
 *
 * @param ResourceType
 *        Supplies system resource type.
 *
 * @param ResourceHeader
 *        Specifies a memory area where a pointer to the system resource header will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
XTSTATUS
KeAcquireSystemResource(IN SYSTEM_RESOURCE_TYPE ResourceType,
                        OUT PSYSTEM_RESOURCE_HEADER *ResourceHeader)
{
    return KE::SystemResources::AcquireResource(ResourceType, ResourceHeader);
}

/**
 * Cancels the timer.
 *
 * @param Timer
 *        Supplies a pointer to a timer object.
 *
 * @return This routine returns TRUE if the cancelled timer was set, or FALSE otherwise.
 *
 * @since NT 3.5
 */
XTCLINK
XTAPI
BOOLEAN
KeCancelTimer(IN PKTIMER Timer)
{
    return KE::Timer::CancelTimer(Timer);
}

/**
 * Gets the current running level of the current processor.
 *
 * @return This routine returns the current running level.
 *
 * @since XT 1.0
 */
XTCLINK
XTFASTCALL
KRUNLEVEL
KeGetCurrentRunLevel(VOID)
{
    return KE::RunLevel::GetCurrentRunLevel();
}

/**
 * Looks for an unacquired system resource of the specified type and returns it without acquiring an ownership.
 *
 * @param ResourceType
 *        Supplies system resource type.
 *
 * @param ResourceHeader
 *        Specifies a memory area where a pointer to the system resource header will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
XTSTATUS
KeGetSystemResource(IN SYSTEM_RESOURCE_TYPE ResourceType,
                    OUT PSYSTEM_RESOURCE_HEADER *ResourceHeader)
{
    return KE::SystemResources::GetResource(ResourceType, ResourceHeader);
}

/**
 * Reads the current signal state of the given timer.
 *
 * @param Timer
 *        Supplies a pointer to a timer object.
 *
 * @return This routine returns TRUE if the timer is set, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
BOOLEAN
KeGetTimerState(IN PKTIMER Timer)
{
    return KE::Timer::GetState(Timer);
}

/**
 * Initializes an APC object.
 *
 * @param Apc
 *        Supplies a pointer to the APC object.
 *
 * @param Thread
 *        Supplies a pointer to the thread object.
 *
 * @param Environment
 *        Specifies an environment in which the APC will run.
 *
 * @param KernelRoutine
 *        Supplies a pointer to routine called at APC_LEVEL.
 *
 * @param RundownRoutine
 *        Supplies a pointer to routine called on thread exit.
 *
 * @param NormalRoutine
 *        Supplies a pointer to routine called at IRQL 0.
 *
 * @param ApcMode
 *        Specifies processor mode, in which NormalRoutine gets called.
 *
 * @param Context
 *        Supplies a pointer to memory area containing data passed to NormalRoutine.
 *
 * @return This routine does not return any value.
 *
 * @since NT 3.5
 */
XTCLINK
XTAPI
VOID
KeInitializeApc(IN PKAPC Apc,
                IN PKTHREAD Thread,
                IN KAPC_ENVIRONMENT Environment,
                IN PKKERNEL_ROUTINE KernelRoutine,
                IN PKRUNDOWN_ROUTINE RundownRoutine,
                IN PKNORMAL_ROUTINE NormalRoutine,
                IN KPROCESSOR_MODE ApcMode,
                IN PVOID Context)
{
    KE::Apc::InitializeApc(Apc, Thread, Environment, KernelRoutine, RundownRoutine, NormalRoutine, ApcMode, Context);

}

/**
 * Initializes Deferred Procedure Call (DPC) object.
 *
 * @param Dpc
 *        Supplies a pointer to the DPC being initialized.
 *
 * @param DpcRoutine
 *        Supplies a pointer to the DPC routine being called on object removal.
 *
 * @param DpcContext
 *        Supplies a pointer to memory area containing context data for DPC routine.
 *
 * @return This routine does not return any value.
 *
 * @since NT 3.5
 */
XTCLINK
XTAPI
VOID
KeInitializeDpc(IN PKDPC Dpc,
                IN PKDEFERRED_ROUTINE DpcRoutine,
                IN PVOID DpcContext)
{
    KE::Dpc::InitializeDpc(Dpc, DpcRoutine, DpcContext);

}

/**
 * Initializes Deferred Procedure Call (DPC) object.
 *
 * @param Dpc
 *        Supplies a pointer to the DPC being initialized.
 *
 * @param DpcRoutine
 *        Supplies a pointer to the DPC routine being called on object removal.
 *
 * @param DpcContext
 *        Supplies a pointer to memory area containing context data for DPC routine.
 *
 * @return This routine does not return any value.
 *
 * @since NT 5.2
 */
XTCLINK
XTAPI
VOID
KeInitializeThreadedDpc(IN PKDPC Dpc,
                        IN PKDEFERRED_ROUTINE DpcRoutine,
                        IN PVOID DpcContext)
{
    KE::Dpc::InitializeThreadedDpc(Dpc, DpcRoutine, DpcContext);
}

/**
 * Initializes an extended kernel timer.
 *
 * @param Timer
 *        Supplies a pointer to a timer object.
 *
 * @param Type
 *        Supplies the type of the timer.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
VOID
KeInitializeTimer(OUT PKTIMER Timer,
                  IN KTIMER_TYPE Type)
{
    KE::Timer::InitializeTimer(Timer, Type);
}

/**
 * Initializes a kernel semaphore object.
 *
 * @param Semaphore
 *        Supplies a pointer to a semaphore object.
 *
 * @param Count
 *        Specifies the initial count value of the semaphore.
 *
 * @param Limit
 *        Specifies a maximum count value of the semaphore.
 *
 * @return This routine does not return any value.
 *
 * @since NT 3.5
 */
XTCLINK
XTAPI
VOID
KeInitializeSemaphore(IN PKSEMAPHORE Semaphore,
                      IN LONG Count,
                      IN LONG Limit)
{
    KE::Semaphore::InitializeSemaphore(Semaphore, Count, Limit);
}

/**
 * Initializes a kernel spinlock object.
 *
 * @param SpinLock
 *        Supplies a pointer to a kernel spin lock.
 *
 * @return This routine does not return any value.
 *
 * @since NT 3.5
 */
XTCLINK
XTAPI
VOID
KeInitializeSpinLock(IN PKSPIN_LOCK SpinLock)
{
    KE::SpinLock::InitializeSpinLock(SpinLock);
}

/**
 * Lowers the running level of the current processor.
 *
 * @param RunLevel
 *        Supplies the new running level to lower to.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTFASTCALL
VOID
KeLowerRunLevel(KRUNLEVEL RunLevel)
{
    KE::RunLevel::LowerRunLevel(RunLevel);
}

/**
 * Raises the running level of the current processor.
 *
 * @param RunLevel
 *        Supplies the new running level to raise to.
 *
 * @return This routine returns the old running level.
 *
 * @since XT 1.0
 */
XTCLINK
XTFASTCALL
KRUNLEVEL
KeRaiseRunLevel(KRUNLEVEL RunLevel)
{
    return KE::RunLevel::RaiseRunLevel(RunLevel);
}

/**
 * Reads semaphore's current signal state.
 *
 * @param Semaphore
 *        Supplies a pointer to a semaphore object.
 *
 * @return This routine returns the current signal state of the semaphore.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
LONG
KeReadSemaphoreState(IN PKSEMAPHORE Semaphore)
{
    return KE::Semaphore::ReadState(Semaphore);
}

/**
 * Releases a queued spinlock.
 *
 * @param LockLevel
 *        Supplies the queued spinlock level.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTFASTCALL
VOID
KeReleaseQueuedSpinLock(IN KSPIN_LOCK_QUEUE_LEVEL LockLevel)
{
    KE::SpinLock::ReleaseQueuedSpinLock(LockLevel);
}

/**
 * Releases a semaphore.
 *
 * @param Semaphore
 *        Supplies a pointer to a semaphore object.
 *
 * @param Increment
 *        Specifies the priority increment value of the semaphore.
 *
 * @param Adjustment
 *        Specifies adjustment value added to the semaphore's initial count value.
 *
 * @param Wait
 *        Determines whether release of the semaphore will be followed by a kernel wait routine call or not.
 *
 * @return This routine returns a previous signal state of the semaphore.
 *
 * @since NT 3.5
 */
XTCLINK
XTAPI
LONG
KeReleaseSemaphore(IN PKSEMAPHORE Semaphore,
                   IN KPRIORITY Increment,
                   IN LONG Adjustment,
                   IN BOOLEAN Wait)
{
    return KE::Semaphore::ReleaseSemaphore(Semaphore, Increment, Adjustment, Wait);
}

/**
 * Releases a kernel spin lock.
 *
 * @param SpinLock
 *        Supplies a pointer to the kernel spin lock.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTFASTCALL
VOID
KeReleaseSpinLock(IN OUT PKSPIN_LOCK SpinLock)
{
    KE::SpinLock::ReleaseSpinLock(SpinLock);
}

/**
 * Releases system resource.
 *
 * @param ResourceHeader
 *        Specifies a pointer to the system resource header.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
VOID
KeReleaseSystemResource(IN PSYSTEM_RESOURCE_HEADER ResourceHeader)
{
    KE::SystemResources::ReleaseResource(ResourceHeader);
}

/**
 * Sets the target processor number for DPC.
 *
 * @param Dpc
 *        Supplies a pointer to the DPC object.
 *
 * @param Number
 *        Supplies the target processor number.
 *
 * @return This routine does not return any value.
 *
 * @since NT 4.0
 */
XTCLINK
XTAPI
VOID
KeSetTargetProcessorDpc(IN PKDPC Dpc,
                        IN CCHAR Number)
{
    KE::Dpc::SetTargetProcessor(Dpc, Number);
}

/**
 * Sets the supplied timer to expire at the specified time.
 *
 * @param Timer
 *        Supplies a pointer to a timer object.
 *
 * @param DueTime
 *        Supplies the time at which the timer should expire (both absolute and relative times are supported).
 *
 * @param Period
 *        Supplies the timer period.
 *
 * @param Dpc
 *        Supplies a pointer to a Deferred Procedure Call (DPC) object.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
VOID
KeSetTimer(IN PKTIMER Timer,
           IN LARGE_INTEGER DueTime,
           IN LONG Period,
           IN PKDPC Dpc)
{
    KE::Timer::SetTimer(Timer, DueTime, Period, Dpc);
}

/**
 * Decrements the DPC call barier.
 *
 * @param SystemArgument
 *        Supplies an address of the DPC call barrier.
 *
 * @return This routine does not return any value.
 *
 * @since NT 5.2
 */
XTCLINK
XTAPI
VOID
KeSignalCallDpcDone(IN PVOID SystemArgument)
{
    KE::Dpc::SignalCallDone(SystemArgument);
}

/**
 * Decrements the DPC call reverse barier.
 *
 * @param SystemArgument
 *        Supplies an address of the DPC call barrier.
 *
 * @return This routine returns TRUE if just one processor is waiting on the barrier, FALSE if more.
 *
 * @since NT 5.2
 */
XTCLINK
XTAPI
BOOLEAN
KeSignalCallDpcSynchronize(IN PVOID SystemArgument)
{
    return KE::Dpc::SignalCallSynchronize(SystemArgument);
}
