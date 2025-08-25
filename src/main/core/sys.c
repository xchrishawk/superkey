/**
 * @file    src/main/core/sys.c
 * @brief   Implementation for the main system module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-17
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <assert.h>
#include <stdint.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>

#include "core/sys.h"
#include "utility/debug.h"
#include "utility/types.h"
#include "utility/utility.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

/**
 * @def     DEBUG_TEST_CLOCK_WRAPAROUND
 * @brief   If set to `ON`, the system tick will be set to 15 seconds prior to a wraparound at startup.
 */
#define DEBUG_TEST_CLOCK_WRAPAROUND     ON

/* --------------------------------------------------- VARIABLES ---------------------------------------------------- */

static event_field_t s_pending_events = 0;  /**< Bitfield of currently pending events.  */
static tick_t s_tick = 0;                   /**< Current system tick.                   */

/* ----------------------------------------------------- MACROS ----------------------------------------------------- */

/**
 * @def     validate_event( _event )
 * @brief   Ensures that the specified `event_t` is valid.
 */
#define validate_event( _event )                                                        \
    assert_always( ( _event ) < EVENT_COUNT )

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      sleep_until_interrupt( void )
 * @brief   Sleeps the unit until the next interrupt.
 */
static void sleep_until_interrupt( void );

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

tick_t sys_elapsed( tick_t now, tick_t then )
{
    if( now >= then )
        return( now - then );
    else
        return( ( TICK_MAX - then + 1 ) + now );

}   /* sys_elapsed() */


tick_t sys_elapsed_now( tick_t then )
{
    return( sys_elapsed( sys_get_tick(), then ) );

}   /* sys_elapsed_now() */


void sys_enqueue_event( event_t event )
{
    validate_event( event );

    set_bit( s_pending_events, event );

}   /* sys_enqueue_event() */


tick_t sys_get_tick( void )
{
    bool int_en = sys_intrpt_enabled();
    sys_cli();
    tick_t ret = s_tick;
    sys_set_intrpt_enabled( int_en );

    return( ret );

}   /* sys_get_tick() */


void sys_init( void )
{
    // Clear interrupts
    sys_cli();

    // Initialize system tick count
    #if DEBUG_TEST_CLOCK_WRAPAROUND
        s_tick = ( TICK_MAX - ( 15 * TICKS_PER_SEC ) );
    #else
        s_tick = 0;
    #endif

    //
    // Initialize timer 0 for main system tick at 1 millisecond period. Based on the following:
    //
    // - F_CPU      = 8 MHz
    // - P_CPU      = 1 / 8 MHz         = 0.125 us
    // - Prescaler  = F_CPU / 64
    // - P_TIMER    = 0.125 us * 64     = 8 us
    // - Clocks     = 1 ms / 8 us       = 125
    // - OCRA       = Clocks - 1        = 124
    //
    // Therefore we have:
    //
    // - Waveform generation mode set to CTC
    // - Clock prescaler set to F_CPU / 64 (F_CPU = 8 MHz)
    // - Compare value set to 124
    //
    TCCR0A = bitmask1( WGM01 );
    TCCR0B = bitmask2( CS00, CS01 );
    OCR0A = 124;

    // Enable interrupt for timer 0
    TIMSK0 = bitmask1( OCIE0A );

}   /* sys_init() */


bool sys_intrpt_enabled( void )
{
    return( is_bit_set( SREG, SREG_I ) );

}   /* sys_intrpt_enabled() */


void sys_set_intrpt_enabled( bool enabled )
{
    if( enabled )   sei();
    else            cli();

}   /* sys_set_intrpt_enabled() */


event_field_t sys_wait( void )
{
    // Wait until an interrupt sets a pending event
    while( s_pending_events == 0 )
        sleep_until_interrupt();

    // Return and reset the pending event
    event_field_t pending_events = s_pending_events;
    s_pending_events = 0;
    return( pending_events );

}   /* sys_wait() */


static void sleep_until_interrupt( void )
{
    set_sleep_mode( SLEEP_MODE_IDLE );
    sys_sei();
    sleep_mode();
    sys_cli();

}   /* sleep_until_interrupt() */


ISR( TIMER0_COMPA_vect )
{
    s_tick++;
    sys_enqueue_event( EVENT_TICK );

}   /* ISR( TIMER0_COMPA_vect ) */
