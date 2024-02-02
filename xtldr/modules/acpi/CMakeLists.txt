# XT Boot Loader ACPI Support Module
PROJECT(XTLDR_ACPI)

# Specify include directories
include_directories(
    ${EXECTOS_SOURCE_DIR}/sdk/xtdk
    ${XTLDR_ACPI_SOURCE_DIR}/includes)

# Specify list of source code files
list(APPEND XTLDR_ACPI_SOURCE
    ${XTLDR_ACPI_SOURCE_DIR}/acpi.c
    ${XTLDR_ACPI_SOURCE_DIR}/globals.c)

# Link module executable
add_executable(acpi ${XTLDR_ACPI_SOURCE})

# Add linker libraries
target_link_libraries(acpi libxtldr libxtos)

# Set proper binary name and install target
set_target_properties(acpi PROPERTIES SUFFIX .efi)
set_install_target(acpi efi/boot/xtldr/modules)

# Set module entrypoint and subsystem
set_entrypoint(acpi "XtLdrModuleMain")
set_linker_map(acpi TRUE)
set_subsystem(acpi efi_boot_service_driver)
