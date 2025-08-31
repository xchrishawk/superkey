/**
 * @file    src/main/application/intf_types.h
 * @brief   Header defining types for the interface port.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-29
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 */

#if !defined( APPLICATION_INTF_TYPES_H )
#define APPLICATION_INTF_TYPES_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdint.h>

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
    INTF_MESSAGE_REQUEST_PANIC,             /**< Immediately stop the keyer.            */
    INTF_MESSAGE_REQUEST_PING,              /**< Check if device is alive.              */
    INTF_MESSAGE_REQUEST_RESTORE_DEFAULT_CONFIG,/**< Restores default configuration.    */
    INTF_MESSAGE_REQUEST_SET_BUZZER_ENABLED,/**< Enable or disable buzzer.              */
    INTF_MESSAGE_REQUEST_SET_BUZZER_FREQUENCY,/**< Set buzzer frequency.                */

    INTF_MESSAGE_REPLY_SUCCESS,             /**< Command successful / acknowledged.     */
    INTF_MESSAGE_REPLY_INVALID_MESSAGE,     /**< Request was invalid.                   */
    INTF_MESSAGE_REPLY_INVALID_SIZE,        /**< Size was invalid.                      */
    INTF_MESSAGE_REPLY_INVALID_CRC,         /**< CRC was invalid.                       */
    INTF_MESSAGE_REPLY_INVALID_PAYLOAD,     /**< Payload was invalid.                   */

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

} intf_header_t;

#endif /* !defined( APPLICATION_INTF_TYPES_H ) */
