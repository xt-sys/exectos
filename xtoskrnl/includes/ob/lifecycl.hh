/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ob/lifecycl.hh
 * DESCRIPTION:     Object Manager Lifecycle Management
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_OB_LIFECYCL_HH
#define __XTOSKRNL_OB_LIFECYCL_HH

#include <xtos.hh>


/* Object Manager */
namespace OB
{
    class LifeCycle
    {
        private:
            STATIC PHANDLE_TABLE KernelHandleTable;
            STATIC PVOID RemoveObjectList;
            STATIC WORK_QUEUE_ITEM RemoveObjectWorkItem;

        public:
            STATIC XTAPI XTSTATUS AllocateObject(IN POBJECT_CREATE_INFORMATION CreateInfo,
                                                 IN KPROCESSOR_MODE OwnerProcessorMode,
                                                 IN POBJECT_TYPE ObjectType,
                                                 IN PUNICODE_STRING ObjectName,
                                                 IN ULONG ObjectBodySize,
                                                 OUT POBJECT_HEADER *ReturnedObjectHeader);
            STATIC XTAPI VOID DeferObjectDeletion(IN POBJECT_HEADER ObjectHeader);
            STATIC XTFASTCALL LONG_PTR DereferenceObject(IN PVOID Object);
            STATIC XTFASTCALL LONG_PTR DereferenceObject(IN PVOID Object,
                                                         IN ULONG Count);
            STATIC XTFASTCALL LONG_PTR DereferenceObjectDeferDelete(IN PVOID Object);
            STATIC XTFASTCALL VOID DereferenceObjectNameInformation(IN POBJECT_HEADER_NAME_INFO NameInfo);
            STATIC XTFASTCALL POBJECT_HEADER_CREATOR_INFO GetObjectCreatorInformation(IN POBJECT_HEADER Header);
            STATIC XTFASTCALL POBJECT_HEADER_NAME_INFO GetObjectNameInformation(IN POBJECT_HEADER Header);
            STATIC XTAPI VOID InitializeObjectLifeCycle(VOID);
            STATIC XTFASTCALL LONG_PTR ReferenceObject(IN PVOID Object);
            STATIC XTFASTCALL LONG_PTR ReferenceObject(IN PVOID Object,
                                                       IN ULONG Count);
            STATIC XTAPI XTSTATUS ReferenceObject(IN HANDLE Handle,
                                                  IN ACCESS_MASK DesiredAccess,
                                                  IN POBJECT_TYPE ObjectType,
                                                  IN KPROCESSOR_MODE AccessMode,
                                                  OUT PVOID *Object,
                                                  OUT POBJECT_HANDLE_INFORMATION HandleInformation);
            STATIC XTFASTCALL POBJECT_HEADER_NAME_INFO ReferenceObjectNameInformation(IN POBJECT_HEADER ObjectHeader);

        private:
            STATIC XTFASTCALL PWCH AllocateObjectName(IN ULONG Length,
                                                      IN BOOLEAN UseLookaside,
                                                      IN OUT PUNICODE_STRING ObjectName);
            STATIC XTAPI VOID CalculateOptionalHeaderSize(IN POBJECT_CREATE_INFORMATION CreateInfo, 
                                                          IN POBJECT_TYPE ObjectType, 
                                                          IN PUNICODE_STRING ObjectName,
                                                          OUT POBJECT_OPTIONAL_HEADER_LAYOUT Layout);
            STATIC XTAPI XTSTATUS CaptureObjectCreateInformation(IN POBJECT_TYPE ObjectType,
                                                                 IN KPROCESSOR_MODE ProcessorMode,
                                                                 IN KPROCESSOR_MODE OwnerProcessorMode,
                                                                 IN POBJECT_ATTRIBUTES ObjectAttributes,
                                                                 IN OUT PUNICODE_STRING CapturedObjectName,
                                                                 IN POBJECT_CREATE_INFORMATION ObjectCreateInfo,
                                                                 IN BOOLEAN UseLookaside);
            STATIC XTAPI XTSTATUS CaptureObjectName(IN KPROCESSOR_MODE ProcessorMode,
                                                    IN PUNICODE_STRING ObjectName,
                                                    IN OUT PUNICODE_STRING CapturedObjectName,
                                                    IN BOOLEAN UseLookaside);
            STATIC XTAPI XTSTATUS CreateObject(IN KPROCESSOR_MODE ProcessorMode,
                                               IN POBJECT_TYPE ObjectType,
                                               IN POBJECT_ATTRIBUTES ObjectAttributes,
                                               IN KPROCESSOR_MODE OwnerProcessorMode,
                                               IN OUT PVOID ParseContext,
                                               IN ULONG ObjectBodySize,
                                               IN ULONG PagedPoolCharge,
                                               IN ULONG NonPagedPoolCharge,
                                               OUT PVOID *Object);
            STATIC XTAPI VOID DeleteObject(IN PVOID Object,
                                           IN BOOLEAN CalledOnWorkerThread);
            STATIC XTAPI VOID FreeObject(IN PVOID Object);
            STATIC XTFASTCALL VOID FreeObjectCreateInformation(IN POBJECT_CREATE_INFORMATION CreateInfo);
            STATIC XTFASTCALL VOID FreeObjectName(IN OUT PUNICODE_STRING ObjectName);
            STATIC XTAPI PVOID GetObjectAllocationBase(IN POBJECT_HEADER ObjectHeader);
            STATIC XTAPI VOID ProcessDeferredDeletionQueue(IN PVOID Parameter);
            STATIC XTFASTCALL VOID ReleaseObjectCreateInformation(IN POBJECT_CREATE_INFORMATION CreateInfo);
            STATIC XTAPI VOID ReturnObjectQuota(IN POBJECT_HEADER ObjectHeader,
                                                IN POBJECT_TYPE ObjectType);
    };
}

#endif /* __XTOSKRNL_OB_LIFECYCL_HH */
