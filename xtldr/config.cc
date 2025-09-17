/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/config.cc
 * DESCRIPTION:     XT Boot Loader Configuration
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#include <xtldr.h>


/**
 * @brief Retrieves the value of a specific OS boot option from a list.
 *
 * @param Options
 *        A pointer to the head of a list of XTBL_CONFIG_ENTRY structures.
 *
 * @param OptionName
 *        A pointer to wide string that contains the name of the boot option to retrieve.
 *
 * @param OptionValue
 *        A pointer to a variable that receives a pointer to the retrieved boot option's value.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlGetBootOptionValue(IN PLIST_ENTRY Options,
                     IN PCWSTR OptionName,
                     OUT PWCHAR *OptionValue)
{
    PXTBL_CONFIG_ENTRY ConfigEntry;
    PLIST_ENTRY ConfigList;
    ULONG KeyLength, ValueLength;
    EFI_STATUS Status;

    /* Assume the option will not be found */
    *OptionValue = NULLPTR;

    /* Get the length of the option name we are looking for */
    KeyLength = RtlWideStringLength(OptionName, 0);

    /* Start iterating from the first entry in the options list */
    ConfigList = Options->Flink;
    while(ConfigList != Options)
    {
        /* Get the container record for the current config entry */
        ConfigEntry = CONTAIN_RECORD(ConfigList, XTBL_CONFIG_ENTRY, Flink);

        /* Compare the current entry's name with the requested option name */
        if(RtlCompareWideStringInsensitive(ConfigEntry->Name, OptionName, KeyLength) == 0)
        {
            /* Found the option, now prepare to copy its value */
            ValueLength = RtlWideStringLength(ConfigEntry->Value, 0);

            /* Allocate memory for the output value string */
            Status = BlAllocateMemoryPool((ValueLength + 1) * sizeof(WCHAR), (PVOID *)OptionValue);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Memory allocation failure, print debug message and return status code */
                BlDebugPrint(L"ERROR: Memory allocation failure (Status Code: 0x%zX)\n", Status);
                *OptionValue = NULLPTR;
                return Status;
            }

            /* Copy the value and NULL-terminate the new string */
            RtlCopyMemory(*OptionValue, ConfigEntry->Value, ValueLength * sizeof(WCHAR));
            (*OptionValue)[ValueLength] = L'\0';

            /* Successfully retrieved the option value, return success */
            return STATUS_EFI_SUCCESS;
        }

        /* Move to the next entry in the list */
        ConfigList = ConfigList->Flink;
    }

    /* Option not found */
    return STATUS_EFI_NOT_FOUND;
}

/**
 * Returns a boolean value of the specified configuration key.
 *
 * @param ConfigName
 *        Specifies the configuration key to return its boolean representation.
 *
 * @return This routine returns a boolean representation of the configuration value.
 *
 * @since XT 1.0
 */
XTCDECL
BOOLEAN
BlGetConfigBooleanValue(IN PCWSTR ConfigName)
{
    PWCHAR Value;

    /* Get config value */
    BlGetConfigValue(ConfigName, &Value);

    /* Check if option is enabled */
    if(RtlCompareWideStringInsensitive(Value, L"ENABLED", 0) == 0 ||
       RtlCompareWideStringInsensitive(Value, L"ON", 0) == 0 ||
       RtlCompareWideStringInsensitive(Value, L"TRUE", 0) == 0 ||
       RtlCompareWideStringInsensitive(Value, L"YES", 0) == 0)
    {
        /* This option is enabled */
        return TRUE;
    }

    /* Return FALSE by default */
    return FALSE;
}

