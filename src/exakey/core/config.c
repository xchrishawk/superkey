/**
 * @file    src/exakey/core/config.c
 * @brief   Implementation for the system configuration module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-20
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "application/led.h"
#include "core/config.h"
#include "utility/debug.h"
#include "utility/utility.h"

/* --------------------------------------------------- VARIABLES ---------------------------------------------------- */

static config_t s_config;                   /**< Currently active system configuration. */

// Ensure array sizes are correct
_Static_assert( array_count( s_config.led_enabled ) == LED_COUNT &&
                array_count( s_config.led_active_lo ) == LED_COUNT,
                "Incorrect number of LEDs!" );

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      default_config( config_t * )
 * @brief   Assigns valid default configuration values to the specified configuration struct.
 */
static void default_config( config_t * config );

/**
 * @fn      validate_config( config_t const * )
 * @brief   Returns `true` if all fields in the specified configuration struct are valid.
 */
static bool validate_config( config_t const * config );

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

void config_get( config_t * config )
{
    memcpy( config, & s_config, sizeof( config_t ) );

}   /* config_get() */


void config_init( void )
{
    // Set configuration to defaults, for now
    default_config( & s_config );

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


static void default_config( config_t * config )
{
    // Default everything to zero
    memset( config, 0, sizeof( config_t ) );

    // Buzzer configuration
    config->buzzer_enabled                  = EXAKEY_DFLT_BUZZER_ENABLED;
    config->buzzer_freq                     = EXAKEY_DFLT_BUZZER_FREQ;

    // LED configuration
    config->led_enabled[ LED_STATUS ]       = EXAKEY_DFLT_LED_STATUS_ENABLED;
    config->led_active_lo[ LED_STATUS ]     = EXAKEY_DFLT_LED_STATUS_ACTIVE_LO;
    config->led_enabled[ LED_KEY ]          = EXAKEY_DFLT_LED_KEY_ENABLED;
    config->led_active_lo[ LED_KEY ]        = EXAKEY_DFLT_LED_KEY_ACTIVE_LO;

    // Ensure we generated a valid configuration
    assert_always( validate_config( config ) );

}   /* default_config() */


static bool validate_config( config_t const * config )
{
    ( void )config;

    return( true );

}   /* validate_config() */
