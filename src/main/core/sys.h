/**
 * @file        src/main/core/sys.h
 * @brief       Header for the main system module.
 *
 * @author      Chris Vig (chris@invictus.so)
 * @date        2025-08-17
 * @copyright   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 */

#if !defined( CORE_SYS_H )
#define CORE_SYS_H

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stdint.h>

#include "utility/constants.h"
#include "utility/types.h"

/* ----------------------------------------------------- TYPES ------------------------------------------------------ */

/**
 * @typedef event_t
 * @brief   Enumeration of the supported system events.
 */
typedef uint8_t event_t;
enum
{
    EVENT_TICK,                             /**< 1 millisecond tick event.              */
    EVENT_INPUT_STATE,                      /**< Key input state changed.               */
    EVENT_USART_0_RX_COMPLETE,              /**< USART 0 completed receiving data.      */
    EVENT_USART_0_TX_COMPLETE,              /**< USART 0 completed transmitting data.   */
    EVENT_USART_1_RX_COMPLETE,              /**< USART 1 completed receiving data.      */
    EVENT_USART_1_TX_COMPLETE,              /**< USART 1 completed transmitting data.   */

    EVENT_COUNT,                            /**< Number of valid events.                */

    EVENT_NONE                              /**< Invalid / no event.                    */
        = EVENT_COUNT,
};

/**
 * @typedef event_field_t
 * @brief   Bitfield of pending events.
 * @note    Bits are indexed based on the `event_t` enumeration.
 */
typedef uint32_t event_field_t;

_Static_assert( EVENT_COUNT < sizeof( event_field_t ) * BITS_PER_BYTE, "Not enough bits in event_field_t!" );

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

/**
 * @def     TICKS_PER_SEC
 * @brief   The number of system ticks per second.
 */
#define TICKS_PER_SEC   ( 1000 )

/**
 * @def     TICKS_PER_MSEC
 * @brief   The number of system ticks per millisecond.
 */
#define TICKS_PER_MSEC  ( 1 )

/**
 * @def     TICK_MAX
 * @brief   The maximum value of the tick counter.
 * @note    After reaching this value, the tick counter will roll over to zero on the next tick.
 */
#define TICK_MAX        UINT32_MAX

/* ----------------------------------------------------- MACROS ----------------------------------------------------- */

/**
 * @def     sys_cli()
 * @brief   Globally disables interrupts.
 */
#define sys_cli()                                                                       \
    sys_set_intrpt_enabled( false )

/**
 * @def     sys_sei()
 * @brief   Globally enables interrupts.
 */
#define sys_sei()                                                                       \
    sys_set_intrpt_enabled( true )

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      sys_elapsed( tick_t, tick_t )
 * @brief   Returns the number of system ticks which elapsed between `then` and `now`.
 * @note    This function is wraparound-aware, and will return the correct value even if the tick counter has reached
 *          its maximum value and rolled over.
 */
tick_t sys_elapsed( tick_t now, tick_t then );

/**
 * @fn      sys_elapsed_now( tick_t )
 * @brief   Returns the number of system ticks which have elapsed since `then`.
 * @note    This function is wraparound-aware, and will return the correct value even if the tick counter has reached
 *          its maximum value and rolled over.
 * @note    This is equivalent to calling `sys_elapsed( sys_get_tick(), then )`.
 */
tick_t sys_elapsed_now( tick_t then );

/**
 * @fn      sys_enqueue_event( event_t )
 * @brief   Sets the currently pending system event.
 */
void sys_enqueue_event( event_t event );

/**
 * @fn      sys_get_tick( void )
 * @brief   Returns the current system tick count.
 */
tick_t sys_get_tick( void );

/**
 * @fn      sys_init( void )
 * @brief   Initializes the system module.
 */
void sys_init( void );

/**
 * @fn      sys_intrpt_enabled( void )
 * @brief   Returns `true` if interrupts are globally enabled.
 */
bool sys_intrpt_enabled( void );

/**
 * @fn      sys_is_tick_gt( tick_t, tick_t )
 * @brief   Returns `true` if `a` is later than `b`.
 * @note    This function is wraparound-aware, and will return the correct value even if the tick counter has reached
 *          its maximum value and rolled over.
 */
bool sys_is_tick_gt( tick_t a, tick_t b );

/**
 * @fn      sys_is_tick_gte( tick_t, tick_t )
 * @brief   Returns `true` if `a` is later than `b`.
 * @note    This function is wraparound-aware, and will return the correct value even if the tick counter has reached
 *          its maximum value and rolled over.
 */
bool sys_is_tick_gte( tick_t a, tick_t b );

/**
 * @fn      sys_set_intrpt_enabled( bool )
 * @brief   Globally enables or disables interrupts.
 */
void sys_set_intrpt_enabled( bool enabled );

/**
 * @fn      sys_wait( void )
 * @brief   Sleeps until the next event.
 */
event_field_t sys_wait( void );

#endif /* !defined( CORE_SYS_H ) */
