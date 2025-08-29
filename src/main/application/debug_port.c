/**
 * @file    src/main/application/debug_port.c
 * @brief   Implementation for the debug port module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-20
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

// Unconditionally include these so the compiler doesn't complain about an empty translation unit
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined( _FEATURE_ENABLE_DEBUG_PORT ) && _FEATURE_ENABLE_DEBUG_PORT

#include "application/buzzer.h"
#include "application/config.h"
#include "application/debug_port.h"
#include "application/keyer.h"
#include "application/strings.h"
#include "core/sys.h"
#include "core/version.h"
#include "drivers/eeprom.h"
#include "drivers/usart.h"
#include "utility/constants.h"
#include "utility/crc.h"
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
#define RX_BUF_SIZE             256

/**
 * @def     TOKEN_MAX_LEN
 * @brief   Maximum length of a parsed token.
 */
#define TOKEN_MAX_LEN           32

/**
 * @def     TOKEN_FMT_STR
 * @brief   Token format string for `sscanf`.
 */
#define TOKEN_FMT_STR           "%31s"

/**
 * @def     CMD_STR_BUZZER
 * @brief   The string for the `buzzer` command.
 */
#define CMD_STR_BUZZER          "Buzzer"

/**
 * @def     CMD_STR_CONFIG
 * @brief   The string for the `config` command.
 */
#define CMD_STR_CONFIG          "Config"

/**
 * @def     CMD_STR_EEPROM
 * @brief   The string for the `eeprom` command.
 */
#define CMD_STR_EEPROM          "EEPROM"

/**
 * @def     CMD_STR_HELP
 * @brief   The string for the `help` command.
 */
#define CMD_STR_HELP            "Help"

/**
 * @def     CMD_STR_IO
 * @brief   The string for the `io` command.
 */
#define CMD_STR_IO              "IO"

/**
 * @def     CMD_STR_KEYER
 * @brief   The string for the `keyer` command.
 */
#define CMD_STR_KEYER           "Keyer"

/**
 * @def     CMD_STR_LED
 * @brief   The string for the `led` command.
 */
#define CMD_STR_LED             "LED"

/**
 * @def     CMD_STR_PANIC
 * @brief   The string for the `panic` command.
 */
#define CMD_STR_PANIC           "Panic"

/**
 * @def     CMD_STR_TICK
 * @brief   The string for the `tick` command.
 */
#define CMD_STR_TICK            "Tick"

/**
 * @def     CMD_STR_VERSION
 * @brief   The string for the `version` command.
 */
#define CMD_STR_VERSION         "Version"

/**
 * @def     CMD_STR_WPM
 * @brief   The string for the `wpm` command.
 */
#define CMD_STR_WPM             "WPM"

/**
 * @def     ENABLE_STR
 * @brief   The token to enable an option.
 */
#define ENABLE_STR              "enable"

/**
 * @def     DISABLE_STR
 * @brief   The token to disable an option.
 */
#define DISABLE_STR             "disable"

/**
 * @def     INVALID_COMMAND_STR
 * @brief   Prefix for invalid command messages.
 */
#define INVALID_COMMAND_STR     "Invalid command: "

/**
 * @def     TERMINATOR_CHAR
 * @brief   Character which terminates a command from the client.
 */
#define TERMINATOR_CHAR         CARRIAGE_RETURN_CHAR

/* --------------------------------------------------- VARIABLES ---------------------------------------------------- */

static char s_rx_buf[ RX_BUF_SIZE ];
static size_t s_rx_count = 0;

bool s_immediate_autokey = false;

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
 * @fn      exec_command( char const * const command )
 * @brief   Executes the specified command, including sending any required response.
 */
static void exec_command( char const * const command );

/**
 * @fn      exec_command_buzzer( char const * const command )
 * @brief   Executes the `buzzer` command.
 */
static void exec_command_buzzer( char const * const command );

/**
 * @fn      exec_command_config( char const * const command )
 * @brief   Executes the `config` command.
 */
static void exec_command_config( char const * const command );

/**
 * @fn      exec_command_eeprom( char const * const command )
 * @brief   Executes the `eeprom` command.
 */
static void exec_command_eeprom( char const * const command );

/**
 * @fn      exec_command_help( char const * const command )
 * @brief   Executes the `help` command.
 */
static void exec_command_help( char const * const command );

/**
 * @fn      exec_command_io( char const * const command )
 * @brief   Executes the `io` command.
 */
static void exec_command_io( char const * const command );

