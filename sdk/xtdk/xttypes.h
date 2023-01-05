/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/xttypes.h
 * DESCRIPTION:     Definitions of basic data types defined by XT
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_XTTYPES_H
#define __XTDK_XTTYPES_H

#include <xttarget.h>


/* Standard C types */
typedef unsigned char BYTE, *PBYTE, *LPBYTE;
typedef char CHAR, *PCHAR, *LPCHAR;
typedef double DOUBLE, *PDOUBLE, *LPDOUBLE;
typedef unsigned long DWORD, *PDWORD, *LPDWORD;
typedef float FLOAT, *PFLOAT, *LPFLOAT;
typedef int INT, *PINT, *LPINT;
typedef long LONG, *PLONG, *LPLONG;
typedef signed char SCHAR, *PSCHAR;
typedef signed long SLONG, *PSLONG;
typedef short SHORT, *PSHORT, *LPSHORT;
typedef unsigned char UCHAR, *PUCHAR;
typedef unsigned int UINT, *PUINT;
typedef unsigned long ULONG, *PULONG, LPULONG;
typedef unsigned short USHORT, *PUSHORT;
typedef unsigned short WORD, *PWORD, *LPWORD;
typedef void VOID, *PVOID, *LPVOID;

/* Constant C types */
typedef const BYTE CBYTE, *PCBYTE, *LPCBYTE;
typedef const CHAR CCHAR, *PCCHAR, *LPCCHAR;
typedef const DOUBLE CDOUBLE, *PCDOUBLE, *LPCDOUBLE;
typedef const DWORD CDWORD, *PCDWORD, *LPCDWORD;
typedef const FLOAT CFLOAT, *PCFLOAT, *LPCFLOAT;
typedef const INT CINT, *PCINT, *LPCINT;
typedef const LONG CLONG, *PCLONG, *LPCLONG;
typedef const SCHAR CSCHAR, *PCSCHAR, *LPCSCHAR;
typedef const SHORT CSHORT, *PCSHORT, *LPCSHORT;
typedef const UCHAR CUCHAR, *PCUCHAR, *LPCUCHAR;
typedef const UINT CUINT, *PCUINT, *LPCUINT;
typedef const ULONG CULONG, *PCULONG, *LPCULONG;
typedef const USHORT CUSHORT, *PCUSHORT, *LPCUSHORT;
typedef const VOID CVOID, *PCVOID, *LPCVOID;
typedef const WORD CWORD, *PCWORD, *LPCWORD;

/* Integer types */
typedef signed char INT8, *PINT8;
typedef unsigned char UINT8, *PUINT8;
typedef signed short INT16, *PINT16;
typedef unsigned short UINT16, *PUINT16;
typedef signed int INT32, *PINT32;
typedef unsigned int UINT32, *PUINT32;
typedef signed long long INT64, *PINT64;
typedef unsigned long long UINT64, *PUINT64;

/* 64-bit types */
typedef long long LONG64, *PLONG64;
typedef long long LONGLONG, *PLONGLONG;
typedef unsigned long long ULONGLONG, *PULONGLONG;

/* Signed 32-bit wide types */
typedef signed int LONG32, *PLONG32;

/* Unsigned 32-bit wide types */
typedef unsigned int DWORD32, *PDWORD32;
typedef unsigned int ULONG32, *PULONG32;

/* Unsigned 64-bit wide types */
typedef unsigned long long DWORD64, *PDWORD64;
typedef unsigned long long ULONG64, *PULONG64;

/* Pointer size guaranteed types */
#if defined(_XT64)
    typedef long long INT_PTR, *PINT_PTR;
    typedef unsigned long long UINT_PTR, *PUINT_PTR;
    typedef long long LONG_PTR, *PLONG_PTR;
    typedef unsigned long long ULONG_PTR, *PULONG_PTR;
    typedef unsigned long long POINTER_64_INT;
#else
    typedef int INT_PTR, *PINT_PTR;
    typedef unsigned int UINT_PTR, *PUINT_PTR;
    typedef long LONG_PTR, *PLONG_PTR;
    typedef unsigned long ULONG_PTR, *PULONG_PTR;
    typedef unsigned long POINTER_64_INT;
#endif

/* Flag (bitfield) types */
typedef BYTE FCHAR;
typedef DWORD FLONG;
typedef WORD FSHORT;

/* SIZE_T types */
typedef ULONG_PTR SIZE_T, *PSIZE_T;
typedef LONG_PTR SSIZE_T, *PSSIZE_T;

/* Pointer to PVOID */
typedef PVOID *PPVOID;

/* Polymorphic values types */
typedef SLONG HPARAM, HRESULT;
typedef LONG_PTR LPARAM, LRESULT;
typedef UINT_PTR WPARAM, WRESULT;

/* Handle types */
typedef VOID *HANDLE, **PHANDLE;
typedef HANDLE *SPHANDLE, *LPHANDLE;

/* XT status types */
typedef LONG XTSTATUS, *PXTSTATUS;

