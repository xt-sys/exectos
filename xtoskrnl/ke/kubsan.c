/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/kubsan.c
 * DESCRIPTION:     Kernel Undefined Behaviour Sanitizer (UBSAN) error reporting handler
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Checks whether handled UBSAN error should be reported.
 *
 * @param Location
 *        Supplies a pointer to the UBSAN location descriptor.
 *
 * @return This routine returns TRUE if UBSAN error should be reported, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTCDECL
BOOLEAN
KepCheckUbsanReport(PKUBSAN_SOURCE_LOCATION Location)
{
    /* Make sure, this error should be reported */
    return !KepUbsanActiveFrame;
}

/**
 * Enters UBSAN frame and marks it as active.
 *
 * @param Location
 *        Supplies a pointer to the UBSAN location descriptor.
 *
 * @param Reason
 *        Supplies a pointer to the reason of the UBSAN failure.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
KepEnterUbsanFrame(PKUBSAN_SOURCE_LOCATION Location,
                   PCCHAR Reason)
{
    /* Enter UBSAN frame */
    KepUbsanActiveFrame = TRUE;

    /* Print generic error message to debug console */
    DebugPrint(L"UBSAN: Undefined behavior (%s) in %s:%d:%d\n",
               Reason, Location->FileName, Location->Line, Location->Column);
}

/**
 * Gets signed value from UBSAN.
 *
 * @param Type
 *        Supplies a pointer to the UBSAN type descriptor.
 *
 * @param Value
 *        Supplies a pointer to the UBSAN value.
 *
 * @return This routine returns a 64-bit signed integer.
 *
 * @since XT 1.0
 */
XTCDECL
LONGLONG
KepGetSignedUbsanValue(PKUBSAN_TYPE_DESCRIPTOR Type,
                       PVOID Value)
{
    ULONG BitWidth, ExtraBits;
    ULONG_PTR LongValue;

    /* Calculate bit width of the type */
    BitWidth = 1 << (Type->TypeInfo >> 1);

    /* Check if inline integer */
    if(BitWidth <= sizeof(ULONG) * 8)
    {
        /* Calculate extra bits and return value */
        ExtraBits = sizeof(LONGLONG) * 8 - BitWidth;
        LongValue = (ULONG_PTR)Value;
        return ((LONGLONG)LongValue) << ExtraBits >> ExtraBits;
    }

    /* Return 64-bit integer */
    return *(PLONGLONG)Value;
}

/**
 * Gets a string representation of the UBSAN type kind.
 *
 * @param TypeCheckKind
 *        Supplies a UBSAN type kind as a numeric value.
 *
 * @return This routine returns a string representation of the UBSAN type.
 *
 * @since XT 1.0
 */
XTCDECL
PCCHAR
KepGetUbsanTypeKind(UCHAR TypeCheckKind)
{
    /* Get type kind name */
    switch(TypeCheckKind)
    {
        case 0:
            return "Load of";
        case 1:
            return "Store to";
        case 2:
            return "Reference binding to";
        case 3:
            return "Member access within";
        case 4:
            return "Member call on";
        case 5:
            return "Constructor call on";
        case 6:
            return "Downcast of";
        case 7:
            return "Downcast of";
        case 8:
            return "Upcast of";
        case 9:
            return "Cast to virtual base of";
        default:
            return "Unrecognized failure code";
    }
}

/**
 * Gets unsigned value from UBSAN.
 *
 * @param Type
 *        Supplies a pointer to the UBSAN type descriptor.
 *
 * @param Value
 *        Supplies a pointer to the UBSAN value.
 *
 * @return This routine returns a 64-bit unsigned integer.
 *
 * @since XT 1.0
 */
XTCDECL
ULONGLONG
KepGetUnsignedUbsanValue(PKUBSAN_TYPE_DESCRIPTOR Type,
                         PVOID Value)
{
    ULONG BitWidth;

    /* Calculate bit width of the type */
    BitWidth = 1 << (Type->TypeInfo >> 1);

    /* Check if inline integer */
    if(BitWidth <= sizeof(ULONG) * 8)
    {
        /* Return value */
        return (ULONG_PTR)Value;
    }

    /* Return 64-bit integer */
    return *(PULONGLONG)Value;
}

