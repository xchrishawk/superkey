/**
 * @file    src/exakey/drivers/gpio.c
 * @brief   Implementation for the low-level GPIO driver module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-17
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include <avr/io.h>

#include "drivers/gpio.h"
#include "utility/debug.h"
#include "utility/types.h"
#include "utility/utility.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

// Helper macro for the register table below
#define gpio_reg( _port, _pin, _pcmsk_reg )                                             \
    { register_addr( DDR ## _port ),                                                    \
      register_addr( PIN ## _port ),                                                    \
      register_addr( PORT ## _port ),                                                   \
      register_addr( PCMSK ## _pcmsk_reg ),                                             \
      _pin }

// This array must be in the same order as the gpio_pin_t enumeration
static struct
{
    register_t  ddr_reg;                    /**< DDR register.                          */
    register_t  in_reg;                     /**< PIN register.                          */
    register_t  out_reg;                    /**< PORT register.                         */
    register_t  pcmsk_reg;                  /**< PCMSK register.                        */
    uint8_t     index;                      /**< Pin index in port.                     */
}
const s_reg_tbl[] =
{
    // Port A
    gpio_reg( A, 0, 0 ),
    gpio_reg( A, 1, 0 ),
    gpio_reg( A, 2, 0 ),
    gpio_reg( A, 3, 0 ),
    gpio_reg( A, 4, 0 ),
    gpio_reg( A, 5, 0 ),
    gpio_reg( A, 6, 0 ),
    gpio_reg( A, 7, 0 ),

    // Port B
    gpio_reg( B, 0, 1 ),
    gpio_reg( B, 1, 1 ),
    gpio_reg( B, 2, 1 ),
    gpio_reg( B, 3, 1 ),
    gpio_reg( B, 4, 1 ),
    gpio_reg( B, 5, 1 ),
    gpio_reg( B, 6, 1 ),
    gpio_reg( B, 7, 1 ),

    // Port C
    gpio_reg( C, 0, 2 ),
    gpio_reg( C, 1, 2 ),
    gpio_reg( C, 2, 2 ),
    gpio_reg( C, 3, 2 ),
    gpio_reg( C, 4, 2 ),
    gpio_reg( C, 5, 2 ),
    gpio_reg( C, 6, 2 ),
    gpio_reg( C, 7, 2 ),

    // Port D
    gpio_reg( D, 0, 3 ),
    gpio_reg( D, 1, 3 ),
    gpio_reg( D, 2, 3 ),
    gpio_reg( D, 3, 3 ),
    gpio_reg( D, 4, 3 ),
    gpio_reg( D, 5, 3 ),
    gpio_reg( D, 6, 3 ),
    gpio_reg( D, 7, 3 ),
};
_Static_assert( array_count( s_reg_tbl ) == GPIO_PIN_COUNT, "Invalid register table!" );

/* ----------------------------------------------------- MACROS ----------------------------------------------------- */

// Validation macros
#define validate_dir( _dir )                                                            \
    assert_always( ( _dir ) < GPIO_DIR_COUNT )
#define validate_pin( _pin )                                                            \
    assert_always( ( _pin ) < GPIO_PIN_COUNT )
#define validate_port( _port )                                                          \
    assert_always( ( _port ) < GPIO_PORT_COUNT )
#define validate_state( _state )                                                        \
    assert_always( ( _state ) < GPIO_STATE_COUNT )

// Helper macros for register table
#define PIN_DDR( _pin )                                                                 \
    ( * s_reg_tbl[ ( _pin ) ].ddr_reg )
#define PIN_IN( _pin )                                                                  \
    ( * s_reg_tbl[ ( _pin ) ].in_reg )
#define PIN_OUT( _pin )                                                                 \
    ( * s_reg_tbl[ ( _pin ) ].out_reg )
#define PIN_PCMSK( _pin )                                                               \
    ( * s_reg_tbl[ ( _pin ) ].pcmsk_reg )
