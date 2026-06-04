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
VOID
KE::KProcess::InitializeIdleProcess(IN OUT PKPROCESS Process,
                                    IN PULONG_PTR DirectoryTable)
{
    ULONG AffinitySize, MapSize;
    PACPI_SYSTEM_INFO SysInfo;

    /* Retrieve hardware topology from ACPI */
    HL::Acpi::GetSystemInformation(&SysInfo);

    /* Calculate the required size for KAFFINITY_MAP structures */
    AffinitySize = (SysInfo->CpuCount + 63) / 64;
    MapSize = sizeof(KAFFINITY_MAP) + (AffinitySize * sizeof(KAFFINITY));

    /* Align size to the 8-byte boundary */
    MapSize = (MapSize + 7) & ~7;

    /* Allocate the process-level affinity structures */
    MM::Allocator::AllocatePool(NonPagedPool, MapSize, (PVOID*)&Process->Affinity);
    MM::Allocator::AllocatePool(NonPagedPool, MapSize, (PVOID*)&Process->ActiveProcessors);

    /* Zero the process-level affinity structures */
    RTL::Memory::ZeroMemory(Process->Affinity, MapSize);
    RTL::Memory::ZeroMemory(Process->ActiveProcessors, MapSize);

    /* Initialize size metadata */
    Process->Affinity->Count = (USHORT)AffinitySize;
    Process->Affinity->Size = (USHORT)AffinitySize;
    Process->ActiveProcessors->Size = (USHORT)AffinitySize;

    /* Initialize Idle process */
    KE::KProcess::InitializeProcess(Process, 0, DirectoryTable, FALSE);
    Process->Quantum = MAXCHAR;
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
