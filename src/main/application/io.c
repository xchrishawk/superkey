/**
 * @file    src/main/application/io.c
 * @brief   Implementation for the keyer input / output module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-29
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stdint.h>

#include <avr/interrupt.h>
#include <avr/io.h>

#include "application/config.h"
#include "application/io.h"
#include "core/sys.h"
#include "drivers/gpio.h"
#include "utility/debug.h"
#include "utility/utility.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

static gpio_pin_t const s_pin_tbl[] =
{
    _PIN_IO_PIN_TRS_0_TIP,
    _PIN_IO_PIN_TRS_0_RING,
    _PIN_IO_PIN_TRS_1_TIP,
    _PIN_IO_PIN_TRS_1_RING,
    _PIN_IO_PIN_TRS_2_TIP,
    _PIN_IO_PIN_TRS_2_RING,
    _PIN_IO_PIN_TRS_3_TIP,
    _PIN_IO_PIN_TRS_3_RING,
};
_Static_assert( array_count( s_pin_tbl ) == IO_PIN_COUNT, "Invalid pin table!" );

/* --------------------------------------------------- VARIABLES ---------------------------------------------------- */

static io_state_t s_output_state[ IO_PIN_COUNT ];

/* ----------------------------------------------------- MACROS ----------------------------------------------------- */

// Validation macros
#define validate_pin( _pin )                                                            \
    assert_always( ( _pin ) < IO_PIN_COUNT )
#define validate_polarity( _polarity )                                                  \
    assert_always( ( _polarity ) < IO_POLARITY_COUNT )
#define validate_state( _state )                                                        \
    assert_always( ( _state ) < IO_STATE_COUNT )
#define validate_type( _type )                                                          \
    assert_always( ( _type ) < IO_TYPE_COUNT )
#define validate_type_or_none( _type )                                                  \
    assert_always( ( _type ) < IO_TYPE_COUNT || ( _type ) == IO_TYPE_NONE )

// Get GPIO pin
#define gpio_pin( _pin )                                                                \
    ( s_pin_tbl[ ( _pin ) ] )

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      update_config( void )
 * @brief   Updates the configuration of all GPIO pins.
 * @note    If `force` is set to `true`, the configuration of all I/O pins will be forcibly updated. Otherwise, only the
 *          pins which appear to be in the wrong state will be updated.
 */
static void update_config( bool force );

/**
 * @fn      update_state( void )
 * @brief   Updates the state of all GPIO pins.
 */
static void update_state( void );

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

io_polarity_t io_get_polarity( io_pin_t pin )
{
    validate_pin( pin );

    return( config()->io_polarity[ pin ] );

}   /* io_get_polarity() */


io_state_t io_get_state( io_pin_t pin )
{
    validate_pin( pin );

    // Action depends on input type
    if( io_pin_is_input( pin ) )
    {
        // Evaluate based on GPIO state, ignore cache
        gpio_state_t gpio_state = gpio_get_state( gpio_pin( pin ) );
        switch( io_get_polarity( pin ) )
        {
        case IO_POLARITY_ACTIVE_LOW:
            return( gpio_state == GPIO_STATE_LOW );
        case IO_POLARITY_ACTIVE_HIGH:
            return( gpio_state == GPIO_STATE_HIGH );
        default:
            return( IO_STATE_NONE );
        }
    }
    else if( io_pin_is_output( pin ) )
        // Return cached state
        return( s_output_state[ pin ] );
    else
        // Not configured
        return( IO_STATE_NONE );

}   /* io_get_state() */


io_state_t io_get_state_type( io_type_t type )
{
    validate_type( type );

    // Search for any pin which is ON
    io_state_t ret = IO_STATE_NONE;
    for( io_pin_t pin = 0; pin < IO_PIN_COUNT; pin++ )
    {
        if( io_get_type( pin ) != type )
            continue;
        if( ret == IO_STATE_NONE )
            ret = IO_STATE_OFF;
        if( io_get_state( pin ) == IO_STATE_ON )
        {
            ret = IO_STATE_ON;
            break;
        }
    }

    return( ret );

}   /* io_get_state_type() */


io_type_t io_get_type( io_pin_t pin )
{
    validate_pin( pin );

    return( config()->io_type[ pin ] );

}   /* io_get_type() */


void io_init( void )
{
    // Initialize output state to all off
    for( io_pin_t pin = 0; pin < IO_PIN_COUNT; pin++ )
        s_output_state[ pin ] = IO_STATE_OFF;

    // Initialize
    update_config( true );
    update_state();

}   /* io_init() */


bool io_pin_is_input( io_pin_t pin )
{
    return( io_type_is_input( io_get_type( pin ) ) );

}   /* io_pin_is_input() */


bool io_pin_is_output( io_pin_t pin )
{
    return( io_type_is_output( io_get_type( pin ) ) );

}   /* io_pin_is_output() */


void io_set_output_state( io_pin_t pin, io_state_t state )
{
    validate_pin( pin );
    validate_state( state );

    // We can only set the state for outputs
    if( ! io_pin_is_output( pin ) )
        return;

    s_output_state[ pin ] = state;

    update_state();

}   /* io_set_output_state() */


