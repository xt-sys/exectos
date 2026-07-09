/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ob/data.cc
 * DESCRIPTION:     Object Manager global and static data
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/* Synchronizes shared read and exclusive write access to system device map structures */
KPUSH_LOCK OB::DeviceMap::DeviceMapLock;

/* Indicates whether the system employs unique device maps */
BOOLEAN OB::DeviceMap::UniqueDeviceMaps;

/* System work queue item responsible for scheduling the deferred object deletion */
WORK_QUEUE_ITEM OB::LifeCycle::RemoveObjectWorkItem;

/* Default kernel event utilized by the Object Manager */
KEVENT OB::TypeRegistry::DefaultEvent;

/* Fundamental Type object type */
POBJECT_TYPE OB::TypeRegistry::MasterObjectType;

/* Points to the Object Types directory */
POBJECT_DIRECTORY OB::TypeRegistry::ObjectTypeDirectory;

/* Index-based lookup for all registered object types */
POBJECT_TYPE OB::TypeRegistry::ObjectTypesTable[OBJECT_MAX_DEFINED_OBJECT_TYPES];

/* General lookaside list used for allocation and deallocation of OBJECT_CREATE_INFORMATION structures */
GENERAL_LOOKASIDE OB::Manager::CreateInfoList;

/* General lookaside list used for allocation and deallocation buffers for Unicode object names */
GENERAL_LOOKASIDE OB::Manager::NameBufferList;

/* The list head for tracking objects that have been marked for deferred deletion */
PVOID OB::LifeCycle::RemoveObjectList;
