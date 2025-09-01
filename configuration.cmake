#
# @file     configuration.cmake
# @brief    CMake file defining the project's configuration options.
#
# @author   Chris Vig (chris@invictus.so)
# @date     2025-08-19
# @cpyrt    © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
#
# @details
# This file defines the various configuration options for the application's build. This allows enabling and disabling
# features, changing configuration defaults, setting device pinout, etc. In most cases, this file should not be
# modified - instead, modify `build/CMakeCache.txt` (once the build system has been initialized) to make any
# modifications required.
#
# @note
# Please update the Developer's Guide if anything in this file changes.
#

# -- Feature Enablement --

# Define values
set(FEATURE_ENABLE_DEBUG_PORT               _FEATURE_ON
    CACHE STRING                            "Set to _FEATURE_OFF to omit the debug port from the build.")

# Set compile definitions
add_compile_definitions(
    _FEATURE_ENABLE_DEBUG_PORT=${FEATURE_ENABLE_DEBUG_PORT}
)

# -- Feature Options --

# Define values
set(FEATURE_OPT_INTF_PORT_BAUD              USART_BAUD_19200
    CACHE STRING                            "Baud rate for the interface serial port. (usart_baud_t)")
set(FEATURE_OPT_DEBUG_PORT_BAUD             USART_BAUD_19200
    CACHE STRING                            "Baud rate for the debug serial port. (usart_baud_t)")

# Set compile definitions
add_compile_definitions(
    _FEATURE_OPT_INTF_PORT_BAUD=${FEATURE_OPT_INTF_PORT_BAUD}
    _FEATURE_OPT_DEBUG_PORT_BAUD=${FEATURE_OPT_DEBUG_PORT_BAUD}
)

# -- Configuration Defaults --

# Define values
set(CONFIG_DFLT_WPM                         20.0f
    CACHE STRING                            "Default words-per-minute setting. (wpm_t)")
set(CONFIG_DFLT_WPM_ELEMENT_SCALE           1.0f
    CACHE STRING                            "Default element scale for all Morse code elements. (wpm_element_scale_t)")
set(CONFIG_DFLT_BUZZER_ENABLED              true
    CACHE STRING                            "Is buzzer enabled by default? (bool)")
set(CONFIG_DFLT_BUZZER_FREQUENCY            700
    CACHE STRING                            "Default buzzer frequency, in Hz. (buzzer_freq_t)")
set(CONFIG_DFLT_LED_STATUS_ENABLED          true
    CACHE STRING                            "Is the status LED enabled by default? (bool)")
set(CONFIG_DFLT_LED_KEY_ENABLED             true
    CACHE STRING                            "Is the keyer LED enabled by default? (bool)")
set(CONFIG_DFLT_IO_TYPE_TRS_0_TIP           IO_TYPE_INPUT_STRAIGHT_KEY
    CACHE STRING                            "I/O type for the tip pin of TRS connector 0. (io_type_t)")
set(CONFIG_DFLT_IO_POLARITY_TRS_0_TIP       IO_POLARITY_ACTIVE_LOW
    CACHE STRING                            "I/O polarity for the tip pin of TRS connector 0. (io_polarity_t)")
set(CONFIG_DFLT_IO_TYPE_TRS_0_RING          IO_TYPE_NONE
    CACHE STRING                            "I/O type for the ring pin of TRS connector 0. (io_type_t)")
set(CONFIG_DFLT_IO_POLARITY_TRS_0_RING      IO_POLARITY_ACTIVE_LOW
    CACHE STRING                            "I/O polarity for the ring pin of TRS connector 0. (io_polarity_t)")
set(CONFIG_DFLT_IO_TYPE_TRS_1_TIP           IO_TYPE_INPUT_PADDLE_LEFT
    CACHE STRING                            "I/O type for the tip pin of TRS connector 1. (io_type_t)")
set(CONFIG_DFLT_IO_POLARITY_TRS_1_TIP       IO_POLARITY_ACTIVE_LOW
    CACHE STRING                            "I/O polarity for the tip pin of TRS connector 1. (io_polarity_t)")
set(CONFIG_DFLT_IO_TYPE_TRS_1_RING          IO_TYPE_INPUT_PADDLE_RIGHT
    CACHE STRING                            "I/O type for the ring pin of TRS connector 1. (io_type_t)")
set(CONFIG_DFLT_IO_POLARITY_TRS_1_RING      IO_POLARITY_ACTIVE_LOW
    CACHE STRING                            "I/O polarity for the ring pin of TRS connector 1. (io_polarity_t)")
set(CONFIG_DFLT_IO_TYPE_TRS_2_TIP           IO_TYPE_INPUT_PADDLE_LEFT
    CACHE STRING                            "I/O type for the tip pin of TRS connector 2. (io_type_t)")
set(CONFIG_DFLT_IO_POLARITY_TRS_2_TIP       IO_POLARITY_ACTIVE_LOW
    CACHE STRING                            "I/O polarity for the tip pin of TRS connector 2. (io_polarity_t)")
