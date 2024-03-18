/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/config.c
 * DESCRIPTION:     XT Boot Loader Configuration
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtldr.h>


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
BlGetConfigBooleanValue(IN CONST PWCHAR ConfigName)
{
    PWCHAR Value;

    /* Get config value */
    Value = BlGetConfigValue(ConfigName);

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
 * @return This routine returns a pointer to the configuration value, or NULL if key was not found.
 *
 * @since XT 1.0
 */
XTCDECL
PWCHAR
BlGetConfigValue(IN CONST PWCHAR ConfigName)
{
    PXTBL_CONFIG_ENTRY ConfigEntry;
    PLIST_ENTRY ConfigListEntry;
    SIZE_T KeyLength, ValueLength;
    EFI_STATUS Status;
    PWCHAR Value;

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
                /* Memory allocation failure, return NULL */
                BlDebugPrint(L"ERROR: Memory allocation failure (Status Code: 0x%zX)\n", Status);
                return NULL;
            }

            /* Copy value and return it */
            RtlCopyMemory(Value, ConfigEntry->Value, ValueLength * sizeof(WCHAR));
            Value[ValueLength] = L'\0';
            return Value;
        }

        /* Move to the next config entry */
        ConfigListEntry = ConfigListEntry->Flink;
    }

    /* Config entry not found, return NULL */
    return NULL;
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
 * @return This routine returns status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlSetConfigValue(IN CONST PWCHAR ConfigName,
                 IN CONST PWCHAR ConfigValue)
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
 * @return This routine returns status code.
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
            Argument = RtlTokenizeWideString(LoadedImage->LoadOptions, L" ", &LastArg);

            /* Iterate over all arguments passed to boot loader */
            while(Argument != NULL)
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
                Argument = RtlTokenizeWideString(NULL, L" ", &LastArg);
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
    Section = NULL;
    Option = NULL;
    SectionName = NULL;
    Key = NULL;
    Value = NULL;

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
            RtlStringToWideString(Section->SectionName, &SectionName, SectionLength);

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
            RtlStringToWideString(Option->Name, &Key, RtlStringLength(Key, 0) + 1);
            RtlStringToWideString(Option->Value, &Value, RtlStringLength(Value, 0) + 1);

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
 * @return This routine returns status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
BlpReadConfigFile(IN CONST PWCHAR ConfigDirectory,
                  IN CONST PWCHAR ConfigFile,
                  OUT PCHAR *ConfigData)
{
    PEFI_FILE_HANDLE DirHandle, FsHandle;
    EFI_HANDLE DiskHandle;
    EFI_STATUS Status;
    SIZE_T FileSize;

    /* Open EFI volume */
    Status = BlOpenVolume(NULL, &DiskHandle, &FsHandle);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open a volume */
        return Status;
    }

    /* Open specified directory, containing the configuration file and close the FS immediately */
    Status = FsHandle->Open(FsHandle, &DirHandle, ConfigDirectory, EFI_FILE_MODE_READ, 0);
    FsHandle->Close(FsHandle);

    /* Check if directory opened successfully */
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open directory */
        BlCloseVolume(DiskHandle);
        return Status;
    }

    /* Read configuration file and close directory */
    Status = BlReadFile(DirHandle, ConfigFile, (PVOID *)ConfigData, &FileSize);
    DirHandle->Close(DirHandle);

    /* Close EFI volume */
    BlCloseVolume(DiskHandle);

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
        if(BlGetConfigValue(ConfigEntry->Name) == NULL)
        {
            /* Remove new config entry from input list and put it into global config list */
            RtlRemoveEntryList(&ConfigEntry->Flink);
            RtlInsertTailList(&BlpConfig, &ConfigEntry->Flink);
        }

        /* Move to the next new config entry */
        ConfigListEntry = NextListEntry;
    }
}
