/**
 * @file    sys.c
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

#include "debug.h"
#include "sys.h"
#include "types.h"
#include "utility.h"

/* --------------------------------------------------- VARIABLES ---------------------------------------------------- */

// TODO: Make pending event into a queue or bitfield

static event_t s_event = EVENT_NONE;
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

void sys_init( void )
{
    // Clear interrupts
    sys_cli();

    //
    // Initialize timer 0 for main system tick at 1 millisecond period
    //
    // - Waveform generation mode set to CTC
    // - Clock prescaler set to F_CPU / 64 (F_CPU = 8 MHz)
    // - Compare value set to 125
    //
    // This leads to the following values;
    //
    // - P_CPU      = 1 / 8 MHz         = 0.125 us
    // - P_TIMER    = 0.125 us * 64     = 8 us
    // - OCRA       = 1 ms / 8 us       = 125
    //
    TCCR0A = bitmask1( WGM01 );
    TCCR0B = bitmask2( CS00, CS01 );
    OCR0A = 125;

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


void sys_set_pending_event( event_t event )
{
    validate_event( event );
    s_event = event;

}   /* sys_set_pending_event() */


tick_t sys_tick( void )
{
    bool int_en = sys_intrpt_enabled();
    sys_cli();
    tick_t ret = s_tick;
    sys_set_intrpt_enabled( int_en );

    return( s_tick );

}   /* sys_tick() */


event_t sys_wait( void )
{
    // Wait until an interrupt sets a pending event
    while( s_event == EVENT_NONE )
        sleep_until_interrupt();

    // Return and reset the pending event
    validate_event( s_event );
    event_t event = s_event;
    s_event = EVENT_NONE;
    return( event );

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
    sys_set_pending_event( EVENT_TICK );

}   /* ISR( TIMER0_COMPA_vect ) */
