# XT Boot Loader
PROJECT(XTLDR_XTOS_O)

# Specify include directories
include_directories(
    ${EXECTOS_SOURCE_DIR}/sdk/xtdk
    ${XTLDR_XTOS_O_SOURCE_DIR}/includes)

# Specify list of source code files
list(APPEND XTLDR_XTOS_O_SOURCE
    ${XTLDR_XTOS_O_SOURCE_DIR}/${ARCH}/memory.cc
    ${XTLDR_XTOS_O_SOURCE_DIR}/data.cc
    ${XTLDR_XTOS_O_SOURCE_DIR}/xtos.cc)

# Link bootloader executable
add_executable(xtos_o ${XTLDR_XTOS_O_SOURCE})

# Add linker libraries
target_link_libraries(xtos_o libxtos libxtldr)

# Set proper binary name and install target
set_target_properties(xtos_o PROPERTIES SUFFIX .efi)
set_install_target(xtos_o efi/boot/xtldr/modules)

# Set module entrypoint and subsystem
set_entrypoint(xtos_o "XtLdrModuleMain")
set_linker_map(xtos_o TRUE)
set_subsystem(xtos_o efi_boot_service_driver)
