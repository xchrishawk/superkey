/**
 * @file    src/main/application/keyer.c
 * @brief   Implementation for the keyer module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-19
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "application/buzzer.h"
#include "application/config.h"
#include "application/io.h"
#include "application/keyer.h"
#include "application/led.h"
#include "application/wpm.h"
#include "core/sys.h"
#include "utility/debug.h"
#include "utility/types.h"
#include "utility/utility.h"

#include "application/debug_port.h"

/* ----------------------------------------------------- TYPES ------------------------------------------------------ */

/**
 * @typedef state_t
 * @brief   Enumeration of states that the keyer module may be in.
 */
typedef uint8_t state_t;
enum
{
    STATE_OFF,                              /**< Keyer is off.                          */
    STATE_ON,                               /**< Keyer is on continuously.              */
    STATE_DOTS,                             /**< Keyer emits continuous dots.           */
    STATE_DASHES,                           /**< Keyer emits continuous dashes.         */
    STATE_INTERLEAVED,                      /**< Keyer emits continuous dots and dashes.*/
    STATE_AUTOKEY,                          /**< Keyer automatically keys characters.   */

    STATE_COUNT,                            /**< Number of valid states.                */
};

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

/**
 * @def     NO_TIMEOUT
 * @brief   May be assigned to `s_element_duration` or `s_element_lockout` to indicate that these values don't apply.
 */
#define NO_TIMEOUT 0

/**
 * @def     AUTOKEY_BUF_SZ
 * @brief   Maximum number of elements queued in autokey buffer.
 * @note    This 4-kilobyte buffer takes up 1/4th of our RAM space. This seems justified since autokey is such a core
 *          part of the device's functionality. This could be reduced if required.
 */
#define AUTOKEY_BUF_SZ  4096

/* --------------------------------------------------- VARIABLES ---------------------------------------------------- */

static bool s_keyed = false;                /**< Is the keyer hardware currently keyed? */
static bool s_panicked = false;             /**< Was the keyer panic activated?         */
static bool s_trainer_mode = false;         /**< Is trainer mode on?                    */

static state_t s_state = STATE_OFF;         /**< Currently active keyer state.          */

static wpm_element_t s_el = WPM_ELEMENT_NONE;/** The element currently being keyed.     */
static wpm_element_t s_lockout_el = WPM_ELEMENT_NONE;/**< Element which started lockout.*/
static tick_t s_el_stop_tick = 0;           /** Tick at which current element must stop.*/
static bool s_el_stop_tick_vld = false;     /** Is `s_el_stop_tick` valid?              */
static tick_t s_el_start_tick = 0;          /** Tick at which next element may start.   */
static bool s_el_start_tick_vld = false;    /** Is `s_el_start_tick` valid?             */

static wpm_element_t s_autokey_buf[ AUTOKEY_BUF_SZ ];/**< El buffer for autokey mode.   */
static size_t s_autokey_head = 0;           /**< Head of autokey circular buffer.       */
static size_t s_autokey_tail = 0;           /**< Tail of autokey circular buffer.       */

static wpm_ticks_t s_ticks;                 /**< Number of ticks for each element type. */
static tick_t s_ticks_tick = 0;             /**< Tick at which s_ticks was updated.     */

/* ----------------------------------------------------- MACROS ----------------------------------------------------- */

/**
 * @def     is_tick_passed( _tick, _timeout )
 * @brief   Returns `true` if `_tick` represents a tick greater than or equal to `_timeout`.
 */
#define is_tick_passed( _tick, _timeout )                                           \
    ( sys_is_tick_gte( _tick, _timeout ) )

/**
 * @def     is_stop_tick_passed( _tick )
 * @brief   Returns `true` if `_tick` has passed `s_el_stop_tick`, or there is no valid stop tick.
 */
#define is_stop_tick_passed( _tick )                                                \
    ( ! s_el_stop_tick_vld || is_tick_passed( _tick, s_el_stop_tick ) )

