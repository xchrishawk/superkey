/**
 * @file    src/main/application/input.h
 * @brief   Header for the key input module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-17
 */

#if !defined( APPLICATION_INPUT_H )
#define APPLICATION_INPUT_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stdint.h>

#include "utility/types.h"
#include "utility/utility.h"

/* ----------------------------------------------------- TYPES ------------------------------------------------------ */

/**
 * @typedef input_pin_t
 * @brief   Enumeration of the supported input pins.
 * @note    The device has three TRS connectors for inputs. The sleeve of each connector is connected to ground, which
 *          means that we have two inputs for each connector, for a total of 6 pins. Hopefully this will be enough for
 *          most users.
 */
typedef uint8_t input_pin_t;
enum
{
    INPUT_PIN_TRS_0_TIP,                    /**< The tip on TRS connector 0.            */
    INPUT_PIN_TRS_0_RING,                   /**< The ring on TRS connector 0.           */
    INPUT_PIN_TRS_1_TIP,                    /**< The tip on TRS connector 1.            */
    INPUT_PIN_TRS_1_RING,                   /**< The ring on TRS connector 1.           */
    INPUT_PIN_TRS_2_TIP,                    /**< The tip on TRS connector 2.            */
    INPUT_PIN_TRS_2_RING,                   /**< The ring on TRS connector 2.           */

    INPUT_PIN_COUNT,                        /**< Number of valid input pins.            */
};

/**
 * @typedef input_polarity_t
 * @brief   Enumeration of the supported input polarities.
 * @note    This represents whether the input is considered "active" when it is grounded or when it is high.
 */
typedef uint8_t input_polarity_t;
enum
{
    INPUT_POLARITY_ACTIVE_LOW,              /**< The input is active when grounded.     */
    INPUT_POLARITY_ACTIVE_HIGH,             /**< The input is active when at Vcc.       */

    INPUT_POLARITY_COUNT,                   /**< Number of valid input polarities.      */
};

/**
 * @typedef input_type_tt
 * @brief   Enumeration of the supported input types.
 */
typedef uint8_t input_type_t;
enum
{
    INPUT_TYPE_STRAIGHT_KEY,                /**< Input is for a straight key.           */
    INPUT_TYPE_PADDLE_LEFT,                 /**< Input is for the left key on paddles.  */
    INPUT_TYPE_PADDLE_RIGHT,                /**< Input is for the right key on paddles. */

    INPUT_TYPE_COUNT,                       /**< Number of valid input types.           */

    INPUT_TYPE_NONE                         /**< Unused input type.                     */
        = INPUT_TYPE_COUNT
};

/**
 * @typedef input_type_field_t
 * @brief   Bitfield of input types.
 * @note    Bit indices are specified by the `input_type_t` enumeration.
 */
typedef uint8_t input_type_field_t;

_Static_assert( INPUT_TYPE_COUNT < sizeof_bits( input_type_field_t ), "Input type field is too small!" );

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      input_get_on( input_type_t )
 * @brief   Returns the state of the specified input pin.
 */
bool input_get_on( input_pin_t pin );

/**
 * @fn      input_get_polarity( input_pin_t )
 * @brief   Gets the input polarity for the specified input pin.
 */
input_polarity_t input_get_polarity( input_pin_t pin );

/**
 * @fn      input_get_type( input_pin_t )
 * @brief   Gets the input type for the specified input pin.
 */
input_type_t input_get_type( input_pin_t pin );

/**
 * @fn      input_init( void )
 * @brief   Initializes the key input module.
 */
void input_init( void );

/**
 * @fn      input_set_polarity( input_pin_t, input_polarity_t )
 * @brief   Sets the input polarity for the specified input pin.
 * @note    This modifies the application configuration.
 */
void input_set_polarity( input_pin_t pin, input_polarity_t polarity );

/**
 * @fn      input_set_type( input_pin_t, input_type_t )
 * @brief   Sets the input type for the specified input pin.
 * @note    This modifies the application configuration.
 */
void input_set_type( input_pin_t pin, input_type_t type );

/**
 * @fn      input_tick( tick_t )
 * @brief   Performs periodic processing at the specified tick count.
 */
void input_tick( tick_t tick );

/**
 * @fn      input_type_get_on( input_type_t )
 * @brief   Returns `true` if any input with the specified type is on.
 */
bool input_type_get_on( input_type_t type );

/**
 * @fn      input_types_get_on( void )
 * @brief   Returns a bitfield of all input types which are currently on.
 */
input_type_field_t input_types_get_on( void );

#endif /* !defined( APPLICATION_INPUT_H ) */
