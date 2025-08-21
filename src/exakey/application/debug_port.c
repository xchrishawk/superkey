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
#include "application/debug_port.h"
#include "core/config.h"
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
 * @def     INVALID_COMMAND_PREFIX
 * @brief   Prefix for replies indicating an invalid command.
 */
#define INVALID_COMMAND_PREFIX  "Invalid command: "

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
 * @fn      exec_command_help( char const * command )
 * @brief   Executes the `help` command.
 */
static void exec_command_help( char const * command );

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
            debug_port_print( INVALID_COMMAND_PREFIX "No data." NEWLINE_STR );
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
        debug_port_print( INVALID_COMMAND_PREFIX "Too long." NEWLINE_STR );
        s_rx_count = 0;
    }

    // Otherwise, just keep receiving data

}   /* evaluate_rx_buf() */


static void exec_command( char const * command )
{
    // Handle known commands
    if( ! strncasecmp( command, "help", 4 ) )
        exec_command_help( command );
    else if( ! strncasecmp( command, "tick", 4 ) )
        exec_command_tick( command );
    else if( ! strncasecmp( command, "version", 7 ) )
        exec_command_version( command );

    // Handle unrecognized commands
    else
    {
        debug_port_print( INVALID_COMMAND_PREFIX );
        debug_port_print( command );
        debug_port_print( NEWLINE_STR );
    }

}   /* exec_command() */


static void exec_command_help( char const * command )
{
    ( void )command;
    debug_port_print( "Not implemented yet." NEWLINE_STR );

}   /* exec_command_help() */


static void exec_command_tick( char const * command )
{
    ( void )command;
    debug_port_printf( "Tick: %lu" NEWLINE_STR, sys_tick() );

}   /* exec_command_tick() */


static void exec_command_version( char const * command )
{
    ( void )command;
    debug_port_print( "Not implemented yet." NEWLINE_STR );

}   /* exec_command_version() */
