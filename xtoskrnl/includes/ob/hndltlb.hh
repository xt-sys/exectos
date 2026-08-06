/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ob/hndltlb.hh
 * DESCRIPTION:     Object Manager Handle Table Management Interface
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_OB_HNDLTLB_HH
#define __XTOSKRNL_OB_HNDLTLB_HH

#include <xtos.hh>


/* Object Manager */
namespace OB
{
    class HandleTable
    {
        private:
            STATIC KPUSH_LOCK HandleTableListLock;
            STATIC LIST_ENTRY HandleTableListHead;

        public:
            STATIC XTAPI BOOLEAN ChangeHandle(IN PHANDLE_TABLE HandleTable,
                                              IN HANDLE Handle,
                                              IN POB_CHANGE_HANDLE_ROUTINE ChangeRoutine,
                                              IN ULONG_PTR Parameter);
            STATIC XTAPI HANDLE CreateHandle(IN PHANDLE_TABLE HandleTable,
                                             IN PHANDLE_TABLE_ENTRY HandleTableEntry);
            STATIC XTAPI PHANDLE_TABLE CreateHandleTable(IN PEPROCESS Process);
            STATIC XTAPI BOOLEAN DestroyHandle(IN PHANDLE_TABLE HandleTable,
                                               IN HANDLE Handle,
                                               IN PHANDLE_TABLE_ENTRY HandleTableEntry);
            STATIC XTAPI VOID DestroyHandleTable(IN PHANDLE_TABLE HandleTable,
                                                 IN POB_DESTROY_HANDLE_ROUTINE DestroyHandleProcedure);
            STATIC XTAPI PHANDLE_TABLE DuplicateHandleTable(IN PEPROCESS Process,
                                                            IN PHANDLE_TABLE OldHandleTable,
                                                            IN POB_DUPLICATE_HANDLE_ROUTINE DupHandleProcedure,
                                                            IN ULONG_PTR Mask);
            STATIC XTAPI BOOLEAN EnumerateHandleTable(IN PHANDLE_TABLE HandleTable,
                                                 IN POB_ENUMERATE_HANDLE_ROUTINE EnumHandleProcedure,
                                                 IN PVOID EnumParameter,
                                                 OUT PHANDLE Handle);
            STATIC XTAPI PHANDLE_TABLE_ENTRY_INFO GetHandleInformation(IN PHANDLE_TABLE HandleTable,
                                                                       IN HANDLE Handle,
                                                                       IN BOOLEAN Locked);
            STATIC XTAPI VOID InitializeHandleTable(VOID);
            STATIC XTAPI BOOLEAN LockHandleTableEntry(IN PHANDLE_TABLE HandleTable,
                                                      IN PHANDLE_TABLE_ENTRY HandleTableEntry);
            STATIC XTAPI PHANDLE_TABLE_ENTRY MapHandleToPointer(IN PHANDLE_TABLE HandleTable,
                                                                IN HANDLE Handle);
            STATIC XTAPI PHANDLE_TABLE_ENTRY MapHandleToPointer(IN PHANDLE_TABLE HandleTable,
                                                                IN HANDLE Handle,
                                                                IN KPROCESSOR_MODE PreviousMode);
            STATIC XTAPI VOID RemoveHandleTable(IN PHANDLE_TABLE HandleTable);
            STATIC XTAPI XTSTATUS SetHandleInformation(IN PHANDLE_TABLE HandleTable,
                                                       IN HANDLE Handle,
                                                       IN PHANDLE_TABLE_ENTRY_INFO EntryInfo,
                                                       IN BOOLEAN EntryLocked);
            STATIC XTAPI XTSTATUS SnapshotHandleTables(IN POB_SNAPSHOT_HANDLE_ENTRY SnapshotHandleEntry,
                                                       IN OUT PSYSTEM_HANDLE_INFORMATION HandleInformation,
                                                       IN ULONG Length,
                                                       IN OUT PULONG RequiredLength);
            STATIC XTAPI XTSTATUS SnapshotHandleTables(IN POB_SNAPSHOT_HANDLE_ENTRY_EX SnapshotHandleEntry,
                                                       IN OUT PSYSTEM_HANDLE_INFORMATION_EX HandleInformation,
                                                       IN ULONG Length,
                                                       IN OUT PULONG RequiredLength);
            STATIC XTAPI VOID SweepHandleTable(IN PHANDLE_TABLE HandleTable,
                                               IN POB_ENUMERATE_HANDLE_ROUTINE EnumHandleProcedure,
                                               IN PVOID EnumParameter);
            STATIC XTAPI VOID UnlockHandleTableEntry(IN PHANDLE_TABLE HandleTable,
                                                     IN PHANDLE_TABLE_ENTRY HandleTableEntry);

        private:
            STATIC XTAPI PHANDLE_TABLE AllocateHandleTable(IN PEPROCESS Process,
                                                           IN BOOLEAN DoInit);
            STATIC XTAPI PHANDLE_TABLE_ENTRY AllocateHandleTableEntry(IN PHANDLE_TABLE HandleTable,
                                                                      OUT POBJECT_HANDLE Handle);
            STATIC XTAPI BOOLEAN AllocateHandleTableEntry(IN PHANDLE_TABLE HandleTable,
                                                          IN BOOLEAN DoInit);
            STATIC XTAPI PHANDLE_TABLE_ENTRY AllocateLowLevelTable(IN PHANDLE_TABLE HandleTable,
                                                                   IN BOOLEAN DoInit);
            STATIC XTAPI PHANDLE_TABLE_ENTRY *AllocateMediumLevelTable(IN PHANDLE_TABLE HandleTable,
                                                                       IN BOOLEAN DoInit,
                                                                       OUT PHANDLE_TABLE_ENTRY *NewLowLevel);
            STATIC XTAPI PVOID AllocateTableMemory(IN PEPROCESS QuotaProcess,
                                                   IN SIZE_T NumberOfBytes,
                                                   IN BOOLEAN ZeroMemory);
            STATIC XTAPI VOID FreeHandleTable(IN PHANDLE_TABLE HandleTable);
            STATIC XTAPI VOID FreeHandleTableEntry(IN PHANDLE_TABLE HandleTable,
                                                   IN OBJECT_HANDLE Handle,
                                                   IN PHANDLE_TABLE_ENTRY HandleTableEntry);
            STATIC XTAPI VOID FreeLowLevelTable(IN PEPROCESS QuotaProcess,
                                                IN PHANDLE_TABLE_ENTRY TableLevel1);
            STATIC XTAPI VOID FreeTableMemory(IN PEPROCESS QuotaProcess,
                                              IN PVOID PoolMemory,
                                              IN SIZE_T NumberOfBytes);
            STATIC XTAPI PHANDLE_TABLE_ENTRY LookupHandleTableEntry(IN PHANDLE_TABLE HandleTable,
                                                                    IN OBJECT_HANDLE Handle);
            STATIC XTAPI ULONG MoveFreeHandles(IN PHANDLE_TABLE HandleTable);
            STATIC XTINLINE BOOLEAN ValidateObjectEntry(IN PHANDLE_TABLE_ENTRY Entry);
    };
}

#endif /* __XTOSKRNL_OB_HNDLTLB_HH */
