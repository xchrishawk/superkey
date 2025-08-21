/**
 * @file    src/exakey/application/debug_port.h
 * @brief   Header for the debug port module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-20
 */

#if !defined( EXAKEY_DEBUG_PORT_H )
#define EXAKEY_DEBUG_PORT_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>

#include "drivers/usart.h"
#include "utility/types.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

/**
 * @def     DEBUG_PORT_USART
 * @brief   The USART for the debug port.
 */
#define DEBUG_PORT_USART        ( USART_1 )

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      debug_port_init( void )
 * @brief   Initializes the debug port.
 */
void debug_port_init( void );

/**
 * @fn      debug_port_print( char const * )
 * @brief   Writes the specified string to the debug port.
 */
bool debug_port_print( char const * str );

/**
 * @fn      debug_port_printf( char const *, ... )
 * @brief   Writes the specified formatted string to the debug port.
 */
bool debug_port_printf( char const * fmt, ... );

/**
 * @fn      debug_port_tick( tick_t )
 * @brief   Performs periodic processing at the specified tick count.
 */
void debug_port_tick( tick_t tick );

/**
 * @fn      debug_port_usart_rx( void )
 * @brief   Notifies the debug port module that the USART has new data available.
 */
void debug_port_usart_rx( void );

#endif /* !defined( EXAKEY_DEBUG_PORT_H ) */
