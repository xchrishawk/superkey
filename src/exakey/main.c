/**
 * @file    main.c
 * @brief   Main entry point for the application.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-16
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include <avr/io.h>
#include <util/delay.h>

#include "sys.h"
#include "types.h"
#include "utility.h"

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      handle_tick( void )
 * @brief   Handles the EVENT_TICK event.
 */
static void handle_tick( void );

/**
 * @fn      init( void )
 * @brief   Initializes the system after startup.
 */
static void init( void );

/**
 * @fn      main_loop( void )
 * @brief   Runs the main loop of the application.
 */
static void main_loop( void );

/**
 * @fn      periodic_1s( void )
 * @brief   Performs periodic processing which should be executed every 1 second.
 */
static void periodic_1s( void );

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

int main( void )
{
    init();
    main_loop();

}   /* main() */


static void handle_tick( void )
{
    // Handle 1 second events
    static uint16_t const COUNT_1S = 1000;
    static uint16_t count_1s = COUNT_1S;
    if( --count_1s == 0 )
    {
        periodic_1s();
        count_1s = COUNT_1S;
    }

}   /* handle_tick() */


static void init( void )
{
    // Initialize system module
    sys_init();

    // TEMP - init GPIO
    set_bit( DDRD, DD6 );

}   /* init() */


static void main_loop( void )
{
    while( true )
    {
        // Wait for next system event
        event_t event = sys_wait();
        switch( event )
        {
        case EVENT_TICK:
            handle_tick();
            break;

        default:
            // Unknown event type??
            assert( false );
            break;
        }
    }

}   /* main_loop() */


static void periodic_1s( void )
{
    toggle_bit( PORTD, PORTD6 );

}   /* periodic_1s() */
