/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ob/typereg.hh
 * DESCRIPTION:     Object Manager Type Registry
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_OB_TYPEREG_HH
#define __XTOSKRNL_OB_TYPEREG_HH

#include <xtos.hh>


/* Object Manager */
namespace OB
{
    class TypeRegistry
    {
        private:
            STATIC KEVENT DefaultEvent;
            STATIC POBJECT_TYPE MasterObjectType;
            STATIC POBJECT_DIRECTORY ObjectTypeDirectory;
            STATIC POBJECT_TYPE ObjectTypesTable[OBJECT_MAX_DEFINED_OBJECT_TYPES];

        public:
            STATIC XTAPI XTSTATUS CreateObjectType(IN PUNICODE_STRING TypeName,
                                                   IN POBJECT_TYPE_INITIALIZER ObjectTypeInitializer,
                                                   IN PSECURITY_DESCRIPTOR SecurityDescriptor,
                                                   OUT POBJECT_TYPE *ObjectType);
            STATIC XTAPI VOID DeleteObjectType(IN PVOID Object);
            STATIC XTAPI XTSTATUS InitializeObjectTypeRegistry(VOID);

        private:
            STATIC XTAPI ULONG GenerateObjectPoolTag(IN PUNICODE_STRING TypeName);
            STATIC XTAPI VOID InitializeObjectType(IN OUT POBJECT_TYPE ObjectType,
                                                   IN POBJECT_TYPE_INITIALIZER Initializer,
                                                   IN PUNICODE_STRING ObjectName);
            STATIC XTAPI XTSTATUS ValidateObjectTypeParameters(IN PUNICODE_STRING TypeName,
                                                               IN POBJECT_TYPE_INITIALIZER Initializer);
    };
}

#endif /* __XTOSKRNL_OB_TYPEREG_HH */
