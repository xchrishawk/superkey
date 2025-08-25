/**
 * @file    src/main/application/debug_port.c
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
#include "core/version.h"
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
 * @def     ENABLE_STR
 * @brief   The token to enable an option.
 */
#define ENABLE_STR              "enable"

/**
 * @def     ENABLED_STR
 * @brief   The token which indicates that an option is enabled.
 */
#define ENABLED_STR             "enabled"

/**
 * @def     DISABLE_STR
 * @brief   The token to disable an option.
 */
#define DISABLE_STR             "disable"

/**
 * @def     DISABLED_STR
 * @brief   The token which indicates that an option is disabled.
 */
#define DISABLED_STR            "disabled"

/**
 * @define  OFF_STR
 * @brief   The token which indicates that an input or output is off.
 */
#define OFF_STR                 "off"

/**
 * @def     ON_STR
 * @brief   The token which indicates that an input or output is on.
 */
#define ON_STR                  "on"

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
static void exec_command( char const * const command );

/**
 * @fn      exec_command_buzzer( char const * command )
 * @brief   Executes the `buzzer` command.
 */
static void exec_command_buzzer( char const * const command );

/**
 * @fn      exec_command_config( char const * command )
 * @brief   Executes the `config` command.
 */
static void exec_command_config( char const * const command );

/**
 * @fn      exec_command_help( char const * command )
 * @brief   Executes the `help` command.
 */
static void exec_command_help( char const * const command );

/**
 * @fn      exec_command_input( char const * command )
 * @brief   Executes the `input` command.
 */
static void exec_command_input( char const * const command );

/**
 * @fn      exec_command_keyer( char const * command )
 * @brief   Executes the `keyer` command.
 */
static void exec_command_keyer( char const * const command );

/**
 * @fn      exec_command_led( char const * command )
 * @brief   Executes the `led` command.
 */
static void exec_command_led( char const * const command );

/**
 * @fn      exec_command_panic( char const * command )
 * @brief   Executes the `panic` command.
 */
static void exec_command_panic( char const * const command );

/**
 * @fn      exec_command_tick( char const * command )
 * @brief   Executes the `tick` command.
 */
static void exec_command_tick( char const * const command );

/**
 * @fn      exec_command_version( char const * command )
 * @brief   Executes the `version` command.
 */
static void exec_command_version( char const * const command );

/**
 * @fn      exec_command_wpm( char const * command )
 * @brief   Executes the `wpm` command.
 */
static void exec_command_wpm( char const * const command );

/**
 * @fn      print_invalid_command( char const * )
 * @brief   Sends an "Invalid command" message for the specified command.
 */
static void print_invalid_command( char const * const command );

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

/**
 * @fn      string_begins_with( char const *, char const * )
 * @brief   Returns `true` if `str` begins with `token`.
 */
static bool string_begins_with( char const * str, char const * token );

/**
 * @fn      string_equals( char const *, char const * )
 * @brief   Returns `true` if the specified strings are equal.
 */
static bool string_equals( char const * str, char const * token );

/**
 * @fn      string_is_empty( char const * )
 * @brief   Returns `true`
 */
static bool string_is_empty( char const * str );

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
    if( string_equals( command, "stop" ) || string_equals( command, "panic" ) )
        exec_command_panic( command );
    else if( string_begins_with( command, "buzzer" ) )
        exec_command_buzzer( command );
    else if( string_begins_with( command, "config" ) )
        exec_command_config( command );
    else if( string_begins_with( command, "help" ) )
        exec_command_help( command );
    else if( string_begins_with( command, "input" ) )
        exec_command_input( command );
    else if( string_begins_with( command, "keyer" ) )
        exec_command_keyer( command );
    else if( string_begins_with( command, "led" ) )
        exec_command_led( command );
    else if( string_begins_with( command, "tick" ) )
        exec_command_tick( command );
    else if( string_begins_with( command, "version" ) )
        exec_command_version( command );
    else if( string_begins_with( command, "wpm" ) )
        exec_command_wpm( command );

    // Handle unrecognized commands
    else
        print_invalid_command( command );

}   /* exec_command() */


