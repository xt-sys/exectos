# NTOS compatibility driver
PROJECT(NTOSDRV)

# Specify include directories
include_directories(
    ${EXECTOS_SOURCE_DIR}/sdk/xtdk)

# Specify list of source code files
list(APPEND NTOSDRV_SOURCE
    ${NTOSDRV_SOURCE_DIR}/ntosdrv.c)

# Set module definition SPEC file
set_specfile(ntosdrv.spec ntosdrv.sys)

# Link bootloader executable
add_executable(ntosdrv ${NTOSDRV_SOURCE} ${CMAKE_CURRENT_BINARY_DIR}/ntosdrv.def)

# Add linker libraries
target_link_libraries(ntosdrv xtoskrnl)

# Set proper binary name and install target
set_target_properties(ntosdrv PROPERTIES SUFFIX .sys)
set_install_target(ntosdrv "exectos/drivers")

# Set loader entrypoint and subsystem
set_entrypoint(ntosdrv "XtDriverEntry")
set_linker_map(ntosdrv TRUE)
set_ordinals(ntosdrv TRUE)
set_subsystem(ntosdrv native xt_native_driver)
