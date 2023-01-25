# XT Boot Loader
PROJECT(XTLDR_XTOS)

# Specify include directories
include_directories(
    ${EXECTOS_SOURCE_DIR}/sdk/xtdk
    ${XTLDR_SOURCE_DIR}/includes
    ${XTLDR_XTOS_SOURCE_DIR}/includes)

# Specify list of source code files
list(APPEND XTLDR_XTOS_SOURCE
    ${XTLDR_SOURCE_DIR}/blproto.c
    ${XTLDR_XTOS_SOURCE_DIR}/xtos.c
    ${XTLDR_XTOS_SOURCE_DIR}/${ARCH}/cpu.c)

# Link bootloader executable
add_executable(xtos ${XTLDR_XTOS_SOURCE})

# Add linker libraries
target_link_libraries(xtos libxtos)

# Set proper binary name and install target
set_target_properties(xtos PROPERTIES SUFFIX .efi)
set_install_target(xtos efi/boot/xtldr)

# Set module entrypoint and subsystem
set_entrypoint(xtos "BlXtLdrModuleMain")
set_subsystem(xtos efi_boot_service_driver)