/**
 * Handles the division overflow error reported by UBSAN.
 *
 * @param Data
 *        Supplies a pointer to the UBSAN overflow data.
 *
 * @param Lhs
 *        Supplies LHS operand.
 *
 * @param Rhs
 *        Supplies RHS operand.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
KepHandleUbsanDivisionOverflow(PKUBSAN_OVERFLOW_DATA Data,
                               PVOID Lhs,
                               PVOID Rhs)
{
    /* Check if this error was already reported */
    if(!KepCheckUbsanReport(&Data->Location))
    {
        /* Don't report twice */
        return;
    }

    /* Enter UBSAN frame */
    KepEnterUbsanFrame(&Data->Location, "Division-Overflow");

    /* Check if signed type, which value is -1 */
    if((Data->Type->TypeInfo & 1) && (KepGetSignedUbsanValue(Data->Type, Rhs) == -1))
    {
        /* Division by -1, print error message to debug console */
        DebugPrint(L"UBSAN: Division by -1 cannot be represented in type %s\n", Data->Type->TypeName);
    }
    else
    {
        /* Division by 0, print error message to debug console */
        DebugPrint(L"UBSAN: Division by zero\n");
    }

    /* Leave UBSAN frame */
    KepLeaveUbsanFrame();
}

/**
 * Handles the float cast overflow error reported by UBSAN.
 *
 * @param Data
 *        Supplies a pointer to the UBSAN float cast overflow data.
 *
 * @param Lhs
 *        Supplies LHS operand.
 *
 * @param Rhs
 *        Supplies RHS operand.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
KepHandleUbsanFloatCastOverflow(PKUBSAN_FLOAT_CAST_OVERFLOW_DATA Data,
                                ULONG_PTR Lhs,
                                ULONG_PTR Rhs)
{
    /* Check if this error was already reported */
    if(!KepCheckUbsanReport(&Data->Location))
    {
        /* Don't report twice */
        return;
    }

    /* Enter UBSAN frame */
    KepEnterUbsanFrame(&Data->Location, "Float-Cast-Overflow");

    /* Print error message to debug console */
    DebugPrint(L"Value of type %s is outside the range of type %s\n", Data->LhsType->TypeName, Data->RhsType->TypeName);

    /* Leave UBSAN frame */
    KepLeaveUbsanFrame();
}

/**
 * Handles the function type mismatch error reported by UBSAN.
 *
 * @param Data
 *        Supplies a pointer to the UBSAN function type mismatch data.
 *
 * @param Pointer
 *        Supplies pointer operand.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
KepHandleUbsanFunctionTypeMismatch(PKUBSAN_FUNCTION_TYPE_MISMATCH_DATA Data,
                                   ULONG_PTR Pointer)
{
    /* Check if this error was already reported */
    if(!KepCheckUbsanReport(&Data->Location))
    {
        /* Don't report twice */
        return;
    }

    /* Enter UBSAN frame */
    KepEnterUbsanFrame(&Data->Location, "Float-Cast-Overflow");

    /* Print error message to debug console */
    DebugPrint(L"UBSAN: Indirect function call through %P address of a wrong type %s\n",
               (PVOID)Pointer, Data->Type->TypeName);

    /* Leave UBSAN frame */
    KepLeaveUbsanFrame();
}

/**
 * Handles the integer overflow error reported by UBSAN.
 *
 * @param Data
 *        Supplies a pointer to the UBSAN overflow data.
 *
 * @param Lhs
 *        Supplies LHS operand.
 *
 * @param Rhs
 *        Supplies RHS operand.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
KepHandleUbsanIntegerOverflow(PKUBSAN_OVERFLOW_DATA Data,
                                 ULONG_PTR Lhs,
                                 ULONG_PTR Rhs)
{
    /* Check if this error was already reported */
    if(!KepCheckUbsanReport(&Data->Location))
    {
        /* Don't report twice */
        return;
    }

    /* Enter UBSAN frame */
    KepEnterUbsanFrame(&Data->Location, "Integer-Overflow");

    /* Print error message to debug console */
    DebugPrint(L"UBSAN: The result of an arithmetic operation cannot be represented in type %s\n", Data->Type->TypeName);

    /* Leave UBSAN frame */
    KepLeaveUbsanFrame();
}