/**
 * @def     is_start_tick_passed( _tick )
 * @brief   Returns `true` if `_tick` has passed `s_el_start_tick`, or there is no valid start tick.
 */
#define is_start_tick_passed( _tick )                                               \
    ( ! s_el_start_tick_vld || is_tick_passed( _tick, s_el_start_tick ) )

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      autokey_avail( void )
 * @brief   Returns the number of available slots in the autokey circular buffer.
 */
static size_t autokey_avail( void );

/**
 * @fn      autokey_count( void )
 * @brief   Returns the number of items in the autokey circular buffer.
 */
static size_t autokey_count( void );

/**
 * @fn      autokey_dequeue( wpm_element_t * )
 * @brief   Dequeues an element from the autokey buffer.
 */
static bool autokey_dequeue( wpm_element_t * el );

/**
 * @fn      autokey_enqueue( void )
 * @brief   Enqueues the specified element in the autokey buffer.
 */
static bool autokey_enqueue( wpm_element_t el );

/**
 * @fn      do_state_autokey( tick_t, bool )
 * @brief   Executes the `STATE_AUTOKEY` state.
 */
static void do_state_autokey( tick_t tick, bool new_state );

/**
 * @fn      do_state_dashes( tick_t, bool )
 * @brief   Executes the `STATE_DASHES` state.
 */
static void do_state_dashes( tick_t tick, bool new_state );

/**
 * @fn      do_state_dots( tick_t, bool )
 * @brief   Executes the `STATE_DOTS` state.
 */
static void do_state_dots( tick_t tick, bool new_state );

/**
 * @fn      do_state_interleaved( tick_t, bool )
 * @brief   Executes the `STATE_INTERLEAVED` state.
 */
static void do_state_interleaved( tick_t tick, bool new_state );

/**
 * @fn      do_state_off( tick_t, bool )
 * @brief   Executes the `STATE_OFF` state.
 */
static void do_state_off( tick_t tick, bool new_state );

/**
 * @fn      do_state_on( tick_t, bool )
 * @brief   Executes the `STATE_ON` state.
 */
static void do_state_on( tick_t tick, bool new_state );

/**
 * @fn      get_keyed( void )
 * @brief   Returns `true` if the keyer hardware is keying.
 */
static bool get_keyed( void );

/**
 * @fn      get_next_state( void )
 * @brief   Returns the next keyer state, based on current inputs.
 */
static state_t get_next_state( void );

/**
 * @fn      set_keyed( bool )
 * @brief   Sets whether the keyer hardware is keying or not.
 */
static void set_keyed( bool keyed );

/**
 * @fn      update_hardware( void )
 * @brief   Synchronizes hardware state with the current keying state.
 */
static void update_hardware( void );

/**
 * @fn      update_ticks( void )
 * @brief   Update cached element tick counts, if required.
 * @note    This is an expensive calculation, so we only do it if the WPM changes.
 */
static void update_ticks( tick_t tick );

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

