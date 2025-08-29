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
 * @typedef usart_baud_t
 * @brief   Enumeration of the supported USART baud settings.
 * @note    The approximate error percentage for each setting is indicated in the comment. Baud settings with an error
 *          of more than 0.5% are not recommended.
 */
typedef uint8_t usart_baud_t;
enum
{
    USART_BAUD_2400,                        /**< 2400 baud (0.0% error).                */
    USART_BAUD_4800,                        /**< 4800 baud (-0.1% error).               */
    USART_BAUD_9600,                        /**< 9600 baud (0.2% error).                */
    USART_BAUD_14400,                       /**< 14400 baud (-0.1% error).              */
    USART_BAUD_19200,                       /**< 19200 baud (0.2% error).               */
    USART_BAUD_28800_NOT_RECOMMENDED,       /**< 28800 baud (0.6% error - N/R).         */
    USART_BAUD_38400,                       /**< 38400 baud (0.2% error).               */
    USART_BAUD_57600_NOT_RECOMMENDED,       /**< 57600 baud (-0.8% error - N/R).        */
    USART_BAUD_76800,                       /**< 76800 baud (0.2% error).               */
    USART_BAUD_115200_NOT_RECOMMENDED,      /**< 115200 baud (2.1% error - N/R).        */
    USART_BAUD_230400_NOT_RECOMMENDED,      /**< 230400 baud (-3.5% error - N/R).       */
    USART_BAUD_250000,                      /**< 250000 baud (0.0% error).              */

    USART_BAUD_COUNT,                       /**< Number of valid USART baud settings.   */
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
 * @fn      usart_init( usart_t, bool, bool, usart_baud_t, usart_data_bits_t, usart_stop_bits_t, usart_parity_t )
 * @brief   Initializes the specified USART with the specified configuration.
 * @note    Must be called prior to attempting any use of the USART.
 */
void usart_init( usart_t usart,
                 bool rx_enabled,
                 bool tx_enabled,
                 usart_baud_t baud,
                 usart_data_bits_t data_bits,
                 usart_stop_bits_t stop_bits,
                 usart_parity_t parity );

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