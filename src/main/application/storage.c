/**
 * @file        src/main/application/storage.c
 * @brief       Implementation for the application-specific storage module.
 *
 * @author      Chris Vig (chris@invictus.so)
 * @date        2025-08-26
 * @copyright   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdint.h>
#include <string.h>

#include "application/storage.h"
#include "drivers/eeprom.h"
#include "utility/crc.h"
#include "utility/types.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

// Same for all layout verions
#define EEPROM_ADDR_RESERVED_START      0x0000
#define EEPROM_ADDR_LAYOUT_VERSION      ( EEPROM_ADDR_RESERVED_START )
#define EEPROM_ADDR_RESERVED_END        EEPROM_ADDR_RESERVED_START + 0x0100

// EEPROM addresses for version 1
#define EEPROM_ADDR_V1_CONFIG_BASE      EEPROM_ADDR_RESERVED_END
#define EEPROM_ADDR_V1_CONFIG_VERSION   ( EEPROM_ADDR_V1_CONFIG_BASE )
#define EEPROM_ADDR_V1_CONFIG_SIZE      ( EEPROM_ADDR_V1_CONFIG_VERSION + sizeof( config_version_t ) )
#define EEPROM_ADDR_V1_CONFIG_CRC16     ( EEPROM_ADDR_V1_CONFIG_SIZE + sizeof( size_t ) )
#define EEPROM_ADDR_V1_CONFIG_DATA      ( EEPROM_ADDR_V1_CONFIG_CRC16 + sizeof( crc16_t ) )
#define EEPROM_ADDR_V1_CONFIG_END       EEPROM_ADDR_V1_CONFIG_BASE + 0x0100
_Static_assert( EEPROM_ADDR_V1_CONFIG_DATA + sizeof( config_t ) < EEPROM_ADDR_V1_CONFIG_END,
                "Not enough space allocated!" );

// EEPROM addresses for current version
#define EEPROM_ADDR_CONFIG_VERSION      EEPROM_ADDR_V1_CONFIG_VERSION
#define EEPROM_ADDR_CONFIG_SIZE         EEPROM_ADDR_V1_CONFIG_SIZE
#define EEPROM_ADDR_CONFIG_CRC16        EEPROM_ADDR_V1_CONFIG_CRC16
#define EEPROM_ADDR_CONFIG_DATA         EEPROM_ADDR_V1_CONFIG_DATA

/* ----------------------------------------------------- TYPES ------------------------------------------------------ */

/**
 * @typedef layout_version_t
 * @brief   Enumeration of supported storage layout versions.
 */
typedef uint8_t layout_version_t;
enum
{
    LAYOUT_VERSION_INVALID_0    = 0x00,
    LAYOUT_VERSION_INVALID_1    = 0xFF,

    LAYOUT_VERSION_1            = 0x01,

    LAYOUT_VERSION_CURRENT
        = LAYOUT_VERSION_1,
};

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      reinit_layout( void )
 * @brief   Re-initializes the storage layout.
 */
static void reinit_layout( void );

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

bool storage_get_config( config_version_t version, size_t size, void * config )
{
    // Validate version
    config_version_t check_version;
    eeprom_read( EEPROM_ADDR_CONFIG_VERSION, & check_version, sizeof( config_version_t ) );
    if( check_version != version )
        return( false );

    // Validate size
    size_t check_size;
    eeprom_read( EEPROM_ADDR_CONFIG_SIZE, & check_size, sizeof( size_t ) );
    if( check_size != size )
        return( false );

    // Read config
    eeprom_read( EEPROM_ADDR_CONFIG_DATA, config, size );

    // Validate CRC
    crc16_t check_crc;
    eeprom_read( EEPROM_ADDR_CONFIG_CRC16, & check_crc, sizeof( crc16_t ) );
    if( check_crc != crc_calc_crc16( config, sizeof( config_t ) ) )
    {
        memset( config, 0, size );
        return( false );
    }

    return( true );

}   /* storage_get_config() */


void storage_init( void )
{
    // Confirm we have the right layout version
    layout_version_t layout = ( layout_version_t )eeprom_read_byte( EEPROM_ADDR_LAYOUT_VERSION );
    if( layout != LAYOUT_VERSION_CURRENT )
        reinit_layout();

}   /* storage_init() */


void storage_set_config( config_version_t version, size_t size, void * config )
{
    crc16_t crc = crc_calc_crc16( config, size );

    eeprom_write( EEPROM_ADDR_CONFIG_VERSION, & version, sizeof( config_version_t ) );
    eeprom_write( EEPROM_ADDR_CONFIG_SIZE, & size, sizeof( size_t ) );
    eeprom_write( EEPROM_ADDR_CONFIG_CRC16, & crc, sizeof( crc16_t ) );
    eeprom_write( EEPROM_ADDR_CONFIG_DATA, config, size );

}   /* storage_set_config() */


static void reinit_layout( void )
{
    // Write layout byte to required address
    eeprom_write_byte( EEPROM_ADDR_LAYOUT_VERSION, ( byte_t )LAYOUT_VERSION_CURRENT );

}   /* reinit_layout() */
