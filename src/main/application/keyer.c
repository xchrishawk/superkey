/**
 * @file    src/main/application/keyer.c
 * @brief   Implementation for the keyer module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-19
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "application/buzzer.h"
#include "application/input.h"
#include "application/keyer.h"
#include "application/led.h"
#include "application/wpm.h"
#include "core/sys.h"
#include "drivers/gpio.h"
#include "utility/debug.h"
#include "utility/types.h"
#include "utility/utility.h"

#include "application/debug_port.h"

/* ----------------------------------------------------- TYPES ------------------------------------------------------ */

/**
 * @typedef state_t
 * @brief   Enumeration of states that the keyer module may be in.
 */
typedef uint8_t state_t;
enum
{
    STATE_OFF,                              /**< Keyer is off.                          */
    STATE_ON,                               /**< Keyer is on continuously.              */
    STATE_DOTS,                             /**< Keyer emits continuous dots.           */
    STATE_DASHES,                           /**< Keyer emits continuous dashes.         */

    STATE_COUNT,                            /**< Number of valid states.                */
};

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

static gpio_pin_t const KEYER_OUT_PIN = GPIO_PIN_A3;
static bool const KEYER_OUT_ACTIVE_HIGH = false;

/* --------------------------------------------------- VARIABLES ---------------------------------------------------- */

static bool s_keyed = false;                /**< Is the keyer hardware currently keyed? */
static bool s_panicked = false;             /**< Was the keyer panic activated?         */

static state_t s_state = STATE_OFF;         /**< Currently active keyer state.          */
static tick_t s_element_start = 0;          /**< Tick at which current element started. */
static tick_t s_element_duration = 0;       /**< Duration of current element.           */
static tick_t s_element_lockout = 0;        /**< Lockout until next element.            */

static wpm_t s_ticks_wpm = 0;               /**< WPM at which ticks were evaluated.     */
static tick_t s_dot_ticks = 0;              /**< Number of ticks per dot.               */
static tick_t s_dash_ticks = 0;             /**< Number of ticks per dash.              */
static tick_t s_space_ticks = 0;            /**< Number of ticks per element space.     */

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      get_keyed( void )
 * @brief   Returns `true` if the keyer hardware is keying.
 */
static bool get_keyed( void );

/**
 * @fn      next_state( void )
 * @brief   Returns the next keyer state, based on current inputs.
 */
static state_t get_next_state( void );

/**
 * @fn      set_keyed( bool )
 * @brief   Sets whether the keyer hardware is keying or not.
 */
static void set_keyed( bool keyed );

/**
 * @fn      update_hardware( void )
 * @brief   Synchronizes hardware state with the current keying state.
 */
static void update_hardware( void );

/**
 * @fn      update_ticks( void )
 * @brief   Update cached element tick counts, if required.
 * @note    This is an expensive calculation, so we only do it if the WPM changes.
 */
static void update_ticks( void );

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

void keyer_init( void )
{
    // Initialize local state
    s_keyed = false;
    s_panicked = false;
    s_state = STATE_OFF;
    s_element_start = 0;
    s_element_duration = 0;
    s_element_lockout = 0;
    s_ticks_wpm = 0;
    s_dot_ticks = 0;
    s_dash_ticks = 0;
    s_space_ticks = 0;

    // Initialize GPIO
    gpio_set_dir( KEYER_OUT_PIN, GPIO_DIR_OUT );

    // Default to key off
    set_keyed( false );

}   /* keyer_init() */


void keyer_panic( void )
{
    s_panicked = true;
    set_keyed( false );

}   /* keyer_panic() */


