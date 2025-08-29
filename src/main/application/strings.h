/**
 * @file    src/main/application/strings.h
 * @brief   Header for the strings module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-28
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 */

#if !defined( APPLICATION_STRINGS_H )
#define APPLICATION_STRINGS_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>

#include "application/io.h"
#include "application/keyer.h"
#include "application/led.h"
#include "application/wpm.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

/**
 * @def     DISABLED_STR
 * @brief   The token which indicates that an option is disabled.
 */
#define DISABLED_STR            "disabled"

/**
 * @def     ENABLED_STR
 * @brief   The token which indicates that an option is enabled.
 */
#define ENABLED_STR             "enabled"

/**
 * @def     OFF_STR
 * @brief   The token which indicates that an io or output is off.
 */
#define OFF_STR                 "off"

/**
 * @def     ON_STR
 * @brief   The token which indicates that an io or output is on.
 */
#define ON_STR                  "on"

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      string_begins_with( char const *, char const * )
 * @brief   Returns true if `str` starts with `token`.
 * @note    This function is case-insensitive.
 */
bool string_begins_with( char const * str, char const * token );

/**
 * @fn      string_equals( char const *, char const * )
 * @brief   Returns `true` if the specified strings are equal.
 * @note    This function is case-insensitive.
 */
bool string_equals( char const * a, char const * b );

/**
 * @fn      string_from_bool( bool )
 * @brief   Returns a string for the specified `bool`.
 */
char const * string_from_bool( bool b );

/**
 * @fn      string_from_element( wpm_element_t )
 * @brief   Returns a string for the specified `wpm_element_t`.
 */
char const * string_from_element( wpm_element_t el );

/**
 * @fn      string_from_io_pin( io_pin_t )
 * @brief   Returns a string for the specified `io_pin_t`.
 */
char const * string_from_io_pin( io_pin_t pin );

/**
 * @fn      string_from_io_polarity( io_polarity_t )
 * @brief   Returns a string for the specified `io_polarity_t`.
 */
char const * string_from_io_polarity( io_polarity_t polarity );

/**
 * @fn      string_from_io_state( io_state_t )
 * @brief   Returns a string for the specified `io_state_t`.
 */
char const * string_from_io_state( io_state_t state );

/**
 * @fn      string_from_io_type( io_type_t )
 * @brief   Returns a string for the specified `io_type_t`.
 */
char const * string_from_io_type( io_type_t type );

/**
 * @fn      string_from_led( led_t )
 * @brief   Returns a string for the specified `led_t`.
 */
char const * string_from_led( led_t led );

/**
 * @fn      string_is_empty( char const * )
 * @brief   Returns `true` if the specified string is empty.
 */
bool string_is_empty( char const * str );

/**
 * @fn      string_to_bool( char const *, bool * )
 * @brief   Converts a string to a `bool`, if possible.
 * @returns `true` if the conversion succeeded.
 */
bool string_to_bool( char const * str, bool * b );

/**
 * @fn      string_to_element( char const *, wpm_element_t * )
 * @brief   Converts a string to a `wpm_element_t`, if possible.
 * @returns `true` if the conversion succeeded.
 */
bool string_to_element( char const * str, wpm_element_t * el );

/**
 * @fn      string_to_io_pin( char const *, io_pin_t * )
 * @brief   Converts a string to an `io_pin_t`, if possible.
 * @returns `true` if the conversion succeeded.
 */
bool string_to_io_pin( char const * str, io_pin_t * pin );

/**
 * @fn      string_to_io_polarity( char const *, io_polarity_t * )
 * @brief   Converts a string to an `io_polarity_t`, if possible.
 * @returns `true` if the conversion succeeded.
 */
bool string_to_io_polarity( char const * str, io_polarity_t * polarity );

/**
 * @fn      string_to_io_state( char const *, io_state_t )
 * @brief   Converts a string to an `io_state_t`, if possible.
 * @returns `true` if the conversion succeeded.
 */
bool string_to_io_state( char const * str, io_state_t * state );

/**
 * @fn      string_to_io_type( char const *, io_type_t * )
 * @brief   Converts a string to an `io_type_t`, if possible.
 * @returns `true` if the conversion succeeded.
 */
bool string_to_io_type( char const * str, io_type_t * type );

/**
 * @fn      string_to_led( char const *, led_t * )
 * @brief   Converts a string to an `led_t`, if possible.
 * @returns `true` if the conversion succeeded.
 */
bool string_to_led( char const * str, led_t * led );

#endif /* !defined( APPLICATION_STRINGS_H ) */
