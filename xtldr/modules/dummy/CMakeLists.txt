# XT Boot Loader Dummy Module
PROJECT(XTLDR_DUMMY)

# Specify include directories
include_directories(
    ${EXECTOS_SOURCE_DIR}/sdk/xtdk
    ${XTLDR_DUMMY_SOURCE_DIR}/includes)

# Specify list of source code files
list(APPEND XTLDR_DUMMY_SOURCE
    ${XTLDR_DUMMY_SOURCE_DIR}/dummy.cc
    ${XTLDR_DUMMY_SOURCE_DIR}/data.cc)

# Link module executable
add_executable(dummy ${XTLDR_DUMMY_SOURCE})

# Add linker libraries
target_link_libraries(dummy libxtldr)

# Set proper binary name and install target
set_target_properties(dummy PROPERTIES SUFFIX .efi)
set_install_target(dummy efi/boot/xtldr/modules)

# Set module entrypoint and subsystem
set_entrypoint(dummy "XtLdrModuleMain")
set_linker_map(dummy TRUE)
set_subsystem(dummy efi_boot_service_driver)
