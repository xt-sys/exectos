# XTLDR FrameBuffer support module
PROJECT(XTLDR_FRAMEBUF)

# Specify include directories
include_directories(
    ${EXECTOS_SOURCE_DIR}/sdk/xtdk
    ${XTLDR_FRAMEBUF_SOURCE_DIR}/includes)

# Specify list of source code files
list(APPEND XTLDR_FRAMEBUF_SOURCE
    ${XTLDR_FRAMEBUF_SOURCE_DIR}/framebuf.cc
    ${XTLDR_FRAMEBUF_SOURCE_DIR}/data.cc)

# Link bootloader executable
add_executable(framebuf ${XTLDR_FRAMEBUF_SOURCE})

# Add linker libraries
target_link_libraries(framebuf libxtldr)

# Set proper binary name and install target
set_target_properties(framebuf PROPERTIES SUFFIX .efi)
set_install_target(framebuf efi/boot/xtldr/modules)

# Set module entrypoint and subsystem
set_entrypoint(framebuf "XtLdrModuleMain")
set_linker_map(framebuf TRUE)
set_subsystem(framebuf efi_boot_service_driver)
