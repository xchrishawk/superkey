/**
 * @file    src/main/application/keyer.h
 * @brief   Header for the keyer module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-19
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 *
 * @details
 * This module is responsible for performing the primary keyer function of the application. It implements a simple state
 * machine which responds to user inputs and produces the expected keyer output. Several different keyer modes and
 * options are supported.
 */

#if !defined( APPLICATION_KEYER_H )
#define APPLICATION_KEYER_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdint.h>
#include <stdlib.h>

#include "utility/types.h"
#include "utility/utility.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

/**
 * @def     KEYER_AUTOKEY_FLAG_NONE
 * @brief   Value for `keyer_autokey_flag_field_t` indicating that no flags are selected.
 */
#define KEYER_AUTOKEY_FLAG_NONE     0

/**
 * @def     KEYER_HUMANIZER_LEVEL_MIN
 * @brief   Minimum humanizer level (unitless fraction).
 */
#define KEYER_HUMANIZER_LEVEL_MIN   0.0f

/**
 * @def     KEYER_HUMANIZER_LEVEL_MAX
 * @brief   Maximum humanizer level (unitless fraction).
 */
#define KEYER_HUMANIZER_LEVEL_MAX   1.0f

/**
 * @def     KEYER_HUMANIZER_OFF
 * @brief   Humanizer level value which disables the humanizer (unitless fraction).
 */
#define KEYER_HUMANIZER_OFF         KEYER_HUMANIZER_LEVEL_MIN

/* ----------------------------------------------------- TYPES ------------------------------------------------------ */

/**
 * @typedef keyer_autokey_flag_t
 * @brief   Enumeration of options for the autokey function.
 */
typedef uint8_t keyer_autokey_flag_t;
enum
{
    KEYER_AUTOKEY_FLAG_NO_LETTER_SPACE,     /**< No letter space after letters.         */

    KEYER_AUTOKEY_FLAG_COUNT,               /**< Number of valid flags.                 */
};

/**
 * @typedef keyer_autokey_flag_field_t
 * @brief   Bitfield of autokey options indexed by `keyer_autokey_flag_t`.
 */
typedef uint8_t keyer_autokey_flag_field_t;

_Static_assert( sizeof_bits( keyer_autokey_flag_field_t ) >= KEYER_AUTOKEY_FLAG_COUNT, "Type not large enough!" );

/**
 * @typedef keyer_paddle_mode_t
 * @brief   Enumeration of the paddle modes that the keyer supports.
 */
typedef uint8_t keyer_paddle_mode_t;
enum
{
    /**
     * In the `KEYER_PADDLE_MODE_IAMBIC` mode, pressing both paddles simultaneously will emit a stream of dashes and
     * dots, starting with whichever one was pressed first.
     */
    KEYER_PADDLE_MODE_IAMBIC,

    /**
     * In the `KEYER_PADDLE_MODE_ULTIMATIC` mode, pressing both paddles simultaneously will emit a stream of either dots
     * or dashes, depending on which paddle was pressed *first*. For example, if the left paddle is held down, and the
     * right paddle is then held down (while the left is still held down), the keyer will emit dots.
     */
    KEYER_PADDLE_MODE_ULTIMATIC,

    /**
     * In the `KEYER_PADDLE_MODE_ULTIMATIC_ALTERNATE` mode, pressing both paddles simultaneously will emit a stream of
     * either dots or dashes, depending on which paddle was pressed *most recently*. For example, if the left paddle is
     * held down, and the right paddle is then held down (while the left is still held down), the keyer will emit
     * dashes.
     */
    KEYER_PADDLE_MODE_ULTIMATIC_ALTERNATE,

    /** Number of valid keyer paddle modes. */
    KEYER_PADDLE_MODE_COUNT,
};

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      keyer_autokey_char( char )
 * @brief   Adds the specified character to the keyer's autokey buffer.
 * @returns `true` if the character was successfully queued.
 * @note    This is equivalent to calling `keyer_autokey_char_ex()` with the `flags` set to `KEYER_AUTOKEY_FLAG_NONE`.
 */
bool keyer_autokey_char( char c );

