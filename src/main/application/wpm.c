/**
 * @file        src/main/application/wpm.c
 * @brief       Implementation for the words-per-minute calculation module.
 *
 * @author      Chris Vig (chris@invictus.so)
 * @date        2025-08-22
 * @copyright   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
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

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

wpm_t wpm_get( void )
{
    return( config()->wpm );

}   /* wpm_get() */


void wpm_set( wpm_t wpm )
{
    config_t config;
    config_get( & config );
    config.wpm = clamp( wpm, WPM_MINIMUM, WPM_MAXIMUM );
    config_set( & config );

}   /* wpm_set() */


void wpm_ticks( wpm_t wpm,
                tick_t * dot,
                tick_t * dash,
                tick_t * element_space,
                tick_t * letter_space,
                tick_t * word_space )
{
    if( wpm < WPM_MINIMUM || wpm > WPM_MAXIMUM )
        return;

    // https://morsecode.world/international/timing/
    float unit_ms = ( float )MSEC_PER_MIN / ( ( ( float )wpm / ( float )WPM_T_SCALE ) * ( float )WORD_UNIT_LENGTH );
    assert_always( unit_ms > 0.0f );

    if( dot )
        * dot               = ( tick_t )roundf( 1.0f * unit_ms ) * TICKS_PER_MSEC;
    if( dash )
        * dash              = ( tick_t )roundf( 3.0f * unit_ms ) * TICKS_PER_MSEC;
    if( element_space )
        * element_space     = ( tick_t )roundf( 1.0f * unit_ms ) * TICKS_PER_MSEC;
    if( letter_space )
        * letter_space      = ( tick_t )roundf( 3.0f * unit_ms ) * TICKS_PER_MSEC;
    if( word_space )
        * word_space        = ( tick_t )roundf( 7.0f * unit_ms ) * TICKS_PER_MSEC;

}   /* wpm_ticks() */