#define PIN_INDEX( _pin )                                                               \
    ( s_reg_tbl[ ( _pin ) ].index )

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

gpio_dir_t gpio_get_dir( gpio_pin_t pin )
{
    validate_pin( pin );

    return( is_bit_set( PIN_DDR( pin ), PIN_INDEX( pin ) ) ? GPIO_DIR_OUT : GPIO_DIR_IN );

}   /* gpio_get_dir() */


gpio_port_t gpio_get_pin_port( gpio_pin_t pin )
{
    validate_pin( pin );

    // GPIO_PIN_PORT_A_FIRST is 0, so that comparison would be redundant
    if( /* pin >= GPIO_PIN_PORT_A_FIRST && */ pin <= GPIO_PIN_PORT_A_LAST )
        return( GPIO_PORT_A );
    else if( pin >= GPIO_PIN_PORT_B_FIRST && pin <= GPIO_PIN_PORT_B_LAST )
        return( GPIO_PORT_B );
    else if( pin >= GPIO_PIN_PORT_C_FIRST && pin <= GPIO_PIN_PORT_C_LAST )
        return( GPIO_PORT_C );
    else if( pin >= GPIO_PIN_PORT_D_FIRST && pin <= GPIO_PIN_PORT_D_LAST )
        return( GPIO_PORT_D );
    else
        fail(); // never executed

}   /* gpio_get_pin_port() */


gpio_state_t gpio_get_state( gpio_pin_t pin )
{
    validate_pin( pin );

    if( gpio_get_dir( pin ) == GPIO_DIR_OUT )
        return( is_bit_set( PIN_OUT( pin ), PIN_INDEX( pin ) ) ? GPIO_STATE_HIGH : GPIO_STATE_LOW );
    else
        return( is_bit_set( PIN_IN( pin ), PIN_INDEX( pin ) ) ? GPIO_STATE_HIGH : GPIO_STATE_LOW );

}   /* gpio_get_state() */


void gpio_set_dir( gpio_pin_t pin, gpio_dir_t dir )
{
    validate_pin( pin );
    validate_dir( dir );

    assign_bit( PIN_DDR( pin ), PIN_INDEX( pin ), dir == GPIO_DIR_OUT );

}   /* gpio_set_dir() */


void gpio_set_pcint_enabled_pin( gpio_pin_t pin, bool enabled )
{
    validate_pin( pin );

    assign_bit( PIN_PCMSK( pin ), PIN_INDEX( pin ), enabled );

}   /* gpio_set_pcint_enabled_pin() */


void gpio_set_pcint_enabled_port( gpio_port_t port, bool enabled )
{
    validate_port( port );

    switch( port )
    {
    case GPIO_PORT_A:
        assign_bit( PCICR, PCIE0, enabled );
        break;
    case GPIO_PORT_B:
        assign_bit( PCICR, PCIE1, enabled );
        break;
    case GPIO_PORT_C:
        assign_bit( PCICR, PCIE2, enabled );
        break;
    case GPIO_PORT_D:
        assign_bit( PCICR, PCIE3, enabled );
        break;
    }

}   /* gpio_set_pcint_enabled_port() */


void gpio_set_pullup( gpio_pin_t pin, bool enabled )
{
    validate_pin( pin );

    assign_bit( PIN_OUT( pin ), PIN_INDEX( pin ), enabled );

}   /* gpio_set_pullup() */


void gpio_set_state( gpio_pin_t pin, gpio_state_t state )
{
    validate_pin( pin );
    validate_state( state );

    assign_bit( PIN_OUT( pin ), PIN_INDEX( pin ), state == GPIO_STATE_HIGH );

}   /* gpio_set_state() */


void gpio_toggle_state( gpio_pin_t pin )
{
    validate_pin( pin );

    toggle_bit( PIN_OUT( pin ), PIN_INDEX( pin ) );

}   /* gpio_toggle_state() */
