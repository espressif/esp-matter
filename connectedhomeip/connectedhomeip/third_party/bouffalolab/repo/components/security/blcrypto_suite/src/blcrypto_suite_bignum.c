/*
 *  Multi-precision integer library
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 *
 *  This file is provided under the Apache License 2.0, or the
 *  GNU General Public License v2.0 or later.
 *
 *  **********
 *  Apache License 2.0:
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  **********
 *
 *  **********
 *  GNU General Public License v2.0 or later:
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *  **********
 */

/*
 *  The following sources were referenced in the design of this Multi-precision
 *  Integer library:
 *
 *  [1] Handbook of Applied Cryptography - 1997
 *      Menezes, van Oorschot and Vanstone
 *
 *  [2] Multi-Precision Math
 *      Tom St Denis
 *      https://github.com/libtom/libtommath/blob/develop/tommath.pdf
 *
 *  [3] GNU Multi-Precision Arithmetic Library
 *      https://gmplib.org/manual/index.html
 *
 */

#if !defined(BLCRYPTO_SUITE_CONFIG_FILE)
#include "blcrypto_suite/blcrypto_suite_config.h"
#else
#include BLCRYPTO_SUITE_CONFIG_FILE
#endif

#if defined(BLCRYPTO_SUITE_BIGNUM_C)

#include "blcrypto_suite/blcrypto_suite_bignum.h"
#include "blcrypto_suite/blcrypto_suite_bn_mul.h"
#include "blcrypto_suite/blcrypto_suite_platform_util.h"

#include <string.h>

#include <blcrypto_suite/blcrypto_suite_top_config.h>

#if defined(BLCRYPTO_SUITE_PLATFORM_C)
#include "blcrypto_suite/blcrypto_suite_platform.h"
#else
#include <stdio.h>
#include <stdlib.h>
#define blcrypto_suite_printf     printf
#define blcrypto_suite_calloc    calloc
#define blcrypto_suite_free       free
#endif

#define MPI_VALIDATE_RET( cond )                                       \
    BLCRYPTO_SUITE_INTERNAL_VALIDATE_RET( cond, BLCRYPTO_SUITE_ERR_MPI_BAD_INPUT_DATA )
#define MPI_VALIDATE( cond )                                           \
    BLCRYPTO_SUITE_INTERNAL_VALIDATE( cond )

#define ciL    (sizeof(blcrypto_suite_mpi_uint))         /* chars in limb  */
#define biL    (ciL << 3)               /* bits  in limb  */
#define biH    (ciL << 2)               /* half limb size */

#define MPI_SIZE_T_MAX  ( (size_t) -1 ) /* SIZE_T_MAX is not standard */

/*
 * Convert between bits/chars and number of limbs
 * Divide first in order to avoid potential overflows
 */
#define BITS_TO_LIMBS(i)  ( (i) / biL + ( (i) % biL != 0 ) )
#define CHARS_TO_LIMBS(i) ( (i) / ciL + ( (i) % ciL != 0 ) )

/* Implementation that should never be optimized out by the compiler */
static void blcrypto_suite_mpi_zeroize( blcrypto_suite_mpi_uint *v, size_t n )
{
    blcrypto_suite_platform_zeroize( v, ciL * n );
}

/*
 * Initialize one MPI
 */
void blcrypto_suite_mpi_init( blcrypto_suite_mpi *X )
{
    MPI_VALIDATE( X != NULL );

    X->s = 1;
    X->n = 0;
    X->p = NULL;
}

/*
 * Unallocate one MPI
 */
void blcrypto_suite_mpi_free( blcrypto_suite_mpi *X )
{
    if( X == NULL )
        return;

    if( X->p != NULL )
    {
        blcrypto_suite_mpi_zeroize( X->p, X->n );
        blcrypto_suite_free( X->p );
    }

    X->s = 1;
    X->n = 0;
    X->p = NULL;
}

/*
 * Enlarge to the specified number of limbs
 */
int blcrypto_suite_mpi_grow( blcrypto_suite_mpi *X, size_t nblimbs )
{
    blcrypto_suite_mpi_uint *p;
    MPI_VALIDATE_RET( X != NULL );

    if( nblimbs > BLCRYPTO_SUITE_MPI_MAX_LIMBS )
        return( BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED );

    if( X->n < nblimbs )
    {
        if( ( p = (blcrypto_suite_mpi_uint*)blcrypto_suite_calloc( nblimbs, ciL ) ) == NULL )
            return( BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED );

        if( X->p != NULL )
        {
            memcpy( p, X->p, X->n * ciL );
            blcrypto_suite_mpi_zeroize( X->p, X->n );
            blcrypto_suite_free( X->p );
        }

        X->n = nblimbs;
        X->p = p;
    }

    return( 0 );
}

/*
 * Resize down as much as possible,
 * while keeping at least the specified number of limbs
 */
int blcrypto_suite_mpi_shrink( blcrypto_suite_mpi *X, size_t nblimbs )
{
    blcrypto_suite_mpi_uint *p;
    size_t i;
    MPI_VALIDATE_RET( X != NULL );

    if( nblimbs > BLCRYPTO_SUITE_MPI_MAX_LIMBS )
        return( BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED );

    /* Actually resize up if there are currently fewer than nblimbs limbs. */
    if( X->n <= nblimbs )
        return( blcrypto_suite_mpi_grow( X, nblimbs ) );
    /* After this point, then X->n > nblimbs and in particular X->n > 0. */

    for( i = X->n - 1; i > 0; i-- )
        if( X->p[i] != 0 )
            break;
    i++;

    if( i < nblimbs )
        i = nblimbs;

    if( ( p = (blcrypto_suite_mpi_uint*)blcrypto_suite_calloc( i, ciL ) ) == NULL )
        return( BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED );

    if( X->p != NULL )
    {
        memcpy( p, X->p, i * ciL );
        blcrypto_suite_mpi_zeroize( X->p, X->n );
        blcrypto_suite_free( X->p );
    }

    X->n = i;
    X->p = p;

    return( 0 );
}

/*
 * Copy the contents of Y into X
 */
int blcrypto_suite_mpi_copy( blcrypto_suite_mpi *X, const blcrypto_suite_mpi *Y )
{
    int ret = 0;
    size_t i;
    MPI_VALIDATE_RET( X != NULL );
    MPI_VALIDATE_RET( Y != NULL );

    if( X == Y )
        return( 0 );

    if( Y->n == 0 )
    {
        blcrypto_suite_mpi_free( X );
        return( 0 );
    }

    for( i = Y->n - 1; i > 0; i-- )
        if( Y->p[i] != 0 )
            break;
    i++;

    X->s = Y->s;

    if( X->n < i )
    {
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_grow( X, i ) );
    }
    else
    {
        memset( X->p + i, 0, ( X->n - i ) * ciL );
    }

    memcpy( X->p, Y->p, i * ciL );

cleanup:

    return( ret );
}

/*
 * Swap the contents of X and Y
 */
void blcrypto_suite_mpi_swap( blcrypto_suite_mpi *X, blcrypto_suite_mpi *Y )
{
    blcrypto_suite_mpi T;
    MPI_VALIDATE( X != NULL );
    MPI_VALIDATE( Y != NULL );

    memcpy( &T,  X, sizeof( blcrypto_suite_mpi ) );
    memcpy(  X,  Y, sizeof( blcrypto_suite_mpi ) );
    memcpy(  Y, &T, sizeof( blcrypto_suite_mpi ) );
}

/**
 * Select between two sign values in constant-time.
 *
 * This is functionally equivalent to second ? a : b but uses only bit
 * operations in order to avoid branches.
 *
 * \param[in] a         The first sign; must be either +1 or -1.
 * \param[in] b         The second sign; must be either +1 or -1.
 * \param[in] second    Must be either 1 (return b) or 0 (return a).
 *
 * \return The selected sign value.
 */
static int mpi_safe_cond_select_sign( int a, int b, unsigned char second )
{
    /* In order to avoid questions about what we can reasonnably assume about
     * the representations of signed integers, move everything to unsigned
     * by taking advantage of the fact that a and b are either +1 or -1. */
    unsigned ua = a + 1;
    unsigned ub = b + 1;

    /* second was 0 or 1, mask is 0 or 2 as are ua and ub */
    const unsigned mask = second << 1;

    /* select ua or ub */
    unsigned ur = ( ua & ~mask ) | ( ub & mask );

    /* ur is now 0 or 2, convert back to -1 or +1 */
    return( (int) ur - 1 );
}

/*
 * Conditionally assign dest = src, without leaking information
 * about whether the assignment was made or not.
 * dest and src must be arrays of limbs of size n.
 * assign must be 0 or 1.
 */
static void mpi_safe_cond_assign( size_t n,
                                  blcrypto_suite_mpi_uint *dest,
                                  const blcrypto_suite_mpi_uint *src,
                                  unsigned char assign )
{
    size_t i;

    /* MSVC has a warning about unary minus on unsigned integer types,
     * but this is well-defined and precisely what we want to do here. */
#if defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 4146 )
#endif

    /* all-bits 1 if assign is 1, all-bits 0 if assign is 0 */
    const blcrypto_suite_mpi_uint mask = -assign;

#if defined(_MSC_VER)
#pragma warning( pop )
#endif

    for( i = 0; i < n; i++ )
        dest[i] = ( src[i] & mask ) | ( dest[i] & ~mask );
}

/*
 * Conditionally assign X = Y, without leaking information
 * about whether the assignment was made or not.
 * (Leaking information about the respective sizes of X and Y is ok however.)
 */
int blcrypto_suite_mpi_safe_cond_assign( blcrypto_suite_mpi *X, const blcrypto_suite_mpi *Y, unsigned char assign )
{
    int ret = 0;
    size_t i;
    blcrypto_suite_mpi_uint limb_mask;
    MPI_VALIDATE_RET( X != NULL );
    MPI_VALIDATE_RET( Y != NULL );

    /* MSVC has a warning about unary minus on unsigned integer types,
     * but this is well-defined and precisely what we want to do here. */
#if defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 4146 )
#endif

    /* make sure assign is 0 or 1 in a time-constant manner */
    assign = (assign | (unsigned char)-assign) >> (sizeof( assign ) * 8 - 1);
    /* all-bits 1 if assign is 1, all-bits 0 if assign is 0 */
    limb_mask = -assign;

#if defined(_MSC_VER)
#pragma warning( pop )
#endif

    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_grow( X, Y->n ) );

    X->s = mpi_safe_cond_select_sign( X->s, Y->s, assign );

    mpi_safe_cond_assign( Y->n, X->p, Y->p, assign );

    for( i = Y->n; i < X->n; i++ )
        X->p[i] &= ~limb_mask;

cleanup:
    return( ret );
}

/*
 * Conditionally swap X and Y, without leaking information
 * about whether the swap was made or not.
 * Here it is not ok to simply swap the pointers, which whould lead to
 * different memory access patterns when X and Y are used afterwards.
 */
int blcrypto_suite_mpi_safe_cond_swap( blcrypto_suite_mpi *X, blcrypto_suite_mpi *Y, unsigned char swap )
{
    int ret, s;
    size_t i;
    blcrypto_suite_mpi_uint limb_mask;
    blcrypto_suite_mpi_uint tmp;
    MPI_VALIDATE_RET( X != NULL );
    MPI_VALIDATE_RET( Y != NULL );

    if( X == Y )
        return( 0 );

    /* MSVC has a warning about unary minus on unsigned integer types,
     * but this is well-defined and precisely what we want to do here. */
#if defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 4146 )
#endif

    /* make sure swap is 0 or 1 in a time-constant manner */
    swap = (swap | (unsigned char)-swap) >> (sizeof( swap ) * 8 - 1);
    /* all-bits 1 if swap is 1, all-bits 0 if swap is 0 */
    limb_mask = -swap;

#if defined(_MSC_VER)
#pragma warning( pop )
#endif

    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_grow( X, Y->n ) );
    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_grow( Y, X->n ) );

    s = X->s;
    X->s = mpi_safe_cond_select_sign( X->s, Y->s, swap );
    Y->s = mpi_safe_cond_select_sign( Y->s, s, swap );


    for( i = 0; i < X->n; i++ )
    {
        tmp = X->p[i];
        X->p[i] = ( X->p[i] & ~limb_mask ) | ( Y->p[i] & limb_mask );
        Y->p[i] = ( Y->p[i] & ~limb_mask ) | (     tmp & limb_mask );
    }

cleanup:
    return( ret );
}

/*
 * Set value from integer
 */
int blcrypto_suite_mpi_lset( blcrypto_suite_mpi *X, blcrypto_suite_mpi_sint z )
{
    int ret;
    MPI_VALIDATE_RET( X != NULL );

    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_grow( X, 1 ) );
    memset( X->p, 0, X->n * ciL );

    X->p[0] = ( z < 0 ) ? -z : z;
    X->s    = ( z < 0 ) ? -1 : 1;

cleanup:

    return( ret );
}

/*
 * Get a specific bit
 */
