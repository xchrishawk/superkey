/**
 * @file    src/main/application/input.c
 * @brief   Implementation for the key input module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-17
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "application/config.h"
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
    GPIO_PIN_A0,                            /* INPUT_PIN_0_TIP                          */
    GPIO_PIN_A1,                            /* INPUT_PIN_0_RING                         */
    GPIO_PIN_A2,                            /* INPUT_PIN_1_TIP                          */
    GPIO_PIN_A3,                            /* INPUT_PIN_1_RING                         */
    GPIO_PIN_A4,                            /* INPUT_PIN_2_TIP                          */
    GPIO_PIN_A5,                            /* INPUT_PIN_2_RING                         */
};
_Static_assert( array_count( s_pin_tbl ) == INPUT_PIN_COUNT, "Invalid pin table!" );

/* ----------------------------------------------------- MACROS ----------------------------------------------------- */

/**
 * @def     validate_pin( _pin )
 * @brief   Validates that the specified `input_pin_t` is valid.
 */
#define validate_pin( _pin )                                                            \
    assert_always( ( _pin ) < INPUT_PIN_COUNT )

/**
 * @def     validate_polarity( _polarity )
 * @brief   Validates that the specified `input_polarity_t` is valid.
 */
#define validate_polarity( _polarity )                                                  \
    assert_always( ( _polarity ) < INPUT_POLARITY_COUNT )

/**
 * @def     validate_type( _type )
 * @brief   Validates that the specified `input_type_t` is valid.
 */
#define validate_type( _input )                                                         \
    assert_always( ( _input ) < INPUT_TYPE_COUNT )

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

bool input_get_on( input_pin_t pin )
{
    validate_pin( pin );

    gpio_state_t state = gpio_get_state( s_pin_tbl[ pin ] );
    if( input_get_polarity( pin ) == INPUT_POLARITY_ACTIVE_LOW )
        return( state == GPIO_STATE_LOW );
    else
        return( state == GPIO_STATE_HIGH );

}   /* input_get() */


input_polarity_t input_get_polarity( input_pin_t pin )
{
    validate_pin( pin );

    return( config()->input_polarity[ pin ] );

}   /* input_get_polarity() */


input_type_t input_get_type( input_pin_t pin )
{
    validate_pin( pin );

    return( config()->input_type[ pin ] );

}   /* input_get_type() */


void input_init( void )
{
    // Configure GPIO, enabling interrupts
    for( input_pin_t pin = 0; pin < INPUT_PIN_COUNT; pin++ )
    {
        gpio_pin_t gpio_pin = s_pin_tbl[ pin ];
        gpio_set_dir( gpio_pin, GPIO_DIR_IN );
        gpio_set_pullup( gpio_pin, true );
        gpio_set_pcint_enabled_port( gpio_get_pin_port( gpio_pin ), true );
        gpio_set_pcint_enabled_pin( gpio_pin, true );
    }

}   /* input_init() */


void input_set_polarity( input_pin_t pin, input_polarity_t polarity )
{
    validate_pin( pin );
    validate_polarity( polarity );

    config_t config;
    config_get( & config );
    config.input_polarity[ pin ] = polarity;
    config_set( & config );

}   /* input_set_polarity() */


void input_set_type( input_pin_t pin, input_type_t type )
{
    validate_pin( pin );
    if( type != INPUT_TYPE_NONE )
        validate_type( type );

    config_t config;
    config_get( & config );
    config.input_type[ pin ] = type;
    config_set( & config );

}   /* input_set_type() */


void input_tick( tick_t tick )
{
    ( void )tick;

}   /* input_tick() */


bool input_type_get_on( input_type_t type )
{
    validate_type( type );

    bool result = false;
    for( input_pin_t pin = 0; pin < INPUT_PIN_COUNT; pin++ )
    {
        if( input_get_type( pin ) == type && input_get_on( pin ) )
        {
            result = true;
            break;
        }
    }

    return( result );

}   /* input_type_get_on() */


input_type_field_t input_types_get_on( void )
{
    input_type_field_t ret = 0;
    for( input_pin_t pin = 0; pin < INPUT_PIN_COUNT; pin++ )
    {
        input_type_t type = input_get_type( pin );
        bool on = input_get_on( pin );
        if( type != INPUT_TYPE_NONE && on )
            set_bit( ret, type );
    }

    return( ret );

}   /* input_types_get_on() */


ISR( PCINT0_vect )
{
    sys_enqueue_event( EVENT_INPUT_STATE );

} /* ISR( PCINT0_vect ) */
