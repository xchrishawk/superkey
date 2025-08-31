/**
 * @file    src/main/drivers/gpio.h
 * @brief   Header for the low-level GPIO driver module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-17
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 */

#if !defined( DRIVERS_GPIO_H )
#define DRIVERS_GPIO_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stdint.h>

/* ----------------------------------------------------- TYPES ------------------------------------------------------ */

/**
 * @typedef gpio_dir_t
 * @brief   Enumeration of GPIO I/O directions.
 */
typedef uint8_t gpio_dir_t;
enum
{
    GPIO_DIR_IN,                            /**< GPIO pin is an input.                  */
    GPIO_DIR_OUT,                           /**< GPIO pin is an output.                 */

    GPIO_DIR_COUNT,                         /**< Number of valid GPIO directions.       */
};

/**
 * @typedef gpio_pin_t
 * @brief   Enumeration of the supported GPIO pins.
 * @note    Note that the availability of a pin in this enumeration does not necessarily mean that it may be used for
 *          I/O. Many of the I/O pins are being used for their alternative functions, and attempting to use them as GPIO
 *          pins will result in undefined behavior. Refer to doc/hardware-guide.md for more details on the usage of each
 *          pin.
 */
typedef uint8_t gpio_pin_t;
enum
{
    // Port A
    GPIO_PIN_A0,                            /**< GPIO port A pin 0.                     */
    GPIO_PIN_A1,                            /**< GPIO port A pin 1.                     */
    GPIO_PIN_A2,                            /**< GPIO port A pin 2.                     */
    GPIO_PIN_A3,                            /**< GPIO port A pin 3.                     */
    GPIO_PIN_A4,                            /**< GPIO port A pin 4.                     */
    GPIO_PIN_A5,                            /**< GPIO port A pin 5.                     */
    GPIO_PIN_A6,                            /**< GPIO port A pin 6.                     */
    GPIO_PIN_A7,                            /**< GPIO port A pin 7.                     */

    // Port B
    GPIO_PIN_B0,                            /**< GPIO port B pin 0.                     */
    GPIO_PIN_B1,                            /**< GPIO port B pin 1.                     */
    GPIO_PIN_B2,                            /**< GPIO port B pin 2.                     */
    GPIO_PIN_B3,                            /**< GPIO port B pin 3.                     */
    GPIO_PIN_B4,                            /**< GPIO port B pin 4.                     */
    GPIO_PIN_B5,                            /**< GPIO port B pin 5.                     */
    GPIO_PIN_B6,                            /**< GPIO port B pin 6.                     */
    GPIO_PIN_B7,                            /**< GPIO port B pin 7.                     */

    // Port C
    GPIO_PIN_C0,                            /**< GPIO port C pin 0.                     */
    GPIO_PIN_C1,                            /**< GPIO port C pin 1.                     */
    GPIO_PIN_C2,                            /**< GPIO port C pin 2.                     */
    GPIO_PIN_C3,                            /**< GPIO port C pin 3.                     */
    GPIO_PIN_C4,                            /**< GPIO port C pin 4.                     */
    GPIO_PIN_C5,                            /**< GPIO port C pin 5.                     */
    GPIO_PIN_C6,                            /**< GPIO port C pin 6.                     */
    GPIO_PIN_C7,                            /**< GPIO port C pin 7.                     */

    // Port D
    GPIO_PIN_D0,                            /**< GPIO port D pin 0.                     */
    GPIO_PIN_D1,                            /**< GPIO port D pin 1.                     */
    GPIO_PIN_D2,                            /**< GPIO port D pin 2.                     */
    GPIO_PIN_D3,                            /**< GPIO port D pin 3.                     */
    GPIO_PIN_D4,                            /**< GPIO port D pin 4.                     */
    GPIO_PIN_D5,                            /**< GPIO port D pin 5.                     */
    GPIO_PIN_D6,                            /**< GPIO port D pin 6.                     */
    GPIO_PIN_D7,                            /**< GPIO pord D pin 7.                     */

    GPIO_PIN_COUNT,                         /**< Number of valid GPIO pins.             */

    GPIO_PIN_INVALID                        /**< Invalid or unknown GPIO pin.           */
        = GPIO_PIN_COUNT,

    GPIO_PIN_PORT_A_FIRST                   /**< First GPIO pin in port A.              */
        = GPIO_PIN_A0,
    GPIO_PIN_PORT_A_LAST                    /**< Last GPIO pin in port A.               */
        = GPIO_PIN_A7,

