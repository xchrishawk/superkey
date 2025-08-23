/**
 * @file    src/exakey/application/buzzer.h
 * @brief   Header for the buzzer driver module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-17
 */

#if !defined( APPLICATION_BUZZER_H )
#define APPLICATION_BUZZER_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stdint.h>

#include "utility/types.h"

/* ----------------------------------------------------- TYPES ------------------------------------------------------ */

/**
 * @typedef buzzer_freq_t
 * @brief   Typedef representing a buzzer frequency.
 */
typedef uint16_t buzzer_freq_t;

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

/**
 * @def     BUZZER_MINIMUM_FREQUENCY
 * @brief   The minimum supported frequency for the buzzer, in Hz.
 * @note    This is empirical, based on audio quality with the current buzzer hardware.
 */
#define BUZZER_MINIMUM_FREQUENCY    400

/**
 * @def     BUZZER_MAXIMUM_FREQUENCY
 * @brief   The maximum supported frequency for the buzzer, in Hz.
 * @note    This is empirical, based on audio quality with the current buzzer hardware.
 */
#define BUZZER_MAXIMUM_FREQUENCY    2000

_Static_assert( BUZZER_MINIMUM_FREQUENCY < BUZZER_MAXIMUM_FREQUENCY, "Invalid frequency range!" );

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      buzzer_get_enabled( void )
 * @brief   Returns `true` if the buzzer is currently enabled.
 */
bool buzzer_get_enabled( void );

/**
 * @fn      buzzer_get_frequency( void )
 * @brief   Returns the current buzzer audio frequency, in Hz.
 */
buzzer_freq_t buzzer_get_frequency( void );

/**
 * @fn      buzzer_get_on( void )
 * @brief   Returns `true` if the buzzer is currently commanded on.
 * @note    The buzzer will only sound if both `buzzer_get_enabled()` and `buzzer_get_on()` return `true`.
 */
bool buzzer_get_on( void );

/**
 * @fn      buzzer_init( void )
 * @brief   Initializes the buzzer driver module.
 */
void buzzer_init( void );

/**
 * @fn      buzzer_set_enabled( bool )
 * @brief   Enables or disables the buzzer.
 * @note    This modifies the application configuration.
 */
void buzzer_set_enabled( bool enabled );

/**
 * @fn      buzzer_set_frequency( buzzer_freq_t )
 * @brief   Sets the buzzer's audio frequency, in Hz.
 * @note    This modifies the application configuration.
 */
void buzzer_set_frequency( buzzer_freq_t freq );

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

#endif /* !defined( APPLICATION_BUZZER_H ) */
