/**
 * @file    src/main/drivers/usart.c
 * @brief   Implementation for the USART driver module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-17
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/setbaud.h>

#include "core/sys.h"
#include "drivers/usart.h"
#include "utility/debug.h"
#include "utility/types.h"
#include "utility/utility.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

#define usart_reg( _idx )                                                               \
    { register_addr( UDR ## _idx ),                                                     \
      register_addr( UCSR ## _idx ## A ),                                               \
      register_addr( UCSR ## _idx ## B ),                                               \
      register_addr( UCSR ## _idx ## C ),                                               \
      register_addr( UBRR ## _idx ## H ),                                               \
      register_addr( UBRR ## _idx ## L ) }

static struct
{
    register_t  udr;                        /**< The UDRn register for this USART.      */
    register_t  ucsra;                      /**< The UCSRnA register for this USART.    */
    register_t  ucsrb;                      /**< The UCSRnB register for this USART.    */
    register_t  ucsrc;                      /**< The UCSRnC register for this USART.    */
    register_t  ubrrh;                      /**< The UBRRnH register for this USART.    */
    register_t  ubrrl;                      /**< The UBRRnL register for this USART.    */
}
const s_reg_tbl[] =
{
    usart_reg( 0 ),                         /* USART_0                                  */
    usart_reg( 1 ),                         /* USART_1                                  */
};
_Static_assert( array_count( s_reg_tbl ) == USART_COUNT, "Invalid register table!" );

#undef usart_reg

/**
 * @def     RX_BUF_SIZE
 * @brief   Buffer size for received data.
 * @note    `RX_BUF_SIZE` is significantly smaller than `TX_BUF_SIZE`, because it is intended that data will be
 *          immediately read from the driver's RX buffer into a function-specific buffer for further processing.
 */
#define RX_BUF_SIZE     16

/**
 * @def     TX_BUF_SIZE
 * @brief   Buffer size for transmitted data.
 * @note    `TX_BUF_SIZE` is significantly larger than `RX_BUF_SIZE`, because it needs to support queuing full-sized
 *          messages without requiring chunking.
 */
#define TX_BUF_SIZE     256

/* ----------------------------------------------------- MACROS ----------------------------------------------------- */

// Validation macros
#define validate_usart( _usart )                                                        \
    assert_always( ( _usart ) < USART_COUNT )
#define validate_data_bits( _data_bits )                                                \
    assert_always( ( _data_bits ) < USART_DATA_BITS_COUNT )
#define validate_parity( _parity )                                                      \
    assert_always( ( _parity ) < USART_PARITY_COUNT )
#define validate_stop_bits( _stop_bits )                                                \
    assert_always( ( _stop_bits ) < USART_STOP_BITS_COUNT )

// Get USART registers
#define UDR( _usart )                                                                   \
    ( * ( s_reg_tbl[ ( _usart ) ].udr ) )
#define UCSRA( _usart )                                                                 \
    ( * ( s_reg_tbl[ ( _usart ) ].ucsra ) )
#define UCSRB( _usart )                                                                 \
    ( * ( s_reg_tbl[ ( _usart ) ].ucsrb ) )
#define UCSRC( _usart )                                                                 \
    ( * ( s_reg_tbl[ ( _usart ) ].ucsrc ) )
#define UBRRH( _usart )                                                                 \
    ( * ( s_reg_tbl[ ( _usart ) ].ubrrh ) )
#define UBRRL( _usart )                                                                 \
    ( * ( s_reg_tbl[ ( _usart ) ].ubrrl ) )

// Get USART buffer info
#define RX_BUF( _usart, _idx )                                                          \
    ( s_rx_buf[ ( _usart ) ][ ( _idx ) ] )
#define RX_HEAD( _usart )                                                               \
    ( s_rx_head[ ( _usart ) ] )
#define RX_TAIL( _usart )                                                               \
    ( s_rx_tail[ ( _usart ) ] )
#define TX_BUF( _usart, _idx )                                                          \
    ( s_tx_buf[ ( _usart ) ][ ( _idx ) ] )
#define TX_HEAD( _usart )                                                               \
    ( s_tx_head[ usart ] )
#define TX_TAIL( _usart )                                                               \
    ( s_tx_tail[ usart ] )

