/**
 * @file    src/main/utility/constants.h
 * @brief   File containing generic global constants.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-19
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 *
 * @details
 * Defines shared constants for the entire application.
 */

#if !defined( UTILITY_CONSTANTS_H )
#define UTILITY_CONSTANTS_H

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

/**
 * @def     BITS_PER_BYTE
 * @brief   The number of bits per byte.
 */
#define BITS_PER_BYTE           8

/**
 * @def     USEC_PER_MSEC
 * @brief   The number of microseconds per millisecond.
 */
#define USEC_PER_MSEC           1000UL

/**
 * @def     MSEC_PER_SEC
 * @brief   The number of milliseconds per second.
 */
#define MSEC_PER_SEC            1000UL

/**
 * @def     SEC_PER_MIN
 * @brief   The number of seconds per minute.
 */
#define SEC_PER_MIN             60UL

/**
 * @def     MIN_PER_HOUR
 * @brief   The number of minutes per hour.
 */
#define MIN_PER_HOUR            60UL

/**
 * @def     NULL_CHAR
 * @brief   The null character (ASCII value 0).
 */
#define NULL_CHAR               ( char )0

/**
 * @def     LINE_FEED_CHAR
 * @brief   The line feed character (ASCII value 10).
 */
#define LINE_FEED_CHAR          ( char )10

/**
 * @def     CARRIAGE_RETURN_CHAR
 * @brief   The carriage return character (ASCII value 13).
 */
#define CARRIAGE_RETURN_CHAR    ( char )13

/**
 * @def     NEWLINE_STR
 * @brief   String resulting in a newline on Windows.
 */
#define NEWLINE_STR             "\r\n"

#endif /* !defined( UTILITY_CONSTANTS_H ) */
