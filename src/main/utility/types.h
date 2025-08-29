/**
 * @file    src/main/utility/types.h
 * @brief   Header defining generic types.
 * @note        This header provides a "safe" place to declare types in order to prevent circular references.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-16
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 */

#if !defined( UTILITY_TYPES_H )
#define UTILITY_TYPES_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdint.h>

/* ----------------------------------------------------- TYPES ------------------------------------------------------ */

/**
 * @typedef byte_t
 * @brief   Typedef for a single unsigned byte.
 */
typedef uint8_t byte_t;

/**
 * @typedef register_t
 * @brief   Typedef for a pointer to a memory-mapped register.
 */
typedef volatile byte_t * register_t;

/**
 * @typedef tick_t
 * @brief   Typedef representing the system tick count.
 */
typedef uint32_t tick_t;

/* ----------------------------------------------------- MACROS ----------------------------------------------------- */

/**
 * @def     register_addr( _reg )
 * @brief   Returns the address of the specified register as a `register_t`.
 */
#define register_addr( _reg )                                                           \
    ( ( register_t )( & ( _reg ) ) )

#endif /* !defined( UTILITY_TYPES_H ) */
