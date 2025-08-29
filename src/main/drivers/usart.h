/**
 * @file    src/main/drivers/usart.h
 * @brief   Header for the USART driver module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-17
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 */

#if !defined( DRIVERS_USART_H )
#define DRIVERS_USART_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "utility/types.h"
#include "utility/utility.h"

/* ----------------------------------------------------- TYPES ------------------------------------------------------ */

/**
 * @typedef usart_t
 * @brief   Enumeration of the USARTs supported by the system.
 */
typedef uint8_t usart_t;
enum
{
    USART_0,                                /**< USART 0.                               */
    USART_1,                                /**< USART 1.                               */

    USART_COUNT,                            /**< Number of valid USARTs.                */
};

/**
 * @typedef usart_data_bits_t
 * @brief   Enumeration of the supported data bits settings for the USARTs.
 * @note    9-bit words are not supported.
 */
typedef uint8_t usart_data_bits_t;
enum
{
    USART_DATA_BITS_5,                      /**< Each word has 5 data bits.             */
    USART_DATA_BITS_6,                      /**< Each word has 6 data bits.             */
    USART_DATA_BITS_7,                      /**< Each word has 7 data bits.             */
    USART_DATA_BITS_8,                      /**< Each word has 8 data bits.             */

    USART_DATA_BITS_COUNT,                  /**< Number of valid data bits settings.    */
};

/**
 * @typedef usart_error_t
 * @brief   Enumeration of the errors which may be returned by a USART.
 */
typedef uint8_t usart_error_t;
enum
{
    USART_ERROR_NONE            = 0,            /**< No error occurred.                 */
    USART_ERROR_FRAME_ERROR     = bitmask( 0 ), /**< Frame error occurred.              */
    USART_ERROR_PARITY_ERROR    = bitmask( 1 ), /**< Parity error occurred.             */
    USART_ERROR_DATA_OVERRUN    = bitmask( 2 ), /**< Data overrun occurred.             */
};

/**
 * @typedef usart_parity_t
 * @brief   Enumeration of the supported parity settings for the USARTs.
 */
typedef uint8_t usart_parity_t;
enum
{
    USART_PARITY_DISABLED,                  /**< Parity bit is disabled.                */
    USART_PARITY_EVEN,                      /**< Even parity bit is enabled.            */
    USART_PARITY_ODD,                       /**< Odd parity bit is enabled.             */

    USART_PARITY_COUNT,                     /**< Number of valid parity settings.       */
};

/**
 * @typedef usart_stop_bits_t
 * @brief   Enumeration of the supported stop bits settings for the USARTs.
 */
typedef uint8_t usart_stop_bits_t;
enum
{
    USART_STOP_BITS_1,                      /**< Each word has 1 stop bit.              */
    USART_STOP_BITS_2,                      /**< Each word has 2 stop bits.             */

    USART_STOP_BITS_COUNT,                  /**< Number of valid stop bits settings.    */
};

/**
 * @typedef usart_wait_mode_t
 * @brief   Enumeration of supported wait modes for calls to `usart_tx()`.
 */
typedef uint8_t usart_wait_mode_t;
enum
{
    /** The call to `usart_tx()` will block until all data has been written to the TX buffer. */
    USART_WAIT_MODE_NORMAL,

    /** The call to `usart_tx()` will block until all data has been transmitted by the USART. */
    USART_WAIT_MODE_TX_COMPLETE,

    /** The call to `usart_tx()` will not block, and will only write as many bytes as will fit in the TX buffer. */
    USART_WAIT_MODE_NONE,

    /** Number of valid wait modes. */
    USART_WAIT_MODE_COUNT,
};

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      usart_deinit( usart_t )
 * @brief   De-initializes the specified USART, making it unusable until re-initialized.
 */
void usart_deinit( usart_t usart );

/**
 * @fn      usart_get_errors( usart_t )
 * @brief   Returns the currently set error flags for the specified USART.
 * @note    Does not currently return anything meaningful.
 */
usart_error_t usart_get_errors( usart_t usart );

/**
 * @fn      usart_init( usart_t, bool, bool, usart_data_bits_t, usart_stop_bits_t, usart_parity_t )
 * @brief   Initializes the specified USART with the specified configuration.
 * @note    Must be called prior to attempting any use of the USART.
 */
void usart_init( usart_t usart,
                 bool rx_enabled,
                 bool tx_enabled,
                 usart_data_bits_t data_bits,
                 usart_stop_bits_t stop_bits,
                 usart_parity_t parity );

/**
 * @fn      usart_max_rx_size( void )
 * @brief   Returns the maximum supported RX buffer size for all USARTs.
 */
size_t usart_max_rx_size( void );

/**
 * @fn      usart_max_tx_size( void )
 * @brief   Returns the maximum supported TX buffer size for all USARTs.
 */
size_t usart_max_tx_size( void );

/**
 * @fn      usart_rx( usart_t, byte_t *, size_t )
 * @brief   Receives up to `max_size` bytes from the RX buffer for the specified USART.
 * @returns The number of bytes read into `data`.
 */
size_t usart_rx( usart_t usart, byte_t * data, size_t max_size );

/**
 * @fn      usart_tx( usart_t, byte_t const *, size_t )
 * @brief   Tranmits the specified data buffer.
 * @returns The number of bytes successfully transmitted.
 */
size_t usart_tx( usart_t usart, byte_t const * data, size_t size, usart_wait_mode_t wait_mode );

/**
 * @fn      usart_tx_str( usart_t, char const * )
 * @brief   Transmits the specified string.
 * @returns The number of bytes successfully transmitted.
 */
size_t usart_tx_str( usart_t usart, char const * str, usart_wait_mode_t wait_mode );

#endif /* !defined( DRIVERS_USART_H ) */