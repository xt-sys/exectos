# XT Boot Loader
PROJECT(XTLDR_PECOFF_O)

# Specify include directories
include_directories(
    ${EXECTOS_SOURCE_DIR}/sdk/xtdk
    ${XTLDR_PECOFF_O_SOURCE_DIR}/includes)

# Specify list of source code files
list(APPEND XTLDR_PECOFF_O_SOURCE
    ${XTLDR_PECOFF_O_SOURCE_DIR}/pecoff.c)

# Link bootloader executable
add_executable(pecoff_o ${XTLDR_PECOFF_O_SOURCE})

# Add linker libraries
target_link_libraries(pecoff_o libxtldr)

# Set proper binary name and install target
set_target_properties(pecoff_o PROPERTIES SUFFIX .efi)
set_install_target(pecoff_o efi/boot/xtldr/modules)

# Set module entrypoint and subsystem
set_entrypoint(pecoff_o "XtLdrModuleMain")
set_linker_map(pecoff_o TRUE)
set_subsystem(pecoff_o efi_boot_service_driver)
