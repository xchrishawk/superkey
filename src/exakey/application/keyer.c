/**
 * @file    src/exakey/application/keyer.c
 * @brief   Implementation for the keyer module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-19
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stdint.h>

#include "application/buzzer.h"
#include "application/input.h"
#include "application/keyer.h"
#include "application/led.h"
#include "drivers/gpio.h"
#include "utility/debug.h"
#include "utility/types.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

static gpio_pin_t const KEYER_OUT_PIN = GPIO_PIN_A3;
static bool const KEYER_OUT_ACTIVE_HIGH = false;

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      set_keyed( bool )
 * @brief   Immediately sets whether the keyer is active or not.
 */
static void set_keyed( bool keyed );

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

void keyer_init( void )
{
    // Initialize GPIO
    gpio_set_dir( KEYER_OUT_PIN, GPIO_DIR_OUT );

    // Default to key off
    set_keyed( false );

}   /* keyer_init() */


void keyer_panic( void )
{
    set_keyed( false );

}   /* keyer_panic() */


void keyer_tick( tick_t tick )
{
    ( void )tick;

    bool keyed = ( input_get( INPUT_STRAIGHT_KEY ) ||
                   input_get( INPUT_PADDLE_A ) ||
                   input_get( INPUT_PADDLE_B ) );

    set_keyed( keyed );

}   /* keyer_tick() */


static void set_keyed( bool keyed )
{
    // Update GPIO
    gpio_set_state( KEYER_OUT_PIN,
                    keyed ?
                        ( KEYER_OUT_ACTIVE_HIGH ? GPIO_STATE_HIGH : GPIO_STATE_LOW ) :
                        ( KEYER_OUT_ACTIVE_HIGH ? GPIO_STATE_LOW : GPIO_STATE_HIGH ) );

    // Update LED
    led_set_on( LED_KEY, keyed );

    // Update buzzer
    buzzer_set_on( keyed );

}   /* set_keyed() */
