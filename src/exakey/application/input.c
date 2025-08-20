/**
 * @file    src/exakey/application/input.c
 * @brief   Implementation for the key input module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-17
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "application/input.h"
#include "core/sys.h"
#include "drivers/gpio.h"
#include "utility/debug.h"
#include "utility/types.h"
#include "utility/utility.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

// Lookup table for GPIO pins
static gpio_pin_t const s_pin_tbl[] =
{
    GPIO_PIN_A0,                            /* INPUT_STRAIGHT_KEY                       */
    GPIO_PIN_A1,                            /* INPUT_PADDLE_A                           */
    GPIO_PIN_A2,                            /* INPUT_PADDLE_B                           */
};
_Static_assert( array_count( s_pin_tbl ) == INPUT_COUNT, "Invalid pin table!" );

/* ----------------------------------------------------- MACROS ----------------------------------------------------- */

// Validation macros
#define validate_input( _input )                                                        \
    assert_always( ( _input ) < INPUT_COUNT )

// Get input info
#define PIN( _input )                                                                   \
    s_pin_tbl[ ( _input ) ]

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

bool input_get( input_t input )
{
    validate_input( input );

    return( gpio_get_state( PIN( input ) ) == GPIO_STATE_LOW );

}   /* input_get() */


void input_init( void )
{
    // Configure GPIO, enabling interrupts
    for( input_t input = 0; input < INPUT_COUNT; input++ )
    {
        gpio_pin_t pin = PIN( input );
        gpio_set_dir( pin, GPIO_DIR_IN );
        gpio_set_pullup( pin, true );
        gpio_set_pcint_enabled_port( gpio_get_pin_port( pin ), true );
        gpio_set_pcint_enabled_pin( pin, true );
    }

}   /* input_init() */


void input_tick( tick_t tick )
{
    ( void )tick;

}   /* input_tick() */


ISR( PCINT0_vect )
{
    sys_enqueue_event( EVENT_INPUT_STATE );

} /* ISR( PCINT0_vect ) */
