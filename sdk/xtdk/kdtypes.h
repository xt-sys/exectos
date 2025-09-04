/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/kdtypes.h
 * DESCRIPTION:     Kernel Debugger data structures
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTDK_KDTYPES_H
#define __XTDK_KDTYPES_H

#include <xtbase.h>
#include <xtstruct.h>
#include <rtltypes.h>


/* Number of debug providers */
#define KDBG_PROVIDERS_COUNT                                2

/* Debug providers bitmask definitions */
#define DEBUG_PROVIDER_COMPORT                              0x00000001
#define DEBUG_PROVIDER_FRAMEBUFFER                          0x00000002

/* Kernel routine callbacks */
typedef XTSTATUS (XTAPI *PKD_INIT_ROUTINE)();
typedef VOID (*PKD_PRINT_ROUTINE)(IN PCWSTR Format, IN ...);

/* Debug mode structure definition */
typedef struct _KD_DEBUG_MODE
{
    BOOLEAN Enabled;
    ULONG Mode;
    ULONG ComPortAddress;
    ULONG ComPortNumber;
    ULONG ComPortBaudRate;
} KD_DEBUG_MODE, *PKD_DEBUG_MODE;

/* Kernel debugger dispatch table structure definition */
typedef struct _KD_DISPATCH_TABLE
{
    LIST_ENTRY ListEntry;
    RTL_PRINT_CONTEXT PrintContext;
} KD_DISPATCH_TABLE, *PKD_DISPATCH_TABLE;

#endif /* __XTDK_KDTYPES_H */
