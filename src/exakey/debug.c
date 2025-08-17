/**
 * @file    debug.c
 * @brief   Implementation for debug utilities.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-17
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>

#include <avr/io.h>
#include <util/delay.h>

#include "debug.h"
#include "utility.h"

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

void fail( void )
{
    // Flash the status LED at an irritating rate
    set_bit( DDRD, 6 );
    while( true )
    {
        toggle_bit( PORTD, 6 );
        _delay_ms( 25 );
    }

}   /* fail() */
