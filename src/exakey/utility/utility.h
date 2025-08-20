/**
 * @file    src/exakey/utility/utility.h
 * @brief   Header defining common utility macros.
 *
 * @author  Chris Vig (chris@invictus.so)
 * @date    2025-08-16
 */

#if !defined( EXAKEY_UTILITY_H )
#define EXAKEY_UTILITY_H

/* ----------------------------------------------------- MACROS ----------------------------------------------------- */

/**
 * @def     array_count( _a )
 * @brief   Returns the number of elements in the specified array.
 */
#define array_count( _a )                                                               \
    ( sizeof( _a ) / sizeof( _a[ 0 ] ) )

/**
 * @def     increment_rollover
 * @brief   Increments `_value` by one. If the result is greater than or equal to `_max`, resets `_value` to 0.
 */
#define increment_rollover( _value, _max )                                              \
    do                                                                                  \
    {                                                                                   \
        ( _value )++;                                                                   \
        if( ( _value ) >= ( _max ) )                                                    \
            ( _value ) = 0;                                                             \
    }                                                                                   \
    while( 0 )

/**
 * @def     stringize( _x )
 * @brief   Returns a string representation of the argument.
 */
#define stringize( _x )                                                                 \
    # _x

/**
 * @def     stringize_value( _x )
 * @brief   Returns a string representation of the *value* of the argument.
 */
#define stringize_value( _x )                                                           \
    stringize( _x )

/**
 * @def     bitmask( _b )
 * @brief   Returns a bit mask for the specified zero-indexed bit (i.e., `( 1 << ( _b ) )`).
 */
#define bitmask( _b )                                                                   \
    ( 1 << ( _b ) )

/**
 * @def     bitmask1( _b1 )
 * @brief   Returns a bit mask for the specified zero-indexed bit.
 */
#define bitmask1( _b1 )                                                                 \
    ( bitmask( _b1 ) )

/**
 * @def     bitmask2( _b1, _b2 )
 * @brief   Returns a bit mask for the specified zero-indexed bits.
 */
#define bitmask2( _b1, _b2 )                                                            \
    ( bitmask1( _b1 ) | bitmask( _b2 ) )

/**
 * @def     bitmask3( _b1, _b2 )
 * @brief   Returns a bit mask for the specified zero-indexed bits.
 */
#define bitmask3( _b1, _b2, _b3 )                                                       \
    ( bitmask2( _b1, _b2 ) | bitmask( _b3 ) )

/**
 * @def     bitmask4( _b1, _b2 )
 * @brief   Returns a bit mask for the specified zero-indexed bits.
 */
#define bitmask4( _b1, _b2, _b3, _b4 )                                                  \
    ( bitmask3( _b1, _b2, _b3 ) | bitmask( _b4 ) )

/**
 * @def     bitmask5( _b1, _b2 )
 * @brief   Returns a bit mask for the specified zero-indexed bits.
 */
#define bitmask5( _b1, _b2, _b3, _b4, _b5 )                                             \
    ( bitmask4( _b1, _b2, _b3, _b4 ) | bitmask( _b5 ) )

/**
 * @def     bitmask6( _b1, _b2 )
 * @brief   Returns a bit mask for the specified zero-indexed bits.
 */
#define bitmask6( _b1, _b2, _b3, _b4, _b5, _b6 )                                        \
    ( bitmask5( _b1, _b2, _b3, _b4, _b5 ) | bitmask( _b6 ) )

/**
 * @def     bitmask7( _b1, _b2 )
 * @brief   Returns a bit mask for the specified zero-indexed bits.
 */
#define bitmask7( _b1, _b2, _b3, _b4, _b5, _b6, _b7 )                                   \
    ( bitmask6( _b1, _b2, _b3, _b4, _b5, _b6 ) | bitmask( _b7 ) )

/**
 * @def     bitmask8( _b1, _b2 )
 * @brief   Returns a bit mask for the specified zero-indexed bits.
 */
#define bitmask8( _b1, _b2, _b3, _b4, _b5, _b6, _b7, _b8 )                              \
    ( bitmask7( _b1, _b2, _b3, _b4, _b5, _b6, _b7 ) | bitmask( _b8 ) )

/**
 * @def     clear_bitmask( _tgt, _m )
 * @brief   Clears all bits in `_tgt` specified by the bit mask `_m`.
 */
#define clear_bitmask( _tgt, _m )                                                       \
    ( _tgt ) &= ( ~ ( _m ) )

/**
 * @def     set_bitmask( _tgt, _m )
 * @brief   Sets all bits in `_tgt` specified by the bit mask `_m`.
 */