// Verify assumption that bit numbers are the same for each USART
_Static_assert( DOR0 == DOR1 &&
                FE0 == FE1 &&
                RXC0 == RXC1 &&
                RXCIE0 == RXCIE1 &&
                RXEN0 == RXEN1 &&
                TXC0 == TXC1 &&
                TXCIE0 == TXCIE1 &&
                TXEN0 == TXEN1 &&
                U2X0 == U2X1 &&
                UCSZ00 == UCSZ10 &&
                UCSZ01 == UCSZ11 &&
                UCSZ02 == UCSZ12 &&
                UDRE0 == UDRE1 &&
                UDRIE0 == UDRIE1 &&
                UPE0 == UPE1 &&
                UPM00 == UPM10 &&
                UPM01 == UPM11 &&
                USBS0 == USBS1,
                "USART bit numbers must be the same!" );

/* --------------------------------------------------- VARIABLES ---------------------------------------------------- */

// Asynchronous receive buffer
static byte_t s_rx_buf[ USART_COUNT ][ RX_BUF_SIZE ];
static size_t s_rx_head[ USART_COUNT ];
static size_t s_rx_tail[ USART_COUNT ];

// Asynchronous transmit buffer
static byte_t s_tx_buf[ USART_COUNT ][ TX_BUF_SIZE ];
static size_t s_tx_head[ USART_COUNT ];
static size_t s_tx_tail[ USART_COUNT ];

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      autoconfigure_baud( usart_t )
 * @brief   Automatically configures the baud rate for the specified USART based on the `BAUD` macro.
 */
static void autoconfigure_baud( usart_t usart );

/**
 * @fn      isr_rx_complete( usart_t )
 * @brief   Runs the USART RX complete interrupt service routine for the specified USART.
 */
static void isr_rx_complete( usart_t usart, event_t event );

/**
 * @fn      isr_tx_complete( usart_t )
 * @brief   Runs the USART TX complete interrupt service routine for the specified USART.
 */
static void isr_tx_complete( usart_t usart, event_t event );

/**
 * @fn      isr_udre( usart_t )
 * @brief   Runs the USART data register empty interrupt service routine for the specified USART.
 */
static void isr_udre( usart_t usart );

/**
 * @fn      rx_buf_avail( usart_t )
 * @brief   Returns the number of available bytes in the RX buffer for the specified USART.
 */
static size_t rx_buf_avail( usart_t usart );

/**
 * @fn      rx_buf_count( usart_t )
 * @brief   Returns the number of bytes in the RX buffer for the specified USART.
 */
static size_t rx_buf_count( usart_t usart );

/**
 * @fn      set_data_bits( usart_t, usart_data_bits_t )
 * @brief   Sets the data bits setting for the specified USART.
 */
static void set_data_bits( usart_t usart, usart_data_bits_t data_bits );

/**
 * @fn      set_data_empty_intrpt_enabled( usart_t, bool )
 * @brief   Enables or disables the data register empty interrupt for the specified USART.
 */
static void set_data_empty_intrpt_enabled( usart_t usart, bool enabled );

/**
 * @fn      set_parity( usart_t, usart_parity_t )
 * @brief   Sets the parity setting for the specified USART.
 */
static void set_parity( usart_t usart, usart_parity_t parity );

/**
 * @fn      set_stop_bits( usart_t, usart_stop_bits_t )
 * @brief   Sets the stop bits setting for the specified USART.
 */
static void set_stop_bits( usart_t usart, usart_stop_bits_t stop_bits );

/**
 * @fn      set_rx_cmplt_intrpt_enabled( usart_t, bool )
 * @brief   Enables or disables the RX complete interrupt for the specified USART.
 */
static void set_rx_cmplt_intrpt_enabled( usart_t usart, bool enabled );

/**
 * @fn      set_rx_enabled( usart, bool )
 * @brief   Enables or disables RX on the specified USART.
 */
static void set_rx_enabled( usart_t usart, bool enabled );

/**
 * @fn      set_tx_cmplt_intrpt_enabled( usart_t, bool )
 * @brief   Enables or disables the TX complete interrupt for the specified USART.
 */
static void set_tx_cmplt_intrpt_enabled( usart_t usart, bool enabled );

/**
 * @fn      set_tx_enabled( usart_t, bool )
 * @brief   Enables or disables TX on the specified USART.
 */
static void set_tx_enabled( usart_t usart, bool enabled );

/**
 * @fn      tx_buf_avail( usart_t )
 * @brief   Returns the number of available bytes in the TX buffer for the specified USART.
 */
static size_t tx_buf_avail( usart_t usart );

