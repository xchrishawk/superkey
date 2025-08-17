/**
 * @file    debug.h
 * @brief   Header for debug utilities.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-17
 */

#if !defined( EXAKEY_DEBUG_H )
#define EXAKEY_DEBUG_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdint.h>

/* ----------------------------------------------------- MACROS ----------------------------------------------------- */

/**
 * @def     assert_debug
 * @brief   Checks an assertion in debug builds. Calls `fail()` if the assertion fails.
 */
#define assert_debug( _cond )                                                           \
    ( ( _cond ) ? ( void )0 : fail() )

/**
 * @def     assert_always
 * @brief   Checks an assertion in all builds. Calls `fail()` if the assertion fails.
 */
#define assert_always( _cond )                                                          \
    ( ( _cond ) ? ( void )0 : fail() )

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      fail( void )
 * @brief   Aborts the application. Enters an infinite loop which flashes the status LED.
 */
void fail( void ) __attribute__((__noreturn__));

/**
 * @fn      fail( uint8_t )
 * @brief   Aborts the application. Enters an infinite loop which flashes the specified code on the status LED.
 */
void fail_code( uint8_t code ) __attribute__((__noreturn__));

#endif /* !defined( EXAKEY_DEBUG_H ) */
