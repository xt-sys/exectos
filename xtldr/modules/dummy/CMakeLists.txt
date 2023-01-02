# XT Boot Loader
PROJECT(XTLDR_DUMMY)

# Specify include directories
include_directories(
    ${EXECTOS_SOURCE_DIR}/sdk/xtdk
    ${XTLDR_SOURCE_DIR}/includes
    ${XTLDR_DUMMY_SOURCE_DIR}/includes)

# Specify list of source code files
list(APPEND XTLDR_DUMMY_SOURCE
    ${XTLDR_SOURCE_DIR}/blproto.c
    ${XTLDR_DUMMY_SOURCE_DIR}/dummy.c)

# Link bootloader executable
add_executable(dummy ${XTLDR_DUMMY_SOURCE})

# Add linker libraries
target_link_libraries(dummy libxtos)

# Set proper binary name and install target
set_target_properties(dummy PROPERTIES SUFFIX .efi)
set_install_target(dummy efi/boot/xtldr)

# Set module entrypoint and subsystem
set_entrypoint(dummy "BlXtLdrModuleMain")
set_subsystem(dummy efi_boot_service_driver)
