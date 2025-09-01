/**
 * @brief   src/main/application/io.h
 * @brief   Header for the keyer input / output module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-29
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 *
 * @details
 * This module is responsible for handling the device's I/O pins (i.e., the TRS connectors). Each pin may be
 * individually configured as an input or an output with a specific type. This allows abstracting the details of the
 * GPIO interface from client modules (like the keyer).
 */

#if !defined( APPLICATION_IO_H )
#define APPLICATION_IO_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stdint.h>

#include "utility/types.h"

/* ----------------------------------------------------- TYPES ------------------------------------------------------ */

/**
 * @typedef io_pin_t
 * @brief   Enumeration of the supported I/O pins.
 */
typedef uint8_t io_pin_t;
enum
{
    IO_PIN_TRS_0_TIP,                       /**< The tip pin for TRS connector 0.       */
    IO_PIN_TRS_0_RING,                      /**< The ring pin for TRS connector 0.      */
    IO_PIN_TRS_1_TIP,                       /**< The tip pin for TRS connector 1.       */
    IO_PIN_TRS_1_RING,                      /**< The ring pin for TRS connector 1.      */
    IO_PIN_TRS_2_TIP,                       /**< The tip pin for TRS connector 2.       */
    IO_PIN_TRS_2_RING,                      /**< The ring pin for TRS connector 2.      */
    IO_PIN_TRS_3_TIP,                       /**< The tip pin for TRS connector 3.       */
    IO_PIN_TRS_3_RING,                      /**< The ring pin for TRS connector 3.      */

    IO_PIN_COUNT,                           /**< Number of valid I/O pins.              */
};

/**
 * @typedef io_polarity_t
 * @brief   Enumeration of the supported I/O polarities.
 */
typedef uint8_t io_polarity_t;
enum
{
    IO_POLARITY_ACTIVE_LOW,                 /**< I/O is "on" when grounded.             */
    IO_POLARITY_ACTIVE_HIGH,                /**< I/O is "on" when at Vcc.               */

    IO_POLARITY_COUNT,                      /**< Number of valid I/O polarities.        */
};

/**
 * @typedef io_state_t
 * @brief   Enumeration of the states that I/O pins may be in.
 */
typedef uint8_t io_state_t;
enum
{
    IO_STATE_OFF,                           /**< I/O is "off" (inactive).               */
    IO_STATE_ON,                            /**< I/O is "on" (active).                  */

    IO_STATE_COUNT,                         /**< Number of valid I/O states.            */

    IO_STATE_NONE                           /**< I/O has no state (not configured).     */
        = IO_STATE_COUNT,
};

/**
 * @typedef io_type_t
 * @brief   Enumeration of the supported I/O types.
 */
typedef uint8_t io_type_t;
enum
{
    IO_TYPE_INPUT_STRAIGHT_KEY,             /**< I/O is a straight key input.           */
    IO_TYPE_INPUT_PADDLE_LEFT,              /**< I/O is a left paddle input.            */
    IO_TYPE_INPUT_PADDLE_RIGHT,             /**< I/O is a right paddle input.           */

    IO_TYPE_OUTPUT_KEYER,                   /**< I/O is the keyer output to the radio.  */

    IO_TYPE_COUNT,                          /**< Number of valid I/O types.             */

    IO_TYPE_NONE                            /**< Pin is not configured.                 */
        = IO_TYPE_COUNT,

    IO_TYPE_INPUT_FIRST                     /**< First input type.                      */
        = IO_TYPE_INPUT_STRAIGHT_KEY,
    IO_TYPE_INPUT_LAST                      /**< Last input type.                       */
        = IO_TYPE_INPUT_PADDLE_RIGHT,

    IO_TYPE_OUTPUT_FIRST                    /**< First output type.                     */
        = IO_TYPE_OUTPUT_KEYER,
    IO_TYPE_OUTPUT_LAST                     /**< Last output type.                      */
        = IO_TYPE_OUTPUT_KEYER,
};

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      io_get_polarity( io_pin_t )
 * @brief   Returns the polarity of the specified I/O pin.
 */
io_polarity_t io_get_polarity( io_pin_t pin );

/**
 * @fn      io_get_state( io_pin_t )
 * @brief   Returns the state of the specified I/O pin.
 */
io_state_t io_get_state( io_pin_t pin );

/**
 * @fn      io_get_state( io_type_t )
 * @brief   Returns the state of the specified I/O type.
 * @note    This will return `IO_STATE_ON` if *any* pin with this input type is currently `IO_STATE_ON`.
 */
io_state_t io_get_state_type( io_type_t type );

/**
 * @fn      io_get_type( io_pin_t )
 * @brief   Returns the I/O type of the specified I/O pin.
 */
io_type_t io_get_type( io_pin_t pin );

/**
 * @fn      io_init( void )
 * @brief   Initializes the keyer input / output module.
 * @note    Requires that the following modules have been initialized:
 *          - `config`
 *          - `sys`
 */
void io_init( void );

/**
 * @fn      io_pin_is_input( io_pin_t )
 * @brief   Returns `true` if the specified `io_pin_t` is configured as an input.
 */
bool io_pin_is_input( io_pin_t pin );

/**
 * @fn      io_pin_is_output( io_pin_t )
 * @brief   Returns `true` if the specified `io_pin_t` is configured as an output.
 */
bool io_pin_is_output( io_pin_t pin );

/**
 * @fn      io_set_output_state( io_pin_t, io_state_t )
 * @brief   Sets the state of the specified I/O pin.
 * @note    Has no effect if the specified pin is not configured as an output.
 */
void io_set_output_state( io_pin_t pin, io_state_t state );

/**
 * @fn      io_set_output_state_type( io_type_t, io_state_t )
 * @brief   Sets the state of the specified I/O type.
 * @note    Has no effect if the specified I/O type is not an output.
 */
void io_set_output_state_type( io_type_t type, io_state_t state );

/**
 * @fn      io_set_polarity( io_pin_t, io_polarity_t )
 * @brief   Sets the polarity of the specified I/O pin.
 * @note    This modifies the application configuration.
 */
void io_set_polarity( io_pin_t pin, io_polarity_t polarity );

/**
 * @fn      io_set_type( io_pin_t, io_type_t )
 * @brief   Sets the I/O type of the specified I/O pin.
 * @note    This modifies the application configuration.
 */
void io_set_type( io_pin_t pin, io_type_t type );

/**
 * @fn      io_tick( tick_t )
 * @brief   Performs periodic processing at the specified tick.
 */
void io_tick( tick_t tick );

/**
 * @fn      io_type_is_input( io_type_t )
 * @brief   Returns `true` if the specified `io_type_t` represents an input.
 */
bool io_type_is_input( io_type_t type );

/**
 * @fn      io_type_is_output( io_type_t )
 * @brief   Returns `true` if the specified `io_type_t` reprsents an output.
 */
bool io_type_is_output( io_type_t type );

#endif /* !defined( APPLICATION_IO_H ) */
