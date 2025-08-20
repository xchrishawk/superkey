#
# @file     configuration.cmake
# @brief    CMake file defining the project's configuration options.
#
# @author   Chris Vig (chris@invictus.so)
# @date     2025-08-19
#

# -- Device Setup --

set(EXAKEY_DEVICE_MCU           atmega1284p
    CACHE STRING                "The MCU model to compile for.")
set(EXAKEY_DEVICE_F_CPU         8000000
    CACHE STRING                "The device's CPU frequency, in Hz.")
set(EXAKEY_DEVICE_BAUD          250000
    CACHE STRING                "The default USART baud rate, in bits per second.")

# -- Configuration Defaults --

set(EXAKEY_DFLT_BUZZER_FREQ     700
    CACHE STRING                "The default buzzer frequency, in Hz.")

# -- Application Options --

set(EXAKEY_OPT_DISABLE_BUZZER   FALSE
    CACHE BOOL                  "Set to TRUE to disable the buzzer.")
