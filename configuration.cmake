#
# @file     configuration.cmake
# @brief    CMake file defining the project's configuration options.
#
# @author   Chris Vig (chris@invictus.so)
# @date     2025-08-19
#

# -- Configuration Defaults --

set(CONFIG_DFLT_WPM                     200
    CACHE STRING                        "Default words per minute (in tenths of a WPM).")
set(CONFIG_DFLT_BUZZER_ENABLED          true
    CACHE STRING                        "Should the buzzer be enabled by default? (true / false)")
set(CONFIG_DFLT_BUZZER_FREQUENCY        700
    CACHE STRING                        "The default buzzer frequency, in Hz.")
set(CONFIG_DFLT_LED_STATUS_ENABLED      true
    CACHE STRING                        "Is the status LED enabled? (true / false)")
set(CONFIG_DFLT_LED_KEY_ENABLED         true
    CACHE STRING                        "Is the key LED enabled? (true / false)")

# Set defines
add_compile_definitions(
    _CONFIG_DFLT_WPM=${CONFIG_DFLT_WPM}
    _CONFIG_DFLT_BUZZER_ENABLED=${CONFIG_DFLT_BUZZER_ENABLED}
    _CONFIG_DFLT_BUZZER_FREQUENCY=${CONFIG_DFLT_BUZZER_FREQUENCY}
    _CONFIG_DFLT_LED_STATUS_ENABLED=${CONFIG_DFLT_LED_STATUS_ENABLED}
    _CONFIG_DFLT_LED_STATUS_ACTIVE_LO=${CONFIG_DFLT_LED_STATUS_ACTIVE_LO}
    _CONFIG_DFLT_LED_KEY_ENABLED=${CONFIG_DFLT_LED_KEY_ENABLED}
    _CONFIG_DFLT_LED_KEY_ACTIVE_LO=${CONFIG_DFLT_LED_KEY_ACTIVE_LO}
)
