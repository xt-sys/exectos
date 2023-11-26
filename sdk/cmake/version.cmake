# Set XTOS version
set(XTOS_VERSION_MAJOR "0")
set(XTOS_VERSION_MINOR "1")
set(XTOS_VERSION_BUILD "devel")

# Set XTOS NT compatibility version
set(XTOS_COMPATIBLE_MAJOR "6")
set(XTOS_COMPATIBLE_MINOR "3")
set(XTOS_COMPATIBLE_VERSION "0x0603")
set(XTOS_COMPATIBLE_BUILD "9600")

# Set common XTOS version variables
string(TIMESTAMP XTOS_VERSION_YEAR %Y)
string(TIMESTAMP XTOS_VERSION_DATE "%Y%m%d")
string(TIMESTAMP XTOS_VERSION_FULLDATE "%d/%m/%Y %H:%M UTC" UTC)

# Set latest GIT revision
set(XTOS_VERSION_HASH "unknown")
if(EXISTS "${EXECTOS_SOURCE_DIR}/.git")
        execute_process(COMMAND git describe --abbrev=10 --long --always
        WORKING_DIRECTORY ${EXECTOS_SOURCE_DIR}
        OUTPUT_VARIABLE XTOS_VERSION_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

# Prepare xtver.h header file
add_custom_target(xtver ALL COMMAND ${CMAKE_COMMAND} -E touch ${EXECTOS_SOURCE_DIR}/sdk/cmake/version/xtver.h.cmake)
configure_file(sdk/cmake/version/xtver.h.cmake ${EXECTOS_BINARY_DIR}/sdk/includes/xtver.h)
include_directories(${EXECTOS_BINARY_DIR}/sdk/includes)
