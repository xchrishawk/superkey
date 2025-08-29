/**
 * @file    src/main/application/keyer.h
 * @brief   Header for the keyer module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-19
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 */

#if !defined( APPLICATION_KEYER_H )
#define APPLICATION_KEYER_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdint.h>
#include <stdlib.h>

#include "utility/types.h"

/* ----------------------------------------------------- TYPES ------------------------------------------------------ */

/**
 * @typedef keyer_paddle_mode_t
 * @brief   Enumeration of the paddle modes that the keyer supports.
 */
typedef uint8_t keyer_paddle_mode_t;
enum
{
    /**
     * In the `KEYER_PADDLE_MODE_IAMBIC` mode, pressing both paddles simultaneously will emit a stream of dashes and
     * dots, starting with whichever one was pressed first.
     */
    KEYER_PADDLE_MODE_IAMBIC,

    /**
     * In the `KEYER_PADDLE_MODE_ULTIMATIC` mode, pressing both paddles simultaneously will emit a stream of either dots
     * or dashes, depending on which paddle was pressed *first*. For example, if the left paddle is held down, and the
     * right paddle is then held down (while the left is still held down), the keyer will emit dots.
     */
    KEYER_PADDLE_MODE_ULTIMATIC,

    /**
     * In the `KEYER_PADDLE_MODE_ULTIMATIC_ALTERNATE` mode, pressing both paddles simultaneously will emit a stream of
     * either dots or dashes, depending on which paddle was pressed *most recently*. For example, if the left paddle is
     * held down, and the right paddle is then held down (while the left is still held down), the keyer will emit
     * dashes.
     */
    KEYER_PADDLE_MODE_ULTIMATIC_ALTERNATE,

    /** Number of valid keyer paddle modes. */
    KEYER_PADDLE_MODE_COUNT,
};

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      keyer_autokey_char( char )
 * @brief   Adds the specified character to the keyer's autokey buffer.
 * @returns `true` if the character was successfully queued.
 */
bool keyer_autokey_char( char c );

/**
 * @fn      keyer_autokey( char const * )
 * @brief   Adds the specified string to the keyer's autokey buffer.
 * @returns The number of characters that were successfully queued.
 */
size_t keyer_autokey_str( char const * str );

/**
 * @fn      keyer_get_on( void )
 * @brief   Returns `true` if the keyer is currently commanding the radio to transmit.
 */
bool keyer_get_on( void );

/**
 * @fn      keyer_get_paddle_invert( void )
 * @brief   Returns `true` if the keyer is configured to invert the paddles. In this case, the right paddle will emit
 *          dots and the left paddle will emit dashes.
 */
bool keyer_get_paddle_invert( void );

/**
 * @fn      keyer_get_paddle_mode( void )
 * @brief   Returns the keyer's currently active mode.
 */
keyer_paddle_mode_t keyer_get_paddle_mode( void );

/**
 * @fn      keyer_init( void )
 * @brief   Initializes the keyer module.
 */
void keyer_init( void );

/**
 * @fn      keyer_panic( void )
 * @brief   Immediately and unconditionally stops the keyer, and clears any pending autokey characters.
 */
void keyer_panic( void );

/**
 * @fn      keyer_set_paddle_invert( bool )
 * @brief   Enables or disables the "invert paddles" setting. If set to `true`, the right paddle will emit dots and the
 *          left paddle will emit dashes.
 * @note    This modifies the application configuration.
 */
void keyer_set_paddle_invert( bool invert );

/**
 * @fn      keyer_set_paddle_mode( keyer_paddle_mode_t )
 * @brief   Sets the keyer's currently active paddle mode.
 * @note    This modifies the application configuration.
 */
void keyer_set_paddle_mode( keyer_paddle_mode_t mode );

/**
 * @fn      keyer_tick( tick_t )
 * @brief   Performs periodic processing at the specified tick count.
 * @note    It is not required to call this function at a uniform rate.
 */
void keyer_tick( tick_t tick );

#endif /* !defined( APPLICATION_KEYER_H ) */
