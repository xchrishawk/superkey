/**
 * @file    src/main/core/version.h
 * @brief   Header defining application version information.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-23
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 *
 * @details
 * This module provides accessors to get the application's version information.
 */

#if !defined( CORE_VERSION_H )
#define CORE_VERSION_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdint.h>

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

/** The major version number. */
extern uint8_t const PRODUCT_VERSION_MAJOR;

/** The minor version number. */
extern uint8_t const PRODUCT_VERSION_MINOR;

/** The revision version number. */
extern uint8_t const PRODUCT_VERSION_REVISION;

/* ----------------------------------------------------- TYPES ------------------------------------------------------ */

/**
 * @struct  version_t
 * @brief   Struct containing version information for the application.
 */
typedef struct
{
    char const *    product_name;           /**< The user-viewable product name.        */
    char const *    executable_name;        /**< The name of the executable.            */
    char const *    version;                /**< The version string of the application. */
    uint8_t         version_major;          /**< The major version number.              */
    uint8_t         version_minor;          /**< The minor version number.              */
    uint8_t         version_revision;       /**< The version revision.                  */
    char const *    build_type;             /**< The build type.                        */
    char const *    build_dir;              /**< The build directory for this build.    */
    char const *    build_date;             /**< The date at which the SW was built.    */
    char const *    build_time;             /**< The time at which the SW was built.    */
    char const *    git_hash;               /**< The long Git hash for the commit.      */
    char const *    git_hash_short;         /**< The short Git hash for the commit.     */
    char const *    git_branch;             /**< The Git branch name.                   */

} version_t;

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      version_get( version_t * )
 * @brief   Returns version information for the current version of the application.
 */
void version_get( version_t * version );

#endif /* !defined( CORE_VERSION_H ) */
