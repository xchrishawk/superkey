/**
 * @file    types.h
 * @brief   Header defining generic types.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-16
 */

#if !defined( EXAKEY_TYPES_H )
#define EXAKEY_TYPES_H

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

/* ----------------------------------------------------- MACROS ----------------------------------------------------- */

/**
 * @def     register_addr( _reg )
 * @brief   Returns the address of the specified register as a `register_t`.
 */
#define register_addr( _reg )                                                   \
    ( ( register_t )( & ( _reg ) ) )

#endif /* !defined( EXAKEY_TYPES_H) */