/**
 * Returns a value of the specified configuration key.
 *
 * @param ConfigName
 *        Specifies the configuration key to return its value.
 *
 * @return This routine returns a pointer to the configuration value, or NULLPTR if key was not found.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlGetConfigValue(IN PCWSTR ConfigName,
                 OUT PWCHAR *ConfigValue)
{
    PXTBL_CONFIG_ENTRY ConfigEntry;
    PLIST_ENTRY ConfigListEntry;
    SIZE_T KeyLength, ValueLength;
    EFI_STATUS Status;
    PWCHAR Value;

    /* Assume the option will not be found */
    *ConfigValue = NULLPTR;

    /* Get config entry name length */
    KeyLength = RtlWideStringLength(ConfigName, 0);

    /* Iterate through config entries */
    ConfigListEntry = BlpConfig.Flink;
    while(ConfigListEntry != &BlpConfig)
    {
        /* Get config entry */
        ConfigEntry = CONTAIN_RECORD(ConfigListEntry, XTBL_CONFIG_ENTRY, Flink);

        /* Check if requested configuration found */
        if(RtlCompareWideStringInsensitive(ConfigEntry->Name, ConfigName, KeyLength) == 0)
        {
            /* Get value length */
            ValueLength = RtlWideStringLength(ConfigEntry->Value, 0);

            /* Allocate memory for value */
            Status = BlAllocateMemoryPool((ValueLength + 1) * sizeof(WCHAR), (PVOID *)&Value);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Memory allocation failure, return NULLPTR */
                BlDebugPrint(L"ERROR: Memory allocation failure (Status Code: 0x%zX)\n", Status);
                return Status;
            }

            /* Copy value and return it */
            RtlCopyMemory(Value, ConfigEntry->Value, ValueLength * sizeof(WCHAR));
            Value[ValueLength] = L'\0';
            *ConfigValue = Value;
            return STATUS_EFI_SUCCESS;
        }

        /* Move to the next config entry */
        ConfigListEntry = ConfigListEntry->Flink;
    }

    /* Config entry not found, return NULLPTR */
    return STATUS_EFI_NOT_FOUND;
}

/**
 * Retrieves the list of user-editable boot options.
 *
 * @param OptionsArray
 *        A pointer to a variable that will receive the pointer to the array of editable option names.
 *
 * @param OptionsCount
 *        A pointer to a variable that will be updated with the number of elements in the OptionsArray.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlGetEditableOptions(OUT PCWSTR **OptionsArray,
                     OUT PULONG OptionsCount)
{
    ULONG Count = 0;

    /* Return a pointer to the global array of editable options */
    *OptionsArray = BlpEditableConfigOptions;

    /* Calculate the number of elements in the array */
    while(BlpEditableConfigOptions[Count])
    {
        Count++;
    }

    /* Return the number of elements */
    *OptionsCount = Count;
}

