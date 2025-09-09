/**
 * @file    src/main/application/storage.c
 * @brief   Implementation for the application-specific storage module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-26
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "application/storage.h"
#include "drivers/eeprom.h"
#include "utility/crc.h"
#include "utility/debug.h"
#include "utility/types.h"
#include "utility/utility.h"

/* ----------------------------------------------------- TYPES ------------------------------------------------------ */

/**
 * @typedef layout_t
 * @brief   Enumeration of the known storage layouts.
 */
typedef uint8_t layout_t;
enum
{
    LAYOUT_V1 = 0x01,

    LAYOUT_CURRENT = LAYOUT_V1,

    LAYOUT_INVALID_0x00 = 0x00,
    LAYOUT_INVALID_0xFF = 0xFF,
};

/**
 * @typedef slot_t
 * @brief   Enumeration of the data "slots" that the storage system supports.
 * @note    The intent of the slot system is to double-buffer stored data, so that a power loss or other failure while
 *          writing will not corrupt the valid data in the other slot.
 */
typedef uint8_t slot_t;
enum
{
    SLOT_0,                                 /**< First slot.                            */
    SLOT_1,                                 /**< Second slot.                           */

    SLOT_COUNT,                             /**< Number of slots.                       */
};

/**
 * @struct  storage_header_t
 * @brief   Struct defining the layout for a storage header.
 * @note    This type is not intended to be instantiated - it is used only to calculate offsets.
 */
typedef struct
{
    bool                valid;              /**< Is data in this slot valid?            */
    size_t              size;               /**< Size of data in this slot.             */
    crc16_t             crc;                /**< CRC for this slot.                     */

} PACKED_STRUCT storage_header_t;

/**
 * @struct  storage_config_t
 * @brief   Struct defining the layout for each configuration slot.
 * @note    This type is not intended to be instantiated - it is used only to calculate offsets.
 */
typedef struct
{
    storage_header_t    header;             /**< Header for this slot.                  */
    byte_t              data[ STORAGE_CONFIG_SIZE ]; /**< Data for this slot.           */

} PACKED_STRUCT storage_config_t;

/**
 * @struct  storage_t
 * @brief   Struct defining the overall layout for the entire EEPROM storage.
 * @note    This type is not intended to be instantiated - it is used only to calculate offsets.
 */
typedef struct
{
    layout_t            layout;             /**< Storage layout version.                */
    storage_config_t    config[ SLOT_COUNT ]; /**< Slots for configuration storage.     */

} PACKED_STRUCT storage_t;

/* ----------------------------------------------------- MACROS ----------------------------------------------------- */

_Static_assert( sizeof( storage_t ) <= EEPROM_COUNT, "Too much storage allocated!" );

/**
 * @def     ADDR( _field )
 * @brief   Returns the EEPROM address for the specified field defined in `storage_t`.
 */
#define ADDR( _field )                                                                  \
    ( ( eeprom_addr_t )( offsetof( storage_t, _field ) ) )

/**
 * @def     OTHER_SLOT( _slot )
 * @brief   Returns the opposite slot index.
 */
#define OTHER_SLOT( _slot )                                                             \
    ( ( _slot ) == SLOT_0 ? SLOT_1 : SLOT_0 )

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      config_addresses( slot_t, eeprom_addr_t *, eeprom_addr_t *, eeprom_addr_t *, eeprom_addr_t * )
 * @brief   Gets the EEPROM addresses for the configuration data in the specified slot.
 */
static void config_addresses( slot_t slot,
                              eeprom_addr_t * valid_addr,
                              eeprom_addr_t * size_addr,
                              eeprom_addr_t * crc_addr,
                              eeprom_addr_t * data_addr );

/**
 * @fn      init_layout( void )
 * @brief   Initializes the EEPROM layout, invalidating any data currently present.
 */
static void init_layout( void );

/**
 * @fn      is_config_valid( slot_t )
 * @brief   Returns `true` if the `valid` flag is set to `true` (i.e., `1`) for the specified configuration data slot.
 * @note    This only checks the `valid` flag - it does not validate the size or CRC.
 */
static bool is_config_valid( slot_t slot );

/**
 * @fn      read_config( slot_t, void *, size_t )
 * @brief   Reads configuration data from the specified slot.
 * @returns `true` if the data was successfully read into the buffer.
 */
static bool read_config( slot_t slot, void * data, size_t size );

/**
 * @fn      write_config( slot_t, void const *, size_t )
 * @brief   Writes the specified configuration data to the specified slot.
 */
static void write_config( slot_t slot, void const * data, size_t size );

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

