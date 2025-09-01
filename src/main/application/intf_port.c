/**
 * @file    src/main/application/intf_port.c
 * @brief   Implementation for the interface port module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-29
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdio.h>
#include <string.h>

#include "application/buzzer.h"
#include "application/config.h"
#include "application/intf_port.h"
#include "application/intf_types.h"
#include "application/keyer.h"
#include "application/led.h"
#include "application/wpm.h"
#include "core/sys.h"
#include "core/version.h"
#include "drivers/usart.h"
#include "utility/constants.h"
#include "utility/crc.h"
#include "utility/debug.h"
#include "utility/types.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

/**
 * @def     RX_BUF_SIZE
 * @brief   Size of the RX buffer, in bytes.
 */
#define RX_BUF_SIZE     INTF_MESSAGE_MAX_SIZE

/**
 * @def     RX_BUF_TIMEOUT
 * @brief   Timeout for stale data in buffer.
 */
#define RX_BUF_TIMEOUT  250 * TICKS_PER_MSEC

/* --------------------------------------------------- VARIABLES ---------------------------------------------------- */

static byte_t s_rx_buf[ RX_BUF_SIZE ];      /**< Local receive buffer.                  */
static size_t s_rx_count = 0;               /**< Number of bytes in receive buffer.     */
static tick_t s_rx_tick = 0;                /**< Tick of most recent byte.              */

/* ----------------------------------------------------- MACROS ----------------------------------------------------- */

/**
 * @def     rx_buf_avail()
 * @brief   Returns the number of bytes available in the RX buffer.
 */
#define rx_buf_avail()                                                                  \
    ( RX_BUF_SIZE - s_rx_count )

/**
 * @def     VALIDATE_ENUM_OR_BAIL( _value, _count )
 * @brief   Sends an error packet and returns if the specified enum value is invalid.
 */
#define VALIDATE_ENUM_OR_BAIL( _value, _count )                                         \
    if( ( _value ) >= ( _count ) )                                                      \
    {                                                                                   \
        send_empty_packet( INTF_MESSAGE_REPLY_INVALID_VALUE );                          \
        return;                                                                         \
    }

/**
 * @def     VALIDATE_PAYLOAD_SIZE_OR_BAIL( _size )
 * @brief   Sends an error packet and returns if the payload does not have the specified size.
 */
#define VALIDATE_PAYLOAD_SIZE_OR_BAIL( _size )                                          \
    if( header->size != ( _size ) )                                                     \
    {                                                                                   \
        send_empty_packet( INTF_MESSAGE_REPLY_INVALID_PAYLOAD );                        \
        return;                                                                         \
    }

/**
 * @def     VALIDATE_RANGE_OR_BAIL( _value, _min, _max )
 * @brief   Sends an error packet and returns if the specifed value is outside of the specified minimum / maximum.
 */
#define VALIDATE_RANGE_OR_BAIL( _value, _min, _max )                                    \
    if( ( _value ) < ( _min ) || ( _value ) > ( _max ) )                                \
    {                                                                                   \
        send_empty_packet( INTF_MESSAGE_REPLY_INVALID_VALUE );                          \
        return;                                                                         \
    }

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      evaluate_rx_buf( void )
 * @brief   Evaluates the current RX buffer.
 */
static void evaluate_rx_buf( void );

/**
 * @fn      process_message( intf_header_t const *, void const * )
 * @brief   Processes the specified interface message.
 */
static void process_message( intf_header_t const * header, void const * payload );

/**
 * @fn      process_message_request_autokey( intf_header_t const *, void const * )
 * @brief   Processes the specified interface message with the `INTF_MESSAGE_REQUEST_AUTOKEY` message ID.
 */
static void process_message_request_autokey( intf_header_t const * header, void const * payload );

/**
 * @fn      process_message_request_get_buzzer_enabled( intf_header_t const *, void const * )
 * @brief   Processes the specified interface message with the `INTF_MESSAGE_REQUEST_GET_BUZZER_ENABLED` message ID.
 */
static void process_message_request_get_buzzer_enabled( intf_header_t const * header, void const * payload );

/**
 * @fn      process_message_request_get_buzzer_frequency( intf_header_t const *, void const * )
 * @brief   Processes the specified interface message with the `INTF_MESSAGE_REQUEST_GET_BUZZER_FREQUENCY` message ID.
 */
