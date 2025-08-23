/**
 * @file    src/exakey/core/version.h
 * @brief   Header defining application version information.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-23
 */

#if !defined( CORE_VERSION_H )
#define CORE_VERSION_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdint.h>

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

/** The name of the executable file (i.e., `exakey`). */
extern char const * const EXAKEY_NAME;

/** The user-viewable product name (i.e., `ExaKey`). */
extern char const * const EXAKEY_PRODUCT_NAME;

/** The combined version string (i.e., `1.0.0`). */
extern char const * const EXAKEY_VERSION;

/** The major version number. */
extern uint8_t const EXAKEY_VERSION_MAJOR;

/** The minor version number. */
extern uint8_t const EXAKEY_VERSION_MINOR;

/** The revision version number. */
extern uint8_t const EXAKEY_VERSION_REVISION;

/** The directory in which the application was built. */
extern char const * const EXAKEY_BUILD_DIR;

/** The date on which the application was built. */
extern char const * const EXAKEY_BUILD_DATE;

/** The time at which the application was built. */
extern char const * const EXAKEY_BUILD_TIME;

/** The full Git hash of the active commit at the time the application was built. */
extern char const * const EXAKEY_BUILD_GIT_HASH;

/** The short Git hash of the active commit at the time the application was built. */
extern char const * const EXAKEY_BUILD_GIT_HASH_SHORT;

/** The branch name of the active commit at the time the application was built. */
extern char const * const EXAKEY_BUILD_GIT_BRANCH;

/* ----------------------------------------------------- TYPES ------------------------------------------------------ */

/**
 * @typedef version_t
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