/* ANSI character types */
typedef CHAR *PCH, *LPCH;
typedef CHAR *PCCH, *LPCCH;
typedef CHAR *PSTR, *LPSTR, *NPSTR;
typedef PSTR *PZPSTR;
typedef const PSTR *PCZPSTR;
typedef const CHAR *PCSTR, *LPCSTR;
typedef PCSTR *PZPCSTR;

/* ASCIIZ character types */
typedef CHAR SZ, *PSZ;
typedef const CHAR CSZ, *PCSZ;

/* UNICODE character types */
typedef USHORT WCHAR, *PWCHAR;
typedef WCHAR *PWCH, *LPWCH;
typedef const WCHAR *PCWCH, *LPCWCH;
typedef WCHAR *PWSTR, *LPWSTR, *NWPSTR;
typedef PWSTR *PZPWSTR;
typedef const PWSTR *PCZPWSTR;
typedef WCHAR *PUWSTR, *LPUWSTR;
typedef const WCHAR *PCWSTR, *LPCWSTR;
typedef PCWSTR *PZPCWSTR;
typedef const WCHAR *PCUWSTR, *LPCUWSTR;

/* Neutral character types */
typedef WCHAR TBYTE, *PTBYTE;
typedef WCHAR TCHAR, *PTCHAR;
typedef LPWSTR LP;
typedef LPWSTR PTCH, LPTCH;
typedef LPWSTR PTSTR, LPTSTR;
typedef LPCWSTR PCTSTR, LPCTSTR;
typedef LPUWSTR PUTSTR, LPUTSTR;
typedef LPCUWSTR PCUTSTR, LPCUTSTR;

/* Boolean type */
typedef enum _BOOLEAN
{
    FALSE = 0,
    TRUE = 1
} BOOLEAN, *PBOOLEAN;

/* 128-bit floats structure */
typedef struct _FLOAT128
{
    LONGLONG LowPart;
    LONGLONG HighPart;
} FLOAT128, *PFLOAT128;

/* 64-bit signed integer union */
typedef union _LARGE_INTEGER
{
    struct
    {
        ULONG LowPart;
        LONG HighPart;
    };
    struct
    {
        ULONG LowPart;
        LONG HighPart;
    } u;
    LONGLONG QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;

/* 64-bit unsigned integer union */
typedef union _ULARGE_INTEGER
{
    struct
    {
        ULONG LowPart;
        ULONG HighPart;
    };
    struct
    {
        ULONG LowPart;
        ULONG HighPart;
    } u;
    ULONGLONG QuadPart;
} ULARGE_INTEGER, *PULARGE_INTEGER;

/* Counted string structure */
typedef struct _STRING
{
    USHORT Length;
    USHORT MaximumLength;
    PCHAR Buffer;
} STRING, *PSTRING;

/* 32-bit counted string structure */
typedef struct _STRING32
{
    USHORT Length;
    USHORT MaximumLength;
    ULONG Buffer;
} STRING32, *PSTRING32;

/* 64-bit counted string structure */
typedef struct _STRING64
{
    USHORT Length;
    USHORT MaximumLength;
    ULONGLONG Buffer;
} STRING64, *PSTRING64;

/* Constant counted string structure */
typedef struct _CSTRING
{
    USHORT Length;
    USHORT MaximumLength;
    PCCHAR Buffer;
} CSTRING, *PCSTRING;

/* ANSI string structure */
typedef struct _ANSI_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    PSTR Buffer;
} ANSI_STRING, *PANSI_STRING;
typedef const ANSI_STRING *PCANSI_STRING;

/* 32-bit ANSI string structure */
typedef struct _ANSI_STRING32
{
    USHORT Length;
    USHORT MaximumLength;
    ULONG Buffer;
} ANSI_STRING32, *PANSI_STRING32;
typedef const ANSI_STRING32 *PCANSI_STRING32;

/* 64-bit ANSI string structure */
typedef struct _ANSI_STRING64
{
    USHORT Length;
    USHORT MaximumLength;
    ULONGLONG Buffer;
} ANSI_STRING64, *PANSI_STRING64;
typedef const ANSI_STRING64 *PCANSI_STRING64;

/* UNICODE string structure */
typedef struct _UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;
typedef const UNICODE_STRING *PCUNICODE_STRING;

/* 32-bit UNICODE string structure */
typedef struct _UNICODE_STRING32
{
    USHORT Length;
    USHORT MaximumLength;
    ULONG Buffer;
} UNICODE_STRING32, *PUNICODE_STRING32;
typedef const UNICODE_STRING32 *PCUNICODE_STRING32;

/* 64-bit UNICODE string structure */
typedef struct _UNICODE_STRING64
{
    USHORT Length;
    USHORT MaximumLength;
    ULONGLONG Buffer;
} UNICODE_STRING64, *PUNICODE_STRING64;
typedef const UNICODE_STRING64 *PCUNICODE_STRING64;

#endif /* __XTDK_XTTYPES_H */
