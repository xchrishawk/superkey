/**
 * @file    keyer.h
 * @brief   Header for the keyer module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-19
 */

#if !defined( EXAKEY_KEYER_H )
#define EXAKEY_KEYER_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdint.h>

#include "types.h"

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

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
 * @fn      keyer_update( tick_t )
 * @brief   Updates keyer state at the specified tick count.
 */
void keyer_update( tick_t tick );

#endif /* !defined( EXAKEY_KEYER_H ) */