int blcrypto_suite_mpi_get_bit( const blcrypto_suite_mpi *X, size_t pos )
{
    MPI_VALIDATE_RET( X != NULL );

    if( X->n * biL <= pos )
        return( 0 );

    return( ( X->p[pos / biL] >> ( pos % biL ) ) & 0x01 );
}

/* Get a specific byte, without range checks. */
#define GET_BYTE( X, i )                                \
    ( ( ( X )->p[( i ) / ciL] >> ( ( ( i ) % ciL ) * 8 ) ) & 0xff )

/*
 * Set a bit to a specific value of 0 or 1
 */
int blcrypto_suite_mpi_set_bit( blcrypto_suite_mpi *X, size_t pos, unsigned char val )
{
    int ret = 0;
    size_t off = pos / biL;
    size_t idx = pos % biL;
    MPI_VALIDATE_RET( X != NULL );

    if( val != 0 && val != 1 )
        return( BLCRYPTO_SUITE_ERR_MPI_BAD_INPUT_DATA );

    if( X->n * biL <= pos )
    {
        if( val == 0 )
            return( 0 );

        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_grow( X, off + 1 ) );
    }

    X->p[off] &= ~( (blcrypto_suite_mpi_uint) 0x01 << idx );
    X->p[off] |= (blcrypto_suite_mpi_uint) val << idx;

cleanup:

    return( ret );
}

/*
 * Return the number of less significant zero-bits
 */
size_t blcrypto_suite_mpi_lsb( const blcrypto_suite_mpi *X )
{
    size_t i, j, count = 0;
    BLCRYPTO_SUITE_INTERNAL_VALIDATE_RET( X != NULL, 0 );

    for( i = 0; i < X->n; i++ )
        for( j = 0; j < biL; j++, count++ )
            if( ( ( X->p[i] >> j ) & 1 ) != 0 )
                return( count );

    return( 0 );
}

/*
 * Count leading zero bits in a given integer
 */
static size_t blcrypto_suite_clz( const blcrypto_suite_mpi_uint x )
{
    size_t j;
    blcrypto_suite_mpi_uint mask = (blcrypto_suite_mpi_uint) 1 << (biL - 1);

    for( j = 0; j < biL; j++ )
    {
        if( x & mask ) break;

        mask >>= 1;
    }

    return j;
}

/*
 * Return the number of bits
 */
size_t blcrypto_suite_mpi_bitlen( const blcrypto_suite_mpi *X )
{
    size_t i, j;

    if( X->n == 0 )
        return( 0 );

    for( i = X->n - 1; i > 0; i-- )
        if( X->p[i] != 0 )
            break;

    j = biL - blcrypto_suite_clz( X->p[i] );

    return( ( i * biL ) + j );
}

/*
 * Return the total size in bytes
 */
size_t blcrypto_suite_mpi_size( const blcrypto_suite_mpi *X )
{
    return( ( blcrypto_suite_mpi_bitlen( X ) + 7 ) >> 3 );
}

/*
 * Convert an ASCII character to digit value
 */
static int mpi_get_digit( blcrypto_suite_mpi_uint *d, int radix, char c )
{
    *d = 255;

    if( c >= 0x30 && c <= 0x39 ) *d = c - 0x30;
    if( c >= 0x41 && c <= 0x46 ) *d = c - 0x37;
    if( c >= 0x61 && c <= 0x66 ) *d = c - 0x57;

    if( *d >= (blcrypto_suite_mpi_uint) radix )
        return( BLCRYPTO_SUITE_ERR_MPI_INVALID_CHARACTER );

    return( 0 );
}

/*
 * Import from an ASCII string
 */
int blcrypto_suite_mpi_read_string( blcrypto_suite_mpi *X, int radix, const char *s )
{
    int ret;
    size_t i, j, slen, n;
    int sign = 1;
    blcrypto_suite_mpi_uint d;
    blcrypto_suite_mpi T;
    MPI_VALIDATE_RET( X != NULL );
    MPI_VALIDATE_RET( s != NULL );

    if( radix < 2 || radix > 16 )
        return( BLCRYPTO_SUITE_ERR_MPI_BAD_INPUT_DATA );

    blcrypto_suite_mpi_init( &T );

    if( s[0] == '-' )
    {
        ++s;
        sign = -1;
    }

    slen = strlen( s );

    if( radix == 16 )
    {
        if( slen > MPI_SIZE_T_MAX >> 2 )
            return( BLCRYPTO_SUITE_ERR_MPI_BAD_INPUT_DATA );

        n = BITS_TO_LIMBS( slen << 2 );

        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_grow( X, n ) );
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_lset( X, 0 ) );

        for( i = slen, j = 0; i > 0; i--, j++ )
        {
            BLCRYPTO_SUITE_MPI_CHK( mpi_get_digit( &d, radix, s[i - 1] ) );
            X->p[j / ( 2 * ciL )] |= d << ( ( j % ( 2 * ciL ) ) << 2 );
        }
    }
    else
    {
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_lset( X, 0 ) );

        for( i = 0; i < slen; i++ )
        {
            BLCRYPTO_SUITE_MPI_CHK( mpi_get_digit( &d, radix, s[i] ) );
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_mul_int( &T, X, radix ) );
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_add_int( X, &T, d ) );
        }
    }

    if( sign < 0 && blcrypto_suite_mpi_bitlen( X ) != 0 )
        X->s = -1;

cleanup:

    blcrypto_suite_mpi_free( &T );

    return( ret );
}

/*
 * Helper to write the digits high-order first.
 */
static int mpi_write_hlp( blcrypto_suite_mpi *X, int radix,
                          char **p, const size_t buflen )
{
    int ret;
    blcrypto_suite_mpi_uint r;
    size_t length = 0;
    char *p_end = *p + buflen;

    do
    {
        if( length >= buflen )
        {
            return( BLCRYPTO_SUITE_ERR_MPI_BUFFER_TOO_SMALL );
        }

        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_mod_int( &r, X, radix ) );
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_div_int( X, NULL, X, radix ) );
        /*
         * Write the residue in the current position, as an ASCII character.
         */
        if( r < 0xA )
            *(--p_end) = (char)( '0' + r );
        else
            *(--p_end) = (char)( 'A' + ( r - 0xA ) );

        length++;
    } while( blcrypto_suite_mpi_cmp_int( X, 0 ) != 0 );

    memmove( *p, p_end, length );
    *p += length;

cleanup:

    return( ret );
}

/*
 * Export into an ASCII string
 */
int blcrypto_suite_mpi_write_string( const blcrypto_suite_mpi *X, int radix,
                              char *buf, size_t buflen, size_t *olen )
{
    int ret = 0;
    size_t n;
    char *p;
    blcrypto_suite_mpi T;
    MPI_VALIDATE_RET( X    != NULL );
    MPI_VALIDATE_RET( olen != NULL );
    MPI_VALIDATE_RET( buflen == 0 || buf != NULL );

    if( radix < 2 || radix > 16 )
        return( BLCRYPTO_SUITE_ERR_MPI_BAD_INPUT_DATA );

    n = blcrypto_suite_mpi_bitlen( X ); /* Number of bits necessary to present `n`. */
    if( radix >=  4 ) n >>= 1;   /* Number of 4-adic digits necessary to present
                                  * `n`. If radix > 4, this might be a strict
                                  * overapproximation of the number of
                                  * radix-adic digits needed to present `n`. */
    if( radix >= 16 ) n >>= 1;   /* Number of hexadecimal digits necessary to
                                  * present `n`. */

    n += 1; /* Terminating null byte */
    n += 1; /* Compensate for the divisions above, which round down `n`
             * in case it's not even. */
    n += 1; /* Potential '-'-sign. */
    n += ( n & 1 ); /* Make n even to have enough space for hexadecimal writing,
                     * which always uses an even number of hex-digits. */

    if( buflen < n )
    {
        *olen = n;
        return( BLCRYPTO_SUITE_ERR_MPI_BUFFER_TOO_SMALL );
    }

    p = buf;
    blcrypto_suite_mpi_init( &T );

    if( X->s == -1 )
    {
        *p++ = '-';
        buflen--;
    }

    if( radix == 16 )
    {
        int c;
        size_t i, j, k;

        for( i = X->n, k = 0; i > 0; i-- )
        {
            for( j = ciL; j > 0; j-- )
            {
                c = ( X->p[i - 1] >> ( ( j - 1 ) << 3) ) & 0xFF;

                if( c == 0 && k == 0 && ( i + j ) != 2 )
                    continue;

                *(p++) = "0123456789ABCDEF" [c / 16];
                *(p++) = "0123456789ABCDEF" [c % 16];
                k = 1;
            }
        }
    }
    else
    {
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_copy( &T, X ) );

        if( T.s == -1 )
            T.s = 1;

        BLCRYPTO_SUITE_MPI_CHK( mpi_write_hlp( &T, radix, &p, buflen ) );
    }

    *p++ = '\0';
    *olen = p - buf;

cleanup:

    blcrypto_suite_mpi_free( &T );

    return( ret );
}

#if defined(BLCRYPTO_SUITE_FS_IO)
/*
 * Read X from an opened file
 */
int blcrypto_suite_mpi_read_file( blcrypto_suite_mpi *X, int radix, FILE *fin )
{
    blcrypto_suite_mpi_uint d;
    size_t slen;
    char *p;
    /*
     * Buffer should have space for (short) label and decimal formatted MPI,
     * newline characters and '\0'
     */
    char s[ BLCRYPTO_SUITE_MPI_RW_BUFFER_SIZE ];

    MPI_VALIDATE_RET( X   != NULL );
    MPI_VALIDATE_RET( fin != NULL );

    if( radix < 2 || radix > 16 )
        return( BLCRYPTO_SUITE_ERR_MPI_BAD_INPUT_DATA );

    memset( s, 0, sizeof( s ) );
    if( fgets( s, sizeof( s ) - 1, fin ) == NULL )
        return( BLCRYPTO_SUITE_ERR_MPI_FILE_IO_ERROR );

    slen = strlen( s );
    if( slen == sizeof( s ) - 2 )
        return( BLCRYPTO_SUITE_ERR_MPI_BUFFER_TOO_SMALL );

    if( slen > 0 && s[slen - 1] == '\n' ) { slen--; s[slen] = '\0'; }
    if( slen > 0 && s[slen - 1] == '\r' ) { slen--; s[slen] = '\0'; }

    p = s + slen;
    while( p-- > s )
        if( mpi_get_digit( &d, radix, *p ) != 0 )
            break;

    return( blcrypto_suite_mpi_read_string( X, radix, p + 1 ) );
}

/*
 * Write X into an opened file (or stdout if fout == NULL)
 */
int blcrypto_suite_mpi_write_file( const char *p, const blcrypto_suite_mpi *X, int radix, FILE *fout )
{
    int ret;
    size_t n, slen, plen;
    /*
     * Buffer should have space for (short) label and decimal formatted MPI,
     * newline characters and '\0'
     */
    char s[ BLCRYPTO_SUITE_MPI_RW_BUFFER_SIZE ];
    MPI_VALIDATE_RET( X != NULL );

    if( radix < 2 || radix > 16 )
        return( BLCRYPTO_SUITE_ERR_MPI_BAD_INPUT_DATA );

    memset( s, 0, sizeof( s ) );

    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_write_string( X, radix, s, sizeof( s ) - 2, &n ) );

    if( p == NULL ) p = "";

    plen = strlen( p );
    slen = strlen( s );
    s[slen++] = '\r';
    s[slen++] = '\n';

    if( fout != NULL )
    {
        if( fwrite( p, 1, plen, fout ) != plen ||
            fwrite( s, 1, slen, fout ) != slen )
            return( BLCRYPTO_SUITE_ERR_MPI_FILE_IO_ERROR );
    }
    else
        blcrypto_suite_printf( "%s%s", p, s );

cleanup:

    return( ret );
}
#endif /* BLCRYPTO_SUITE_FS_IO */


/* Convert a big-endian byte array aligned to the size of blcrypto_suite_mpi_uint
 * into the storage form used by blcrypto_suite_mpi. */

static blcrypto_suite_mpi_uint mpi_uint_bigendian_to_host_c( blcrypto_suite_mpi_uint x )
{
    uint8_t i;
    unsigned char *x_ptr;
    blcrypto_suite_mpi_uint tmp = 0;

    for( i = 0, x_ptr = (unsigned char*) &x; i < ciL; i++, x_ptr++ )
    {
        tmp <<= CHAR_BIT;
        tmp |= (blcrypto_suite_mpi_uint) *x_ptr;
    }

    return( tmp );
}

static blcrypto_suite_mpi_uint mpi_uint_bigendian_to_host( blcrypto_suite_mpi_uint x )
{
#if defined(__BYTE_ORDER__)

/* Nothing to do on bigendian systems. */
#if ( __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__ )
    return( x );
#endif /* __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__ */

#if ( __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ )

/* For GCC and Clang, have builtins for byte swapping. */
#if defined(__GNUC__) && defined(__GNUC_PREREQ)
#if __GNUC_PREREQ(4,3)
#define have_bswap
#endif
#endif

#if defined(__clang__) && defined(__has_builtin)
#if __has_builtin(__builtin_bswap32)  &&                 \
    __has_builtin(__builtin_bswap64)
#define have_bswap
#endif
#endif

#if defined(have_bswap)
    /* The compiler is hopefully able to statically evaluate this! */
    switch( sizeof(blcrypto_suite_mpi_uint) )
    {
        case 4:
            return( __builtin_bswap32(x) );
        case 8:
            return( __builtin_bswap64(x) );
    }
#endif
#endif /* __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ */
#endif /* __BYTE_ORDER__ */

    /* Fall back to C-based reordering if we don't know the byte order
     * or we couldn't use a compiler-specific builtin. */
    return( mpi_uint_bigendian_to_host_c( x ) );
}