/**
 * @fn      exec_command_keyer( char const * const command )
 * @brief   Executes the `keyer` command.
 */
static void exec_command_keyer( char const * const command );

/**
 * @fn      exec_command_led( char const * const command )
 * @brief   Executes the `led` command.
 */
static void exec_command_led( char const * const command );

/**
 * @fn      exec_command_panic( char const * const command )
 * @brief   Executes the `panic` command.
 */
static void exec_command_panic( char const * const command );

/**
 * @fn      exec_command_tick( char const * const command )
 * @brief   Executes the `tick` command.
 */
static void exec_command_tick( char const * const command );

/**
 * @fn      exec_command_version( char const * const command )
 * @brief   Executes the `version` command.
 */
static void exec_command_version( char const * const command );

/**
 * @fn      exec_command_wpm( char const * const command )
 * @brief   Executes the `wpm` command.
 */
static void exec_command_wpm( char const * const command );

/**
 * @fn      exec_immediate_autokey_mode( void )
 * @brief   Executes the special "immediate autokey" mode.
 */
static void exec_immediate_autokey_mode( void );

/**
 * @fn      print_invalid_command( char const * const )
 * @brief   Sends an "Invalid command" message for the specified command.
 */
static void print_invalid_command( char const * const command );

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
    return( usart_tx_str( DEBUG_PORT_USART, str, USART_WAIT_MODE_NORMAL ) );

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
    // Are we in immediate autokey mode?
    if( s_immediate_autokey )
    {
        exec_immediate_autokey_mode();
        return;
    }

    // If we receive the terminating character...
    if( s_rx_buf[ s_rx_count - 1 ] == TERMINATOR_CHAR )
    {
        // Ensure we actually received some data
        if( s_rx_count == 1 )
        {
            debug_port_print( INVALID_COMMAND_STR "No data." NEWLINE_STR );
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
        debug_port_print( INVALID_COMMAND_STR "Too long." NEWLINE_STR );
        s_rx_count = 0;
    }

    // Otherwise, just keep receiving data

}   /* evaluate_rx_buf() */


static void exec_command( char const * const command )
{
    // Handle known commands, starting with panic because I'm paranoid
    if( string_begins_with( command, CMD_STR_PANIC ) )
        exec_command_panic( command );
    else if( string_begins_with( command, CMD_STR_BUZZER ) )
        exec_command_buzzer( command );
    else if( string_begins_with( command, CMD_STR_CONFIG ) )
        exec_command_config( command );
    else if( string_begins_with( command, CMD_STR_EEPROM ) )
        exec_command_eeprom( command );
    else if( string_begins_with( command, CMD_STR_HELP ) )
        exec_command_help( command );
    else if( string_begins_with( command, CMD_STR_IO ) )
        exec_command_io( command );
    else if( string_begins_with( command, CMD_STR_KEYER ) )
        exec_command_keyer( command );
    else if( string_begins_with( command, CMD_STR_LED ) )
        exec_command_led( command );
    else if( string_begins_with( command, CMD_STR_TICK ) )
        exec_command_tick( command );
    else if( string_begins_with( command, CMD_STR_VERSION ) )
        exec_command_version( command );
    else if( string_begins_with( command, CMD_STR_WPM ) )
        exec_command_wpm( command );

    // Handle unrecognized commands
    else
        print_invalid_command( command );

}   /* exec_command() */


static void exec_command_buzzer( char const * const command )
{
    // Scanned variables
    unsigned int freq;
    int sscanf_count;

    // Parse subcommand
    if( string_equals( command, CMD_STR_BUZZER ) )
    {
        // No subcommand - interpret as a status request. no action required
    }
    else if( string_equals( command, CMD_STR_BUZZER " " ENABLED_STR " " stringize( true ) ) )
    {
        // Turn buzzer on
        buzzer_set_enabled( true );
    }
    else if( string_equals( command, CMD_STR_BUZZER " " ENABLED_STR " " stringize( false ) ) )
    {
        // Turn buzzer oiff
        buzzer_set_enabled( false );
    }
    else if( string_begins_with( command, CMD_STR_BUZZER " frequency " ) &&
             sscanf( command + 17, "%u %n", & freq, & sscanf_count ) == 1 &&
             ( command + 17 )[ sscanf_count ] == NULL_CHAR )
    {
        if( freq < BUZZER_MINIMUM_FREQUENCY || freq > BUZZER_MAXIMUM_FREQUENCY )
        {
            debug_port_printf( "Invalid frequency: \"%u\". Must be between "
                               stringize_value( BUZZER_MINIMUM_FREQUENCY ) " and "
                               stringize_value( BUZZER_MAXIMUM_FREQUENCY ) " Haz. " NEWLINE_STR,
                               freq );
            return;
        }
        buzzer_set_frequency( ( buzzer_freq_t )freq );
    }
    else
    {
        // Unrecognized subcommand?
        print_invalid_command( command );
        return;
    }

    // Send buzzer status
    debug_port_printf( CMD_STR_BUZZER ": %s (%u Hz)" NEWLINE_STR,
                       buzzer_get_enabled() ? ENABLED_STR : DISABLED_STR,
                       buzzer_get_frequency() );

}   /* exec_command_buzzer() */


