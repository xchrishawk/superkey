#
# @file     configuration.cmake
# @brief    CMake file defining the project's configuration options.
#
# @author   Chris Vig (chris@invictus.so)
# @date     2025-08-19
#

# -- Configuration Defaults --

set(CONFIG_DFLT_WPM                         200
    CACHE STRING                            "Default words per minute, in tenths of a WPM. (10 - 1000)")
set(CONFIG_DFLT_BUZZER_ENABLED              true
    CACHE STRING                            "Should the buzzer be enabled by default? (true / false)")
set(CONFIG_DFLT_BUZZER_FREQUENCY            700
    CACHE STRING                            "The default buzzer frequency, in Hz. (500 - 2000 Hz)")
set(CONFIG_DFLT_LED_STATUS_ENABLED          true
    CACHE STRING                            "Is the status LED enabled? (true / false)")
set(CONFIG_DFLT_LED_KEY_ENABLED             true
    CACHE STRING                            "Is the key LED enabled? (true / false)")
set(CONFIG_DFLT_INPUT_TYPE_TRS_0_TIP        INPUT_TYPE_STRAIGHT_KEY
    CACHE STRING                            "Input type for the tip of TRS 0. (input_type_t)")
set(CONFIG_DFLT_INPUT_POLARITY_TRS_0_TIP    INPUT_POLARITY_ACTIVE_LOW
    CACHE STRING                            "Input polarity for the tip of TRS 0. (input_polarity_t)")
set(CONFIG_DFLT_INPUT_TYPE_TRS_0_RING       INPUT_TYPE_NONE
    CACHE STRING                            "Input type for the ring of TRS 0. (input_type_t)")
set(CONFIG_DFLT_INPUT_POLARITY_TRS_0_RING   INPUT_POLARITY_ACTIVE_LOW
    CACHE STRING                            "Input polarity for the ring of TRS 0. (input_polarity_t)")
set(CONFIG_DFLT_INPUT_TYPE_TRS_1_TIP        INPUT_TYPE_PADDLE_LEFT
    CACHE STRING                            "Input type for the tip of TRS 0. (input_type_t)")
set(CONFIG_DFLT_INPUT_POLARITY_TRS_1_TIP    INPUT_POLARITY_ACTIVE_LOW
    CACHE STRING                            "Input polarity for the tip of TRS 0. (input_polarity_t)")
set(CONFIG_DFLT_INPUT_TYPE_TRS_1_RING       INPUT_TYPE_PADDLE_RIGHT
    CACHE STRING                            "Input type for the ring of TRS 0. (input_type_t)")
set(CONFIG_DFLT_INPUT_POLARITY_TRS_1_RING   INPUT_POLARITY_ACTIVE_LOW
    CACHE STRING                            "Input polarity for the ring of TRS 0. (input_polarity_t)")
set(CONFIG_DFLT_INPUT_TYPE_TRS_2_TIP        INPUT_TYPE_NONE
    CACHE STRING                            "Input type for the tip of TRS 0. (input_type_t)")
set(CONFIG_DFLT_INPUT_POLARITY_TRS_2_TIP    INPUT_POLARITY_ACTIVE_LOW
    CACHE STRING                            "Input polarity for the tip of TRS 0. (input_polarity_t)")
set(CONFIG_DFLT_INPUT_TYPE_TRS_2_RING       INPUT_TYPE_NONE
    CACHE STRING                            "Input type for the ring of TRS 0. (input_type_t)")
set(CONFIG_DFLT_INPUT_POLARITY_TRS_2_RING   INPUT_POLARITY_ACTIVE_LOW
    CACHE STRING                            "Input polarity for the ring of TRS 0. (input_polarity_t)")
set(CONFIG_DFLT_KEYER_MODE                  KEYER_MODE_IAMBIC
    CACHE STRING                            "Keyer mode. (keyer_mode_t)")
set(CONFIG_DFLT_KEYER_OUTPUT_ACTIVE_LOW     true
    CACHE STRING                            "Set to true for active low keyer output. (true / false)")
set(CONFIG_DFLT_KEYER_INVERT_PADDLES        false
    CACHE STRING                            "Set to true to invert the paddles. (true / false)")

# Set defines
add_compile_definitions(
    _CONFIG_DFLT_WPM=${CONFIG_DFLT_WPM}
    _CONFIG_DFLT_BUZZER_ENABLED=${CONFIG_DFLT_BUZZER_ENABLED}
    _CONFIG_DFLT_BUZZER_FREQUENCY=${CONFIG_DFLT_BUZZER_FREQUENCY}
    _CONFIG_DFLT_LED_STATUS_ENABLED=${CONFIG_DFLT_LED_STATUS_ENABLED}
    _CONFIG_DFLT_LED_STATUS_ACTIVE_LO=${CONFIG_DFLT_LED_STATUS_ACTIVE_LO}
    _CONFIG_DFLT_LED_KEY_ENABLED=${CONFIG_DFLT_LED_KEY_ENABLED}
    _CONFIG_DFLT_LED_KEY_ACTIVE_LO=${CONFIG_DFLT_LED_KEY_ACTIVE_LO}
    _CONFIG_DFLT_INPUT_TYPE_TRS_0_TIP=${CONFIG_DFLT_INPUT_TYPE_TRS_0_TIP}
    _CONFIG_DFLT_INPUT_POLARITY_TRS_0_TIP=${CONFIG_DFLT_INPUT_POLARITY_TRS_0_TIP}
    _CONFIG_DFLT_INPUT_TYPE_TRS_0_RING=${CONFIG_DFLT_INPUT_TYPE_TRS_0_RING}
    _CONFIG_DFLT_INPUT_POLARITY_TRS_0_RING=${CONFIG_DFLT_INPUT_POLARITY_TRS_0_RING}
    _CONFIG_DFLT_INPUT_TYPE_TRS_1_TIP=${CONFIG_DFLT_INPUT_TYPE_TRS_1_TIP}
    _CONFIG_DFLT_INPUT_POLARITY_TRS_1_TIP=${CONFIG_DFLT_INPUT_POLARITY_TRS_1_TIP}
    _CONFIG_DFLT_INPUT_TYPE_TRS_1_RING=${CONFIG_DFLT_INPUT_TYPE_TRS_1_RING}
    _CONFIG_DFLT_INPUT_POLARITY_TRS_1_RING=${CONFIG_DFLT_INPUT_POLARITY_TRS_1_RING}
    _CONFIG_DFLT_INPUT_TYPE_TRS_2_TIP=${CONFIG_DFLT_INPUT_TYPE_TRS_2_TIP}
    _CONFIG_DFLT_INPUT_POLARITY_TRS_2_TIP=${CONFIG_DFLT_INPUT_POLARITY_TRS_2_TIP}
    _CONFIG_DFLT_INPUT_TYPE_TRS_2_RING=${CONFIG_DFLT_INPUT_TYPE_TRS_2_RING}
    _CONFIG_DFLT_INPUT_POLARITY_TRS_2_RING=${CONFIG_DFLT_INPUT_POLARITY_TRS_2_RING}
    _CONFIG_DFLT_KEYER_MODE=${CONFIG_DFLT_KEYER_MODE}
    _CONFIG_DFLT_KEYER_OUTPUT_ACTIVE_LOW=${CONFIG_DFLT_KEYER_OUTPUT_ACTIVE_LOW}
    _CONFIG_DFLT_KEYER_INVERT_PADDLES=${CONFIG_DFLT_KEYER_INVERT_PADDLES}
)