static void process_message_request_get_buzzer_frequency( intf_header_t const * header, void const * payload );

/**
 * @fn      process_message_request_get_invert_paddles( intf_header_t const *, void const * )
 * @brief   Processes the specified interface message with the `INTF_MESSAGE_REQUEST_GET_INVERT_PADDLES` message ID.
 */
static void process_message_request_get_invert_paddles( intf_header_t const * header, void const * payload );

/**
 * @fn      process_message_request_get_io_polarity( intf_header_t const *, void const * )
 * @brief   Processes the specified interface message with the `INTF_MESSAGE_REQUEST_GET_IO_POLARITY` message ID.
 */
static void process_message_request_get_io_polarity( intf_header_t const * header, void const * payload );

/**
 * @fn      process_message_request_get_io_state( intf_header_t const *, void const * )
 * @brief   Processes the specified interface message with the `INTF_MESSAGE_REQUEST_GET_IO_STATE` message ID.
 */
static void process_message_request_get_io_state( intf_header_t const * header, void const * payload );

/**
 * @fn      process_message_request_get_io_state_for_type( intf_header_t const *, void const * )
 * @brief   Processes the specified interface message with the `INTF_MESSAGE_REQUEST_GET_IO_STATE_FOR_TYPE` message ID.
 */
static void process_message_request_get_io_state_for_type( intf_header_t const * header, void const * payload );

/**
 * @fn      process_message_request_get_io_type( intf_header_t const *, void const * )
 * @brief   Processes the specified interface message with the `INTF_MESSAGE_REQUEST_GET_IO_TYPE` message ID.
 */
static void process_message_request_get_io_type( intf_header_t const * header, void const * payload );

/**
 * @fn      process_message_request_get_led_enabled( intf_header_t const *, void const * )
 * @brief   Processes the specified interface message with the `INTF_MESSAGE_REQUEST_GET_LED_ENABLED` message ID.
 */
static void process_message_request_get_led_enabled( intf_header_t const * header, void const * payload );

/**
 * @fn      process_message_request_get_paddle_mode( intf_header_t const *, void const * )
 * @brief   Processes the specified interface message with the `INTF_MESSAGE_REQUEST_GET_PADDLE_MODE` message ID.
 */
static void process_message_request_get_paddle_mode( intf_header_t const * header, void const * payload );

/**
 * @fn      process_message_request_get_trainer_mode( intf_header_t const *, void const * )
 * @brief   Processes the specified interface message with the `INTF_MESSAGE_REQUEST_GET_TRAINER_MODE` message ID.
 */
static void process_message_request_get_trainer_mode( intf_header_t const * header, void const * payload );

/**
 * @fn      process_message_request_get_wpm( intf_header_t const *, void const * )
 * @brief   Processes the specified interface message with the `INTF_MESSAGE_REQUEST_GET_WPM` message ID.
 */
static void process_message_request_get_wpm( intf_header_t const * header, void const * payload );

/**
 * @fn      process_message_request_get_wpm_scale( intf_header_t const *, void const * )
 * @brief   Processes the specified interface message with the `INTF_MESSAGE_REQUEST_GET_WPM_SCALE` message ID.
 */
static void process_message_request_get_wpm_scale( intf_header_t const * header, void const * payload );

/**
 * @fn      process_message_request_panic( intf_header_t const *, void const * )
 * @brief   Processes the specified interface message with the `INTF_MESSAGE_REQUEST_PANIC` message ID.
 */
static void process_message_request_panic( intf_header_t const * header, void const * payload );

/**
 * @fn      process_message_request_ping( intf_header_t const *, void const * )
 * @brief   Processes the specified interface message with the `INTF_MESSAGE_REQUEST_PING` message ID.
 */
static void process_message_request_ping( intf_header_t const * header, void const * payload );

/**
 * @fn      process_message_request_restore_default_config( intf_header_t const *, void const * )
 * @brief   Processes the specified interface message with the `INTF_MESSAGE_REQUEST_RESTORE_DEFAULT_CONFIG` message ID.
 */
static void process_message_request_restore_default_config( intf_header_t const * header, void const * payload );

/**
 * @fn      process_message_request_set_buzzer_enabled( intf_header_const *, void const * )
 * @brief   Processes the specified interface message with the `INTF_MESSAGE_REQUEST_SET_BUZZER_ENABLED` message ID.
 */
