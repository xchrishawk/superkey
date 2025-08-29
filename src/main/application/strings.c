/**
 * @file    src/main/application/strings.c
 * @brief   Implementation for the strings module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-28
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <string.h>

#include "application/strings.h"
#include "utility/constants.h"
#include "utility/utility.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

static char const * const s_bool_tbl[] =
{
    stringize( false ),
    stringize( true ),
};
_Static_assert( array_count( s_bool_tbl ) == 2, "Invalid string table!" );

#define ELEMENT_PREFIX_LEN 12
static char const * const s_element_tbl[] =
{
    stringize( WPM_ELEMENT_DOT ),
    stringize( WPM_ELEMENT_DASH ),
    stringize( WPM_ELEMENT_ELEMENT_SPACE ),
    stringize( WPM_ELEMENT_LETTER_SPACE ),
    stringize( WPM_ELEMENT_WORD_SPACE ),
};
_Static_assert( array_count( s_element_tbl ) == WPM_ELEMENT_COUNT, "Invalid string table!" );

#define INPUT_PIN_PREFIX_LEN 10
static char const * const s_input_pin_tbl[] =
{
    stringize( INPUT_PIN_TRS_0_TIP ),
    stringize( INPUT_PIN_TRS_0_RING ),
    stringize( INPUT_PIN_TRS_1_TIP ),
    stringize( INPUT_PIN_TRS_1_RING ),
    stringize( INPUT_PIN_TRS_2_TIP ),
    stringize( INPUT_PIN_TRS_2_RING ),
};
_Static_assert( array_count( s_input_pin_tbl ) == INPUT_PIN_COUNT, "Invalid string table!" );

#define INPUT_POLARITY_PREFIX_LEN 15
static char const * const s_input_polarity_tbl[] =
{
    stringize( INPUT_POLARITY_ACTIVE_LOW ),
    stringize( INPUT_POLARITY_ACTIVE_HIGH ),
};
_Static_assert( array_count( s_input_polarity_tbl ) == INPUT_POLARITY_COUNT, "Invalid string table!" );

#define INPUT_TYPE_PREFIX_LEN 11
static char const * const s_input_type_tbl[] =
{
    stringize( INPUT_TYPE_STRAIGHT_KEY ),
    stringize( INPUT_TYPE_PADDLE_LEFT ),
    stringize( INPUT_TYPE_PADDLE_RIGHT ),
};
_Static_assert( array_count( s_input_type_tbl ) == INPUT_TYPE_COUNT, "Invalid string table!" );

#define LED_PREFIX_LEN 4
static char const * const s_led_tbl[] =
{
    stringize( LED_STATUS ),
    stringize( LED_KEY ),
};
_Static_assert( array_count( s_led_tbl ) == LED_COUNT, "Invalid string table!" );

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      find_string( char const * const *, size_t, size_t )
 * @brief   Returns the string for the specified value from the specified table.
 */
char const * find_string( char const * const * tbl,
                          size_t tbl_count,
                          size_t prefix_len,
                          size_t index );

/**
 * @fn      find_value( char const * const *, size_t, char const * )
 * @brief   Returns the value for the specified string from the specified table.
 */
size_t find_value( char const * const * tbl,
                   size_t tbl_count,
                   size_t prefix_len,
                   char const * str );

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

bool string_begins_with( char const * str, char const * token )
{
    return( ! strncasecmp( str, token, strlen( token ) ) );

}   /* string_begins_with() */


bool string_equals( char const * str, char const * token )
{
    return( ! strcasecmp( str, token ) );

}   /* string_equals() */


char const * string_from_bool( bool b )
{
    return( find_string( s_bool_tbl, array_count( s_bool_tbl ), 0, ( size_t )b ) );

}   /* string_from_bool() */


char const * string_from_element( wpm_element_t el )
{
    return( find_string( s_element_tbl, array_count( s_element_tbl ), ELEMENT_PREFIX_LEN, ( size_t )el ) );

}   /* string_from_element() */


