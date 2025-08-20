/**
 * @file    src/exakey/core/config.h
 * @brief   Header for the system configuration module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-20
 */

#if !defined( EXAKEY_CONFIG_H )
#define EXAKEY_CONFIG_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stdint.h>

/* ----------------------------------------------------- TYPES ------------------------------------------------------ */

// These array counts are defined as local constants to avoid having to include application-specific headers.
// They are checked against the actual value using static asserts in config.c.
#define CONFIG_LED_COUNT        ( 2 )

/**
 * @typedef config_t
 * @brief   Struct containing the current system configuration information.
 */
typedef struct
{
    /** If set to `false`, the buzzer will be disabled and will not sound. */
    bool        buzzer_enabled;

    /** The buzzer frequency, in Hz. */
    uint16_t    buzzer_freq;

    /** If set to `false`, the LED will be disabled and will not illuminate. */
    bool        led_enabled[ CONFIG_LED_COUNT ];

    /** If set to `true`, the LED is illuminated by making the GPIO pin low (not high). */
    bool        led_active_lo[ CONFIG_LED_COUNT ];

} config_t;

// Undefine utility macros that we don't want anyone else accidentally using
#undef CONFIG_LED_COUNT

/* ----------------------------------------------------- MACROS ----------------------------------------------------- */

/**
 * @def     config()
 * @brief   Convenience macro which calls `config_read_only()`.
 */
#define config()                                                                        \
    config_read_only()

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      config_get( config_t * )
 * @brief   Copies the current system configuration into the specified struct.
 */
void config_get( config_t * config );

/**
 * @fn      config_init( void )
 * @brief   Initializes the system's configuration.
 */
void config_init( void );

/**
 * @fn      config_read_only( void )
 * @brief   Returns a read-only pointer to the current system configuration.
 */
config_t const * config_read_only( void );

/**
 * @fn      config_set( config_t const * )
 * @brief   Updates the current system configuration with the specified struct.
 * @returns `true` if the configuration was successfully saved, and `false` if saving the configuration failed for any
 *          reason (for example, if any of the configuration parameters are invalid).
 */
bool config_set( config_t const * config );

#endif /* !defined( EXAKEY_CONFIG_H ) */