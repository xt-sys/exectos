/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/sha1.cc
 * DESCRIPTION:     SHA1 computation support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * Computes the SHA-1 hash of a buffer in a single step.
 *
 * @param Buffer
 *        Supplies a pointer to the buffer containing the data to be hashed.
 *
 * @param BufferSize
 *        Specifies the size of the buffer in bytes.
 *
 * @param Digest
 *        Supplies a pointer to the buffer that receives the 20-byte SHA-1 hash digest.
 *
 * @return This routine returns STATUS_SUCCESS if the hash was computed successfully,
 *         or STATUS_INVALID_PARAMETER if an invalid parameter was supplied.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
RTL::SHA1::ComputeDigest(IN PCUCHAR Buffer,
                         IN SIZE_T BufferSize,
                         OUT PUCHAR Digest)
{
    RTL_SHA1_CONTEXT Context;
    XTSTATUS Status;

    /* Validate input parameters */
    if(!Digest || (!Buffer && BufferSize > 0))
    {
        /* Invalid parameters, return error */
        return STATUS_INVALID_PARAMETER;
    }

    /* Initialize SHA-1 context */
    Status = InitializeContext(&Context);
    if(Status != STATUS_SUCCESS)
    {
        /* Failed to initialize SHA-1 context, return error */
        return Status;
    }

    /* Hash data and compute SHA-1 digest */
    HashData(&Context, Buffer, BufferSize);
    ComputeHash(&Context, Digest);

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Finalizes the SHA-1 hash computation and provides the digest.
 *
 * @param Context
 *        Supplies a pointer to the SHA-1 context structure.
 *
 * @param Digest
 *        Supplies a pointer to a buffer that receives the 20-byte SHA-1 hash digest.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
RTL::SHA1::ComputeHash(IN OUT PRTL_SHA1_CONTEXT Context,
                       OUT PUCHAR Digest)
{
    ULONG Index, PaddingLength;
    UCHAR Padding[64];
    UCHAR Bits[8];

    /* Zero padding and append 0x80 to it */
    RTL::Memory::ZeroMemory(Padding, 64);
    Padding[0] = 0x80;

    /* Encode the total message length in bits as a Big Endian 64-bit integer */
    *(PULONG)(Bits) = RTL::Endianness::SwapByte32(Context->Count[1]);
    *(PULONG)(Bits + 4) = RTL::Endianness::SwapByte32(Context->Count[0]);

    /* Calculate padding length needed to align the message to 56 bytes */
    Index = (Context->Count[0] >> 3) & 0x3F;
    PaddingLength = (Index < 56) ? (56 - Index) : (120 - Index);

    /* Append the padding bytes to the message stream to satisfy the block boundary */
    HashData(Context, Padding, PaddingLength);

    /* Append bits before final transformation */
    HashData(Context, Bits, 8);

    /* Store hash to output in Big Endian format */
    Digest[0] = (UCHAR)((Context->State[0] >> 24) & 0xFF);
    Digest[1] = (UCHAR)((Context->State[0] >> 16) & 0xFF);
    Digest[2] = (UCHAR)((Context->State[0] >> 8) & 0xFF);
    Digest[3] = (UCHAR)(Context->State[0] & 0xFF);
    Digest[4] = (UCHAR)((Context->State[1] >> 24) & 0xFF);
    Digest[5] = (UCHAR)((Context->State[1] >> 16) & 0xFF);
    Digest[6] = (UCHAR)((Context->State[1] >> 8) & 0xFF);
    Digest[7] = (UCHAR)(Context->State[1] & 0xFF);
    Digest[8] = (UCHAR)((Context->State[2] >> 24) & 0xFF);
    Digest[9] = (UCHAR)((Context->State[2] >> 16) & 0xFF);
    Digest[10] = (UCHAR)((Context->State[2] >> 8) & 0xFF);
    Digest[11] = (UCHAR)(Context->State[2] & 0xFF);
    Digest[12] = (UCHAR)((Context->State[3] >> 24) & 0xFF);
    Digest[13] = (UCHAR)((Context->State[3] >> 16) & 0xFF);
    Digest[14] = (UCHAR)((Context->State[3] >> 8) & 0xFF);
    Digest[15] = (UCHAR)(Context->State[3] & 0xFF);
    Digest[16] = (UCHAR)((Context->State[4] >> 24) & 0xFF);
    Digest[17] = (UCHAR)((Context->State[4] >> 16) & 0xFF);
    Digest[18] = (UCHAR)((Context->State[4] >> 8) & 0xFF);
    Digest[19] = (UCHAR)(Context->State[4] & 0xFF);

    /* Clear context memory for security */
    RTL::Memory::ZeroMemory(Context, sizeof(RTL_SHA1_CONTEXT));
}

/**
 * Updates the SHA-1 context with the provided data buffer.
 *
 * @param Context
 *        Supplies a pointer to the SHA-1 context structure.
 *
 * @param Data
 *        Supplies a pointer to the buffer containing the data to be hashed.
 *
 * @param Length
 *        Specifies the length of the data buffer in bytes.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
RTL::SHA1::HashData(IN OUT PRTL_SHA1_CONTEXT Context,
                    IN PCUCHAR Data,
                    IN ULONG Length)
{
    ULONG Index, Input, PartialLength;

    /* Calculate byte offset within the 64-byte block buffer */
    Index = (Context->Count[0] >> 3) & 0x3F;

    /* Update the total bit count, handling overflow from low to high word */
    if((Context->Count[0] += (Length << 3)) < (Length << 3))
    {
        /* Increment high 32-bit counter on low counter overflow */
        Context->Count[1]++;
    }

    /* Add the high-order bits of the length (bytes -> bits overflow) to the high counter */
    Context->Count[1] += (Length >> 29);

    /* Calculate the number of bytes required to fill the remaining buffer space */
    PartialLength = 64 - Index;

    /* Transform as many times as possible */
    if(Length >= PartialLength)
    {
        /* Fill the buffer partially and transform */
        RTL::Memory::CopyMemory(&Context->Buffer[Index], Data, PartialLength);
        TransformData(Context->State, Context->Buffer);

        /* Process remaining full 64-byte blocks directly from the input pointer */
        for(Input = PartialLength; Input + 63 < Length; Input += 64)
        {
            /* Transform the full block without copying to the internal buffer */
            TransformData(Context->State, &Data[Input]);
        }

        /* Reset buffer index to indicate the buffer is now empty */
        Index = 0;
    }
    else
    {
        /* No full block was formed, begin copying from the start of input */
        Input = 0;
    }

    /* Buffer remaining input */
    RTL::Memory::CopyMemory(&Context->Buffer[Index], &Data[Input], Length - Input);
}

