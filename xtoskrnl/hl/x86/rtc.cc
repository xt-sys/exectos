/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/x86/rtc.cc
 * DESCRIPTION:     Hardware Real-Time Clock (RTC) support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Queries the hardware Real-Time Clock (RTC) for the current date and time.
 *
 * @param Time
 *        Supplies a pointer to a structure to receive the system time.
 *
 * @return This routine returns the status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
HL::Rtc::GetRealTimeClock(OUT PTIME_FIELDS Time)
{
    UCHAR Century1, Century2, CenturyRegister, RegisterB;
    TIME_FIELDS TimeProbe1, TimeProbe2;
    PACPI_FADT FadtTable;
    BOOLEAN PostMeridiem;
    XTSTATUS Status;
    ULONG Index;

    /* Locate the ACPI FADT table */
    Status = HL::Acpi::GetAcpiTable(ACPI_FADT_SIGNATURE, (PACPI_DESCRIPTION_HEADER*)&FadtTable);
    if(Status == STATUS_SUCCESS && FadtTable && FadtTable->CenturyAlarmIndex)
    {
        /* Cache the dynamically provided Century register index */
        CenturyRegister = FadtTable->CenturyAlarmIndex;
    }
    else
    {
        /* Century register is unavailable */
        CenturyRegister = 0;
        Century1 = 0;
        Century2 = 0;
    }

    /* Read the RTC Status Register B to determine hardware data formats */
    RegisterB = HL::Firmware::ReadCmosRegister(CMOS_REGISTER_B);

    /* Assume failure */
    Status = STATUS_UNSUCCESSFUL;

    /* Execute a maximum of 100 retries to obtain a stable RTC snapshot */
    for(Index = 0; Index < 100; Index++)
    {
        /* Wait until the RTC hardware finishes any ongoing background updates */
        while(HL::Firmware::ReadCmosRegister(CMOS_REGISTER_A) & CMOS_REGISTER_A_UPDATE_IN_PROGRESS)
        {
            /* Yield the processor */
            AR::CpuFunc::YieldProcessor();
        }

        /* Latch the first sequential hardware time snapshot */
        TimeProbe1.Hour = HL::Firmware::ReadCmosRegister(CMOS_REGISTER_HOUR);
        TimeProbe1.Minute = HL::Firmware::ReadCmosRegister(CMOS_REGISTER_MINUTE);
        TimeProbe1.Second = HL::Firmware::ReadCmosRegister(CMOS_REGISTER_SECOND);
        TimeProbe1.Day = HL::Firmware::ReadCmosRegister(CMOS_REGISTER_DAY);
        TimeProbe1.Month = HL::Firmware::ReadCmosRegister(CMOS_REGISTER_MONTH);
        TimeProbe1.Year = HL::Firmware::ReadCmosRegister(CMOS_REGISTER_YEAR);
        TimeProbe1.Weekday = HL::Firmware::ReadCmosRegister(CMOS_REGISTER_WEEKDAY);

        /* Check if Century register is available */
        if(CenturyRegister)
        {
            /* Read the corresponding Century register */
            Century1 = HL::Firmware::ReadCmosRegister(CenturyRegister);
        }

        /* Wait until the RTC hardware finishes any ongoing background updates */
        while(HL::Firmware::ReadCmosRegister(CMOS_REGISTER_A) & CMOS_REGISTER_A_UPDATE_IN_PROGRESS)
        {
            /* Yield the processor */
            AR::CpuFunc::YieldProcessor();
        }

        /* Latch the second sequential hardware time snapshot for verification */
        TimeProbe2.Hour = HL::Firmware::ReadCmosRegister(CMOS_REGISTER_HOUR);
        TimeProbe2.Minute = HL::Firmware::ReadCmosRegister(CMOS_REGISTER_MINUTE);
        TimeProbe2.Second = HL::Firmware::ReadCmosRegister(CMOS_REGISTER_SECOND);
        TimeProbe2.Day = HL::Firmware::ReadCmosRegister(CMOS_REGISTER_DAY);
        TimeProbe2.Month = HL::Firmware::ReadCmosRegister(CMOS_REGISTER_MONTH);
        TimeProbe2.Year = HL::Firmware::ReadCmosRegister(CMOS_REGISTER_YEAR);
        TimeProbe2.Weekday = HL::Firmware::ReadCmosRegister(CMOS_REGISTER_WEEKDAY);

        /* Check if Century register is available */
        if(CenturyRegister)
        {
            /* Read the corresponding Century register */
            Century2 = HL::Firmware::ReadCmosRegister(CenturyRegister);
        }

        /* Compare both snapshots to guarantee data consistency */
        if((TimeProbe1.Hour == TimeProbe2.Hour) &&
           (TimeProbe1.Minute == TimeProbe2.Minute) &&
           (TimeProbe1.Second == TimeProbe2.Second) &&
           (TimeProbe1.Day == TimeProbe2.Day) &&
           (TimeProbe1.Month == TimeProbe2.Month) &&
           (TimeProbe1.Year == TimeProbe2.Year) &&
           (TimeProbe1.Weekday == TimeProbe2.Weekday) &&
           (Century1 == Century2))
        {
            /* A stable time sample was acquired, break the loop */
            Status = STATUS_SUCCESS;
            break;
        }
    }

    /* Copy the validated data into the output buffer */
    Time->Hour = TimeProbe1.Hour;
    Time->Minute = TimeProbe1.Minute;
    Time->Second = TimeProbe1.Second;
    Time->Milliseconds = 0;
    Time->Day = TimeProbe1.Day;
    Time->Month = TimeProbe1.Month;
    Time->Year = TimeProbe1.Year;
    Time->Weekday = TimeProbe1.Weekday;

    /* Check if RTC is operating in 12-hour mode */
    if(!(RegisterB & CMOS_REGISTER_B_24_HOUR))
    {
        /* Cache the PM status and strip the hardware flag */
        PostMeridiem = (Time->Hour & CMOS_RTC_POST_MERIDIEM) != 0;
        Time->Hour &= ~CMOS_RTC_POST_MERIDIEM;
    }

    /* Convert Binary-Coded Decimal (BCD) values to standard integers if necessary */
    if(!(RegisterB & CMOS_REGISTER_B_BINARY))
    {
        /* Decode all standard time fields */
        Time->Hour = BCD_TO_DECIMAL(Time->Hour);
        Time->Minute = BCD_TO_DECIMAL(Time->Minute);
        Time->Second = BCD_TO_DECIMAL(Time->Second);
        Time->Day = BCD_TO_DECIMAL(Time->Day);
        Time->Month = BCD_TO_DECIMAL(Time->Month);
        Time->Year = BCD_TO_DECIMAL(Time->Year);
        Time->Weekday = BCD_TO_DECIMAL(Time->Weekday);

        /* Check if Century byte is available */
        if(CenturyRegister)
        {
            /* Convert Century byte */
            Century1 = BCD_TO_DECIMAL(Century1);
        }
    }

    /* Standardize hours into a strict 24-hour format */
    if(!(RegisterB & CMOS_REGISTER_B_24_HOUR))
    {
        /* Adjust for midnight and noon boundary cases */
        if(Time->Hour == 12)
        {
            /* 12 AM evaluates to 00:00, 12 PM evaluates to 12:00 */
            Time->Hour = PostMeridiem ? 12 : 0;
        }
        else
        {
            /* Add 12 hours for PM times */
            Time->Hour += PostMeridiem ? 12 : 0;
        }
    }

    /* Merge the century offset with the 2-digit hardware year */
    if(Century1 >= 19 && Century1 <= 30)
    {
        /* Utilize the hardware-provided century base */
        Time->Year += (Century1 * 100);
    }
    else
    {
        /* Century byte is invalid; apply the sliding window */
        Time->Year += (Time->Year > 80) ? 1900 : 2000;
    }

    /* Return status code */
    return Status;
}

