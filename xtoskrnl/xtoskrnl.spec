@ cdecl HlIoPortInByte(ptr)
@ cdecl HlIoPortInLong(ptr)
@ cdecl HlIoPortInShort(ptr)
@ cdecl HlIoPortOutByte(ptr long)
@ cdecl HlIoPortOutLong(ptr long)
@ cdecl HlIoPortOutShort(ptr long)
@ stdcall KeInitializeSemaphore(ptr long long)
@ stdcall KeInitializeSpinLock(ptr)
@ stdcall KeInitializeTimer(ptr)
@ stdcall KeInitializeTimerEx(ptr long)
@ stdcall KeReadSemaphoreState(ptr)
@ stdcall KeReleaseSemaphore(ptr long long long)
@ stdcall RtlCompareMemory(ptr ptr long)
@ stdcall RtlCopyMemory(ptr ptr long)
@ stdcall RtlFillMemory(ptr long long)
@ stdcall RtlMoveMemory(ptr ptr long)
@ stdcall RtlSameMemory(ptr ptr long)
@ stdcall RtlSetMemory(ptr long long)
@ cdecl RtlStringLength(str long)
@ cdecl RtlStringToWideString(wstr str long)
@ cdecl RtlWideStringCompare(wstr wstr long)
@ cdecl RtlWideStringConcatenate(wstr wstr long)
@ cdecl RtlWideStringLength(wstr long)
@ cdecl RtlWideStringTokenize(wstr wstr wstr)
@ stdcall RtlZeroMemory(ptr long)