/**
 * Sets the value of a specific OS boot option in a list, or adds it if it doesn't exist.
 *
 * @param Options
 *       A pointer to the head of a list of XTBL_CONFIG_ENTRY structures.
 *
 * @param OptionName
 *       A pointer to a wide string that contains the name of the boot option to set.
 *
 * @param OptionValue
 *       A pointer to a wide string that contains the new value for the boot option.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlSetBootOptionValue(IN PLIST_ENTRY Options,
                     IN PCWSTR OptionName,
                     IN PCWSTR OptionValue)
{
    PXTBL_CONFIG_ENTRY ConfigEntry;
    PLIST_ENTRY ConfigList;
    ULONG Length;
    EFI_STATUS Status;

    /* Get the length of the option name we are looking for */
    Length = RtlWideStringLength(OptionName, 0);

    /* Start iterating from the first entry in the options list */
    ConfigList = Options->Flink;
    while(ConfigList != Options)
    {
        /* Get the container record for the current config entry */
        ConfigEntry = CONTAIN_RECORD(ConfigList, XTBL_CONFIG_ENTRY, Flink);

        /* Compare the current entry's name with the requested option name */
        if(RtlCompareWideStringInsensitive(ConfigEntry->Name, OptionName, Length) == 0)
        {
            /* Found the option, get its length */
            Length = RtlWideStringLength(OptionValue, 0);

            /* Reallocate memory for the new value */
            Status = BlFreeMemoryPool(ConfigEntry->Value);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Failed to free memory, return status code */
                return Status;
            }

            /* Allocate new memory for the updated value */
            Status = BlAllocateMemoryPool((Length + 1) * sizeof(WCHAR), (PVOID *)&ConfigEntry->Value);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Memory allocation failure, print debug message and return status code */
                BlDebugPrint(L"ERROR: Memory allocation failure (Status Code: 0x%zX)\\n", Status);
                return Status;
            }

            /* Copy the value and NULL-terminate the new string */
            RtlCopyMemory(ConfigEntry->Value, OptionValue, Length * sizeof(WCHAR));
            ConfigEntry->Value[Length] = L'\0';
            return STATUS_EFI_SUCCESS;
        }

        /* Move to the next entry in the list */
        ConfigList = ConfigList->Flink;
    }

    /* Option not found, allocate memory for the new one */
    Status = BlAllocateMemoryPool(sizeof(XTBL_CONFIG_ENTRY), (PVOID *)&ConfigEntry);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure, print debug message and return status code */
        BlDebugPrint(L"ERROR: Memory allocation failure (Status Code: 0x%zX)\\n", Status);
        return Status;
    }

    /* Allocate memory for the option name */
    Length = RtlWideStringLength(OptionName, 0);
    Status = BlAllocateMemoryPool((Length + 1) * sizeof(WCHAR), (PVOID *)&ConfigEntry->Name);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure, print debug message and return status code */
        BlDebugPrint(L"ERROR: Memory allocation failure (Status Code: 0x%zX)\\n", Status);
        BlFreeMemoryPool(ConfigEntry);
        return Status;
    }

    /* Copy the option name and NULL-terminate the new string */
    RtlCopyMemory(ConfigEntry->Name, OptionName, Length * sizeof(WCHAR));
    ConfigEntry->Name[Length] = L'\0';

    /* Allocate memory for the option value */
    Length = RtlWideStringLength(OptionValue, 0);
    Status = BlAllocateMemoryPool((Length + 1) * sizeof(WCHAR), (PVOID *)&ConfigEntry->Value);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure, print debug message and return status code */
        BlDebugPrint(L"ERROR: Memory allocation failure (Status Code: 0x%zX)\\n", Status);
        BlFreeMemoryPool(ConfigEntry->Name);
        BlFreeMemoryPool(ConfigEntry);
        return Status;
    }

    /* Copy the value and NULL-terminate the new string */
    RtlCopyMemory(ConfigEntry->Value, OptionValue, Length * sizeof(WCHAR));
    ConfigEntry->Value[Length] = L'\0';

    /* Insert the new config entry at the end of the options list */
    RtlInsertTailList(Options, &ConfigEntry->Flink);

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Updates existing configuration value.
 *
 * @param ConfigName
 *        Specifies the configuration key to update.
 *
 * @param ConfigValue
 *        Specifies the new configuration value.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlSetConfigValue(IN PCWSTR ConfigName,
                 IN PCWSTR ConfigValue)
{
    PXTBL_CONFIG_ENTRY ConfigEntry;
    PLIST_ENTRY ConfigListEntry;
    EFI_STATUS Status;
    SIZE_T Length;

    /* Get config entry name length */
    Length = RtlWideStringLength(ConfigName, 0);

    /* Iterate through config entries */
    ConfigListEntry = BlpConfig.Flink;
    while(ConfigListEntry != &BlpConfig)
    {
        /* Get config entry */
        ConfigEntry = CONTAIN_RECORD(ConfigListEntry, XTBL_CONFIG_ENTRY, Flink);

        /* Check if requested configuration found */
        if(RtlCompareWideStringInsensitive(ConfigEntry->Name, ConfigName, Length) == 0)
        {
            /* Check new config value length */
            Length = RtlWideStringLength(ConfigValue, 0);

            /* Reallocate memory for new config value */
            Status = BlFreeMemoryPool(ConfigEntry->Value);
            if(Status == STATUS_EFI_SUCCESS)
            {
                /* Successfully freed memory, allocate a new pool */
                Status = BlAllocateMemoryPool((Length + 1) * sizeof(WCHAR), (PVOID *)&ConfigEntry->Value);
            }

            /* Check memory reallocation status */
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Failed to reallocate memory */
                return Status;
            }

            /* Update config value */
            RtlCopyMemory(ConfigEntry->Value, ConfigValue, Length * sizeof(WCHAR));
            ConfigEntry->Value[Length] = L'\0';

            /* Return success */
            return STATUS_EFI_SUCCESS;
        }

        /* Move to the next config entry */
        ConfigListEntry = ConfigListEntry->Flink;
    }

    /* Config entry not found */
    return STATUS_EFI_NOT_FOUND;
}

