/**
 * @file    src/exakey/application/debug_port.c
 * @brief   Implementation for the debug port module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-20
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "application/buzzer.h"
#include "application/config.h"
#include "application/debug_port.h"
#include "application/keyer.h"
#include "core/sys.h"
#include "drivers/usart.h"
#include "utility/constants.h"
#include "utility/types.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

/**
 * @def     PRINTF_BUF_SIZE
 * @brief   Size of the local buffer used by `debug_port_printf()`.
 * @note    This buffer is statically allocated and always takes up space.
 */
#define PRINTF_BUF_SIZE         256

/**
 * @def     RX_BUF_SIZE
 * @brief   Size of the command receive buffer, in bytes.
 */
#define RX_BUF_SIZE             64

/**
 * @def     TERMINATOR_CHAR
 * @brief   Character which terminates a command from the client.
 */
#define TERMINATOR_CHAR         CARRIAGE_RETURN_CHAR

/* --------------------------------------------------- VARIABLES ---------------------------------------------------- */

static char s_rx_buf[ RX_BUF_SIZE ];
static size_t s_rx_count = 0;

/* ----------------------------------------------------- MACROS ----------------------------------------------------- */

/**
 * @def     rx_buf_avail()
 * @brief   Returns the number of bytes available in the RX buffer.
 */
#define rx_buf_avail()                                                                  \
    ( RX_BUF_SIZE - s_rx_count - 1 )

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      evaluate_rx_buf( void )
 * @brief   Evaluates the RX buffer for a command.
 */
static void evaluate_rx_buf( void );

/**
 * @fn      exec_command( char const * command )
 * @brief   Executes the specified command, including sending any required response.
 */
static void exec_command( char const * command );

/**
 * @fn      exec_command_buzzer( char const * command )
 * @brief   Executes the `buzzer` command.
 */
static void exec_command_buzzer( char const * command );

/**
 * @fn      exec_command_config( char const * command )
 * @brief   Executes the `config` command.
 */
static void exec_command_config( char const * command );

/**
 * @fn      exec_command_help( char const * command )
 * @brief   Executes the `help` command.
 */
static void exec_command_help( char const * command );

/**
 * @fn      exec_command_panic( char const * command )
 * @brief   Executes the `panic` command.
 */
static void exec_command_panic( char const * command );

/**
 * @fn      exec_command_tick( char const * command )
 * @brief   Executes the `tick` command.
 */
static void exec_command_tick( char const * command );

/**
 * @fn      exec_command_version( char const * command )
 * @brief   Executes the `version` command.
 */
static void exec_command_version( char const * command );

/**
 * @fn      exec_command_wpm( char const * command )
 * @brief   Executes the `wpm` command.
 */
static void exec_command_wpm( char const * command );

/**
 * @fn      print_invalid_command( char const * )
 * @brief   Sends an "Invalid command" message for the specified command.
 */
static void print_invalid_command( char const * command );

/**
 * @fn      read_long( char const *, long * )
 * @brief   Attempts to read a signed long integer from the specified buffer.
 * @returns `true` if reading the value succeeded and all data was consumed.
 */
static bool read_long( char const * buf, long * value ) FUNC_MAY_BE_UNUSED;

/**
 * @fn      read_ulong( char const *, unsigned long * )
 * @brief   Attempts to read an unsigned long integer from the specified buffer.
 * @returns `true` if reading the value succeeded and all data was consumed.
 */
static bool read_ulong( char const * buf, unsigned long * value ) FUNC_MAY_BE_UNUSED;

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

void debug_port_init( void )
{
    // Initialize local state
    s_rx_count = 0;

    // Initialize USART
    usart_init( DEBUG_PORT_USART,
                true,
                true,
                USART_DATA_BITS_8,
                USART_STOP_BITS_1,
                USART_PARITY_DISABLED );

}   /* debug_port_init() */


bool debug_port_print( char const * str )
{
    return( usart_tx_str( DEBUG_PORT_USART, str ) );

}   /* debug_port_print() */