static void mpi_bigendian_to_host( blcrypto_suite_mpi_uint * const p, size_t limbs )
{
    blcrypto_suite_mpi_uint *cur_limb_left;
    blcrypto_suite_mpi_uint *cur_limb_right;
    if( limbs == 0 )
        return;

    /*
     * Traverse limbs and
     * - adapt byte-order in each limb
     * - swap the limbs themselves.
     * For that, simultaneously traverse the limbs from left to right
     * and from right to left, as long as the left index is not bigger
     * than the right index (it's not a problem if limbs is odd and the
     * indices coincide in the last iteration).
     */
    for( cur_limb_left = p, cur_limb_right = p + ( limbs - 1 );
         cur_limb_left <= cur_limb_right;
         cur_limb_left++, cur_limb_right-- )
    {
        blcrypto_suite_mpi_uint tmp;
        /* Note that if cur_limb_left == cur_limb_right,
         * this code effectively swaps the bytes only once. */
        tmp             = mpi_uint_bigendian_to_host( *cur_limb_left  );
        *cur_limb_left  = mpi_uint_bigendian_to_host( *cur_limb_right );
        *cur_limb_right = tmp;
    }
}

/*
 * Import X from unsigned binary data, big endian
 */
int blcrypto_suite_mpi_read_binary( blcrypto_suite_mpi *X, const unsigned char *buf, size_t buflen )
{
    int ret;
    size_t const limbs    = CHARS_TO_LIMBS( buflen );
    size_t const overhead = ( limbs * ciL ) - buflen;
    unsigned char *Xp;

    MPI_VALIDATE_RET( X != NULL );
    MPI_VALIDATE_RET( buflen == 0 || buf != NULL );

    /* Ensure that target MPI has exactly the necessary number of limbs */
    if( X->n != limbs )
    {
        blcrypto_suite_mpi_free( X );
        blcrypto_suite_mpi_init( X );
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_grow( X, limbs ) );
    }
    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_lset( X, 0 ) );

    /* Avoid calling `memcpy` with NULL source argument,
     * even if buflen is 0. */
    if( buf != NULL )
    {
        Xp = (unsigned char*) X->p;
        memcpy( Xp + overhead, buf, buflen );

        mpi_bigendian_to_host( X->p, limbs );
    }

cleanup:

    return( ret );
}

/*
 * Export X into unsigned binary data, big endian
 */
int blcrypto_suite_mpi_write_binary( const blcrypto_suite_mpi *X,
                              unsigned char *buf, size_t buflen )
{
    size_t stored_bytes;
    size_t bytes_to_copy;
    unsigned char *p;
    size_t i;

    MPI_VALIDATE_RET( X != NULL );
    MPI_VALIDATE_RET( buflen == 0 || buf != NULL );

    stored_bytes = X->n * ciL;

    if( stored_bytes < buflen )
    {
        /* There is enough space in the output buffer. Write initial
         * null bytes and record the position at which to start
         * writing the significant bytes. In this case, the execution
         * trace of this function does not depend on the value of the
         * number. */
        bytes_to_copy = stored_bytes;
        p = buf + buflen - stored_bytes;
        memset( buf, 0, buflen - stored_bytes );
    }
    else
    {
        /* The output buffer is smaller than the allocated size of X.
         * However X may fit if its leading bytes are zero. */
        bytes_to_copy = buflen;
        p = buf;
        for( i = bytes_to_copy; i < stored_bytes; i++ )
        {
            if( GET_BYTE( X, i ) != 0 )
                return( BLCRYPTO_SUITE_ERR_MPI_BUFFER_TOO_SMALL );
        }
    }

    for( i = 0; i < bytes_to_copy; i++ )
        p[bytes_to_copy - i - 1] = GET_BYTE( X, i );

    return( 0 );
}

/*
 * Left-shift: X <<= count
 */
int blcrypto_suite_mpi_shift_l( blcrypto_suite_mpi *X, size_t count )
{
    int ret;
    size_t i, v0, t1;
    blcrypto_suite_mpi_uint r0 = 0, r1;
    MPI_VALIDATE_RET( X != NULL );

    v0 = count / (biL    );
    t1 = count & (biL - 1);

    i = blcrypto_suite_mpi_bitlen( X ) + count;

    if( X->n * biL < i )
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_grow( X, BITS_TO_LIMBS( i ) ) );

    ret = 0;

    /*
     * shift by count / limb_size
     */
    if( v0 > 0 )
    {
        for( i = X->n; i > v0; i-- )
            X->p[i - 1] = X->p[i - v0 - 1];

        for( ; i > 0; i-- )
            X->p[i - 1] = 0;
    }

    /*
     * shift by count % limb_size
     */
    if( t1 > 0 )
    {
        for( i = v0; i < X->n; i++ )
        {
            r1 = X->p[i] >> (biL - t1);
            X->p[i] <<= t1;
            X->p[i] |= r0;
            r0 = r1;
        }
    }

cleanup:

    return( ret );
}

/*
 * Right-shift: X >>= count
 */
int blcrypto_suite_mpi_shift_r( blcrypto_suite_mpi *X, size_t count )
{
    size_t i, v0, v1;
    blcrypto_suite_mpi_uint r0 = 0, r1;
    MPI_VALIDATE_RET( X != NULL );

    v0 = count /  biL;
    v1 = count & (biL - 1);

    if( v0 > X->n || ( v0 == X->n && v1 > 0 ) )
        return blcrypto_suite_mpi_lset( X, 0 );

    /*
     * shift by count / limb_size
     */
    if( v0 > 0 )
    {
        for( i = 0; i < X->n - v0; i++ )
            X->p[i] = X->p[i + v0];

        for( ; i < X->n; i++ )
            X->p[i] = 0;
    }

    /*
     * shift by count % limb_size
     */
    if( v1 > 0 )
    {
        for( i = X->n; i > 0; i-- )
        {
            r1 = X->p[i - 1] << (biL - v1);
            X->p[i - 1] >>= v1;
            X->p[i - 1] |= r0;
            r0 = r1;
        }
    }

    return( 0 );
}

/*
 * Compare unsigned values
 */
int blcrypto_suite_mpi_cmp_abs( const blcrypto_suite_mpi *X, const blcrypto_suite_mpi *Y )
{
    size_t i, j;
    MPI_VALIDATE_RET( X != NULL );
    MPI_VALIDATE_RET( Y != NULL );

    for( i = X->n; i > 0; i-- )
        if( X->p[i - 1] != 0 )
            break;

    for( j = Y->n; j > 0; j-- )
        if( Y->p[j - 1] != 0 )
            break;

    if( i == 0 && j == 0 )
        return( 0 );

    if( i > j ) return(  1 );
    if( j > i ) return( -1 );

    for( ; i > 0; i-- )
    {
        if( X->p[i - 1] > Y->p[i - 1] ) return(  1 );
        if( X->p[i - 1] < Y->p[i - 1] ) return( -1 );
    }

    return( 0 );
}

/*
 * Compare signed values
 */
int blcrypto_suite_mpi_cmp_mpi( const blcrypto_suite_mpi *X, const blcrypto_suite_mpi *Y )
{
    size_t i, j;
    MPI_VALIDATE_RET( X != NULL );
    MPI_VALIDATE_RET( Y != NULL );

    for( i = X->n; i > 0; i-- )
        if( X->p[i - 1] != 0 )
            break;

    for( j = Y->n; j > 0; j-- )
        if( Y->p[j - 1] != 0 )
            break;

    if( i == 0 && j == 0 )
        return( 0 );

    if( i > j ) return(  X->s );
    if( j > i ) return( -Y->s );

    if( X->s > 0 && Y->s < 0 ) return(  1 );
    if( Y->s > 0 && X->s < 0 ) return( -1 );

    for( ; i > 0; i-- )
    {
        if( X->p[i - 1] > Y->p[i - 1] ) return(  X->s );
        if( X->p[i - 1] < Y->p[i - 1] ) return( -X->s );
    }

    return( 0 );
}

/** Decide if an integer is less than the other, without branches.
 *
 * \param x         First integer.
 * \param y         Second integer.
 *
 * \return          1 if \p x is less than \p y, 0 otherwise
 */
static unsigned ct_lt_mpi_uint( const blcrypto_suite_mpi_uint x,
        const blcrypto_suite_mpi_uint y )
{
    blcrypto_suite_mpi_uint ret;
    blcrypto_suite_mpi_uint cond;

    /*
     * Check if the most significant bits (MSB) of the operands are different.
     */
    cond = ( x ^ y );
    /*
     * If the MSB are the same then the difference x-y will be negative (and
     * have its MSB set to 1 during conversion to unsigned) if and only if x<y.
     */
    ret = ( x - y ) & ~cond;
    /*
     * If the MSB are different, then the operand with the MSB of 1 is the
     * bigger. (That is if y has MSB of 1, then x<y is true and it is false if
     * the MSB of y is 0.)
     */
    ret |= y & cond;


    ret = ret >> ( biL - 1 );

    return (unsigned) ret;
}

/*
 * Compare signed values in constant time
 */
int blcrypto_suite_mpi_lt_mpi_ct( const blcrypto_suite_mpi *X, const blcrypto_suite_mpi *Y,
        unsigned *ret )
{
    size_t i;
    /* The value of any of these variables is either 0 or 1 at all times. */
    unsigned cond, done, X_is_negative, Y_is_negative;

    MPI_VALIDATE_RET( X != NULL );
    MPI_VALIDATE_RET( Y != NULL );
    MPI_VALIDATE_RET( ret != NULL );

    if( X->n != Y->n )
        return BLCRYPTO_SUITE_ERR_MPI_BAD_INPUT_DATA;

    /*
     * Set sign_N to 1 if N >= 0, 0 if N < 0.
     * We know that N->s == 1 if N >= 0 and N->s == -1 if N < 0.
     */
    X_is_negative = ( X->s & 2 ) >> 1;
    Y_is_negative = ( Y->s & 2 ) >> 1;

    /*
     * If the signs are different, then the positive operand is the bigger.
     * That is if X is negative (X_is_negative == 1), then X < Y is true and it
     * is false if X is positive (X_is_negative == 0).
     */
    cond = ( X_is_negative ^ Y_is_negative );
    *ret = cond & X_is_negative;

    /*
     * This is a constant-time function. We might have the result, but we still
     * need to go through the loop. Record if we have the result already.
     */
    done = cond;

    for( i = X->n; i > 0; i-- )
    {
        /*
         * If Y->p[i - 1] < X->p[i - 1] then X < Y is true if and only if both
         * X and Y are negative.
         *
         * Again even if we can make a decision, we just mark the result and
         * the fact that we are done and continue looping.
         */
        cond = ct_lt_mpi_uint( Y->p[i - 1], X->p[i - 1] );
        *ret |= cond & ( 1 - done ) & X_is_negative;
        done |= cond;

        /*
         * If X->p[i - 1] < Y->p[i - 1] then X < Y is true if and only if both
         * X and Y are positive.
         *
         * Again even if we can make a decision, we just mark the result and
         * the fact that we are done and continue looping.
         */
        cond = ct_lt_mpi_uint( X->p[i - 1], Y->p[i - 1] );
        *ret |= cond & ( 1 - done ) & ( 1 - X_is_negative );
        done |= cond;
    }

    return( 0 );
}

/*
 * Compare signed values
 */
int blcrypto_suite_mpi_cmp_int( const blcrypto_suite_mpi *X, blcrypto_suite_mpi_sint z )
{
    blcrypto_suite_mpi Y;
    blcrypto_suite_mpi_uint p[1];
    MPI_VALIDATE_RET( X != NULL );

    *p  = ( z < 0 ) ? -z : z;
    Y.s = ( z < 0 ) ? -1 : 1;
    Y.n = 1;
    Y.p = p;

    return( blcrypto_suite_mpi_cmp_mpi( X, &Y ) );
}

/*
 * Unsigned addition: X = |A| + |B|  (HAC 14.7)
 */