static void exec_command_config( char const * const command )
{
    // Parse subcommand
    if( string_equals( command, CMD_STR_CONFIG " crc" ) )
    {
        // Print config CRC
        config_t config;
        config_get( & config );
        crc16_t crc = crc_calc_crc16( & config, sizeof( config_t ) );
        debug_port_printf( CMD_STR_CONFIG " CRC16: 0x%04X" NEWLINE_STR, crc );
    }
    else if( string_equals( command, CMD_STR_CONFIG " default" ) )
    {
        // Restore default configuration
        config_t config;
        config_default( & config );
        config_set( & config );

        // Send response
        debug_port_print( CMD_STR_CONFIG ": Default restored." NEWLINE_STR );
        return;
    }
    else if( string_equals( command, CMD_STR_CONFIG " flush" ) )
    {
        // Flush config
        config_flush();
        debug_port_print( CMD_STR_CONFIG ": Flushed." NEWLINE_STR );
    }
    else if( string_equals( command, CMD_STR_CONFIG " size" ) )
    {
        // Report configuration size
        debug_port_printf( CMD_STR_CONFIG " size: %u bytes." NEWLINE_STR, sizeof( config_t ) );
    }
    else
    {
        // Unknown subcommand?
        print_invalid_command( command );
        return;
    }

}   /* exec_command_config() */


static void exec_command_eeprom( char const * const command )
{
    // Scanned variables
    unsigned int addr;
    unsigned int data;
    int sscanf_count;

    // Parse subcommand
    if( string_begins_with( command, CMD_STR_EEPROM " erase_byte " ) &&
        sscanf( command + 18, "%u %n", & addr, & sscanf_count ) == 1 &&
        ( command + 18 )[ sscanf_count ] == NULL_CHAR )
    {
        // Erase byte at address
        if( addr >= EEPROM_COUNT )
        {
            debug_port_printf( "Invalid address: \"%u\". Must be less than "
                               stringize_value( EEPROM_COUNT ) "." NEWLINE_STR,
                               addr );
            return;
        }
        eeprom_erase_byte( ( eeprom_addr_t )addr );
        debug_port_printf( CMD_STR_EEPROM " Erase 0x%04X" NEWLINE_STR, addr );
        return;
    }
    else if( string_begins_with( command, CMD_STR_EEPROM " read_byte " ) &&
             sscanf( command + 17, "%u %n", & addr, & sscanf_count ) == 1 &&
             ( command + 17 )[ sscanf_count ] == NULL_CHAR )
    {
        // Read byte at address
        if( addr >= EEPROM_COUNT )
        {
            debug_port_printf( "Invalid address: \"%u\". Must be less than "
                               stringize_value( EEPROM_COUNT ) "." NEWLINE_STR,
                               addr );
            return;
        }
        byte_t byte;
        eeprom_read( ( eeprom_addr_t )addr, & byte, 1 );
        debug_port_printf( CMD_STR_EEPROM " Read 0x%04X: 0x%02X" NEWLINE_STR, addr, byte );
        return;
    }
    else if( string_begins_with( command, CMD_STR_EEPROM " write_byte " ) &&
             sscanf( command + 18, "%u %u %n", & addr, & data, & sscanf_count ) == 2 &&
             ( command + 18 )[ sscanf_count ] == NULL_CHAR )
    {
        // Write byte at address
        if( addr >= EEPROM_COUNT )
        {
            debug_port_printf( "Invalid address: \"%u\". Must be less than "
                               stringize_value( EEPROM_COUNT ) "." NEWLINE_STR,
                               addr );
            return;
        }
        if( data >= UINT8_MAX )
        {
            debug_port_printf( "Invalid byte: \"%u\". Must be less than "
                               stringize_value( UINT8_MAX ) "." NEWLINE_STR,
                               data );
        }
        byte_t byte = ( byte_t )data;
        eeprom_write( ( eeprom_addr_t )addr, & byte, 1 );
        debug_port_printf( CMD_STR_EEPROM " Write 0x%04X: 0x%02X" NEWLINE_STR, addr, byte );
        return;
    }
    else
    {
        // Unrecognized command?
        print_invalid_command( command );
        return;
    }

}   /* exec_command_eeprom() */


