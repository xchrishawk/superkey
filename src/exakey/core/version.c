/**
 * @file    src/exakey/core/version.c
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

// Manually maintained
#define VERSION_MAJOR    0
#define VERSION_MINOR    1
#define VERSION_REVISION 0

// Auto-generated variables
char const * const EXAKEY_NAME = EXECUTABLE_NAME;
char const * const EXAKEY_PRODUCT_NAME = EXECUTABLE_PROJECT_NAME;
char const * const EXAKEY_VERSION = stringize_value( VERSION_MAJOR ) "."
                                    stringize_value( VERSION_MINOR ) "."
                                    stringize_value( VERSION_REVISION );
uint8_t const EXAKEY_VERSION_MAJOR = VERSION_MAJOR;
uint8_t const EXAKEY_VERSION_MINOR = VERSION_MINOR;
uint8_t const EXAKEY_VERSION_REVISION = VERSION_REVISION;
char const * const EXAKEY_BUILD_DIR = EXECUTABLE_PROJECT_BUILD_DIR;
char const * const EXAKEY_BUILD_DATE = EXECUTABLE_BUILD_DATE;
char const * const EXAKEY_BUILD_TIME = EXECUTABLE_BUILD_TIME;
char const * const EXAKEY_BUILD_GIT_HASH = EXECUTABLE_BUILD_GIT_HASH;
char const * const EXAKEY_BUILD_GIT_HASH_SHORT = EXECUTABLE_BUILD_GIT_HASH_SHORT;
char const * const EXAKEY_BUILD_GIT_BRANCH = EXECUTABLE_BUILD_GIT_BRANCH;

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

void version_get( version_t * version )
{
    version->product_name       = EXAKEY_PRODUCT_NAME;
    version->executable_name    = EXAKEY_NAME;
    version->version            = EXAKEY_VERSION;
    version->version_major      = EXAKEY_VERSION_MAJOR;
    version->version_minor      = EXAKEY_VERSION_MINOR;
    version->version_revision   = EXAKEY_VERSION_REVISION;
    version->build_dir          = EXAKEY_BUILD_DIR;
    version->build_date         = EXAKEY_BUILD_DATE;
    version->build_time         = EXAKEY_BUILD_TIME;
    version->git_hash           = EXAKEY_BUILD_GIT_HASH;
    version->git_hash_short     = EXAKEY_BUILD_GIT_HASH_SHORT;
    version->git_branch         = EXAKEY_BUILD_GIT_BRANCH;

}   /* version_get() */
