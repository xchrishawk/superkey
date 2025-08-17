#
# @file     CMakeLists.txt
# @brief    CMake configuration for the exakey executable.
#
# @author   Chris Vig (chris@invictus.so)
# @date     2025-08-16
#

# -- Executable Configuration --

set(EXECUTABLE_NAME     exakey)
set(EXECUTABLE_SOURCE   debug.c
                        debug.h
                        gpio.c
                        gpio.h
                        input.c
                        input.h
                        led.c
                        led.h
                        main.c
                        sys.c
                        sys.h
                        types.h
                        utility.h)

# -- Set Up Project --

include(${PROJECT_EXECUTABLE_DIR}/executable.cmake)
