/**
 * @file    src/main/core/main.c
 * @brief   Main entry point for the application.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-16
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 *
 * @details
 * This module is the main entry point of the application. It is responsible for initializing the device, and
 * coordinating the main loop.
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include <avr/io.h>
#include <util/delay.h>

#include "application/buzzer.h"
#include "application/config.h"
#include "application/debug_port.h"
#include "application/intf_port.h"
#include "application/io.h"
#include "application/keyer.h"
#include "application/led.h"
#include "application/quick_msg.h"
#include "application/storage.h"
#include "core/sys.h"
#include "drivers/eeprom.h"
#include "drivers/gpio.h"
#include "drivers/usart.h"
#include "utility/debug.h"
#include "utility/types.h"
#include "utility/utility.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

/**
 * @def     TICKS_1_MS
 * @brief   Number of ticks for 1 millisecond tasks.
 */
#define TICKS_1_MS      ( 1 * TICKS_PER_MSEC )

/**
 * @def     TICKS_50_MS
 * @brief   Number of ticks for 50 millisecond tasks.
 */
#define TICKS_50_MS     ( 50 * TICKS_PER_MSEC )

/**
 * @def     TICKS_1_S
 * @brief   Number of ticks for 1 second tasks.
 */
#define TICKS_1_S       ( 1 * TICKS_PER_SEC )

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      main( void )
 * @brief   Main entry point for the application.
 */
int main( void );

/**
 * @fn      handle_io_state( void )
 * @brief   Handles the `EVENT_IO_STATE` event.
 */
static void handle_io_state( void );

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
 * @fn      periodic_1ms( tick_t )
 * @brief   Performs periodic processing which should be executed every 1 millisecond (i.e., 1000 Hz).
 */
static void periodic_1ms( tick_t tick );

/**
 * @fn      periodic_50ms( tick_t )
 * @brief   Performs periodic processing which should be executed every 50 milliseconds (i.e., 20 Hz).
 */
static void periodic_50ms( tick_t tick );

/**
 * @fn      periodic_1s( tick_t )
 * @brief   Performs periodic processing which should be executed every 1 second (i.e., 1 Hz).
 */
static void periodic_1s( tick_t tick );

/**
 * @fn      startup_display( void )
 * @brief   Performs a short display at startup.
 */
static void startup_display( void );

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
    main_loop(); // never returns

}   /* main() */


static void handle_io_state( void )
{
    // An input state changed - immediately update the keyer
    keyer_tick( sys_get_tick() );

}   /* handle_input_state() */


static void handle_tick( void )
{
    // Get system tick count
    tick_t tick = sys_get_tick();

    // Handle 1 millisecond (1000 Hz) events
    _Static_assert( TICKS_1_MS < UINT8_MAX, "Tick counter too small!" );
    static uint8_t count_1ms = TICKS_1_MS;
    if( --count_1ms == 0 )
    {
        periodic_1ms( tick );
        count_1ms = TICKS_1_MS;
    }

    // Handle 50 millisecond (20 Hz) events
    _Static_assert( TICKS_50_MS < UINT8_MAX, "Tick counter too small!" );
    static uint8_t count_50ms = TICKS_50_MS;
    if( --count_50ms == 0 )
    {
        periodic_50ms( tick );
        count_50ms = TICKS_50_MS;
    }

    // Handle 1 second (1 Hz) events
    _Static_assert( TICKS_1_S < UINT16_MAX, "Tick counter too small!" );
    static uint16_t count_1s = TICKS_1_S;
    if( --count_1s == 0 )
    {
        periodic_1s( tick );
        count_1s = TICKS_1_S;
    }

}   /* handle_tick() */


static void handle_usart_rx_complete( usart_t usart )
{
    // Notify correct module
    switch( usart )
    {
    case INTF_PORT_USART:
        intf_port_usart_rx();
        break;
    case DEBUG_PORT_USART:
        debug_port_usart_rx();
        break;
    }

}   /* handle_usart_rx_complete() */


static void handle_usart_tx_complete( usart_t usart )
{
    ( void )usart;

}   /* handle_usart_tx_complete() */


static void init( void )
{
    // Initialize all system modules
    sys_init();
    storage_init();
    config_init();
    quick_msg_init();
    led_init();
    io_init();
    buzzer_init();
    keyer_init();
    intf_port_init();
    debug_port_init();

    // Flash LEDs to indicate successful startup
    startup_display();

}   /* init() */


static void main_loop( void )
{
    while( true )
    {
        // Wait for next system event
        event_field_t events = sys_wait();
        if( is_bit_set( events, EVENT_TICK ) )
            handle_tick();
        if( is_bit_set( events, EVENT_IO_STATE ) )
            handle_io_state();
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
    // Periodic processing for modules with 1000 Hz tick rates
    keyer_tick( tick );

}   /* periodic_1ms() */


static void periodic_50ms( tick_t tick )
{
    // Periodic processing for modules with 20 Hz tick rates
    buzzer_tick( tick );
    intf_port_tick( tick );
    io_tick( tick );
    led_tick( tick );

}   /* periodic_50ms() */


static void periodic_1s( tick_t tick )
{
    // Periodic processing for modules with 1 Hz tick rates
    config_tick( tick );
    quick_msg_tick( tick );
    debug_port_tick( tick );

    // Toggle the status LED. We're still alive!
    led_toggle_on( LED_STATUS );

}   /* periodic_1s() */


static void startup_display( void )
{
    // Flash an S (for Superkey) at 20 WPM
    for( uint8_t idx = 0; idx < 3; idx++ )
    {
        #define DELAY_MS 60
        led_set_on( LED_STATUS, true );
        _delay_ms( DELAY_MS );
        led_set_on( LED_STATUS, false );
        _delay_ms( DELAY_MS );
        #undef DELAY_MS
    }

    // Leave the status LED on to indicate successful startup
    led_set_on( LED_STATUS, true );

}   /* startup_display() */


static void test( void )
{

}   /* test() */
