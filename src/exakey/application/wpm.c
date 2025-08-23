/**
 * @file    src/exakey/application/wpm.c
 * @brief   Implementation for the words-per-minute calculation module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-22
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include "application/config.h"
#include "application/wpm.h"
#include "core/sys.h"
#include "utility/types.h"
#include "utility/utility.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

/**
 * @def     WORD_UNIT_LENGTH
 * @brief   The length of a word, in code units.
 */
#define WORD_UNIT_LENGTH    50

/**
 * @def     MSEC_PER_HOUR
 * @brief   The number of milliseconds per hour.
 */
#define MSEC_PER_HOUR       ( ( uint32_t )MSEC_PER_SEC * ( uint32_t )SEC_PER_MIN * ( uint32_t )MIN_PER_HOUR )

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
    // TODO: Is this right???
    #define SCALE ( 1UL << 16 )
    uint32_t unit_ms = ( ( ( MSEC_PER_HOUR * SCALE ) / ( ( wpm * SCALE ) / WPM_T_SCALE ) ) / ( uint32_t )WORD_UNIT_LENGTH );
    #undef SCALE

    if( dot )
        * dot               = 1 * unit_ms * TICKS_PER_MSEC;
    if( dash )
        * dash              = 3 * unit_ms * TICKS_PER_MSEC;
    if( element_space )
        * element_space     = 1 * unit_ms * TICKS_PER_MSEC;
    if( letter_space )
        * letter_space      = 3 * unit_ms * TICKS_PER_MSEC;
    if( word_space )
        * word_space        = 7 * unit_ms * TICKS_PER_MSEC;

}   /* wpm_ticks() */