set(CONFIG_DFLT_IO_TYPE_TRS_2_RING          IO_TYPE_INPUT_PADDLE_RIGHT
    CACHE STRING                            "I/O type for the ring pin of TRS connector 2. (io_type_t)")
set(CONFIG_DFLT_IO_POLARITY_TRS_2_RING      IO_POLARITY_ACTIVE_LOW
    CACHE STRING                            "I/O polarity for the ring pin of TRS connector 2. (io_polarity_t)")
set(CONFIG_DFLT_IO_TYPE_TRS_3_TIP           IO_TYPE_OUTPUT_KEYER
    CACHE STRING                            "I/O type for the tip pin of TRS connector 3. (io_type_t)")
set(CONFIG_DFLT_IO_POLARITY_TRS_3_TIP       IO_POLARITY_ACTIVE_LOW
    CACHE STRING                            "I/O polarity for the tip pin of TRS connector 3. (io_polarity_t)")
set(CONFIG_DFLT_IO_TYPE_TRS_3_RING          IO_TYPE_NONE
    CACHE STRING                            "I/O type for the ring pin of TRS connector 3. (io_type_t)")
set(CONFIG_DFLT_IO_POLARITY_TRS_3_RING      IO_POLARITY_ACTIVE_LOW
    CACHE STRING                            "I/O polarity for the ring pin of TRS connector 3. (io_polarity_t)")
set(CONFIG_DFLT_KEYER_PADDLE_MODE           KEYER_PADDLE_MODE_IAMBIC
    CACHE STRING                            "Default keyer paddle mode. (keyer_paddle_mode_t)")
set(CONFIG_DFLT_KEYER_PADDLE_INVERT         false
    CACHE STRING                            "Are keyer paddles inverted by default? (bool)")

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

# -- Pin Configuration --

# Define values
set(PIN_IO_PIN_TRS_0_TIP                    GPIO_PIN_A0
    CACHE STRING                            "GPIO pin for the tip pin of TRS connector 0. (gpio_pin_t)")
set(PIN_IO_PIN_TRS_0_RING                   GPIO_PIN_A1
    CACHE STRING                            "GPIO pin for the ring pin of TRS connector 0. (gpio_pin_t)")
set(PIN_IO_PIN_TRS_1_TIP                    GPIO_PIN_A2
    CACHE STRING                            "GPIO pin for the tip pin of TRS connector 1. (gpio_pin_t)")
set(PIN_IO_PIN_TRS_1_RING                   GPIO_PIN_A3
    CACHE STRING                            "GPIO pin for the ring pin of TRS connector 1. (gpio_pin_t)")
set(PIN_IO_PIN_TRS_2_TIP                    GPIO_PIN_A4
    CACHE STRING                            "GPIO pin for the tip pin of TRS connector 2. (gpio_pin_t)")
set(PIN_IO_PIN_TRS_2_RING                   GPIO_PIN_A5
    CACHE STRING                            "GPIO pin for the ring pin of TRS connector 2. (gpio_pin_t)")
set(PIN_IO_PIN_TRS_3_TIP                    GPIO_PIN_A6
    CACHE STRING                            "GPIO pin for the tip pin of TRS connector 3. (gpio_pin_t)")
set(PIN_IO_PIN_TRS_3_RING                   GPIO_PIN_A7
    CACHE STRING                            "GPIO pin for the ring pin of TRS connector 3. (gpio_pin_t)")
set(PIN_LED_STATUS                          GPIO_PIN_D7
    CACHE STRING                            "GPIO pin for the status LED. (gpio_pin_t)")
set(PIN_LED_KEY                             GPIO_PIN_D6
    CACHE STRING                            "GPIO pin for the keyer LED. (gpio_pin_t)")

# Set compile definitions
add_compile_definitions(
    _PIN_IO_PIN_TRS_0_TIP=${PIN_IO_PIN_TRS_0_TIP}
    _PIN_IO_PIN_TRS_0_RING=${PIN_IO_PIN_TRS_0_RING}
    _PIN_IO_PIN_TRS_1_TIP=${PIN_IO_PIN_TRS_1_TIP}
    _PIN_IO_PIN_TRS_1_RING=${PIN_IO_PIN_TRS_1_RING}
    _PIN_IO_PIN_TRS_2_TIP=${PIN_IO_PIN_TRS_2_TIP}
    _PIN_IO_PIN_TRS_2_RING=${PIN_IO_PIN_TRS_2_RING}
    _PIN_IO_PIN_TRS_3_TIP=${PIN_IO_PIN_TRS_3_TIP}
    _PIN_IO_PIN_TRS_3_RING=${PIN_IO_PIN_TRS_3_RING}
    _PIN_LED_STATUS=${PIN_LED_STATUS}
    _PIN_LED_KEY=${PIN_LED_KEY}
)
