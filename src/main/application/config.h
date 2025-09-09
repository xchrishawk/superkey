/**
 * @file    src/main/application/config.h
 * @brief   Header for the application configuration module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-20
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 *
 * @details
 * This module is responsible for handling the application's configuration. Getters and setters for individual
 * configuration items are not provided - instead, client modules should implement these, if required.
 */

#if !defined( APPLICATION_CONFIG_H )
#define APPLICATION_CONFIG_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stdint.h>

#include "application/buzzer.h"
#include "application/io.h"
#include "application/keyer.h"
#include "application/led.h"
#include "application/wpm.h"
#include "utility/utility.h"

/* ----------------------------------------------------- TYPES ------------------------------------------------------ */

/**
 * @struct  config_t
 * @brief   Struct containing the current application configuration information.
 */
typedef struct
{
    /** Global words per minute setting. */
    wpm_t               wpm;

    /** Scale factor for each Morse code element. */
    wpm_element_scale_t wpm_element_scale[ WPM_ELEMENT_COUNT ];

    /** If set to `false`, the buzzer will be disabled and will not sound. */
    bool                buzzer_enabled;

    /** The buzzer frequency, in Hz. */
    buzzer_freq_t       buzzer_frequency;

    /** If set to `false`, the LED will be disabled and will not illuminate. */
    bool                led_enabled[ LED_COUNT ];

    /** The configured I/O type for each I/O pin. */
    io_type_t           io_type[ IO_PIN_COUNT ];

    /** The configured I/O polarity for each I/O pin. */
    io_polarity_t       io_polarity[ IO_PIN_COUNT ];

    /** The keyer's paddle mode. */
    keyer_paddle_mode_t keyer_paddle_mode;

    /** If set to `true`, the keyer will emit dashes from the left paddle and dots from the right paddle. */
    bool                keyer_paddle_invert;

} PACKED_STRUCT config_t;

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
 * @note    Requires that the following modules have been initialized:
 *          - `storage`
 *          - `sys`
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