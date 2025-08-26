/**
 * @file    src/main/drivers/eeprom.h
 * @brief   Header for the EEPROM driver module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-25
 */

#if !defined( DRIVERS_EEPROM_H )
#define DRIVERS_EEPROM_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdlib.h>

#include "utility/types.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

/**
 * @def     EEPROM_COUNT
 * @brief   The number of bytes of EEPROM memory available.
 */
#define EEPROM_COUNT        4096

/* ----------------------------------------------------- TYPES ------------------------------------------------------ */

/**
 * @typedef eeprom_addr_t
 * @brief   Typedef representing an EEPROM byte address.
 */
typedef uint16_t eeprom_addr_t;

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      eeprom_erase_byte( eeprom_addr_t addr )
 * @brief   Synchronously erases the byte at the specified EEPROM address.
 * @note    This function blocks until the EEPROM is ready to write data.
 */
void eeprom_erase_byte( eeprom_addr_t addr );

/**
 * @fn      eeprom_read( eeprom_addr_t, byte_t *, size_t )
 * @brief   Reads the specified number of bytes from the specified EEPROM address into a buffer.
 */
void eeprom_read( eeprom_addr_t addr, void * buf, size_t size );

/**
 * @fn      eeprom_read_byte( eeprom_addr_t addr )
 * @brief   Returns the byte at the specified EEPROM address.
 * @note    This function blocks until the EEPROM is ready to read data.
 */
byte_t eeprom_read_byte( eeprom_addr_t addr );

/**
 * @fn      eeprom_write( eeprom_addr_t, byte_t const *, size_t )
 * @brief   Writes the specified data buffer to the specified EEPROM address.
 */
void eeprom_write( eeprom_addr_t addr, void const * buf, size_t size );

/**
 * @fn      eeprom_write_byte( eeprom_addr_t addr, byte_t byte )
 * @brief   Synchronously writes the byte at the specified EEPROM address.
 * @note    This function blocks until the EEPROM is ready to write data.
 */
void eeprom_write_byte( eeprom_addr_t addr, byte_t byte );

#endif /* !defined( DRIVERS_EEPROM_H ) */
