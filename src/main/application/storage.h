/**
 * @file    src/main/application/storage.h
 * @brief   Header for the application-specific storage module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-26
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 *
 * @details
 * This module acts as an abstract data storage mechanism for the rest of the application. It hides the details of the
 * underlying EEPROM interface, and automatically handles versioning, CRCs, etc.
 */

#if !defined( APPLICATION_STORAGE_H )
#define APPLICATION_STORAGE_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stdlib.h>

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

/**
 * @def     STORAGE_CONFIG_SIZE
 * @brief   The number of bytes allocated for each configuration data slot.
 * @note    The config module should fail a static assert if this size is not large enough.
 */
#define STORAGE_CONFIG_SIZE     ( 64 )

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      storage_get_config( void * data, size_t size )
 * @brief   Gets the stored configuration from non-volatile memory.
 */
bool storage_get_config( void * data, size_t size );

/**
 * @fn      storage_init( void )
 * @brief   Initializes the storage module.
 * @note    Does not currently depend on any other modules being initialized.
 */
void storage_init( void );

/**
 * @fn      storage_set_config( void const *, size_t )
 * @brief   Sets the stored configuration in non-volatile memory.
 */
void storage_set_config( void const * data, size_t size );

#endif /* !defined( APPLICATION_STORAGE_H ) */
