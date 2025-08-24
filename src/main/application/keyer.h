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

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      keyer_get_invert_paddles( void )
 * @brief   Returns `true` if the keyer is configured to invert the paddles. In this case, the right paddle will emit
 *          dots and the left paddle will emit dashes.
 */
bool keyer_get_invert_paddles( void );

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
 * @fn      keyer_tick( tick_t )
 * @brief   Performs periodic processing at the specified tick count.
 * @note    It is not required to call this function at a uniform rate.
 */
void keyer_tick( tick_t tick );

#endif /* !defined( APPLICATION_KEYER_H ) */
