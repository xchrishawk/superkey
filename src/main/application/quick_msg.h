/**
 * @file    src/main/application/quick_msg.h
 * @brief   Header for the quick message module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-09-08
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 */

#if !defined( APPLICATION_QUICK_MSG_H )
#define APPLICATION_QUICK_MSG_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "utility/types.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

/**
 * @def     QUICK_MSG_IDX_COUNT
 * @brief   The number of valid quick message indices.
 * @note    This represents the maximum number of quick messages that the system supports.
 */
#define QUICK_MSG_IDX_COUNT     ( 16 )

/**
 * @def     QUICK_MSG_MAX_LEN
 * @brief   Maximum length of each quick emssage, including the null terminator (in bytes).
 */
#define QUICK_MSG_MAX_LEN       ( 64 )

/* ----------------------------------------------------- TYPES ------------------------------------------------------ */

/**
 * @typedef quick_msg_idx_t
 * @brief   Typedef representing the index of a quick message.
 * @note    Must be less than `QUICK_MSG_IDX_COUNT`.
 */
typedef uint8_t quick_msg_idx_t;

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      quick_msg_get( quick_msg_idx_t )
 * @brief   Gets the text of the specified quick message.
 */
char const * quick_msg_get( quick_msg_idx_t idx );

/**
 * @fn      quick_msg_init( void )
 * @brief   Initializes the quick message system.
 * @note    Requires that the following modules have been initialized:
 *          - `storage`
 *          - `keyer`
 */
void quick_msg_init( void );

/**
 * @fn      quick_msg_invalidate( quick_msg_idx_t )
 * @brief   Invalidates (i.e., clears) the specified quick message.
 */
void quick_msg_invalidate( quick_msg_idx_t idx );

/**
 * @fn      quick_msg_is_valid( quick_msg_idx_t )
 * @brief   Returns `true` if the specified quick message is not empty.
 */
bool quick_msg_is_valid( quick_msg_idx_t idx );

/**
 * @fn      quick_msg_key( quick_msg_idx_t )
 * @brief   Immediately queues the specified quick message to be keyed.
 */
size_t quick_msg_key( quick_msg_idx_t idx );

/**
 * @fn      quick_msg_set( quick_msg_idx_t, char const * )
 * @brief   Sets the text of the specified quick message.
 */
void quick_msg_set( quick_msg_idx_t idx, char const * message );

/**
 * @fn      quick_msg_tick( tick_t )
 * @brief   Performs periodic processing at the specified tick.
 */
void quick_msg_tick( tick_t tick );

#endif /* !defined( QUICK_MSG_H ) */
