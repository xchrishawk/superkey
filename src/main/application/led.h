/**
 * @file        src/main/application/led.h
 * @brief       Header for the LED driver module.
 *
 * @author      Chris Vig (chris@invictus.so)
 * @date        2025-08-17
 * @copyright   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 */

#if !defined( APPLICATION_LED_H )
#define APPLICATION_LED_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stdint.h>

#include "utility/types.h"

/* ----------------------------------------------------- TYPES ------------------------------------------------------ */

/**
 * @typedef led_t
 * @brief   Enumeration of the available LEDs.
 */
typedef uint8_t led_t;
enum
{
    LED_STATUS,                             /**< The status LED.                        */
    LED_KEY,                                /**< The "key on" LED.                      */

    LED_COUNT,                              /**< Number of valid LEDs.                  */
};

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      led_get_enabled( led_t )
 * @brief   Returns `true` if the specified LED is enabled.
 */
bool led_get_enabled( led_t led );

/**
 * @fn      led_get_on( led_t )
 * @brief   Returns `true` if the specified LED is currently commanded on.
 */
bool led_get_on( led_t led );

/**
 * @fn      led_init( void )
 * @brief   Initializes the LED driver module.
 */
void led_init( void );

/**
 * @fn      led_set_enabled( led_t, bool )
 * @brief   Enables or disables the specified LED.
 * @note    This modifies the application configuration.
 */
void led_set_enabled( led_t led, bool enabled );

/**
 * @fn      led_set_on( led_t, bool )
 * @brief   Turns the specified LED on or off.
 */
void led_set_on( led_t led, bool on );

/**
 * @fn      led_tick( tick_t )
 * @brief   Performs periodic processing at the specified tick count.
 */
void led_tick( tick_t tick );

/**
 * @fn      led_toggle_on( led_t )
 * @brief   Toggles the state of the specified LED.
 */
void led_toggle_on( led_t led );

#endif /* !defined( APPLICATION_LED_H ) */