void io_set_output_state_type( io_type_t type, io_state_t state )
{
    validate_type( type );
    validate_state( state );

    // We can only set the state for outputs
    if( ! io_type_is_output( type ) )
        return;

    for( io_pin_t pin = 0; pin < IO_PIN_COUNT; pin++ )
        if( io_get_type( pin ) == type )
            s_output_state[ pin ] = state;

    update_state();

}   /* io_set_output_state_type() */


void io_set_polarity( io_pin_t pin, io_polarity_t polarity )
{
    validate_pin( pin );
    validate_polarity( polarity );

    config_t config;
    config_get( & config );
    config.io_polarity[ pin ] = polarity;
    config_set( & config );

    update_config( false );
    update_state();

}   /* io_set_polarity() */


void io_set_type( io_pin_t pin, io_type_t type )
{
    validate_pin( pin );
    validate_type_or_none( type );

    config_t config;
    config_get( & config );
    config.io_type[ pin ] = type;
    config_set( & config );

    update_config( false );
    update_state();

}   /* io_set_type() */


void io_tick( tick_t tick )
{
    ( void )tick;

    update_config( false );
    update_state();

}   /* io_tick() */


bool io_type_is_input( io_type_t type )
{
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtype-limits"
    return( type >= IO_TYPE_INPUT_FIRST && type <= IO_TYPE_INPUT_LAST );
    #pragma GCC diagnostic pop

}   /* io_type_is_input() */


bool io_type_is_output( io_type_t type )
{
    return( type >= IO_TYPE_OUTPUT_FIRST && type <= IO_TYPE_OUTPUT_LAST );

}   /* io_type_is_output() */


static void update_config( bool force )
{
    // Enable interrupt for entire port
    gpio_set_pcint_enabled_port( GPIO_PORT_A, true );

    // Configure each individual pin
    for( io_pin_t pin = 0; pin < IO_PIN_COUNT; pin++ )
    {
        // Get I/O type for this pin
        io_type_t type = io_get_type( pin );
        bool type_is_output = io_type_is_output( type );
        bool type_is_input = io_type_is_input( type );

        // Configure the pin if forced, or if the I/O direction is wrong
        if( type_is_output &&
            ( force || gpio_get_dir( gpio_pin( pin ) ) != GPIO_DIR_OUT ) )
        {
            // Pin is an output
            gpio_set_dir( gpio_pin( pin ), GPIO_DIR_OUT );
            gpio_set_pullup( gpio_pin( pin ), false );
            gpio_set_pcint_enabled_pin( gpio_pin( pin ), false );
        }
        else if( type_is_input &&
                 ( force ||
                   gpio_get_dir( gpio_pin( pin ) ) != GPIO_DIR_IN ||
                   ( gpio_get_dir( gpio_pin( pin ) ) == GPIO_DIR_IN &&
                     gpio_get_pullup( gpio_pin( pin ) ) == false ) ) )
        {
            // Pin is an input
            gpio_set_dir( gpio_pin( pin ), GPIO_DIR_IN );
            gpio_set_pullup( gpio_pin( pin ), true );
            gpio_set_pcint_enabled_pin( gpio_pin( pin ), true );
        }
        else if( ! type_is_input &&
                 ! type_is_output &&
                 ( force ||
                   gpio_get_dir( gpio_pin( pin ) ) != GPIO_DIR_IN ||
                   ( gpio_get_dir( gpio_pin( pin ) ) == GPIO_DIR_IN &&
                     gpio_get_pullup( gpio_pin( pin ) ) == true ) ) )
        {
            // Pin is unused
            gpio_set_dir( gpio_pin( pin ), GPIO_DIR_IN );
            gpio_set_pullup( gpio_pin( pin ), false );
            gpio_set_pcint_enabled_pin( gpio_pin( pin ), false );
        }
    }

}   /* update_config() */


static void update_state( void )
{
    for( io_pin_t pin = 0; pin < IO_PIN_COUNT; pin++ )
    {
        // Only touch pins which are configured as outputs
        if( ! io_pin_is_output( pin ) )
            continue;

        // Get polarity and state
        io_polarity_t polarity = io_get_polarity( pin );
        io_state_t state = s_output_state[ pin ];

        // Take action only in definitive states
        if( polarity == IO_POLARITY_ACTIVE_LOW )
        {
            if( state == IO_STATE_OFF )
                gpio_set_state( gpio_pin( pin ), GPIO_STATE_HIGH );
            else if( state == IO_STATE_ON )
                gpio_set_state( gpio_pin( pin ), GPIO_STATE_LOW );
        }
        else if( polarity == IO_POLARITY_ACTIVE_HIGH )
        {
            if( state == IO_STATE_OFF )
                gpio_set_state( gpio_pin( pin ), GPIO_STATE_LOW );
            else if( state == IO_STATE_ON )
                gpio_set_state( gpio_pin( pin ), GPIO_STATE_HIGH );
        }
    }

}   /* update_state() */


ISR( PCINT0_vect )
{
    sys_enqueue_event( EVENT_IO_STATE );

}   /* ISR( PCINT0_vect ) */
