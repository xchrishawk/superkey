/**
 * @file    src/main/application/intf_types.h
 * @brief   Header defining types for the interface port.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-29
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 *
 * @details
 * This header file defines types and constants used by the interface serial port.
 */

#if !defined( APPLICATION_INTF_TYPES_H )
#define APPLICATION_INTF_TYPES_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdint.h>

#include "utility/utility.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

/**
 * @def     INTF_MESSAGE_MAX_SIZE
 * @brief   Maximum size of an interface message.
 */
#define INTF_MESSAGE_MAX_SIZE           ( 128 )

/**
 * @def     INTF_MESSAGE_PAYLOAD_MAX_SIZE
 * @brief   Maximum payload size of an interface message.
 */
#define INTF_MESSAGE_PAYLOAD_MAX_SIZE   ( INTF_MESSAGE_MAX_SIZE - sizeof( intf_header_t ) )

/* ----------------------------------------------------- TYPES ------------------------------------------------------ */

/**
 * @typedef intf_message_t
 * @brief   Enumeration of the interface port's supported message IDs.
 * @note    This enumeration must be kept in sync with the corresponding enumeration in superkey_types.py.
 */
typedef uint16_t intf_message_t;
enum
{
    INTF_MESSAGE_REQUEST_AUTOKEY,           /**< Queues a string to be autokeyed.       */
    INTF_MESSAGE_REQUEST_AUTOKEY_COUNT,     /**< Get number of Morse code elements in autokey buffer. */
    INTF_MESSAGE_REQUEST_AUTOKEY_EX,        /**< Queues a string to be autokeyed with flags. */
    INTF_MESSAGE_REQUEST_AUTOKEY_QUICK_MSG, /**< Queues a quick message to be autokeyed.*/
    INTF_MESSAGE_REQUEST_GET_BUZZER_ENABLED,/**< Get buzzer enablement.                 */
    INTF_MESSAGE_REQUEST_GET_BUZZER_FREQUENCY,/**< Get buzzer frequency.                */
    INTF_MESSAGE_REQUEST_GET_HUMANIZER_LEVEL,/**< Get humanizer level.                  */
    INTF_MESSAGE_REQUEST_GET_INVERT_PADDLES,/**< Gets paddle inversion setting.         */
    INTF_MESSAGE_REQUEST_GET_IO_POLARITY,   /**< Gets I/O pin polarity.                 */
    INTF_MESSAGE_REQUEST_GET_IO_STATE,      /**< Gets I/O pin state.                    */
    INTF_MESSAGE_REQUEST_GET_IO_STATE_FOR_TYPE,/**< Gets I/O type state.                */
    INTF_MESSAGE_REQUEST_GET_IO_TYPE,       /**< Gets I/O pin type.                     */
    INTF_MESSAGE_REQUEST_GET_LED_ENABLED,   /**< Gets enablement for LED.               */
    INTF_MESSAGE_REQUEST_GET_PADDLE_MODE,   /**< Gets the paddle mode.                  */
    INTF_MESSAGE_REQUEST_GET_QUICK_MSG,     /**< Gets the specified quick message.      */
    INTF_MESSAGE_REQUEST_GET_TRAINER_MODE,  /**< Gets enablement for trainer mode.      */
    INTF_MESSAGE_REQUEST_GET_WPM,           /**< Get WPM setting.                       */
    INTF_MESSAGE_REQUEST_GET_WPM_SCALE,     /**< Get element WPM scale.                 */
    INTF_MESSAGE_REQUEST_INVALIDATE_QUICK_MSG,/**< Invalidate quick message.            */
    INTF_MESSAGE_REQUEST_PANIC,             /**< Immediately stop the keyer.            */
    INTF_MESSAGE_REQUEST_PING,              /**< Check if device is alive.              */
    INTF_MESSAGE_REQUEST_RESTORE_DEFAULT_CONFIG,/**< Restores default configuration.    */
    INTF_MESSAGE_REQUEST_SET_BUZZER_ENABLED,/**< Enable or disable buzzer.              */
    INTF_MESSAGE_REQUEST_SET_BUZZER_FREQUENCY,/**< Set buzzer frequency.                */
    INTF_MESSAGE_REQUEST_SET_HUMANIZER_LEVEL,/**< Set humanizer level.                  */
    INTF_MESSAGE_REQUEST_SET_INVERT_PADDLES,/**< Sets paddle inversion setting.         */
    INTF_MESSAGE_REQUEST_SET_IO_POLARITY,   /**< Sets I/O pin polarity.                 */
    INTF_MESSAGE_REQUEST_SET_IO_TYPE,       /**< Sets I/O pin type.                     */
    INTF_MESSAGE_REQUEST_SET_LED_ENABLED,   /**< Sets enablement for LED.               */
    INTF_MESSAGE_REQUEST_SET_PADDLE_MODE,   /**< Sets the paddle mode.                  */
    INTF_MESSAGE_REQUEST_SET_QUICK_MSG,     /**< Sets the specified quick message.      */
    INTF_MESSAGE_REQUEST_SET_TRAINER_MODE,  /**< Set enablement for trainer mode.       */
    INTF_MESSAGE_REQUEST_SET_WPM,           /**< Set WPM.                               */
    INTF_MESSAGE_REQUEST_SET_WPM_SCALE,     /**< Set WPM scale.                         */
    INTF_MESSAGE_REQUEST_VERSION,           /**< Get application version information.   */

    INTF_MESSAGE_REPLY_SUCCESS,             /**< Command successful / acknowledged.     */
    INTF_MESSAGE_REPLY_INVALID_MESSAGE,     /**< Request was invalid.                   */
    INTF_MESSAGE_REPLY_INVALID_SIZE,        /**< Size was invalid.                      */
    INTF_MESSAGE_REPLY_INVALID_CRC,         /**< CRC was invalid.                       */
    INTF_MESSAGE_REPLY_INVALID_PAYLOAD,     /**< Payload was invalid.                   */
    INTF_MESSAGE_REPLY_INVALID_VALUE,       /**< A specified value was invalid.         */

    INTF_MESSAGE_COUNT,                     /**< Number of valid message IDs.           */
};

/**
 * @struct  intf_header_t
 * @brief   Struct representing the header for an interface port message.
 */
typedef struct
{
    intf_message_t  message;                /**< Interface message ID.                  */
    uint16_t        reserved;               /**< Reserved / unused.                     */
    uint16_t        size;                   /**< Total size of message payload.         */
    uint16_t        crc;                    /**< 16-bit CRC of message payload.         */

} PACKED_STRUCT intf_header_t;

#endif /* !defined( APPLICATION_INTF_TYPES_H ) */