static void exec_command_help( char const * const command )
{
    ( void )command;
    debug_port_print( "Not implemented yet." NEWLINE_STR );

}   /* exec_command_help() */


static void exec_command_io( char const * const command )
{
    char pin_str[ TOKEN_MAX_LEN ];
    char subcommand_str[ TOKEN_MAX_LEN ];
    int sscanf_count;

    // Drop `io` prefix
    if( ! string_begins_with( command, CMD_STR_IO " " ) )
    {
        print_invalid_command( command );
        return;
    }
    char const * c = command + 3;

    // Get tokens
    int token_count = sscanf( c, TOKEN_FMT_STR " " TOKEN_FMT_STR " %n", pin_str, subcommand_str, & sscanf_count );
    if( ( token_count != 1 && token_count != 2 ) || c[ sscanf_count ] != NULL_CHAR )
    {
        print_invalid_command( command );
        return;
    }

    // Parse pin
    io_pin_t pin;
    if( ! string_to_io_pin( pin_str, & pin ) )
    {
        print_invalid_command( command );
        return;
    }

    // Did we get a subcommand?
    if( token_count == 2 )
    {
        // Check subcommand
        io_polarity_t polarity;
        io_type_t type;
        if( string_to_io_polarity( subcommand_str, & polarity ) )
        {
            // Set polarity
            io_set_polarity( pin, polarity );
        }
        else if( string_to_io_type( subcommand_str, & type ) )
        {
            // Set type
            io_set_type( pin, type );
        }
        else if( string_equals( subcommand_str, "disable" ) )
        {
            // Set type to NONE
            io_set_type( pin, IO_TYPE_NONE );
        }
        else
        {
            debug_port_printf( "%s\r\n", subcommand_str );
            debug_port_printf( "%s\r\n", string_from_io_type( IO_TYPE_NONE ) );
            print_invalid_command( command );
            return;
        }
    }

    // Print status info
    debug_port_printf( CMD_STR_IO " %s: %s (%s - %s)" NEWLINE_STR,
                       string_from_io_pin( pin ),
                       string_from_io_state( io_get_state( pin ) ),
                       string_from_io_type( io_get_type( pin ) ),
                       string_from_io_polarity( io_get_polarity( pin ) ) );

}   /* exec_command_io() */


static void exec_command_keyer( char const * const command )
{
    // String table
    static char const * s_paddle_mode_tbl[] =
    {
        stringize( KEYER_PADDLE_MODE_IAMBIC ),
        stringize( KEYER_PADDLE_MODE_ULTIMATIC ),
        stringize( KEYER_PADDLE_MODE_ULTIMATIC_ALTERNATE ),
    };
    _Static_assert( array_count( s_paddle_mode_tbl ) == KEYER_PADDLE_MODE_COUNT, "Invalid string table!" );

    // Parse subcommand
    if( string_equals( command, CMD_STR_KEYER ) )
    {
        // No subcommand - interpret as a status request. no action required
    }
    else if( string_equals( command, CMD_STR_KEYER " immediate" ) )
    {
        // Enter immediate autokey mode
        s_immediate_autokey = true;
        debug_port_print( CMD_STR_KEYER ": Now in immediate autokey mode. Send null character to exit." NEWLINE_STR );
        return;
    }
    else if( string_begins_with( command, CMD_STR_KEYER " key " ) )
    {
        // Add remaining string to autokey buffer
        size_t count = keyer_autokey_str( command + 10 );
        debug_port_printf( CMD_STR_KEYER ": \"%s\" (%u chars queued)" NEWLINE_STR, command + 10, count );
        return;
    }
    else if( string_equals( command, CMD_STR_KEYER " " stringize( KEYER_PADDLE_MODE_IAMBIC ) ) )
    {
        // Set to iambic mode
        keyer_set_paddle_mode( KEYER_PADDLE_MODE_IAMBIC );
    }
    else if( string_equals( command, CMD_STR_KEYER " " stringize( KEYER_PADDLE_MODE_ULTIMATIC ) ) )
    {
        // Set to ultimatic mode
        keyer_set_paddle_mode( KEYER_PADDLE_MODE_ULTIMATIC );
    }
    else if( string_equals( command, CMD_STR_KEYER " " stringize( KEYER_PADDLE_MODE_ULTIMATIC_ALTERNATE ) ) )
    {
        // Set to ultimatic alternate mode
        keyer_set_paddle_mode( KEYER_PADDLE_MODE_ULTIMATIC_ALTERNATE );
    }
    else if( string_equals( command, CMD_STR_KEYER " paddle_invert " ENABLE_STR ) )
    {
        // Set invert paddles to true
        keyer_set_paddle_invert( true );
    }
    else if( string_equals( command, CMD_STR_KEYER " paddle_invert " DISABLE_STR ) )
    {
        // Set invert paddles to false
        keyer_set_paddle_invert( false );
    }
    else
    {
        // Unknown subcommand
        print_invalid_command( command );
        return;
    }

    // Print status info
    debug_port_printf( CMD_STR_KEYER ": %s (%s - %s)" NEWLINE_STR,
                       keyer_get_on() ? ON_STR : OFF_STR,
                       s_paddle_mode_tbl[ keyer_get_paddle_mode() ],
                       keyer_get_paddle_invert() ? "inverted" : "normal" );

}   /* exec_command_keyer() */


