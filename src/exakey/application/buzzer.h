/**
 * @file    src/exakey/application/buzzer.h
 * @brief   Header for the buzzer driver module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-17
 */

#if !defined( EXAKEY_BUZZER_H )
#define EXAKEY_BUZZER_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stdint.h>

#include "utility/types.h"

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      buzzer_init( void )
 * @brief   Initializes the buzzer driver module.
 */
void buzzer_init( void );

/**
 * @fn      buzzer_set_on( bool )
 * @brief   Turns the buzzer on or off.
 */
void buzzer_set_on( bool on );

/**
 * @fn      buzzer_tick( tick_t )
 * @brief   Performs periodic processing at the specified tick count.
 */
void buzzer_tick( tick_t tick );

#endif /* !defined( EXAKEY_BUZZER_H ) */
