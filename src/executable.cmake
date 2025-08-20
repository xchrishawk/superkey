#
# @file     executable.cmake
# @brief    Template CMake file for executable images.
#
# @author   Chris Vig (chris@invictus.so)
# @date     2025-08-16
#
# Skeleton CMake file which can be included in a CMakeLists.txt file to set up an executable.
# This allows quickly creating new executables with a minimum amount of boilerplate.
#
# Example usage (CMakeLists.txt):
#
# ```
# set(EXECUTABLE_NAME some-image)
# set(EXECUTABLE_SOURCE main.c some-file.c)
# set(EXECUTABLE_LIBS some-lib)
# include(${PROJECT_SRC_DIR}/executable.cmake)
# ```
#

# -- Project Setup --

# Define new subproject for executable
project(${CMAKE_PROJECT_NAME}::${EXECUTABLE_NAME})

# -- Validate Arguments --

# Set default arguments
if(NOT DEFINED EXECUTABLE_LIBS)
    set(EXECUTABLE_LIBS "")
endif()
if(NOT DEFINED EXECUTABLE_COMPILE_OPTIONS)
    set(EXECUTABLE_COMPILE_OPTIONS "")
endif()
if(NOT DEFINED EXECUTABLE_LINK_OPTIONS)
    set(EXECUTABLE_LINK_OPTIONS "")
endif()

# Ensure required arguments are set
if(NOT DEFINED EXECUTABLE_NAME)
    message(FATAL_ERROR "EXECUTABLE_NAME must be defined!")
endif()
if(NOT DEFINED EXECUTABLE_SOURCE)
    message(FATAL_ERROR "EXECUTABLE_SOURCE must be defined!")
endif()
if(NOT DEFINED EXECUTABLE_LIBS)
    message(FATAL_ERROR "EXECUTABLE_LIBS must be defined!")
endif()
if(NOT DEFINED EXECUTABLE_COMPILE_OPTIONS)
    message(FATAL_ERROR "EXECUTABLE_COMPILE_OPTIONS must be defined!")
endif()
if(NOT DEFINED EXECUTABLE_LINK_OPTIONS)
    message(FATAL_ERROR "EXECUTABLE_LINK_OPTIONS must be defined!")
endif()

# -- Local Variables --

# Names for supplemental files
set(EXECUTABLE_DUMP_FILE            ${EXECUTABLE_NAME}.txt)
set(EXECUTABLE_ELF_FILE             ${EXECUTABLE_NAME}.elf)
set(EXECUTABLE_ELF_STRIPPED_FILE    ${EXECUTABLE_NAME}.elf.stripped)
set(EXECUTABLE_IHEX_FILE            ${EXECUTABLE_NAME}.ihex)
set(EXECUTABLE_MAP_FILE             ${EXECUTABLE_NAME}.map)

# -- Main Executable --

# Define executable
add_executable(
    ${EXECUTABLE_NAME}
    ${EXECUTABLE_SOURCE}
)

# Set compile options
target_compile_options(
    ${EXECUTABLE_NAME}
    PRIVATE
    ${EXECUTABLE_COMPILE_OPTIONS}
)

# Ensure that the main application directory is included as an include dir
target_include_directories(
    ${EXECUTABLE_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set linker options
target_link_options(
    ${EXECUTABLE_NAME}
    PRIVATE
    ${EXECUTABLE_LINK_OPTIONS}
    -Wl,-Map=${PROJECT_BUILD_DIR}/${EXECUTABLE_MAP_FILE}
)

# Add libraries
target_link_libraries(
    ${EXECUTABLE_NAME}
    ${EXECUTABLE_LIBS}
)

# Set custom filename for executable
set_target_properties(
    ${EXECUTABLE_NAME}
    PROPERTIES
        OUTPUT_NAME ${EXECUTABLE_ELF_FILE}
)

# -- Supplemental Files --

# Add commands for supplemental files
add_custom_command(
    # List outputs of the command
    OUTPUT ${EXECUTABLE_DUMP_FILE}
    OUTPUT ${EXECUTABLE_ELF_STRIPPED_FILE}
    OUTPUT ${EXECUTABLE_IHEX_FILE}
    # Make this command depend on the main ELF file
    DEPENDS ${EXECUTABLE_NAME}
    # Step 1 - Generate the dump file using avr-objdump
    COMMAND ${AVR_OBJDUMP}
    ARGS -x -D -S ${EXECUTABLE_ELF_FILE} > ${EXECUTABLE_DUMP_FILE}
    # Step 2 - Create a copy of the main ELF file to strip
    COMMAND cp
    ARGS ${EXECUTABLE_ELF_FILE} ${EXECUTABLE_ELF_STRIPPED_FILE}
    # Step 3 - Strip the copy with avr-strip
    COMMAND ${AVR_STRIP}
    ARGS ${EXECUTABLE_ELF_STRIPPED_FILE}
    # Step 4 - Convert the stripped ELF file to IHEX, removing any EEPROM section
    COMMAND ${AVR_OBJCOPY}
    ARGS -R .eeprom -O ihex ${EXECUTABLE_ELF_STRIPPED_FILE} ${EXECUTABLE_IHEX_FILE}
    # Do this all in the build directory
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

# Add target for supplemental files so that they get built
add_custom_target(
    ${EXECUTABLE_NAME}-post-build
    ALL
    DEPENDS
        ${EXECUTABLE_DUMP_FILE}
        ${EXECUTABLE_ELF_STRIPPED_FILE}
        ${EXECUTABLE_IHEX_FILE}
)

# Clean additional support files
set_property(
    DIRECTORY
    APPEND PROPERTY
    ADDITIONAL_MAKE_CLEAN_FILES
        ${PROJECT_BIN_DIR}/${EXECUTABLE_DUMP_FILE}
        ${PROJECT_BIN_DIR}/${EXECUTABLE_ELF_STRIPPED_FILE}
        ${PROJECT_BIN_DIR}/${EXECUTABLE_IHEX_FILE}
        ${PROJECT_BIN_DIR}/${EXECUTABLE_MAP_FILE}
)