/**
 * Handles the invalid builtin error reported by UBSAN.
 *
 * @param Data
 *        Supplies a pointer to the UBSAN invalid builtin data.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
KepHandleUbsanInvalidBuiltin(PKUBSAN_INVALID_BUILTIN_DATA Data)
{
    /* Check if this error was already reported */
    if(!KepCheckUbsanReport(&Data->Location))
    {
        /* Don't report twice */
        return;
    }

    /* Enter UBSAN frame */
    KepEnterUbsanFrame(&Data->Location, "Invalid-Builtin");

    /* Check kind of UBSAN error */
    if(Data->Kind == 0 || Data->Kind == 1)
    {
        /* Invalid ctz()/clz() parameter, print error message to debug console */
        DebugPrint(L"UBSAN: Passing zero to ctz() or clz() which is not a valid argument\n");
    }
    else
    {
        /* Unknown kind, print error message to debug console */
        DebugPrint(L"UBSAN: Unknown kind %u\n", Data->Kind);
    }

    /* Leave UBSAN frame */
    KepLeaveUbsanFrame();
}

/**
 * Handles the misaligned access error reported by UBSAN.
 *
 * @param Data
 *        Supplies a pointer to the UBSAN type mismatch data.
 *
 * @param Pointer
 *        Supplies a pointer operand.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
KepHandleUbsanMisalignedAccess(PKUBSAN_TYPE_MISMATCH_DATA Data,
                               ULONG_PTR Pointer)
{
    /* Check if this error was already reported */
    if(!KepCheckUbsanReport(&Data->Location))
    {
        /* Don't report twice */
        return;
    }

    /* Enter UBSAN frame */
    KepEnterUbsanFrame(&Data->Location, "Misaligned-Access");

    /* Print error message to debug console */
    DebugPrint(L"UBSAN: %s misaligned address %p for type %s which requires %ld byte alignment\n",
               KepGetUbsanTypeKind(Data->TypeCheckKind), (PVOID)Pointer, Data->Type->TypeName, Data->Alignment);

    /* Leave UBSAN frame */
    KepLeaveUbsanFrame();
}

/**
 * Handles the negate overflow error reported by UBSAN.
 *
 * @param Data
 *        Supplies a pointer to the UBSAN overflow data.
 *
 * @param OldValue
 *        Supplies old value.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
KepHandleUbsanNegateOverflow(PKUBSAN_OVERFLOW_DATA Data,
                             ULONG_PTR OldValue)
{
    /* Check if this error was already reported */
    if(!KepCheckUbsanReport(&Data->Location))
    {
        /* Don't report twice */
        return;
    }

    /* Enter UBSAN frame */
    KepEnterUbsanFrame(&Data->Location, "Negate-Overflow");

    /* Print error message to debug console */
    DebugPrint(L"UBSAN: Negation of %lu cannot be represented in type %s\n", OldValue, Data->Type->TypeName);

    /* Leave UBSAN frame */
    KepLeaveUbsanFrame();
}


/**
 * Handles the NULL pointer dereference error reported by UBSAN.
 *
 * @param Data
 *        Supplies a pointer to the UBSAN type mismatch data.
 *
 * @param Pointer
 *        Supplies a pointer operand.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
KepHandleUbsanNullPointerDereference(PKUBSAN_TYPE_MISMATCH_DATA Data)
{
    /* Check if this error was already reported */
    if(!KepCheckUbsanReport(&Data->Location))
    {
        /* Don't report twice */
        return;
    }

    /* Enter UBSAN frame */
    KepEnterUbsanFrame(&Data->Location, "NULL-Pointer-Dereference");

    /* Print error message to debug console */
    DebugPrint(L"UBSAN: %s NULL pointer of type %s\n", KepGetUbsanTypeKind(Data->TypeCheckKind), Data->Type->TypeName);

    /* Leave UBSAN frame */
    KepLeaveUbsanFrame();
}