static void exec_command_led( char const * const command )
{
    char led_str[ TOKEN_MAX_LEN ];
    char subcommand_str[ TOKEN_MAX_LEN ];
    char value_str[ TOKEN_MAX_LEN ];
    int sscanf_count;

    // Drop `led` prefix
    if( ! string_begins_with( command, CMD_STR_LED " " ) )
    {
        print_invalid_command( command );
        return;
    }
    char const * c = command + 4;

    // Get tokens
    int token_count = sscanf( c, TOKEN_FMT_STR " " TOKEN_FMT_STR " " TOKEN_FMT_STR " %n",
                              led_str, subcommand_str, value_str, & sscanf_count );
    if( ( token_count != 1 && token_count != 3 ) || c[ sscanf_count ] != NULL_CHAR )
    {
        print_invalid_command( command );
        return;
    }

    // Parse led
    led_t led;
    if( ! string_to_led( led_str, & led ) )
    {
        print_invalid_command( command );
        return;
    }

    // Did we get a subcommand?
    if( token_count == 3 )
    {
        // Check subcommand
        bool value;
        if( string_equals( subcommand_str, ENABLED_STR ) && string_to_bool( value_str, & value ) )
        {
            // Enable / disable
            led_set_enabled( led, value );
        }
        else
        {
            print_invalid_command( command );
            return;
        }
    }

    // Print LED status
    debug_port_printf( CMD_STR_LED " %s: %s (%s)" NEWLINE_STR,
                       string_from_led( led ),
                       led_get_on( led ) ? ON_STR : OFF_STR,
                       led_get_enabled( led ) ? ENABLED_STR : DISABLED_STR );

}   /* exec_command_led() */


static void exec_command_panic( char const * const command )
{
    // No arguments are supported
    if( ! string_equals( command, CMD_STR_PANIC ) )
    {
        print_invalid_command( command );
        return;
    }

    keyer_panic();
    debug_port_print( "Stopped keyer." NEWLINE_STR );

}   /* exec_command_panic() */


static void exec_command_tick( char const * const command )
{
    // No arguments are supported
    if( ! string_equals( command, CMD_STR_TICK ) )
    {
        print_invalid_command( command );
        return;
    }

    debug_port_printf( CMD_STR_TICK ": %lu" NEWLINE_STR, sys_get_tick() );

}   /* exec_command_tick() */


static void exec_command_version( char const * const command )
{
    // No arguments are supported
    if( ! string_equals( command, CMD_STR_VERSION ) )
    {
        print_invalid_command( command );
        return;
    }

    version_t version;
    version_get( & version );

    debug_port_printf( "%s %s v%s" NEWLINE_STR "%s %s (%s %s)" NEWLINE_STR,
                       version.product_name,
                       version.build_type,
                       version.version,
                       version.build_date,
                       version.build_time,
                       version.git_branch,
                       version.git_hash_short );

}   /* exec_command_version() */


