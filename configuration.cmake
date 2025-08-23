#
# @file     configuration.cmake
# @brief    CMake file defining the project's configuration options.
#
# @author   Chris Vig (chris@invictus.so)
# @date     2025-08-19
#

# -- Device Setup --

# Options
set(DEVICE_MCU                          atmega1284p
    CACHE STRING                        "The MCU model to compile for.")
set(DEVICE_F_CPU                        8000000
    CACHE STRING                        "The device's CPU frequency, in Hz.")
set(DEVICE_BAUD                         19200
    CACHE STRING                        "The default USART baud rate, in bits per second.")

# Set defines
add_compile_definitions(
    F_CPU=${DEVICE_F_CPU}
    BAUD=${DEVICE_BAUD}
)

# -- Configuration Defaults --

set(CONFIG_DFLT_WPM                     200
    CACHE STRING                        "Default words per minute (in tenths of a WPM).")
set(CONFIG_DFLT_BUZZER_ENABLED          true
    CACHE STRING                        "Should the buzzer be enabled by default? (true / false)")
set(CONFIG_DFLT_BUZZER_FREQUENCY        700
    CACHE STRING                        "The default buzzer frequency, in Hz.")
set(CONFIG_DFLT_LED_STATUS_ENABLED      true
    CACHE STRING                        "Is the status LED enabled? (true / false)")
set(CONFIG_DFLT_LED_STATUS_ACTIVE_LO    false
    CACHE STRING                        "Is the status LED active when the GPIO is low? (true / false)")
set(CONFIG_DFLT_LED_KEY_ENABLED         true
    CACHE STRING                        "Is the key LED enabled? (true / false)")
set(CONFIG_DFLT_LED_KEY_ACTIVE_LO       false
    CACHE STRING                        "Is the key LED active when the GPIO is low? (true / false)")

# Set defines
add_compile_definitions(
    CONFIG_DFLT_WPM=${CONFIG_DFLT_WPM}
    CONFIG_DFLT_BUZZER_ENABLED=${CONFIG_DFLT_BUZZER_ENABLED}
    CONFIG_DFLT_BUZZER_FREQUENCY=${CONFIG_DFLT_BUZZER_FREQUENCY}
    CONFIG_DFLT_LED_STATUS_ENABLED=${CONFIG_DFLT_LED_STATUS_ENABLED}
    CONFIG_DFLT_LED_STATUS_ACTIVE_LO=${CONFIG_DFLT_LED_STATUS_ACTIVE_LO}
    CONFIG_DFLT_LED_KEY_ENABLED=${CONFIG_DFLT_LED_KEY_ENABLED}
    CONFIG_DFLT_LED_KEY_ACTIVE_LO=${CONFIG_DFLT_LED_KEY_ACTIVE_LO}
)
