/**
 * @file    src/main/application/wpm.h
 * @brief   Header for the words-per-minute calculation module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-22
 */

#if !defined( APPLICATION_WPM_H )
#define APPLICATION_WPM_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdint.h>

#include "utility/types.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

/**
 * @def     WPM_T_SCALE
 * @brief   The integer value of `wpm_t` which represents a value of exactly 1 word per minute.
 */
#define WPM_T_SCALE     10

/**
 * @def     WPM_MAXIMUM
 * @brief   The maximum permissible WPM value.
 */
#define WPM_MAXIMUM     1000

/**
 * @def     WPM_MINIMUM
 * @brief   The minimum permissible WPM value.
 */
#define WPM_MINIMUM     10

/* ------------------------------------------------------ TYPES ----------------------------------------------------- */

/**
 * @typedef wpm_t
 * @brief   Type representing words per minute (in one tenths).
 * @note    This is an integer value which represents tenths of a word per minute. For example, a value of 200 would
 *          represent 20 words per minute. 157 would represent 15.7 words per minute.
 */
typedef uint16_t wpm_t;

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      wpm_get( void )
 * @brief   Returns the current global WPM setting.
 */
wpm_t wpm_get( void );

/**
 * @fn      wpm_set( wpm_t )
 * @brief   Sets the global WPM setting.
 * @note    This modifies the application configuration.
 */
void wpm_set( wpm_t wpm );

/**
 * @fn      wpm_ticks( wpm, tick_t *, tick_t *, tick_t *, tick_t *, tick_t * )
 * @brief   Calculates the duration (in ticks) of various Morse code elements.
 * @note    This function does floating-point math and should not be called frequently.
 */
void wpm_ticks( wpm_t wpm,
                tick_t * dot,
                tick_t * dash,
                tick_t * element_space,
                tick_t * letter_space,
                tick_t * word_space );

#endif /* !defined( APPLICATION_WPM_H ) */
