#
# @file     CMakeLists.txt
# @brief    Main project CMake configuration.
#
# @author   Chris Vig (chris@invictus.so)
# @date     2025-08-16
#

cmake_minimum_required(VERSION 3.22)

# -- Project Configuration --

include(configuration.cmake)

# -- Toolchain Configuration --

# Tool names
set(AVR_GCC                 avr-gcc)
set(AVR_OBJCOPY             avr-objcopy)
set(AVR_OBJDUMP             avr-objdump)
set(AVR_STRIP               avr-strip)

# Configure CMake system
set(CMAKE_SYSTEM_NAME       Generic)
set(CMAKE_ASM_COMPILER      ${AVR_GCC})
set(CMAKE_C_COMPILER        ${AVR_GCC})

# -- Project Setup --

# Top-level project definition
project(exakey LANGUAGES C CXX ASM)

# Set up project directories
set(PROJECT_BASE_DIR        ${CMAKE_CURRENT_SOURCE_DIR})
set(PROJECT_LIBRARY_DIR     ${PROJECT_BASE_DIR}/lib)
set(PROJECT_EXECUTABLE_DIR  ${PROJECT_BASE_DIR}/src)
set(PROJECT_BUILD_DIR       ${CMAKE_CURRENT_BINARY_DIR})
set(PROJECT_BIN_DIR         ${PROJECT_BUILD_DIR})

# Configure output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BIN_DIR})

# Shared variables for compilation flags
set(PROJECT_COMMON_FLAGS    "-mmcu=${EXAKEY_DEVICE_MCU} -Wall -Wextra -Wpedantic -Werror")
set(PROJECT_DEBUG_FLAGS     "-g")
set(PROJECT_OPTIMIZE_FLAGS  "-Os")

# Set compilation flags
# (note: quotation marks are required)
set(CMAKE_ASM_FLAGS         "${PROJECT_COMMON_FLAGS} ${PROJECT_DEBUG_FLAGS}")
set(CMAKE_C_FLAGS           "${PROJECT_COMMON_FLAGS} ${PROJECT_DEBUG_FLAGS} ${PROJECT_OPTIMIZE_FLAGS}")

# -- Subdirectories --

add_subdirectory(${PROJECT_EXECUTABLE_DIR}/exakey)
