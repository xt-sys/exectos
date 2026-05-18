/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/data.cc
 * DESCRIPTION:     Kernel Library global and static data
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/* Kernel initialization block passed by boot loader */
PKERNEL_INITIALIZATION_BLOCK KE::BootInformation::InitializationBlock = {};

/* Kernel initial process */
EPROCESS KE::KProcess::InitialProcess;

/* Kernel initial thread */
ETHREAD KE::KThread::InitialThread = {};

/* Kernel UBSAN active frame flag */
BOOLEAN KE::KUbsan::ActiveFrame = FALSE;

/* Total number of installed processors in the system */
ULONG KE::Processor::InstalledCpus;

/* Array of pointers to processor control blocks */
PKPROCESSOR_BLOCK *KE::Processor::ProcessorBlocks;

/* Kernel shared data (KSD) */
PKSHARED_DATA KE::SharedData::KernelSharedData;

/* Kernel dispatcher lock queue */
KSPIN_LOCK KE::SpinLock::DispatcherLockQueue;

/* Kernel expansion lock queue */
KSPIN_LOCK KE::SpinLock::ExpansionLockQueue;

/* Kernel file system structures lock queue */
KSPIN_LOCK KE::SpinLock::FileSystemLockQueue;

/* Kernel IO cancel lock queue */
KSPIN_LOCK KE::SpinLock::IoCancelLockQueue;

/* Kernel IO completion lock queue */
KSPIN_LOCK KE::SpinLock::IoCompletionLockQueue;

/* Kernel IO database lock queue */
KSPIN_LOCK KE::SpinLock::IoDatabaseLockQueue;

/* Kernel IO VPB lock queue */
KSPIN_LOCK KE::SpinLock::IoVpbLockQueue;

/* Kernel cache master lock queue */
KSPIN_LOCK KE::SpinLock::MasterLockQueue;

/* Kernel non-paged allocator lock queue */
KSPIN_LOCK KE::SpinLock::NonPagedAllocLockQueue;

/* Kernel non-paged pool lock queue */
KSPIN_LOCK KE::SpinLock::NonPagedPoolLockQueue;

/* Kernel PFN lock queue */
KSPIN_LOCK KE::SpinLock::PfnLockQueue;

/* Kernel system space lock queue */
KSPIN_LOCK KE::SpinLock::SystemSpaceLockQueue;

/* Kernel Timer table lock queue */
KSPIN_LOCK KE::SpinLock::TimerTableLockQueue;

/* Kernel VACB lock queue */
KSPIN_LOCK KE::SpinLock::VacbLockQueue;

/* Kernel work queue lock queue */
KSPIN_LOCK KE::SpinLock::WorkLockQueue;

/* Kernel boot resources list */
LIST_ENTRY KE::SystemResources::ResourcesListHead;

/* Kernel boot resources lock */
KSPIN_LOCK KE::SystemResources::ResourcesLock;

/* Kernel boot time */
LARGE_INTEGER KE::SystemTime::BootTime;

/* The maximum interval between system clock interrupts */
ULONG KE::SystemTime::MaximumIncrement;

/* The minimum interval between system clock interrupts */
ULONG KE::SystemTime::MinimumIncrement;

/* Accumulator tracking fractional ticks, decremented until a full tick elapses */
LONG KE::SystemTime::TickOffset;

/* The runtime adjustment value applied to the system clock at each interrupt */
ULONG KE::SystemTime::TimeAdjustment;
