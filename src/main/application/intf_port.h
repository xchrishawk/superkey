/**
 * @file    src/main/application/intf_port.h
 * @brief   Header for the interface port module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-29
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 */

#if !defined( APPLICATION_INTF_PORT_H )
#define APPLICATION_INTF_PORT_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include "drivers/usart.h"
#include "utility/types.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

/**
 * @def     INTF_PORT_USART
 * @brief   The USART for the interface port.
 */
#define INTF_PORT_USART     ( USART_0 )

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      intf_port_init( void )
 * @brief   Initializes the interface port.
 * @note    Requires that the following modules have been initialized:
 *          - `config`
 *          - `io`
 *          - `keyer`
 *          - `sys`
 */
void intf_port_init( void );

/**
 * @fn      intf_port_tick( tick_t )
 * @brief   Performs periodic processing at the specified tick.
 */
void intf_port_tick( tick_t tick );

/**
 * @fn      intf_port_usart_rx( void )
 * @brief   Notifies the interface port module that the USART has new data available.
 */
void intf_port_usart_rx( void );

#endif /* !defined( APPLICATION_INTF_PORT_H ) */
