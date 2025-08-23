/**
 * @file    src/exakey/utility/constants.h
 * @brief   File containing generic global constants.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-19
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
 * @def     MSEC_PER_SEC
 * @brief   The number of milliseconds per second.
 */
#define MSEC_PER_SEC            1000

/**
 * @def     SEC_PER_MIN
 * @brief   The number of seconds per minute.
 */
#define SEC_PER_MIN             60

/**
 * @def     MIN_PER_HOUR
 * @brief   The number of minutes per hour.
 */
#define MIN_PER_HOUR            60

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