/**
 * Handles the object size mismatch error reported by UBSAN.
 *
 * @param Data
 *        Supplies a pointer to the UBSAN type mismatch data.
 *
 * @param Pointer
 *        Supplies a pointer operand.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
KepHandleUbsanObjectSizeMismatch(PKUBSAN_TYPE_MISMATCH_DATA Data,
                                 ULONG_PTR Pointer)
{
    /* Check if this error was already reported */
    if(!KepCheckUbsanReport(&Data->Location))
    {
        /* Don't report twice */
        return;
    }

    /* Enter UBSAN frame */
    KepEnterUbsanFrame(&Data->Location, "Object-Size-Mismatch");

    /* Print error message to debug console */
    DebugPrint(L"UBSAN: %s address %p with insufficient space for an object of type %s\n",
               KepGetUbsanTypeKind(Data->TypeCheckKind), (PVOID)Pointer, Data->Type->TypeName);

    /* Leave UBSAN frame */
    KepLeaveUbsanFrame();
}

/**
 * Handles the out of bounds error reported by UBSAN.
 *
 * @param Data
 *        Supplies a pointer to the UBSAN out of bounds data.
 *
 * @param Index
 *        Supplies an index operand.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
KepHandleUbsanOutOfBounds(PKUBSAN_OUT_OF_BOUNDS_DATA Data,
                          ULONG_PTR Index)
{
    /* Check if this error was already reported */
    if(!KepCheckUbsanReport(&Data->Location))
    {
        /* Don't report twice */
        return;
    }

    /* Enter UBSAN frame */
    KepEnterUbsanFrame(&Data->Location, "Array-Index-Out-Of-Bounds");

    /* Print error message to debug console */
    DebugPrint(L"UBSAN: index is out of range for type %s", Data->ArrayType->TypeName);

    /* Leave UBSAN frame */
    KepLeaveUbsanFrame();
}

/**
 * Handles the pointer overflow error reported by UBSAN.
 *
 * @param Data
 *        Supplies a pointer to the UBSAN overflow data.
 *
 * @param Lhs
 *        Supplies LHS operand.
 *
 * @param Rhs
 *        Supplies RHS operand.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
KepHandleUbsanPointerOverflow(PKUBSAN_OVERFLOW_DATA Data,
                              ULONG_PTR Lhs,
                              ULONG_PTR Rhs)
{
    /* Check if this error was already reported */
    if(!KepCheckUbsanReport(&Data->Location))
    {
        /* Don't report twice */
        return;
    }

    /* Enter UBSAN frame */
    KepEnterUbsanFrame(&Data->Location, "Pointer-Overflow");

    /* Print error message to debug console */
    DebugPrint(L"UBSAN: Pointer operation %s %p to %p\n",
               Lhs > Rhs ? "overflowed" : "underflowed", (PVOID)Lhs, (PVOID)Rhs);

    /* Leave UBSAN frame */
    KepLeaveUbsanFrame();
}


/**
 * Handles the shift out of bounds error reported by UBSAN.
 *
 * @param Data
 *        Supplies a pointer to the UBSAN shift out of bounds data.
 *
 * @param Lhs
 *        Supplies LHS operand.
 *
 * @param Rhs
 *        Supplies RHS operand.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
KepHandleUbsanShiftOutOfBounds(PKUBSAN_SHIFT_OUT_OF_BOUNDS_DATA Data,
                               ULONG_PTR Lhs,
                               ULONG_PTR Rhs)
{
    ULONG LhsBitWidth;

    /* Check if this error was already reported */
    if(!KepCheckUbsanReport(&Data->Location))
    {
        /* Don't report twice */
        return;
    }

    /* Enter UBSAN frame */
    KepEnterUbsanFrame(&Data->Location, "Shift-Out-Of-Bounds");

    /* Calculate Lhs bit width */
    LhsBitWidth = 1 << (Data->LhsType->TypeInfo >> 1);

    /* Check a type of out of bounds error */
    if((Data->RhsType->TypeInfo & 1) && (KepGetSignedUbsanValue(Data->RhsType, (PVOID)Rhs) < 0))
    {
        /* Negative shift exponent, print error message to debug console */
        DebugPrint(L"UBSAN: Shift exponent %lld is negative\n", KepGetSignedUbsanValue(Data->RhsType, (PVOID)Rhs));
    }
    else if((Data->LhsType->TypeInfo & 1) && (KepGetSignedUbsanValue(Data->LhsType, (PVOID)Lhs) < 0))
    {
        /* Negative left shift value, print error message to debug console */
        DebugPrint(L"UBSAN: Left shift of negative value %lld\n", KepGetSignedUbsanValue(Data->LhsType, (PVOID)Lhs));
    }
    else if(KepGetUnsignedUbsanValue(Data->RhsType, (PVOID)Rhs) >= LhsBitWidth)
    {
        /* Shift exponent too large, print error message to debug console */
        DebugPrint(L"UBSAN: Shift exponent %lld is too large for %u-bit type %s\n",
                   KepGetUnsignedUbsanValue(Data->RhsType, (PVOID)Rhs), LhsBitWidth, Data->LhsType->TypeName);
    }
    else
    {
        /* Left shift too large, print error message to debug console */
        DebugPrint(L"UBSAN: Left shift of %lld by %lld places cannot be represented in type %s\n",
                   KepGetSignedUbsanValue(Data->LhsType, (PVOID)Lhs), KepGetSignedUbsanValue(Data->RhsType, (PVOID)Rhs),
                   Data->LhsType->TypeName);
    }

    /* Leave UBSAN frame */
    KepLeaveUbsanFrame();
}