char const * string_from_input_pin( input_pin_t pin )
{
    return( find_string( s_input_pin_tbl, INPUT_PIN_COUNT, INPUT_PIN_PREFIX_LEN, ( size_t )pin ) );

}   /* string_from_input_pin() */


char const * string_from_input_polarity( input_polarity_t polarity )
{
    return( find_string( s_input_polarity_tbl, INPUT_POLARITY_COUNT, INPUT_POLARITY_PREFIX_LEN, ( size_t )polarity ) );

}   /* string_from_input_polarity() */


char const * string_from_input_type( input_type_t type )
{
    return( find_string( s_input_type_tbl, INPUT_TYPE_COUNT, INPUT_TYPE_PREFIX_LEN, ( size_t )type ) );

}   /* string_from_input_type() */


char const * string_from_led( led_t led )
{
    return( find_string( s_led_tbl, LED_COUNT, LED_PREFIX_LEN, ( size_t )led ) );

}   /* string_from_led() */


bool string_is_empty( char const * str )
{
    return( ( * str ) == NULL_CHAR );

}   /* string_is_empty() */


bool string_to_bool( char const * str, bool * b )
{
    size_t result = find_value( s_bool_tbl, array_count( s_bool_tbl ), 0, str );
    if( result == array_count( s_bool_tbl ) )
        return( false );

    * b = ( bool )result;
    return( true );

}   /* string_to_bool() */


bool string_to_element( char const * str, wpm_element_t * el )
{
    size_t result = find_value( s_element_tbl, array_count( s_element_tbl ), ELEMENT_PREFIX_LEN, str );
    if( result == WPM_ELEMENT_COUNT )
        return( false );

    * el = ( wpm_element_t )result;
    return( true );

}   /* string_to_element() */


bool string_to_input_pin( char const * str, input_pin_t * pin )
{
    size_t result = find_value( s_input_pin_tbl, array_count( s_input_pin_tbl ), INPUT_PIN_PREFIX_LEN, str );
    if( result == INPUT_PIN_COUNT )
        return( false );

    * pin = ( input_pin_t )result;
    return( true );

}   /* string_to_input_pin() */


bool string_to_input_polarity( char const * str, input_polarity_t * polarity )
{
    size_t result = find_value( s_input_polarity_tbl, array_count( s_input_polarity_tbl ), INPUT_POLARITY_PREFIX_LEN, str );
    if( result == INPUT_POLARITY_COUNT )
        return( false );

    * polarity = ( input_polarity_t )result;
    return( true );

}   /* string_to_input_polarity() */


bool string_to_input_type( char const * str, input_type_t * type )
{
    size_t result = find_value( s_input_type_tbl, array_count( s_input_type_tbl ), INPUT_TYPE_PREFIX_LEN, str );
    if( result == INPUT_TYPE_COUNT )
        return( false );

    * type = ( input_type_t )result;
    return( true );

}   /* string_to_input_type() */


bool string_to_led( char const * str, led_t * led )
{
    size_t result = find_value( s_led_tbl, array_count( s_led_tbl ), LED_PREFIX_LEN, str );
    if( result == LED_COUNT )
        return( false );

    * led = ( led_t )result;
    return( true );

}   /* string_to_led() */


char const * find_string( char const * const * tbl,
                          size_t tbl_count,
                          size_t prefix_len,
                          size_t index )
{
    if( index < tbl_count )
        return( tbl[ index ] + prefix_len );
    else
        return( "Unknown" );

}   /* find_string() */


size_t find_value( char const * const * tbl,
                   size_t tbl_count,
                   size_t prefix_len,
                   char const * str )
{
    for( size_t index = 0; index < tbl_count; index++ )
        if( string_equals( str, tbl[ index ] + prefix_len ) )
            return index;

    return( tbl_count );

}   /* find_value() */
