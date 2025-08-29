/**
 * @file    src/main/application/storage.h
 * @brief   Header for the application-specific storage module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-26
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 */

#if !defined( APPLICATION_STORAGE_H )
#define APPLICATION_STORAGE_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stdlib.h>

#include "application/config.h"

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      storage_get_config( config_t * )
 * @brief   Gets the stored application configuration from storage.
 * @returns `true` if the configuration was successfully retrieved.
 * @note    This will return `false` if the version number does not match the specified value, if the stored size is
 *          invalid, or if the CRC does not match.
 */
bool storage_get_config( config_version_t version, size_t size, void * config );

/**
 * @fn      storage_init( void )
 * @brief   Initializes the storage module.
 * @note    Does not currently depend on any other modules being initialized.
 */
void storage_init( void );

/**
 * @fn      storage_set_config( config_t const * )
 * @brief   Stores the specified configuration.
 */
void storage_set_config( config_version_t version, size_t size, void * config );

#endif /* !defined( APPLICATION_STORAGE_H ) */
