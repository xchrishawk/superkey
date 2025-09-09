/**
 * @file    src/main/application/config.c
 * @brief   Implementation for the application configuration module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-20
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "application/config.h"
#include "application/keyer.h"
#include "application/led.h"
#include "application/storage.h"
#include "core/sys.h"
#include "utility/debug.h"
#include "utility/types.h"
#include "utility/utility.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

/**
 * @def     MINIMUM_SAVE_PERIOD
 * @brief   Minimum elapsed time between saving config to storage.
 */
#define MINIMUM_SAVE_PERIOD     ( 5 * TICKS_PER_SEC )

/* ----------------------------------------------------- TYPES ------------------------------------------------------ */

/**
 * @typedef config_version_t
 * @brief   Enumeration of the supported configuration versions.
 * @note    This is to support backwards compatibility with saved configuration when the software changes.
 */
typedef uint8_t config_version_t;
enum
{
    CONFIG_VERSION_0,                       /**< Configuration version 0.               */

    CONFIG_VERSION_COUNT,                   /**< Number of valid configuration versions.*/

    CONFIG_VERSION_CURRENT                  /**< Current configuration version.         */
        = CONFIG_VERSION_0
};

/**
 * @struct  config_storage_t
 * @brief   Struct defining data saved in storage for unit configuration.
 */
typedef struct
{
    config_version_t version;               /**< Saved version number.                  */
    union
    {
        config_t current;                   /**< Configuration data as current value.   */
    };

} config_storage_t;

/* ----------------------------------------------------- MACROS ----------------------------------------------------- */

_Static_assert( _CONFIG_DFLT_BUZZER_FREQUENCY >= BUZZER_MINIMUM_FREQUENCY &&
                _CONFIG_DFLT_BUZZER_FREQUENCY <= BUZZER_MAXIMUM_FREQUENCY,
                "Invalid default buzzer frequency!" );

_Static_assert( STORAGE_CONFIG_SIZE >= sizeof( config_storage_t ),
                "Not enough storage allocated for configuration!" );

/* --------------------------------------------------- VARIABLES ---------------------------------------------------- */

static config_t s_config;                   /**< Currently active app configuration.    */
static bool s_modified = false;             /**< Has the configuration been modified?   */
static tick_t s_save_tick = 0;              /**< Tick config was last saved.            */

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      flush( tick_t )
 * @brief   Writes the configuration to non-volatile storage and updates state.
 */
static void flush( tick_t tick );

/**
 * @fn      validate_config( config_t const * )
 * @brief   Returns `true` if all fields in the specified configuration struct are valid.
 */
static bool validate_config( config_t const * config );

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

