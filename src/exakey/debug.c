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
#include "led.h"
#include "utility.h"

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

void fail( void )
{
    fail_code( 0 );

}   /* fail() */


void fail_code( uint8_t code )
{
#define FLASH_DELAY 150
#define NUMBER_DELAY 1000

    if( code != 0 )
    {
        // Flash the provided status code
        while( true )
        {
            for( uint8_t idx = 0; idx < code; idx++ )
            {
                led_set( LED_STATUS, true );
                _delay_ms( FLASH_DELAY );
                led_set( LED_STATUS, false );
                _delay_ms( FLASH_DELAY );
            }
            _delay_ms( NUMBER_DELAY );
        }
    }
    else
    {
        // No code provided - just flash continuously at an irritating rate
        while( true )
        {
            led_toggle( LED_STATUS );
            _delay_ms( FLASH_DELAY );
        }
    }

#undef FLASH_DELAY
#undef NUMBER_DELAY

}   /* fail_code() */
