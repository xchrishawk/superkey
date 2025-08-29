/**
 * @file    src/main/application/wpm.c
 * @brief   Implementation for the Morse code utility module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-22
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <math.h>

#include "application/config.h"
#include "application/wpm.h"
#include "core/sys.h"
#include "utility/debug.h"
#include "utility/types.h"
#include "utility/utility.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

/**
 * @def     WORD_UNIT_LENGTH
 * @brief   The length of a word, in code units.
 * @note    This is the length of the word "PARIS" in Morse code units.
 */
#define WORD_UNIT_LENGTH    50

/**
 * @def     MSEC_PER_MIN
 * @brief   The number of milliseconds per minute.
 */
#define MSEC_PER_MIN        ( ( uint32_t )MSEC_PER_SEC * ( uint32_t )SEC_PER_MIN )

/* ----------------------------------------------------- MACROS ----------------------------------------------------- */

// Validation macros
#define validate_element( _element )                                                    \
    assert_always( ( _element ) < WPM_ELEMENT_COUNT )

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

bool wpm_element_is_keyed( wpm_element_t el )
{
    // Do not validate element for this function (allow checking WPM_ELEMENT_UNKNOWN, etc.)

    return( el == WPM_ELEMENT_DOT || el == WPM_ELEMENT_DASH );

}   /* wpm_element_is_keyed() */


void wpm_element_scale_default( void )
{
    config_t config;
    config_get( & config );
    for( wpm_element_t el = 0; el < WPM_ELEMENT_COUNT; el++ )
        config.wpm_element_scale[ el ] = WPM_ELEMENT_SCALE_DEFAULT;
    config_set( & config );

}   /* wpm_element_scale_default() */


wpm_t wpm_get( void )
{
    return( config()->wpm );

}   /* wpm_get() */


wpm_element_scale_t wpm_get_element_scale( wpm_element_t el )
{
    validate_element( el );

    return( config()->wpm_element_scale[ el ] );

}   /* wpm_get_element_scale() */


void wpm_set( wpm_t wpm )
{
    config_t config;
    config_get( & config );
    config.wpm = clamp( wpm, WPM_MINIMUM, WPM_MAXIMUM );
    config_set( & config );

}   /* wpm_set() */


void wpm_set_element_scale( wpm_element_t el, wpm_element_scale_t scale )
{
    validate_element( el );

    config_t config;
    config_get( & config );
    config.wpm_element_scale[ el ] = clamp( scale, WPM_ELEMENT_SCALE_MINIMUM, WPM_ELEMENT_SCALE_MAXIMUM );
    config_set( & config );

}   /* wpm_set_element_scale() */

#include "debug_port.h"

void wpm_ticks( wpm_t wpm, wpm_ticks_t ticks )
{
    if( wpm < WPM_MINIMUM || wpm > WPM_MAXIMUM )
        return;

    // https://morsecode.world/international/timing/
    float unit_ms = ( float )MSEC_PER_MIN / ( wpm * ( float )WORD_UNIT_LENGTH );
    assert_always( unit_ms > 0.0f );

    ticks[ WPM_ELEMENT_DOT ]
        = ( tick_t )roundf( 1.0f * unit_ms * wpm_get_element_scale( WPM_ELEMENT_DOT ) ) * TICKS_PER_MSEC;
    ticks[ WPM_ELEMENT_DASH ]
        = ( tick_t )roundf( 3.0f * unit_ms * wpm_get_element_scale( WPM_ELEMENT_DASH ) ) * TICKS_PER_MSEC;
    ticks[ WPM_ELEMENT_ELEMENT_SPACE ]
        = ( tick_t )roundf( 1.0f * unit_ms * wpm_get_element_scale( WPM_ELEMENT_ELEMENT_SPACE ) ) * TICKS_PER_MSEC;
    ticks[ WPM_ELEMENT_LETTER_SPACE ]
        = ( tick_t )roundf( 3.0f * unit_ms * wpm_get_element_scale( WPM_ELEMENT_LETTER_SPACE ) ) * TICKS_PER_MSEC;
    ticks[ WPM_ELEMENT_WORD_SPACE ]
        = ( tick_t )roundf( 7.0f * unit_ms * wpm_get_element_scale( WPM_ELEMENT_WORD_SPACE ) ) * TICKS_PER_MSEC;

}   /* wpm_ticks() */