bool storage_get_config( void * data, size_t size )
{
    for( slot_t slot = 0; slot < SLOT_COUNT; slot++ )
        if( read_config( slot, data, size ) )
            return( true );

    return( false );

}   /* storage_get_config() */


void storage_init( void )
{
    // Reinitialize the storage layout if incorrect
    layout_t layout = ( layout_t )eeprom_read_byte( ADDR( layout ) );
    if( layout != LAYOUT_CURRENT )
        init_layout();

}   /* storage_init() */


void storage_set_config( void const * data, size_t size )
{
    // Write to first empty slot
    for( slot_t slot = 0; slot < SLOT_COUNT; slot++ )
    {
        if( is_config_valid( slot ) )
            continue;
        write_config( slot, data, size );
        return;
    }

    // If no empty slot found, overwrite first slot
    write_config( SLOT_0, data, size );

}   /* storage_set_config() */


static void config_addresses( slot_t slot,
                              eeprom_addr_t * valid_addr,
                              eeprom_addr_t * size_addr,
                              eeprom_addr_t * crc_addr,
                              eeprom_addr_t * data_addr )
{
    // Calculate base address
    eeprom_addr_t base = ADDR( config );
    base += sizeof( storage_config_t ) * slot;

    // Assign individual addresses
    if( valid_addr )
        * valid_addr = base + offsetof( storage_config_t, header.valid );
    if( size_addr )
        * size_addr = base + offsetof( storage_config_t, header.size );
    if( crc_addr )
        * crc_addr = base + offsetof( storage_config_t, header.crc );
    if( data_addr )
        * data_addr = base + offsetof( storage_config_t, data );

}   /* config_addresses() */


static void init_layout( void )
{
    // Clear all validity flags
    eeprom_write_byte( ADDR( config[ SLOT_0 ].header.valid ), ( byte_t )false );
    eeprom_write_byte( ADDR( config[ SLOT_1 ].header.valid ), ( byte_t )false );

    // Write layout byte
    eeprom_write_byte( ADDR( layout ), ( byte_t )LAYOUT_CURRENT );

}   /* init_layout() */


static bool is_config_valid( slot_t slot )
{
    eeprom_addr_t valid_addr = EEPROM_COUNT;
    config_addresses( slot, & valid_addr, NULL, NULL, NULL );

    bool valid = false;
    eeprom_read( valid_addr, & valid, sizeof( valid ) );

    // We do an explicit comparison against true (i.e., 1) here.
    // This is to prevent the empty value of 0xFF from being interpreted as a "valid" flag.
    return( valid == true );

}   /* is_config_valid() */


static bool read_config( slot_t slot, void * data, size_t size )
{
    // Read and check validity flag
    if( ! is_config_valid( slot ) )
        return( false );

    // Get other addresses we need
    eeprom_addr_t size_addr, crc_addr, data_addr;
    config_addresses( slot, NULL, & size_addr, & crc_addr, & data_addr );

    // Read and check stored size
    size_t stored_size = 0;
    eeprom_read( size_addr, & stored_size, sizeof( stored_size ) );
    if( stored_size > size )
        return( false );

    // Read data into buffer
    eeprom_read( data_addr, data, size );

    // Calculate CRC for data we read out
    crc16_t crc = crc_calc_crc16( data, size );

    // Read and check stored CRC
    crc16_t stored_crc = 0;
    eeprom_read( crc_addr, & stored_crc, sizeof( stored_crc ) );
    if( stored_crc != crc )
        return( false );

    return( true );

}   /* read_config() */


static void write_config( slot_t slot, void const * data, size_t size )
{
    // Get EEPROM addresses
    eeprom_addr_t valid_addr, size_addr, crc_addr, data_addr, other_valid_addr;
    config_addresses( slot, & valid_addr, & size_addr, & crc_addr, & data_addr );
    config_addresses( OTHER_SLOT( slot ), & other_valid_addr, NULL, NULL, NULL );

    // Get metadata
    bool valid = true;
    crc16_t crc = crc_calc_crc16( data, size );

    // Write data buffer
    eeprom_write( data_addr, data, size );

    // Write metadata, setting the validity flag last
    eeprom_write( size_addr, & size, sizeof( size ) );
    eeprom_write( crc_addr, & crc, sizeof( crc ) );
    eeprom_write( valid_addr, & valid, sizeof( valid ) );

    // Finally, clear the validity byte for the opposite-side config
    valid = false;
    eeprom_write( other_valid_addr, & valid, sizeof( valid ) );

}   /* write_config() */
