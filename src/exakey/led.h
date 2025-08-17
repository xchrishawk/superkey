/**
 * @file    led.h
 * @brief   Header for the LED driver module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-17
 */

#if !defined( EXAKEY_LED_H )
#define EXAKEY_LED_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stdint.h>

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
 * @fn      led_init( void )
 * @brief   Initializes the LED driver module.
 */
void led_init( void );

/**
 * @fn      led_set( led_t, bool )
 * @brief   Turns the specified LED on or off.
 */
void led_set( led_t led, bool on );

/**
 * @fn      led_toggle( led_t )
 * @brief   Toggles the state of the specified LED.
 */
void led_toggle( led_t led );

#endif /* !defined( EXAKEY_LED_H ) */
