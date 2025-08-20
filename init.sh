#!/bin/sh
#
# @file     init.sh
# @brief    Script to set up the build system with CMake.
#
# @author   Chris Vig (chris@invictus.so)
# @date     2025-08-16
#
# @warning  Any existing build system will be deleted.
#

# Variables
SOURCE_DIR=./
BUILD_DIR=${SOURCE_DIR}/build
CMAKE_ARGS=

# Delete and recreate build directory
rm -rf ${BUILD_DIR}

# Define function to make an MCU-specific build directory
make_build_dir() {
    # Create directory
    local THIS_BUILD_DIR=${BUILD_DIR}/$1
    mkdir -p "${THIS_BUILD_DIR}"
    # Initialize build system
    cmake ${CMAKE_ARGS}                 \
        -S "${SOURCE_DIR}"              \
        -B "${THIS_BUILD_DIR}"          \
        -DDEVICE_MCU:STRING=${1}        \
        -DDEVICE_F_CPU:STRING=${2}UL    \
        -DDEVICE_BAUD:STRING=${3}       \
        -DDISABLE_BUZZER=${4}
}

# Make all required directories
make_build_dir atmega1284p 8000000 250000 1
