# XT Kernel
PROJECT(XTOSKRNL)

# Specify include directories
include_directories(
    ${EXECTOS_SOURCE_DIR}/sdk/xtdk
    ${XTOSKRNL_SOURCE_DIR}/includes)

# Specify list of source code files
list(APPEND XTOSKRNL_SOURCE
    ${XTOSKRNL_SOURCE_DIR}/ar/${ARCH}/cpufunc.c
    ${XTOSKRNL_SOURCE_DIR}/ar/${ARCH}/globals.c
    ${XTOSKRNL_SOURCE_DIR}/ar/${ARCH}/procsup.c
    ${XTOSKRNL_SOURCE_DIR}/ar/${ARCH}/traps.c
    ${XTOSKRNL_SOURCE_DIR}/hl/cport.c
    ${XTOSKRNL_SOURCE_DIR}/hl/efifb.c
    ${XTOSKRNL_SOURCE_DIR}/hl/globals.c
    ${XTOSKRNL_SOURCE_DIR}/hl/${ARCH}/ioport.c
    ${XTOSKRNL_SOURCE_DIR}/ke/apc.c
    ${XTOSKRNL_SOURCE_DIR}/ke/dpc.c
    ${XTOSKRNL_SOURCE_DIR}/ke/globals.c
    ${XTOSKRNL_SOURCE_DIR}/ke/kprocess.c
    ${XTOSKRNL_SOURCE_DIR}/ke/krnlinit.c
    ${XTOSKRNL_SOURCE_DIR}/ke/kthread.c
    ${XTOSKRNL_SOURCE_DIR}/ke/semphore.c
    ${XTOSKRNL_SOURCE_DIR}/ke/spinlock.c
    ${XTOSKRNL_SOURCE_DIR}/ke/timer.c
    ${XTOSKRNL_SOURCE_DIR}/ke/${ARCH}/krnlinit.c
    ${XTOSKRNL_SOURCE_DIR}/ke/${ARCH}/kthread.c
    ${XTOSKRNL_SOURCE_DIR}/ke/${ARCH}/proc.c
    ${XTOSKRNL_SOURCE_DIR}/mm/kpools.c
    ${XTOSKRNL_SOURCE_DIR}/mm/${ARCH}/pages.c
    ${XTOSKRNL_SOURCE_DIR}/po/idle.c
    ${XTOSKRNL_SOURCE_DIR}/rtl/atomic.c
    ${XTOSKRNL_SOURCE_DIR}/rtl/byteswap.c
    ${XTOSKRNL_SOURCE_DIR}/rtl/memory.c
    ${XTOSKRNL_SOURCE_DIR}/rtl/plist.c
    ${XTOSKRNL_SOURCE_DIR}/rtl/string.c
    ${XTOSKRNL_SOURCE_DIR}/rtl/widestr.c
    ${XTOSKRNL_SOURCE_DIR}/rtl/${ARCH}/dispatch.c
    ${XTOSKRNL_SOURCE_DIR}/rtl/${ARCH}/exsup.c)

# Set module definition SPEC file
set_specfile(xtoskrnl.spec)

# Link static XTOS library
add_library(libxtos ${XTOSKRNL_SOURCE})

# Link kernel executable
add_executable(xtoskrnl
               ${XTOSKRNL_SOURCE}
               ${CMAKE_CURRENT_BINARY_DIR}/xtoskrnl.def)

# Add linker libraries
target_link_libraries(xtoskrnl)

# Set instal target
set_install_target(xtoskrnl "exectos/boot")

# Set kernel entrypoint, imagebase address, ordinals and subsystem
set_entrypoint(xtoskrnl "KeStartXtSystem")
set_imagebase(xtoskrnl ${BASEADDRESS_XTOSKRNL})
set_ordinals(xtoskrnl TRUE)
set_subsystem(xtoskrnl native xt_native_kernel)
