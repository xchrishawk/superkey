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

#include "application/io.h"
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

#define IO_PIN_PREFIX_LEN 7
static char const * const s_io_pin_tbl[] =
{
    stringize( IO_PIN_TRS_0_TIP ),
    stringize( IO_PIN_TRS_0_RING ),
    stringize( IO_PIN_TRS_1_TIP ),
    stringize( IO_PIN_TRS_1_RING ),
    stringize( IO_PIN_TRS_2_TIP ),
    stringize( IO_PIN_TRS_2_RING ),
    stringize( IO_PIN_TRS_3_TIP ),
    stringize( IO_PIN_TRS_3_RING ),
};
_Static_assert( array_count( s_io_pin_tbl ) == IO_PIN_COUNT, "Invalid string table!" );

#define IO_POLARITY_PREFIX_LEN 12
static char const * const s_io_polarity_tbl[] =
{
    stringize( IO_POLARITY_ACTIVE_LOW ),
    stringize( IO_POLARITY_ACTIVE_HIGH ),
};
_Static_assert( array_count( s_io_polarity_tbl ) == IO_POLARITY_COUNT, "Invalid string table!" );

#define IO_STATE_PREFIX_LEN 9
static char const * const s_io_state_tbl[] =
{
    stringize( IO_STATE_OFF ),
    stringize( IO_STATE_ON ),
    stringize( IO_STATE_NONE ),
};
_Static_assert( array_count( s_io_state_tbl ) == IO_STATE_COUNT + 1 /* for NONE */, "Invalid string table!" );

#define IO_TYPE_PREFIX_LEN 8
static char const * const s_io_type_tbl[] =
{
    stringize( IO_TYPE_INPUT_STRAIGHT_KEY ),
    stringize( IO_TYPE_INPUT_PADDLE_LEFT ),
    stringize( IO_TYPE_INPUT_PADDLE_RIGHT ),
    stringize( IO_TYPE_OUTPUT_KEYER ),
    stringize( IO_TYPE_NONE ),
};
_Static_assert( array_count( s_io_type_tbl ) == IO_TYPE_COUNT + 1 /* for NONE */, "Invalid string table!" );

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


char const * string_from_io_pin( io_pin_t pin )
{
    return( find_string( s_io_pin_tbl, array_count( s_io_pin_tbl ), IO_PIN_PREFIX_LEN, ( size_t )pin ) );

}   /* string_from_io_pin() */


char const * string_from_io_polarity( io_polarity_t polarity )
{
    return( find_string( s_io_polarity_tbl, array_count( s_io_polarity_tbl ), IO_POLARITY_PREFIX_LEN, ( size_t )polarity ) );

}   /* string_from_io_polarity() */


char const * string_from_io_state( io_state_t state )
{
    return( find_string( s_io_state_tbl, array_count( s_io_state_tbl ), IO_STATE_PREFIX_LEN, ( size_t )state ) );

}   /* string_from_io_state() */


char const * string_from_io_type( io_type_t type )
{
    return( find_string( s_io_type_tbl, array_count( s_io_type_tbl ), IO_TYPE_PREFIX_LEN, ( size_t )type ) );

}   /* string_from_io_type() */


char const * string_from_led( led_t led )
{
    return( find_string( s_led_tbl, array_count( s_led_tbl ), LED_PREFIX_LEN, ( size_t )led ) );

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
    if( result == array_count( s_element_tbl ) )
        return( false );

    * el = ( wpm_element_t )result;
    return( true );

}   /* string_to_element() */


bool string_to_io_pin( char const * str, io_pin_t * pin )
{
    size_t result = find_value( s_io_pin_tbl, array_count( s_io_pin_tbl ), IO_PIN_PREFIX_LEN, str );
    if( result == array_count( s_io_pin_tbl ) )
        return( false );

    * pin = ( io_pin_t )result;
    return( true );

}   /* string_to_io_pin() */


bool string_to_io_polarity( char const * str, io_polarity_t * polarity )
{
    size_t result = find_value( s_io_polarity_tbl, array_count( s_io_polarity_tbl ), IO_POLARITY_PREFIX_LEN, str );
    if( result == array_count( s_io_polarity_tbl ) )
        return( false );

    * polarity = ( io_polarity_t )result;
    return( true );

}   /* string_to_io_polarity() */


bool string_to_io_state( char const * str, io_state_t * state )
{
    size_t result = find_value( s_io_state_tbl, array_count( s_io_state_tbl ), IO_STATE_PREFIX_LEN, str );
    if( result == array_count( s_io_state_tbl ) )
        return( false );

    * state = ( io_state_t )result;
    return( true );

}   /* string_to_io_state() */


bool string_to_io_type( char const * str, io_type_t * type )
{
    size_t result = find_value( s_io_type_tbl, array_count( s_io_type_tbl ), IO_TYPE_PREFIX_LEN, str );
    if( result == array_count( s_io_type_tbl ) )
        return( false );

    * type = ( io_type_t )result;
    return( true );

}   /* string_to_io_type() */


bool string_to_led( char const * str, led_t * led )
{
    size_t result = find_value( s_led_tbl, array_count( s_led_tbl ), LED_PREFIX_LEN, str );
    if( result == array_count( s_led_tbl ) )
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
