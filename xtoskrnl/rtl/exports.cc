/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/exports.cc
 * DESCRIPTION:     C-compatible API wrappers for exported kernel functions
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Clears all bits in the bit map.
 *
 * @param BitMap
 *        Supplies a pointer to the bit map.
 *
 * @return This routine does not return any value.
 *
 * @since NT 3.5
 */
XTCLINK
XTAPI
VOID
RtlClearAllBits(IN PRTL_BITMAP BitMap)
{
    RTL::BitMap::ClearAllBits(BitMap);
}

/**
 * Clears a single bit in the bit map.
 *
 * @param BitMap
 *        Supplies a pointer to the bit map.
 *
 * @param Bit
 *        Specifies the number of the bit to be cleared.
 *
 * @return This routine does not return any value.
 *
 * @since NT 5.1
 */
XTCLINK
XTAPI
VOID
RtlClearBit(IN PRTL_BITMAP BitMap,
            IN ULONG_PTR Bit)
{
    RTL::BitMap::ClearBit(BitMap, Bit);
}

/**
 * Clears a specified set of bits within a bit map.
 *
 * @param BitMap
 *        Supplies a pointer to the bit map.
 *
 * @param StartingIndex
 *        Supplies the starting index of the first bit to clear.
 *
 * @param Length
 *        Supplies the length (number of bits) to clear.
 *
 * @return This routine does not return any value.
 *
 * @since NT 3.5
 */
XTCLINK
XTAPI
VOID
RtlClearBits(IN PRTL_BITMAP BitMap,
             IN ULONG_PTR StartingIndex,
             IN ULONG_PTR Length)
{
    RTL::BitMap::ClearBits(BitMap, StartingIndex, Length);
}

/**
 * Searches the bit map for a contiguous region of set bits and clears them.
 *
 * @param BitMap
 *        Supplies a pointer to the bit map.
 *
 * @param Length
 *        Supplies the length of contiguous region (number of set bits) to look for.
 *
 * @param Index
 *        Supplies the index of the first bit to start the search at a given position.
 *
 * @return This routine returns the bit map index position of the contiguous region found, or MAXULONG_PTR if not found.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
ULONG
RtlClearSetBits(IN PRTL_BITMAP BitMap,
                IN ULONG_PTR Length,
                IN ULONG_PTR Index)
{
    return RTL::BitMap::ClearSetBits(BitMap, Length, Index);
}

/**
 * Compares two GUIDs (Globally Unique Identifiers).
 *
 * @param Guid1
 *        Supplies the first GUID to compare.
 *
 * @param Guid2
 *        Supplies the second GUID to compare.
 *
 * @return This routine returns TRUE if the provided GUIDs are equal, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
RtlCompareGuids(IN PGUID Guid1,
                IN PGUID Guid2)
{
    return RTL::Guid::CompareGuids(Guid1, Guid2);
}

/**
 * This routine compares the first bytes of the specified memory buffers.
 *
 * @param LeftBuffer
 *        Supplies a pointer to the first block of memory to compare.
 *
 * @param RightBuffer
 *        Supplies a pointer to the second block of memory to compare.
 *
 * @param Length
 *        Specifies a number of bytes to compare.
 *
 * @return This routine returns a number of bytes that are equal in both memory blocks.
 *
 * @since NT 3.5
 */
XTCLINK
XTAPI
SIZE_T
RtlCompareMemory(IN PCVOID LeftBuffer,
                 IN PCVOID RightBuffer,
                 IN SIZE_T Length)
{
    return RTL::Memory::CompareMemory(LeftBuffer, RightBuffer, Length);
}

