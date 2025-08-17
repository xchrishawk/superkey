/**
 * @file    buzzer.h
 * @brief   Implementation for the buzzer driver module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-17
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>

#include <avr/io.h>

#include "buzzer.h"
#include "gpio.h"
#include "led.h"
#include "utility.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

/**
 * @def     DISABLE_BUZZER
 * @brief   Set to `true` to globally disable the buzzer, in order to avoid annoying your spouse.
 */
#define DISABLE_BUZZER      true

/**
 * @def     DEFAULT_FREQUENCY
 * @brief   The default frequency that the buzzer will use.
 */
#define DEFAULT_FREQUENCY   700

/**
 * @def     OCRA_GPIO_PIN
 * @brief   The GPIO pin associated with the OCRA pin connected to the buzzer.
 */
#define OCRA_GPIO_PIN       GPIO_PIN_D5

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

void buzzer_init( void )
{
    // Pin must be configured as an output
    gpio_set_dir( OCRA_GPIO_PIN, GPIO_DIR_OUT );

    // Configure timer
    // - Waveform generation mode = CTC
    // - Clock prescaler = /8
    TCCR1B = bitmask2( WGM12, CS11 );

    // Set default frequency
    buzzer_set_frequency( DEFAULT_FREQUENCY );

}   /* buzzer_init() */


void buzzer_set_buzz( bool buzz )
{
#if !DISABLE_BUZZER
    assign_bit( TCCR1A, COM1A0, buzz );
#else
    clear_bit( TCCR1A, COM1A0 );
#endif

}   /* buzzer_set() */


void buzzer_set_frequency( uint16_t freq )
{
    OCR1A = ( F_CPU / ( 2UL * 8 /* prescaler */ * freq ) ) - 1;

}   /* buzzer_set_frequency() */
