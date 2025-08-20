/**
 * @file    src/exakey/core/sys.c
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

/* --------------------------------------------------- VARIABLES ---------------------------------------------------- */

// TODO: Make pending event into a queue or bitfield

static event_field_t s_pending_events = 0;
static tick_t s_tick = 0;

/* ----------------------------------------------------- MACROS ----------------------------------------------------- */

// Validation macros
#define validate_event( _event )                                                        \
    assert_always( ( _event ) < EVENT_COUNT )

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      sleep_until_interrupt( void )
 * @brief   Sleeps the unit until the next interrupt.
 */
static void sleep_until_interrupt( void );

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

void sys_enqueue_event( event_t event )
{
    validate_event( event );

    set_bit( s_pending_events, event );

}   /* sys_enqueue_event() */


void sys_init( void )
{
    // Clear interrupts
    sys_cli();

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


tick_t sys_tick( void )
{
    bool int_en = sys_intrpt_enabled();
    sys_cli();
    tick_t ret = s_tick;
    sys_set_intrpt_enabled( int_en );

    return( ret );

}   /* sys_tick() */


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