static void process_message_request_set_buzzer_enabled( intf_header_t const * header, void const * payload );

/**
 * @fn      process_message_request_set_buzzer_frequency( intf_header_const *, void const * )
 * @brief   Processes the specified interface message with the `INTF_MESSAGE_REQUEST_SET_BUZZER_FREQUENCY` message ID.
 */
static void process_message_request_set_buzzer_frequency( intf_header_t const * header, void const * payload );

/**
 * @fn      process_message_request_set_invert_paddles( intf_header_t const *, void const * )
 * @brief   Processes the specified interface message with the `INTF_MESSAGE_REQUEST_SET_INVERT_PADDLES` message ID.
 */
static void process_message_request_set_invert_paddles( intf_header_t const * header, void const * payload );

/**
 * @fn      process_message_request_set_io_polarity( intf_header_t const *, void const * )
 * @brief   Processes the specified interface message with the `INTF_MESSAGE_REQUEST_SET_IO_POLARITY` message ID.
 */
static void process_message_request_set_io_polarity( intf_header_t const * header, void const * payload );

/**
 * @fn      process_message_request_set_io_type( intf_header_t const *, void const * )
 * @brief   Processes the specified interface message with the `INTF_MESSAGE_REQUEST_SET_IO_TYPE` message ID.
 */
static void process_message_request_set_io_type( intf_header_t const * header, void const * payload );

/**
 * @fn      process_message_request_set_led_enabled( intf_header_t const *, void const * )
 * @brief   Processes the specified interface message with the `INTF_MESSAGE_REQUEST_SET_LED_ENABLED` message ID.
 */
static void process_message_request_set_led_enabled( intf_header_t const * header, void const * payload );

/**
 * @fn      process_message_request_set_paddle_mode( intf_header_t const *, void const * )
 * @brief   Processes the specified interface message with the `INTF_MESSAGE_REQUEST_SET_PADDLE_MODE` message ID.
 */
static void process_message_request_set_paddle_mode( intf_header_t const * header, void const * payload );

/**
 * @fn      process_message_request_set_trainer_mode( intf_header_t const *, void const * )
 * @brief   Processes the specified interface message with the `INTF_MESSAGE_REQUEST_SET_TRAINER_MODE` message ID.
 */
static void process_message_request_set_trainer_mode( intf_header_t const * header, void const * payload );

/**
 * @fn      process_message_request_set_wpm( intf_header_t const *, void const * )
 * @brief   Processes the specified interface message with the `INTF_MESSAGE_REQUEST_sET_WPM` message ID.
 */
static void process_message_request_set_wpm( intf_header_t const * header, void const * payload );

/**
 * @fn      process_message_request_set_wpm_scale( intf_header_t const *, void const * )
 * @brief   Processes the specified interface message with the `INTF_MESSAGE_REQUEST_SET_WPM_SCALE` message ID.
 */
static void process_message_request_set_wpm_scale( intf_header_t const * header, void const * payload );

/**
 * @fn      process_message_request_version( intf_header_t const *, void const * )
 * @brief   Processes the specified interface message with the `INTF_MESSAGE_REQUEST_VERSION` message ID.
 */
static void process_message_request_version( intf_header_t const * header, void const * payload );

/**
 * @fn      send_empty_packet( intf_message_t )
 * @brief   Sends a packet with the specified message ID and no payload.
 */
static void send_empty_packet( intf_message_t message );

/**
 * @fn      send_packet( intf_message_t message, void *, size_t )
 * @brief   Sends a packet with the specified message ID and payload.
 */
static void send_packet( intf_message_t message, void * payload, size_t size );

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

void intf_port_init( void )
{
    usart_init( INTF_PORT_USART,
                true,
                true,
                _FEATURE_OPT_INTF_PORT_BAUD,
                USART_DATA_BITS_8,
                USART_STOP_BITS_1,
                USART_PARITY_DISABLED );

}   /* intf_port_init() */


void intf_port_tick( tick_t tick )
{
    // Reset the RX buffer if the data has timed out.
    // This is to prevent stale data from malformed packets from lingering and interfering with future packets.
    if( sys_elapsed( tick, s_rx_tick ) > RX_BUF_TIMEOUT )
        s_rx_count = 0;

}   /* intf_port_tick() */


