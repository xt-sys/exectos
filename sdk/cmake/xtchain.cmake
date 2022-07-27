# Architecture specific flags
if(ARCH STREQUAL i686)
    add_compiler_flags("-m32 --target=i686-w64-windows-msvc")
    add_linker_flags("/machine:X86")
    set(HOTPATCH_LINKER_FLAG "/FUNCTIONPADMIN:5")
elseif(ARCH STREQUAL amd64)
    add_compiler_flags("-m64 --target=x86_64-w64-windows-msvc")
    add_linker_flags("/machine:X64")
    set(HOTPATCH_LINKER_FLAG "/FUNCTIONPADMIN:6")
endif()

# Set build optimisation
if(BUILD_TYPE STREQUAL "DEBUG")
    add_compiler_flags("/Zi")
    add_compiler_ccxxflags("-Ob0 -Od")
    add_linker_flags("/DEBUG /INCREMENTAL /OPT:NOREF /OPT:NOICF")
else()
    add_compiler_ccxxflags("-Ob2 -Oy")
    add_linker_flags("/INCREMENTAL:NO /OPT:REF /OPT:ICF")
endif()

# Enable string pooling
add_compiler_ccxxflags("-GF")

# Disable builtin CRT library
add_compiler_ccxxflags("-Zl")

# Disable RTTI and buffer security checks
add_compiler_ccxxflags("-GR- -GS-")

# Disable thread-safe initialization
add_compiler_flags("-Zc:threadSafeInit-")

# Enable function level linking
add_compiler_ccxxflags("-Gy")

# Enable Structured Exception Handling (SEH)
add_compiler_ccxxflags("-EHa")

# Control warnings levels
add_compiler_flags("-W3 -w14115")
add_compiler_flags("-wd4200 -wd4214 -wd4244 -wd4290 -wd4800")
add_compiler_flags("-we4013 -we4020 -we4022 -we4028 -we4047 -we4098 -we4101 -we4113 -we4129 -we4133")
add_compiler_flags("-we4163 -we4189 -we4229 -we4311 -we4312 -we4313 -we4477 -we4603 -we4700 -we4715 -we4716")

# Disable warnings about specific features
add_compiler_ccxxflags("-nostdinc -Wno-char-subscripts -Wno-incompatible-library-redeclaration -Wno-microsoft-anon-tag")
add_compiler_ccxxflags("-Wno-microsoft-enum-forward-reference -Wno-multichar -Wno-parentheses-equality -Wno-undefined-inline")
add_compiler_ccxxflags("-Wno-gnu-folding-constant")

# Set debugging symbols output directory
set(CMAKE_PDB_OUTPUT_DIRECTORY "${EXECTOS_BINARY_DIR}/output/symbols")

# Set linker flags
add_linker_flags("${HOTPATCH_LINKER_FLAG} /IGNORE:4039 /IGNORE:4104 /MANIFEST:NO /NODEFAULTLIB /SAFESEH:NO")

# Set runtime library
set(CMAKE_MSVC_RUNTIME_LIBRARY "")

# Set default subsystem
set(CMAKE_CREATE_CONSOLE_EXE "")

# Export compile commands for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

# This function sets entrypoint of the binary
function(set_entrypoint MODULE ENTRYPOINT)
    if(${ENTRYPOINT} STREQUAL "0")
        add_module_linker_flags(${MODULE} "/NOENTRY")
    else()
        add_module_linker_flags(${MODULE} "/ENTRY:${ENTRYPOINT}")
    endif()
endfunction()

# This function sets imagebase address of the binary
function(set_imagebase MODULE IMAGEBASE)
    add_module_linker_flags(${MODULE} "/BASE:${IMAGEBASE}")
endfunction()

# This functions sets PE/COFF subsystem of the binary
function(set_subsystem MODULE SUBSYSTEM)
    string(TOUPPER ${SUBSYSTEM} SUBSYSTEM)
    add_module_linker_flags(${MODULE} "/SUBSYSTEM:${SUBSYSTEM},6.03")
endfunction()