/**
 * Compares at most specified number of characters of two C strings.
 *
 * @param String1
 *        String to be compared.
 *
 * @param String2
 *        String to be compared.
 *
 * @param Length
 *        Maximum number of characters to compare. If no limit set, it compares whole strings.
 *
 * @return Integral value indicating the relationship between the strings.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
SIZE_T
RtlCompareString(IN PCSTR String1,
                 IN PCSTR String2,
                 IN SIZE_T Length)
{
    return RTL::String::CompareString(String1, String2, Length);
}

/**
 * Compares at most specified number of characters of two C strings, while ignoring differences in case.
 *
 * @param String1
 *        String to be compared.
 *
 * @param String2
 *        String to be compared.
 *
 * @param Length
 *        Maximum number of characters to compare. If no limit set, it compares whole strings.
 *
 * @return Integral value indicating the relationship between the strings.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
SIZE_T
RtlCompareStringInsensitive(IN PCSTR String1,
                            IN PCSTR String2,
                            IN SIZE_T Length)
{
    return RTL::String::CompareStringInsensitive(String1, String2, Length);
}

/**
 * Compares at most specified number of characters of two C wide strings.
 *
 * @param String1
 *        Wide string to be compared.
 *
 * @param String2
 *        Wide string to be compared.
 *
 * @param Length
 *        Maximum number of characters to compare. If no limit set, it compares whole wide strings.
 *
 * @return Integral value indicating the relationship between the wide strings.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
SIZE_T
RtlCompareWideString(IN PCWSTR String1,
                     IN PCWSTR String2,
                     IN SIZE_T Length)
{
    return RTL::WideString::CompareWideString(String1, String2, Length);
}

/**
 * Compares at most specified number of characters of two C wide strings, while ignoring differences in case.
 *
 * @param String1
 *        Wide string to be compared.
 *
 * @param String2
 *        Wide string to be compared.
 *
 * @param Length
 *        Maximum number of characters to compare. If no limit set, it compares whole wide strings.
 *
 * @return Integral value indicating the relationship between the wide strings.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
SIZE_T
RtlCompareWideStringInsensitive(IN PCWSTR String1,
                                IN PCWSTR String2,
                                IN SIZE_T Length)
{
    return RTL::WideString::CompareWideStringInsensitive(String1, String2, Length);
}

/**
 * Appends a copy of the source string to the end of the destination string.
 *
 * @param Destination
 *        Supplies a pointer to the NULL-terminated string to append to.
 *
 * @param Source
 *        Supplies a pointer to the NULL-terminated string to copy from.
 *
 * @param Count
 *        Sets a maximum number of characters to copy. If no limit set, appends whole string.
 *
 * @return This routine returns a copy of a destination string.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
PCHAR
RtlConcatenateString(OUT PCHAR Destination,
                               IN PCHAR Source,
                               IN SIZE_T Count)
{
    return RTL::String::ConcatenateString(Destination, Source, Count);
}

/**
 * Appends a copy of the source wide string to the end of the destination wide string.
 *
 * @param Destination
 *        Supplies a pointer to the NULL-terminated wide string to append to.
 *
 * @param Source
 *        Supplies a pointer to the NULL-terminated wide string to copy from.
 *
 * @param Count
 *        Sets a maximum number of wide characters to copy. If no limit set, appends whole wide string.
 *
 * @return This routine returns a copy of a destination wide string.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
PWCHAR
RtlConcatenateWideString(OUT PWCHAR Destination,
                         IN PWCHAR Source,
                         IN SIZE_T Count)
{
    return RTL::WideString::ConcatenateWideString(Destination, Source, Count);
}

/**
 * Converts the 32-bit signed value to a large integer.
 *
 * @param Value
 *        Supplies the value to convert.
 *
 * @return This routine returns the large integer representation of the given value.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
LARGE_INTEGER
RtlConvertToLargeInteger32(IN LONG Value)
{
    return RTL::Math::ConvertToLargeInteger32(Value);
}

/**
 * Converts the 32-bit unsigned value to a large integer.
 *
 * @param Value
 *        Supplies the value to convert.
 *
 * @return This routine returns the large integer representation of the given value.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
LARGE_INTEGER
RtlConvertToLargeIntegerUnsigned32(IN ULONG Value)
{
    return RTL::Math::ConvertToLargeIntegerUnsigned32(Value);
}

/**
 * This routine copies a block of memory.
 *
 * @param Destination
 *        Supplies a pointer to the buffer where data will be copied to.
 *
 * @param Source
 *        Supplies a pointer to the source buffer that will be copied.
 *
 * @param Length
 *        Specifies the number of bytes to copy.
 *
 * @return This routine does not return any value.
 *
 * @since NT 3.5
 */
XTCLINK
XTAPI
VOID
RtlCopyMemory(OUT PVOID Destination,
              IN PCVOID Source,
              IN SIZE_T Length)
{
    RTL::Memory::CopyMemory(Destination, Source, Length);
}