int blcrypto_suite_mpi_add_abs( blcrypto_suite_mpi *X, const blcrypto_suite_mpi *A, const blcrypto_suite_mpi *B )
{
    int ret;
    size_t i, j;
    blcrypto_suite_mpi_uint *o, *p, c, tmp;
    MPI_VALIDATE_RET( X != NULL );
    MPI_VALIDATE_RET( A != NULL );
    MPI_VALIDATE_RET( B != NULL );

    if( X == B )
    {
        const blcrypto_suite_mpi *T = A; A = X; B = T;
    }

    if( X != A )
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_copy( X, A ) );

    /*
     * X should always be positive as a result of unsigned additions.
     */
    X->s = 1;

    for( j = B->n; j > 0; j-- )
        if( B->p[j - 1] != 0 )
            break;

    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_grow( X, j ) );

    o = B->p; p = X->p; c = 0;

    /*
     * tmp is used because it might happen that p == o
     */
    for( i = 0; i < j; i++, o++, p++ )
    {
        tmp= *o;
        *p +=  c; c  = ( *p <  c );
        *p += tmp; c += ( *p < tmp );
    }

    while( c != 0 )
    {
        if( i >= X->n )
        {
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_grow( X, i + 1 ) );
            p = X->p + i;
        }

        *p += c; c = ( *p < c ); i++; p++;
    }

cleanup:

    return( ret );
}

/**
 * Helper for blcrypto_suite_mpi subtraction.
 *
 * Calculate d - s where d and s have the same size.
 * This function operates modulo (2^ciL)^n and returns the carry
 * (1 if there was a wraparound, i.e. if `d < s`, and 0 otherwise).
 *
 * \param n             Number of limbs of \p d and \p s.
 * \param[in,out] d     On input, the left operand.
 *                      On output, the result of the subtraction:
 * \param[in] s         The right operand.
 *
 * \return              1 if `d < s`.
 *                      0 if `d >= s`.
 */
static blcrypto_suite_mpi_uint mpi_sub_hlp( size_t n,
                                     blcrypto_suite_mpi_uint *d,
                                     const blcrypto_suite_mpi_uint *s )
{
    size_t i;
    blcrypto_suite_mpi_uint c, z;

    for( i = c = 0; i < n; i++, s++, d++ )
    {
        z = ( *d <  c );     *d -=  c;
        c = ( *d < *s ) + z; *d -= *s;
    }

    return( c );
}

/*
 * Unsigned subtraction: X = |A| - |B|  (HAC 14.9, 14.10)
 */
int blcrypto_suite_mpi_sub_abs( blcrypto_suite_mpi *X, const blcrypto_suite_mpi *A, const blcrypto_suite_mpi *B )
{
    blcrypto_suite_mpi TB;
    int ret;
    size_t n;
    blcrypto_suite_mpi_uint carry;
    MPI_VALIDATE_RET( X != NULL );
    MPI_VALIDATE_RET( A != NULL );
    MPI_VALIDATE_RET( B != NULL );

    blcrypto_suite_mpi_init( &TB );

    if( X == B )
    {
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_copy( &TB, B ) );
        B = &TB;
    }

    if( X != A )
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_copy( X, A ) );

    /*
     * X should always be positive as a result of unsigned subtractions.
     */
    X->s = 1;

    ret = 0;

    for( n = B->n; n > 0; n-- )
        if( B->p[n - 1] != 0 )
            break;
    if( n > A->n )
    {
        /* B >= (2^ciL)^n > A */
        ret = BLCRYPTO_SUITE_ERR_MPI_NEGATIVE_VALUE;
        goto cleanup;
    }

    carry = mpi_sub_hlp( n, X->p, B->p );
    if( carry != 0 )
    {
        /* Propagate the carry to the first nonzero limb of X. */
        for( ; n < X->n && X->p[n] == 0; n++ )
            --X->p[n];
        /* If we ran out of space for the carry, it means that the result
         * is negative. */
        if( n == X->n )
        {
            ret = BLCRYPTO_SUITE_ERR_MPI_NEGATIVE_VALUE;
            goto cleanup;
        }
        --X->p[n];
    }

cleanup:

    blcrypto_suite_mpi_free( &TB );

    return( ret );
}

/*
 * Signed addition: X = A + B
 */
int blcrypto_suite_mpi_add_mpi( blcrypto_suite_mpi *X, const blcrypto_suite_mpi *A, const blcrypto_suite_mpi *B )
{
    int ret, s;
    MPI_VALIDATE_RET( X != NULL );
    MPI_VALIDATE_RET( A != NULL );
    MPI_VALIDATE_RET( B != NULL );

    s = A->s;
    if( A->s * B->s < 0 )
    {
        if( blcrypto_suite_mpi_cmp_abs( A, B ) >= 0 )
        {
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_sub_abs( X, A, B ) );
            X->s =  s;
        }
        else
        {
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_sub_abs( X, B, A ) );
            X->s = -s;
        }
    }
    else
    {
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_add_abs( X, A, B ) );
        X->s = s;
    }

cleanup:

    return( ret );
}

/*
 * Signed subtraction: X = A - B
 */
int blcrypto_suite_mpi_sub_mpi( blcrypto_suite_mpi *X, const blcrypto_suite_mpi *A, const blcrypto_suite_mpi *B )
{
    int ret, s;
    MPI_VALIDATE_RET( X != NULL );
    MPI_VALIDATE_RET( A != NULL );
    MPI_VALIDATE_RET( B != NULL );

    s = A->s;
    if( A->s * B->s > 0 )
    {
        if( blcrypto_suite_mpi_cmp_abs( A, B ) >= 0 )
        {
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_sub_abs( X, A, B ) );
            X->s =  s;
        }
        else
        {
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_sub_abs( X, B, A ) );
            X->s = -s;
        }
    }
    else
    {
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_add_abs( X, A, B ) );
        X->s = s;
    }

cleanup:

    return( ret );
}

/*
 * Signed addition: X = A + b
 */
int blcrypto_suite_mpi_add_int( blcrypto_suite_mpi *X, const blcrypto_suite_mpi *A, blcrypto_suite_mpi_sint b )
{
    blcrypto_suite_mpi _B;
    blcrypto_suite_mpi_uint p[1];
    MPI_VALIDATE_RET( X != NULL );
    MPI_VALIDATE_RET( A != NULL );

    p[0] = ( b < 0 ) ? -b : b;
    _B.s = ( b < 0 ) ? -1 : 1;
    _B.n = 1;
    _B.p = p;

    return( blcrypto_suite_mpi_add_mpi( X, A, &_B ) );
}

/*
 * Signed subtraction: X = A - b
 */
int blcrypto_suite_mpi_sub_int( blcrypto_suite_mpi *X, const blcrypto_suite_mpi *A, blcrypto_suite_mpi_sint b )
{
    blcrypto_suite_mpi _B;
    blcrypto_suite_mpi_uint p[1];
    MPI_VALIDATE_RET( X != NULL );
    MPI_VALIDATE_RET( A != NULL );

    p[0] = ( b < 0 ) ? -b : b;
    _B.s = ( b < 0 ) ? -1 : 1;
    _B.n = 1;
    _B.p = p;

    return( blcrypto_suite_mpi_sub_mpi( X, A, &_B ) );
}

/*
 * Helper for blcrypto_suite_mpi multiplication
 */
static
#if defined(__APPLE__) && defined(__arm__)
/*
 * Apple LLVM version 4.2 (clang-425.0.24) (based on LLVM 3.2svn)
 * appears to need this to prevent bad ARM code generation at -O3.
 */
__attribute__ ((noinline))
#endif
void mpi_mul_hlp( size_t i, blcrypto_suite_mpi_uint *s, blcrypto_suite_mpi_uint *d, blcrypto_suite_mpi_uint b )
{
    blcrypto_suite_mpi_uint c = 0, t = 0;

#if defined(MULADDC_HUIT)
    for( ; i >= 8; i -= 8 )
    {
        MULADDC_INIT
        MULADDC_HUIT
        MULADDC_STOP
    }

    for( ; i > 0; i-- )
    {
        MULADDC_INIT
        MULADDC_CORE
        MULADDC_STOP
    }
#else /* MULADDC_HUIT */
    for( ; i >= 16; i -= 16 )
    {
        MULADDC_INIT
        MULADDC_CORE   MULADDC_CORE
        MULADDC_CORE   MULADDC_CORE
        MULADDC_CORE   MULADDC_CORE
        MULADDC_CORE   MULADDC_CORE

        MULADDC_CORE   MULADDC_CORE
        MULADDC_CORE   MULADDC_CORE
        MULADDC_CORE   MULADDC_CORE
        MULADDC_CORE   MULADDC_CORE
        MULADDC_STOP
    }

    for( ; i >= 8; i -= 8 )
    {
        MULADDC_INIT
        MULADDC_CORE   MULADDC_CORE
        MULADDC_CORE   MULADDC_CORE

        MULADDC_CORE   MULADDC_CORE
        MULADDC_CORE   MULADDC_CORE
        MULADDC_STOP
    }

    for( ; i > 0; i-- )
    {
        MULADDC_INIT
        MULADDC_CORE
        MULADDC_STOP
    }
#endif /* MULADDC_HUIT */

    t++;

    do {
        *d += c; c = ( *d < c ); d++;
    }
    while( c != 0 );
}

/*
 * Baseline multiplication: X = A * B  (HAC 14.12)
 */
int blcrypto_suite_mpi_mul_mpi( blcrypto_suite_mpi *X, const blcrypto_suite_mpi *A, const blcrypto_suite_mpi *B )
{
    int ret;
    size_t i, j;
    blcrypto_suite_mpi TA, TB;
    int result_is_zero = 0;
    MPI_VALIDATE_RET( X != NULL );
    MPI_VALIDATE_RET( A != NULL );
    MPI_VALIDATE_RET( B != NULL );

    blcrypto_suite_mpi_init( &TA ); blcrypto_suite_mpi_init( &TB );

    if( X == A ) { BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_copy( &TA, A ) ); A = &TA; }
    if( X == B ) { BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_copy( &TB, B ) ); B = &TB; }

    for( i = A->n; i > 0; i-- )
        if( A->p[i - 1] != 0 )
            break;
    if( i == 0 )
        result_is_zero = 1;

    for( j = B->n; j > 0; j-- )
        if( B->p[j - 1] != 0 )
            break;
    if( j == 0 )
        result_is_zero = 1;

    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_grow( X, i + j ) );
    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_lset( X, 0 ) );

    for( ; j > 0; j-- )
        mpi_mul_hlp( i, A->p, X->p + j - 1, B->p[j - 1] );

    /* If the result is 0, we don't shortcut the operation, which reduces
     * but does not eliminate side channels leaking the zero-ness. We do
     * need to take care to set the sign bit properly since the library does
     * not fully support an MPI object with a value of 0 and s == -1. */
    if( result_is_zero )
        X->s = 1;
    else
        X->s = A->s * B->s;

cleanup:

    blcrypto_suite_mpi_free( &TB ); blcrypto_suite_mpi_free( &TA );

    return( ret );
}

/*
 * Baseline multiplication: X = A * b
 */
int blcrypto_suite_mpi_mul_int( blcrypto_suite_mpi *X, const blcrypto_suite_mpi *A, blcrypto_suite_mpi_uint b )
{
    blcrypto_suite_mpi _B;
    blcrypto_suite_mpi_uint p[1];
    MPI_VALIDATE_RET( X != NULL );
    MPI_VALIDATE_RET( A != NULL );

    _B.s = 1;
    _B.n = 1;
    _B.p = p;
    p[0] = b;

    return( blcrypto_suite_mpi_mul_mpi( X, A, &_B ) );
}

/*
 * Unsigned integer divide - double blcrypto_suite_mpi_uint dividend, u1/u0, and
 * blcrypto_suite_mpi_uint divisor, d
 */
static blcrypto_suite_mpi_uint blcrypto_suite_int_div_int( blcrypto_suite_mpi_uint u1,
            blcrypto_suite_mpi_uint u0, blcrypto_suite_mpi_uint d, blcrypto_suite_mpi_uint *r )
{
#if defined(BLCRYPTO_SUITE_HAVE_UDBL)
    blcrypto_suite_t_udbl dividend, quotient;
#else
    const blcrypto_suite_mpi_uint radix = (blcrypto_suite_mpi_uint) 1 << biH;
    const blcrypto_suite_mpi_uint uint_halfword_mask = ( (blcrypto_suite_mpi_uint) 1 << biH ) - 1;
    blcrypto_suite_mpi_uint d0, d1, q0, q1, rAX, r0, quotient;
    blcrypto_suite_mpi_uint u0_msw, u0_lsw;
    size_t s;
#endif

    /*
     * Check for overflow
     */
    if( 0 == d || u1 >= d )
    {
        if (r != NULL) *r = ~0;

        return ( ~0 );
    }

#if defined(BLCRYPTO_SUITE_HAVE_UDBL)
    dividend  = (blcrypto_suite_t_udbl) u1 << biL;
    dividend |= (blcrypto_suite_t_udbl) u0;
    quotient = dividend / d;
    if( quotient > ( (blcrypto_suite_t_udbl) 1 << biL ) - 1 )
        quotient = ( (blcrypto_suite_t_udbl) 1 << biL ) - 1;

    if( r != NULL )
        *r = (blcrypto_suite_mpi_uint)( dividend - (quotient * d ) );

    return (blcrypto_suite_mpi_uint) quotient;
#else

    /*
     * Algorithm D, Section 4.3.1 - The Art of Computer Programming
     *   Vol. 2 - Seminumerical Algorithms, Knuth
     */

    /*
     * Normalize the divisor, d, and dividend, u0, u1
     */
    s = blcrypto_suite_clz( d );
    d = d << s;

    u1 = u1 << s;
    u1 |= ( u0 >> ( biL - s ) ) & ( -(blcrypto_suite_mpi_sint)s >> ( biL - 1 ) );
    u0 =  u0 << s;

    d1 = d >> biH;
    d0 = d & uint_halfword_mask;

    u0_msw = u0 >> biH;
    u0_lsw = u0 & uint_halfword_mask;

    /*
     * Find the first quotient and remainder
     */
    q1 = u1 / d1;
    r0 = u1 - d1 * q1;

    while( q1 >= radix || ( q1 * d0 > radix * r0 + u0_msw ) )
    {
        q1 -= 1;
        r0 += d1;

        if ( r0 >= radix ) break;
    }

    rAX = ( u1 * radix ) + ( u0_msw - q1 * d );
    q0 = rAX / d1;
    r0 = rAX - q0 * d1;

    while( q0 >= radix || ( q0 * d0 > radix * r0 + u0_lsw ) )
    {
        q0 -= 1;
        r0 += d1;

        if ( r0 >= radix ) break;
    }

    if (r != NULL)
        *r = ( rAX * radix + u0_lsw - q0 * d ) >> s;

    quotient = q1 * radix + q0;

    return quotient;
#endif
}