void intf_port_usart_rx( void )
{
    s_rx_tick = sys_get_tick();

    while( rx_buf_avail() != 0 &&
           usart_rx( INTF_PORT_USART, s_rx_buf + s_rx_count, 1 ) )
    {
        s_rx_count++;
        evaluate_rx_buf();
    }

}   /* intf_port_usart_rx() */


static void evaluate_rx_buf( void )
{
    // Sanity check - reset the RX buffer if we overflow
    if( s_rx_count >= RX_BUF_SIZE )
    {
        send_empty_packet( INTF_MESSAGE_REPLY_INVALID_SIZE );
        s_rx_count = 0;
        return;
    }

    // If we haven't received enough data to process a header, then bail out
    if( s_rx_count < sizeof( intf_header_t ) )
        return;

    // We have received a header - ensure that the payload size is in range
    intf_header_t const * header = ( intf_header_t * )s_rx_buf;
    if( header->size > INTF_MESSAGE_PAYLOAD_MAX_SIZE )
    {
        send_empty_packet( INTF_MESSAGE_REPLY_INVALID_SIZE );
        s_rx_count = 0;
        return;
    }

    // If we haven't received enough data yet, bail out
    if( s_rx_count < sizeof( intf_header_t ) + header->size )
        return;

    // If we've received *too much* data, reset and bail out
    if( s_rx_count > sizeof( intf_header_t ) + header->size )
    {
        send_empty_packet( INTF_MESSAGE_REPLY_INVALID_SIZE );
        s_rx_count = 0;
        return;
    }

    // At this point, we have received exactly the correct number of bytes per the header.
    // If the size is non-zero, check the CRC16.
    if( header->size != 0 )
    {
        crc16_t calc_crc = crc_calc_crc16( s_rx_buf + sizeof( intf_header_t ), header->size );
        if( calc_crc != header->crc )
        {
            send_empty_packet( INTF_MESSAGE_REPLY_INVALID_CRC );
            s_rx_count = 0;
            return;
        }
    }
    else if( header->crc != 0 )
    {
        // Enforce a CRC of zero for zero-sized packets
        send_empty_packet( INTF_MESSAGE_REPLY_INVALID_CRC );
        s_rx_count = 0;
        return;
    }

    // CRC check passed - process the packet then reset the buffer
    process_message( header, s_rx_buf + sizeof( intf_header_t ) );
    s_rx_count = 0;

}   /* evaluate_rx_buf() */