    GPIO_PIN_PORT_B_FIRST                   /**< First GPIO pin in port B.              */
        = GPIO_PIN_B0,
    GPIO_PIN_PORT_B_LAST                    /**< Last GPIO pin in port B.               */
        = GPIO_PIN_B7,

    GPIO_PIN_PORT_C_FIRST                   /**< First GPIO pin in port C.              */
        = GPIO_PIN_C0,
    GPIO_PIN_PORT_C_LAST                    /**< Last GPIO pin in port C.               */
        = GPIO_PIN_C7,

    GPIO_PIN_PORT_D_FIRST                   /**< First GPIO pin in port D.              */
        = GPIO_PIN_D0,
    GPIO_PIN_PORT_D_LAST                    /**< Last GPIO pin in port D.               */
        = GPIO_PIN_D7,
};

/**
 * @typedef gpio_port_t
 * @brief   Enumeration of GPIO ports.
 */
typedef uint8_t gpio_port_t;
enum
{
    GPIO_PORT_A,                            /**< GPIO port A.                           */
    GPIO_PORT_B,                            /**< GPIO port B.                           */
    GPIO_PORT_C,                            /**< GPIO port C.                           */
    GPIO_PORT_D,                            /**< GPIO port D.                           */

    GPIO_PORT_COUNT,                        /**< Number of valid GPIO ports.            */
};

/**
 * @typedef gpio_state_t
 * @brief   Enumeration of GPIO pin states.
 */
typedef uint8_t gpio_state_t;
enum
{
    GPIO_STATE_LOW,                         /**< Pin is in the low state.               */
    GPIO_STATE_HIGH,                        /**< Pin is in the high state.              */

    GPIO_STATE_COUNT,                       /**< Number of valid GPIO states.           */
};

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      gpio_get_dir( gpio_pin_t )
 * @brief   Returns the I/O direction for the specified pin.
 */
gpio_dir_t gpio_get_dir( gpio_pin_t pin );

/**
 * @fn      gpio_get_pin_port( gpio_pin_t )
 * @brief   Returns the port for the specified pin.
 */
gpio_port_t gpio_get_pin_port( gpio_pin_t pin );

/**
 * @fn      gpio_get_pullup( gpio_pin_t )
 * @brief   Returns the state of the pull-up resistor for the specified pin.
 * @note    This is only applicable to input pins.
 */
bool gpio_get_pullup( gpio_pin_t pin );

/**
 * @fn      gpio_get_state( gpio_pin_t )
 * @brief   Returns the current state of the specified pin.
 */
gpio_state_t gpio_get_state( gpio_pin_t pin );

/**
 * @fn      gpio_set_dir( gpio_pin_t, gpio_dir_t )
 * @brief   Sets the I/O direction for the specified pin.
 */
void gpio_set_dir( gpio_pin_t pin, gpio_dir_t dir );

/**
 * @fn      gpio_set_pcint_enabled_pin( gpio_pin_t, bool )
 * @brief   Enables or disables the pin change interrupt for the specified GPIO pin.
 * @note    The interrupt must also be enabled on the corresponding GPIO port - see `gpio_set_pcint_enabled_port()`.
 */
void gpio_set_pcint_enabled_pin( gpio_pin_t pin, bool enabled );

/**
 * @fn      gpio_set_pcint_enabled_port( gpio_port_t, bool )
 * @brief   Enables or disables the pin change interrupt for the specified GPIO port.
 * @note    The interrupt must also be enabled on the corresponding GPIO pin - see `gpio_set_pcint_enabled_pin()`.
 */
void gpio_set_pcint_enabled_port( gpio_port_t port, bool enabled );

/**
 * @fn      gpio_set_pullup( gpio_pin_t, bool )
 * @brief   Sets the state of the pull-up resistor for the specified pin.
 * @note    This is only applicable to input pins.
 */
void gpio_set_pullup( gpio_pin_t pin, bool enabled );

/**
 * @fn      gpio_set_state( gpio_pin_t, gpio_dir_t )
 * @brief   Sets the I/O state (low or high) for the specified pin.
 * @note    This is only applicable to output pins.
 */
void gpio_set_state( gpio_pin_t pin, gpio_state_t state );

/**
 * @fn      gpio_toggle_state( gpio_pin_t )
 * @brief   Toggles the I/O state (low or high) for the specified pin.
 * @note    This is only applicable to output pins.
 */
void gpio_toggle_state( gpio_pin_t pin );

#endif /* !defined( DRIVERS_GPIO_H ) */
