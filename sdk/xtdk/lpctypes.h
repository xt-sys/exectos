/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/lpctypes.h
 * DESCRIPTION:     Local Procedure Call (LPC) structures and definitions
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTDK_LPCTYPES_H
#define __XTDK_LPCTYPES_H

#include <xttypes.h>
#include <xtstruct.h>
#include <iotypes.h>


/* C/C++ specific code */
#ifndef __XTOS_ASSEMBLER__

/* LPC non-paged port queue structure definition */
typedef struct _LPCP_NONPAGED_PORT_QUEUE
{
    KSEMAPHORE Semaphore;
    struct _LPCP_PORT_OBJECT *BackPointer;
} LPCP_NONPAGED_PORT_QUEUE, *PLPCP_NONPAGED_PORT_QUEUE;

/* LPC port queue structure definition */
typedef struct _LPCP_PORT_QUEUE
{
    PLPCP_NONPAGED_PORT_QUEUE NonPagedPortQueue;
    PKSEMAPHORE Semaphore;
    LIST_ENTRY ReceiveHead;
} LPCP_PORT_QUEUE, *PLPCP_PORT_QUEUE;

/* LPC port object structure definition */
typedef struct _LPCP_PORT_OBJECT
{
    struct _LPCP_PORT_OBJECT *ConnectionPort;
    struct _LPCP_PORT_OBJECT *ConnectedPort;
    LPCP_PORT_QUEUE MsgQueue;
    CLIENT_ID Creator;
    PVOID ClientSectionBase;
    PVOID ServerSectionBase;
    PVOID PortContext;
    PETHREAD ClientThread;
    SECURITY_QUALITY_OF_SERVICE SecurityQos;
    SECURITY_CLIENT_CONTEXT StaticSecurity;
    LIST_ENTRY LpcReplyChainHead;
    LIST_ENTRY LpcDataInfoChainHead;
    union
    {
        PEPROCESS ServerProcess;
        PEPROCESS MappingProcess;
    };
    ULONG MaxMessageLength;
    ULONG MaxConnectionInfoLength;
    ULONG Flags;
    KEVENT WaitEvent;
} LPCP_PORT_OBJECT, *PLPCP_PORT_OBJECT;

#endif /* __XTOS_ASSEMBLER__ */
#endif /* __XTDK_LPCTYPES_H */