static void process_message( intf_header_t const * header, void const * payload )
{
    switch( header->message )
    {
    case INTF_MESSAGE_REQUEST_AUTOKEY:
        process_message_request_autokey( header, payload );
        break;

    case INTF_MESSAGE_REQUEST_GET_BUZZER_ENABLED:
        process_message_request_get_buzzer_enabled( header, payload );
        break;

    case INTF_MESSAGE_REQUEST_GET_BUZZER_FREQUENCY:
        process_message_request_get_buzzer_frequency( header, payload );
        break;

    case INTF_MESSAGE_REQUEST_GET_INVERT_PADDLES:
        process_message_request_get_invert_paddles( header, payload );
        break;

    case INTF_MESSAGE_REQUEST_GET_IO_POLARITY:
        process_message_request_get_io_polarity( header, payload );
        break;

    case INTF_MESSAGE_REQUEST_GET_IO_STATE:
        process_message_request_get_io_state( header, payload );
        break;

    case INTF_MESSAGE_REQUEST_GET_IO_STATE_FOR_TYPE:
        process_message_request_get_io_state_for_type( header, payload );
        break;

    case INTF_MESSAGE_REQUEST_GET_IO_TYPE:
        process_message_request_get_io_type( header, payload );
        break;

    case INTF_MESSAGE_REQUEST_GET_LED_ENABLED:
        process_message_request_get_led_enabled( header, payload );
        break;

    case INTF_MESSAGE_REQUEST_GET_PADDLE_MODE:
        process_message_request_get_paddle_mode( header, payload );
        break;

    case INTF_MESSAGE_REQUEST_GET_TRAINER_MODE:
        process_message_request_get_trainer_mode( header, payload );
        break;

    case INTF_MESSAGE_REQUEST_GET_WPM:
        process_message_request_get_wpm( header, payload );
        break;

    case INTF_MESSAGE_REQUEST_GET_WPM_SCALE:
        process_message_request_get_wpm_scale( header, payload );
        break;

    case INTF_MESSAGE_REQUEST_PANIC:
        process_message_request_panic( header, payload );
        break;

    case INTF_MESSAGE_REQUEST_PING:
        process_message_request_ping( header, payload );
        break;

    case INTF_MESSAGE_REQUEST_RESTORE_DEFAULT_CONFIG:
        process_message_request_restore_default_config( header, payload );
        return;

    case INTF_MESSAGE_REQUEST_SET_BUZZER_ENABLED:
        process_message_request_set_buzzer_enabled( header, payload );
        break;

    case INTF_MESSAGE_REQUEST_SET_BUZZER_FREQUENCY:
        process_message_request_set_buzzer_frequency( header, payload );
        break;

    case INTF_MESSAGE_REQUEST_SET_INVERT_PADDLES:
        process_message_request_set_invert_paddles( header, payload );
        break;

    case INTF_MESSAGE_REQUEST_SET_IO_POLARITY:
        process_message_request_set_io_polarity( header, payload );
        break;

    case INTF_MESSAGE_REQUEST_SET_IO_TYPE:
        process_message_request_set_io_type( header, payload );
        break;

    case INTF_MESSAGE_REQUEST_SET_LED_ENABLED:
        process_message_request_set_led_enabled( header, payload );
        break;

    case INTF_MESSAGE_REQUEST_SET_PADDLE_MODE:
        process_message_request_set_paddle_mode( header, payload );
        break;

    case INTF_MESSAGE_REQUEST_SET_TRAINER_MODE:
        process_message_request_set_trainer_mode( header, payload );
        break;

    case INTF_MESSAGE_REQUEST_SET_WPM:
        process_message_request_set_wpm( header, payload );
        break;

    case INTF_MESSAGE_REQUEST_SET_WPM_SCALE:
        process_message_request_set_wpm_scale( header, payload );
        break;

    case INTF_MESSAGE_REQUEST_VERSION:
        process_message_request_version( header, payload );
        break;

    default:
        // Unknown message?
        send_empty_packet( INTF_MESSAGE_REPLY_INVALID_MESSAGE );
        break;
    }

}   /* process_message() */


static void process_message_request_autokey( intf_header_t const * header, void const * payload )
{
    // Ensure string is null terminated
    char const * str = ( char const * )payload;
    if( str[ header->size - 1 ] != NULL_CHAR )
    {
        send_empty_packet( INTF_MESSAGE_REPLY_INVALID_PAYLOAD );
        return;
    }

    // Key string
    keyer_autokey_str( str );

    // Send reply
    send_empty_packet( INTF_MESSAGE_REPLY_SUCCESS );

}   /* process_message_request_autokey() */


static void process_message_request_get_buzzer_enabled( intf_header_t const * header, void const * payload )
{
    ( void )payload;
    VALIDATE_PAYLOAD_SIZE_OR_BAIL( 0 );

    bool enabled = buzzer_get_enabled();
    send_packet( INTF_MESSAGE_REPLY_SUCCESS, & enabled, sizeof( enabled ) );

}   /* process_message_request_get_buzzer_enabled() */


static void process_message_request_get_buzzer_frequency( intf_header_t const * header, void const * payload )
{
    ( void )payload;
    VALIDATE_PAYLOAD_SIZE_OR_BAIL( 0 );

    buzzer_freq_t freq = buzzer_get_frequency();
    send_packet( INTF_MESSAGE_REPLY_SUCCESS, & freq, sizeof( freq ) );

}   /* process_message_request_get_buzzer_frequency() */


static void process_message_request_get_invert_paddles( intf_header_t const * header, void const * payload )
{
    ( void )payload;
    VALIDATE_PAYLOAD_SIZE_OR_BAIL( 0 );

    bool inverted = keyer_get_paddle_invert();
    send_packet( INTF_MESSAGE_REPLY_SUCCESS, & inverted, sizeof( inverted ) );

}   /* process_message_request_get_invert_paddles() */


static void process_message_request_get_paddle_mode( intf_header_t const * header, void const * payload )
{
    ( void )payload;
    VALIDATE_PAYLOAD_SIZE_OR_BAIL( 0 );

    keyer_paddle_mode_t mode = keyer_get_paddle_mode();
    send_packet( INTF_MESSAGE_REPLY_SUCCESS, & mode, sizeof( mode ) );

}   /* process_message_request_get_paddle_mode() */


