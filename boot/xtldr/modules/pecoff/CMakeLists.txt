# XTLDR PE/COFF image support module
PROJECT(XTLDR_PECOFF)

# Specify include directories
include_directories(
    ${EXECTOS_SOURCE_DIR}/sdk/xtdk
    ${XTLDR_PECOFF_SOURCE_DIR}/includes)

# Specify list of source code files
list(APPEND XTLDR_PECOFF_SOURCE
    ${XTLDR_PECOFF_SOURCE_DIR}/data.cc
    ${XTLDR_PECOFF_SOURCE_DIR}/pecoff.cc)

# Link module executable
add_executable(pecoff ${XTLDR_PECOFF_SOURCE})

# Add linker libraries
target_link_libraries(pecoff libxtldr libxtos)

# Set proper binary name and install target
set_target_properties(pecoff PROPERTIES SUFFIX .efi)
set_install_target(pecoff efi/boot/xtldr/modules)

# Set module entrypoint and subsystem
set_entrypoint(pecoff "XtLdrModuleMain")
set_linker_map(pecoff TRUE)
set_subsystem(pecoff efi_boot_service_driver)
