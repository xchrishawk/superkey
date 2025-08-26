/**
 * @file    src/main/drivers/eeprom.c
 * @brief   Implementation for the EEPROM driver module.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-25
 */

/* ---------------------------------------------------- INCLUDES ---------------------------------------------------- */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "core/sys.h"
#include "drivers/eeprom.h"
#include "utility/debug.h"
#include "utility/types.h"
#include "utility/utility.h"

/* --------------------------------------------------- CONSTANTS ---------------------------------------------------- */

/**
 * @def     WRITE_BUF_SZ
 * @brief   The size for the write buffer.
 * @note    The largest object we currently write is the config structure, which is ~22 bytes (at time of this writing).
 *          Set the buffer size to 64 bytes as an initial approximation.
 */
#define WRITE_BUF_SZ    64

/**
 * @def     WRITE_DELAY_MS
 * @brief   Time to delay when waiting for space in write buffer.
 * @note    This is a guess and could be tuned / modified.
 */
#define WRITE_DELAY_MS  2

/* --------------------------------------------------- VARIABLES ---------------------------------------------------- */

static struct
{
    eeprom_addr_t   address;                    /**< Address to write to.               */
    byte_t          data;                       /**< Data to write.                     */
}
s_write_buf[ WRITE_BUF_SZ ];                    /**< Circular write buffer for EEPROM.  */
static volatile size_t s_write_head = 0;        /**< Head position for circular buffer. */
static volatile size_t s_write_tail = 0;        /**< Tail position for circular buffer. */

/* ----------------------------------------------------- MACROS ----------------------------------------------------- */

/**
 * @def     validate_addr( _addr )
 * @brief   Validates the specified `eeprom_addr_t`.
 */
#define validate_addr( _addr )                                                          \
    assert_always( ( _addr ) < EEPROM_COUNT )

/* ---------------------------------------------- PROCEDURE PROTOTYPES ---------------------------------------------- */

/**
 * @fn      get_data( void )
 * @brief   Immediately returns the content of the EEDR data register.
 */
static byte_t get_data( void );

/**
 * @fn      ready_isr( void )
 * @brief   Runs the ISR for the "EEPROM ready" interrupt.
 */
static void ready_isr( void );

/**
 * @fn      set_address( eeprom_addr_t )
 * @brief   Sets the content of the EEAR address register.
 */
static void set_address( eeprom_addr_t addr );

/**
 * @fn      set_data( byte_t byte )
 * @brief   Sets the content of the EEDR data register.
 */
static void set_data( byte_t byte );

/**
 * @fn      set_eeprom_interrupt_enabled( bool )
 * @brief   Enables or disables the EEPROM ready interrupt.
 */
static void set_eeprom_interrupt_enabled( bool enabled );

/**
 * @fn      set_write_mode_erase_only( void )
 * @brief   Sets the write mode to erase only.
 */
static void set_write_mode_erase_only( void );

/**
 * @fn      set_write_mode_erase_write( void )
 * @brief   Sets the write mode to erase and write atomically.
 */
static void set_write_mode_erase_write( void );

/**
 * @fn      strobe_read_enable( void )
 * @brief   Strobes the EERE bit to start an EEPROM read.
 */
static void strobe_read_enable( void );

/**
 * @fn      strobe_write_enable( void )
 * @brief   Strobes the EEMPE and EEPE bits to start an EEPROM write.
 */
static void strobe_write_enable( void );

/**
 * @fn      wait_ready( void )
 * @brief   Synchronously waits for the `EEPE` bit in `EECR` to clear, indicating that EEPROM is ready to read or write.
 */
static void wait_ready( void );

/**
 * @fn      write_buf_avail( void )
 * @brief   Returns the number of bytes available in the write buffer.
 */
static size_t write_buf_avail( void );

/**
 * @fn      write_buf_count( void )
 * @brief   Returns the number of bytes currently in the write buffer.
 */
static size_t write_buf_count( void );

/* --------------------------------------------------- PROCEDURES --------------------------------------------------- */

void eeprom_erase_byte( eeprom_addr_t addr )
{
    validate_addr( addr );

    wait_ready();
    set_write_mode_erase_only();
    set_address( addr );

    strobe_write_enable();

}   /* eeprom_erase_byte() */