/**
 * Copies a string from a buffer into another buffer, ensuring that the destination string is NULL-terminated.
 *
 * @param Destination
 *        Supplies a pointer to the destination buffer.
 *
 * @param Source
 *        Supplies a pointer to the source buffer.
 *
 * @param Length
 *        Supplies the length of the string to copy.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
VOID
RtlCopyString(IN PCHAR Destination,
              IN PCSTR Source,
              IN ULONG Length)
{
    RTL::String::CopyString(Destination, Source, Length);
}

/**
 * Copies a wide string from a buffer into another buffer, ensuring that the destination string is NULL-terminated.
 *
 * @param Destination
 *        Supplies a pointer to the destination buffer.
 *
 * @param Source
 *        Supplies a pointer to the source buffer.
 *
 * @param Length
 *        Supplies the length of the wide string to copy.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
VOID
RtlCopyWideString(IN PWCHAR Destination,
                  IN PCWSTR Source,
                  IN ULONG Length)
{
    RTL::WideString::CopyWideString(Destination, Source, Length);
}

/**
 * Divides a signed large integer by a 32-bit divisor.
 *
 * @param Dividend
 *        Supplies a large integer to be divided.
 *
 * @param Divisor
 *        Supplies a 32-bit divisor.
 *
 * @param Remainder
 *        Supplies a pointer that receives the divide remainder.
 *
 * @return This routine returns the quotient.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
LARGE_INTEGER
RtlDivideLargeInteger(IN LARGE_INTEGER Dividend,
                      IN ULONG Divisor,
                      OUT PULONG Remainder)
{
    return RTL::Math::DivideLargeInteger(Dividend, Divisor, Remainder);
}

/**
 * Searches the bit map for a contiguous region of clear bits.
 *
 * @param BitMap
 *        Supplies a pointer to the bit map.
 *
 * @param Length
 *        Supplies the length of contiguous region (number of clear bits) to look for.
 *
 * @param Index
 *        Supplies the index of the first bit to start the search at a given position.
 *
 * @return This routine returns the bit map index position of the contiguous region found, or MAXULONG_PTR if not found.
 *
 * @since NT 3.5
 */
XTCLINK
XTAPI
ULONG_PTR
RtlFindClearBits(IN PRTL_BITMAP BitMap,
                 IN ULONG_PTR Length,
                 IN ULONG_PTR Index)
{
    return RTL::BitMap::FindClearBits(BitMap, Length, Index);
}

/**
 * Searches the bit map for a contiguous region of set bits.
 *
 * @param BitMap
 *        Supplies a pointer to the bit map.
 *
 * @param Length
 *        Supplies the length of contiguous region (number of set bits) to look for.
 *
 * @param Index
 *        Supplies the index of the first bit to start the search at a given position.
 *
 * @return This routine returns the bit map index position of the contiguous region found, or MAXULONG_PTR if not found.
 *
 * @since NT 3.5
 */
XTCLINK
XTAPI
ULONG_PTR
RtlFindSetBits(IN PRTL_BITMAP BitMap,
               IN ULONG_PTR Length,
               IN ULONG_PTR Index)
{
    return RTL::BitMap::FindSetBits(BitMap, Length, Index);
}

