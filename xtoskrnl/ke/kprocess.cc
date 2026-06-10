/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/kprocess.cc
 * DESCRIPTION:     XT kernel process manipulation support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * Retrieves the pointer to the global Idle process.
 *
 * @return Returns a pointer to the Idle process.
 *
 * @since XT 1.0
 */
XTAPI
PKPROCESS
KE::KProcess::GetIdleProcess(VOID)
{
    /* Return pointer to the idle process */
    return &InitialProcess.ProcessControlBlock;
}

/**
 * Retrieves a pointer to the system's initial executive process object.
 *
 * @return This routine returns a pointer to the initial executive process.
 *
 * @since XT 1.0
 */
XTAPI
PEPROCESS
KE::KProcess::GetInitialProcess(VOID)
{
    return &InitialProcess;
}

/**
 * Initializes the system-wide Idle Process.
 *
 * @param Process
 *        Supplies a pointer to the KPROCESS structure representing the idle process.
 *
 * @param DirectoryTable
 *        Supplies a pointer to the initial hardware page directory table for the process.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
KE::KProcess::InitializeIdleProcess(IN OUT PKPROCESS Process,
                                    IN PULONG_PTR DirectoryTable)
{
    ULONG Cpus, Index;
    XTSTATUS Status;

    /* Get the number of natively installed CPUs */
    Cpus = KE::Processor::GetInstalledCpus();

    /* Allocate and initialize the baseline affinity map for the process */
    Status = KE::Affinity::CreateAffinityMap(Cpus, &Process->Affinity);
    if(Status != STATUS_SUCCESS)
    {
        /* Affinity map allocation failed, return status code */
        return Status;
    }

    /* Allocate and initialize the dynamic map used to track actively running CPUs */
    Status = KE::Affinity::CreateAffinityMap(Cpus, &Process->ActiveProcessors);
    if(Status != STATUS_SUCCESS)
    {
        /* Affinity map allocation failed, return status code */
        return Status;
    }

    /* Set Idle Process affinity */
    KE::Affinity::SetAllProcessorsAffinity(Process->Affinity);

    /* Initialize Idle process */
    KE::KProcess::InitializeProcess(Process, 0, NULLPTR, DirectoryTable, FALSE);
    Process->Quantum = MAXCHAR;

    /* Populate the global Idle Process affinity */
    for(Index = 0; Index < Cpus; Index++)
    {
        /* Include every installed hardware thread in the base affinity map */
        KE::Affinity::SetProcessorAffinity(Process->Affinity, Index);
    }

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Initializes the process.
 *
 * @param Process
 *        Supplies a pointer to process that will be initialized.
 *
 * @param Priority
 *        Specifies the process priority.
 *
 * @param AffinityMap
 *        Specifies a process affinity map designating processors on which process can run.
 *
 * @param DirectoryTable
 *        Supplies a pointer to the directory table.
 *
 * @param Alignment
 *        Specifies the exceptions alignment of the process.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::KProcess::InitializeProcess(IN OUT PKPROCESS Process,
                                IN KPRIORITY Priority,
                                IN PKAFFINITY_MAP AffinityMap,
                                IN PULONG_PTR DirectoryTable,
                                IN BOOLEAN Alignment)
{
    /* Initialize process dispatcher header */
    Process->Header.Type = ProcessObject;

    /* Initialize process wait list */
    RtlInitializeListHead(&Process->Header.WaitListHead);

    /* Initialize process list heads */
    RtlInitializeListHead(&Process->ProfileListHead);
    RtlInitializeListHead(&Process->ReadyListHead);
    RtlInitializeListHead(&Process->ThreadListHead);

    /* Check if source affinity map was provided */
    if(AffinityMap != NULLPTR && AffinityMap != Process->Affinity)
    {
        /* Set process affinity */
        KE::Affinity::CopyAffinity(Process->Affinity, AffinityMap);
    }

    /* Set base process properties */
    Process->BasePriority = Priority;
    Process->AutoAlignment = Alignment;

    /* Set directory tables */
    Process->DirectoryTable[0] = DirectoryTable[0];
    Process->DirectoryTable[1] = DirectoryTable[1];

    /* Set the initial stack count and process quantum */
    Process->StackCount = MAXSHORT;
    Process->Quantum = THREAD_QUANTUM;

    /* Set IOPM offset */
    Process->IopmOffset = sizeof(KTSS);

    /* Set initial process state */
    Process->State = ProcessInMemory;
}
