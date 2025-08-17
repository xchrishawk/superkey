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

#define DEFAULT_FREQUENCY   700
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
    assign_bit( TCCR1A, COM1A0, buzz );

}   /* buzzer_set() */


void buzzer_set_frequency( uint16_t freq )
{
    OCR1A = ( F_CPU / ( 2UL * 8 /* prescaler */ * freq ) ) - 1;

}   /* buzzer_set_frequency() */