/**
 * Finds the first occurrence of the search string in the source string.
 *
 * @param Source
 *        Supplies a pointer to the source string.
 *
 * @param Search
 *        Supplies a pointer to the search string.
 *
 * @return This routine returns a pointer to the first occurrence of the search string in the source string.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
PCSTR
RtlFindString(IN PCSTR Source,
              IN PCSTR Search)
{
    return RTL::String::FindString(Source, Search);
}

/**
 * Finds the first case-insensitive occurrence of the search string in the source string.
 *
 * @param Source
 *        Supplies a pointer to the source string.
 *
 * @param Search
 *        Supplies a pointer to the search string.
 *
 * @return This routine returns a pointer to the first occurrence of the search string in the source string.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
PCSTR
RtlFindStringInsensitive(IN PCSTR Source,
                         IN PCSTR Search)
{
    return RTL::String::FindStringInsensitive(Source, Search);
}

/**
 * Finds the first occurrence of the search wide string in the source wide string.
 *
 * @param Source
 *        Supplies a pointer to the source wide string.
 *
 * @param Search
 *        Supplies a pointer to the search wide string.
 *
 * @return This routine returns a pointer to the first occurrence of the search wide string in the source wide string.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
PCWSTR
RtlFindWideString(IN PCWSTR Source,
               IN PCWSTR Search)
{
    return RTL::WideString::FindWideString(Source, Search);
}

/**
 * Finds the first case-insensitive occurrence of the search wide string in the source wide string.
 *
 * @param Source
 *        Supplies a pointer to the source wide string.
 *
 * @param Search
 *        Supplies a pointer to the search wide string.
 *
 * @return This routine returns a pointer to the first occurrence of the search wide string in the source wide string.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
PCWSTR
RtlFindWideStringInsensitive(IN PCWSTR Source,
                             IN PCWSTR Search)
{
    return RTL::WideString::FindWideStringInsensitive(Source, Search);
}

/**
 * Initializes a bit map.
 *
 * @param BitMap
 *        Supplies a pointer to the bit map to initialize.
 *
 * @param Buffer
 *        Supplies a pointer to the buffer that will be used as a bit map.
 *
 * @param Size
 *        Supplies a size of the bit map.
 *
 * @return This routine does not return any value.
 *
 * @since NT 3.5
 */
XTCLINK
XTAPI
VOID
RtlInitializeBitMap(IN PRTL_BITMAP BitMap,
                    IN PULONG_PTR Buffer,
                    IN ULONG Size)
{
    RTL::BitMap::InitializeBitMap(BitMap, Buffer, Size);
}

/**
 * This routine initializes a structure representing the head of a double-linked list.
 *
 * @param ListHead
 *        Pointer to a structure that serves as the list header.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTCDECL
VOID
RtlInitializeListHead(IN PLIST_ENTRY ListHead)
{
    RTL::LinkedList::InitializeListHead(ListHead);
}

/**
 * This routine inserts an entry at the head of a doubly linked list.
 *
 * @param ListHead
 *        Pointer to the head of the list.
 *
 * @param Entry
 *        Pointer to the entry that will be inserted in the list.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTCDECL
VOID
RtlInsertHeadList(IN OUT PLIST_ENTRY ListHead,
                  IN PLIST_ENTRY Entry)
{
    RTL::LinkedList::InsertHeadList(ListHead, Entry);
}

/**
 * This routine inserts an entry at the tail of a doubly linked list.
 *
 * @param ListHead
 *        Pointer to the head of the list.
 *
 * @param Entry
 *        Pointer to the entry that will be inserted in the list.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTCDECL
VOID
RtlInsertTailList(IN OUT PLIST_ENTRY ListHead,
                  IN PLIST_ENTRY Entry)
{
    RTL::LinkedList::InsertTailList(ListHead, Entry);
}

/**
 * Indicates whether a doubly linked list structure is empty, or not initialized at all.
 *
 * @param ListHead
 *        Pointer to a structure that represents the head of the list.
 *
 * @return TRUE if there are currently no entries in the list or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTCLINK
XTCDECL
BOOLEAN
RtlListEmpty(IN PLIST_ENTRY ListHead)
{
    return RTL::LinkedList::ListEmpty(ListHead);
}

/**
 * This routine detects a loop in a doubly linked list.
 *
 * @param ListHead
 *        Pointer to a structure that represents the head of the list.
 *
 * @return TRUE if linked list contains a loop or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTCLINK
XTCDECL
BOOLEAN
RtlListLoop(IN PLIST_ENTRY ListHead)
{
    return RTL::LinkedList::ListLoop(ListHead);
}

/**
 * This routine copies a block of memory either forward of backward, depeding
 * if source and destination buffers overlap or not.
 *
 * @param Destination
 *        Supplies a pointer to the buffer where data will be copied to.
 *
 * @param Source
 *        Supplies a pointer to the source buffer that will be copied.
 *
 * @param Length
 *        Specifies the number of bytes to copy.
 *
 * @return This routine does not return any value.
 *
 * @since NT 3.5
 */
XTCLINK
XTAPI
VOID
RtlMoveMemory(OUT PVOID Destination,
              IN PCVOID Source,
              IN SIZE_T Length)
{
    RTL::Memory::MoveMemory(Destination, Source, Length);
}

