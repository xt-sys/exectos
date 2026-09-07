/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/cmtypes.h
 * DESCRIPTION:     Configuration Manager structures definitions
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTDK_CMTYPES_H
#define __XTDK_CMTYPES_H

#include <xttypes.h>
#include <xtstruct.h>
#include <mmtypes.h>


/* C/C++ specific code */
#ifndef __XTOS_ASSEMBLER__

/* Partial resource descriptor */
typedef struct _CM_PARTIAL_RESOURCE_DESCRIPTOR
{
    UCHAR Type;
    UCHAR ShareDisposition;
    USHORT Flags;
    union
    {
        struct
        {
            PHYSICAL_ADDRESS Start;
            ULONG Length;
        } Generic;
        struct
        {
            PHYSICAL_ADDRESS Start;
            ULONG Length;
        } Port;
        struct
        {
            USHORT Level;
            USHORT Group;
            ULONG Vector;
            PKAFFINITY_MAP Affinity;
        } Interrupt;
        struct
        {
            union
            {
                struct
                {
                    USHORT Group;
                    USHORT MessageCount;
                    ULONG Vector;
                    PKAFFINITY_MAP Affinity;
                } Raw;
                struct
                {
                    USHORT Level;
                    USHORT Group;
                    ULONG Vector;
                    PKAFFINITY_MAP Affinity;
                } Translated;
            };
        } MessageInterrupt;
        struct
        {
            PHYSICAL_ADDRESS Start;
            ULONG Length;
        } Memory;
        struct
        {
            ULONG Channel;
            ULONG Port;
            ULONG Reserved1;
        } Dma;
        struct
        {
            ULONG Data[3];
        } DevicePrivate;
        struct
        {
            ULONG Start;
            ULONG Length;
            ULONG Reserved;
        } BusNumber;
        struct
        {
            ULONG DataSize;
            ULONG Reserved1;
            ULONG Reserved2;
        } DeviceSpecificData;
        struct
        {
            PHYSICAL_ADDRESS Start;
            ULONG Length40;
        } Memory40;
        struct
        {
            PHYSICAL_ADDRESS Start;
            ULONG Length48;
        } Memory48;
        struct
        {
            PHYSICAL_ADDRESS Start;
            ULONG Length64;
        } Memory64;
    } u;
} CM_PARTIAL_RESOURCE_DESCRIPTOR, *PCM_PARTIAL_RESOURCE_DESCRIPTOR;

/* Partial resource list */
typedef struct _CM_PARTIAL_RESOURCE_LIST
{
    USHORT Version;
    USHORT Revision;
    ULONG Count;
    CM_PARTIAL_RESOURCE_DESCRIPTOR PartialDescriptors[1];
} CM_PARTIAL_RESOURCE_LIST, *PCM_PARTIAL_RESOURCE_LIST;

/* Full resource descriptor */
typedef struct _CM_FULL_RESOURCE_DESCRIPTOR
{
    INTERFACE_TYPE InterfaceType;
    ULONG BusNumber;
    CM_PARTIAL_RESOURCE_LIST PartialResourceList;
} CM_FULL_RESOURCE_DESCRIPTOR, *PCM_FULL_RESOURCE_DESCRIPTOR;

/* Full resource list */
typedef struct _CM_RESOURCE_LIST
{
    ULONG Count;
    CM_FULL_RESOURCE_DESCRIPTOR List[1];
} CM_RESOURCE_LIST, *PCM_RESOURCE_LIST;

#endif /* __XTOS_ASSEMBLER__ */
#endif /* __XTDK_CMTYPES_H */
