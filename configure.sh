#!/bin/bash
# PROJECT:     ExectOS
# LICENSE:     See the COPYING.md in the top level directory
# FILE:        configure.sh
# DESCRIPTION: Project configuration script for preparing the build environment
# DEVELOPERS:  Rafal Kupiec <belliash@codingworkshop.eu.org>


# Check XTCHain
if [ "x${XTCVER}" = "x" ]; then
	echo "XTChain not detected or corrupted!"
	exit 1
fi

# Set target architecture
: ${ARCH:=${TARGET}}
: ${ARCH:=amd64}

# Set target build type
: ${BUILD_TYPE:=${BUILD_TYPE}}
: ${BUILD_TYPE:=DEBUG}

# Set variables
EXECTOS_SOURCE_DIR=$(cd `dirname ${0}` && pwd)
EXECTOS_BINARY_DIR=build-${ARCH}-${BUILD_TYPE,,}

# Create directories if needed
if [ "${EXECTOS_SOURCE_DIR}" = "${PWD}" ]; then
	echo Creating directories in ${EXECTOS_BINARY_DIR}
	mkdir -p "${EXECTOS_BINARY_DIR}"
	cd "${EXECTOS_BINARY_DIR}"
fi

# Delete old cache
rm -f CMakeCache.txt host-tools/CMakeCache.txt

# Configure project
cmake -G Ninja -DARCH:STRING=${ARCH} -DBUILD_TYPE:STRING=${BUILD_TYPE} "${EXECTOS_SOURCE_DIR}"

# Check if configuration succeeded
if [ ${?} -ne 0 ]; then
    echo "Configure script failed."
    exit 1
else
	echo "${ARCH}" > build.arch
    echo "Configure script completed. Enter '${EXECTOS_BINARY_DIR}' directory and execute 'xbuild' to build ExectOS."
fi