/**
 * Handles the type mismatch error reported by UBSAN.
 *
 * @param Data
 *        Supplies a pointer to the UBSAN type mismatch data.
 *
 * @param Pointer
 *        Supplies a pointer operand.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
KepHandleUbsanTypeMismatch(PKUBSAN_TYPE_MISMATCH_DATA Data,
                           ULONG_PTR Pointer)
{
    /* Check the type of mismatch */
    if(!Pointer)
    {
        /* Handle NULL pointer dereference */
        KepHandleUbsanNullPointerDereference(Data);
    }
    else if(Data->Alignment && (((Pointer) & ((typeof(Pointer))(Data->Alignment) - 1)) != 0)) 
    {
        /* Handle misaligned access */
        KepHandleUbsanMisalignedAccess(Data, Pointer);
    }
    else
    {
        /* Handle object size mismatch */
        KepHandleUbsanObjectSizeMismatch(Data, Pointer);
    }
}

/**
 * Leaves the UBSAN frame by marking it as inactive.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
KepLeaveUbsanFrame()
{
    /* Leave UBSAN frame */
    KepUbsanActiveFrame = FALSE;
}

/**
 * Handles the addition overflow error. This is internal routine implementing ABI defined by CLANG UBSAN.
 *
 * @param Data
 *        Supplies a pointer to the UBSAN overflow data.
 *
 * @param Lhs
 *        Supplies LHS operand.
 *
 * @param Rhs
 *        Supplies RHS operand.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 *
 * @see https://github.com/llvm/llvm-project/blob/release/18.x/clang/lib/CodeGen/CodeGenFunction.h#L113
 */
XTCDECL
VOID
__ubsan_handle_add_overflow(IN PKUBSAN_OVERFLOW_DATA Data,
                            IN ULONG_PTR Lhs,
                            IN ULONG_PTR Rhs)
{
    /* Call UBSAN arithmetic overflow handler */
    KepHandleUbsanIntegerOverflow(Data, Lhs, Rhs);
}

/**
 * Handles the division overflow error. This is internal routine implementing ABI defined by CLANG UBSAN.
 *
 * @param Data
 *        Supplies a pointer to the UBSAN overflow data.
 *
 * @param Lhs
 *        Supplies LHS operand.
 *
 * @param Rhs
 *        Supplies RHS operand.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 *
 * @see https://github.com/llvm/llvm-project/blob/release/18.x/clang/lib/CodeGen/CodeGenFunction.h#L113
 */
XTCDECL
VOID
__ubsan_handle_divrem_overflow(IN PKUBSAN_OVERFLOW_DATA Data,
                               IN PVOID Lhs,
                               IN PVOID Rhs)
{
    /* Call UBSAN division overflow handler */
    KepHandleUbsanDivisionOverflow(Data, Lhs, Rhs);
}

/**
 * Handles the float cast overflow error. This is internal routine implementing ABI defined by CLANG UBSAN.
 *
 * @param Data
 *        Supplies a pointer to the UBSAN float cast overflow data.
 *
 * @param Lhs
 *        Supplies LHS operand.
 *
 * @param Rhs
 *        Supplies RHS operand.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 *
 * @see https://github.com/llvm/llvm-project/blob/release/18.x/clang/lib/CodeGen/CodeGenFunction.h#L113
 */
