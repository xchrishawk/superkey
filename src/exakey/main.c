/**
 * @file    main.c
 * @brief   Main entry point for the application.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-16
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>

#include <avr/io.h>
#include <util/delay.h>

#include "types.h"
#include "utility.h"

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

int main( void )
{
    register_t dir = register_addr( DDRD );
    register_t state = register_addr( PORTD );

    set_bit( * dir, 6 );

    while( true )
    {
        toggle_bit( * state, 6 );
        _delay_ms( 1000 );
    }

}   /* main() */
