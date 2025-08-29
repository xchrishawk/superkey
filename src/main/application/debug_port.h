/**
 * @file    src/main/application/debug_port.h
 * @brief   Header for the debug port module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-20
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 */

#if !defined( APPLICATION_DEBUG_PORT_H )
#define APPLICATION_DEBUG_PORT_H

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

#if defined( _FEATURE_ENABLE_DEBUG_PORT ) && _FEATURE_ENABLE_DEBUG_PORT

/**
 * @fn      debug_port_init( void )
 * @brief   Initializes the debug port.
 * @note    The debug port should be initialized after all other modules.
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

#else

// Define dummy macros if debug port is disabled
#define debug_port_init()
#define debug_port_print( _str )        ( false )
#define debug_port_printf( _fmt, ... )  ( false )
#define debug_port_tick( _tick )
#define debug_port_usart_rx()

#endif /* defined( _FEATURE_ENABLE_DEBUG_PORT ) && _FEATURE_ENABLE_DEBUG_PORT */

#endif /* !defined( APPLICATION_DEBUG_PORT_H ) */