/*
 * Division by blcrypto_suite_mpi: A = Q * B + R  (HAC 14.20)
 */
int blcrypto_suite_mpi_div_mpi( blcrypto_suite_mpi *Q, blcrypto_suite_mpi *R, const blcrypto_suite_mpi *A,
                         const blcrypto_suite_mpi *B )
{
    int ret;
    size_t i, n, t, k;
    blcrypto_suite_mpi X, Y, Z, T1, T2;
    MPI_VALIDATE_RET( A != NULL );
    MPI_VALIDATE_RET( B != NULL );

    if( blcrypto_suite_mpi_cmp_int( B, 0 ) == 0 )
        return( BLCRYPTO_SUITE_ERR_MPI_DIVISION_BY_ZERO );

    blcrypto_suite_mpi_init( &X ); blcrypto_suite_mpi_init( &Y ); blcrypto_suite_mpi_init( &Z );
    blcrypto_suite_mpi_init( &T1 ); blcrypto_suite_mpi_init( &T2 );

    if( blcrypto_suite_mpi_cmp_abs( A, B ) < 0 )
    {
        if( Q != NULL ) BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_lset( Q, 0 ) );
        if( R != NULL ) BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_copy( R, A ) );
        return( 0 );
    }

    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_copy( &X, A ) );
    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_copy( &Y, B ) );
    X.s = Y.s = 1;

    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_grow( &Z, A->n + 2 ) );
    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_lset( &Z,  0 ) );
    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_grow( &T1, 2 ) );
    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_grow( &T2, 3 ) );

    k = blcrypto_suite_mpi_bitlen( &Y ) % biL;
    if( k < biL - 1 )
    {
        k = biL - 1 - k;
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_shift_l( &X, k ) );
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_shift_l( &Y, k ) );
    }
    else k = 0;

    n = X.n - 1;
    t = Y.n - 1;
    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_shift_l( &Y, biL * ( n - t ) ) );

    while( blcrypto_suite_mpi_cmp_mpi( &X, &Y ) >= 0 )
    {
        Z.p[n - t]++;
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_sub_mpi( &X, &X, &Y ) );
    }
    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_shift_r( &Y, biL * ( n - t ) ) );

    for( i = n; i > t ; i-- )
    {
        if( X.p[i] >= Y.p[t] )
            Z.p[i - t - 1] = ~0;
        else
        {
            Z.p[i - t - 1] = blcrypto_suite_int_div_int( X.p[i], X.p[i - 1],
                                                            Y.p[t], NULL);
        }

        Z.p[i - t - 1]++;
        do
        {
            Z.p[i - t - 1]--;

            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_lset( &T1, 0 ) );
            T1.p[0] = ( t < 1 ) ? 0 : Y.p[t - 1];
            T1.p[1] = Y.p[t];
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_mul_int( &T1, &T1, Z.p[i - t - 1] ) );

            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_lset( &T2, 0 ) );
            T2.p[0] = ( i < 2 ) ? 0 : X.p[i - 2];
            T2.p[1] = ( i < 1 ) ? 0 : X.p[i - 1];
            T2.p[2] = X.p[i];
        }
        while( blcrypto_suite_mpi_cmp_mpi( &T1, &T2 ) > 0 );

        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_mul_int( &T1, &Y, Z.p[i - t - 1] ) );
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_shift_l( &T1,  biL * ( i - t - 1 ) ) );
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_sub_mpi( &X, &X, &T1 ) );

        if( blcrypto_suite_mpi_cmp_int( &X, 0 ) < 0 )
        {
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_copy( &T1, &Y ) );
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_shift_l( &T1, biL * ( i - t - 1 ) ) );
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_add_mpi( &X, &X, &T1 ) );
            Z.p[i - t - 1]--;
        }
    }

    if( Q != NULL )
    {
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_copy( Q, &Z ) );
        Q->s = A->s * B->s;
    }

    if( R != NULL )
    {
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_shift_r( &X, k ) );
        X.s = A->s;
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_copy( R, &X ) );

        if( blcrypto_suite_mpi_cmp_int( R, 0 ) == 0 )
            R->s = 1;
    }

cleanup:

    blcrypto_suite_mpi_free( &X ); blcrypto_suite_mpi_free( &Y ); blcrypto_suite_mpi_free( &Z );
    blcrypto_suite_mpi_free( &T1 ); blcrypto_suite_mpi_free( &T2 );

    return( ret );
}

/*
 * Division by int: A = Q * b + R
 */
int blcrypto_suite_mpi_div_int( blcrypto_suite_mpi *Q, blcrypto_suite_mpi *R,
                         const blcrypto_suite_mpi *A,
                         blcrypto_suite_mpi_sint b )
{
    blcrypto_suite_mpi _B;
    blcrypto_suite_mpi_uint p[1];
    MPI_VALIDATE_RET( A != NULL );

    p[0] = ( b < 0 ) ? -b : b;
    _B.s = ( b < 0 ) ? -1 : 1;
    _B.n = 1;
    _B.p = p;

    return( blcrypto_suite_mpi_div_mpi( Q, R, A, &_B ) );
}

/*
 * Modulo: R = A mod B
 */
int blcrypto_suite_mpi_mod_mpi( blcrypto_suite_mpi *R, const blcrypto_suite_mpi *A, const blcrypto_suite_mpi *B )
{
    int ret;
    MPI_VALIDATE_RET( R != NULL );
    MPI_VALIDATE_RET( A != NULL );
    MPI_VALIDATE_RET( B != NULL );

    if( blcrypto_suite_mpi_cmp_int( B, 0 ) < 0 )
        return( BLCRYPTO_SUITE_ERR_MPI_NEGATIVE_VALUE );

    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_div_mpi( NULL, R, A, B ) );

    while( blcrypto_suite_mpi_cmp_int( R, 0 ) < 0 )
      BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_add_mpi( R, R, B ) );

    while( blcrypto_suite_mpi_cmp_mpi( R, B ) >= 0 )
      BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_sub_mpi( R, R, B ) );

cleanup:

    return( ret );
}

/*
 * Modulo: r = A mod b
 */
int blcrypto_suite_mpi_mod_int( blcrypto_suite_mpi_uint *r, const blcrypto_suite_mpi *A, blcrypto_suite_mpi_sint b )
{
    size_t i;
    blcrypto_suite_mpi_uint x, y, z;
    MPI_VALIDATE_RET( r != NULL );
    MPI_VALIDATE_RET( A != NULL );

    if( b == 0 )
        return( BLCRYPTO_SUITE_ERR_MPI_DIVISION_BY_ZERO );

    if( b < 0 )
        return( BLCRYPTO_SUITE_ERR_MPI_NEGATIVE_VALUE );

    /*
     * handle trivial cases
     */
    if( b == 1 )
    {
        *r = 0;
        return( 0 );
    }

    if( b == 2 )
    {
        *r = A->p[0] & 1;
        return( 0 );
    }

    /*
     * general case
     */
    for( i = A->n, y = 0; i > 0; i-- )
    {
        x  = A->p[i - 1];
        y  = ( y << biH ) | ( x >> biH );
        z  = y / b;
        y -= z * b;

        x <<= biH;
        y  = ( y << biH ) | ( x >> biH );
        z  = y / b;
        y -= z * b;
    }

    /*
     * If A is negative, then the current y represents a negative value.
     * Flipping it to the positive side.
     */
    if( A->s < 0 && y != 0 )
        y = b - y;

    *r = y;

    return( 0 );
}

/*
 * Fast Montgomery initialization (thanks to Tom St Denis)
 */
static void mpi_montg_init( blcrypto_suite_mpi_uint *mm, const blcrypto_suite_mpi *N )
{
    blcrypto_suite_mpi_uint x, m0 = N->p[0];
    unsigned int i;

    x  = m0;
    x += ( ( m0 + 2 ) & 4 ) << 1;

    for( i = biL; i >= 8; i /= 2 )
        x *= ( 2 - ( m0 * x ) );

    *mm = ~x + 1;
}

/** Montgomery multiplication: A = A * B * R^-1 mod N  (HAC 14.36)
 *
 * \param[in,out]   A   One of the numbers to multiply.
 *                      It must have at least as many limbs as N
 *                      (A->n >= N->n), and any limbs beyond n are ignored.
 *                      On successful completion, A contains the result of
 *                      the multiplication A * B * R^-1 mod N where
 *                      R = (2^ciL)^n.
 * \param[in]       B   One of the numbers to multiply.
 *                      It must be nonzero and must not have more limbs than N
 *                      (B->n <= N->n).
 * \param[in]       N   The modulo. N must be odd.
 * \param           mm  The value calculated by `mpi_montg_init(&mm, N)`.
 *                      This is -N^-1 mod 2^ciL.
 * \param[in,out]   T   A bignum for temporary storage.
 *                      It must be at least twice the limb size of N plus 2
 *                      (T->n >= 2 * (N->n + 1)).
 *                      Its initial content is unused and
 *                      its final content is indeterminate.
 *                      Note that unlike the usual convention in the library
 *                      for `const blcrypto_suite_mpi*`, the content of T can change.
 */
static void mpi_montmul( blcrypto_suite_mpi *A, const blcrypto_suite_mpi *B, const blcrypto_suite_mpi *N, blcrypto_suite_mpi_uint mm,
                         const blcrypto_suite_mpi *T )
{
    size_t i, n, m;
    blcrypto_suite_mpi_uint u0, u1, *d;

    memset( T->p, 0, T->n * ciL );

    d = T->p;
    n = N->n;
    m = ( B->n < n ) ? B->n : n;

    for( i = 0; i < n; i++ )
    {
        /*
         * T = (T + u0*B + u1*N) / 2^biL
         */
        u0 = A->p[i];
        u1 = ( d[0] + u0 * B->p[0] ) * mm;

        mpi_mul_hlp( m, B->p, d, u0 );
        mpi_mul_hlp( n, N->p, d, u1 );

        *d++ = u0; d[n + 1] = 0;
    }

    /* At this point, d is either the desired result or the desired result
     * plus N. We now potentially subtract N, avoiding leaking whether the
     * subtraction is performed through side channels. */

    /* Copy the n least significant limbs of d to A, so that
     * A = d if d < N (recall that N has n limbs). */
    memcpy( A->p, d, n * ciL );
    /* If d >= N then we want to set A to d - N. To prevent timing attacks,
     * do the calculation without using conditional tests. */
    /* Set d to d0 + (2^biL)^n - N where d0 is the current value of d. */
    d[n] += 1;
    d[n] -= mpi_sub_hlp( n, d, N->p );
    /* If d0 < N then d < (2^biL)^n
     * so d[n] == 0 and we want to keep A as it is.
     * If d0 >= N then d >= (2^biL)^n, and d <= (2^biL)^n + N < 2 * (2^biL)^n
     * so d[n] == 1 and we want to set A to the result of the subtraction
     * which is d - (2^biL)^n, i.e. the n least significant limbs of d.
     * This exactly corresponds to a conditional assignment. */
    mpi_safe_cond_assign( n, A->p, d, (unsigned char) d[n] );
}

/*
 * Montgomery reduction: A = A * R^-1 mod N
 *
 * See mpi_montmul() regarding constraints and guarantees on the parameters.
 */
static void mpi_montred( blcrypto_suite_mpi *A, const blcrypto_suite_mpi *N,
                         blcrypto_suite_mpi_uint mm, const blcrypto_suite_mpi *T )
{
    blcrypto_suite_mpi_uint z = 1;
    blcrypto_suite_mpi U;

    U.n = U.s = (int) z;
    U.p = &z;

    mpi_montmul( A, &U, N, mm, T );
}

/*
 * Constant-flow boolean "equal" comparison:
 * return x == y
 *
 * This function can be used to write constant-time code by replacing branches
 * with bit operations - it can be used in conjunction with
 * blcrypto_suite_ssl_cf_mask_from_bit().
 *
 * This function is implemented without using comparison operators, as those
 * might be translated to branches by some compilers on some platforms.
 */