/**
 * @fn      tx_buf_count( usart_t )
 * @brief   Returns the number of bytes in the TX buffer for the specified USART.
 */
static size_t tx_buf_count( usart_t usart );

/**
 * @fn      read( usart_t )
 * @brief   Immediately returns the contents of the specified USART's data register.
 */
static byte_t read( usart_t usart );

/**
 * @fn      wait_data_register_empty( usart_t )
 * @brief   Waits for the UDREn bit to be set for the specified USART.
 */
static void wait_data_register_empty( usart_t usart );

/**
 * @fn      wait_rx_complete( usart_t )
 * @brief   Waits for the RXCn bit to be set for the specified USART.
 */
static void wait_rx_complete( usart_t usart ) FUNC_MAY_BE_UNUSED;

/**
 * @fn      wait_tx_complete( usart_t )
 * @brief   Waits for the TXCn bit to be set for the specified USART.
 */
static void wait_tx_complete( usart_t usart ) FUNC_MAY_BE_UNUSED;

/**
 * @fn      write( usart_t )
 * @brief   Immediately sets the contents of the specified USART's data register.
 */
static void write( usart_t usart, byte_t byte );

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

void usart_deinit( usart_t usart )
{
    validate_usart( usart );

    // Disable RX and TX
    set_rx_enabled( usart, false );
    set_tx_enabled( usart, false );

    // Disable interrupts
    set_rx_cmplt_intrpt_enabled( usart, false );
    set_tx_cmplt_intrpt_enabled( usart, false );
    set_data_empty_intrpt_enabled( usart, false );

}   /* usart_deinit() */


usart_error_t usart_get_errors( usart_t usart )
{
    validate_usart( usart );

    usart_error_t error = USART_ERROR_NONE;

    if( is_bit_set( UCSRA( usart ), FE0 ) )
        set_bit( error, USART_ERROR_FRAME_ERROR );
    if( is_bit_set( UCSRA( usart ), UPE0 ) )
        set_bit( error, USART_ERROR_PARITY_ERROR );
    if( is_bit_set( UCSRA( usart ), DOR0 ) )
        set_bit( error, USART_ERROR_DATA_OVERRUN );

    return( error );

}   /* usart_get_errors() */


void usart_init( usart_t usart,
                 bool rx_enabled,
                 bool tx_enabled,
                 usart_data_bits_t data_bits,
                 usart_stop_bits_t stop_bits,
                 usart_parity_t parity )
{
    validate_usart( usart );
    validate_data_bits( data_bits );
    validate_stop_bits( stop_bits );
    validate_parity( parity );

    // Initialize RX buffer
    RX_HEAD( usart ) = 0;
    RX_TAIL( usart ) = 0;

    // Initialize TX buffer
    TX_HEAD( usart ) = 0;
    TX_TAIL( usart ) = 0;

    // Configure USART
    autoconfigure_baud( usart );
    set_data_bits( usart, data_bits );
    set_stop_bits( usart, stop_bits );
    set_parity( usart, parity );

    // Enable RX interrupt, if required
    set_rx_cmplt_intrpt_enabled( usart, rx_enabled );

    // Enable USART
    set_rx_enabled( usart, rx_enabled );
    set_tx_enabled( usart, tx_enabled );

}   /* usart_init() */


size_t usart_max_rx_size( void )
{
    return( RX_BUF_SIZE );

}   /* usart_max_rx_size() */


size_t usart_max_tx_size( void )
{
    return( TX_BUF_SIZE );

}   /* usart_max_tx_size() */


size_t usart_rx( usart_t usart, byte_t * data, size_t max_size )
{
    validate_usart( usart );

    // Do nothing for zero-sized data buffers
    if( max_size == 0 )
        return( 0 );

    // Copy bytes as long as possible
    size_t count = 0;
    while( rx_buf_count( usart ) > 0 && ( max_size-- ) > 0 )
    {
        data[ count++ ] = RX_BUF( usart, RX_TAIL( usart ) );
        increment_rollover( RX_TAIL( usart ), RX_BUF_SIZE );
    }

    return( count );

}   /* usart_rx() */


bool usart_tx( usart_t usart, byte_t const * data, size_t size )
{
    validate_usart( usart );

    // Do nothing if there's no data
    if( size == 0 )
        return( true );

    // Ensure we have enough room in the TX buffer
    if( tx_buf_avail( usart ) < size )
        return( false );

    // Copy data into TX buffer
    for( size_t idx = 0; idx < size; idx++ )
    {
        TX_BUF( usart, TX_HEAD( usart ) ) = data[ idx ];
        increment_rollover( TX_HEAD( usart ), TX_BUF_SIZE );
    }

    // Enable interrupt to start transfer
    set_data_empty_intrpt_enabled( usart, true );
    return( true );

}   /* usart_tx() */