void eeprom_read( eeprom_addr_t addr, void * buf, size_t size )
{
    validate_addr( addr );
    assert_always( addr + size <= EEPROM_COUNT );

    for( size_t idx = 0; idx < size; idx++ )
        ( ( uint8_t * )buf )[ idx ] = eeprom_read_byte( addr + idx );

}   /* eeprom_read() */


byte_t eeprom_read_byte( eeprom_addr_t addr )
{
    validate_addr( addr );

    wait_ready();
    set_address( addr );
    strobe_read_enable();

    return( get_data() );

}   /* eeprom_read_byte() */


void eeprom_write( eeprom_addr_t addr, void const * buf, size_t size )
{
    validate_addr( addr );
    assert_always( addr + size <= EEPROM_COUNT );

    byte_t const * ptr = ( byte_t * )buf;

    // Get interrupt flag
    bool intrpt_en = sys_intrpt_enabled();

    // Loop as long as there's still data
    while( size )
    {
        // Wait until we have room available in the write buffer.
        // If there's no room, re-enable interrupts and then wait long enough for a couple of bytes to write.
        sys_cli();
        size_t avail = write_buf_avail();
        if( avail == 0 )
        {
            sys_sei();
            _delay_ms( WRITE_DELAY_MS );
            continue;
        }

        // Write as many bytes as will fit into buffer
        while( size && avail )
        {
            s_write_buf[ s_write_head ].address = addr++;
            s_write_buf[ s_write_head ].data = *( ptr++ );
            increment_rollover( s_write_head, WRITE_BUF_SZ );
            size--;
            avail--;
        }

        // Enable interrupt to start writing
        set_eeprom_interrupt_enabled( true );
    }

    // Restore original state of interrupt flag
    sys_set_intrpt_enabled( intrpt_en );

}   /* eeprom_write() */


void eeprom_write_byte( eeprom_addr_t addr, byte_t byte )
{
    validate_addr( addr );

    wait_ready();
    set_write_mode_erase_write();
    set_address( addr );
    set_data( byte );

    strobe_write_enable();

}   /* eeprom_write_byte() */


static byte_t get_data( void )
{
    return( EEDR );

}   /* get_data() */


static void ready_isr( void )
{
    // If no data is left, disable the interrupt and return
    if( write_buf_count() == 0 )
    {
        set_eeprom_interrupt_enabled( false );
        return;
    }

    // Write the next byte
    set_write_mode_erase_write();
    set_address( s_write_buf[ s_write_tail ].address );
    set_data( s_write_buf[ s_write_tail ].data );
    increment_rollover( s_write_tail, WRITE_BUF_SZ );
    strobe_write_enable();

}   /* ready_isr() */


static void set_address( eeprom_addr_t addr )
{
    EEAR = addr;

}   /* set_address() */


static void set_data( byte_t byte )
{
    EEDR = byte;

}   /* set_data() */


static void set_eeprom_interrupt_enabled( bool enabled )
{
    assign_bit( EECR, EERIE, enabled );

}   /* set_eeprom_interrupt_enabled() */


static void set_write_mode_erase_only( void )
{
    set_bit( EECR, EEPM0 );
    clear_bit( EECR, EEPM1 );

}   /* set_write_mode_erase_only() */


static void set_write_mode_erase_write( void )
{
    clear_bit( EECR, EEPM0 );
    clear_bit( EECR, EEPM1 );

}   /* set_write_mode_erase_write() */


static void strobe_read_enable( void )
{
    set_bit( EECR, EERE );

}   /* strobe_read_enable() */


static void strobe_write_enable( void )
{
    // The write to EEPE must happen within 4 clock cycles of the write to EEMPE.
    // Clear interrupt flag while writing bits to prevent this from being interrupted.
    sys_cli();
    set_bit( EECR, EEMPE );     // enable writes
    set_bit( EECR, EEPE );      // command write
    sys_sei();

}   /* strobe_write_enable() */


static void wait_ready( void )
{
    wait_bit_clear( EECR, EEPE );

}   /* wait_ready() */


static size_t write_buf_avail( void )
{
    return( WRITE_BUF_SZ - write_buf_count() - 1 );

}   /* write_buf_avail() */


static size_t write_buf_count( void )
{
    if( s_write_head >= s_write_tail )
        return( s_write_head - s_write_tail );
    else
        return( WRITE_BUF_SZ - s_write_tail + s_write_head );

}   /* write_buf_count() */


ISR( EE_READY_vect )
{
    ready_isr();

}   /* ISR( EE_READY_vect ) */