#define set_bitmask( _tgt, _m )                                                         \
    ( _tgt ) |= ( _m )

/**
 * @def     toggle_bitmask( _tgt, _m )
 * @brief   Toggles all bits in `_tgt` specified by the bit mask `_m`.
 */
#define toggle_bitmask( _tgt, _m )                                                      \
    ( _tgt ) ^= ( _m )

/**
 * @def     assign_bitmask( _tgt, _m, _v )
 * @brief   Sets all bits in `_tgt` specified by the bit mask `_m` if `_v` evaluates to a true value, or clears them
 *          if `_v` evaluates to a false value.
 */
#define assign_bitmask( _tgt, _m, _v )                                                  \
    do                                                                                  \
    {                                                                                   \
        if( _v )                                                                        \
            set_bitmask( _tgt, _m );                                                    \
        else                                                                            \
            clear_bitmask( _tgt, _m );                                                  \
    }                                                                                   \
    while( 0 )

/**
 * @def     clear_bit( _tgt, _b )
 * @brief   Clears the zero-indexed bit `_b` in `_tgt`.
 */
#define clear_bit( _tgt, _b )                                                           \
    clear_bitmask( _tgt, bitmask( _b ) )

/**
 * @def     set_bit( _tgt, _b )
 * @brief   Sets the zero-indexed bit `_b` in `_tgt`.
 */
#define set_bit( _tgt, _b )                                                             \
    set_bitmask( _tgt, bitmask( _b ) )

/**
 * @def     toggle_bit( _tgt, _b )
 * @brief   Toggles the zero-indexed bit `_b` in `_tgt`.
 */
#define toggle_bit( _tgt, _b )                                                          \
    toggle_bitmask( _tgt, bitmask( _b ) )

/**
 * @def     assign_bit( _tgt, _b, _v )
 * @brief   Sets the zero-indexed bit `_b` in `_tgt` if `_v` evaluates to a true value, or clears it if `_v` evaluates
 *          to a false value.
 */
#define assign_bit( _tgt, _b, _v )                                                      \
    assign_bitmask( _tgt, bitmask( _b ), _v )

/**
 * @def     is_bitmask_set( _tgt, _m )
 * @brief   Returns true if all bits specified by the bit mask `_m` in `_tgt` are set.
 */
#define is_bitmask_set( _tgt, _m )                                                      \
    ( ( ( _tgt ) & ( _m ) ) == ( _m ) )

/**
 * @def     is_bitmask_clear( _tgt, _m )
 * @brief   Returns true if all bits specified by the bit mask `_m` in `_tgt` are clear.
 */
#define is_bitmask_clear( _tgt, _m )                                                    \
    ( ( ( _tgt ) & ( _m ) ) == 0 )

/**
 * @def     is_bit_set( _tgt, _b )
 * @brief   Returns true if the zero-indexed bit `_b` is set in `_tgt`.
 */
#define is_bit_set( _tgt, _b )                                                          \
    is_bitmask_set( _tgt, bitmask( _b ) )

/**
 * @def     is_bit_clear( _tgt, _b )
 * @brief   Returns true if the zero-indexed bit `_b` is clear in `_tgt`.
 */
#define is_bit_clear( _tgt, _b )                                                        \
    is_bitmask_clear( _tgt, bitmask( _b ) )

/**
 * @def     wait_bitmask_clear( _tgt, _m )
 * @brief   Waits in a spin loop until all bits specified by `_m` in `_tgt` are clear.
 */
#define wait_bitmask_clear( _tgt, _m )                                                  \
    do { } while( ! is_bitmask_clear( _tgt, _m ) )

/**
 * @def     wait_bitmask_set( _tgt, _m )
 * @brief   Waits in a spin loop until all bits specified by `_m` in `_tgt` are set.
 */
#define wait_bitmask_set( _tgt, _m )                                                    \
    do { } while( ! is_bitmask_set( _tgt, _m ) )

/**
 * @def     wait_bit_clear( _tgt, _b )
 * @brief   Waits in a spin loop until the zero-indexed bit specified by `_b` in `_tgt` is clear.
 */
#define wait_bit_clear( _tgt, _b )                                                      \
    do { } while( ! is_bit_clear( _tgt, _b ) )

/**
 * @def     wait_bit_set( _tgt, _b )
 * @brief   Waits in a spin loop until the zero-indexed bit specified by `_b` in `_tgt` is set.
 */
#define wait_bit_set( _tgt, _b )                                                        \
    do { } while( ! is_bit_set( _tgt, _b ) )

#endif  /* !defined( EXAKEY_UTILITY_H ) */
