# XT Kernel
PROJECT(XTOSKRNL)

# Specify include directories
include_directories(
    ${EXECTOS_SOURCE_DIR}/sdk/xtdk)

# Specify list of source code files
list(APPEND XTOSKRNL_SOURCE
    ${XTOSKRNL_SOURCE_DIR}/hl/cport.c
    ${XTOSKRNL_SOURCE_DIR}/hl/${ARCH}/cpufunc.c
    ${XTOSKRNL_SOURCE_DIR}/ke/krnlinit.c
    ${XTOSKRNL_SOURCE_DIR}/rtl/memory.c
    ${XTOSKRNL_SOURCE_DIR}/rtl/plist.c
    ${XTOSKRNL_SOURCE_DIR}/rtl/widestr.c)

# Set module definition SPEC file
set_specfile(xtoskrnl.spec)

# Link static XTOS library
add_library(xtoslib ${XTOSKRNL_SOURCE})

# Link kernel executable
add_executable(xtoskrnl
               ${XTOSKRNL_SOURCE}
               ${CMAKE_CURRENT_BINARY_DIR}/xtoskrnl.def)

# Add linker libraries
target_link_libraries(xtoskrnl)

# Set instal target
set_install_target(xtoskrnl "exectos/system")

# Set kernel entrypoint, imagebase address, ordinals and subsystem
set_entrypoint(xtoskrnl "KeStartXtSystem")
set_imagebase(xtoskrnl ${BASEADDRESS_XTOSKRNL})
set_ordinals(xtoskrnl TRUE)
set_subsystem(xtoskrnl native)
