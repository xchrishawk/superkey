/**
 * @file    src/main/application/quick_msg.h
 * @brief   Header for the quick message module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-09-10
 * @cpyrt   © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "application/keyer.h"
#include "application/quick_msg.h"
#include "application/storage.h"
#include "core/sys.h"
#include "utility/constants.h"
#include "utility/debug.h"
#include "utility/utility.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

/**
 * @def     MINIMUM_SAVE_PERIOD
 * @brief   Minimum elapsed time between saving quick messages to storage.
 */
#define MINIMUM_SAVE_PERIOD         ( 5 * TICKS_PER_SEC )

/* --------------------------------------------------- VARIABLES ---------------------------------------------------- */

static char s_quick_msgs[ QUICK_MSG_IDX_COUNT ][ QUICK_MSG_MAX_LEN ];
static bool s_modified[ QUICK_MSG_IDX_COUNT ];
static tick_t s_save_tick = 0;

/* ----------------------------------------------------- MACROS ----------------------------------------------------- */

#define validate_idx( _idx )                                                            \
    assert_always( ( _idx ) < QUICK_MSG_IDX_COUNT )


_Static_assert( STORAGE_QUICK_MSG_COUNT == QUICK_MSG_IDX_COUNT, "Incorrect number of quick message indices!" );
_Static_assert( STORAGE_QUICK_MSG_SIZE == QUICK_MSG_MAX_LEN, "Incorrect size of storage!" );

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      load_from_storage( void )
 * @brief   Loads the current local working copy from non-volatile storage.
 */
static void load_from_storage( void );

/**
 * @fn      save_to_storage( void )
 * @brief   Saves the current local working copy to non-volatile storage.
 */
static void save_to_storage( tick_t tick );

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

char const * quick_msg_get( quick_msg_idx_t idx )
{
    validate_idx( idx );

    return( ( char const * )s_quick_msgs[ idx ] );

}   /* quick_msg_get() */


void quick_msg_init( void )
{
    // Initialize state
    memset( s_quick_msgs, 0, sizeof( s_quick_msgs ) );
    memset( s_modified, 0, sizeof( s_modified ) );
    s_save_tick = 0;

    // Load strings from non-volatile storage
    load_from_storage();

}   /* quick_msg_init() */


void quick_msg_invalidate( quick_msg_idx_t idx )
{
    validate_idx( idx );

    // Make string zero length
    s_quick_msgs[ idx ][ 0 ] = NULL_CHAR;
    s_modified[ idx ] = true;

    // Save to storage, if allowed
    save_to_storage( sys_get_tick() );

}   /* quick_msg_invalidate() */


bool quick_msg_is_valid( quick_msg_idx_t idx )
{
    validate_idx( idx );

    return( s_quick_msgs[ idx ][ 0 ] != NULL_CHAR );

}   /* quick_msg_is_valid() */


size_t quick_msg_key( quick_msg_idx_t idx )
{
    validate_idx( idx );

    if( ! quick_msg_is_valid( idx ) )
        return( 0 );

    return( keyer_autokey_str( s_quick_msgs[ idx ] ) );

}   /* quick_msg_key() */


void quick_msg_set( quick_msg_idx_t idx, char const * message )
{
    validate_idx( idx );

    // Save message to local buffer, ensuring it's null-terminated
    strncpy( s_quick_msgs[ idx ], message, QUICK_MSG_MAX_LEN );
    s_quick_msgs[ idx ][ QUICK_MSG_MAX_LEN - 1 ] = NULL_CHAR;
    s_modified[ idx ] = true;

    // Save to storage, if allowed
    save_to_storage( sys_get_tick() );

}   /* quick_msg_set() */


void quick_msg_tick( tick_t tick )
{
    // Save to storage, if required
    save_to_storage( tick );

}   /* quick_msg_tick() */


static void load_from_storage( void )
{
    for( quick_msg_idx_t idx = 0; idx < QUICK_MSG_IDX_COUNT; idx++ )
        if( ! storage_get_quick_msg( idx, s_quick_msgs[ idx ], sizeof( s_quick_msgs[ idx ] ) ) )
            memset( s_quick_msgs[ idx ], 0, sizeof( s_quick_msgs[ idx ] ) );

}   /* load_from_storage() */


static void save_to_storage( tick_t tick )
{
    // Take no action if we've saved recently
    if( sys_elapsed( tick, s_save_tick ) < MINIMUM_SAVE_PERIOD )
        return;

    // Loop through each message
    bool any_saved = false;
    for( quick_msg_idx_t idx = 0; idx < QUICK_MSG_IDX_COUNT; idx++ )
    {
        // If the message isn't modified, skip it
        if( ! s_modified[ idx ] )
            continue;

        // Save the message
        storage_set_quick_msg( idx, s_quick_msgs[ idx ], sizeof( s_quick_msgs[ idx ] ) );
        any_saved = true;

        // Clear modified flag
        s_modified[ idx ] = false;
    }

    // If we saved anything, update tick for most recent save
    if( any_saved )
        s_save_tick = tick;

}   /* save_from_storage() */