/**
 * Multiplies a signed large integer by a signed integer.
 *
 * @param Multiplicand
 *        Supplies a large integer to be multiplied.
 *
 * @param Multiplier
 *        Supplies an integer by which the large integer is multiplied.
 *
 * @return This routine returns the result of the multiplication.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
LARGE_INTEGER
RtlMultiplyLargeInteger(IN LARGE_INTEGER Multiplicand,
                                IN LONG Multiplier)
{
    return RTL::Math::MultiplyLargeInteger(Multiplicand, Multiplier);
}

/**
 * This routine removes an entry from a doubly linked list.
 *
 * @param Entry
 *        Pointer to the entry that will be removed from the list.
 *
 * @return This routine returns TRUE if the list is empty after removal, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTCLINK
XTCDECL
BOOLEAN
RtlRemoveEntryList(IN PLIST_ENTRY Entry)
{
    return RTL::LinkedList::RemoveEntryList(Entry);
}

/**
 * Reverses a characters order in a string. It modifies the original, input variable.
 *
 * @param String
 *        Supplies a pointer to the string to reverse.
 *
 * @param Length
 *        Supplies the length of the string to reverse.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
VOID
RtlReverseString(IN OUT PCHAR String,
                 IN ULONG Length)
{
    RTL::String::ReverseString(String, Length);
}

/**
 * Reverses a characters order in a wide string. It modifies the original, input variable.
 *
 * @param String
 *        Supplies a pointer to the wide string to reverse.
 *
 * @param Length
 *        Supplies the length of the wide string to reverse.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
VOID
RtlReverseWideString(IN OUT PWCHAR String,
                     IN ULONG Length)
{
    RTL::WideString::ReverseWideString(String, Length);
}

/**
 * This routine compares the first bytes of the specified memory buffers.
 *
 * @param LeftBuffer
 *        Supplies a pointer to the first block of memory to compare.
 *
 * @param RightBuffer
 *        Supplies a pointer to the second block of memory to compare.
 *
 * @param Length
 *        Specifies a number of bytes to compare.
 *
 * @return Returns TRUE if both buffers are equal up to the specified length, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
BOOLEAN
RtlSameMemory(IN PCVOID LeftBuffer,
              IN PCVOID RightBuffer,
              IN SIZE_T Length)
{
    return RTL::Memory::SameMemory(LeftBuffer, RightBuffer, Length);
}

/**
 * Sets all bits in the bit map.
 *
 * @param BitMap
 *        Supplies a pointer to the bit map.
 *
 * @return This routine does not return any value.
 *
 * @since NT 3.5
 */
XTCLINK
XTAPI
VOID
RtlSetAllBits(IN PRTL_BITMAP BitMap)
{
    RTL::BitMap::SetAllBits(BitMap);
}

/**
 * Sets a single bit in the bit map.
 *
 * @param BitMap
 *        Supplies a pointer to the bit map.
 *
 * @param Bit
 *        Specifies the number of the bit to be set.
 *
 * @return This routine does not return any value.
 *
 * @since NT 5.1
 */
XTCLINK
XTAPI
VOID
RtlSetBit(IN PRTL_BITMAP BitMap,
          IN ULONG_PTR Bit)
{
    RTL::BitMap::SetBit(BitMap, Bit);
}

/**
 * Sets a specified set of bits within a bit map.
 *
 * @param BitMap
 *        Supplies a pointer to the bit map.
 *
 * @param StartingIndex
 *        Supplies the starting index of the first bit to set.
 *
 * @param Length
 *        Supplies the length (number of bits) to set.
 *
 * @return This routine does not return any value.
 *
 * @since NT 3.5
 */
XTCLINK
XTAPI
VOID
RtlSetBits(IN PRTL_BITMAP BitMap,
           IN ULONG_PTR StartingIndex,
           IN ULONG_PTR Length)
{
    RTL::BitMap::SetBits(BitMap, StartingIndex, Length);
}

