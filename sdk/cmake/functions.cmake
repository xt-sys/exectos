# This function enables the addition of ASM compiler switches
function(add_compiler_asmflags FLAGS)
    if(NOT ${ARGC} EQUAL 1)
        message(FATAL_ERROR "Invalid number of arguments passed to add_compiler_asmflags() function")
    endif()
    set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${FLAGS}" PARENT_SCOPE)
endfunction()

# This function enables the addition of C compiler switches
function(add_compiler_cflags FLAGS)
    if(NOT ${ARGC} EQUAL 1)
        message(FATAL_ERROR "Invalid number of arguments passed to add_compiler_cflags() function")
    endif()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAGS}" PARENT_SCOPE)
endfunction()

# This function enables the addition of C/C++ compilers switches
function(add_compiler_ccxxflags FLAGS)
    if(NOT ${ARGC} EQUAL 1)
        message(FATAL_ERROR "Invalid number of arguments passed to add_compiler_ccxxflags() function")
    endif()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAGS}" PARENT_SCOPE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS}" PARENT_SCOPE)
endfunction()

# This function enables the addition of C++ compiler switches
function(add_compiler_cxxflags FLAGS)
    if(NOT ${ARGC} EQUAL 1)
        message(FATAL_ERROR "Invalid number of arguments passed to add_compiler_cxxflags() function")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS}" PARENT_SCOPE)
endfunction()

# This function enables the addition of ASM/C/C++ compilers switches
function(add_compiler_flags FLAGS)
    if(NOT ${ARGC} EQUAL 1)
        message(FATAL_ERROR "Invalid number of arguments passed to add_compiler_flags() function")
    endif()
    set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${FLAGS}" PARENT_SCOPE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAGS}" PARENT_SCOPE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS}" PARENT_SCOPE)
endfunction()

# This function enables the addition of linker switches
function(add_linker_flags FLAGS)
    if(NOT ${ARGC} EQUAL 1)
        message(FATAL_ERROR "Invalid number of arguments passwd to add_linker_flags() function")
    endif()
    foreach(TYPE EXE MODULE SHARED)
        set(CMAKE_${TYPE}_LINKER_FLAGS "${CMAKE_${TYPE}_LINKER_FLAGS} ${FLAGS}" PARENT_SCOPE)
    endforeach()
endfunction()

# This function enabled the addition of linker switches for specified module
function(add_module_linker_flags MODULE FLAGS)
    if(NOT ${ARGC} EQUAL 2)
        message(FATAL_ERROR "Invalid number of arguments passwd to add_module_linker_flags() function")
    endif()
    set_module_property(${MODULE} LINK_FLAGS ${FLAGS})
endfunction()

# This function compiles an assembly bootsector file into a flat binary
function(compile_bootsector NAME SOURCE BASEADDR ENTRYPOINT)
    set(BINARY_NAME "${NAME}.bin")
    set(OBJECT_NAME "${NAME}.obj")

    get_directory_property(DEFS COMPILE_DEFINITIONS)
    foreach(def ${DEFS})
        list(APPEND ASM_DEFS "-D${def}")
    endforeach()

    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${BINARY_NAME}
        COMMAND ${CMAKE_ASM_COMPILER}
            /nologo
            --target=i386-none-elf
            ${ASM_DEFS}
            -I${CMAKE_CURRENT_SOURCE_DIR}
            /Fo${CMAKE_CURRENT_BINARY_DIR}/${OBJECT_NAME}
            -c -- ${SOURCE}
        COMMAND ${CMAKE_ASM_LINKER}
            -m elf_i386
            --image-base=0
            --oformat binary
            -Ttext=${BASEADDR}
            --entry=_start${ENTRYPOINT}
            -o ${CMAKE_CURRENT_BINARY_DIR}/${BINARY_NAME}
            ${CMAKE_CURRENT_BINARY_DIR}/${OBJECT_NAME}
        DEPENDS ${SOURCE}
    )

    add_custom_target(${NAME} ALL
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${BINARY_NAME}
    )
endfunction()

# This function sets the the qemu disk image size (in MiB)
function(set_disk_image_size SIZE)
	MATH(EXPR DISK_BLOCKS ${SIZE}*1024*1024/512)
	MATH(EXPR PART_BLOCKS ${DISK_BLOCKS}-2048)
	set(PROJECT_DISK_IMAGE_SIZE ${SIZE} CACHE INTERNAL "PROJECT_DISK_IMAGE_SIZE")
	set(PROJECT_DISK_IMAGE_BLOCKS ${DISK_BLOCKS} CACHE INTERNAL "PROJECT_DISK_IMAGE_BLOCKS")
	set(PROJECT_PART_IMAGE_BLOCKS ${PART_BLOCKS} CACHE INTERNAL "PROJECT_PART_IMAGE_BLOCKS")
endfunction()

# This function installs specified directory recursively under destination directory
function(set_install_dir DIRECTORY DESTINATION)
    install(DIRECTORY ${DIRECTORY} DESTINATION ${EXECTOS_BINARY_DIR}/output/binaries/${DESTINATION})
endfunction()

# This function installs specified file under destination directory
function(set_install_file FILENAME DESTINATION)
    install(FILES ${FILENAME} DESTINATION ${EXECTOS_BINARY_DIR}/output/binaries/${DESTINATION})
endfunction()

# This function installs specified target results under destination directory
function(set_install_target TARGET DESTINATION)
    set_target_properties(${TARGET} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${EXECTOS_BINARY_DIR}/output/sdk/lib")
    install(TARGETS ${TARGET} DESTINATION ${EXECTOS_BINARY_DIR}/output/binaries/${DESTINATION})
endfunction()

# This function sets a property for specified module
function(set_module_property MODULE PROPERTY FLAGS)
    if(NOT ${ARGC} EQUAL 3)
        message(FATAL_ERROR "Invalid number of arguments passwd to add_module_property() function")
    endif()
    get_target_property(VAL ${MODULE} ${PROPERTY})
    if(VAL)
        set(VAL "${VAL} ${FLAGS}")
    else()
        set(VAL "${FLAGS}")
    endif()
    set_property(TARGET ${MODULE} PROPERTY ${PROPERTY} ${VAL})
endfunction()

function(set_sdk_target FILENAME DESTINATION)
    install(DIRECTORY ${FILENAME} DESTINATION ${EXECTOS_BINARY_DIR}/output/sdk/${DESTINATION})
endfunction()

# This function is responsible for compiling module SPEC file
function(set_specfile SPECFILE EXPORTNAME)
    if(NOT ${ARGC} EQUAL 2)
        message(FATAL_ERROR "Invalid number of arguments passed to set_specfile() function")
    endif()
    get_filename_component(FILENAME ${SPECFILE} NAME_WE)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}.def ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}.c
                       COMMAND ${CMAKE_SPEC_COMPILER} -a=${ARCH} -n=${EXPORTNAME} -d=${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}.def -s=${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}.c ${CMAKE_CURRENT_SOURCE_DIR}/${SPECFILE})
endfunction()
