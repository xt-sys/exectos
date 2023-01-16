# XT Boot Loader
PROJECT(XTLDR_FRAMEBUF)

# Specify include directories
include_directories(
    ${EXECTOS_SOURCE_DIR}/sdk/xtdk
    ${XTLDR_SOURCE_DIR}/includes
    ${XTLDR_FRAMEBUF_SOURCE_DIR}/includes)

# Specify list of source code files
list(APPEND XTLDR_FRAMEBUF_SOURCE
    ${XTLDR_SOURCE_DIR}/blproto.c
    ${XTLDR_FRAMEBUF_SOURCE_DIR}/framebuf.c
    ${XTLDR_FRAMEBUF_SOURCE_DIR}/gop.c)

# Link bootloader executable
add_executable(framebuf ${XTLDR_FRAMEBUF_SOURCE})

# Add linker libraries
target_link_libraries(framebuf libxtos)

# Set proper binary name and install target
set_target_properties(framebuf PROPERTIES SUFFIX .efi)
set_install_target(framebuf efi/boot/xtldr)

# Set module entrypoint and subsystem
set_entrypoint(framebuf "BlXtLdrModuleMain")
set_subsystem(framebuf efi_boot_service_driver)
