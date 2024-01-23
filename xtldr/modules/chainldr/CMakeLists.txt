# XTLDR Chain Loader Module
PROJECT(XTLDR_CHAINLDR)

# Specify include directories
include_directories(
    ${EXECTOS_SOURCE_DIR}/sdk/xtdk
    ${XTLDR_CHAINLDR_SOURCE_DIR}/includes)

# Specify list of source code files
list(APPEND XTLDR_CHAINLDR_SOURCE
    ${XTLDR_CHAINLDR_SOURCE_DIR}/chainldr.c
    ${XTLDR_CHAINLDR_SOURCE_DIR}/globals.c)

# Link module executable
add_executable(chainldr ${XTLDR_CHAINLDR_SOURCE})

# Add linker libraries
target_link_libraries(chainldr libxtldr libxtos)

# Set proper binary name and install target
set_target_properties(chainldr PROPERTIES SUFFIX .efi)
set_install_target(chainldr efi/boot/xtldr/modules)

# Set module entrypoint and subsystem
set_entrypoint(chainldr "XtLdrModuleMain")
set_linker_map(chainldr TRUE)
set_subsystem(chainldr efi_boot_service_driver)
