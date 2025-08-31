#
# @file     configuration.cmake
# @brief    CMake file defining the project's configuration options.
#
# @author   Chris Vig (chris@invictus.so)
# @date     2025-08-19
# @cpyrt    © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
#

# -- Feature Enablement --

# Define values
set(FEATURE_ENABLE_DEBUG_PORT               _FEATURE_ON
    CACHE STRING                            "Set to _FEATURE_ON to enable the debug port.")

# Set compile definitions
add_compile_definitions(
    _FEATURE_ENABLE_DEBUG_PORT=${FEATURE_ENABLE_DEBUG_PORT}
)

# -- Feature Options --

# Define values
set(FEATURE_OPT_INTF_PORT_BAUD              USART_BAUD_19200
    CACHE STRING                            "Baud rate for interface port. (usart_baud_t)")
set(FEATURE_OPT_DEBUG_PORT_BAUD             USART_BAUD_19200
    CACHE STRING                            "Baud rate for debug port. (usart_baud_t)")

# Set compile definitions
add_compile_definitions(
    _FEATURE_OPT_INTF_PORT_BAUD=${FEATURE_OPT_INTF_PORT_BAUD}
    _FEATURE_OPT_DEBUG_PORT_BAUD=${FEATURE_OPT_DEBUG_PORT_BAUD}
)

# -- Configuration Defaults --

# Define values
set(CONFIG_DFLT_WPM                         20.0f
    CACHE STRING                            "Default words per minute. (1.0f - 100.0f)")
set(CONFIG_DFLT_WPM_ELEMENT_SCALE           1.0f
    CACHE STRING                            "Default Morse code element scale. (0.1f - 10.f)")
set(CONFIG_DFLT_BUZZER_ENABLED              true
    CACHE STRING                            "Should the buzzer be enabled by default? (true / false)")
set(CONFIG_DFLT_BUZZER_FREQUENCY            700
    CACHE STRING                            "The default buzzer frequency, in Hz. (500 - 2000 Hz)")
set(CONFIG_DFLT_LED_STATUS_ENABLED          true
    CACHE STRING                            "Is the status LED enabled? (true / false)")
set(CONFIG_DFLT_LED_KEY_ENABLED             true
    CACHE STRING                            "Is the key LED enabled? (true / false)")
set(CONFIG_DFLT_IO_TYPE_TRS_0_TIP           IO_TYPE_INPUT_STRAIGHT_KEY
    CACHE STRING                            "I/O type for the tip of TRS 0. (io_type_t)")
set(CONFIG_DFLT_IO_POLARITY_TRS_0_TIP       IO_POLARITY_ACTIVE_LOW
    CACHE STRING                            "I/O polarity for the tip of TRS 0. (io_polarity_t)")
set(CONFIG_DFLT_IO_TYPE_TRS_0_RING          IO_TYPE_NONE
    CACHE STRING                            "I/O type for the ring of TRS 0. (io_type_t)")
set(CONFIG_DFLT_IO_POLARITY_TRS_0_RING      IO_POLARITY_ACTIVE_LOW
    CACHE STRING                            "I/O polarity for the ring of TRS 0. (io_polarity_t)")
set(CONFIG_DFLT_IO_TYPE_TRS_1_TIP           IO_TYPE_INPUT_PADDLE_LEFT
    CACHE STRING                            "I/O type for the tip of TRS 1. (io_type_t)")
set(CONFIG_DFLT_IO_POLARITY_TRS_1_TIP       IO_POLARITY_ACTIVE_LOW
    CACHE STRING                            "I/O polarity for the tip of TRS 1. (io_polarity_t)")
set(CONFIG_DFLT_IO_TYPE_TRS_1_RING          IO_TYPE_INPUT_PADDLE_RIGHT
    CACHE STRING                            "I/O type for the ring of TRS 1. (io_type_t)")
set(CONFIG_DFLT_IO_POLARITY_TRS_1_RING      IO_POLARITY_ACTIVE_LOW
    CACHE STRING                            "I/O polarity for the ring of TRS 1. (io_polarity_t)")
set(CONFIG_DFLT_IO_TYPE_TRS_2_TIP           IO_TYPE_INPUT_PADDLE_LEFT
    CACHE STRING                            "I/O type for the tip of TRS 2. (io_type_t)")
set(CONFIG_DFLT_IO_POLARITY_TRS_2_TIP       IO_POLARITY_ACTIVE_LOW
    CACHE STRING                            "I/O polarity for the tip of TRS 2. (io_polarity_t)")
set(CONFIG_DFLT_IO_TYPE_TRS_2_RING          IO_TYPE_INPUT_PADDLE_RIGHT
    CACHE STRING                            "I/O type for the ring of TRS 2. (io_type_t)")
set(CONFIG_DFLT_IO_POLARITY_TRS_2_RING      IO_POLARITY_ACTIVE_LOW
    CACHE STRING                            "I/O polarity for the ring of TRS 2. (io_polarity_t)")
set(CONFIG_DFLT_IO_TYPE_TRS_3_TIP           IO_TYPE_OUTPUT_KEYER
    CACHE STRING                            "I/O type for the tip of TRS 3. (io_type_t)")
