/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/kprocess.c
 * DESCRIPTION:     XT kernel process manipulation support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Initializes the process.
 *
 * @param Process
 *        Supplies a pointer to process that will be initialized.
 *
 * @param Priority
 *        Specifies the process priority.
 *
 * @param Affinity
 *        Specifies a process affinity designating processors on which process can run.
 *
 * @param DirectoryTable
 *        Supplies a pointer to the directory table.
 *
 * @param Alignment
 *        Specifies the exceptions alignment of the process.
 *
 * @return This routine does not return any value.
 *
 * @since NT 3.5
 */
XTAPI
VOID
KeInitializeProcess(IN OUT PKPROCESS Process,
                    IN KPRIORITY Priority,
                    IN KAFFINITY Affinity,
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
    Process->Affinity = Affinity;
    Process->AutoAlignment = Alignment;
    Process->DirectoryTable[0] = DirectoryTable[0];
    Process->DirectoryTable[1] = DirectoryTable[1];
    Process->StackCount = MAXSHORT;

    /* Set thread quantum */
    Process->Quantum = THREAD_QUANTUM;

    /* Set IOPM offset */
    Process->IopmOffset = sizeof(KTSS);

    /* Set initial process state */
    Process->State = ProcessInMemory;
}
