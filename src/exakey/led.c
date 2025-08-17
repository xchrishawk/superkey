/**
 * @file    led.c
 * @brief   Implementation for the LED driver module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-17
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stdint.h>

#include "debug.h"
#include "gpio.h"
#include "led.h"
#include "utility.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

static struct
{
    gpio_pin_t      pin;                    /**< GPIO pin for the LED.                  */
    bool            active_lo;              /**< True if pin is set low to activate LED.*/
}
const s_gpio_tbl[] =
{
    // Must be in same order as led_t enum
    { GPIO_PIN_D6,  false },                /* LED_STATUS                               */
    { GPIO_PIN_D7,  false },                /* LED_KEY                                  */
};
_Static_assert( array_count( s_gpio_tbl ) == LED_COUNT, "Invalid GPIO table!" );

/* ----------------------------------------------------- MACROS ----------------------------------------------------- */

// Validation macros
#define validate_led( _led )                                                            \
    assert_always( ( _led ) < LED_COUNT )

#define LED_PIN( _led )                                                                 \
    ( s_gpio_tbl[ ( _led ) ].pin )
#define LED_ACTIVE_LO( _led )                                                           \
    ( s_gpio_tbl[ ( _led ) ].active_lo )

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

void led_init( void )
{
    // Initialize GPIO
    for( led_t led = 0; led < LED_COUNT; led++ )
    {
        gpio_set_dir( LED_PIN( led ), GPIO_DIR_OUT );
        gpio_set_state( LED_PIN( led ), LED_ACTIVE_LO( led ) ? GPIO_STATE_HIGH : GPIO_STATE_LOW );
    }

}   /* led_init() */


void led_set( led_t led, bool on )
{
    validate_led( led );

    gpio_set_state(
        LED_PIN( led ),
        LED_ACTIVE_LO( led ) ?
            ( on ? GPIO_STATE_LOW : GPIO_STATE_HIGH ) :
            ( on ? GPIO_STATE_HIGH : GPIO_STATE_LOW )
    );

}   /* led_set() */


void led_toggle( led_t led )
{
    validate_led( led );

    gpio_toggle_state( LED_PIN( led ) );

}   /* led_toggle() */
