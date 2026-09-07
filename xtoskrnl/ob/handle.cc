/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ob/handle.cc
 * DESCRIPTION:     Object Manager Handle Management
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Encodes a standard handle into a kernel-space handle.
 *
 * @param Handle
 *        Supplies the raw standard handle to be encoded.
 *
 * @return This routine returns the encoded kernel handle.
 *
 * @since XT 1.0
 */
XTFASTCALL
HANDLE
OB::Handle::DecodeKernelHandle(IN HANDLE Handle)
{
    /* Clear the kernel handle flag */
    return (HANDLE)(OBJECT_HANDLE_KERNEL_MASK ^ (ULONG_PTR)Handle);
}

/**
 * Decodes a kernel-space handle into a standard handle.
 *
 * @param Handle
 *        Supplies the encoded kernel handle to be decoded.
 *
 * @return This routine returns the decoded standard handle.
 *
 * @since XT 1.0
 */
XTFASTCALL
HANDLE
OB::Handle::EncodeKernelHandle(IN HANDLE Handle)
{
    /* Set the kernel handle flag */
    return (HANDLE)(OBJECT_HANDLE_KERNEL_MASK | (ULONG_PTR)Handle);
}

/**
 * Retrieves and decodes the handle attributes from a given handle table entry.
 *
 * @param HandleTableEntry
 *        Supplies a pointer to the handle table entry to inspect.
 *
 * @return This routine returns a bitmask representing the active handle attributes.
 *
 * @since XT 1.0
 */
XTFASTCALL
ULONG
OB::Handle::GetHandleAttributes(IN PHANDLE_TABLE_ENTRY HandleTableEntry)
{
    ULONG Attributes;

    /* Extract the base attributes */
    Attributes = HandleTableEntry->ObAttributes;

    /* Check if the protect-on-close access bit is set */
    if(HandleTableEntry->GrantedAccess & OBJECT_ACCESS_PROTECT_CLOSE_BIT)
    {
        /* Retain valid handle attributes and set the protect-close flag */
        return (Attributes & OBJECT_HANDLE_ATTRIBUTES) | OBJECT_PROTECT_CLOSE;
    }

    /* Isolate and return the inherit and audit-on-close flags */
    return Attributes & (OBJECT_INHERIT | OBJECT_AUDIT_OBJECT_CLOSE);
}