/**
 * Loads and parses XTLDR configuration file.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlpLoadConfiguration()
{
    PLIST_ENTRY SectionListEntry;
    EFI_STATUS Status;
    PCHAR ConfigData;

    /* Initialize configuration pointer */
    RtlInitializeListHead(&BlpConfigSections);

    /* Read data from configuration file */
    Status = BlpReadConfigFile(XTBL_LOADER_DIRECTORY_PATH, L"XTLDR.INI", &ConfigData);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to read config file, try with architecture specific directory */
        Status = BlpReadConfigFile(XTBL_ARCH_LOADER_DIRECTORY_PATH, L"XTLDR.INI", &ConfigData);
    }

    /* Check if configuration was read successfully */
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to load configuration */
        BlDebugPrint(L"Failed to load FS0:/EFI/BOOT/XTLDR/XTLDR.INI configuration file (Status Code: 0x%zX)\n", Status);
        return Status;
    }

    /* Parse configuration data */
    Status = BlpParseConfigFile(ConfigData, &BlpConfigSections);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to parse configuration */
        BlDebugPrint(L"Failed to parse FS0:/EFI/BOOT/XTLDR/XTLDR.INI configuration file (Status Code: 0x%zX)\n", Status);
        return Status;
    }

    /* Iterate through config sections */
    SectionListEntry = BlpConfigSections.Flink;
    while(SectionListEntry != &BlpConfigSections)
    {
        /* Get config section */
        PXTBL_CONFIG_SECTION Section = CONTAIN_RECORD(SectionListEntry, XTBL_CONFIG_SECTION, Flink);

        /* Look for global XTLDR configuration section */
        if(RtlCompareWideStringInsensitive(Section->SectionName, L"XTLDR", 5) == 0)
        {
            /* Update global configuration */
            BlpUpdateConfiguration(&Section->Options);

            /* Remove XTLDR section from the list */
            RtlRemoveEntryList(SectionListEntry);
            break;
        }

        /* Move to the next section */
        SectionListEntry = SectionListEntry->Flink;
    }

    /* Update boot menu OS list */
    BlpMenuList = &BlpConfigSections;

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Parses command line arguments and updates global configuration.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlpParseCommandLine(VOID)
{
    EFI_GUID LIPGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    PEFI_LOADED_IMAGE_PROTOCOL LoadedImage;
    PWCHAR Argument, Key, LastArg, Value;
    PXTBL_CONFIG_ENTRY Option;
    EFI_STATUS Status;
    SIZE_T KeyLength, ValueLength;
    LIST_ENTRY Config;

    /* Initialize configuration list */
    RtlInitializeListHead(&Config);

    /* Handle loaded image protocol */
    Status = EfiSystemTable->BootServices->HandleProtocol(EfiImageHandle, &LIPGuid, (PVOID *)&LoadedImage);
    if(Status == STATUS_EFI_SUCCESS)
    {
        /* Check if launched from UEFI shell */
        if(LoadedImage && LoadedImage->LoadOptions)
        {
            /* Tokenize provided options */
            Argument = RtlTokenizeWideString((PWCHAR)LoadedImage->LoadOptions, L" ", &LastArg);

            /* Iterate over all arguments passed to boot loader */
            while(Argument != NULLPTR)
            {
                /* Store key name */
                Key = Argument;

                /* Find end of the key */
                while(*Argument != L'=' && *Argument != L'\0' && *Argument != L'\n')
                {
                    /* Advance to the next character */
                    Argument++;
                }

                /* Mark end of the key and advance to the next character */
                *Argument = L'\0';
                Argument++;

                /* Store value */
                Value = Argument;

                /* Find end of the value */
                while(*Argument != L'\0' && *Argument != L'\n')
                {
                    /* Advance to the next character */
                    Argument++;
                }

                /* Mark end of the value and advance to the next character */
                *Argument = L'\0';
                Argument++;

                /* Get length of the key and its value */
                KeyLength = RtlWideStringLength(Key, 0);
                ValueLength = RtlWideStringLength(Value, 0);

                /* Check if argument is valid */
                if(KeyLength == 0 || ValueLength == 0)
                {
                    /* Invalid argument, skip to the next one */
                    continue;
                }

                /* Allocate memory for new option */
                Status = BlAllocateMemoryPool(sizeof(XTBL_CONFIG_ENTRY), (PVOID*)&Option);
                if(Status == STATUS_EFI_SUCCESS)
                {
                    /* Allocate more memory for option name */
                    Status = BlAllocateMemoryPool(sizeof(WCHAR) * (KeyLength + 1), (PVOID*)&Option->Name);
                    if(Status == STATUS_EFI_SUCCESS)
                    {
                        /* Allocate even more memory for option value */
                        Status = BlAllocateMemoryPool(sizeof(WCHAR) * (ValueLength + 1), (PVOID*)&Option->Value);
                    }
                }
                if(Status != STATUS_EFI_SUCCESS)
                {
                    /* Some memory allocation failed */
                    return Status;
                }

                /* Set entry name and value */
                RtlCopyMemory(Option->Name, Key, (KeyLength * sizeof(WCHAR)));
                RtlCopyMemory(Option->Value, Value, (ValueLength * sizeof(WCHAR)));
                Option->Name[KeyLength] = L'\0';
                Option->Value[ValueLength] = L'\0';

                /* Add entry to the list */
                RtlInsertTailList(&Config, &Option->Flink);

                /* Take next argument */
                Argument = RtlTokenizeWideString(NULLPTR, L" ", &LastArg);
            }

            /* Update global configuration */
            BlpUpdateConfiguration(&Config);
        }
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Parses configuration INI file.
 *
 * @param RawConfig
 *        Suplies a pointer to configuration INI file to be parsed.
 *
 * @param Configuration
 *        Supplies a pointer to memory region where parsed configuration will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlpParseConfigFile(IN CONST PCHAR RawConfig,
                   OUT PLIST_ENTRY Configuration)
{
    SIZE_T SectionLength, KeyLength, ValueLength;
    PCHAR InputData, Key, SectionName, Value;
    PXTBL_CONFIG_SECTION Section;
    PXTBL_CONFIG_ENTRY Option;
    EFI_STATUS Status;

    /* Initialize pointers */
    InputData = RawConfig;
    Section = NULLPTR;
    Option = NULLPTR;
    SectionName = NULLPTR;
    Key = NULLPTR;
    Value = NULLPTR;

    /* Analyze configuration data until end of file is reached */
    while(*InputData != '\0')
    {
        if(*InputData == ';' || *InputData == '#')
        {
            /* Skip comment until end of the line */
            while(*InputData != '\0' && *InputData != '\n')
            {
                /* Advance to the next character */
                InputData++;
            }
        }
        else if(*InputData == ' ' || *InputData == '\t' || *InputData == '\r' || *InputData == '\n')
        {
            /* Skip whitespaces */
            InputData++;
        }
        else if(*InputData == '[')
        {
            /* Skip leading bracket */
            InputData++;

            /* Store section name */
            SectionName = InputData;

            /* Find end of the section name */
            while(*InputData != ']' && *InputData != '\0' && *InputData != '\n')
            {
                /* Advance to the next character */
                InputData++;
            }

            /* Check if end of the section name is reached */
            if(*InputData != ']')
            {
                /* Section name does not end */
                return STATUS_EFI_INVALID_PARAMETER;
            }

            /* Mark end of the section name and advance to the next character */
            *InputData = '\0';
            InputData++;

            /* Remove leading and trailing spaces from section name */
            SectionName = RtlTrimString(SectionName);

            /* Find length of the section name */
            SectionLength = RtlStringLength(SectionName, 0);

            /* Allocate memory for new section */
            Status = BlAllocateMemoryPool(sizeof(XTBL_CONFIG_SECTION), (PVOID*)&Section);
            if(Status == STATUS_EFI_SUCCESS)
            {
                /* Allocate more memory for section name */
                Status = BlAllocateMemoryPool(sizeof(WCHAR) * (SectionLength + 1), (PVOID*)&Section->SectionName);
            }
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Some memory allocation failed */
                return Status;
            }

            /* Initialize new section and convert its name to wide string */
            RtlInitializeListHead(&Section->Options);
            RtlStringToWideString(Section->SectionName, (PCSTR*)&SectionName, SectionLength);

            /* Ensure string is NULL-terminated and add new section to the configuration list */
            Section->SectionName[SectionLength] = L'\0';
            RtlInsertTailList(Configuration, &Section->Flink);
        }
        else
        {
            /* Store key */
            Key = InputData;

            /* Find end of the key */
            while(*InputData != '=' && *InputData != '\0' && *InputData != '\n')
            {
                /* Advance to the next character */
                InputData++;
            }

            /* Check if end of the key is reached */
            if(*InputData != '=')
            {
                /* Key name does not end */
                return STATUS_EFI_INVALID_PARAMETER;
            }

            /* Mark end of the key and advance to the next character */
            *InputData = 0;
            InputData++;

            /* Skip all leading spaces in the value */
            while(*InputData == ' ')
            {
                /* Advance to the next character */
                InputData++;
            }

            /* Store value */
            Value = InputData;

            /* Find end of the value */
            while(*InputData != '\0' && *InputData != '\n')
            {
                /* Advance to the next character */
                InputData++;
            }

            /* Mark end of the value and advance to the next character */
            *InputData = 0;
            InputData++;

            /* Remove leading and trailing spaces from key and value */
            Key = RtlTrimString(Key);
            Value = RtlTrimString(Value);

            /* Find length of the key and its value */
            KeyLength = RtlStringLength(Key, 0);
            ValueLength = RtlStringLength(Value, 0);

            /* Allocate memory for new option */
            Status = BlAllocateMemoryPool(sizeof(XTBL_CONFIG_ENTRY), (PVOID*)&Option);
            if(Status == STATUS_EFI_SUCCESS)
            {
                /* Allocate more memory for option name */
                Status = BlAllocateMemoryPool(sizeof(WCHAR) * (KeyLength + 1), (PVOID*)&Option->Name);
                if(Status == STATUS_EFI_SUCCESS)
                {
                    /* Allocate even more memory for option value */
                    Status = BlAllocateMemoryPool(sizeof(WCHAR) * (ValueLength + 1), (PVOID*)&Option->Value);
                }
            }
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Some memory allocation failed */
                return Status;
            }

            /* Remove leading quotes from the value */
            if(*Value == '"' || *Value == '\'')
            {
                Value++;
            }

            /* Remove trailing quotes from the value */
            if(Value[ValueLength - 2] == '"' || Value[ValueLength - 2] == '\'')
            {
                Value[ValueLength - 2] = '\0';
            }

            /* Convert key and value to wide strings */
            RtlStringToWideString(Option->Name, (PCSTR*)&Key, RtlStringLength(Key, 0) + 1);
            RtlStringToWideString(Option->Value, (PCSTR*)&Value, RtlStringLength(Value, 0) + 1);

            /* Ensure strings are NULL-terminated and add new option to the list */
            Option->Name[KeyLength] = L'\0';
            Option->Value[ValueLength] = L'\0';
            RtlInsertTailList(&Section->Options, &Option->Flink);
        }
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Loads configuration file from the specified directory on the FS0:/ drive.
 *
 * @param ConfigDirectory
 *        Specifies a path to the directory containing the configuration file.
 *
 * @param ConfigFile
 *        Specifies the name of the configuration file.
 *
 * @param ConfigData
 *        Provides a buffer to store the data read from the configuration file.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlpReadConfigFile(IN PCWSTR ConfigDirectory,
                  IN PCWSTR ConfigFile,
                  OUT PCHAR *ConfigData)
{
    PEFI_FILE_HANDLE DirHandle, FsHandle;
    EFI_HANDLE DiskHandle;
    EFI_STATUS Status;
    SIZE_T FileSize;

    /* Open EFI volume */
    Status = BlOpenVolume(NULLPTR, &DiskHandle, &FsHandle);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open a volume */
        return Status;
    }

    /* Open specified directory, containing the configuration file and close the FS immediately */
    Status = FsHandle->Open(FsHandle, &DirHandle, (PWCHAR)ConfigDirectory, EFI_FILE_MODE_READ, 0);
    FsHandle->Close(FsHandle);

    /* Check if directory opened successfully */
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open directory */
        BlCloseVolume(&DiskHandle);
        return Status;
    }

    /* Read configuration file and close directory */
    Status = BlReadFile(DirHandle, ConfigFile, (PVOID *)ConfigData, &FileSize);
    DirHandle->Close(DirHandle);

    /* Close EFI volume */
    BlCloseVolume(&DiskHandle);

    /* Return read status */
    return Status;
}

