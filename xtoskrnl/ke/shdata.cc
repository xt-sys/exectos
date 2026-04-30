/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/shdata.cc
 * DESCRIPTION:     Kernel Shared Data
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Retrieves the current interrupt time using a lock-free read mechanism.
 *
 * @return This routine returns a LARGE_INTEGER containing the interrupt time.
 *
 * @since XT 1.0
 */
XTAPI
LARGE_INTEGER
KE::SharedData::GetInterruptTime(VOID)
{
    LARGE_INTEGER InterruptTime;

    /* Initialize to zero */
    InterruptTime.QuadPart = 0;

    /* Perform a lock-free read sequence */
    do
    {
        /* Read the primary high part and low part */
        InterruptTime.HighPart = KernelSharedData->SystemTime.High1Part;
        InterruptTime.LowPart = KernelSharedData->SystemTime.LowPart;
    }
    while(InterruptTime.HighPart != KernelSharedData->SystemTime.High2Part);

    /* Return the 64-bit time */
    return InterruptTime;
}

/**
 * Retrieves a pointer to the memory-mapped Kernel Shared Data.
 *
 * @return This routine returns a pointer to the KSHARED_DATA structure.
 *
 * @since XT 1.0
 */
XTAPI
PKSHARED_DATA
KE::SharedData::GetKernelSharedData(VOID)
{
    /* Return the internally managed pointer */
    return KernelSharedData;
}

/**
 * Retrieves the current system time using a lock-free read mechanism.
 *
 * @return This routine returns a LARGE_INTEGER containing the system time.
 *
 * @since XT 1.0
 */
XTAPI
LARGE_INTEGER
KE::SharedData::GetSystemTime(VOID)
{
    LARGE_INTEGER CurrentTime;

    /* Initialize to zero */
    CurrentTime.QuadPart = 0;

    /* Perform a lock-free read sequence */
    do
    {
        /* Read the primary high part and low part */
        CurrentTime.HighPart = KernelSharedData->SystemTime.High1Part;
        CurrentTime.LowPart = KernelSharedData->SystemTime.LowPart;
    }
    while(CurrentTime.HighPart != KernelSharedData->SystemTime.High2Part);

    /* Return the 64-bit time */
    return CurrentTime;
}

/**
 * Maps and initializes the Kernel Shared Data (KSD) structure.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::SharedData::InitializeKernelSharedData(VOID)
{
    PCSTR SourceString;
    XTSTATUS Status;

    /* Map Kernel Shared Data (KSD) */
    Status = MM::Manager::MapKernelSharedData();
    if(Status != STATUS_SUCCESS)
    {
        /* Failed to map KSD, raise kernel panic */
        KE::Crash::Panic(0);
    }

    /* Bind the pointer to the architectural virtual address */
    KernelSharedData = (PKSHARED_DATA)MM_KERNEL_SHARED_DATA_ADDRESS;

    /* Populate numeric version identifiers */
    KernelSharedData->XtMajorVersion = XTOS_VERSION_MAJOR;
    KernelSharedData->XtMinorVersion = XTOS_VERSION_MINOR;

    /* Convert and copy system build string */
    SourceString = XTOS_VERSION_BUILD;
    RTL::String::StringToWideString(KernelSharedData->XtBuild, (PCSTR*)&SourceString,
                                    (sizeof(KernelSharedData->XtBuild) / sizeof(WCHAR)) - 1);

    /* Convert and copy system build hash string */
    SourceString = XTOS_VERSION_HASH;
    RTL::String::StringToWideString(KernelSharedData->XtBuildHash, (PCSTR*)&SourceString,
                                    (sizeof(KernelSharedData->XtBuildHash) / sizeof(WCHAR)) - 1);

    /* Convert and copy system architecture string */
    SourceString = XTOS_VERSION_ARCH;
    RTL::String::StringToWideString(KernelSharedData->XtArchitecture, (PCSTR*)&SourceString,
                                    (sizeof(KernelSharedData->XtArchitecture) / sizeof(WCHAR)) - 1);

    /* Convert and copy system build date string */
    SourceString = XTOS_VERSION_DATE;
    RTL::String::StringToWideString(KernelSharedData->XtDate, (PCSTR*)&SourceString,
                                    (sizeof(KernelSharedData->XtDate) / sizeof(WCHAR)) - 1);

    /* Convert and copy system build full date string */
    SourceString = XTOS_VERSION_FULLDATE;
    RTL::String::StringToWideString(KernelSharedData->XtFullDate, (PCSTR*)&SourceString,
                                    (sizeof(KernelSharedData->XtFullDate) / sizeof(WCHAR)) - 1);
}

/**
 * Updates the global interrupt time using a strict lock-free write mechanism.
 *
 * @param Time
 *        Supplies the new interrupt time as a 64-bit LARGE_INTEGER value.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::SharedData::SetInterruptTime(IN LARGE_INTEGER Time)
{
    /* Set the new interrupt time */
    KernelSharedData->InterruptTime.High2Part = Time.HighPart;
    KernelSharedData->InterruptTime.LowPart = Time.LowPart;
    KernelSharedData->InterruptTime.High1Part = Time.HighPart;
}

/**
 * Updates the global system time using a strict lock-free write mechanism.
 *
 * @param Time
 *        Supplies the new system time as a 64-bit LARGE_INTEGER value.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KE::SharedData::SetSystemTime(IN LARGE_INTEGER Time)
{
    /* Set the new system time */
    KernelSharedData->SystemTime.High2Part = Time.HighPart;
    KernelSharedData->SystemTime.LowPart = Time.LowPart;
    KernelSharedData->SystemTime.High1Part = Time.HighPart;
}