/**
 * Searches the bit map for a contiguous region of clear bits and sets them.
 *
 * @param BitMap
 *        Supplies a pointer to the bit map.
 *
 * @param Length
 *        Supplies the length of contiguous region (number of clear bits) to look for.
 *
 * @param Index
 *        Supplies the index of the first bit to start the search at a given position.
 *
 * @return This routine returns the bit map index position of the contiguous region found, or MAXULONG_PTR if not found.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
ULONG
RtlSetClearBits(IN PRTL_BITMAP BitMap,
                IN ULONG_PTR Length,
                IN ULONG_PTR Index)
{
    return RTL::BitMap::SetClearBits(BitMap, Length, Index);
}

/**
 * This routine fills a section of memory with a specified byte.
 *
 * @param Destination
 *        Supplies a pointer to the buffer to fill.
 *
 * @param Byte
 *        Supplies a pattern to fill memory.
 *
 * @param Length
 *        Specifies a number of bytes to store in memory.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
VOID
RtlSetMemory(OUT PVOID Destination,
             IN UCHAR Byte,
             IN SIZE_T Length)
{
    RTL::Memory::SetMemory(Destination, Byte, Length);
}

/**
 * Calculates the length of a given string.
 *
 * @param String
 *        Pointer to the NULL-terminated string to be examined.
 *
 * @param MaxLength
 *        Maximum number of characters to examine. If no limit set, it examines whole string.
 *
 * @return The length of the NULL-terminated string.
 *
 * @since: XT 1.0
 */
XTCLINK
XTAPI
SIZE_T
RtlStringLength(IN PCSTR String,
                IN SIZE_T MaxLength)
{
    return RTL::String::StringLength(String, MaxLength);
}

/**
 * Converts a multibyte character string to its wide character representation.
 *
 * @param Destination
 *        Pointer to wide character array where the wide string will be stored
 *
 * @param Source
 *        Pointer to the first element of a multibyte string to convert.
 *
 * @param Length
 *        Number of characters in the source string.
 *
 * @return Returns the number of wide characters written to the destination array on success, or -1 on error.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
SIZE_T
RtlStringToWideString(OUT PWCHAR Destination,
                      IN PCSTR *Source,
                      IN SIZE_T Length)
{
    return RTL::String::StringToWideString(Destination, Source, Length);
}

/**
 * Tests a state of a single bit in the bit map.
 *
 * @param BitMap
 *        Supplies a pointer to the bit map.
 *
 * @param Bit
 *        Specifies the number of the bit to be tested.
 *
 * @return This routine returns TRUE when bit is set, or FALSE otherwise.
 *
 * @since NT 5.1
 */
XTCLINK
XTAPI
BOOLEAN
RtlTestBit(IN PRTL_BITMAP BitMap,
                     IN ULONG_PTR Bit)
{
    return RTL::BitMap::TestBit(BitMap, Bit);
}

/**
 * Finds the next token in a NULL-terminated string.
 *
 * @param String
 *        Pointer to the NULL-terminated string to tokenize.
 *
 * @param Delimiter
 *        Pointer to the NULL-terminated string identifying delimiters.
 *
 * @param SavePtr
 *        Pointer to an object used to store routine internal state.
 *
 * @return Pointer to the beginning of the next token or NULL if there are no more tokens.
 *
 * @since: XT 1.0
 */
XTCLINK
XTAPI
PCHAR
RtlTokenizeString(IN PCHAR String,
                  IN PCSTR Delimiter,
                  IN OUT PCHAR *SavePtr)
{
    return RTL::String::TokenizeString(String, Delimiter, SavePtr);
}

/**
 * Finds the next token in a NULL-terminated wide string.
 *
 * @param String
 *        Pointer to the NULL-terminated wide string to tokenize.
 *
 * @param Delimiter
 *        Pointer to the NULL-terminated wide string identifying delimiters.
 *
 * @param SavePtr
 *        Pointer to an object used to store routine internal state.
 *
 * @return Pointer to the beginning of the next token or NULLPTR if there are no more tokens.
 *
 * @since: XT 1.0
 */
XTCLINK
XTAPI
PWCHAR
RtlTokenizeWideString(IN PWCHAR String,
                      IN PCWSTR Delimiter,
                      IN OUT PWCHAR *SavePtr)
{
    return RTL::WideString::TokenizeWideString(String, Delimiter, SavePtr);
}