/**
 * Adds new XTLDR configuration entries to the global configuration list. Existing entries are not overwritten.
 *
 * @param NewConfig
 *        Supplies a pointer to a linked list containing new configuration entries.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlpUpdateConfiguration(IN PLIST_ENTRY NewConfig)
{
    PXTBL_CONFIG_ENTRY ConfigEntry;
    PWCHAR ConfigValue;
    PLIST_ENTRY ConfigListEntry, NextListEntry;

    /* Iterate through new config entries */
    ConfigListEntry = NewConfig->Flink;
    while(ConfigListEntry != NewConfig)
    {
        /* Get new config entry */
        ConfigEntry = CONTAIN_RECORD(ConfigListEntry, XTBL_CONFIG_ENTRY, Flink);

        /* Get next config entry */
        NextListEntry = ConfigListEntry->Flink;

        /* Make sure config entry does not exist yet */
        BlGetConfigValue(ConfigEntry->Name, &ConfigValue);
        if(ConfigValue == NULLPTR)
        {
            /* Remove new config entry from input list and put it into global config list */
            RtlRemoveEntryList(&ConfigEntry->Flink);
            RtlInsertTailList(&BlpConfig, &ConfigEntry->Flink);
        }

        /* Move to the next new config entry */
        ConfigListEntry = NextListEntry;
    }
}