static void process_message_request_get_io_polarity( intf_header_t const * header, void const * payload )
{
    VALIDATE_PAYLOAD_SIZE_OR_BAIL( sizeof( io_pin_t ) );

    io_pin_t pin = *( ( io_pin_t const * )payload );
    VALIDATE_ENUM_OR_BAIL( pin, IO_PIN_COUNT );

    io_polarity_t polarity = io_get_polarity( pin );
    send_packet( INTF_MESSAGE_REPLY_SUCCESS, & polarity, sizeof( polarity ) );

}   /* process_message_request_get_io_polarity() */


static void process_message_request_get_io_state( intf_header_t const * header, void const * payload )
{
    VALIDATE_PAYLOAD_SIZE_OR_BAIL( sizeof( io_pin_t ) );

    io_pin_t pin = *( ( io_pin_t const * )payload );
    VALIDATE_ENUM_OR_BAIL( pin, IO_PIN_COUNT );

    io_state_t state = io_get_state( pin );
    bool state_is_on = ( state == IO_STATE_ON );
    send_packet( INTF_MESSAGE_REPLY_SUCCESS, & state_is_on, sizeof( state_is_on ) );

}   /* process_message_request_get_io_state() */


static void process_message_request_get_io_state_for_type( intf_header_t const * header, void const * payload )
{
    VALIDATE_PAYLOAD_SIZE_OR_BAIL( sizeof( io_type_t ) );

    io_type_t type = *( ( io_type_t const * )payload );
    VALIDATE_ENUM_OR_BAIL( type, IO_TYPE_COUNT );

    io_state_t state = io_get_state_type( type );
    bool state_is_on = ( state == IO_STATE_ON );
    send_packet( INTF_MESSAGE_REPLY_SUCCESS, & state_is_on, sizeof( state_is_on ) );

}   /* process_message_request_get_io_state_for_type() */


static void process_message_request_get_io_type( intf_header_t const * header, void const * payload )
{
    VALIDATE_PAYLOAD_SIZE_OR_BAIL( sizeof( io_pin_t ) );

    io_pin_t pin = *( ( io_pin_t const * )payload );
    VALIDATE_ENUM_OR_BAIL( pin, IO_PIN_COUNT );

    io_type_t type = io_get_type( pin );
    send_packet( INTF_MESSAGE_REPLY_SUCCESS, & type, sizeof( type ) );

}   /* process_message_request_get_io_type() */


static void process_message_request_get_led_enabled( intf_header_t const * header, void const * payload )
{
    VALIDATE_PAYLOAD_SIZE_OR_BAIL( sizeof( led_t ) );

    led_t led = *( ( led_t const * )payload );
    VALIDATE_ENUM_OR_BAIL( led, LED_COUNT );

    bool enabled = led_get_enabled( led );
    send_packet( INTF_MESSAGE_REPLY_SUCCESS, & enabled, sizeof( enabled ) );

}   /* process_message_request_get_paddle_mode() */


static void process_message_request_get_trainer_mode( intf_header_t const * header, void const * payload )
{
    ( void )payload;
    VALIDATE_PAYLOAD_SIZE_OR_BAIL( 0 );

    bool enabled = keyer_get_trainer_mode_enabled();
    send_packet( INTF_MESSAGE_REPLY_SUCCESS, & enabled, sizeof( enabled ) );

}   /* process_message_request_get_trainer_mode() */


static void process_message_request_get_wpm( intf_header_t const * header, void const * payload )
{
    ( void )payload;
    VALIDATE_PAYLOAD_SIZE_OR_BAIL( 0 );

    wpm_t wpm = wpm_get();
    send_packet( INTF_MESSAGE_REPLY_SUCCESS, & wpm, sizeof( wpm ) );

}   /* process_message_request_get_wpm() */


static void process_message_request_get_wpm_scale( intf_header_t const * header, void const * payload )
{
    VALIDATE_PAYLOAD_SIZE_OR_BAIL( sizeof( wpm_element_t ) );

    wpm_element_t el = *( ( wpm_element_t const * )payload );
    VALIDATE_ENUM_OR_BAIL( el, WPM_ELEMENT_COUNT );

    wpm_element_scale_t scale = wpm_get_element_scale( el );
    send_packet( INTF_MESSAGE_REPLY_SUCCESS, & scale, sizeof( wpm_element_scale_t ) );

}   /* process_message_request_get_wpm_scale() */