static size_t blcrypto_suite_mpi_cf_bool_eq( size_t x, size_t y )
{
    /* diff = 0 if x == y, non-zero otherwise */
    const size_t diff = x ^ y;

    /* MSVC has a warning about unary minus on unsigned integer types,
     * but this is well-defined and precisely what we want to do here. */
#if defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 4146 )
#endif

    /* diff_msb's most significant bit is equal to x != y */
    const size_t diff_msb = ( diff | (size_t) -diff );

#if defined(_MSC_VER)
#pragma warning( pop )
#endif

    /* diff1 = (x != y) ? 1 : 0 */
    const size_t diff1 = diff_msb >> ( sizeof( diff_msb ) * 8 - 1 );

    return( 1 ^ diff1 );
}

/**
 * Select an MPI from a table without leaking the index.
 *
 * This is functionally equivalent to blcrypto_suite_mpi_copy(R, T[idx]) except it
 * reads the entire table in order to avoid leaking the value of idx to an
 * attacker able to observe memory access patterns.
 *
 * \param[out] R        Where to write the selected MPI.
 * \param[in] T         The table to read from.
 * \param[in] T_size    The number of elements in the table.
 * \param[in] idx       The index of the element to select;
 *                      this must satisfy 0 <= idx < T_size.
 *
 * \return \c 0 on success, or a negative error code.
 */
static int mpi_select( blcrypto_suite_mpi *R, const blcrypto_suite_mpi *T, size_t T_size, size_t idx )
{
    int ret = BLCRYPTO_SUITE_ERR_MPI_BAD_INPUT_DATA;
    size_t i;

    for( i = 0; i < T_size; i++ )
    {
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_safe_cond_assign( R, &T[i],
                        (unsigned char) blcrypto_suite_mpi_cf_bool_eq( i, idx ) ) );
    }

cleanup:
    return( ret );
}

/*
 * Sliding-window exponentiation: X = A^E mod N  (HAC 14.85)
 */
int blcrypto_suite_mpi_exp_mod( blcrypto_suite_mpi *X, const blcrypto_suite_mpi *A,
                         const blcrypto_suite_mpi *E, const blcrypto_suite_mpi *N,
                         blcrypto_suite_mpi *_RR )
{
    int ret;
    size_t wbits, wsize, one = 1;
    size_t i, j, nblimbs;
    size_t bufsize, nbits;
    blcrypto_suite_mpi_uint ei, mm, state;
    blcrypto_suite_mpi RR, T, W[ 1 << BLCRYPTO_SUITE_MPI_WINDOW_SIZE ], WW, Apos;
    int neg;

    MPI_VALIDATE_RET( X != NULL );
    MPI_VALIDATE_RET( A != NULL );
    MPI_VALIDATE_RET( E != NULL );
    MPI_VALIDATE_RET( N != NULL );

    if( blcrypto_suite_mpi_cmp_int( N, 0 ) <= 0 || ( N->p[0] & 1 ) == 0 )
        return( BLCRYPTO_SUITE_ERR_MPI_BAD_INPUT_DATA );

    if( blcrypto_suite_mpi_cmp_int( E, 0 ) < 0 )
        return( BLCRYPTO_SUITE_ERR_MPI_BAD_INPUT_DATA );

    if( blcrypto_suite_mpi_bitlen( E ) > BLCRYPTO_SUITE_MPI_MAX_BITS ||
        blcrypto_suite_mpi_bitlen( N ) > BLCRYPTO_SUITE_MPI_MAX_BITS )
        return ( BLCRYPTO_SUITE_ERR_MPI_BAD_INPUT_DATA );

    /*
     * Init temps and window size
     */
    mpi_montg_init( &mm, N );
    blcrypto_suite_mpi_init( &RR ); blcrypto_suite_mpi_init( &T );
    blcrypto_suite_mpi_init( &Apos );
    blcrypto_suite_mpi_init( &WW );
    memset( W, 0, sizeof( W ) );

    i = blcrypto_suite_mpi_bitlen( E );

    wsize = ( i > 671 ) ? 6 : ( i > 239 ) ? 5 :
            ( i >  79 ) ? 4 : ( i >  23 ) ? 3 : 1;

#if( BLCRYPTO_SUITE_MPI_WINDOW_SIZE < 6 )
    if( wsize > BLCRYPTO_SUITE_MPI_WINDOW_SIZE )
        wsize = BLCRYPTO_SUITE_MPI_WINDOW_SIZE;
#endif

    j = N->n + 1;
    /* All W[i] and X must have at least N->n limbs for the mpi_montmul()
     * and mpi_montred() calls later. Here we ensure that W[1] and X are
     * large enough, and later we'll grow other W[i] to the same length.
     * They must not be shrunk midway through this function!
     */
    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_grow( X, j ) );
    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_grow( &W[1],  j ) );
    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_grow( &T, j * 2 ) );

    /*
     * Compensate for negative A (and correct at the end)
     */
    neg = ( A->s == -1 );
    if( neg )
    {
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_copy( &Apos, A ) );
        Apos.s = 1;
        A = &Apos;
    }

    /*
     * If 1st call, pre-compute R^2 mod N
     */
    if( _RR == NULL || _RR->p == NULL )
    {
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_lset( &RR, 1 ) );
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_shift_l( &RR, N->n * 2 * biL ) );
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_mod_mpi( &RR, &RR, N ) );

        if( _RR != NULL )
            memcpy( _RR, &RR, sizeof( blcrypto_suite_mpi ) );
    }
    else
        memcpy( &RR, _RR, sizeof( blcrypto_suite_mpi ) );

    /*
     * W[1] = A * R^2 * R^-1 mod N = A * R mod N
     */
    if( blcrypto_suite_mpi_cmp_mpi( A, N ) >= 0 )
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_mod_mpi( &W[1], A, N ) );
    else
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_copy( &W[1], A ) );
    /* Re-grow W[1] if necessary. This should be only necessary in one corner
     * case: when A == 0 represented with A.n == 0, blcrypto_suite_mpi_copy shrinks
     * W[1] to 0 limbs. */
    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_grow( &W[1], N->n +1 ) );

    mpi_montmul( &W[1], &RR, N, mm, &T );

    /*
     * X = R^2 * R^-1 mod N = R mod N
     */
    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_copy( X, &RR ) );
    mpi_montred( X, N, mm, &T );

    if( wsize > 1 )
    {
        /*
         * W[1 << (wsize - 1)] = W[1] ^ (wsize - 1)
         */
        j =  one << ( wsize - 1 );

        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_grow( &W[j], N->n + 1 ) );
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_copy( &W[j], &W[1]    ) );

        for( i = 0; i < wsize - 1; i++ )
            mpi_montmul( &W[j], &W[j], N, mm, &T );

        /*
         * W[i] = W[i - 1] * W[1]
         */
        for( i = j + 1; i < ( one << wsize ); i++ )
        {
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_grow( &W[i], N->n + 1 ) );
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_copy( &W[i], &W[i - 1] ) );

            mpi_montmul( &W[i], &W[1], N, mm, &T );
        }
    }

    nblimbs = E->n;
    bufsize = 0;
    nbits   = 0;
    wbits   = 0;
    state   = 0;

    while( 1 )
    {
        if( bufsize == 0 )
        {
            if( nblimbs == 0 )
                break;

            nblimbs--;

            bufsize = sizeof( blcrypto_suite_mpi_uint ) << 3;
        }

        bufsize--;

        ei = (E->p[nblimbs] >> bufsize) & 1;

        /*
         * skip leading 0s
         */
        if( ei == 0 && state == 0 )
            continue;

        if( ei == 0 && state == 1 )
        {
            /*
             * out of window, square X
             */
            mpi_montmul( X, X, N, mm, &T );
            continue;
        }

        /*
         * add ei to current window
         */
        state = 2;

        nbits++;
        wbits |= ( ei << ( wsize - nbits ) );

        if( nbits == wsize )
        {
            /*
             * X = X^wsize R^-1 mod N
             */
            for( i = 0; i < wsize; i++ )
                mpi_montmul( X, X, N, mm, &T );

            /*
             * X = X * W[wbits] R^-1 mod N
             */
            BLCRYPTO_SUITE_MPI_CHK( mpi_select( &WW, W, (size_t) 1 << wsize, wbits ) );
            mpi_montmul( X, &WW, N, mm, &T );

            state--;
            nbits = 0;
            wbits = 0;
        }
    }

    /*
     * process the remaining bits
     */
    for( i = 0; i < nbits; i++ )
    {
        mpi_montmul( X, X, N, mm, &T );

        wbits <<= 1;

        if( ( wbits & ( one << wsize ) ) != 0 )
            mpi_montmul( X, &W[1], N, mm, &T );
    }

    /*
     * X = A^E * R * R^-1 mod N = A^E mod N
     */
    mpi_montred( X, N, mm, &T );

    if( neg && E->n != 0 && ( E->p[0] & 1 ) != 0 )
    {
        X->s = -1;
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_add_mpi( X, N, X ) );
    }

cleanup:

    for( i = ( one << ( wsize - 1 ) ); i < ( one << wsize ); i++ )
        blcrypto_suite_mpi_free( &W[i] );

    blcrypto_suite_mpi_free( &W[1] ); blcrypto_suite_mpi_free( &T ); blcrypto_suite_mpi_free( &Apos );
    blcrypto_suite_mpi_free( &WW );

    if( _RR == NULL || _RR->p == NULL )
        blcrypto_suite_mpi_free( &RR );

    return( ret );
}

/*
 * Greatest common divisor: G = gcd(A, B)  (HAC 14.54)
 */
int blcrypto_suite_mpi_gcd( blcrypto_suite_mpi *G, const blcrypto_suite_mpi *A, const blcrypto_suite_mpi *B )
{
    int ret;
    size_t lz, lzt;
    blcrypto_suite_mpi TG, TA, TB;

    MPI_VALIDATE_RET( G != NULL );
    MPI_VALIDATE_RET( A != NULL );
    MPI_VALIDATE_RET( B != NULL );

    blcrypto_suite_mpi_init( &TG ); blcrypto_suite_mpi_init( &TA ); blcrypto_suite_mpi_init( &TB );

    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_copy( &TA, A ) );
    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_copy( &TB, B ) );

    lz = blcrypto_suite_mpi_lsb( &TA );
    lzt = blcrypto_suite_mpi_lsb( &TB );

    /* The loop below gives the correct result when A==0 but not when B==0.
     * So have a special case for B==0. Leverage the fact that we just
     * calculated the lsb and lsb(B)==0 iff B is odd or 0 to make the test
     * slightly more efficient than cmp_int(). */
    if( lzt == 0 && blcrypto_suite_mpi_get_bit( &TB, 0 ) == 0 )
    {
        ret = blcrypto_suite_mpi_copy( G, A );
        goto cleanup;
    }

    if( lzt < lz )
        lz = lzt;

    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_shift_r( &TA, lz ) );
    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_shift_r( &TB, lz ) );

    TA.s = TB.s = 1;

    /* We mostly follow the procedure described in HAC 14.54, but with some
     * minor differences:
     * - Sequences of multiplications or divisions by 2 are grouped into a
     *   single shift operation.
     * - The procedure in HAC assumes that 0 < TB <= TA.
     *     - The condition TB <= TA is not actually necessary for correctness.
     *       TA and TB have symmetric roles except for the loop termination
     *       condition, and the shifts at the beginning of the loop body
     *       remove any significance from the ordering of TA vs TB before
     *       the shifts.
     *     - If TA = 0, the loop goes through 0 iterations and the result is
     *       correctly TB.
     *     - The case TB = 0 was short-circuited above.
     *
     * For the correctness proof below, decompose the original values of
     * A and B as
     *   A = sa * 2^a * A' with A'=0 or A' odd, and sa = +-1
     *   B = sb * 2^b * B' with B'=0 or B' odd, and sb = +-1
     * Then gcd(A, B) = 2^{min(a,b)} * gcd(A',B'),
     * and gcd(A',B') is odd or 0.
     *
     * At the beginning, we have TA = |A|/2^a and TB = |B|/2^b.
     * The code maintains the following invariant:
     *     gcd(A,B) = 2^k * gcd(TA,TB) for some k   (I)
     */

    /* Proof that the loop terminates:
     * At each iteration, either the right-shift by 1 is made on a nonzero
     * value and the nonnegative integer bitlen(TA) + bitlen(TB) decreases
     * by at least 1, or the right-shift by 1 is made on zero and then
     * TA becomes 0 which ends the loop (TB cannot be 0 if it is right-shifted
     * since in that case TB is calculated from TB-TA with the condition TB>TA).
     */
    while( blcrypto_suite_mpi_cmp_int( &TA, 0 ) != 0 )
    {
        /* Divisions by 2 preserve the invariant (I). */
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_shift_r( &TA, blcrypto_suite_mpi_lsb( &TA ) ) );
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_shift_r( &TB, blcrypto_suite_mpi_lsb( &TB ) ) );

        /* Set either TA or TB to |TA-TB|/2. Since TA and TB are both odd,
         * TA-TB is even so the division by 2 has an integer result.
         * Invariant (I) is preserved since any odd divisor of both TA and TB
         * also divides |TA-TB|/2, and any odd divisor of both TA and |TA-TB|/2
         * also divides TB, and any odd divisior of both TB and |TA-TB|/2 also
         * divides TA.
         */
        if( blcrypto_suite_mpi_cmp_mpi( &TA, &TB ) >= 0 )
        {
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_sub_abs( &TA, &TA, &TB ) );
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_shift_r( &TA, 1 ) );
        }
        else
        {
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_sub_abs( &TB, &TB, &TA ) );
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_shift_r( &TB, 1 ) );
        }
        /* Note that one of TA or TB is still odd. */
    }

    /* By invariant (I), gcd(A,B) = 2^k * gcd(TA,TB) for some k.
     * At the loop exit, TA = 0, so gcd(TA,TB) = TB.
     * - If there was at least one loop iteration, then one of TA or TB is odd,
     *   and TA = 0, so TB is odd and gcd(TA,TB) = gcd(A',B'). In this case,
     *   lz = min(a,b) so gcd(A,B) = 2^lz * TB.
     * - If there was no loop iteration, then A was 0, and gcd(A,B) = B.
     *   In this case, lz = 0 and B = TB so gcd(A,B) = B = 2^lz * TB as well.
     */

    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_shift_l( &TB, lz ) );
    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_copy( G, &TB ) );

