# XT Boot Loader
PROJECT(XTLDR_FB_O)

# Specify include directories
include_directories(
    ${EXECTOS_SOURCE_DIR}/sdk/xtdk
    ${XTLDR_FB_O_SOURCE_DIR}/includes)

# Specify list of source code files
list(APPEND XTLDR_FB_O_SOURCE
    ${XTLDR_FB_O_SOURCE_DIR}/framebuf.c
    ${XTLDR_FB_O_SOURCE_DIR}/gop.c)

# Link bootloader executable
add_executable(fb_o ${XTLDR_FB_O_SOURCE})

# Add linker libraries
target_link_libraries(fb_o libxtldr)

# Set proper binary name and install target
set_target_properties(fb_o PROPERTIES SUFFIX .efi)
set_install_target(fb_o efi/boot/xtldr/modules)

# Set module entrypoint and subsystem
set_entrypoint(fb_o "XtLdrModuleMain")
set_linker_map(fb_o TRUE)
set_subsystem(fb_o efi_boot_service_driver)