set(CONFIG_DFLT_IO_POLARITY_TRS_3_TIP       IO_POLARITY_ACTIVE_LOW
    CACHE STRING                            "I/O polarity for the tip of TRS 3. (io_polarity_t)")
set(CONFIG_DFLT_IO_TYPE_TRS_3_RING          IO_TYPE_NONE
    CACHE STRING                            "I/O type for the ring of TRS 3. (io_type_t)")
set(CONFIG_DFLT_IO_POLARITY_TRS_3_RING      IO_POLARITY_ACTIVE_LOW
    CACHE STRING                            "I/O polarity for the ring of TRS 3. (io_polarity_t)")
set(CONFIG_DFLT_KEYER_PADDLE_MODE           KEYER_PADDLE_MODE_IAMBIC
    CACHE STRING                            "Keyer paddle mode. (keyer_paddle_mode_t)")
set(CONFIG_DFLT_KEYER_PADDLE_INVERT         false
    CACHE STRING                            "Set to true to invert the paddles. (true / false)")
set(CONFIG_DFLT_KEYER_OUTPUT_ACTIVE_LOW     true
    CACHE STRING                            "Set to true for active low keyer output. (true / false)")

# Set compile definitions
add_compile_definitions(
    _CONFIG_DFLT_WPM=${CONFIG_DFLT_WPM}
    _CONFIG_DFLT_WPM_ELEMENT_SCALE=${CONFIG_DFLT_WPM_ELEMENT_SCALE}
    _CONFIG_DFLT_BUZZER_ENABLED=${CONFIG_DFLT_BUZZER_ENABLED}
    _CONFIG_DFLT_BUZZER_FREQUENCY=${CONFIG_DFLT_BUZZER_FREQUENCY}
    _CONFIG_DFLT_LED_STATUS_ENABLED=${CONFIG_DFLT_LED_STATUS_ENABLED}
    _CONFIG_DFLT_LED_STATUS_ACTIVE_LO=${CONFIG_DFLT_LED_STATUS_ACTIVE_LO}
    _CONFIG_DFLT_LED_KEY_ENABLED=${CONFIG_DFLT_LED_KEY_ENABLED}
    _CONFIG_DFLT_LED_KEY_ACTIVE_LO=${CONFIG_DFLT_LED_KEY_ACTIVE_LO}
    _CONFIG_DFLT_IO_TYPE_TRS_0_TIP=${CONFIG_DFLT_IO_TYPE_TRS_0_TIP}
    _CONFIG_DFLT_IO_POLARITY_TRS_0_TIP=${CONFIG_DFLT_IO_POLARITY_TRS_0_TIP}
    _CONFIG_DFLT_IO_TYPE_TRS_0_RING=${CONFIG_DFLT_IO_TYPE_TRS_0_RING}
    _CONFIG_DFLT_IO_POLARITY_TRS_0_RING=${CONFIG_DFLT_IO_POLARITY_TRS_0_RING}
    _CONFIG_DFLT_IO_TYPE_TRS_1_TIP=${CONFIG_DFLT_IO_TYPE_TRS_1_TIP}
    _CONFIG_DFLT_IO_POLARITY_TRS_1_TIP=${CONFIG_DFLT_IO_POLARITY_TRS_1_TIP}
    _CONFIG_DFLT_IO_TYPE_TRS_1_RING=${CONFIG_DFLT_IO_TYPE_TRS_1_RING}
    _CONFIG_DFLT_IO_POLARITY_TRS_1_RING=${CONFIG_DFLT_IO_POLARITY_TRS_1_RING}
    _CONFIG_DFLT_IO_TYPE_TRS_2_TIP=${CONFIG_DFLT_IO_TYPE_TRS_2_TIP}
    _CONFIG_DFLT_IO_POLARITY_TRS_2_TIP=${CONFIG_DFLT_IO_POLARITY_TRS_2_TIP}
    _CONFIG_DFLT_IO_TYPE_TRS_2_RING=${CONFIG_DFLT_IO_TYPE_TRS_2_RING}
    _CONFIG_DFLT_IO_POLARITY_TRS_2_RING=${CONFIG_DFLT_IO_POLARITY_TRS_2_RING}
    _CONFIG_DFLT_IO_TYPE_TRS_3_TIP=${CONFIG_DFLT_IO_TYPE_TRS_3_TIP}
    _CONFIG_DFLT_IO_POLARITY_TRS_3_TIP=${CONFIG_DFLT_IO_POLARITY_TRS_3_TIP}
    _CONFIG_DFLT_IO_TYPE_TRS_3_RING=${CONFIG_DFLT_IO_TYPE_TRS_3_RING}
    _CONFIG_DFLT_IO_POLARITY_TRS_3_RING=${CONFIG_DFLT_IO_POLARITY_TRS_3_RING}
    _CONFIG_DFLT_KEYER_PADDLE_MODE=${CONFIG_DFLT_KEYER_PADDLE_MODE}
    _CONFIG_DFLT_KEYER_PADDLE_INVERT=${CONFIG_DFLT_KEYER_PADDLE_INVERT}
    _CONFIG_DFLT_KEYER_OUTPUT_ACTIVE_LOW=${CONFIG_DFLT_KEYER_OUTPUT_ACTIVE_LOW}
)
