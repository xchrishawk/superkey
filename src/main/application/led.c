/**
 * @file        src/main/application/led.c
 * @brief       Implementation for the LED driver module.
 *
 * @author      Chris Vig (chris@invictus.so)
 * @date        2025-08-17
 * @copyright   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stdint.h>

#include "application/config.h"
#include "application/led.h"
#include "drivers/gpio.h"
#include "utility/debug.h"
#include "utility/types.h"
#include "utility/utility.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

static gpio_pin_t const s_gpio_tbl[] =
{
    // Must be in same order as led_t enum
    GPIO_PIN_D7,                            /* LED_STATUS                               */
    GPIO_PIN_D6,                            /* LED_KEY                                  */
};
_Static_assert( array_count( s_gpio_tbl ) == LED_COUNT, "Invalid GPIO table!" );

/* ----------------------------------------------------- MACROS ----------------------------------------------------- */

// Validation macros
#define validate_led( _led )                                                            \
    assert_always( ( _led ) < LED_COUNT )

// Get pin for LED
#define LED_PIN( _led )                                                                 \
    ( s_gpio_tbl[ ( _led ) ] )

/**
 * @def     is_led_on_and_enabled( _led )
 * @brief   Returns `true` if the specified LED is enabled *and* requested on.
 */
#define is_led_on_and_enabled( _led )                                                   \
    ( ( s_on[ ( _led ) ] ) && ( config()->led_enabled[ ( _led ) ] ) )

/* --------------------------------------------------- VARIABLES ---------------------------------------------------- */

static bool s_on[ LED_COUNT ];              /**< Is LED currently requested on?         */

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      update_hardware( void )
 * @brief   Updates the GPIO hardware based on current LED state and configuration.
 * @note    For now, this always updates the state for all LEDs. This can be revisited if this becomes a performance
 *          concern.
 */
static void update_hardware( void );

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */


bool led_get_enabled( led_t led )
{
    validate_led( led );

    return( config()->led_enabled[ led ] );

}   /* led_get_enabled() */


bool led_get_on( led_t led )
{
    validate_led( led );

    return( s_on[ led ] );

}   /* led_get_on() */


void led_init( void )
{
    // Initialize local state
    for( led_t led = 0; led < LED_COUNT; led++ )
        s_on[ led ] = false;

    // Initialize GPIO directions
    for( led_t led = 0; led < LED_COUNT; led++ )
        gpio_set_dir( LED_PIN( led ), GPIO_DIR_OUT );

    // Synchronize state
    update_hardware();

}   /* led_init() */


void led_set_enabled( led_t led, bool enabled )
{
    validate_led( led );

    config_t config;
    config_get( & config );
    config.led_enabled[ led ] = enabled;
    config_set( & config );

    update_hardware();

}   /* led_set_enabled() */


void led_set_on( led_t led, bool on )
{
    validate_led( led );

    s_on[ led ] = on;
    update_hardware();

}   /* led_set_on() */


void led_tick( tick_t tick )
{
    ( void )tick;
    update_hardware();

}   /* led_tick() */


void led_toggle_on( led_t led )
{
    validate_led( led );

    s_on[ led ] = ! s_on[ led ];
    update_hardware();

}   /* led_toggle_on() */


static void update_hardware( void )
{
    for( led_t led = 0; led < LED_COUNT; led++ )
    {
        gpio_set_state( LED_PIN( led ),
                        is_led_on_and_enabled( led ) ? GPIO_STATE_HIGH : GPIO_STATE_LOW );
    }

}   /* update_hardware() */
