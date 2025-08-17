/**
 * @file    input.h
 * @brief   Header for the key input module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-17
 */

#if !defined( EXAKEY_INPUT_H )
#define EXAKEY_INPUT_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdint.h>

/* ----------------------------------------------------- TYPES ------------------------------------------------------ */

/**
 * @typedef input_t
 * @brief   Enumeration of the supported input types.
 */
typedef uint8_t input_t;
enum
{
    INPUT_STRAIGHT_KEY,                     /**< Input is the straight key.             */
    INPUT_PADDLE_A,                         /**< Input is key A from the paddles.       */
    INPUT_PADDLE_B,                         /**< Input is key B from the paddles.       */

    INPUT_COUNT,                            /**< Number of valid inputs.                */
};

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      input_get( input_t )
 * @brief   Returns the state of the specified input key.
 */
bool input_get( input_t input );

/**
 * @fn      input_init( void )
 * @brief   Initializes the key input module.
 */
void input_init( void );

#endif /* !defined( EXAKEY_INPUT_H ) */
