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

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

int main( void )
{
    DDRD = ( 1 << 6 );

    while( true )
    {
        PORTD = ( 1 << 6 );
        _delay_ms( 1000 );
        PORTD = 0;
        _delay_ms( 1000 );
    }

}   /* main() */