static void process_message_request_panic( intf_header_t const * header, void const * payload )
{
    ( void )payload;
    VALIDATE_PAYLOAD_SIZE_OR_BAIL( 0 );

    keyer_panic();
    send_empty_packet( INTF_MESSAGE_REPLY_SUCCESS );

}   /* process_message_request_panic() */


static void process_message_request_ping( intf_header_t const * header, void const * payload )
{
    ( void )payload;
    VALIDATE_PAYLOAD_SIZE_OR_BAIL( 0 );

    keyer_autokey_str( "73ee" );
    send_empty_packet( INTF_MESSAGE_REPLY_SUCCESS );

}   /* process_message_request_ping() */


static void process_message_request_restore_default_config( intf_header_t const * header, void const * payload )
{
    ( void )payload;
    VALIDATE_PAYLOAD_SIZE_OR_BAIL( 0 );

    config_t config;
    config_default( & config );
    config_set( & config );
    send_empty_packet( INTF_MESSAGE_REPLY_SUCCESS );

}   /* process_message_request_restore_default_config() */


static void process_message_request_set_buzzer_enabled( intf_header_t const * header, void const * payload )
{
    VALIDATE_PAYLOAD_SIZE_OR_BAIL( sizeof( bool ) );

    bool enabled = *( ( bool const * )payload );
    buzzer_set_enabled( enabled );
    send_empty_packet( INTF_MESSAGE_REPLY_SUCCESS );

}   /* process_message_request_set_buzzer_enabled() */


static void process_message_request_set_buzzer_frequency( intf_header_t const * header, void const * payload )
{
    VALIDATE_PAYLOAD_SIZE_OR_BAIL( sizeof( buzzer_freq_t ) );

    buzzer_freq_t freq = *( ( buzzer_freq_t const * )payload );
    VALIDATE_RANGE_OR_BAIL( freq, BUZZER_MINIMUM_FREQUENCY, BUZZER_MAXIMUM_FREQUENCY );

    buzzer_set_frequency( freq );
    send_empty_packet( INTF_MESSAGE_REPLY_SUCCESS );

}   /* process_message_request_set_buzzer_frequency() */


static void process_message_request_set_invert_paddles( intf_header_t const * header, void const * payload )
{
    VALIDATE_PAYLOAD_SIZE_OR_BAIL( sizeof( bool ) );

    bool inverted = *( ( bool const * )payload );
    keyer_set_paddle_invert( inverted );
    send_empty_packet( INTF_MESSAGE_REPLY_SUCCESS );

}   /* process_message_request_set_invert_paddles() */


static void process_message_request_set_io_polarity( intf_header_t const * header, void const * payload )
{
    typedef struct
    {
        io_pin_t pin;
        io_polarity_t polarity;
    } format_t;

    VALIDATE_PAYLOAD_SIZE_OR_BAIL( sizeof( format_t ) );

    format_t const * pkt = ( format_t const * )payload;
    VALIDATE_ENUM_OR_BAIL( pkt->pin, IO_PIN_COUNT );
    VALIDATE_ENUM_OR_BAIL( pkt->polarity, IO_POLARITY_COUNT );

    io_set_polarity( pkt->pin, pkt->polarity );
    send_empty_packet( INTF_MESSAGE_REPLY_SUCCESS );

}   /* process_message_request_set_io_polarity() */


static void process_message_request_set_io_type( intf_header_t const * header, void const * payload )
{
    typedef struct
    {
        io_pin_t pin;
        io_type_t type;
    } format_t;

    VALIDATE_PAYLOAD_SIZE_OR_BAIL( sizeof( format_t ) );

    format_t const * pkt = ( format_t const * )payload;
    VALIDATE_ENUM_OR_BAIL( pkt->pin, IO_PIN_COUNT );
    VALIDATE_ENUM_OR_BAIL( pkt->type, IO_TYPE_COUNT );

    io_set_type( pkt->pin, pkt->type );
    send_empty_packet( INTF_MESSAGE_REPLY_SUCCESS );

}   /* process_message_request_set_io_type() */


