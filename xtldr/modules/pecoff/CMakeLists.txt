# XT Boot Loader
PROJECT(XTLDR_PECOFF)

# Specify include directories
include_directories(
    ${EXECTOS_SOURCE_DIR}/sdk/xtdk
    ${XTLDR_SOURCE_DIR}/includes)

# Specify list of source code files
list(APPEND XTLDR_PECOFF_SOURCE
    ${XTLDR_SOURCE_DIR}/blproto.c
    ${XTLDR_PECOFF_SOURCE_DIR}/pecoff.c)

# Link bootloader executable
add_executable(pecoff ${XTLDR_PECOFF_SOURCE})

# Add linker libraries
target_link_libraries(pecoff libxtos)

# Set proper binary name and install target
set_target_properties(pecoff PROPERTIES SUFFIX .efi)
set_install_target(pecoff efi/boot/xtldr)

# Set loader entrypoint, imagebase address, ordinals and subsystem
set_entrypoint(pecoff "BlXtLdrModuleMain")
set_subsystem(pecoff efi_boot_service_driver)
