#
# @file     CMakeLists.txt
# @brief    CMake configuration for the main project executable.
#
# @author   Chris Vig (chris@invictus.so)
# @date     2025-08-16
# @cpyrt    © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
#

# -- Executable Configuration --

# Note that for the main executable, we use the project name forced to lowercase
string(TOLOWER "${CMAKE_PROJECT_NAME}" EXECUTABLE_NAME)
set(EXECUTABLE_SOURCE   application/buzzer.c
                        application/buzzer.h
                        application/config.c
                        application/config.h
                        application/debug_port.c
                        application/debug_port.h
                        application/intf_port.c
                        application/intf_port.h
                        application/io.c
                        application/io.h
                        application/keyer.c
                        application/keyer.h
                        application/led.c
                        application/led.h
                        application/storage.c
                        application/storage.h
                        application/strings.c
                        application/strings.h
                        application/wpm.c
                        application/wpm.h
                        core/main.c
                        core/sys.c
                        core/sys.h
                        core/version.c
                        core/version.h
                        drivers/eeprom.c
                        drivers/eeprom.h
                        drivers/gpio.c
                        drivers/gpio.h
                        drivers/usart.c
                        drivers/usart.h
                        utility/constants.h
                        utility/crc.c
                        utility/crc.h
                        utility/debug.c
                        utility/debug.h
                        utility/types.h
                        utility/utility.h)

# -- Set Up Project --

include(${PROJECT_EXECUTABLE_DIR}/executable.cmake)