bool debug_port_printf( char const * fmt, ... )
{
    // Local static buffer to print into
    static char buf[ PRINTF_BUF_SIZE ];

    // Call vsnprintf. Note that the returned value (count) is the number of characters that *would* have been written
    // if the buffer had infinite length. Therefore we can use it to check if we ran out of space.
    va_list args;
    va_start( args, fmt );
    int count = vsnprintf( buf, PRINTF_BUF_SIZE, fmt, args );
    va_end( args );

    // If there was not enough space for the entire string, bail out
    if( count > PRINTF_BUF_SIZE )
        return false;

    // Forward the string on to the print function
    return( debug_port_print( buf ) );

}   /* debug_port_printf() */


void debug_port_tick( tick_t tick )
{
    // No-op currently
    ( void )tick;

}   /* debug_port_tick() */


void debug_port_usart_rx( void )
{
    // Receive single characters and evaluate as long as possible
    while( rx_buf_avail() != 0 &&
           usart_rx( DEBUG_PORT_USART, ( byte_t * )( s_rx_buf + s_rx_count ), 1 ) )
    {
        s_rx_count++;
        evaluate_rx_buf();
    }

}   /* debug_port_usart_rx() */


static void evaluate_rx_buf( void )
{
    // If we receive the terminating character...
    if( s_rx_buf[ s_rx_count - 1 ] == TERMINATOR_CHAR )
    {
        // Ensure we actually received some data
        if( s_rx_count == 1 )
        {
            print_invalid_command( "No data." );
            s_rx_count = 0;
            return;
        }

        // Replace the terminator with the null character to terminate the string, then process the command
        s_rx_buf[ s_rx_count - 1 ] = NULL_CHAR;
        exec_command( s_rx_buf );
        s_rx_count = 0;
    }

    // Have we run out of space?
    else if( rx_buf_avail() == 0 )
    {
        print_invalid_command( "Too long." );
        s_rx_count = 0;
    }

    // Otherwise, just keep receiving data

}   /* evaluate_rx_buf() */


static void exec_command( char const * command )
{
    // Handle known commands, starting with panic because I'm paranoid
    if( ! strncasecmp( command, "stop", 4 ) || ! strncasecmp( command, "panic", 5 ) )
        exec_command_panic( command );
    else if( ! strncasecmp( command, "buzzer", 6 ) )
        exec_command_buzzer( command );
    else if( ! strncasecmp( command, "config", 6 ) )
        exec_command_config( command );
    else if( ! strncasecmp( command, "help", 4 ) )
        exec_command_help( command );
    else if( ! strncasecmp( command, "tick", 4 ) )
        exec_command_tick( command );
    else if( ! strncasecmp( command, "version", 7 ) )
        exec_command_version( command );
    else if( ! strncasecmp( command, "wpm", 3 ) )
        exec_command_wpm( command );

    // Handle unrecognized commands
    else
        print_invalid_command( command );

}   /* exec_command() */


static void exec_command_buzzer( char const * command )
{
#define PREFIX_LEN 6
#define FREQUENCY_LEN 11

    if( command[ PREFIX_LEN ] == NULL_CHAR )
    {
        // No subcommand - interpret as a status request. no action required
    }
    else if( ! strcasecmp( command + PREFIX_LEN, " enable" ) )
    {
        // Turn buzzer on
        buzzer_set_enabled( true );
    }
    else if( ! strcasecmp( command + PREFIX_LEN, " disable" ) )
    {
        // Turn buzzer oiff
        buzzer_set_enabled( false );
    }
    else if( ! strncasecmp( command + PREFIX_LEN, " frequency ", FREQUENCY_LEN ) )
    {
        unsigned long freq;
        if( read_ulong( command + PREFIX_LEN + FREQUENCY_LEN, & freq ) &&
            freq >= BUZZER_MINIMUM_FREQUENCY &&
            freq <= BUZZER_MAXIMUM_FREQUENCY )
        {
            buzzer_set_frequency( ( buzzer_freq_t )freq );
        }
        else
        {
            debug_port_print( "Invalid frequency: " );
            debug_port_print( command + PREFIX_LEN + FREQUENCY_LEN );
            debug_port_print( ". Must be between " stringize_value( BUZZER_MINIMUM_FREQUENCY )
                              " and " stringize_value( BUZZER_MAXIMUM_FREQUENCY ) " Hz." NEWLINE_STR );
            return;
        }
    }
    else
    {
        // Unrecognized subcommand?
        print_invalid_command( command );
        return;
    }

    // Send buzzer status
    debug_port_printf( "Buzzer: %s (%u Hz)" NEWLINE_STR,
                       buzzer_get_enabled() ? "Enabled" : "Disabled",
                       buzzer_get_frequency() );

#undef PREFIX_LEN
#undef FREQUENCY_LEN

}   /* exec_command_buzzer() */


