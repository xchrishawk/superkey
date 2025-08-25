/**
 * @file    src/main/core/version.c
 * @brief   Implementation for the version module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-23
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include "autogen/build.h"
#include "core/version.h"
#include "utility/utility.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

// Public variables
uint8_t const PRODUCT_VERSION_MAJOR = _VERSION_MAJOR;
uint8_t const PRODUCT_VERSION_MINOR = _VERSION_MINOR;
uint8_t const PRODUCT_VERSION_REVISION = _VERSION_REVISION;

// Auto-generated variables
static char const * const PRODUCT_NAME = _EXECUTABLE_PROJECT_NAME;
static char const * const EXECUTABLE_NAME = _EXECUTABLE_NAME;
static char const * const VERSION = stringize_value( _VERSION_MAJOR ) "."
                                    stringize_value( _VERSION_MINOR ) "."
                                    stringize_value( _VERSION_REVISION );
static char const * const BUILD_TYPE = _BUILD_TYPE;
static char const * const BUILD_DIR = _EXECUTABLE_PROJECT_BUILD_DIR;
static char const * const BUILD_DATE = _EXECUTABLE_BUILD_DATE;
static char const * const BUILD_TIME = _EXECUTABLE_BUILD_TIME;
static char const * const BUILD_GIT_HASH = _EXECUTABLE_BUILD_GIT_HASH;
static char const * const BUILD_GIT_HASH_SHORT = _EXECUTABLE_BUILD_GIT_HASH_SHORT;
static char const * const BUILD_GIT_BRANCH = _EXECUTABLE_BUILD_GIT_BRANCH;

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

void version_get( version_t * version )
{
    version->product_name       = PRODUCT_NAME;
    version->executable_name    = EXECUTABLE_NAME;
    version->version            = VERSION;
    version->version_major      = PRODUCT_VERSION_MAJOR;
    version->version_minor      = PRODUCT_VERSION_MINOR;
    version->version_revision   = PRODUCT_VERSION_REVISION;
    version->build_type         = BUILD_TYPE;
    version->build_dir          = BUILD_DIR;
    version->build_date         = BUILD_DATE;
    version->build_time         = BUILD_TIME;
    version->git_hash           = BUILD_GIT_HASH;
    version->git_hash_short     = BUILD_GIT_HASH_SHORT;
    version->git_branch         = BUILD_GIT_BRANCH;

}   /* version_get() */
