/**
 * @file    src/main/application/buzzer.c
 * @brief   Implementation for the buzzer driver module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-17
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stdlib.h>

#include <avr/io.h>

#include "application/buzzer.h"
#include "application/config.h"
#include "application/led.h"
#include "drivers/gpio.h"
#include "utility/utility.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

/**
 * @def     OCRA_GPIO_PIN
 * @brief   The GPIO pin associated with the OCRA pin connected to the buzzer.
 */
#define OCRA_GPIO_PIN       GPIO_PIN_D5

/* --------------------------------------------------- VARIABLES ---------------------------------------------------- */

bool s_on = false;                          /**< Is the buzzer currently requested on?  */

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      update_hardware( void )
 * @brief   Updates the buzzer hardware based on current state and configuration.
 */
static void update_hardware( void );

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

bool buzzer_get_enabled( void )
{
    return( config()->buzzer_enabled );

}   /* buzzer_get_enabled() */


buzzer_freq_t buzzer_get_frequency( void )
{
    return( config()->buzzer_frequency );

}   /* buzzer_get_frequency() */


bool buzzer_get_on( void )
{
    return( s_on );

}   /* buzzer_get_on() */


void buzzer_init( void )
{
    // Update local state
    s_on = false;

    // Configure timer
    // - Waveform generation mode = CTC
    // - Clock prescaler = /8
    // - OCRA pin must be configured as an output
    TCCR1B = bitmask2( WGM12, CS11 );
    gpio_set_dir( OCRA_GPIO_PIN, GPIO_DIR_OUT );

    // Initialize hardware
    update_hardware();

}   /* buzzer_init() */


void buzzer_set_enabled( bool enabled )
{
    config_t config;
    config_get( & config );
    config.buzzer_enabled = enabled;
    config_set( & config );

    update_hardware();

}   /* buzzer_set_enabled() */


void buzzer_set_frequency( buzzer_freq_t freq )
{
    freq = clamp( freq, BUZZER_MINIMUM_FREQUENCY, BUZZER_MAXIMUM_FREQUENCY );

    config_t config;
    config_get( & config );
    config.buzzer_frequency = freq;
    config_set( & config );

    update_hardware();

}   /* buzzer_set_frequency() */


void buzzer_set_on( bool on )
{
    s_on = on;

    update_hardware();

}   /* buzzer_set_on() */


void buzzer_tick( tick_t tick )
{
    ( void )tick;
    update_hardware();

}   /* buzzer_tick() */


static void update_hardware( void )
{
    bool enabled = config()->buzzer_enabled;
    uint16_t freq = config()->buzzer_frequency;

    // Update OCR1A register to set frequency
    OCR1A = ( F_CPU / ( 2UL * 8 /* prescaler */ * freq ) ) - 1;

    // If the buzzer is requested to be on, and the buzzer is enabled, then start the timer
    assign_bit( TCCR1A, COM1A0, s_on && enabled );

}   /* update_hardware() */
