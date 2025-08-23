#
# @file     configuration.cmake
# @brief    CMake file defining the project's configuration options.
#
# @author   Chris Vig (chris@invictus.so)
# @date     2025-08-19
#

# -- Device Setup --

# Options
set(EXAKEY_DEVICE_MCU                   atmega1284p
    CACHE STRING                        "The MCU model to compile for.")
set(EXAKEY_DEVICE_F_CPU                 8000000
    CACHE STRING                        "The device's CPU frequency, in Hz.")
set(EXAKEY_DEVICE_BAUD                  19200
    CACHE STRING                        "The default USART baud rate, in bits per second.")

# Set defines
add_compile_definitions(
    F_CPU=${EXAKEY_DEVICE_F_CPU}
    BAUD=${EXAKEY_DEVICE_BAUD}
)

# -- Configuration Defaults --

set(EXAKEY_DFLT_WPM                     200
    CACHE STRING                        "Default words per minute (in tenths of a WPM).")
set(EXAKEY_DFLT_BUZZER_ENABLED          true
    CACHE STRING                        "Should the buzzer be enabled by default? (true / false)")
set(EXAKEY_DFLT_BUZZER_FREQUENCY        700
    CACHE STRING                        "The default buzzer frequency, in Hz.")
set(EXAKEY_DFLT_LED_STATUS_ENABLED      true
    CACHE STRING                        "Is the status LED enabled? (true / false)")
set(EXAKEY_DFLT_LED_STATUS_ACTIVE_LO    false
    CACHE STRING                        "Is the status LED active when the GPIO is low? (true / false)")
set(EXAKEY_DFLT_LED_KEY_ENABLED         true
    CACHE STRING                        "Is the key LED enabled? (true / false)")
set(EXAKEY_DFLT_LED_KEY_ACTIVE_LO       false
    CACHE STRING                        "Is the key LED active when the GPIO is low? (true / false)")

# Set defines
add_compile_definitions(
    EXAKEY_DFLT_WPM=${EXAKEY_DFLT_WPM}
    EXAKEY_DFLT_BUZZER_ENABLED=${EXAKEY_DFLT_BUZZER_ENABLED}
    EXAKEY_DFLT_BUZZER_FREQUENCY=${EXAKEY_DFLT_BUZZER_FREQUENCY}
    EXAKEY_DFLT_LED_STATUS_ENABLED=${EXAKEY_DFLT_LED_STATUS_ENABLED}
    EXAKEY_DFLT_LED_STATUS_ACTIVE_LO=${EXAKEY_DFLT_LED_STATUS_ACTIVE_LO}
    EXAKEY_DFLT_LED_KEY_ENABLED=${EXAKEY_DFLT_LED_KEY_ENABLED}
    EXAKEY_DFLT_LED_KEY_ACTIVE_LO=${EXAKEY_DFLT_LED_KEY_ACTIVE_LO}
)
