/**
 * @file    src/exakey/application/buzzer.c
 * @brief   Implementation for the buzzer driver module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-17
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>

#include <avr/io.h>

#include "application/buzzer.h"
#include "application/led.h"
#include "drivers/gpio.h"
#include "utility/utility.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

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


void buzzer_set_frequency( uint16_t freq )
{
    // thx chatgpt
    OCR1A = ( F_CPU / ( 2UL * 8 /* prescaler */ * freq ) ) - 1;

}   /* buzzer_set_frequency() */


void buzzer_set_on( bool on )
{
#if defined( DISABLE_BUZZER ) && DISABLE_BUZZER
    clear_bit( TCCR1A, COM1A0 );
#else
    assign_bit( TCCR1A, COM1A0, on );
#endif

}   /* buzzer_set_on() */


void buzzer_tick( tick_t tick )
{

}   /* buzzer_tick() */