cleanup:

    blcrypto_suite_mpi_free( &TG ); blcrypto_suite_mpi_free( &TA ); blcrypto_suite_mpi_free( &TB );

    return( ret );
}

/*
 * Fill X with size bytes of random.
 *
 * Use a temporary bytes representation to make sure the result is the same
 * regardless of the platform endianness (useful when f_rng is actually
 * deterministic, eg for tests).
 */
int blcrypto_suite_mpi_fill_random( blcrypto_suite_mpi *X, size_t size,
                     int (*f_rng)(void *, unsigned char *, size_t),
                     void *p_rng )
{
    int ret;
    size_t const limbs = CHARS_TO_LIMBS( size );
    size_t const overhead = ( limbs * ciL ) - size;
    unsigned char *Xp;

    MPI_VALIDATE_RET( X     != NULL );
    MPI_VALIDATE_RET( f_rng != NULL );

    /* Ensure that target MPI has exactly the necessary number of limbs */
    if( X->n != limbs )
    {
        blcrypto_suite_mpi_free( X );
        blcrypto_suite_mpi_init( X );
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_grow( X, limbs ) );
    }
    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_lset( X, 0 ) );

    Xp = (unsigned char*) X->p;
    BLCRYPTO_SUITE_MPI_CHK( f_rng( p_rng, Xp + overhead, size ) );

    mpi_bigendian_to_host( X->p, limbs );

cleanup:
    return( ret );
}

/*
 * Modular inverse: X = A^-1 mod N  (HAC 14.61 / 14.64)
 */
int blcrypto_suite_mpi_inv_mod( blcrypto_suite_mpi *X, const blcrypto_suite_mpi *A, const blcrypto_suite_mpi *N )
{
    int ret;
    blcrypto_suite_mpi G, TA, TU, U1, U2, TB, TV, V1, V2;
    MPI_VALIDATE_RET( X != NULL );
    MPI_VALIDATE_RET( A != NULL );
    MPI_VALIDATE_RET( N != NULL );

    if( blcrypto_suite_mpi_cmp_int( N, 1 ) <= 0 )
        return( BLCRYPTO_SUITE_ERR_MPI_BAD_INPUT_DATA );

    blcrypto_suite_mpi_init( &TA ); blcrypto_suite_mpi_init( &TU ); blcrypto_suite_mpi_init( &U1 ); blcrypto_suite_mpi_init( &U2 );
    blcrypto_suite_mpi_init( &G ); blcrypto_suite_mpi_init( &TB ); blcrypto_suite_mpi_init( &TV );
    blcrypto_suite_mpi_init( &V1 ); blcrypto_suite_mpi_init( &V2 );

    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_gcd( &G, A, N ) );

    if( blcrypto_suite_mpi_cmp_int( &G, 1 ) != 0 )
    {
        ret = BLCRYPTO_SUITE_ERR_MPI_NOT_ACCEPTABLE;
        goto cleanup;
    }

    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_mod_mpi( &TA, A, N ) );
    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_copy( &TU, &TA ) );
    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_copy( &TB, N ) );
    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_copy( &TV, N ) );

    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_lset( &U1, 1 ) );
    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_lset( &U2, 0 ) );
    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_lset( &V1, 0 ) );
    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_lset( &V2, 1 ) );

    do
    {
        while( ( TU.p[0] & 1 ) == 0 )
        {
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_shift_r( &TU, 1 ) );

            if( ( U1.p[0] & 1 ) != 0 || ( U2.p[0] & 1 ) != 0 )
            {
                BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_add_mpi( &U1, &U1, &TB ) );
                BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_sub_mpi( &U2, &U2, &TA ) );
            }

            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_shift_r( &U1, 1 ) );
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_shift_r( &U2, 1 ) );
        }

        while( ( TV.p[0] & 1 ) == 0 )
        {
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_shift_r( &TV, 1 ) );

            if( ( V1.p[0] & 1 ) != 0 || ( V2.p[0] & 1 ) != 0 )
            {
                BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_add_mpi( &V1, &V1, &TB ) );
                BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_sub_mpi( &V2, &V2, &TA ) );
            }

            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_shift_r( &V1, 1 ) );
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_shift_r( &V2, 1 ) );
        }

        if( blcrypto_suite_mpi_cmp_mpi( &TU, &TV ) >= 0 )
        {
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_sub_mpi( &TU, &TU, &TV ) );
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_sub_mpi( &U1, &U1, &V1 ) );
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_sub_mpi( &U2, &U2, &V2 ) );
        }
        else
        {
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_sub_mpi( &TV, &TV, &TU ) );
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_sub_mpi( &V1, &V1, &U1 ) );
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_sub_mpi( &V2, &V2, &U2 ) );
        }
    }
    while( blcrypto_suite_mpi_cmp_int( &TU, 0 ) != 0 );

    while( blcrypto_suite_mpi_cmp_int( &V1, 0 ) < 0 )
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_add_mpi( &V1, &V1, N ) );

    while( blcrypto_suite_mpi_cmp_mpi( &V1, N ) >= 0 )
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_sub_mpi( &V1, &V1, N ) );

    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_copy( X, &V1 ) );

cleanup:

    blcrypto_suite_mpi_free( &TA ); blcrypto_suite_mpi_free( &TU ); blcrypto_suite_mpi_free( &U1 ); blcrypto_suite_mpi_free( &U2 );
    blcrypto_suite_mpi_free( &G ); blcrypto_suite_mpi_free( &TB ); blcrypto_suite_mpi_free( &TV );
    blcrypto_suite_mpi_free( &V1 ); blcrypto_suite_mpi_free( &V2 );

    return( ret );
}

#if defined(BLCRYPTO_SUITE_GENPRIME)

static const int small_prime[] =
{
        3,    5,    7,   11,   13,   17,   19,   23,
       29,   31,   37,   41,   43,   47,   53,   59,
       61,   67,   71,   73,   79,   83,   89,   97,
      101,  103,  107,  109,  113,  127,  131,  137,
      139,  149,  151,  157,  163,  167,  173,  179,
      181,  191,  193,  197,  199,  211,  223,  227,
      229,  233,  239,  241,  251,  257,  263,  269,
      271,  277,  281,  283,  293,  307,  311,  313,
      317,  331,  337,  347,  349,  353,  359,  367,
      373,  379,  383,  389,  397,  401,  409,  419,
      421,  431,  433,  439,  443,  449,  457,  461,
      463,  467,  479,  487,  491,  499,  503,  509,
      521,  523,  541,  547,  557,  563,  569,  571,
      577,  587,  593,  599,  601,  607,  613,  617,
      619,  631,  641,  643,  647,  653,  659,  661,
      673,  677,  683,  691,  701,  709,  719,  727,
      733,  739,  743,  751,  757,  761,  769,  773,
      787,  797,  809,  811,  821,  823,  827,  829,
      839,  853,  857,  859,  863,  877,  881,  883,
      887,  907,  911,  919,  929,  937,  941,  947,
      953,  967,  971,  977,  983,  991,  997, -103
};

/*
 * Small divisors test (X must be positive)
 *
 * Return values:
 * 0: no small factor (possible prime, more tests needed)
 * 1: certain prime
 * BLCRYPTO_SUITE_ERR_MPI_NOT_ACCEPTABLE: certain non-prime
 * other negative: error
 */
static int mpi_check_small_factors( const blcrypto_suite_mpi *X )
{
    int ret = 0;
    size_t i;
    blcrypto_suite_mpi_uint r;

    if( ( X->p[0] & 1 ) == 0 )
        return( BLCRYPTO_SUITE_ERR_MPI_NOT_ACCEPTABLE );

    for( i = 0; small_prime[i] > 0; i++ )
    {
        if( blcrypto_suite_mpi_cmp_int( X, small_prime[i] ) <= 0 )
            return( 1 );

        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_mod_int( &r, X, small_prime[i] ) );

        if( r == 0 )
            return( BLCRYPTO_SUITE_ERR_MPI_NOT_ACCEPTABLE );
    }

cleanup:
    return( ret );
}

/*
 * Miller-Rabin pseudo-primality test  (HAC 4.24)
 */
static int mpi_miller_rabin( const blcrypto_suite_mpi *X, size_t rounds,
                             int (*f_rng)(void *, unsigned char *, size_t),
                             void *p_rng )
{
    int ret, count;
    size_t i, j, k, s;
    blcrypto_suite_mpi W, R, T, A, RR;

    MPI_VALIDATE_RET( X     != NULL );
    MPI_VALIDATE_RET( f_rng != NULL );

    blcrypto_suite_mpi_init( &W ); blcrypto_suite_mpi_init( &R );
    blcrypto_suite_mpi_init( &T ); blcrypto_suite_mpi_init( &A );
    blcrypto_suite_mpi_init( &RR );

    /*
     * W = |X| - 1
     * R = W >> lsb( W )
     */
    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_sub_int( &W, X, 1 ) );
    s = blcrypto_suite_mpi_lsb( &W );
    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_copy( &R, &W ) );
    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_shift_r( &R, s ) );

    for( i = 0; i < rounds; i++ )
    {
        /*
         * pick a random A, 1 < A < |X| - 1
         */
        count = 0;
        do {
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_fill_random( &A, X->n * ciL, f_rng, p_rng ) );

            j = blcrypto_suite_mpi_bitlen( &A );
            k = blcrypto_suite_mpi_bitlen( &W );
            if (j > k) {
                A.p[A.n - 1] &= ( (blcrypto_suite_mpi_uint) 1 << ( k - ( A.n - 1 ) * biL - 1 ) ) - 1;
            }

            if (count++ > 30) {
                ret = BLCRYPTO_SUITE_ERR_MPI_NOT_ACCEPTABLE;
                goto cleanup;
            }

        } while ( blcrypto_suite_mpi_cmp_mpi( &A, &W ) >= 0 ||
                  blcrypto_suite_mpi_cmp_int( &A, 1 )  <= 0    );

        /*
         * A = A^R mod |X|
         */
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_exp_mod( &A, &A, &R, X, &RR ) );

        if( blcrypto_suite_mpi_cmp_mpi( &A, &W ) == 0 ||
            blcrypto_suite_mpi_cmp_int( &A,  1 ) == 0 )
            continue;

        j = 1;
        while( j < s && blcrypto_suite_mpi_cmp_mpi( &A, &W ) != 0 )
        {
            /*
             * A = A * A mod |X|
             */
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_mul_mpi( &T, &A, &A ) );
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_mod_mpi( &A, &T, X  ) );

            if( blcrypto_suite_mpi_cmp_int( &A, 1 ) == 0 )
                break;

            j++;
        }

        /*
         * not prime if A != |X| - 1 or A == 1
         */
        if( blcrypto_suite_mpi_cmp_mpi( &A, &W ) != 0 ||
            blcrypto_suite_mpi_cmp_int( &A,  1 ) == 0 )
        {
            ret = BLCRYPTO_SUITE_ERR_MPI_NOT_ACCEPTABLE;
            break;
        }
    }

cleanup:
    blcrypto_suite_mpi_free( &W ); blcrypto_suite_mpi_free( &R );
    blcrypto_suite_mpi_free( &T ); blcrypto_suite_mpi_free( &A );
    blcrypto_suite_mpi_free( &RR );

    return( ret );
}

/*
 * Pseudo-primality test: small factors, then Miller-Rabin
 */
