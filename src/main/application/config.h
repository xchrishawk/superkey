/**
 * @file    src/main/application/config.h
 * @brief   Header for the application configuration module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-20
 */

#if !defined( APPLICATION_CONFIG_H )
#define APPLICATION_CONFIG_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stdint.h>

#include "application/buzzer.h"
#include "application/input.h"
#include "application/keyer.h"
#include "application/led.h"
#include "application/wpm.h"

/* ----------------------------------------------------- TYPES ------------------------------------------------------ */

/**
 * @struct  config_t
 * @brief   Struct containing the current application configuration information.
 */
typedef struct
{
    /** Global words per minute setting. */
    wpm_t               wpm;

    /** If set to `false`, the buzzer will be disabled and will not sound. */
    bool                buzzer_enabled;

    /** The buzzer frequency, in Hz. */
    buzzer_freq_t       buzzer_frequency;

    /** If set to `false`, the LED will be disabled and will not illuminate. */
    bool                led_enabled[ LED_COUNT ];

    /** The configured input type for each input pin. */
    input_type_t        input_type[ INPUT_PIN_COUNT ];

    /** The configured input polarity for each input pin. */
    input_polarity_t    input_polarity[ INPUT_PIN_COUNT ];

    /** The keyer's paddle mode. */
    keyer_paddle_mode_t keyer_paddle_mode;

    /** If set to `true`, the keyer will emit dashes from the left paddle and dots from the right paddle. */
    bool                keyer_paddle_invert;

    /** If set to `true`, the keyer's output is active low. */
    bool                keyer_output_active_low;

} config_t;

/**
 * @typedef config_version_t
 * @brief   Typedef for the configuration version number.
 */
typedef uint8_t config_version_t;

/* ----------------------------------------------------- MACROS ----------------------------------------------------- */

/**
 * @def     config()
 * @brief   Convenience macro which calls `config_read_only()`.
 */
#define config()                                                                        \
    config_read_only()

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      config_default( config_t * )
 * @brief   Populates the specified configuration struct with valid defaults.
 */
void config_default( config_t * config );

/**
 * @fn      config_flush( void )
 * @brief   Immediately writes the current configuration to storage.
 */
void config_flush( void );

/**
 * @fn      config_get( config_t * )
 * @brief   Copies the current application configuration into the specified struct.
 */
void config_get( config_t * config );

/**
 * @fn      config_init( void )
 * @brief   Initializes the system's configuration.
 */
void config_init( void );

/**
 * @fn      config_read_only( void )
 * @brief   Returns a read-only pointer to the current application configuration.
 */
config_t const * config_read_only( void );

/**
 * @fn      config_set( config_t const * )
 * @brief   Updates the current application configuration with the specified struct.
 * @returns `true` if the configuration was successfully saved, and `false` if saving the configuration failed for any
 *          reason (for example, if any of the configuration parameters are invalid).
 */
bool config_set( config_t const * config );

/**
 * @fn      config_tick( tick_t )
 * @brief   Performs periodic processing at the specified tick.
 */
void config_tick( tick_t tick );

#endif /* !defined( APPLICATION_CONFIG_H ) */