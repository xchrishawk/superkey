/**
 * @file    src/main/core/config.c
 * @brief   Implementation for the application configuration module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-20
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "application/config.h"
#include "application/led.h"
#include "utility/debug.h"
#include "utility/utility.h"

/* ----------------------------------------------------- MACROS ----------------------------------------------------- */

_Static_assert( _CONFIG_DFLT_WPM >= WPM_MINIMUM &&
                _CONFIG_DFLT_WPM <= WPM_MAXIMUM,
                "Invalid default WPM!" );
_Static_assert( _CONFIG_DFLT_BUZZER_FREQUENCY >= BUZZER_MINIMUM_FREQUENCY &&
                _CONFIG_DFLT_BUZZER_FREQUENCY <= BUZZER_MAXIMUM_FREQUENCY,
                "Invalid default buzzer frequency!" );

/* --------------------------------------------------- VARIABLES ---------------------------------------------------- */

static config_t s_config;                   /**< Currently active app configuration.    */

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

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
    config->wpm                             = _CONFIG_DFLT_WPM;

    // Buzzer configuration
    config->buzzer_enabled                  = _CONFIG_DFLT_BUZZER_ENABLED;
    config->buzzer_frequency                = _CONFIG_DFLT_BUZZER_FREQUENCY;

    // LED configuration
    config->led_enabled[ LED_STATUS ]       = _CONFIG_DFLT_LED_STATUS_ENABLED;
    config->led_active_lo[ LED_STATUS ]     = _CONFIG_DFLT_LED_STATUS_ACTIVE_LO;
    config->led_enabled[ LED_KEY ]          = _CONFIG_DFLT_LED_KEY_ENABLED;
    config->led_active_lo[ LED_KEY ]        = _CONFIG_DFLT_LED_KEY_ACTIVE_LO;

    // Ensure we generated a valid configuration
    assert_always( validate_config( config ) );

}   /* config_default() */


void config_get( config_t * config )
{
    memcpy( config, & s_config, sizeof( config_t ) );

}   /* config_get() */


void config_init( void )
{
    // Set configuration to defaults, for now
    config_default( & s_config );

}   /* config_init() */


config_t const * config_read_only( void )
{
    return( ( config_t const * ) & s_config );

}   /* config_read_only() */


bool config_set( config_t const * config )
{
    if( ! validate_config( config ) )
        return( false );

    memcpy( & s_config, config, sizeof( config_t ) );
    return( true );

}   /* config_set() */


static bool validate_config( config_t const * config )
{
    if( config->wpm < WPM_MINIMUM ||
        config->wpm > WPM_MAXIMUM ||
        config->buzzer_frequency < BUZZER_MINIMUM_FREQUENCY ||
        config->buzzer_frequency > BUZZER_MAXIMUM_FREQUENCY )
        return( false );

    return( true );

}   /* validate_config() */
