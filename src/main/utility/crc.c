/**
 * @file        src/main/utility/crc.c
 * @brief       Implementation for the CRC module.
 *
 * @author      Chris Vig (chris@invictus.so)
 * @date        2025-08-26
 * @copyright   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdint.h>
#include <stdlib.h>

#include <util/crc16.h>

#include "utility/crc.h"
#include "utility/types.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

/**
 * @def     CRC8_SEED
 * @brief   The seed value for 8-bit CRCs.
 */
#define CRC8_SEED       0xFF

/**
 * @def     CRC16_SEED
 * @brief   The seed value for 16-bit CRCs.
 */
#define CRC16_SEED      0xFFFF

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

crc8_t crc_calc_crc8( void const * buf, size_t size )
{
    byte_t const * ptr = ( byte_t const * )buf;

    crc8_t crc = CRC8_SEED;
    while( size-- )
        crc = _crc8_ccitt_update( crc, *( ptr++ ) );

    return( crc );

}   /* crc_calc_crc8() */


crc16_t crc_calc_crc16( void const * buf, size_t size )
{
    byte_t const * ptr = ( byte_t const * )buf;

    crc16_t crc = CRC16_SEED;
    while( size-- )
        crc = _crc16_update( crc, *( ptr++ ) );

    return( crc );

}   /* crc_calc_crc16() */
