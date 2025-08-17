/**
 * @file    buzzer.h
 * @brief   Header for the buzzer driver module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-17
 */

#if !defined( EXAKEY_BUZZER_H )
#define EXAKEY_BUZZER_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      buzzer_init( void )
 * @brief   Initializes the buzzer driver module.
 */
void buzzer_init( void );

/**
 * @fn      buzzer_set_buzz( bool )
 * @brief   Turns the buzzer on or off.
 */
void buzzer_set_buzz( bool buzz );

/**
 * @fn      buzzer_set_frequency( uint16_t )
 * @brief   Sets the frequency of the buzzer.
 */
void buzzer_set_frequency( uint16_t freq );

#endif /* !defined( EXAKEY_BUZZER_H ) */