int blcrypto_suite_mpi_is_prime_ext( const blcrypto_suite_mpi *X, int rounds,
                              int (*f_rng)(void *, unsigned char *, size_t),
                              void *p_rng )
{
    int ret;
    blcrypto_suite_mpi XX;
    MPI_VALIDATE_RET( X     != NULL );
    MPI_VALIDATE_RET( f_rng != NULL );

    XX.s = 1;
    XX.n = X->n;
    XX.p = X->p;

    if( blcrypto_suite_mpi_cmp_int( &XX, 0 ) == 0 ||
        blcrypto_suite_mpi_cmp_int( &XX, 1 ) == 0 )
        return( BLCRYPTO_SUITE_ERR_MPI_NOT_ACCEPTABLE );

    if( blcrypto_suite_mpi_cmp_int( &XX, 2 ) == 0 )
        return( 0 );

    if( ( ret = mpi_check_small_factors( &XX ) ) != 0 )
    {
        if( ret == 1 )
            return( 0 );

        return( ret );
    }

    return( mpi_miller_rabin( &XX, rounds, f_rng, p_rng ) );
}

#if !defined(BLCRYPTO_SUITE_DEPRECATED_REMOVED)
/*
 * Pseudo-primality test, error probability 2^-80
 */
int blcrypto_suite_mpi_is_prime( const blcrypto_suite_mpi *X,
                  int (*f_rng)(void *, unsigned char *, size_t),
                  void *p_rng )
{
    MPI_VALIDATE_RET( X     != NULL );
    MPI_VALIDATE_RET( f_rng != NULL );

    /*
     * In the past our key generation aimed for an error rate of at most
     * 2^-80. Since this function is deprecated, aim for the same certainty
     * here as well.
     */
    return( blcrypto_suite_mpi_is_prime_ext( X, 40, f_rng, p_rng ) );
}
#endif

/*
 * Prime number generation
 *
 * To generate an RSA key in a way recommended by FIPS 186-4, both primes must
 * be either 1024 bits or 1536 bits long, and flags must contain
 * BLCRYPTO_SUITE_MPI_GEN_PRIME_FLAG_LOW_ERR.
 */
int blcrypto_suite_mpi_gen_prime( blcrypto_suite_mpi *X, size_t nbits, int flags,
                   int (*f_rng)(void *, unsigned char *, size_t),
                   void *p_rng )
{
#ifdef BLCRYPTO_SUITE_HAVE_INT64
// ceil(2^63.5)
#define CEIL_MAXUINT_DIV_SQRT2 0xb504f333f9de6485ULL
#else
// ceil(2^31.5)
#define CEIL_MAXUINT_DIV_SQRT2 0xb504f334U
#endif
    int ret = BLCRYPTO_SUITE_ERR_MPI_NOT_ACCEPTABLE;
    size_t k, n;
    int rounds;
    blcrypto_suite_mpi_uint r;
    blcrypto_suite_mpi Y;

    MPI_VALIDATE_RET( X     != NULL );
    MPI_VALIDATE_RET( f_rng != NULL );

    if( nbits < 3 || nbits > BLCRYPTO_SUITE_MPI_MAX_BITS )
        return( BLCRYPTO_SUITE_ERR_MPI_BAD_INPUT_DATA );

    blcrypto_suite_mpi_init( &Y );

    n = BITS_TO_LIMBS( nbits );

    if( ( flags & BLCRYPTO_SUITE_MPI_GEN_PRIME_FLAG_LOW_ERR ) == 0 )
    {
        /*
         * 2^-80 error probability, number of rounds chosen per HAC, table 4.4
         */
        rounds = ( ( nbits >= 1300 ) ?  2 : ( nbits >=  850 ) ?  3 :
                   ( nbits >=  650 ) ?  4 : ( nbits >=  350 ) ?  8 :
                   ( nbits >=  250 ) ? 12 : ( nbits >=  150 ) ? 18 : 27 );
    }
    else
    {
        /*
         * 2^-100 error probability, number of rounds computed based on HAC,
         * fact 4.48
         */
        rounds = ( ( nbits >= 1450 ) ?  4 : ( nbits >=  1150 ) ?  5 :
                   ( nbits >= 1000 ) ?  6 : ( nbits >=   850 ) ?  7 :
                   ( nbits >=  750 ) ?  8 : ( nbits >=   500 ) ? 13 :
                   ( nbits >=  250 ) ? 28 : ( nbits >=   150 ) ? 40 : 51 );
    }

    while( 1 )
    {
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_fill_random( X, n * ciL, f_rng, p_rng ) );
        /* make sure generated number is at least (nbits-1)+0.5 bits (FIPS 186-4 §B.3.3 steps 4.4, 5.5) */
        if( X->p[n-1] < CEIL_MAXUINT_DIV_SQRT2 ) continue;

        k = n * biL;
        if( k > nbits ) BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_shift_r( X, k - nbits ) );
        X->p[0] |= 1;

        if( ( flags & BLCRYPTO_SUITE_MPI_GEN_PRIME_FLAG_DH ) == 0 )
        {
            ret = blcrypto_suite_mpi_is_prime_ext( X, rounds, f_rng, p_rng );

            if( ret != BLCRYPTO_SUITE_ERR_MPI_NOT_ACCEPTABLE )
                goto cleanup;
        }
        else
        {
            /*
             * An necessary condition for Y and X = 2Y + 1 to be prime
             * is X = 2 mod 3 (which is equivalent to Y = 2 mod 3).
             * Make sure it is satisfied, while keeping X = 3 mod 4
             */

            X->p[0] |= 2;

            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_mod_int( &r, X, 3 ) );
            if( r == 0 )
                BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_add_int( X, X, 8 ) );
            else if( r == 1 )
                BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_add_int( X, X, 4 ) );

            /* Set Y = (X-1) / 2, which is X / 2 because X is odd */
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_copy( &Y, X ) );
            BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_shift_r( &Y, 1 ) );

            while( 1 )
            {
                /*
                 * First, check small factors for X and Y
                 * before doing Miller-Rabin on any of them
                 */
                if( ( ret = mpi_check_small_factors(  X         ) ) == 0 &&
                    ( ret = mpi_check_small_factors( &Y         ) ) == 0 &&
                    ( ret = mpi_miller_rabin(  X, rounds, f_rng, p_rng  ) )
                                                                    == 0 &&
                    ( ret = mpi_miller_rabin( &Y, rounds, f_rng, p_rng  ) )
                                                                    == 0 )
                    goto cleanup;

                if( ret != BLCRYPTO_SUITE_ERR_MPI_NOT_ACCEPTABLE )
                    goto cleanup;

                /*
                 * Next candidates. We want to preserve Y = (X-1) / 2 and
                 * Y = 1 mod 2 and Y = 2 mod 3 (eq X = 3 mod 4 and X = 2 mod 3)
                 * so up Y by 6 and X by 12.
                 */
                BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_add_int(  X,  X, 12 ) );
                BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_add_int( &Y, &Y, 6  ) );
            }
        }
    }

cleanup:

    blcrypto_suite_mpi_free( &Y );

    return( ret );
}

#endif /* BLCRYPTO_SUITE_GENPRIME */

#if defined(BLCRYPTO_SUITE_SELF_TEST)

#define GCD_PAIR_COUNT  3

static const int gcd_pairs[GCD_PAIR_COUNT][3] =
{
    { 693, 609, 21 },
    { 1764, 868, 28 },
    { 768454923, 542167814, 1 }
};

/*
 * Checkup routine
 */
int blcrypto_suite_mpi_self_test( int verbose )
{
    int ret, i;
    blcrypto_suite_mpi A, E, N, X, Y, U, V;

    blcrypto_suite_mpi_init( &A ); blcrypto_suite_mpi_init( &E ); blcrypto_suite_mpi_init( &N ); blcrypto_suite_mpi_init( &X );
    blcrypto_suite_mpi_init( &Y ); blcrypto_suite_mpi_init( &U ); blcrypto_suite_mpi_init( &V );

    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_read_string( &A, 16,
        "EFE021C2645FD1DC586E69184AF4A31E" \
        "D5F53E93B5F123FA41680867BA110131" \
        "944FE7952E2517337780CB0DB80E61AA" \
        "E7C8DDC6C5C6AADEB34EB38A2F40D5E6" ) );

    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_read_string( &E, 16,
        "B2E7EFD37075B9F03FF989C7C5051C20" \
        "34D2A323810251127E7BF8625A4F49A5" \
        "F3E27F4DA8BD59C47D6DAABA4C8127BD" \
        "5B5C25763222FEFCCFC38B832366C29E" ) );

    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_read_string( &N, 16,
        "0066A198186C18C10B2F5ED9B522752A" \
        "9830B69916E535C8F047518A889A43A5" \
        "94B6BED27A168D31D4A52F88925AA8F5" ) );

    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_mul_mpi( &X, &A, &N ) );

    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_read_string( &U, 16,
        "602AB7ECA597A3D6B56FF9829A5E8B85" \
        "9E857EA95A03512E2BAE7391688D264A" \
        "A5663B0341DB9CCFD2C4C5F421FEC814" \
        "8001B72E848A38CAE1C65F78E56ABDEF" \
        "E12D3C039B8A02D6BE593F0BBBDA56F1" \
        "ECF677152EF804370C1A305CAF3B5BF1" \
        "30879B56C61DE584A0F53A2447A51E" ) );

    if( verbose != 0 )
        blcrypto_suite_printf( "  MPI test #1 (mul_mpi): " );

    if( blcrypto_suite_mpi_cmp_mpi( &X, &U ) != 0 )
    {
        if( verbose != 0 )
            blcrypto_suite_printf( "failed\n" );

        ret = 1;
        goto cleanup;
    }

    if( verbose != 0 )
        blcrypto_suite_printf( "passed\n" );

    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_div_mpi( &X, &Y, &A, &N ) );

    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_read_string( &U, 16,
        "256567336059E52CAE22925474705F39A94" ) );

    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_read_string( &V, 16,
        "6613F26162223DF488E9CD48CC132C7A" \
        "0AC93C701B001B092E4E5B9F73BCD27B" \
        "9EE50D0657C77F374E903CDFA4C642" ) );

    if( verbose != 0 )
        blcrypto_suite_printf( "  MPI test #2 (div_mpi): " );

    if( blcrypto_suite_mpi_cmp_mpi( &X, &U ) != 0 ||
        blcrypto_suite_mpi_cmp_mpi( &Y, &V ) != 0 )
    {
        if( verbose != 0 )
            blcrypto_suite_printf( "failed\n" );

        ret = 1;
        goto cleanup;
    }

    if( verbose != 0 )
        blcrypto_suite_printf( "passed\n" );

    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_exp_mod( &X, &A, &E, &N, NULL ) );

    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_read_string( &U, 16,
        "36E139AEA55215609D2816998ED020BB" \
        "BD96C37890F65171D948E9BC7CBAA4D9" \
        "325D24D6A3C12710F10A09FA08AB87" ) );

    if( verbose != 0 )
        blcrypto_suite_printf( "  MPI test #3 (exp_mod): " );

    if( blcrypto_suite_mpi_cmp_mpi( &X, &U ) != 0 )
    {
        if( verbose != 0 )
            blcrypto_suite_printf( "failed\n" );

        ret = 1;
        goto cleanup;
    }

    if( verbose != 0 )
        blcrypto_suite_printf( "passed\n" );

    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_inv_mod( &X, &A, &N ) );

    BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_read_string( &U, 16,
        "003A0AAEDD7E784FC07D8F9EC6E3BFD5" \
        "C3DBA76456363A10869622EAC2DD84EC" \
        "C5B8A74DAC4D09E03B5E0BE779F2DF61" ) );

    if( verbose != 0 )
        blcrypto_suite_printf( "  MPI test #4 (inv_mod): " );

    if( blcrypto_suite_mpi_cmp_mpi( &X, &U ) != 0 )
    {
        if( verbose != 0 )
            blcrypto_suite_printf( "failed\n" );

        ret = 1;
        goto cleanup;
    }

    if( verbose != 0 )
        blcrypto_suite_printf( "passed\n" );

    if( verbose != 0 )
        blcrypto_suite_printf( "  MPI test #5 (simple gcd): " );

    for( i = 0; i < GCD_PAIR_COUNT; i++ )
    {
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_lset( &X, gcd_pairs[i][0] ) );
        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_lset( &Y, gcd_pairs[i][1] ) );

        BLCRYPTO_SUITE_MPI_CHK( blcrypto_suite_mpi_gcd( &A, &X, &Y ) );

        if( blcrypto_suite_mpi_cmp_int( &A, gcd_pairs[i][2] ) != 0 )
        {
            if( verbose != 0 )
                blcrypto_suite_printf( "failed at %d\n", i );

            ret = 1;
            goto cleanup;
        }
    }

    if( verbose != 0 )
        blcrypto_suite_printf( "passed\n" );

cleanup:

    if( ret != 0 && verbose != 0 )
        blcrypto_suite_printf( "Unexpected error, return code = %08X\n", ret );

    blcrypto_suite_mpi_free( &A ); blcrypto_suite_mpi_free( &E ); blcrypto_suite_mpi_free( &N ); blcrypto_suite_mpi_free( &X );
    blcrypto_suite_mpi_free( &Y ); blcrypto_suite_mpi_free( &U ); blcrypto_suite_mpi_free( &V );

    if( verbose != 0 )
        blcrypto_suite_printf( "\n" );

    return( ret );
}

#endif /* BLCRYPTO_SUITE_SELF_TEST */

#endif /* BLCRYPTO_SUITE_BIGNUM_C */