static void exec_command_buzzer( char const * const command )
{
    // Drop "buzzer" token
    char const * c = command + 6;

    if( string_is_empty( c ) )
    {
        // No subcommand - interpret as a status request. no action required
    }
    else if( string_equals( c, " " ENABLE_STR ) )
    {
        // Turn buzzer on
        buzzer_set_enabled( true );
    }
    else if( string_equals( c, " " DISABLE_STR ) )
    {
        // Turn buzzer oiff
        buzzer_set_enabled( false );
    }
    else if( string_begins_with( c, " frequency " ) )
    {
        // Drop " frequency "
        c += 11;
        unsigned long freq;
        if( read_ulong( c, & freq ) &&
            freq >= BUZZER_MINIMUM_FREQUENCY &&
            freq <= BUZZER_MAXIMUM_FREQUENCY )
        {
            buzzer_set_frequency( ( buzzer_freq_t )freq );
        }
        else
        {
            debug_port_print( "Invalid frequency: \"" );
            debug_port_print( c );
            debug_port_print( "\". Must be between " stringize_value( BUZZER_MINIMUM_FREQUENCY )
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
                       buzzer_get_enabled() ? ENABLED_STR : DISABLED_STR,
                       buzzer_get_frequency() );

}   /* exec_command_buzzer() */


static void exec_command_config( char const * const command )
{
    // Drop "config" token
    char const * c = command + 6;

    if( string_equals( c, " default" ) )
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


static void exec_command_help( char const * const command )
{
    ( void )command;
    debug_port_print( "Not implemented yet." NEWLINE_STR );

}   /* exec_command_help() */


static void exec_command_input( char const * const command )
{
    // Local constants
    static char const * const s_pin_tbl[] =
    {
        stringize( INPUT_PIN_TRS_0_TIP ),
        stringize( INPUT_PIN_TRS_0_RING ),
        stringize( INPUT_PIN_TRS_1_TIP ),
        stringize( INPUT_PIN_TRS_1_RING ),
        stringize( INPUT_PIN_TRS_2_TIP ),
        stringize( INPUT_PIN_TRS_2_RING ),
    };
    _Static_assert( array_count( s_pin_tbl ) == INPUT_PIN_COUNT, "Invalid string table!" );
    static char const * const s_type_tbl[] =
    {
        stringize( INPUT_TYPE_STRAIGHT_KEY ),
        stringize( INPUT_TYPE_PADDLE_LEFT ),
        stringize( INPUT_TYPE_PADDLE_RIGHT ),
        stringize( INPUT_TYPE_PADDLE_NONE ),
    };
    _Static_assert( array_count( s_type_tbl ) == INPUT_TYPE_COUNT + 1, "Invalid string table!" );
    static char const * const s_polarity_tbl[] =
    {
        stringize( INPUT_POLARITY_ACTIVE_LOW ),
        stringize( INPUT_POLARITY_ACTIVE_HIGH ),
    };
    _Static_assert( array_count( s_polarity_tbl ) == INPUT_POLARITY_COUNT, "Invalid string table!" );

    // Drop "input" prefix
    char const * c = command + 5;

    // Ensure we have data remaining (input pin is a mandatory argument)
    if( string_is_empty( c++ ) )
    {
        print_invalid_command( command );
        return;
    }

    // Find the relevant input pin
    input_pin_t pin = INPUT_PIN_COUNT;
    for( input_pin_t search_pin = 0; search_pin < INPUT_PIN_COUNT; search_pin++ )
    {
        if( ! string_begins_with( c, s_pin_tbl[ search_pin ] ) )
            continue;
        pin = search_pin;
        c += strlen( s_pin_tbl[ search_pin ] );
        break;
    }
    if( pin == INPUT_PIN_COUNT )
    {
        print_invalid_command( c );
        return;
    }

    // Interpret command
    if( string_is_empty( c ) )
    {
        // No subcommand - interpret as a status request. no action required
    }
    else if( string_equals( c, " " DISABLE_STR ) ||
             string_equals( c, " " stringize( INPUT_TYPE_NONE ) ) )
    {
        // Disable input
        input_set_type( pin, INPUT_TYPE_NONE );
    }
    else if( string_equals( c, " " stringize( INPUT_TYPE_STRAIGHT_KEY ) ) )
    {
        // Set input as straight key
        input_set_type( pin, INPUT_TYPE_STRAIGHT_KEY );
    }
    else if( string_equals( c, " " stringize( INPUT_TYPE_PADDLE_LEFT ) ) )
    {
        // Set input as left paddle
        input_set_type( pin, INPUT_TYPE_PADDLE_LEFT );
    }
    else if( string_equals( c, " " stringize( INPUT_TYPE_PADDLE_RIGHT ) ) )
    {
        // Set input as right paddle
        input_set_type( pin, INPUT_TYPE_PADDLE_RIGHT );
    }
    else if( string_equals( c, " " stringize( INPUT_POLARITY_ACTIVE_LOW ) ) )
    {
        // Set input to active low
        input_set_polarity( pin, INPUT_POLARITY_ACTIVE_LOW );
    }
    else if( string_equals( c, " " stringize( INPUT_POLARITY_ACTIVE_HIGH ) ) )
    {
        // Set input to active high
        input_set_polarity( pin, INPUT_POLARITY_ACTIVE_HIGH );
    }
    else
    {
        // Unknown subcommand
        print_invalid_command( c );
        return;
    }

    // Print status info
    debug_port_printf( "%s: %s (%s - %s)" NEWLINE_STR,
                       s_pin_tbl[ pin ],
                       input_get_on( pin ) ? ON_STR : OFF_STR,
                       s_type_tbl[ input_get_type( pin ) ],
                       s_polarity_tbl[ input_get_polarity( pin ) ] );

}   /* exec_command_input() */


static void exec_command_keyer( char const * const command )
{
    static char const * s_mode_tbl[] =
    {
        stringize( KEYER_MODE_IAMBIC ),
        stringize( KEYER_MODE_ULTIMATIC ),
        stringize( KEYER_MODE_ULTIMATIC_ALTERNATE ),
    };
    _Static_assert( array_count( s_mode_tbl ) == KEYER_MODE_COUNT, "Invalid string table!" );

    // Drop "keyer" prefix
    char const * c = command + 5;

    if( string_is_empty( c ) )
    {
        // No subcommand - interpret as a status request. no action required
    }
    else if( string_equals( c, " invert_paddles " ENABLE_STR ) )
    {
        // Set invert paddles to true
        keyer_set_invert_paddles( true );
    }
    else if( string_equals( c, " invert_paddles " DISABLE_STR ) )
    {
        // Set invert paddles to false
        keyer_set_invert_paddles( false );
    }
    else if( string_equals( c, " output_active_low " ENABLE_STR ) )
    {
        // Set output to active low
        keyer_set_output_active_low( true );
    }
    else if( string_equals( c, " output_active_low " DISABLE_STR ) )
    {
        // Set output to active high
        keyer_set_output_active_low( false );
    }
    else if( string_equals( c, " " stringize( KEYER_MODE_IAMBIC ) ) )
    {
        // Set to iambic mode
        keyer_set_mode( KEYER_MODE_IAMBIC );
    }
    else if( string_equals( c, " " stringize( KEYER_MODE_ULTIMATIC ) ) )
    {
        // Set to ultimatic mode
        keyer_set_mode( KEYER_MODE_ULTIMATIC );
    }
    else if( string_equals( c, " " stringize( KEYER_MODE_ULTIMATIC_ALTERNATE ) ) )
    {
        // Set to ultimatic alternate mode
        keyer_set_mode( KEYER_MODE_ULTIMATIC_ALTERNATE );
    }
    else
    {
        // Unknown subcommand
        print_invalid_command( command );
        return;
    }

    // Print status info
    debug_port_printf( "Keyer: %s (active low %s, invert paddles %s)" NEWLINE_STR,
                       s_mode_tbl[ keyer_get_mode() ],
                       keyer_get_output_active_low() ? ENABLED_STR : DISABLED_STR,
                       keyer_get_invert_paddles() ? ENABLED_STR : DISABLED_STR );

}   /* exec_command_keyer() */


static void exec_command_led( char const * const command )
{
    // Local constants
    static char const * s_led_tbl[] =
    {
        stringize( LED_STATUS ),
        stringize( LED_KEY ),
    };
    _Static_assert( array_count( s_led_tbl ) == LED_COUNT, "Invalid string table!" );

    // Drop "led" prefix
    char const * c = command + 3;

    // Ensure we have data remaining (input pin is a mandatory argument)
    if( string_is_empty( c++ ) )
    {
        print_invalid_command( command );
        return;
    }

    // Find the relevant LED
    led_t led = LED_COUNT;
    for( led_t search_led = 0; search_led < LED_COUNT; search_led++ )
    {
        if( ! string_begins_with( c, s_led_tbl[ search_led ] ) )
            continue;
        led = search_led;
        c += strlen( s_led_tbl[ search_led ] );
        break;
    }
    if( led == LED_COUNT )
    {
        print_invalid_command( c );
        return;
    }

    if( string_is_empty( c ) )
    {
        // No subcommand - interpret as a status request. no action required
    }
    else if( string_equals( c, " " ENABLE_STR ) )
    {
        // Enable LED
        led_set_enabled( led, true );
    }
    else if( string_equals( c, " " DISABLE_STR ) )
    {
        // Disable LED
        led_set_enabled( led, false );
    }
    else
    {
        // Unknown subcommand
        print_invalid_command( command );
        return;
    }

    // Print LED status
    debug_port_printf( "%s: %s (%s)" NEWLINE_STR,
                       s_led_tbl[ led ],
                       led_get_on( led ) ? ON_STR : OFF_STR,
                       led_get_enabled( led ) ? ENABLED_STR : DISABLED_STR );

}   /* exec_command_led() */


static void exec_command_panic( char const * const command )
{
    ( void )command;
    keyer_panic();
    debug_port_print( "Stopped keyer." NEWLINE_STR );

}   /* exec_command_panic() */


static void exec_command_tick( char const * const command )
{
    ( void )command;
    debug_port_printf( "Tick: %lu" NEWLINE_STR, sys_get_tick() );

}   /* exec_command_tick() */


static void exec_command_version( char const * const command )
{
    ( void )command;

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
    // Drop "wpm" prefix
    char const * c = command + 3;

    unsigned long wpm;
    if( string_is_empty( c ) )
    {
        // No subcommand - interpret as a status request. no action required
    }
    else if( ( * c ) == ' ' && read_ulong( c + 1, & wpm ) )
    {
        if( wpm >= WPM_MINIMUM && wpm <= WPM_MAXIMUM )
        {
            wpm_set( ( wpm_t )wpm );
        }
        else
        {
            debug_port_print( "Invalid WPM: \"" );
            debug_port_print( c + 1 );
            debug_port_print( "\". Must be between " stringize_value( WPM_MINIMUM )
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

}   /* exec_command_wpm() */


static void print_invalid_command( char const * const command )
{
    debug_port_print( INVALID_COMMAND_STR "\"" );
    debug_port_print( command );
    debug_port_print( "\"" NEWLINE_STR );

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


static bool string_begins_with( char const * str, char const * token )
{
    return( ! strncasecmp( str, token, strlen( token ) ) );

}   /* string_begins_with() */


static bool string_equals( char const * str, char const * token )
{
    return( ! strcasecmp( str, token ) );

}   /* string_equals() */


static bool string_is_empty( char const * str )
{
    return( ( * str ) == NULL_CHAR );

}   /* string_is_empty() */