void keyer_tick( tick_t tick )
{
    // Utility macro
    #define is_timeout_elapsed( _duration )                                             \
        ( sys_elapsed( tick, s_element_start ) > ( _duration ) )

    // Update element tick counts if required
    update_ticks();

    // Check the next state, and determine if it's different than the current state
    state_t next_state = get_next_state();
    bool new_state = ( next_state != s_state );
    s_state = next_state;

    // Reset the panic flag if the state changed
    // (Note that in the logic below, this means that new_state==TRUE implies s_panicked==FALSE)
    if( new_state && s_panicked )
    {
        s_panicked = false;
        s_element_start = 0;
        s_element_duration = 0;
        s_element_lockout = 0;
    }

    // Execute whatever state we evaluate to
    switch( s_state )
    {
        case STATE_OFF:
        {
            // Deactivate keyer hardware, once allowed
            if( is_timeout_elapsed( s_element_duration ) )
                set_keyed( false );
            break;
        }

        case STATE_ON:
        {
            // Activate unconditionally, unless panicked
            if( ! s_panicked )
                set_keyed( true );
            break;
        }

        case STATE_DASHES:
        {
            if( ( new_state &&
                  is_timeout_elapsed( s_element_lockout ) ) ||
                ( ! new_state &&
                  ! s_panicked &&
                  ! get_keyed() &&
                  is_timeout_elapsed( s_element_lockout ) ) )
            {
                // Activate keyer hardware
                set_keyed( true );
                s_element_start = tick;
                s_element_duration = s_dash_ticks;
                s_element_lockout = s_dash_ticks + s_space_ticks;
            }
            else if( get_keyed() && is_timeout_elapsed( s_element_duration ) )
            {
                // Deactivate keyer hardware
                set_keyed( false );
            }
            break;
        }

        case STATE_DOTS:
        {
            if( ( new_state &&
                  is_timeout_elapsed( s_element_lockout ) ) ||
                ( ! new_state &&
                  ! s_panicked &&
                  ! get_keyed() &&
                  is_timeout_elapsed( s_element_lockout ) ) )
            {
                // Activate keyer hardware
                set_keyed( true );
                s_element_start = tick;
                s_element_duration = s_dot_ticks;
                s_element_lockout = s_dot_ticks + s_space_ticks;
            }
            else if( get_keyed() && is_timeout_elapsed( s_element_duration ) )
            {
                // Deactivate keyer hardware
                set_keyed( false );
            }
            break;
        }
    }

    #undef is_timeout_elapsed

}   /* keyer_tick() */


static bool get_keyed( void )
{
    return( s_keyed );

}   /* get_keyed() */


static state_t get_next_state( void )
{
    // Get all relevant inputs
    bool straight_key = input_get( INPUT_STRAIGHT_KEY );
    bool paddle_a = input_get( INPUT_PADDLE_A );
    bool paddle_b = input_get( INPUT_PADDLE_B );

    // Determine next state
    // TODO: lots of things
    if( straight_key )
        return( STATE_ON );
    else if( paddle_a && paddle_b )
        return( s_state );
    else if( paddle_a )
        return( STATE_DASHES );
    else if( paddle_b )
        return( STATE_DOTS );
    else
        return( STATE_OFF );

}   /* get_next_state() */


static void set_keyed( bool keyed )
{
    s_keyed = keyed;
    update_hardware();

}   /* set_keyed() */


static void update_hardware( void )
{
    // Update GPIO
    gpio_set_state( KEYER_OUT_PIN,
                    s_keyed ?
                        ( KEYER_OUT_ACTIVE_HIGH ? GPIO_STATE_HIGH : GPIO_STATE_LOW ) :
                        ( KEYER_OUT_ACTIVE_HIGH ? GPIO_STATE_LOW : GPIO_STATE_HIGH ) );

    // Update LED
    led_set_on( LED_KEY, s_keyed );

    // Update buzzer
    buzzer_set_on( s_keyed );

}   /* update_hardware() */


static void update_ticks( void )
{
    wpm_t wpm = wpm_get();
    if( s_ticks_wpm == wpm )
        return;

    wpm_ticks( s_ticks_wpm = wpm, & s_dot_ticks, & s_dash_ticks, & s_space_ticks, NULL, NULL );

}   /* update_ticks() */