static void process_message_request_set_led_enabled( intf_header_t const * header, void const * payload )
{
    typedef struct
    {
        led_t led;
        bool enabled;
    } format_t;

    VALIDATE_PAYLOAD_SIZE_OR_BAIL( sizeof( format_t ) );

    format_t const * pkt = ( format_t const * )payload;
    VALIDATE_ENUM_OR_BAIL( pkt->led, LED_COUNT );

    led_set_enabled( pkt->led, pkt->enabled );
    send_empty_packet( INTF_MESSAGE_REPLY_SUCCESS );

}   /* process_message_request_set_paddle_mode() */


static void process_message_request_set_paddle_mode( intf_header_t const * header, void const * payload )
{
    VALIDATE_PAYLOAD_SIZE_OR_BAIL( sizeof( keyer_paddle_mode_t ) );

    keyer_paddle_mode_t mode = *( ( keyer_paddle_mode_t const * )payload );
    VALIDATE_ENUM_OR_BAIL( mode, KEYER_PADDLE_MODE_COUNT );

    keyer_set_paddle_mode( mode );
    send_empty_packet( INTF_MESSAGE_REPLY_SUCCESS );

}   /* process_message_request_set_paddle_mode() */


static void process_message_request_set_trainer_mode( intf_header_t const * header, void const * payload )
{
    VALIDATE_PAYLOAD_SIZE_OR_BAIL( sizeof( bool ) );

    bool enabled = *( ( bool const * )payload );
    keyer_set_trainer_mode_enabled( enabled );
    send_empty_packet( INTF_MESSAGE_REPLY_SUCCESS );

}   /* process_message_request_set_trainer_mode() */


static void process_message_request_set_wpm( intf_header_t const * header, void const * payload )
{
    VALIDATE_PAYLOAD_SIZE_OR_BAIL( sizeof( wpm_t ) );

    wpm_t wpm = *( ( wpm_t const * )payload );
    VALIDATE_RANGE_OR_BAIL( wpm, WPM_MINIMUM, WPM_MAXIMUM );

    wpm_set( wpm );
    send_empty_packet( INTF_MESSAGE_REPLY_SUCCESS );

}   /* process_message_request_set_wpm() */


static void process_message_request_set_wpm_scale( intf_header_t const * header, void const * payload )
{
    typedef struct
    {
        wpm_element_t el;
        wpm_element_scale_t scale;
    } format_t;

    VALIDATE_PAYLOAD_SIZE_OR_BAIL( sizeof( format_t ) );

    format_t const * pkt = ( format_t const * )payload;
    VALIDATE_ENUM_OR_BAIL( pkt->el, WPM_ELEMENT_COUNT );
    VALIDATE_RANGE_OR_BAIL( pkt->scale, WPM_ELEMENT_SCALE_MINIMUM, WPM_ELEMENT_SCALE_MAXIMUM );

    wpm_set_element_scale( pkt->el, pkt->scale );
    send_empty_packet( INTF_MESSAGE_REPLY_SUCCESS );

}   /* process_message_request_set_wpm_scale() */


static void process_message_request_version( intf_header_t const * header, void const * payload )
{
    ( void )payload;
    VALIDATE_PAYLOAD_SIZE_OR_BAIL( 0 );

    version_t version;
    version_get( & version );
    #define BUF_SZ 128
    char buf[ BUF_SZ ];
    size_t count = snprintf( buf, BUF_SZ, "%s v%s (%s [%s] %s %s)",
                             version.product_name,
                             version.version,
                             version.git_branch,
                             version.git_hash_short,
                             version.build_date,
                             version.build_time );
    assert_always( count <= BUF_SZ );
    #undef BUF_SZ

    send_packet( INTF_MESSAGE_REPLY_SUCCESS, buf, count );

}   /* process_message_request_version() */


static void send_empty_packet( intf_message_t message )
{
    intf_header_t header;
    header.message = message;
    header.size = 0;
    header.crc = 0;

    usart_tx( INTF_PORT_USART, ( byte_t * ) & header, sizeof( intf_header_t ), USART_WAIT_MODE_NORMAL );

}   /* send_empty_packet() */


static void send_packet( intf_message_t message, void * payload, size_t size )
{
    intf_header_t header;
    header.message = message;
    header.size = size;
    header.crc = ( uint16_t )crc_calc_crc16( payload, size );

    usart_tx( INTF_PORT_USART, ( byte_t * ) & header, sizeof( intf_header_t ), USART_WAIT_MODE_NORMAL );
    usart_tx( INTF_PORT_USART, ( byte_t * ) payload, size, USART_WAIT_MODE_NORMAL );

}   /* send_packet() */