bool usart_tx_str( usart_t usart, char const * str )
{
    return( usart_tx( usart, ( byte_t const * )str, strlen( str ) ) );

}   /* usart_tx_str() */


void usart_tx_sync( usart_t usart, byte_t const * data, size_t size )
{
    // Spin loop until USART is ready
    wait_data_register_empty( usart );

    // Disable interrupts, if required
    bool intrpt_en = sys_intrpt_enabled();
    sys_cli();

    // Transmit each byte in order
    for( size_t idx = 0; idx < size; idx++ )
    {
        write( usart, data[ idx ] );
        wait_data_register_empty( usart );
    }

    // Re-enable interrupts, if required
    sys_set_intrpt_enabled( intrpt_en );

}   /* usart_tx_sync() */


void usart_tx_sync_str( usart_t usart, char const * str )
{
    usart_tx_sync( usart, ( byte_t const * )str, strlen( str ) );

}   /* usart_tx_sync_str() */


static void autoconfigure_baud( usart_t usart )
{
    UBRRH( usart ) = UBRRH_VALUE;
    UBRRL( usart ) = UBRRL_VALUE;
    #if( USE_2X )
        set_bit( UCSRA( usart ), U2X0 );
    #else
        clear_bit( UCSRA( usart ), U2X0 );
    #endif

}   /* autoconfigure_baud() */


static void isr_rx_complete( usart_t usart, event_t event )
{
    // If the buffer is full, drop the oldest received byte
    if( rx_buf_avail( usart ) == 0 )
        increment_rollover( RX_TAIL( usart ), RX_BUF_SIZE );

    // TODO: errors???

    // Read the next byte from the USART
    RX_BUF( usart, RX_HEAD( usart ) ) = read( usart );
    increment_rollover( RX_HEAD( usart ), RX_BUF_SIZE );

    // Set system event
    sys_enqueue_event( event );

}   /* isr_rx_complete() */


static void isr_tx_complete( usart_t usart, event_t event )
{
    ( void )usart;

    // Set system event
    sys_enqueue_event( event );

}   /* isr_tx_complete() */


static void isr_udre( usart_t usart )
{
    // If no data is left, disable the interrupt and return
    if( tx_buf_count( usart ) == 0 )
    {
        set_data_empty_intrpt_enabled( usart, false );
        return;
    }

    // Write the next byte to the USART
    write( usart, TX_BUF( usart, TX_TAIL( usart ) ) );
    increment_rollover( TX_TAIL( usart ), TX_BUF_SIZE );

}   /* isr_udre() */


static size_t rx_buf_avail( usart_t usart )
{
    return( RX_BUF_SIZE - rx_buf_count( usart ) - 1 );

}   /* rx_buf_avail() */


static size_t rx_buf_count( usart_t usart )
{
    if( RX_HEAD( usart ) >= RX_TAIL( usart ) )
        return( RX_HEAD( usart ) - RX_TAIL( usart ) );
    else
        return( RX_BUF_SIZE - RX_TAIL( usart ) + RX_HEAD( usart ) );

}   /* rx_buf_count() */


static void set_data_bits( usart_t usart, usart_data_bits_t data_bits )
{
    // Note that UCSZn2 is in UCSRnB, not UCSRnC
    switch( data_bits )
    {
    case USART_DATA_BITS_5:
        clear_bit( UCSRC( usart ), UCSZ00 );
        clear_bit( UCSRC( usart ), UCSZ01 );
        clear_bit( UCSRB( usart ), UCSZ02 );
        break;

    case USART_DATA_BITS_6:
        set_bit(   UCSRC( usart ), UCSZ00 );
        clear_bit( UCSRC( usart ), UCSZ01 );
        clear_bit( UCSRB( usart ), UCSZ02 );
        break;

    case USART_DATA_BITS_7:
        clear_bit( UCSRC( usart ), UCSZ00 );
        set_bit(   UCSRC( usart ), UCSZ01 );
        clear_bit( UCSRB( usart ), UCSZ02 );
        break;

    case USART_DATA_BITS_8:
        set_bit(   UCSRC( usart ), UCSZ00 );
        set_bit(   UCSRC( usart ), UCSZ01 );
        clear_bit( UCSRB( usart ), UCSZ02 );
        break;
    }

}   /* set_data_bits() */