/**
 * Initializes a SHA-1 context structure with the standard initial hash values.
 *
 * @param Context
 *        Supplies a pointer to the SHA-1 context structure to initialize.
 *
 * @return This routine returns STATUS_SUCCESS if the context was initialized successfully,
 *         or STATUS_INVALID_PARAMETER if the pointer is NULLPTR.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
RTL::SHA1::InitializeContext(OUT PRTL_SHA1_CONTEXT Context)
{
    /* Validate input parameter */
    if(!Context)
    {
        /* Invalid parameter, return error */
        return STATUS_INVALID_PARAMETER;
    }

    /* Initialize hash constants defined by FIPS 180-4 */
    Context->State[0] = 0x67452301;
    Context->State[1] = 0xEFCDAB89;
    Context->State[2] = 0x98BADCFE;
    Context->State[3] = 0x10325476;
    Context->State[4] = 0xC3D2E1F0;
    Context->Count[0] = 0;
    Context->Count[1] = 0;

    /* Clear buffer */
    RTL::Memory::ZeroMemory(Context->Buffer, SHA1_BLOCK_SIZE);

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Transforms a single 64-byte block using the SHA-1 compression function.
 *
 * @param State
 *        Supplies a pointer to the array containing the current hash state variables.
 *
 * @param Buffer
 *        Supplies a pointer to the 64-byte input buffer to be transformed.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
RTL::SHA1::TransformData(IN OUT PULONG State,
                         IN PCUCHAR Buffer)
{
    ULONG Constant, Function, Index, TempHash;
    ULONG TempState[5];
    ULONG Stack[80];

    /* Initialize working variables with current state */
    TempState[0] = State[0];
    TempState[1] = State[1];
    TempState[2] = State[2];
    TempState[3] = State[3];
    TempState[4] = State[4];

    /* Initialize the first 16 words of the message schedule from the input buffer */
    for(Index = 0; Index < 16; Index++)
    {
        /* Convert input words from Little Endian to Big Endian required by SHA-1 */
        Stack[Index] = RTL::Endianness::SwapByte32(((const PULONG)Buffer)[Index]);
    }

    /* Expand the message to 80 words using the SHA-1 algorithm */
    for(Index = 16; Index < 80; Index++)
    {
        /* Compute the next word value based on XOR and left rotation */
        Stack[Index] = ROTATE_LEFT((Stack[Index - 3] ^ Stack[Index - 8] ^ Stack[Index - 14] ^ Stack[Index - 16]), 1);
    }

    /* Execute the main compression loop for 80 rounds */
    for(Index = 0; Index < 80; Index++)
    {
        /* Determine the logical function and constant based on the current round */
        if(Index < 20)
        {
            /* Set the constant defined by FIPS 180-4 for rounds 0-19 and calculate the logical function */
            Constant = 0x5A827999;
            Function = (TempState[1] & TempState[2]) | ((~TempState[1]) & TempState[3]);
        }
        else if(Index < 40)
        {
            /* Set the constant defined by FIPS 180-4 for rounds 20-39 and calculate the logical function */
            Constant = 0x6ED9EBA1;
            Function = TempState[1] ^ TempState[2] ^ TempState[3];
        }
        else if(Index < 60)
        {
            /* Set the constant defined by FIPS 180-4 for rounds 40-59 and calculate the logical function */
            Constant = 0x8F1BBCDC;
            Function = (TempState[1] & TempState[2]) | (TempState[1] & TempState[3]) | (TempState[2] & TempState[3]);
        }
        else
        {
            /* Set the constant defined by FIPS 180-4 for rounds 60-79 and calculate the logical function */
            Constant = 0xCA62C1D6;
            Function = TempState[1] ^ TempState[2] ^ TempState[3];
        }

        /* Calculate the temporary hash value for this round */
        TempHash = ROTATE_LEFT(TempState[0], 5) + Function + TempState[4] + Constant + Stack[Index];
        TempState[4] = TempState[3];
        TempState[3] = TempState[2];
        TempState[2] = ROTATE_LEFT(TempState[1], 30);
        TempState[1] = TempState[0];
        TempState[0] = TempHash;
    }

    /* Add the compressed chunk to the current hash value */
    State[0] += TempState[0];
    State[1] += TempState[1];
    State[2] += TempState[2];
    State[3] += TempState[3];
    State[4] += TempState[4];

    /* Clear sensitive data from stack */
    RTL::Memory::ZeroMemory(Stack, sizeof(Stack));
}
