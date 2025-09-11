# Minimum CMake version requirement
cmake_minimum_required(VERSION 3.19.0)

# Lowercase target architecture
string(TOLOWER ${ARCH} ARCH)

# Validate and set architectures specific definitions
if(ARCH STREQUAL "i686")
    add_definitions(-D__i386__ -D__i686__)
elseif(ARCH STREQUAL "amd64")
    add_definitions(-D__amd64__ -D__x86_64__)
else()
    message(FATAL_ERROR "Unknown target architecture (${ARCH}) set!")
endif()

# Print target architecture
message("-- Target architecture: ${ARCH}")

# Set the build type
if(NOT BUILD_TYPE)
    set(BUILD_TYPE DEBUG)
endif()
string(TOUPPER ${BUILD_TYPE} BUILD_TYPE)

# Set build type specific definitions
if(BUILD_TYPE STREQUAL "DEBUG")
    add_definitions(-DDBG=1)
    set(CMAKE_BUILD_TYPE DEBUG)
else()
    set(BUILD_TYPE RELEASE)
    set(CMAKE_BUILD_TYPE NONE)
endif()

# Print build type
message("-- Target build type: ${BUILD_TYPE}")

# Set toolchain file
set(CMAKE_TOOLCHAIN_FILE "sdk/cmake/toolchain.cmake")

# Set project name
project(EXECTOS)

# Load all the CMake SDK
include(sdk/cmake/baseaddress/${ARCH}.cmake)
include(sdk/cmake/emulation.cmake)
include(sdk/cmake/functions.cmake)
include(sdk/cmake/version.cmake)
include(sdk/cmake/xtchain.cmake)

# Enable compilers
enable_language(ASM C CXX)

# Add project specific definitions
add_definitions(-D__XTOS__)
add_definitions(-DXTOS_SOURCE_DIR="${EXECTOS_SOURCE_DIR}")
add_definitions(-DXTOS_BINARY_DIR="${EXECTOS_BINARY_DIR}")

# Compute __FILE__ definition
file(RELATIVE_PATH _PATH_PREFIX ${EXECTOS_BINARY_DIR} ${EXECTOS_SOURCE_DIR})
add_compiler_flags(-D__RELFILE__="&__FILE__[__FILE__[0] == '.' ? sizeof \\\"${_PATH_PREFIX}\\\" - 1 : sizeof XTOS_SOURCE_DIR]")

# Set the virtual disk image size (in MiB)
set_disk_image_size(32)

# Build all subprojects
add_subdirectory(bootdata)
add_subdirectory(drivers)
add_subdirectory(xtldr)
add_subdirectory(xtoskrnl)
