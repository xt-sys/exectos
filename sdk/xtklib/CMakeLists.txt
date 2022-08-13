# XT kernel-mode library
PROJECT(XTKLIB)

# Specify include directories
include_directories(
    ${EXECTOS_SOURCE_DIR}/sdk/xtdk
    ${XTKLIB_SOURCE_DIR}/includes)

# Specify list of source code files
list(APPEND XTKLIB_SOURCE
    ${XTKLIB_SOURCE_DIR}/hl/cport.c
    ${XTKLIB_SOURCE_DIR}/hl/${ARCH}/cpufunc.c
    ${XTKLIB_SOURCE_DIR}/rtl/plist.c
    ${XTKLIB_SOURCE_DIR}/rtl/widestr.c)

# Add library
add_library(xtklib ${XTKLIB_SOURCE})