void config_default( config_t * config )
{
    // Default everything to zero
    memset( config, 0, sizeof( config_t ) );

    // Global settings
    config->wpm                                     = _CONFIG_DFLT_WPM;
    for( wpm_element_t el = 0; el < WPM_ELEMENT_COUNT; el++ )
        config->wpm_element_scale[ el ]             = _CONFIG_DFLT_WPM_ELEMENT_SCALE;

    // Buzzer configuration
    config->buzzer_enabled                          = _CONFIG_DFLT_BUZZER_ENABLED;
    config->buzzer_frequency                        = _CONFIG_DFLT_BUZZER_FREQUENCY;

    // LED configuration
    config->led_enabled[ LED_STATUS ]               = _CONFIG_DFLT_LED_STATUS_ENABLED;
    config->led_enabled[ LED_KEY ]                  = _CONFIG_DFLT_LED_KEY_ENABLED;

    // I/O configuration
    // TODO: Compile-time constants
    config->io_type[        IO_PIN_TRS_0_TIP    ]   = _CONFIG_DFLT_IO_TYPE_TRS_0_TIP;
    config->io_polarity[    IO_PIN_TRS_0_TIP    ]   = _CONFIG_DFLT_IO_POLARITY_TRS_0_TIP;
    config->io_type[        IO_PIN_TRS_0_RING   ]   = _CONFIG_DFLT_IO_TYPE_TRS_0_RING;
    config->io_polarity[    IO_PIN_TRS_0_RING   ]   = _CONFIG_DFLT_IO_POLARITY_TRS_0_RING;
    config->io_type[        IO_PIN_TRS_1_TIP    ]   = _CONFIG_DFLT_IO_TYPE_TRS_1_TIP;
    config->io_polarity[    IO_PIN_TRS_1_TIP    ]   = _CONFIG_DFLT_IO_POLARITY_TRS_1_TIP;
    config->io_type[        IO_PIN_TRS_1_RING   ]   = _CONFIG_DFLT_IO_TYPE_TRS_1_RING;
    config->io_polarity[    IO_PIN_TRS_1_RING   ]   = _CONFIG_DFLT_IO_POLARITY_TRS_1_RING;
    config->io_type[        IO_PIN_TRS_2_TIP    ]   = _CONFIG_DFLT_IO_TYPE_TRS_2_TIP;
    config->io_polarity[    IO_PIN_TRS_2_TIP    ]   = _CONFIG_DFLT_IO_POLARITY_TRS_2_TIP;
    config->io_type[        IO_PIN_TRS_2_RING   ]   = _CONFIG_DFLT_IO_TYPE_TRS_2_RING;
    config->io_polarity[    IO_PIN_TRS_2_RING   ]   = _CONFIG_DFLT_IO_POLARITY_TRS_2_RING;
    config->io_type[        IO_PIN_TRS_3_TIP    ]   = _CONFIG_DFLT_IO_TYPE_TRS_3_TIP;
    config->io_polarity[    IO_PIN_TRS_3_TIP    ]   = _CONFIG_DFLT_IO_POLARITY_TRS_3_TIP;
    config->io_type[        IO_PIN_TRS_3_RING   ]   = _CONFIG_DFLT_IO_TYPE_TRS_3_RING;
    config->io_polarity[    IO_PIN_TRS_3_RING   ]   = _CONFIG_DFLT_IO_POLARITY_TRS_3_RING;

    // Keyer configuration
    config->keyer_paddle_mode                       = _CONFIG_DFLT_KEYER_PADDLE_MODE;
    config->keyer_paddle_invert                     = _CONFIG_DFLT_KEYER_PADDLE_INVERT;

    // Ensure we generated a valid configuration
    assert_always( validate_config( config ) );

}   /* config_default() */


void config_flush( void )
{
    flush( sys_get_tick() );

}   /* config_flush() */


void config_get( config_t * config )
{
    * config = s_config;

}   /* config_get() */


void config_init( void )
{
    // Try to get configuration from storage, and restore defaults if that fails.
    // In this case, any existing configuration will be lost the next time config is saved.
    // In the future, this could do migration from previous configuration versions.
    config_storage_t storage;
    if( ! storage_get_config( & storage, sizeof( storage ) ) ||
        storage.version != CONFIG_VERSION_CURRENT ||
        ! validate_config( & storage.current ) )
    {
        config_default( & storage.current );
    }

    // Set local copy
    s_config = storage.current;

}   /* config_init() */


config_t const * config_read_only( void )
{
    return( ( config_t const * ) & s_config );

}   /* config_read_only() */


bool config_set( config_t const * config )
{
    if( ! validate_config( config ) )
        return( false );

    s_config = * config;
    s_modified = true;

    flush( sys_get_tick() );

    return( true );

}   /* config_set() */


void config_tick( tick_t tick )
{
    if( ! s_modified || sys_elapsed( tick, s_save_tick ) < MINIMUM_SAVE_PERIOD )
        return;
    flush( tick );

}   /* config_tick() */


static void flush( tick_t tick )
{
    config_storage_t storage;
    storage.version = CONFIG_VERSION_CURRENT;
    storage.current = s_config;

    storage_set_config( & storage, sizeof( storage ) );
    s_modified = false;
    s_save_tick = tick;

}   /* flush() */


static bool validate_config( config_t const * config )
{
    if( config->wpm < WPM_MINIMUM ||
        config->wpm > WPM_MAXIMUM )
        return false;

    for( wpm_element_t el = 0; el < WPM_ELEMENT_COUNT; el++ )
        if( config->wpm_element_scale[ el ] < WPM_ELEMENT_SCALE_MINIMUM ||
            config->wpm_element_scale[ el ] > WPM_ELEMENT_SCALE_MAXIMUM )
            return( false );

    if( config->buzzer_frequency < BUZZER_MINIMUM_FREQUENCY ||
        config->buzzer_frequency > BUZZER_MAXIMUM_FREQUENCY )
        return( false );

    return( true );

}   /* validate_config() */