bool keyer_autokey_char( char c )
{
    switch( c )
    {
    case ' ':
        // Space
        return( autokey_enqueue( WPM_ELEMENT_WORD_SPACE ) );

    case 'a':
    case 'A':
        // Letter A
        return( autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case 'b':
    case 'B':
        // Letter B
        return( autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case 'c':
    case 'C':
        // Letter C
        return( autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case 'd':
    case 'D':
        // Letter D
        return( autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case 'e':
    case 'E':
        // Letter E
        return( autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case 'f':
    case 'F':
        // Letter F
        return( autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case 'g':
    case 'G':
        // Letter G
        return( autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case 'h':
    case 'H':
        // Letter H
        return( autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case 'i':
    case 'I':
        // Letter I
        return( autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case 'j':
    case 'J':
        // Letter J
        return( autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case 'k':
    case 'K':
        // Letter K
        return( autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case 'l':
    case 'L':
        // Letter L
        return( autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case 'm':
    case 'M':
        // Letter M
        return( autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case 'n':
    case 'N':
        // Letter N
        return( autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case 'o':
    case 'O':
        // Letter O
        return( autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case 'p':
    case 'P':
        // Letter P
        return( autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case 'q':
    case 'Q':
        // Letter Q
        return( autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case 'r':
    case 'R':
        // Letter R
        return( autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case 's':
    case 'S':
        // Letter S
        return( autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case 't':
    case 'T':
        // Letter T
        return( autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case 'u':
    case 'U':
        // Letter U
        return( autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case 'v':
    case 'V':
        // Letter V
        return( autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case 'w':
    case 'W':
        // Letter W
        return( autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case 'x':
    case 'X':
        // Letter X
        return( autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case 'y':
    case 'Y':
        // Letter Y
        return( autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case 'z':
    case 'Z':
        // Letter Z
        return( autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case '0':
        // Number 0
        return( autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case '1':
        // Number 1
        return( autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case '2':
        // Number 2
        return( autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case '3':
        // Number 3
        return( autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case '4':
        // Number 4
        return( autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case '5':
        // Number 5
        return( autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case '6':
        // Number 6
        return( autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case '7':
        // Number 7
        return( autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case '8':
        // Number 8
        return( autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case '9':
        // Number 9
        return( autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case '.':
        // Period
        return( autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case ',':
        // Comma
        return( autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case '?':
        // Question mark
        return( autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case '\'':
        // Single quote
        return( autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case '!':
        // Exclamation mark
        return( autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) );

    case '-':
        // Dash
        return( autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) );

    case '/':
        // Slash
        return( autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case '=':
        // Equals sign
        return( autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case '+':
        // Plus sign
        return( autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case '"':
        // Double quote
        return( autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    case '_':
        // Underscore
        return( autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_DOT ) &&
                autokey_enqueue( WPM_ELEMENT_DASH ) &&
                autokey_enqueue( WPM_ELEMENT_LETTER_SPACE ) );

    default:
        return( false );
    }

}   /* keyer_autokey_char() */


size_t keyer_autokey_str( char const * str )
{
    size_t count = 0;
    while( * str )
        if( keyer_autokey_char( *( str++ ) ) )
            count++;

    return( count );

}   /* keyer_autokey_str() */


bool keyer_get_on( void )
{
    return( get_keyed() );

}   /* keyer_get_on() */


bool keyer_get_paddle_invert( void )
{
    return( config()->keyer_paddle_invert );

}   /* keyer_get_paddle_invert() */


keyer_paddle_mode_t keyer_get_paddle_mode( void )
{
    return( config()->keyer_paddle_mode );

}   /* keyer_get_paddle_mode() */


bool keyer_get_trainer_mode_enabled( void )
{
    return( s_trainer_mode );

}   /* keyer_get_trainer_mode_enabled() */


void keyer_init( void )
{
    // Initialize local state
    s_keyed = false;
    s_panicked = false;
    s_trainer_mode = false;
    s_state = STATE_OFF;
    s_el = WPM_ELEMENT_NONE;
    s_lockout_el = WPM_ELEMENT_NONE;
    s_el_stop_tick = 0;
    s_el_stop_tick_vld = false;
    s_el_start_tick = 0;
    s_el_start_tick_vld = false;
    for( wpm_element_t el = 0; el < WPM_ELEMENT_COUNT; el++ )
        s_ticks[ el ] = 0;
    s_ticks_tick = 0;

    // Default to key off
    set_keyed( false );

    // Update ticks
    update_ticks( sys_get_tick() );

}   /* keyer_init() */


void keyer_panic( void )
{
    s_panicked = true;
    s_autokey_tail = s_autokey_head;
    set_keyed( false );

}   /* keyer_panic() */


void keyer_set_paddle_invert( bool invert )
{
    config_t config;
    config_get( & config );
    config.keyer_paddle_invert = invert;
    config_set( & config );

}   /* keyer_set_paddle_invert() */


void keyer_set_paddle_mode( keyer_paddle_mode_t mode )
{
    config_t config;
    config_get( & config );
    config.keyer_paddle_mode = mode;
    config_set( & config );

}   /* keyer_set_paddle_mode() */


void keyer_set_trainer_mode_enabled( bool enabled )
{
    s_trainer_mode = enabled;

    update_hardware();

}   /* keyer_set_trainer_mode_enabled() */


void keyer_tick( tick_t tick )
{
    // Update element tick counts if required
    update_ticks( tick );

    // Check the next state, and determine if it's different than the current state
    state_t next_state = get_next_state();
    bool new_state = ( next_state != s_state );
    s_state = next_state;

    // Reset the panic flag if the state changed
    // (Note that in the logic below, this means that new_state==TRUE implies s_panicked==FALSE)
    if( new_state && s_panicked )
        s_panicked = false;

    // Execute whatever state we evaluate to
    switch( s_state )
    {
        case STATE_OFF:
            do_state_off( tick, new_state );
            break;
        case STATE_ON:
            do_state_on( tick, new_state );
            break;
        case STATE_DOTS:
            do_state_dots( tick, new_state );
            break;
        case STATE_DASHES:
            do_state_dashes( tick, new_state );
            break;
        case STATE_INTERLEAVED:
            do_state_interleaved( tick, new_state );
            break;
        case STATE_AUTOKEY:
            do_state_autokey( tick, new_state );
            break;
    }

}   /* keyer_tick() */


static size_t autokey_avail( void )
{
    return( AUTOKEY_BUF_SZ - autokey_count() - 1 );

}   /* autokey_avail() */


static size_t autokey_count( void )
{
    if( s_autokey_head >= s_autokey_tail )
        return( s_autokey_head - s_autokey_tail );
    else
        return( AUTOKEY_BUF_SZ - s_autokey_tail + s_autokey_head );

}   /* autokey_count() */


static bool autokey_dequeue( wpm_element_t * el )
{
    if( autokey_count() == 0 )
        return( false );

    * el = s_autokey_buf[ s_autokey_tail ];
    increment_rollover( s_autokey_tail, AUTOKEY_BUF_SZ );
    return( true );

}   /* autokey_dequeue() */


static bool autokey_enqueue( wpm_element_t el )
{
    if( autokey_avail() == 0 )
        return( false );

    s_autokey_buf[ s_autokey_head ] = el;
    increment_rollover( s_autokey_head, AUTOKEY_BUF_SZ );
    return( true );

}   /* autokey_enqueue() */


static void do_state_autokey( tick_t tick, bool new_state )
{
    ( void )new_state;

    wpm_element_t el;
    if( ! s_panicked && is_start_tick_passed( tick ) && autokey_dequeue( & el ) )
    {
        // Get info for current state
        bool prev_el_was_letter_space = ( s_el == WPM_ELEMENT_LETTER_SPACE );
        bool prev_lockout_el_was_keyed = wpm_element_is_keyed( s_lockout_el );

        // Activate keyer hardware (if required)
        s_el = el;
        if( wpm_element_is_keyed( s_el ) )
        {
            s_lockout_el = s_el;
            s_el_stop_tick = tick + s_ticks[ s_el ];
            s_el_stop_tick_vld = true;
            s_el_start_tick = tick + s_ticks[ s_el ] + s_ticks[ WPM_ELEMENT_ELEMENT_SPACE ];
            s_el_start_tick_vld = true;
            set_keyed( true );
        }
        else
        {
            s_el_stop_tick = 0;
            s_el_stop_tick_vld = false;
            s_el_start_tick = tick + s_ticks[ s_el ]
                            - ( prev_lockout_el_was_keyed ?
                                s_ticks[ WPM_ELEMENT_ELEMENT_SPACE ] : 0 )
                            - ( prev_el_was_letter_space ?
                                ( s_ticks[ WPM_ELEMENT_LETTER_SPACE ] - s_ticks[ WPM_ELEMENT_ELEMENT_SPACE ] ) : 0 );
            s_el_start_tick_vld = true;
        }
    }
    else if( is_stop_tick_passed( tick ) && get_keyed() )
    {
        // Deactivate keyer hardware
        set_keyed( false );
    }

}   /* do_state_autokey() */


static void do_state_dashes( tick_t tick, bool new_state )
{
    ( void )new_state;

    if( ! s_panicked && is_start_tick_passed( tick ) && ! get_keyed() )
    {
        // Activate keyer hardware
        s_el = WPM_ELEMENT_DASH;
        s_lockout_el = s_el;
        s_el_stop_tick = tick + s_ticks[ WPM_ELEMENT_DASH ];
        s_el_stop_tick_vld = true;
        s_el_start_tick = tick + s_ticks[ WPM_ELEMENT_DASH ] + s_ticks[ WPM_ELEMENT_ELEMENT_SPACE ];
        s_el_start_tick_vld = true;
        set_keyed( true );
    }
    else if( is_stop_tick_passed( tick ) && get_keyed() )
    {
        // Deactivate keyer hardware
        set_keyed( false );
    }

}   /* do_state_dashes() */


static void do_state_dots( tick_t tick, bool new_state )
{
    ( void )new_state;

    if( ! s_panicked && is_start_tick_passed( tick ) && ! get_keyed() )
    {
        // Activate keyer hardware
        s_el = WPM_ELEMENT_DOT;
        s_lockout_el = s_el;
        s_el_stop_tick = tick + s_ticks[ WPM_ELEMENT_DOT ];
        s_el_stop_tick_vld = true;
        s_el_start_tick = tick + s_ticks[ WPM_ELEMENT_DOT ] + s_ticks[ WPM_ELEMENT_ELEMENT_SPACE ];
        s_el_start_tick_vld = true;
        set_keyed( true );
    }
    else if( is_stop_tick_passed( tick ) && get_keyed() )
    {
        // Deactivate keyer hardware
        set_keyed( false );
    }

}   /* do_state_dots() */


static void do_state_interleaved( tick_t tick, bool new_state )
{
    ( void )new_state;

    if( ! s_panicked && is_start_tick_passed( tick ) && ! get_keyed() )
    {
        // Activate keyer hardware
        s_el = ( s_lockout_el == WPM_ELEMENT_DOT ? WPM_ELEMENT_DASH : WPM_ELEMENT_DOT );
        s_lockout_el = s_el;
        s_el_stop_tick = tick + s_ticks[ s_el ];
        s_el_stop_tick_vld = true;
        s_el_start_tick = tick + s_ticks[ s_el ] + s_ticks[ WPM_ELEMENT_ELEMENT_SPACE ];
        s_el_start_tick_vld = true;
        set_keyed( true );
    }
    else if( is_stop_tick_passed( tick ) && get_keyed() )
    {
        // Deactivate keyer hardware
        set_keyed( false );
    }

}   /* do_state_interleaved() */


static void do_state_off( tick_t tick, bool new_state )
{
    ( void )new_state;

    // Deactivate keyer hardware, once allowed
    if( get_keyed() && is_stop_tick_passed( tick ) )
    {
        set_keyed( false );
    }

    // Reset state, once allowed
    if( s_el != WPM_ELEMENT_NONE && is_start_tick_passed( tick ) )
    {
        s_el = WPM_ELEMENT_NONE;
        s_lockout_el = s_el;
        s_el_stop_tick = 0;
        s_el_stop_tick_vld = false;
        s_el_start_tick = 0;
        s_el_start_tick_vld = false;
    }

}   /* do_state_off() */


static void do_state_on( tick_t tick, bool new_state )
{
    ( void )tick;

    // Activate unconditionally, unless panicked
    if( ! s_panicked && ( new_state || ! get_keyed() ) )
    {
        s_el = WPM_ELEMENT_UNKNOWN;
        s_lockout_el = s_el;
        s_el_stop_tick = 0;
        s_el_stop_tick_vld = false;
        s_el_start_tick = 0;
        s_el_start_tick_vld = false;
        set_keyed( true );
    }

}   /* do_state_on() */


static bool get_keyed( void )
{
    return( s_keyed );

}   /* get_keyed() */


static state_t get_next_state( void )
{
    // Static variables to store values from previous invocations
    static bool straight_key = false;
    static bool paddle_left  = false;
    static bool paddle_right = false;

    // Store previous values for paddles
    bool prev_paddle_left  = paddle_left;
    bool prev_paddle_right = paddle_right;

    // Get updated inputs
    straight_key = ( io_get_state_type( IO_TYPE_INPUT_STRAIGHT_KEY ) == IO_STATE_ON );
    paddle_left  = ( io_get_state_type( IO_TYPE_INPUT_PADDLE_LEFT ) == IO_STATE_ON );
    paddle_right = ( io_get_state_type( IO_TYPE_INPUT_PADDLE_RIGHT ) == IO_STATE_ON );

    // Get other required settings
    bool paddle_invert = keyer_get_paddle_invert();

    // Determine next state
    if( autokey_count() != 0 )
    {
        // Autokey has highest priority
        return( STATE_AUTOKEY );
    }
    else if( straight_key )
    {
        // Straight key supersedes all other inputs
        return( STATE_ON );
    }
    else if( paddle_left && paddle_right )
    {
        switch( keyer_get_paddle_mode() )
        {
        case KEYER_PADDLE_MODE_IAMBIC:
            // Always do interleaved in iambic mode
            return( STATE_INTERLEAVED );

        case KEYER_PADDLE_MODE_ULTIMATIC:
            // The first activated paddle wins
            return( s_state );

        case KEYER_PADDLE_MODE_ULTIMATIC_ALTERNATE:
            // The most recently activated paddle wins
            if( paddle_left && ! prev_paddle_left )
            {
                // Left paddle was more recently activated
                return( paddle_invert ? STATE_DASHES : STATE_DOTS );
            }
            else if( paddle_right && ! prev_paddle_right )
            {
                // Right paddle was more recently activated
                return( paddle_invert ? STATE_DOTS : STATE_DASHES );
            }
            else
            {
                // No change in state
                return( s_state );
            }

        default:
            // This should not be possible
            fail();
        }
    }
    else if( ( ! paddle_invert && paddle_left ) ||
             (   paddle_invert && paddle_right ) )
    {
        // The left paddle traditionally emits dots
        return( STATE_DOTS );
    }
    else if( ( ! paddle_invert && paddle_right ) ||
             (   paddle_invert && paddle_left ) )
    {
        // The right paddle traditionally emits dashes
        return( STATE_DASHES );
    }
    else
    {
        // This should not be hit unless there are no inputs active
        assert_debug( inputs == 0 );
        return( STATE_OFF );
    }

}   /* get_next_state() */


static void set_keyed( bool keyed )
{
    s_keyed = keyed;
    update_hardware();

}   /* set_keyed() */


static void update_hardware( void )
{
    io_set_output_state_type( IO_TYPE_OUTPUT_KEYER, s_keyed && ! s_trainer_mode ? IO_STATE_ON : IO_STATE_OFF );
    led_set_on( LED_KEY, s_keyed );
    buzzer_set_on( s_keyed );

}   /* update_hardware() */


static void update_ticks( tick_t tick )
{
    // Limit this update to ~20 Hz, since it does floating point math
    #define MIN_DELAY_MS 50
    if( s_ticks_tick != 0 && sys_elapsed( tick, s_ticks_tick ) < ( MIN_DELAY_MS * TICKS_PER_MSEC ) )
        return;
    #undef MIN_DELAY_MS

    wpm_ticks( wpm_get(), s_ticks );
    s_ticks_tick = tick;

}   /* update_ticks() */