static void exec_command_config( char const * command )
{
    if( ! strcasecmp( command + 6, " default" ) )
    {
        // Restore default configuration
        config_t config;
        config_default( & config );
        config_set( & config );

        // Send response
        debug_port_print( "Default configuration restored." NEWLINE_STR );
    }
    else
        print_invalid_command( command );

}   /* exec_command_config() */


static void exec_command_help( char const * command )
{
    ( void )command;
    debug_port_print( "Not implemented yet." NEWLINE_STR );

}   /* exec_command_help() */


static void exec_command_panic( char const * command )
{
    ( void )command;
    keyer_panic();
    debug_port_print( "Stopped keyer." NEWLINE_STR );

}   /* exec_command_panic() */


static void exec_command_tick( char const * command )
{
    ( void )command;
    debug_port_printf( "Tick: %lu" NEWLINE_STR, sys_get_tick() );

}   /* exec_command_tick() */


static void exec_command_version( char const * command )
{
    ( void )command;
    debug_port_print( "Not implemented yet." NEWLINE_STR );

}   /* exec_command_version() */


static void exec_command_wpm( char const * command )
{
#define PREFIX_LEN 3

    unsigned long wpm;
    if( command[ PREFIX_LEN ] == NULL_CHAR )
    {
        // No subcommand - interpret as a status request. no action required
    }
    else if( command[ PREFIX_LEN ] == ' ' &&
             read_ulong( command + PREFIX_LEN + 1, & wpm ) )
    {
        unsigned long wpm;
        if( read_ulong( command + PREFIX_LEN + 1, & wpm ) &&
            wpm >= WPM_MINIMUM &&
            wpm <= WPM_MAXIMUM )
        {
            wpm_set( ( wpm_t )wpm );
        }
        else
        {
            debug_port_print( "Invalid WPM: " );
            debug_port_print( command + PREFIX_LEN + 1 );
            debug_port_print( ". Must be between " stringize_value( WPM_MINIMUM )
                              " and " stringize_value( WPM_MAXIMUM ) "." NEWLINE_STR );
            return;
        }
    }
    else
    {
        print_invalid_command( command );
        return;
    }

    // Print status info
    tick_t dot, dash;
    wpm_ticks( wpm_get(), & dot, & dash, NULL, NULL, NULL );
    debug_port_printf( "WPM: %u (%u.%u wpm - dot %lu ms, dash %lu ms)" NEWLINE_STR,
                       wpm_get(),
                       wpm_get() / 10,
                       wpm_get() % 10,
                       dot / TICKS_PER_MSEC,
                       dash / TICKS_PER_MSEC );

#undef PREFIX_LEN

}   /* exec_command_wpm() */


static void print_invalid_command( char const * command )
{
    debug_port_print( "Invalid command: " );
    debug_port_print( command );
    debug_port_print( NEWLINE_STR );

}   /* print_invalid_command() */


static bool read_long( char const * buf, long * value )
{
    char * end;
    long parsed_value = strtol( buf, & end, 10 );
    if( end == buf || * end != NULL_CHAR )
        return( false );

    *value = parsed_value;
    return( true );

}   /* read_long() */


static bool read_ulong( char const * buf, unsigned long * value )
{
    char * end;
    unsigned long parsed_value = strtoul( buf, & end, 10 );
    if( end == buf || * end != NULL_CHAR )
        return( false );

    *value = parsed_value;
    return( true );

}   /* read_ulong() */
