# XT Kernel and library
PROJECT(LIBXTOS)
PROJECT(XTOSKRNL)

# Specify include directories
include_directories(
    ${EXECTOS_SOURCE_DIR}/sdk/xtdk
    ${XTOSKRNL_SOURCE_DIR}/includes)

# Specify list of kernel source code files
list(APPEND XTOSKRNL_SOURCE
    ${XTOSKRNL_SOURCE_DIR}/ar/${ARCH}/archsup.S
    ${XTOSKRNL_SOURCE_DIR}/ar/${ARCH}/boot.S
    ${XTOSKRNL_SOURCE_DIR}/ar/${ARCH}/cpufunc.cc
    ${XTOSKRNL_SOURCE_DIR}/ar/${ARCH}/data.cc
    ${XTOSKRNL_SOURCE_DIR}/ar/${ARCH}/procsup.cc
    ${XTOSKRNL_SOURCE_DIR}/ar/${ARCH}/traps.cc
    ${XTOSKRNL_SOURCE_DIR}/ex/exports.cc
    ${XTOSKRNL_SOURCE_DIR}/ex/rundown.cc
    ${XTOSKRNL_SOURCE_DIR}/hl/${ARCH}/cpu.cc
    ${XTOSKRNL_SOURCE_DIR}/hl/${ARCH}/pic.cc
    ${XTOSKRNL_SOURCE_DIR}/hl/${ARCH}/ioport.cc
    ${XTOSKRNL_SOURCE_DIR}/hl/${ARCH}/runlevel.cc
    ${XTOSKRNL_SOURCE_DIR}/hl/acpi.cc
    ${XTOSKRNL_SOURCE_DIR}/hl/cport.cc
    ${XTOSKRNL_SOURCE_DIR}/hl/data.cc
    ${XTOSKRNL_SOURCE_DIR}/hl/exports.cc
    ${XTOSKRNL_SOURCE_DIR}/hl/fbdev.cc
    ${XTOSKRNL_SOURCE_DIR}/hl/init.cc
    ${XTOSKRNL_SOURCE_DIR}/hl/ioreg.cc
    ${XTOSKRNL_SOURCE_DIR}/kd/data.cc
    ${XTOSKRNL_SOURCE_DIR}/kd/dbgio.cc
    ${XTOSKRNL_SOURCE_DIR}/kd/exports.cc
    ${XTOSKRNL_SOURCE_DIR}/ke/${ARCH}/irq.cc
    ${XTOSKRNL_SOURCE_DIR}/ke/${ARCH}/krnlinit.cc
    ${XTOSKRNL_SOURCE_DIR}/ke/${ARCH}/kthread.cc
    ${XTOSKRNL_SOURCE_DIR}/ke/${ARCH}/proc.cc
    ${XTOSKRNL_SOURCE_DIR}/ke/apc.cc
    ${XTOSKRNL_SOURCE_DIR}/ke/bootinfo.cc
    ${XTOSKRNL_SOURCE_DIR}/ke/crash.cc
    ${XTOSKRNL_SOURCE_DIR}/ke/data.cc
    ${XTOSKRNL_SOURCE_DIR}/ke/dpc.cc
    ${XTOSKRNL_SOURCE_DIR}/ke/event.cc
    ${XTOSKRNL_SOURCE_DIR}/ke/exports.cc
    ${XTOSKRNL_SOURCE_DIR}/ke/kprocess.cc
    ${XTOSKRNL_SOURCE_DIR}/ke/krnlinit.cc
    ${XTOSKRNL_SOURCE_DIR}/ke/kthread.cc
    ${XTOSKRNL_SOURCE_DIR}/ke/kubsan.cc
    ${XTOSKRNL_SOURCE_DIR}/ke/runlevel.cc
    ${XTOSKRNL_SOURCE_DIR}/ke/semphore.cc
    ${XTOSKRNL_SOURCE_DIR}/ke/spinlock.cc
    ${XTOSKRNL_SOURCE_DIR}/ke/sysres.cc
    ${XTOSKRNL_SOURCE_DIR}/ke/timer.cc
    ${XTOSKRNL_SOURCE_DIR}/mm/${ARCH}/mmgr.cc
    ${XTOSKRNL_SOURCE_DIR}/mm/${ARCH}/pagemap.cc
    ${XTOSKRNL_SOURCE_DIR}/mm/${ARCH}/paging.cc
    ${XTOSKRNL_SOURCE_DIR}/mm/${ARCH}/pte.cc
    ${XTOSKRNL_SOURCE_DIR}/mm/colors.cc
    ${XTOSKRNL_SOURCE_DIR}/mm/data.cc
    ${XTOSKRNL_SOURCE_DIR}/mm/hlpool.cc
    ${XTOSKRNL_SOURCE_DIR}/mm/kpool.cc
    ${XTOSKRNL_SOURCE_DIR}/mm/mmgr.cc
    ${XTOSKRNL_SOURCE_DIR}/mm/paging.cc
    ${XTOSKRNL_SOURCE_DIR}/mm/pfn.cc
    ${XTOSKRNL_SOURCE_DIR}/mm/pte.cc
    ${XTOSKRNL_SOURCE_DIR}/po/idle.cc
    ${XTOSKRNL_SOURCE_DIR}/rtl/${ARCH}/dispatch.cc
    ${XTOSKRNL_SOURCE_DIR}/rtl/${ARCH}/exsup.cc
    ${XTOSKRNL_SOURCE_DIR}/rtl/atomic.cc
    ${XTOSKRNL_SOURCE_DIR}/rtl/bitmap.cc
    ${XTOSKRNL_SOURCE_DIR}/rtl/data.cc
    ${XTOSKRNL_SOURCE_DIR}/rtl/endian.cc
    ${XTOSKRNL_SOURCE_DIR}/rtl/exports.cc
    ${XTOSKRNL_SOURCE_DIR}/rtl/guid.cc
    ${XTOSKRNL_SOURCE_DIR}/rtl/llist.cc
    ${XTOSKRNL_SOURCE_DIR}/rtl/math.cc
    ${XTOSKRNL_SOURCE_DIR}/rtl/memory.cc
    ${XTOSKRNL_SOURCE_DIR}/rtl/string.cc
    ${XTOSKRNL_SOURCE_DIR}/rtl/widestr.cc)

# Set module definition SPEC file
set_specfile(xtoskrnl.spec xtoskrnl.exe)

# Link static XTOS library
add_library(libxtos ${XTOSKRNL_SOURCE})

# Link kernel executable
add_executable(xtoskrnl
               ${CMAKE_CURRENT_BINARY_DIR}/xtoskrnl.def)

# Add linker libraries
target_link_libraries(xtoskrnl
                      PRIVATE
                      libxtos)

# Set proper binary name and install target
set_target_properties(xtoskrnl PROPERTIES SUFFIX .exe)
set_install_target(xtoskrnl "exectos/boot")

# Set kernel entrypoint, imagebase address, ordinals and subsystem
set_entrypoint(xtoskrnl "KeStartXtSystem")
set_imagebase(xtoskrnl ${BASEADDRESS_XTOSKRNL})
set_linker_map(xtoskrnl TRUE)
set_ordinals(xtoskrnl TRUE)
set_subsystem(xtoskrnl native xt_native_kernel)
