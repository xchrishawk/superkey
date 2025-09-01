/**
 * @file    src/main/utility/types.h
 * @brief   Header defining generic types.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-16
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 *
 * @details
 * Header providing a location for shared, application-wide types.
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
 * @typedef register8_t
 * @brief   Typedef for a pointer to an 8-bit memory-mapped register.
 */
typedef volatile byte_t * register8_t;

/**
 * @typedef register16_t
 * @brief   Typedef for a pointer to a 16-bit memory-mapped register.
 */
typedef volatile uint16_t * register16_t;

/**
 * @typedef tick_t
 * @brief   Typedef representing the system tick count.
 */
typedef uint32_t tick_t;

/* ----------------------------------------------------- MACROS ----------------------------------------------------- */

/**
 * @def     register8_addr( _reg )
 * @brief   Returns the address of the specified 8-bit register as a `register8_t`.
 */
#define register8_addr( _reg )                                                          \
    ( ( register8_t )( & ( _reg ) ) )

/**
 * @def     register16_addr( _reg )
 * @brief   Returns the address of the specified 16-bit register as a `register16_t`.
 */
#define register16_addr( _reg )                                                         \
    ( ( register16_t )( & ( _reg ) ) )

#endif /* !defined( UTILITY_TYPES_H ) */
