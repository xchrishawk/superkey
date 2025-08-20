/**
 * @file    src/exakey/utility/debug.c
 * @brief   Implementation for debug utilities.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-17
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>

#include <avr/io.h>
#include <util/delay.h>

#include "application/keyer.h"
#include "application/led.h"
#include "core/sys.h"
#include "utility/debug.h"
#include "utility/utility.h"

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

void fail( void )
{
    fail_code( 0 );

}   /* fail() */


void fail_code( uint8_t code )
{
#define ANNOYING_DELAY 37
#define FLASH_DELAY 150
#define NUMBER_DELAY 1000

    // Disable interrupts
    sys_cli();

    // Stop keying, if required
    keyer_panic();

    if( code != 0 )
    {
        // Flash the provided status code
        while( true )
        {
            for( uint8_t idx = 0; idx < code; idx++ )
            {
                led_set_on( LED_STATUS, true );
                _delay_ms( FLASH_DELAY );
                led_set_on( LED_STATUS, false );
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
            led_toggle_on( LED_STATUS );
            _delay_ms( ANNOYING_DELAY );
        }
    }

#undef ANNOYING_DELAY
#undef FLASH_DELAY
#undef NUMBER_DELAY

}   /* fail_code() */
