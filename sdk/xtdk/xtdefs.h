/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/xtdefs.h
 * DESCRIPTION:     XT definitions for basic types
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_XTDEFS_H
#define __XTDK_XTDEFS_H


/* Routines and arguments modifiers */
#define IN
#define OUT
#define XTAPI                                  __stdcall
#define XTCDECL                                __cdecl
#define XTFASTCALL                             __fastcall
#define XTVECTORCALL                           __vectorcall
#define XTINLINE                               __inline
#define XTASSEMBLY                             __attribute__((naked))
#define XTINTERRUPT                            __attribute__((interrupt))

/* Variable modifiers */
#define CONST                                  const
#define EXTERN                                 extern
#define STRUCT                                 struct
#define STATIC                                 static
#define UNION                                  union
#define VOLATILE                               volatile

/* NULL values */
#define NULL                                   ((PVOID) 0)
#define NULL64                                 ((VOID * PVOID) 0)

/* Type limits */
#define MINCHAR                                0x80
#define MAXCHAR                                0x7F
#define MINSHORT                               0x8000
#define MAXSHORT                               0x7FFF
#define MINLONG                                0x80000000
#define MAXLONG                                0x7FFFFFFF
#define MAXULONG                               0xFFFFFFFF

/* Pointer limits */
#define MININT_PTR                             (~MAXINT_PTR)
#define MAXINT_PTR                             ((INT_PTR)(MAXUINT_PTR >> 1))
#define MAXUINT_PTR                            (~((UINT_PTR)0))
#define MINLONG_PTR                            (~MAXLONG_PTR)
#define MAXLONG_PTR                            ((LONG_PTR)(MAXULONG_PTR >> 1))
#define MAXULONG_PTR                           (~((ULONG_PTR)0))

/* Number of bits per byte */
#define BITS_PER_BYTE                          8

/* Preprocessor macros for defining an additional compiler attributes */
#define ALIGN(Alignment)                       __attribute__((aligned(Alignment)))
#define PACKED                                 __attribute__((packed))
#define SEGMENT(Segment)                       __attribute__((section(Segment)))
#define USED                                   __attribute__((__used__))

/* Macro for calculating size of an array */
#define ARRAY_SIZE(Array)                      (sizeof(Array) / sizeof(*Array))

/* Macro for concatenating two strings */
#define CONCAT_STRING(Str1, Str2)              Str1##Str2
#define CONCATENATE(Str1, Str2)                CONCAT_STRING(Str1, Str2)

/* Macro for accessing the base address of a structure from a structure member */
#define CONTAIN_RECORD(Address, Type, Field)   ((Type *)(((ULONG_PTR)Address) - (ULONG_PTR)(&(((Type *)0)->Field))))

/* EFI size to pages conversion macro */
#define EFI_SIZE_TO_PAGES(Size)                (((Size) >> EFI_PAGE_SHIFT) + (((Size) & EFI_PAGE_MASK) ? 1 : 0))

/* Macro for calculating byte offset of a field in the structure */
#define FIELD_OFFSET(Structure, Field)         ((LONG)(LONG_PTR)&(((Structure *)0)->Field))

/* Macro for calculating size of a field in the structure */
#define FIELD_SIZE(Structure, Field)           (sizeof(((Structure *)0)->Field))

/* Macro that yields field type in the structure */
#define FIELD_TYPE(Structure, Field)           (((Structure*)0)->Field)

/* Macro that page-aligns a virtual address */
#define PAGE_ALIGN(VirtualAddress)             ((PVOID)((ULONG_PTR)VirtualAddress & ~MM_PAGE_MASK))

/* Macro that returns offset of the virtual address */
#define PAGE_OFFSET(VirtualAddress)            ((ULONG)((ULONG_PTR)VirtualAddress & MM_PAGE_MASK))

/* Macro for rounding down */
#define ROUND_DOWN(Value, Alignment)           ((Value) & ~((Alignment) - 1))

/* Macro for rounding up */
#define ROUND_UP(Value, Alignment)             ROUND_DOWN((Value) + (Alignment - 1), Alignment)

/* Macros for defining signatures built from ASCII characters */
#define SIGNATURE16(A, B)                      ((A) | (B << 8))
#define SIGNATURE32(A, B, C, D)                (SIGNATURE16(A, B) | (SIGNATURE16(C, D) << 16))
#define SIGNATURE64(A, B, C, D, E, F, G, H)    (SIGNATURE32(A, B, C, D) | ((UINT64)(SIGNATURE32(E, F, G, H)) << 32))

/* XT size to pages conversion macro */
#define SIZE_TO_PAGES(Size)                    (((Size) >> MM_PAGE_SHIFT) + (((Size) & (MM_PAGE_MASK)) ? 1 : 0))

/* Macros for concatenating strings */
#define STRINGIFY(String...)                   STRINGIZE(String)
#define STRINGIZE(String...)                   #String

/* Macro for generating unique identifiers */
#define UNIQUE(Prefix)                         CONCATENATE(CONCATENATE(__UNIQUE_ID_, Prefix), __COUNTER__)

/* Variadic ABI functions */
typedef __builtin_va_list VA_LIST, *PVA_LIST;
#define VA_ARG(Marker, Type)                   ((sizeof (Type) < sizeof(UINT_PTR)) ? \
                                               (Type)(__builtin_va_arg(Marker, UINT_PTR)) : \
                                               (Type)(__builtin_va_arg(Marker, Type)))
#define VA_COPY(Dest, Start)                   __builtin_va_copy(Dest, Start)
#define VA_START(Marker, Parameter)            __builtin_va_start(Marker, Parameter)
#define VA_END(Marker)                         __builtin_va_end(Marker)

/* Data conversion macros */
#define HandleToLong(Var)                      ((LONG)(LONG_PTR)Var)
#define HandleToUlong(Var)                     ((ULONG)(ULONG_PTR)Var)
#define IntToPtr(Var)                          ((PVOID)(INT_PTR)Var)
#define LongToHandle(Var)                      ((HANDLE)(LONG_PTR)Var)
#define LongToPtr(Var)                         ((PVOID)(LONG_PTR)Var)
#define PtrToInt(Var)                          ((INT)(INT_PTR)Var)
#define PtrToLong(Var)                         ((LONG)(LONG_PTR)Var)
#define PtrToShort(Var)                        ((SHORT)(LONG_PTR)Var)
#define PtrToUint(Var)                         ((UINT)(UINT_PTR)Var)
#define PtrToUlong(Var)                        ((ULONG)(ULONG_PTR)Var)
#define PtrToUshort(Var)                       ((USHORT)(ULONG_PTR)Var)
#define UintToPtr(Var)                         ((PVOID)(UINT_PTR)Var)
#define UlongToHandle(Var)                     ((HANDLE)(ULONG_PTR)Var)
#define UlongToPtr(Var)                        ((PVOID)(ULONG_PTR)Var)

#endif /* __XTDK_XTDEFS_H */