XTCDECL
VOID
__ubsan_handle_float_cast_overflow(IN PKUBSAN_FLOAT_CAST_OVERFLOW_DATA Data,
                                   IN ULONG_PTR Lhs,
                                   IN ULONG_PTR Rhs)
{
    /* Call UBSAN float cast overflow handler */
    KepHandleUbsanFloatCastOverflow(Data, Lhs, Rhs);
}

/**
 * Handles the function type mismatch error. This is internal routine implementing ABI defined by CLANG UBSAN.
 *
 * @param Data
 *        Supplies a pointer to the UBSAN function type mismatch data.
 *
 * @param Pointer
 *        Supplies pointer operand.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 *
 * @see https://github.com/llvm/llvm-project/blob/release/18.x/clang/lib/CodeGen/CodeGenFunction.h#L113
 */
XTCDECL
VOID
__ubsan_handle_function_type_mismatch(IN PKUBSAN_FUNCTION_TYPE_MISMATCH_DATA Data,
                                      IN ULONG_PTR Pointer)
{
    /* Call UBSAN function type mismatch handler */
    KepHandleUbsanFunctionTypeMismatch(Data, Pointer);
}

/**
 * Handles the invalid builtin error. This is internal routine implementing ABI defined by CLANG UBSAN.
 *
 * @param Data
 *        Supplies a pointer to the UBSAN invalid builtin data.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 *
 * @see https://github.com/llvm/llvm-project/blob/release/18.x/clang/lib/CodeGen/CodeGenFunction.h#L113
 */
XTCDECL
VOID
__ubsan_handle_invalid_builtin(IN PKUBSAN_INVALID_BUILTIN_DATA Data)
{
    /* Call UBSAN invalid builtin handler */
    KepHandleUbsanInvalidBuiltin(Data);
}

/**
 * Handles the multiplication overflow error. This is internal routine implementing ABI defined by CLANG UBSAN.
 *
 * @param Data
 *        Supplies a pointer to the UBSAN overflow data.
 *
 * @param Lhs
 *        Supplies LHS operand.
 *
 * @param Rhs
 *        Supplies RHS operand.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 *
 * @see https://github.com/llvm/llvm-project/blob/release/18.x/clang/lib/CodeGen/CodeGenFunction.h#L113
 */
XTCDECL
VOID
__ubsan_handle_mul_overflow(IN PKUBSAN_OVERFLOW_DATA Data,
                            IN ULONG_PTR Lhs,
                            IN ULONG_PTR Rhs)
{
    /* Call UBSAN arithmetic overflow handler */
    KepHandleUbsanIntegerOverflow(Data, Lhs, Rhs);
}

/**
 * Handles the negate overflow error. This is internal routine implementing ABI defined by CLANG UBSAN.
 *
 * @param Data
 *        Supplies a pointer to the UBSAN overflow data.
 *
 * @param OldValue
 *        Supplies old value.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 *
 * @see https://github.com/llvm/llvm-project/blob/release/18.x/clang/lib/CodeGen/CodeGenFunction.h#L113
 */
XTCDECL
VOID
__ubsan_handle_negate_overflow(IN PKUBSAN_OVERFLOW_DATA Data,
                               IN ULONG_PTR OldValue)
{
    /* Call UBSAN negate overflow handler */
    KepHandleUbsanNegateOverflow(Data, OldValue);
}

/**
 * Handles the out of bounds error. This is internal routine implementing ABI defined by CLANG UBSAN.
 *
 * @param Data
 *        Supplies a pointer to the UBSAN out of bounds data.
 *
 * @param Index
 *        Supplies an index operand.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 *
 * @see https://github.com/llvm/llvm-project/blob/release/18.x/clang/lib/CodeGen/CodeGenFunction.h#L113
 */
XTCDECL
VOID
__ubsan_handle_out_of_bounds(IN PKUBSAN_OUT_OF_BOUNDS_DATA Data,
                             IN ULONG_PTR Index)
{
    /* Call UBSAN out of bounds handler */
    KepHandleUbsanOutOfBounds(Data, Index);
}

