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

#include "debug.h"
#include "gpio.h"
#include "input.h"
#include "led.h"
#include "sys.h"
#include "types.h"
#include "utility.h"

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      handle_input_state( void )
 * @brief   Handles the `EVENT_INPUT_STATE` event.
 */
static void handle_input_state( void );

/**
 * @fn      handle_tick( void )
 * @brief   Handles the `EVENT_TICK` event.
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

/**
 * @fn      test( void )
 * @brief   Test / prototyping function run after `init()`.
 */
static void test( void );

/**
 * @fn      update_keyer( void )
 * @brief   Updates the keyer based on current input state.
 */
static void update_keyer( void );

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

int main( void )
{
    init();
    test();
    main_loop();

}   /* main() */


static void handle_input_state( void )
{
    update_keyer();

}   /* handle_input_state() */


static void handle_tick( void )
{
    // Handle events which happen every tick
    update_keyer();

    // Handle 1 second events
    static uint16_t count_1s = ( 1 * TICKS_PER_SEC );
    if( --count_1s == 0 )
    {
        periodic_1s();
        count_1s = ( 1 * TICKS_PER_SEC );
    }

}   /* handle_tick() */


static void init( void )
{
    // Initialize all system modules
    sys_init();
    gpio_init();
    led_init();
    input_init();

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

        case EVENT_INPUT_STATE:
            handle_input_state();
            break;

        default:
            // Unknown event type??
            fail();
            break;
        }
    }

}   /* main_loop() */


static void periodic_1s( void )
{
    led_toggle( LED_STATUS );

}   /* periodic_1s() */


static void test( void )
{

}   /* test() */


static void update_keyer( void )
{
    led_set( LED_KEY, input_get( INPUT_STRAIGHT_KEY ) );

}   /* update_keyer() */
