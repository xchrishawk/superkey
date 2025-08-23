#
# @file     CMakeLists.txt
# @brief    Main project CMake configuration.
#
# @author   Chris Vig (chris@invictus.so)
# @date     2025-08-16
#

cmake_minimum_required(VERSION 3.22)

# -- Packages --

find_package(Git REQUIRED)

# -- Project Configuration --

include(configuration.cmake)

# -- Toolchain Configuration --

# Tool names
set(AVR_GCC                         avr-gcc)
set(AVR_OBJCOPY                     avr-objcopy)
set(AVR_OBJDUMP                     avr-objdump)
set(AVR_STRIP                       avr-strip)

# Configure CMake system
set(CMAKE_SYSTEM_NAME               Generic)
set(CMAKE_ASM_COMPILER              ${AVR_GCC})
set(CMAKE_C_COMPILER                ${AVR_GCC})

# -- Device Setup --

set(DEVICE_MCU                      atmega1284p
    CACHE STRING                    "The MCU model to compile for.")
set(DEVICE_F_CPU                    8000000
    CACHE STRING                    "The device's CPU frequency, in Hz.")
set(DEVICE_BAUD                     19200
    CACHE STRING                    "The default USART baud rate, in bits per second.")

# -- Project Setup --

# Top-level project definition
project(SuperKey LANGUAGES C CXX ASM)

# Set up project directories
set(PROJECT_BASE_DIR                ${CMAKE_CURRENT_SOURCE_DIR})
set(PROJECT_LIBRARY_DIR             ${PROJECT_BASE_DIR}/lib)
set(PROJECT_EXECUTABLE_DIR          ${PROJECT_BASE_DIR}/src)
set(PROJECT_BUILD_DIR               ${CMAKE_CURRENT_BINARY_DIR})
set(PROJECT_AUTOGEN_DIR             ${PROJECT_BUILD_DIR}/autogen)
set(PROJECT_BIN_DIR                 ${PROJECT_BUILD_DIR})

# Configure output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BIN_DIR})

# Delete CMake default flags that we don't want to use
set(CMAKE_ASM_FLAGS_DEBUG           "" CACHE STRING "" FORCE)
set(CMAKE_ASM_FLAGS_RELEASE         "" CACHE STRING "" FORCE)
set(CMAKE_ASM_FLAGS_RELWITHDEBINFO  "" CACHE STRING "" FORCE)
set(CMAKE_ASM_FLAGS_MINSIZEREL      "" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_DEBUG             "" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_RELEASE           "" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_RELWITHDEBINFO    "" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_MINSIZEREL        "" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG           "" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE         "" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  "" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_MINSIZEREL      "" CACHE STRING "" FORCE)

# Set preprocessor defines
add_compile_definitions(
    # Device information
    MCU="${DEVICE_MCU}"
    F_CPU=${DEVICE_F_CPU}
    BAUD=${DEVICE_BAUD}
    # Build type
    _BUILD_TYPE="${CMAKE_BUILD_TYPE}"
    $<$<CONFIG:Debug>:_DEBUG=1>
    $<$<CONFIG:Release>:NDEBUG>
    $<$<CONFIG:Release>:_DEBUG=0>
    $<$<CONFIG:RelWithDebInfo>:NDEBUG>
    $<$<CONFIG:RelWithDebuInfo>:_DEBUG=0>
    $<$<CONFIG:MinSizeRel>:NDEBUG>
    $<$<CONFIG:MinSizeRel>:_DEBUG=0>
)

# Set compiler options
add_compile_options(
    # Device MCU type
    -mmcu=${DEVICE_MCU}
    # Warning options
    -Wall -Wextra -Wpedantic -Werror
    # -Os is used for all builds
    -Os
    # Build-specific flags
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:RelWithDebugInfo>:-g>
)

# Set linker options
add_link_options(
    # Device MCU type
    -mmcu=${DEVICE_MCU}
)

# -- Subdirectories --

# Main executable
add_subdirectory(${PROJECT_EXECUTABLE_DIR}/main)
