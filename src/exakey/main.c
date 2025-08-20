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

#include "buzzer.h"
#include "debug.h"
#include "gpio.h"
#include "input.h"
#include "keyer.h"
#include "led.h"
#include "sys.h"
#include "types.h"
#include "usart.h"
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
 * @fn      handle_usart_rx_complete( usart_t )
 * @brief   Handles the `EVENT_USART_n_RX_COMPLETE` event for the specified USART.
 */
static void handle_usart_rx_complete( usart_t usart );

/**
 * @fn      handle_usart_tx_complete( usart_t )
 * @brief   Handles the `EVENT_USART_n_TX_COMPLETE` event for the specified USART.
 */
static void handle_usart_tx_complete( usart_t usart );

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
 * @fn      periodic_1ms( void )
 * @brief   Performs periodic processing which should be executed every 1 millisecond.
 */
static void periodic_1ms( tick_t tick );

/**
 * @fn      periodic_1s( void )
 * @brief   Performs periodic processing which should be executed every 1 second.
 */
static void periodic_1s( tick_t tick );

/**
 * @fn      test( void )
 * @brief   Test / prototyping function run after `init()`.
 */
static void test( void );

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

int main( void )
{
    init();
    test();
    main_loop();

}   /* main() */


static void handle_input_state( void )
{
    keyer_update( sys_tick() );

}   /* handle_input_state() */


static void handle_tick( void )
{
    // Get system tick count
    tick_t tick = sys_tick();

    // Handle 1 millisecond events
    static uint16_t count_1ms = ( 1 * TICKS_PER_MSEC );
    if( --count_1ms == 0 )
    {
        periodic_1ms( tick );
        count_1ms = ( 1 * TICKS_PER_MSEC );
    }

    // Handle 1 second events
    static uint16_t count_1s = ( 1 * TICKS_PER_SEC );
    if( --count_1s == 0 )
    {
        periodic_1s( tick );
        count_1s = ( 1 * TICKS_PER_SEC );
    }

}   /* handle_tick() */


static void handle_usart_rx_complete( usart_t usart )
{

}   /* handle_usart_rx_complete() */


static void handle_usart_tx_complete( usart_t usart )
{

}   /* handle_usart_tx_complete() */


static void init( void )
{
    // Initialize all system modules
    sys_init();
    gpio_init();
    led_init();
    input_init();
    buzzer_init();
    keyer_init();

}   /* init() */


static void main_loop( void )
{
    while( true )
    {
        // Wait for next system event
        event_field_t events = sys_wait();
        if( is_bit_set( events, EVENT_TICK ) )
            handle_tick();
        if( is_bit_set( events, EVENT_INPUT_STATE ) )
            handle_input_state();
        if( is_bit_set( events, EVENT_USART_0_RX_COMPLETE ) )
            handle_usart_rx_complete( USART_0 );
        if( is_bit_set( events, EVENT_USART_0_TX_COMPLETE ) )
            handle_usart_tx_complete( USART_0 );
        if( is_bit_set( events, EVENT_USART_1_RX_COMPLETE ) )
            handle_usart_rx_complete( USART_1 );
        if( is_bit_set( events, EVENT_USART_1_TX_COMPLETE ) )
            handle_usart_tx_complete( USART_1 );
    }

}   /* main_loop() */


static void periodic_1ms( tick_t tick )
{
    keyer_update( tick );

}   /* periodic_1ms() */


static void periodic_1s( tick_t tick )
{
    led_toggle_on( LED_STATUS );

}   /* periodic_1s() */


static void test( void )
{
    usart_init( USART_0, true, true, USART_DATA_BITS_8, USART_STOP_BITS_1, USART_PARITY_DISABLED );
    usart_init( USART_1, false, true, USART_DATA_BITS_8, USART_STOP_BITS_1, USART_PARITY_DISABLED );

}   /* test() */
