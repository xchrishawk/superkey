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
#include "gpio.h"
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

    gpio_set_dir( GPIO_PIN_D6, GPIO_DIR_OUT );

    if( code != 0 )
    {
        // Flash the provided status code
        while( true )
        {
            for( uint8_t idx = 0; idx < code; idx++ )
            {
                gpio_set_state( GPIO_PIN_D6, GPIO_STATE_HIGH );
                _delay_ms( FLASH_DELAY );
                gpio_set_state( GPIO_PIN_D6, GPIO_STATE_LOW );
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
            gpio_toggle_state( GPIO_PIN_D6 );
            _delay_ms( FLASH_DELAY );
        }
    }

#undef FLASH_DELAY
#undef NUMBER_DELAY

}   /* fail_code() */