/**
 * Updates the hardware Real-Time Clock (RTC) with the provided date and time.
 *
 * @param Time
 *        Supplies a pointer to a structure with populated data and time.
 *
 * @return This routine returns the status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
HL::Rtc::SetRealTimeClock(IN PTIME_FIELDS Time)
{
    UCHAR Century, CenturyRegister, RegisterB;
    TIME_FIELDS SystemTime;
    BOOLEAN PostMeridiem;
    PACPI_FADT FadtTable;
    XTSTATUS Status;

    /* Validate the input time boundaries against calendar limits */
    if(Time->Hour > 23 || Time->Minute > 59 || Time->Second > 59 ||
       Time->Day == 0 || Time->Day > 31 || Time->Month == 0 ||
       Time->Month > 12 || Time->Weekday == 0 || Time->Weekday > 7)
    {
        /* Invalid time parameters, return error code */
        return STATUS_INVALID_PARAMETER;
    }

    /* Assume Ante Meridiem */
    PostMeridiem = FALSE;

    /* Extract local copy */
    SystemTime.Hour = Time->Hour;
    SystemTime.Minute = Time->Minute;
    SystemTime.Second = Time->Second;
    SystemTime.Day = Time->Day;
    SystemTime.Month = Time->Month;
    SystemTime.Year = (Time->Year % 100);
    SystemTime.Weekday = Time->Weekday;
    Century = (UCHAR)(Time->Year / 100);

    /* Locate the ACPI FADT table */
    Status = HL::Acpi::GetAcpiTable(ACPI_FADT_SIGNATURE, (PACPI_DESCRIPTION_HEADER*)&FadtTable);
    if(Status == STATUS_SUCCESS && FadtTable && FadtTable->CenturyAlarmIndex)
    {
        /* Cache the dynamically provided Century register index */
        CenturyRegister = FadtTable->CenturyAlarmIndex;
    }
    else
    {
        /* Century register is unavailable */
        CenturyRegister = 0;
    }

    /* Read the RTC Status Register B to determine hardware data formats */
    RegisterB = HL::Firmware::ReadCmosRegister(CMOS_REGISTER_B);

    /* Format hours if the hardware is running in 12-hour mode */
    if(!(RegisterB & CMOS_REGISTER_B_24_HOUR))
    {
        /* Determine if the time is PM */
        PostMeridiem = (SystemTime.Hour >= 12);

        /* Adjust for midnight and noon boundary cases */
        if(SystemTime.Hour == 0)
        {
            /* Midnight evaluates to 12 AM */
            SystemTime.Hour = 12;
        }
        else if(SystemTime.Hour > 12)
        {
            /* Post-noon hour */
            SystemTime.Hour -= 12;
        }

        /* Convert to BCD first if needed and apply the hardware PM flag */
        if(!(RegisterB & CMOS_REGISTER_B_BINARY))
        {
            /* Encode to BCD */
            SystemTime.Hour = DECIMAL_TO_BCD(SystemTime.Hour);
        }

        /* Apply the hardware PM flag to the highest bit */
        if(PostMeridiem)
        {
            /* Set PM flag */
            SystemTime.Hour |= CMOS_RTC_POST_MERIDIEM;
        }
    }
    else
    {
        /* 24-hour mode, simply encode to BCD if necessary */
        if(!(RegisterB & CMOS_REGISTER_B_BINARY))
        {
            /* Encode to BCD */
            SystemTime.Hour = DECIMAL_TO_BCD(SystemTime.Hour);
        }
    }

    /* Convert remaining standard fields to BCD if necessary */
    if(!(RegisterB & CMOS_REGISTER_B_BINARY))
    {
        /* Encode all standard time fields */
        SystemTime.Minute  = DECIMAL_TO_BCD(SystemTime.Minute);
        SystemTime.Second  = DECIMAL_TO_BCD(SystemTime.Second);
        SystemTime.Day     = DECIMAL_TO_BCD(SystemTime.Day);
        SystemTime.Month   = DECIMAL_TO_BCD(SystemTime.Month);
        SystemTime.Year    = DECIMAL_TO_BCD(SystemTime.Year);
        SystemTime.Weekday = DECIMAL_TO_BCD((UCHAR)SystemTime.Weekday);

        /* Encode the Century byte */
        Century = DECIMAL_TO_BCD(Century);
    }

    /* Freeze the RTC to prevent data tearing */
    HL::Firmware::WriteCmosRegister(CMOS_REGISTER_B, RegisterB | CMOS_REGISTER_B_SET_CLOCK);

    /* Push the formatted time values into the hardware registers */
    HL::Firmware::WriteCmosRegister(CMOS_REGISTER_HOUR, SystemTime.Hour);
    HL::Firmware::WriteCmosRegister(CMOS_REGISTER_MINUTE, SystemTime.Minute);
    HL::Firmware::WriteCmosRegister(CMOS_REGISTER_SECOND, SystemTime.Second);
    HL::Firmware::WriteCmosRegister(CMOS_REGISTER_DAY, SystemTime.Day);
    HL::Firmware::WriteCmosRegister(CMOS_REGISTER_MONTH, SystemTime.Month);
    HL::Firmware::WriteCmosRegister(CMOS_REGISTER_YEAR, SystemTime.Year);
    HL::Firmware::WriteCmosRegister(CMOS_REGISTER_WEEKDAY, SystemTime.Weekday);

    /* Check if Century register is available */
    if(CenturyRegister)
    {
        /* Write the corresponding Century register */
        HL::Firmware::WriteCmosRegister(CenturyRegister, Century);
    }

    /* Unfreeze the RTC */
    HL::Firmware::WriteCmosRegister(CMOS_REGISTER_B, RegisterB);

    /* Return success status code */
    return STATUS_SUCCESS;
}
