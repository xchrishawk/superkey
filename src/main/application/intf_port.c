/**
 * @file    src/main/application/intf_port.c
 * @brief   Implementation for the interface port module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-29
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include "application/buzzer.h"
#include "application/config.h"
#include "application/intf_port.h"
#include "application/intf_types.h"
#include "application/keyer.h"
#include "core/sys.h"
#include "drivers/usart.h"
#include "utility/constants.h"
#include "utility/crc.h"
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
 * @fn      send_empty_packet( intf_message_t )
 * @brief   Sends a packet with the specified message ID and no payload.
 */
static void send_empty_packet( intf_message_t message );

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


static void process_message_request_panic( intf_header_t const * header, void const * payload )
{
    ( void )payload;

    // Validate size
    if( header->size != 0 )
    {
        send_empty_packet( INTF_MESSAGE_REPLY_INVALID_PAYLOAD );
        return;
    }

    // Stop the keyer
    keyer_panic();

    // Send reply
    send_empty_packet( INTF_MESSAGE_REPLY_SUCCESS );

}   /* process_message_request_panic() */


static void process_message_request_ping( intf_header_t const * header, void const * payload )
{
    ( void )payload;

    // Validate size
    if( header->size != 0 )
    {
        send_empty_packet( INTF_MESSAGE_REPLY_INVALID_PAYLOAD );
        return;
    }

    // Send a friendly greeting to the user
    keyer_autokey_str( "73ee" );

    // Send reply
    send_empty_packet( INTF_MESSAGE_REPLY_SUCCESS );

}   /* process_message_request_ping() */


static void process_message_request_restore_default_config( intf_header_t const * header, void const * payload )
{
    ( void )payload;

    // Validate size
    if( header->size != 0 )
    {
        send_empty_packet( INTF_MESSAGE_REPLY_INVALID_PAYLOAD );
        return;
    }

    // Restore default configuration
    config_t config;
    config_default( & config );
    config_set( & config );

    // Send reply
    send_empty_packet( INTF_MESSAGE_REPLY_SUCCESS );

}   /* process_message_request_restore_default_config() */


static void process_message_request_set_buzzer_enabled( intf_header_t const * header, void const * payload )
{
    // Validate size
    if( header->size != sizeof( bool ) )
    {
        send_empty_packet( INTF_MESSAGE_REPLY_INVALID_PAYLOAD );
        return;
    }

    // Set buzzer enablement
    bool enabled = *( ( bool const * )payload );
    buzzer_set_enabled( enabled );

    // Send reply
    send_empty_packet( INTF_MESSAGE_REPLY_SUCCESS );

}   /* process_message_request_set_buzzer_enabled() */


static void process_message_request_set_buzzer_frequency( intf_header_t const * header, void const * payload )
{
    // Validate size
    if( header->size != sizeof( buzzer_freq_t ) )
    {
        send_empty_packet( INTF_MESSAGE_REPLY_INVALID_PAYLOAD );
        return;
    }

    // Set buzzer frequency
    buzzer_freq_t freq = *( ( buzzer_freq_t const * )payload );
    if( freq < BUZZER_MINIMUM_FREQUENCY || freq > BUZZER_MAXIMUM_FREQUENCY )
    {
        send_empty_packet( INTF_MESSAGE_REPLY_INVALID_PAYLOAD );
        return;
    }
    buzzer_set_frequency( freq );

    // Send reply
    send_empty_packet( INTF_MESSAGE_REPLY_SUCCESS );

}   /* process_message_request_set_buzzer_frequency() */


static void send_empty_packet( intf_message_t message )
{
    intf_header_t header;
    header.message = message;
    header.size = 0;
    header.crc = 0;

    usart_tx( INTF_PORT_USART, ( byte_t * ) & header, sizeof( intf_header_t ), USART_WAIT_MODE_NORMAL );

}   /* send_empty_packet() */
