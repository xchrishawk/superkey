/**
 * @file    src/main/application/keyer.h
 * @brief   Header for the keyer module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-19
 */

#if !defined( APPLICATION_KEYER_H )
#define APPLICATION_KEYER_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdint.h>

#include "utility/types.h"

/* ----------------------------------------------------- TYPES ------------------------------------------------------ */

/**
 * @typedef keyer_mode_t
 * @brief   Enumeration of the supported keyer modes.
 */
typedef uint8_t keyer_mode_t;
enum
{
    /**
     * In the `KEYER_MODE_IAMBIC` mode, pressing both paddles simultaneously will emit a stream of dashes and dots,
     * starting with whichever one was pressed first.
     */
    KEYER_MODE_IAMBIC,

    /**
     * In the `KEYER_MODE_ULTIMATIC` mode, pressing both paddles simultaneously will emit a stream of either dots or
     * dashes, depending on which paddle was pressed *most recently*. For example, if the left paddle is held down, and
     * the right paddle is then held down (while the left is still held down), the keyer will emit dashes.
     */
    KEYER_MODE_ULTIMATIC,

    /**
     * In the `KEYER_MODE_ULTIMATIC_ALTERNATE` mode, pressing both paddles simultaneously will emit a stream of either
     * dots or dashes, depending on which paddle was pressed *first*. For example, if the left paddle is held down, and
     * the right paddle is then held down (while the left is still held down), the keyer will emit dots.
     */
    KEYER_MODE_ULTIMATIC_ALTERNATE,

    /** Number of valid keyer modes. */
    KEYER_MODE_COUNT,
};

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      keyer_get_invert_paddles( void )
 * @brief   Returns `true` if the keyer is configured to invert the paddles. In this case, the right paddle will emit
 *          dots and the left paddle will emit dashes.
 */
bool keyer_get_invert_paddles( void );

/**
 * @fn      keyer_get_mode( void )
 * @brief   Returns the currently active keyer mode.
 */
keyer_mode_t keyer_get_mode( void );

/**
 * @fn      keyer_get_output_active_low( void )
 * @brief   Returns `true` if the keyer's output is active low.
 */
bool keyer_get_output_active_low( void );

/**
 * @fn      keyer_init( void )
 * @brief   Initializes the keyer module.
 */
void keyer_init( void );

/**
 * @fn      keyer_panic( void )
 * @brief   Immediately and unconditionally stops the keyer.
 */
void keyer_panic( void );

/**
 * @fn      keyer_set_invert_paddles( bool )
 * @brief   Enables or disables the "invert paddles" setting.
 * @note    This modifies the application configuration.
 */
void keyer_set_invert_paddles( bool invert );

/**
 * @fn      keyer_set_mode( keyer_mode_t )
 * @brief   Sets the currently active keyer mode.
 * @note    This modifies the application configuration.
 */
void keyer_set_mode( keyer_mode_t mode );

/**
 * @fn      keyer_set_output_active_low( bool )
 * @brief   Sets whether the keyer's output is active low or not.
 * @note    This modifies the application configuration.
 */
void keyer_set_output_active_low( bool active_lo );

/**
 * @fn      keyer_tick( tick_t )
 * @brief   Performs periodic processing at the specified tick count.
 * @note    It is not required to call this function at a uniform rate.
 */
void keyer_tick( tick_t tick );

#endif /* !defined( APPLICATION_KEYER_H ) */
