/**
 * @file        src/main/application/wpm.h
 * @brief       Header for the Morse code utility module.
 *
 * @author      Chris Vig (chris@invictus.so)
 * @date        2025-08-22
 * @copyright   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 */

#if !defined( APPLICATION_WPM_H )
#define APPLICATION_WPM_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stdint.h>

#include "utility/types.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

/**
 * @def     WPM_MAXIMUM
 * @brief   The maximum permissible WPM value.
 */
#define WPM_MAXIMUM                 100.0f

/**
 * @def     WPM_MINIMUM
 * @brief   The minimum permissible WPM value.
 */
#define WPM_MINIMUM                 1.0f

/**
 * @def     WPM_ELEMENT_SCALE_MAXIMUM
 * @brief   THe maximum scale value for element durations.
 */
#define WPM_ELEMENT_SCALE_MAXIMUM   10.0f

/**
 * @def     WPM_ELEMENT_SCALE_MINIMUM
 * @brief   The minimum scale value for element durations.
 */
#define WPM_ELEMENT_SCALE_MINIMUM   0.1f

/**
 * @def     WPM_ELEMENT_SCALE_DEFAULT
 * @brief   The default scale value for element durations.
 */
#define WPM_ELEMENT_SCALE_DEFAULT   _CONFIG_DFLT_WPM_ELEMENT_SCALE

/* ------------------------------------------------------ TYPES ----------------------------------------------------- */

/**
 * @typedef wpm_t
 * @brief   Type representing words per minute.
 */
typedef float wpm_t;

/**
 * @typedef wpm_element_t
 * @brief   Enumeration of supported Morse code elements.
 */
typedef uint8_t wpm_element_t;
enum
{
    WPM_ELEMENT_DOT,                        /**< Element is a dot.                      */
    WPM_ELEMENT_DASH,                       /**< Element is a dash.                     */
    WPM_ELEMENT_ELEMENT_SPACE,              /**< Element is an inter-element space.     */
    WPM_ELEMENT_LETTER_SPACE,               /**< Element is a space between letters.    */
    WPM_ELEMENT_WORD_SPACE,                 /**< Element is a space between words.      */

    WPM_ELEMENT_COUNT,                      /**< Number of valid element types.         */

    WPM_ELEMENT_NONE,                       /**< No valid element.                      */
    WPM_ELEMENT_UNKNOWN,                    /**< Unknown (manually keyed) element.      */
};

/**
 * @typedef wpm_element_scale_t
 * @brief   Scaling factor for an element duration.
 */
typedef float wpm_element_scale_t;

/**
 * @typedef wpm_ticks_t
 * @brief   Typedef for an array of `tick_t` values representing the duration of each known Morse code element.
 */
typedef tick_t wpm_ticks_t[ WPM_ELEMENT_COUNT ];

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      wpm_element_is_keyed( wpm_element_t )
 * @brief   Returns `true` if the specified element requires keying the radio.
 */
bool wpm_element_is_keyed( wpm_element_t el );

/**
 * @fn      wpm_element_scale_default( void )
 * @brief   Resets all element scales to the default value.
 * @note    This modifies the application configuration.
 */
void wpm_element_scale_default( void );

/**
 * @fn      wpm_get( void )
 * @brief   Returns the current global WPM setting.
 */
wpm_t wpm_get( void );

/**
 * @fn      wpm_get_element_scale( wpm_element_t )
 * @brief   Returns the element scale for the specified element type.
 */
wpm_element_scale_t wpm_get_element_scale( wpm_element_t el );

/**
 * @fn      wpm_set( wpm_t )
 * @brief   Sets the global WPM setting.
 * @note    This modifies the application configuration.
 */
void wpm_set( wpm_t wpm );

/**
 * @fn      wpm_set_element_scale( wpm_element_t, wpm_element_scale_t )
 * @brief   Sets the element scale for the specified element type.
 * @note    This modifies the application configuration.
 */
void wpm_set_element_scale( wpm_element_t el, wpm_element_scale_t scale );

/**
 * @fn      wpm_ticks( wpm_t, wpm_ticks_t )
 * @brief   Calculates the duration (in ticks) of various Morse code elements.
 * @note    This function does floating-point math and should not be called frequently.
 */
void wpm_ticks( wpm_t wpm, wpm_ticks_t ticks );

#endif /* !defined( APPLICATION_WPM_H ) */
