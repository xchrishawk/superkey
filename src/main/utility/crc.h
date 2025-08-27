/**
 * @file        src/main/utility/crc.h
 * @brief       Header for the CRC module.
 *
 * @author      Chris Vig (chris@invictus.so)
 * @date        2025-08-26
 * @copyright   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 */

#if !defined( UTILITY_CRC_H )
#define UTILITY_CRC_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdint.h>
#include <stdlib.h>

/* ----------------------------------------------------- TYPES ------------------------------------------------------ */

/**
 * @typedef crc8_t
 * @brief   Typedef for an 8-bit CRC.
 */
typedef uint8_t crc8_t;

/**
 * @typedef crc16_t
 * @brief   Typedef for a 16-bit CRC.
 */
typedef uint16_t crc16_t;

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      crc_calc_crc8( void const *, size_t )
 * @brief   Calculates an 8-bit CRC over the specified buffer.
 */
crc8_t crc_calc_crc8( void const * buf, size_t size );

/**
 * @fn      crc_calc_crc16( void const *, size_t )
 * @brief   Calculates a 16-bit CRC over the specified buffer.
 */
crc16_t crc_calc_crc16( void const * buf, size_t size );

#endif /* !defined( UTILITY_CRC_H ) */