/**
 * Converts a character to lowercase.
 *
 * @param Character
 *        Character to be converted.
 *
 * @return Converted character or original character if it was not uppercase.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
CHAR
RtlToLowerCharacter(IN CHAR Character)
{
    return RTL::String::ToLowerCharacter(Character);
}

/**
 * Converts a wide character to lowercase.
 *
 * @param Character
 *        Wide character to be converted.
 *
 * @return Converted wide character or original character if it was not uppercase.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
WCHAR
RtlToLowerWideCharacter(IN WCHAR Character)
{
    return RTL::WideString::ToLowerWideCharacter(Character);
}

/**
 * Converts a character to uppercase.
 *
 * @param Character
 *        Character to be converted.
 *
 * @return Converted character or original character if it was not lowercase.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
CHAR
RtlToUpperCharacter(IN CHAR Character)
{
    return RTL::String::ToUpperCharacter(Character);
}

/**
 * Converts a wide character to uppercase.
 *
 * @param Character
 *        Wide character to be converted.
 *
 * @return Converted wide character or original character if it was not lowercase.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
WCHAR
RtlToUpperWideCharacter(IN WCHAR Character)
{
    return RTL::WideString::ToUpperWideCharacter(Character);
}

/**
 * Removes certain characters from a beginning of the string.
 *
 * @param String
 *        Pointer to the NULL-terminated string to be trimmed.
 *
 * @return This routine returns a pointer to the left-trimmed string.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
PCHAR
RtlTrimLeftString(IN PCHAR String)
{
    return RTL::String::TrimLeftString(String);
}

/**
 * Removes certain characters from a beginning of the wide string.
 *
 * @param String
 *        Pointer to the NULL-terminated wide string to be trimmed.
 *
 * @return This routine returns a pointer to the left-trimmed wide string.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
PWCHAR
RtlTrimLeftWideString(IN PWCHAR String)
{
    return RTL::WideString::TrimLeftWideString(String);
}

/**
 * Removes certain characters from the end of the string.
 *
 * @param String
 *        Pointer to the NULL-terminated string to be trimmed.
 *
 * @return This routine returns a pointer to the right-trimmed string.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
PCHAR
RtlTrimRightString(IN PCHAR String)
{
    return RTL::String::TrimRightString(String);
}

/**
 * Removes certain characters from the end of the wide string.
 *
 * @param String
 *        Pointer to the NULL-terminated wide string to be trimmed.
 *
 * @return This routine returns a pointer to the right-trimmed wide string.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
PWCHAR
RtlTrimRightWideString(IN PWCHAR String)
{
    return RTL::WideString::TrimRightWideString(String);
}

/**
 * Removes certain characters from the beginning and the end of the string.
 *
 * @param String
 *        Pointer to the NULL-terminated string to be trimmed.
 *
 * @return This routine returns a pointer to the trimmed string.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
PCHAR
RtlTrimString(IN PCHAR String)
{
    return RTL::String::TrimLeftString(RTL::String::TrimRightString(String));
}

/**
 * Removes certain characters from the beginning and the end of the wide string.
 *
 * @param String
 *        Pointer to the NULL-terminated wide string to be trimmed.
 *
 * @return This routine returns a pointer to the trimmed wide string.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
PWCHAR
RtlTrimWideString(IN PWCHAR String)
{
    return RTL::WideString::TrimWideString(String);
}

/**
 * Calculates the length of a given wide string.
 *
 * @param String
 *        Pointer to the NULL-terminated wide string to be examined.
 *
 * @param MaxLength
 *        Maximum number of wide characters to examine. If no limit set, it examines whole string.
 *
 * @return The length of the NULL-terminated wide string.
 *
 * @since: XT 1.0
 */
XTCLINK
XTAPI
SIZE_T
RtlWideStringLength(IN PCWSTR String,
                    IN SIZE_T MaxLength)
{
    return RTL::WideString::WideStringLength(String, MaxLength);
}

/**
 * This routine fills a section of memory with zeroes.
 *
 * @param Destination
 *        Supplies a pointer to the buffer to fill.
 *
 * @param Length
 *        Specifies a number of bytes to be filled with zeroes.
 *
 * @return This routine does not return any value.
 *
 * @since NT 3.5
 */
XTCLINK
XTAPI
VOID
RtlZeroMemory(OUT PVOID Destination,
              IN SIZE_T Length)
{
    RTL::Memory::ZeroMemory(Destination, Length);
}
