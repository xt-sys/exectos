# Set target operating system name
set(CMAKE_SYSTEM_NAME Windows)

# Set toolchain compilers
set(CMAKE_ASM_COMPILER clang-cl)
set(CMAKE_ASM_LINKER ld.lld)
set(CMAKE_C_COMPILER clang-cl)
set(CMAKE_CXX_COMPILER clang-cl)
set(CMAKE_LINKER lld-link)
set(CMAKE_MC_COMPILER wmc)
set(CMAKE_RC_COMPILER wrc)
set(CMAKE_SPEC_COMPILER xtcspecc)

# Assume that ASM and C/C++ compilers are working
set(CMAKE_ASM_COMPILER_WORKS 1)
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

# Set C/C++ standard and disable extensions
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

# Disable standard C and C++ libraries
set(CMAKE_C_STANDARD_LIBRARIES "" CACHE INTERNAL "")
set(CMAKE_CXX_STANDARD_LIBRARIES "" CACHE INTERNAL "")

# Clean linker flags
set(CMAKE_STATIC_LINKER_FLAGS "")
set(CMAKE_MODULE_LINKER_FLAGS "")
set(CMAKE_EXE_LINKER_FLAGS "")
set(CMAKE_SHARED_LINKER_FLAGS "")
