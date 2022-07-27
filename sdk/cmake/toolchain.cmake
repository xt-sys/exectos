# Set target operating system name
set(CMAKE_SYSTEM_NAME Windows)

# Set toolchain compilers
set(CMAKE_ASM_COMPILER nasm)
set(CMAKE_C_COMPILER clang-cl)
set(CMAKE_CXX_COMPILER clang-cl)
set(CMAKE_MC_COMPILER wmc)
set(CMAKE_RC_COMPILER wrc)
set(CMAKE_SPEC_COMPILER xtcspecc)

# Assume that C/C++ compiler is working
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

# Disable standard C libraries
set(CMAKE_C_STANDARD_LIBRARIES "" CACHE INTERNAL "")
