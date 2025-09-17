# XT Boot Loader
PROJECT(XTLDR)

# Build XTLDR modules
add_subdirectory(modules)

# Specify include directories
include_directories(
    ${EXECTOS_SOURCE_DIR}/sdk/xtdk
    ${XTLDR_SOURCE_DIR}/includes)

# Specify list of library source code files
list(APPEND LIBXTLDR_SOURCE
    ${XTLDR_SOURCE_DIR}/library/modproto.cc)

# Specify list of source code files
list(APPEND XTLDR_SOURCE
    ${XTLDR_SOURCE_DIR}/arch/${ARCH}/memory.cc
    ${XTLDR_SOURCE_DIR}/bootutil.cc
    ${XTLDR_SOURCE_DIR}/config.cc
    ${XTLDR_SOURCE_DIR}/console.cc
    ${XTLDR_SOURCE_DIR}/debug.cc
    ${XTLDR_SOURCE_DIR}/efiutils.cc
    ${XTLDR_SOURCE_DIR}/globals.cc
    ${XTLDR_SOURCE_DIR}/hardware.cc
    ${XTLDR_SOURCE_DIR}/memory.cc
    ${XTLDR_SOURCE_DIR}/protocol.cc
    ${XTLDR_SOURCE_DIR}/shell.cc
    ${XTLDR_SOURCE_DIR}/textui.cc
    ${XTLDR_SOURCE_DIR}/volume.cc
    ${XTLDR_SOURCE_DIR}/xtldr.cc)

# Link static XTLDR library
add_library(libxtldr ${LIBXTLDR_SOURCE})

# Link bootloader executable
add_executable(xtldr ${XTLDR_SOURCE})

# Add linker libraries
target_link_libraries(xtldr libxtos)

# Set proper binary name and install target
if(ARCH STREQUAL "i686")
	set(BINARY_NAME "bootia32")
elseif(ARCH STREQUAL "amd64")
	set(BINARY_NAME "bootx64")
endif()
set_target_properties(xtldr PROPERTIES OUTPUT_NAME ${BINARY_NAME} SUFFIX .efi)
set_install_target(xtldr efi/boot)

# Set loader entrypoint and subsystem
set_entrypoint(xtldr "BlStartXtLoader")
set_linker_map(xtldr TRUE)
set_subsystem(xtldr efi_application)
