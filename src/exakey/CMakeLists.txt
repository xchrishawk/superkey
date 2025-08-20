#
# @file     CMakeLists.txt
# @brief    CMake configuration for the exakey executable.
#
# @author   Chris Vig (chris@invictus.so)
# @date     2025-08-16
#

# -- Executable Configuration --

set(EXECUTABLE_NAME     exakey)
set(EXECUTABLE_SOURCE   application/buzzer.c
                        application/buzzer.h
                        application/input.c
                        application/input.h
                        application/keyer.c
                        application/keyer.h
                        application/led.c
                        application/led.h
                        core/config.c
                        core/config.h
                        core/main.c
                        core/sys.c
                        core/sys.h
                        drivers/gpio.c
                        drivers/gpio.h
                        drivers/usart.c
                        drivers/usart.h
                        utility/constants.h
                        utility/debug.c
                        utility/debug.h
                        utility/types.h
                        utility/utility.h)

# -- Set Up Project --

include(${PROJECT_EXECUTABLE_DIR}/executable.cmake)
