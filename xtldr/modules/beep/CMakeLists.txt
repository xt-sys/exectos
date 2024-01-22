# XT Boot Loader Beep Module
PROJECT(XTLDR_BEEP)

# Specify include directories
include_directories(
    ${EXECTOS_SOURCE_DIR}/sdk/xtdk
    ${XTLDR_BEEP_SOURCE_DIR}/includes)

# Specify list of source code files
list(APPEND XTLDR_BEEP_SOURCE
    ${XTLDR_BEEP_SOURCE_DIR}/beep.c
    ${XTLDR_BEEP_SOURCE_DIR}/globals.c)

# Link module executable
add_executable(beep ${XTLDR_BEEP_SOURCE})

# Add linker libraries
target_link_libraries(beep libxtldr libxtos)

# Set proper binary name and install target
set_target_properties(beep PROPERTIES SUFFIX .efi)
set_install_target(beep efi/boot/xtldr/modules)

# Set module entrypoint and subsystem
set_entrypoint(beep "XtLdrModuleMain")
set_linker_map(beep TRUE)
set_subsystem(beep efi_boot_service_driver)
