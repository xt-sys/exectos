/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/includes/libxtos.hh
 * DESCRIPTION:     XT Loader to LIBXTOS interface
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTLDR_LIBXTOS_HH
#define __XTLDR_LIBXTOS_HH

#include <xtblapi.h>


/* Minimal forward references for AR classes used by XTLDR */
namespace AR
{
    class CpuFunc
    {
        public:
            STATIC XTCDECL BOOLEAN CpuId(IN OUT PCPUID_REGISTERS Registers);
            STATIC XTCDECL ULONG_PTR ReadControlRegister(IN USHORT ControlRegister);
            STATIC XTCDECL ULONGLONG ReadModelSpecificRegister(IN ULONG Register);
            STATIC XTCDECL VOID WriteControlRegister(IN USHORT ControlRegister,
                                                     IN UINT_PTR Value);
    };
}

/* Minimal forward references for HL classes used by XTLDR */
namespace HL
{
    class ComPort
    {
        public:
            STATIC XTCDECL XTSTATUS InitializeComPort(IN OUT PCPPORT Port,
                                                      IN PUCHAR PortAddress,
                                                      IN ULONG BaudRate);
            STATIC XTCDECL XTSTATUS WriteComPort(IN PCPPORT Port,
                                                 IN UCHAR Byte);
    };

    class IoPort
    {
        public:
            STATIC XTCDECL UCHAR ReadPort8(IN USHORT Port);
            STATIC XTCDECL USHORT ReadPort16(IN USHORT Port);
            STATIC XTCDECL ULONG ReadPort32(IN USHORT Port);
            STATIC XTCDECL VOID WritePort8(IN USHORT Port,
                                           IN UCHAR Value);
            STATIC XTCDECL VOID WritePort16(IN USHORT Port,
                                            IN USHORT Value);
            STATIC XTCDECL VOID WritePort32(IN USHORT Port,
                                            IN ULONG Value);
    };
}

/* Minimal forward references for RTL classes used by XTLDR */
namespace RTL
{
    class Guid
    {
        public:
            STATIC XTAPI BOOLEAN CompareGuids(IN PGUID Guid1,
                                              IN PGUID Guid2);
    };

    class LinkedList
    {
        public:
            STATIC XTCDECL VOID InitializeListHead(IN PLIST_ENTRY ListHead);
            STATIC XTCDECL VOID InsertHeadList(IN OUT PLIST_ENTRY ListHead,
                                               IN PLIST_ENTRY Entry);
            STATIC XTCDECL VOID InsertTailList(IN OUT PLIST_ENTRY ListHead,
                                               IN PLIST_ENTRY Entry);
            STATIC XTCDECL VOID RemoveEntryList(IN PLIST_ENTRY Entry);
    };

    class Memory
    {
        public:
            STATIC XTAPI SIZE_T CompareMemory(IN PCVOID LeftBuffer,
                                              IN PCVOID RightBuffer,
                                              IN SIZE_T Length);
            STATIC XTAPI VOID CopyMemory(OUT PVOID Destination,
                                         IN PCVOID Source,
                                         IN SIZE_T Length);
            STATIC XTAPI VOID MoveMemory(OUT PVOID Destination,
                                         IN PCVOID Source,
                                         IN SIZE_T Length);
            STATIC XTAPI VOID SetMemory(OUT PVOID Destination,
                                        IN UCHAR Byte,
                                        IN SIZE_T Length);
            STATIC XTAPI VOID ZeroMemory(OUT PVOID Destination,
                                         IN SIZE_T Length);
    };

    class String
    {
        public:
            STATIC XTAPI SIZE_T CompareString(IN PCSTR String1,
                                              IN PCSTR String2,
                                              IN SIZE_T Length);
            STATIC XTAPI SIZE_T StringLength(IN PCSTR String,
                                             IN SIZE_T MaxLength);
            STATIC XTAPI SIZE_T StringToWideString(OUT PWCHAR Destination,
                                                   IN PCSTR *Source,
                                                   IN SIZE_T Length);
            STATIC XTAPI PCHAR TrimString(IN PCHAR String);
    };

    class WideString
    {
        public:
            STATIC XTAPI SIZE_T CompareWideString(IN PCWSTR String1,
                                                  IN PCWSTR String2,
                                                  IN SIZE_T Length);
            STATIC XTAPI SIZE_T CompareWideStringInsensitive(IN PCWSTR String1,
                                                             IN PCWSTR String2,
                                                             IN SIZE_T Length);
            STATIC XTAPI PWCHAR ConcatenateWideString(OUT PWCHAR Destination,
                                                      IN PWCHAR Source,
                                                      IN SIZE_T Count);
            STATIC XTAPI XTSTATUS FormatWideString(IN PRTL_PRINT_CONTEXT Context,
                                                   IN PCWSTR Format,
                                                   IN VA_LIST ArgumentList);
            STATIC XTAPI PWCHAR TokenizeWideString(IN PWCHAR String,
                                                   IN PCWSTR Delimiter,
                                                   IN OUT PWCHAR *SavePtr);
            STATIC XTAPI SIZE_T WideStringLength(IN PCWSTR String,
                                                 IN SIZE_T MaxLength);
    };
}

#endif /* __XTLDR_LIBXTOS_HH */