/**
 * @fn      keyer_autokey_char_ex( char, keyer_autokey_flag_field_t )
 * @brief   Adds the specified character to the keyer's autokey buffer with the specified flags.
 * @returns `true` if the character was successfully queued.
 */
bool keyer_autokey_char_ex( char c, keyer_autokey_flag_field_t flags );

/**
 * @fn      keyer_autokey_count( void )
 * @brief   Returns the number of Morse code elements currently in the autokey buffer.
 */
size_t keyer_autokey_count( void );

/**
 * @fn      keyer_autokey_str( char const * )
 * @brief   Adds the specified string to the keyer's autokey buffer.
 * @returns The number of characters that were successfully queued.
 * @note    This is equivalent to calling `keyer_autokey_str_ex()` with the `flags` set to `KEYER_AUTOKEY_FLAG_NONE`.
 */
size_t keyer_autokey_str( char const * str );

/**
 * @fn      keyer_autokey_str_ex( char const *, keyer_autokey_flag_field_t )
 * @brief   Adds the specified string to the keyer's autokey buffer with the specified flags.
 * @returns The number of characters that were successfully queued.
 */
size_t keyer_autokey_str_ex( char const * str, keyer_autokey_flag_field_t flags );

/**
 * @fn      keyer_get_humanizer_level( void )
 * @brief   Returns the current humanizer level.
 */
float keyer_get_humanizer_level( void );

/**
 * @fn      keyer_get_on( void )
 * @brief   Returns `true` if the keyer is currently commanding the radio to transmit.
 */
bool keyer_get_on( void );

/**
 * @fn      keyer_get_paddle_invert( void )
 * @brief   Returns `true` if the keyer is configured to invert the paddles. In this case, the right paddle will emit
 *          dots and the left paddle will emit dashes.
 */
bool keyer_get_paddle_invert( void );

/**
 * @fn      keyer_get_paddle_mode( void )
 * @brief   Returns the keyer's currently active mode.
 */
keyer_paddle_mode_t keyer_get_paddle_mode( void );

/**
 * @fn      keyer_get_trainer_mode_enabled( void )
 * @brief   Returns `true` if trainer mode is enabled.
 * @note    In trainer mode, the keyer output to any connected radios will not actually be activated.
 */
bool keyer_get_trainer_mode_enabled( void );

/**
 * @fn      keyer_init( void )
 * @brief   Initializes the keyer module.
 * @note    Requires that the following modules have been initialized:
 *          - `buzzer`
 *          - `config`
 *          - `io`
 *          - `led`
 *          - `sys`
 */
void keyer_init( void );

/**
 * @fn      keyer_panic( void )
 * @brief   Immediately and unconditionally stops the keyer, and clears any pending autokey characters.
 */
void keyer_panic( void );

/**
 * @fn      keyer_set_humanizer_level( float )
 * @brief   Sets the current humanizer level.
 * @note    The argument will be clamped to the range [`KEYER_HUMANIZER_MIN`, `KEYER_HUMANIZER_MAX`].
 */
void keyer_set_humanizer_level( float humanizer );

/**
 * @fn      keyer_set_paddle_invert( bool )
 * @brief   Enables or disables the "invert paddles" setting. If set to `true`, the right paddle will emit dots and the
 *          left paddle will emit dashes.
 * @note    This modifies the application configuration.
 */
void keyer_set_paddle_invert( bool invert );

/**
 * @fn      keyer_set_paddle_mode( keyer_paddle_mode_t )
 * @brief   Sets the keyer's currently active paddle mode.
 * @note    This modifies the application configuration.
 */
void keyer_set_paddle_mode( keyer_paddle_mode_t mode );

/**
 * @fn      keyer_set_trainer_mode_enabled( bool )
 * @brief   Enables or disables trainer mode.
 * @note    In trainer mode, the keyer output to any connected radios will not actually be activated.
 */
void keyer_set_trainer_mode_enabled( bool enabled );

/**
 * @fn      keyer_tick( tick_t )
 * @brief   Performs periodic processing at the specified tick count.
 * @note    It is not required to call this function at a uniform rate.
 */
void keyer_tick( tick_t tick );

#endif /* !defined( APPLICATION_KEYER_H ) */