/**
 * Handles the pointer overflow error. This is internal routine implementing ABI defined by CLANG UBSAN.
 *
 * @param Data
 *        Supplies a pointer to the UBSAN overflow data.
 *
 * @param Lhs
 *        Supplies LHS operand.
 *
 * @param Rhs
 *        Supplies RHS operand.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 *
 * @see https://github.com/llvm/llvm-project/blob/release/18.x/clang/lib/CodeGen/CodeGenFunction.h#L113
 */
XTCDECL
VOID
__ubsan_handle_pointer_overflow(IN PKUBSAN_OVERFLOW_DATA Data,
                                IN ULONG_PTR Lhs,
                                IN ULONG_PTR Rhs)
{
    /* Call UBSAN pointer overflow handler */
    KepHandleUbsanPointerOverflow(Data, Lhs, Rhs);
}

/**
 * Handles the shift out of bounds error. This is internal routine implementing ABI defined by CLANG UBSAN.
 *
 * @param Data
 *        Supplies a pointer to the UBSAN shift out of bounds data.
 *
 * @param Lhs
 *        Supplies LHS operand.
 *
 * @param Rhs
 *        Supplies RHS operand.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 *
 * @see https://github.com/llvm/llvm-project/blob/release/18.x/clang/lib/CodeGen/CodeGenFunction.h#L113
 */
XTCDECL
VOID
__ubsan_handle_shift_out_of_bounds(IN PKUBSAN_SHIFT_OUT_OF_BOUNDS_DATA Data,
                                   IN ULONG_PTR Lhs,
                                   IN ULONG_PTR Rhs)
{
    /* Call UBSAN out of bounds handler */
    KepHandleUbsanShiftOutOfBounds(Data, Lhs, Rhs);
}

/**
 * Handles the subtraction overflow error. This is internal routine implementing ABI defined by CLANG UBSAN.
 *
 * @param Data
 *        Supplies a pointer to the UBSAN overflow data.
 *
 * @param Lhs
 *        Supplies LHS operand.
 *
 * @param Rhs
 *        Supplies RHS operand.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 *
 * @see https://github.com/llvm/llvm-project/blob/release/18.x/clang/lib/CodeGen/CodeGenFunction.h#L113
 */
XTCDECL
VOID
__ubsan_handle_sub_overflow(IN PKUBSAN_OVERFLOW_DATA Data,
                            IN ULONG_PTR Lhs,
                            IN ULONG_PTR Rhs)
{
    /* Call UBSAN arithmetic overflow handler */
    KepHandleUbsanIntegerOverflow(Data, Lhs, Rhs);
}

/**
 * Handles the type mismatch error. This is internal routine implementing ABI defined by CLANG UBSAN.
 *
 * @param Data
 *        Supplies a pointer to the UBSAN type mismatch data.
 *
 * @param Pointer
 *        Supplies a pointer operand.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 *
 * @see https://github.com/llvm/llvm-project/blob/release/18.x/clang/lib/CodeGen/CodeGenFunction.h#L113
 */
XTCDECL
VOID
__ubsan_handle_type_mismatch(IN PKUBSAN_TYPE_MISMATCH_DATA Data,
                             IN ULONG_PTR Pointer)
{
    /* Call UBSAN type mismatch handler */
    KepHandleUbsanTypeMismatch(Data, Pointer);
}

/**
 * Handles the type mismatch error. This is internal routine implementing ABI defined by CLANG UBSAN.
 *
 * @param Data
 *        Supplies a pointer to the UBSAN type mismatch data.
 *
 * @param Pointer
 *        Supplies a pointer operand.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 *
 * @see https://github.com/llvm/llvm-project/blob/release/18.x/clang/lib/CodeGen/CodeGenFunction.h#L113
 */
XTCDECL
VOID
__ubsan_handle_type_mismatch_v1(IN PKUBSAN_TYPE_MISMATCH_DATA_V1 Data,
                                IN ULONG_PTR Pointer)
{
    KUBSAN_TYPE_MISMATCH_DATA MismatchData;

    /* Prepare UBSAN type mismatch data in old format */
    MismatchData.Alignment = 1UL << Data->LogAlignment;
    MismatchData.Location = Data->Location;
    MismatchData.Type = Data->Type;
    MismatchData.TypeCheckKind = Data->TypeCheckKind;

    /* Call UBSAN type mismatch handler */
    KepHandleUbsanTypeMismatch(&MismatchData, Pointer);
}
