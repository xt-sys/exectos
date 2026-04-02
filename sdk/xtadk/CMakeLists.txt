# XT Assembly Development Kit
PROJECT(XTADK)

# Specify include directories
include_directories(
    ${EXECTOS_SOURCE_DIR}/sdk/xtdk
    ${XTADK_SOURCE_DIR}/includes)

# Specify list of XTADK source code files
list(APPEND XTADK_SOURCE
    ${XTADK_SOURCE_DIR}/${ARCH}/ke.cc)

# Generate assembly header from XTADK sources
generate_xtadk(xtadk "${XTADK_SOURCE}")