static void exec_command_wpm( char const * const command )
{
    // Scanned variables
    float wpm;
    char element_str[ TOKEN_MAX_LEN ];
    wpm_element_t element;
    float scale;
    int sscanf_count;

    // Parse subcommand
    if( string_equals( command, CMD_STR_WPM ) )
    {
        // No subcommand - interpret as a status request. No action required.
    }
    else if( string_begins_with( command, CMD_STR_WPM " " ) &&
             sscanf( command + 4, "%f %n", & wpm, & sscanf_count ) == 1 &&
             ( command + 4 )[ sscanf_count ] == NULL_CHAR )
    {
        // Set WPM
        if( wpm < WPM_MINIMUM || wpm > WPM_MAXIMUM )
        {
            debug_port_printf( "Invalid WPM: \"%.1f\". Must be between %.1f and %.1f." NEWLINE_STR,
                               wpm, WPM_MINIMUM, WPM_MAXIMUM );
            return;
        }
        wpm_set( ( wpm_t )wpm );
    }
    else if( string_equals( command, CMD_STR_WPM " scale default" ) )
    {
        // Reset element scales to defaults
        wpm_element_scale_default();
    }
    else if( string_begins_with( command, CMD_STR_WPM " scale " ) &&
             sscanf( command + 10, TOKEN_FMT_STR " %n", element_str, & sscanf_count ) == 1 &&
             ( command + 10 )[ sscanf_count ] == NULL_CHAR &&
             string_to_element( element_str, & element ) )
    {
        // Report scale
        debug_port_printf( CMD_STR_WPM " scale (%s): %.3f" NEWLINE_STR,
                           string_from_element( element ),
                           wpm_get_element_scale( element ) );
        return;
    }
    else if( string_begins_with( command, CMD_STR_WPM " scale " ) &&
             sscanf( command + 10, TOKEN_FMT_STR " %f %n", element_str, & scale, & sscanf_count ) == 2 &&
             ( command + 10 )[ sscanf_count ] == NULL_CHAR &&
             string_to_element( element_str, & element ) )
    {
        // Set scale
        if( scale < WPM_ELEMENT_SCALE_MINIMUM || scale > WPM_ELEMENT_SCALE_MAXIMUM )
        {
            debug_port_printf( "Invalid scale: \"%.1f\". Must be between %.1f and %.1f." NEWLINE_STR,
                               scale, WPM_ELEMENT_SCALE_MINIMUM, WPM_ELEMENT_SCALE_MAXIMUM );
            return;
        }
        wpm_set_element_scale( element, scale );
    }
    else
    {
        // Unrecognized command?
        print_invalid_command( command );
        return;
    }


    // Print status info
    wpm_ticks_t ticks;
    wpm_ticks( wpm_get(), ticks );
    debug_port_printf( CMD_STR_WPM ": %.1f (dot %lu ms, dash %lu ms, space %lu / %lu / %lu ms)" NEWLINE_STR,
                       wpm_get(),
                       ticks[ WPM_ELEMENT_DOT ] / TICKS_PER_MSEC,
                       ticks[ WPM_ELEMENT_DASH ] / TICKS_PER_MSEC,
                       ticks[ WPM_ELEMENT_ELEMENT_SPACE ] / TICKS_PER_MSEC,
                       ticks[ WPM_ELEMENT_LETTER_SPACE ] / TICKS_PER_MSEC,
                       ticks[ WPM_ELEMENT_WORD_SPACE ] / TICKS_PER_MSEC );

}   /* exec_command_wpm() */


static void exec_immediate_autokey_mode( void )
{
    // Loop through each available character
    for( size_t idx = 0; idx < s_rx_count; idx++ )
    {
        // If we receive the null character, exit autokey mode
        if( s_rx_buf[ idx ] == 0 )
        {
            debug_port_print( NEWLINE_STR CMD_STR_KEYER ": Exited immediate autokey mode." NEWLINE_STR );
            s_immediate_autokey = false;
            keyer_panic();
            break;
        }

        // Otherwise, queue the character
        keyer_autokey_char( ( char )s_rx_buf[ idx ] );
    }

    // All characters were consumed
    s_rx_count = 0;

}   /* exec_immediate_autokey_mode() */


static void print_invalid_command( char const * const command )
{
    debug_port_print( INVALID_COMMAND_STR "\"" );
    debug_port_print( command );
    debug_port_print( "\"" NEWLINE_STR );

}   /* print_invalid_command() */

#endif /* defined( _FEATURE_ENABLE_DEBUG_PORT ) && _FEATURE_ENABLE_DEBUG_PORT */