static void set_data_empty_intrpt_enabled( usart_t usart, bool enabled )
{
    assign_bit( UCSRB( usart ), UDRIE0, enabled );

}   /* set_data_empty_intrpt_enabled() */


static void set_parity( usart_t usart, usart_parity_t parity )
{
    switch( parity )
    {
    case USART_PARITY_DISABLED:
        clear_bit( UCSRC( usart ), UPM00 );
        clear_bit( UCSRC( usart ), UPM01 );
        break;

    case USART_PARITY_EVEN:
        clear_bit( UCSRC( usart ), UPM00 );
        set_bit(   UCSRC( usart ), UPM01 );
        break;

    case USART_PARITY_ODD:
        set_bit(   UCSRC( usart ), UPM00 );
        set_bit(   UCSRC( usart ), UPM01 );
        break;
    }

}   /* set_parity() */


static void set_stop_bits( usart_t usart, usart_stop_bits_t stop_bits )
{
    switch( stop_bits )
    {
    case USART_STOP_BITS_1:
        clear_bit( UCSRC( usart ), USBS0 );
        break;

    case USART_STOP_BITS_2:
        set_bit( UCSRC( usart ), USBS0 );
        break;
    }

}   /* set_stop_bits() */


static void set_rx_cmplt_intrpt_enabled( usart_t usart, bool enabled )
{
    assign_bit( UCSRB( usart ), RXCIE0, enabled );

}   /* set_rx_complt_intrpt_enabled() */


static void set_rx_enabled( usart_t usart, bool enabled )
{
    assign_bit( UCSRB( usart ), RXEN0, enabled );

}   /* set_rx_enabled() */


static void set_tx_cmplt_intrpt_enabled( usart_t usart, bool enabled )
{
    assign_bit( UCSRB( usart ), TXCIE0, enabled );

}   /* set_tx_cmplt_intrpt_enabled() */


static void set_tx_enabled( usart_t usart, bool enabled )
{
    assign_bit( UCSRB( usart ), TXEN0, enabled );

}   /* set_tx_enabled() */


static size_t tx_buf_avail( usart_t usart )
{
    return( TX_BUF_SIZE - tx_buf_count( usart ) - 1 );

}   /* tx_buf_avail() */


static size_t tx_buf_count( usart_t usart )
{
    if( TX_HEAD( usart ) >= TX_TAIL( usart ) )
        return( TX_HEAD( usart ) - TX_TAIL( usart ) );
    else
        return( TX_BUF_SIZE - TX_TAIL( usart ) + TX_HEAD( usart ) );

}   /* tx_buf_count() */


static byte_t read( usart_t usart )
{
    return( ( byte_t )UDR( usart ) );

}   /* read() */


static void wait_data_register_empty( usart_t usart )
{
    wait_bit_set( UCSRA( usart ), UDRE0 );

}   /* wait_data_register_empty() */


static void wait_rx_complete( usart_t usart )
{
    wait_bit_set( UCSRA( usart ), RXC0 );

}   /* wait_rx_complete() */


static void wait_tx_complete( usart_t usart )
{
    wait_bit_set( UCSRA( usart ), TXC0 );

}   /* wait_tx_complete() */


static void write( usart_t usart, byte_t byte )
{
    UDR( usart ) = ( uint8_t )byte;

}   /* write() */


ISR( USART0_RX_vect )
{
    isr_rx_complete( USART_0, EVENT_USART_0_RX_COMPLETE );

}   /* ISR( USART0_RX_vect ) */


ISR( USART0_TX_vect )
{
    isr_tx_complete( USART_0, EVENT_USART_0_TX_COMPLETE );

}   /* ISR( USART0_TX_vect ) */


ISR( USART0_UDRE_vect )
{
    isr_udre( USART_0 );

}   /* ISR( USART0_UDRE_vect ) */


ISR( USART1_RX_vect )
{
    isr_rx_complete( USART_1, EVENT_USART_1_RX_COMPLETE );

}   /* ISR( USART1_RX_vect ) */


ISR( USART1_TX_vect )
{
    isr_tx_complete( USART_1, EVENT_USART_1_TX_COMPLETE );

}   /* ISR( USART1_TX_vect ) */


ISR( USART1_UDRE_vect )
{
    isr_udre( USART_1 );

}   /* ISR( USART1_UDRE_vect ) */
