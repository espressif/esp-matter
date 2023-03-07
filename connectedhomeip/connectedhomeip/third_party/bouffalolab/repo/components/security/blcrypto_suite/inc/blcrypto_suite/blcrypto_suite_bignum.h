/**
 * \file bignum.h
 *
 * \brief Multi-precision integer library
 */
/*
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
#ifndef BLCRYPTO_SUITE_BIGNUM_H
#define BLCRYPTO_SUITE_BIGNUM_H

#if !defined(BLCRYPTO_SUITE_CONFIG_FILE)
#include "blcrypto_suite/blcrypto_suite_config.h"
#else
#include BLCRYPTO_SUITE_CONFIG_FILE
#endif

#include <stddef.h>
#include <stdint.h>

#if defined(BLCRYPTO_SUITE_FS_IO)
#include <stdio.h>
#endif

#define BLCRYPTO_SUITE_ERR_MPI_FILE_IO_ERROR                     -0x0002  /**< An error occurred while reading from or writing to a file. */
#define BLCRYPTO_SUITE_ERR_MPI_BAD_INPUT_DATA                    -0x0004  /**< Bad input parameters to function. */
#define BLCRYPTO_SUITE_ERR_MPI_INVALID_CHARACTER                 -0x0006  /**< There is an invalid character in the digit string. */
#define BLCRYPTO_SUITE_ERR_MPI_BUFFER_TOO_SMALL                  -0x0008  /**< The buffer is too small to write to. */
#define BLCRYPTO_SUITE_ERR_MPI_NEGATIVE_VALUE                    -0x000A  /**< The input arguments are negative or result in illegal output. */
#define BLCRYPTO_SUITE_ERR_MPI_DIVISION_BY_ZERO                  -0x000C  /**< The input argument for division is zero, which is not allowed. */
#define BLCRYPTO_SUITE_ERR_MPI_NOT_ACCEPTABLE                    -0x000E  /**< The input arguments are not acceptable. */
#define BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED                      -0x0010  /**< Memory allocation failed. */

#define BLCRYPTO_SUITE_MPI_CHK(f)       \
    do                           \
    {                            \
        if( ( ret = (f) ) != 0 ) \
            goto cleanup;        \
    } while( 0 )

/*
 * Maximum size MPIs are allowed to grow to in number of limbs.
 */
#define BLCRYPTO_SUITE_MPI_MAX_LIMBS                             10000

#if !defined(BLCRYPTO_SUITE_MPI_WINDOW_SIZE)
/*
 * Maximum window size used for modular exponentiation. Default: 6
 * Minimum value: 1. Maximum value: 6.
 *
 * Result is an array of ( 2 ** BLCRYPTO_SUITE_MPI_WINDOW_SIZE ) MPIs used
 * for the sliding window calculation. (So 64 by default)
 *
 * Reduction in size, reduces speed.
 */
#define BLCRYPTO_SUITE_MPI_WINDOW_SIZE                           6        /**< Maximum window size used. */
#endif /* !BLCRYPTO_SUITE_MPI_WINDOW_SIZE */

#if !defined(BLCRYPTO_SUITE_MPI_MAX_SIZE)
/*
 * Maximum size of MPIs allowed in bits and bytes for user-MPIs.
 * ( Default: 512 bytes => 4096 bits, Maximum tested: 2048 bytes => 16384 bits )
 *
 * Note: Calculations can temporarily result in larger MPIs. So the number
 * of limbs required (BLCRYPTO_SUITE_MPI_MAX_LIMBS) is higher.
 */
#define BLCRYPTO_SUITE_MPI_MAX_SIZE                              1024     /**< Maximum number of bytes for usable MPIs. */
#endif /* !BLCRYPTO_SUITE_MPI_MAX_SIZE */

#define BLCRYPTO_SUITE_MPI_MAX_BITS                              ( 8 * BLCRYPTO_SUITE_MPI_MAX_SIZE )    /**< Maximum number of bits for usable MPIs. */

/*
 * When reading from files with blcrypto_suite_mpi_read_file() and writing to files with
 * blcrypto_suite_mpi_write_file() the buffer should have space
 * for a (short) label, the MPI (in the provided radix), the newline
 * characters and the '\0'.
 *
 * By default we assume at least a 10 char label, a minimum radix of 10
 * (decimal) and a maximum of 4096 bit numbers (1234 decimal chars).
 * Autosized at compile time for at least a 10 char label, a minimum radix
 * of 10 (decimal) for a number of BLCRYPTO_SUITE_MPI_MAX_BITS size.
 *
 * This used to be statically sized to 1250 for a maximum of 4096 bit
 * numbers (1234 decimal chars).
 *
 * Calculate using the formula:
 *  BLCRYPTO_SUITE_MPI_RW_BUFFER_SIZE = ceil(BLCRYPTO_SUITE_MPI_MAX_BITS / ln(10) * ln(2)) +
 *                                LabelSize + 6
 */
#define BLCRYPTO_SUITE_MPI_MAX_BITS_SCALE100          ( 100 * BLCRYPTO_SUITE_MPI_MAX_BITS )
#define BLCRYPTO_SUITE_LN_2_DIV_LN_10_SCALE100                 332
#define BLCRYPTO_SUITE_MPI_RW_BUFFER_SIZE             ( ((BLCRYPTO_SUITE_MPI_MAX_BITS_SCALE100 + BLCRYPTO_SUITE_LN_2_DIV_LN_10_SCALE100 - 1) / BLCRYPTO_SUITE_LN_2_DIV_LN_10_SCALE100) + 10 + 6 )

/*
 * Define the base integer type, architecture-wise.
 *
 * 32 or 64-bit integer types can be forced regardless of the underlying
 * architecture by defining BLCRYPTO_SUITE_HAVE_INT32 or BLCRYPTO_SUITE_HAVE_INT64
 * respectively and undefining BLCRYPTO_SUITE_HAVE_ASM.
 *
 * Double-width integers (e.g. 128-bit in 64-bit architectures) can be
 * disabled by defining BLCRYPTO_SUITE_NO_UDBL_DIVISION.
 */
#if !defined(BLCRYPTO_SUITE_HAVE_INT32)
    #if defined(_MSC_VER) && defined(_M_AMD64)
        /* Always choose 64-bit when using MSC */
        #if !defined(BLCRYPTO_SUITE_HAVE_INT64)
            #define BLCRYPTO_SUITE_HAVE_INT64
        #endif /* !BLCRYPTO_SUITE_HAVE_INT64 */
        typedef  int64_t blcrypto_suite_mpi_sint;
        typedef uint64_t blcrypto_suite_mpi_uint;
    #elif defined(__GNUC__) && (                         \
        defined(__amd64__) || defined(__x86_64__)     || \
        defined(__ppc64__) || defined(__powerpc64__)  || \
        defined(__ia64__)  || defined(__alpha__)      || \
        ( defined(__sparc__) && defined(__arch64__) ) || \
        defined(__s390x__) || defined(__mips64) )
        #if !defined(BLCRYPTO_SUITE_HAVE_INT64)
            #define BLCRYPTO_SUITE_HAVE_INT64
        #endif /* BLCRYPTO_SUITE_HAVE_INT64 */
        typedef  int64_t blcrypto_suite_mpi_sint;
        typedef uint64_t blcrypto_suite_mpi_uint;
        #if !defined(BLCRYPTO_SUITE_NO_UDBL_DIVISION)
            /* blcrypto_suite_t_udbl defined as 128-bit unsigned int */
            typedef unsigned int blcrypto_suite_t_udbl __attribute__((mode(TI)));
            #define BLCRYPTO_SUITE_HAVE_UDBL
        #endif /* !BLCRYPTO_SUITE_NO_UDBL_DIVISION */
    #elif defined(__ARMCC_VERSION) && defined(__aarch64__)
        /*
         * __ARMCC_VERSION is defined for both armcc and armclang and
         * __aarch64__ is only defined by armclang when compiling 64-bit code
         */
        #if !defined(BLCRYPTO_SUITE_HAVE_INT64)
            #define BLCRYPTO_SUITE_HAVE_INT64
        #endif /* !BLCRYPTO_SUITE_HAVE_INT64 */
        typedef  int64_t blcrypto_suite_mpi_sint;
        typedef uint64_t blcrypto_suite_mpi_uint;
        #if !defined(BLCRYPTO_SUITE_NO_UDBL_DIVISION)
            /* blcrypto_suite_t_udbl defined as 128-bit unsigned int */
            typedef __uint128_t blcrypto_suite_t_udbl;
            #define BLCRYPTO_SUITE_HAVE_UDBL
        #endif /* !BLCRYPTO_SUITE_NO_UDBL_DIVISION */
    #elif defined(BLCRYPTO_SUITE_HAVE_INT64)
        /* Force 64-bit integers with unknown compiler */
        typedef  int64_t blcrypto_suite_mpi_sint;
        typedef uint64_t blcrypto_suite_mpi_uint;
    #endif
#endif /* !BLCRYPTO_SUITE_HAVE_INT32 */

#if !defined(BLCRYPTO_SUITE_HAVE_INT64)
    /* Default to 32-bit compilation */
    #if !defined(BLCRYPTO_SUITE_HAVE_INT32)
        #define BLCRYPTO_SUITE_HAVE_INT32
    #endif /* !BLCRYPTO_SUITE_HAVE_INT32 */
    typedef  int32_t blcrypto_suite_mpi_sint;
    typedef uint32_t blcrypto_suite_mpi_uint;
    #if !defined(BLCRYPTO_SUITE_NO_UDBL_DIVISION)
        typedef uint64_t blcrypto_suite_t_udbl;
        #define BLCRYPTO_SUITE_HAVE_UDBL
    #endif /* !BLCRYPTO_SUITE_NO_UDBL_DIVISION */
#endif /* !BLCRYPTO_SUITE_HAVE_INT64 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          MPI structure
 */
typedef struct blcrypto_suite_mpi
{
    int s;              /*!<  Sign: -1 if the mpi is negative, 1 otherwise */
    size_t n;           /*!<  total # of limbs  */
    blcrypto_suite_mpi_uint *p;          /*!<  pointer to limbs  */
}
blcrypto_suite_mpi;

/**
 * \brief           Initialize an MPI context.
 *
 *                  This makes the MPI ready to be set or freed,
 *                  but does not define a value for the MPI.
 *
 * \param X         The MPI context to initialize. This must not be \c NULL.
 */
void blcrypto_suite_mpi_init( blcrypto_suite_mpi *X );

/**
 * \brief          This function frees the components of an MPI context.
 *
 * \param X        The MPI context to be cleared. This may be \c NULL,
 *                 in which case this function is a no-op. If it is
 *                 not \c NULL, it must point to an initialized MPI.
 */
void blcrypto_suite_mpi_free( blcrypto_suite_mpi *X );

/**
 * \brief          Enlarge an MPI to the specified number of limbs.
 *
 * \note           This function does nothing if the MPI is
 *                 already large enough.
 *
 * \param X        The MPI to grow. It must be initialized.
 * \param nblimbs  The target number of limbs.
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED if memory allocation failed.
 * \return         Another negative error code on other kinds of failure.
 */
int blcrypto_suite_mpi_grow( blcrypto_suite_mpi *X, size_t nblimbs );

/**
 * \brief          This function resizes an MPI downwards, keeping at least the
 *                 specified number of limbs.
 *
 *                 If \c X is smaller than \c nblimbs, it is resized up
 *                 instead.
 *
 * \param X        The MPI to shrink. This must point to an initialized MPI.
 * \param nblimbs  The minimum number of limbs to keep.
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED if memory allocation failed
 *                 (this can only happen when resizing up).
 * \return         Another negative error code on other kinds of failure.
 */
int blcrypto_suite_mpi_shrink( blcrypto_suite_mpi *X, size_t nblimbs );

/**
 * \brief          Make a copy of an MPI.
 *
 * \param X        The destination MPI. This must point to an initialized MPI.
 * \param Y        The source MPI. This must point to an initialized MPI.
 *
 * \note           The limb-buffer in the destination MPI is enlarged
 *                 if necessary to hold the value in the source MPI.
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED if memory allocation failed.
 * \return         Another negative error code on other kinds of failure.
 */
int blcrypto_suite_mpi_copy( blcrypto_suite_mpi *X, const blcrypto_suite_mpi *Y );

/**
 * \brief          Swap the contents of two MPIs.
 *
 * \param X        The first MPI. It must be initialized.
 * \param Y        The second MPI. It must be initialized.
 */
void blcrypto_suite_mpi_swap( blcrypto_suite_mpi *X, blcrypto_suite_mpi *Y );

/**
 * \brief          Perform a safe conditional copy of MPI which doesn't
 *                 reveal whether the condition was true or not.
 *
 * \param X        The MPI to conditionally assign to. This must point
 *                 to an initialized MPI.
 * \param Y        The MPI to be assigned from. This must point to an
 *                 initialized MPI.
 * \param assign   The condition deciding whether to perform the
 *                 assignment or not. Possible values:
 *                 * \c 1: Perform the assignment `X = Y`.
 *                 * \c 0: Keep the original value of \p X.
 *
 * \note           This function is equivalent to
 *                      `if( assign ) blcrypto_suite_mpi_copy( X, Y );`
 *                 except that it avoids leaking any information about whether
 *                 the assignment was done or not (the above code may leak
 *                 information through branch prediction and/or memory access
 *                 patterns analysis).
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED if memory allocation failed.
 * \return         Another negative error code on other kinds of failure.
 */
int blcrypto_suite_mpi_safe_cond_assign( blcrypto_suite_mpi *X, const blcrypto_suite_mpi *Y, unsigned char assign );

/**
 * \brief          Perform a safe conditional swap which doesn't
 *                 reveal whether the condition was true or not.
 *
 * \param X        The first MPI. This must be initialized.
 * \param Y        The second MPI. This must be initialized.
 * \param assign   The condition deciding whether to perform
 *                 the swap or not. Possible values:
 *                 * \c 1: Swap the values of \p X and \p Y.
 *                 * \c 0: Keep the original values of \p X and \p Y.
 *
 * \note           This function is equivalent to
 *                      if( assign ) blcrypto_suite_mpi_swap( X, Y );
 *                 except that it avoids leaking any information about whether
 *                 the assignment was done or not (the above code may leak
 *                 information through branch prediction and/or memory access
 *                 patterns analysis).
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED if memory allocation failed.
 * \return         Another negative error code on other kinds of failure.
 *
 */
int blcrypto_suite_mpi_safe_cond_swap( blcrypto_suite_mpi *X, blcrypto_suite_mpi *Y, unsigned char assign );

/**
 * \brief          Store integer value in MPI.
 *
 * \param X        The MPI to set. This must be initialized.
 * \param z        The value to use.
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED if memory allocation failed.
 * \return         Another negative error code on other kinds of failure.
 */
int blcrypto_suite_mpi_lset( blcrypto_suite_mpi *X, blcrypto_suite_mpi_sint z );

/**
 * \brief          Get a specific bit from an MPI.
 *
 * \param X        The MPI to query. This must be initialized.
 * \param pos      Zero-based index of the bit to query.
 *
 * \return         \c 0 or \c 1 on success, depending on whether bit \c pos
 *                 of \c X is unset or set.
 * \return         A negative error code on failure.
 */
int blcrypto_suite_mpi_get_bit( const blcrypto_suite_mpi *X, size_t pos );

/**
 * \brief          Modify a specific bit in an MPI.
 *
 * \note           This function will grow the target MPI if necessary to set a
 *                 bit to \c 1 in a not yet existing limb. It will not grow if
 *                 the bit should be set to \c 0.
 *
 * \param X        The MPI to modify. This must be initialized.
 * \param pos      Zero-based index of the bit to modify.
 * \param val      The desired value of bit \c pos: \c 0 or \c 1.
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED if memory allocation failed.
 * \return         Another negative error code on other kinds of failure.
 */
int blcrypto_suite_mpi_set_bit( blcrypto_suite_mpi *X, size_t pos, unsigned char val );

/**
 * \brief          Return the number of bits of value \c 0 before the
 *                 least significant bit of value \c 1.
 *
 * \note           This is the same as the zero-based index of
 *                 the least significant bit of value \c 1.
 *
 * \param X        The MPI to query.
 *
 * \return         The number of bits of value \c 0 before the least significant
 *                 bit of value \c 1 in \p X.
 */
size_t blcrypto_suite_mpi_lsb( const blcrypto_suite_mpi *X );

/**
 * \brief          Return the number of bits up to and including the most
 *                 significant bit of value \c 1.
 *
 * * \note         This is same as the one-based index of the most
 *                 significant bit of value \c 1.
 *
 * \param X        The MPI to query. This must point to an initialized MPI.
 *
 * \return         The number of bits up to and including the most
 *                 significant bit of value \c 1.
 */
size_t blcrypto_suite_mpi_bitlen( const blcrypto_suite_mpi *X );

/**
 * \brief          Return the total size of an MPI value in bytes.
 *
 * \param X        The MPI to use. This must point to an initialized MPI.
 *
 * \note           The value returned by this function may be less than
 *                 the number of bytes used to store \p X internally.
 *                 This happens if and only if there are trailing bytes
 *                 of value zero.
 *
 * \return         The least number of bytes capable of storing
 *                 the absolute value of \p X.
 */
size_t blcrypto_suite_mpi_size( const blcrypto_suite_mpi *X );

/**
 * \brief          Import an MPI from an ASCII string.
 *
 * \param X        The destination MPI. This must point to an initialized MPI.
 * \param radix    The numeric base of the input string.
 * \param s        Null-terminated string buffer.
 *
 * \return         \c 0 if successful.
 * \return         A negative error code on failure.
 */
int blcrypto_suite_mpi_read_string( blcrypto_suite_mpi *X, int radix, const char *s );

/**
 * \brief          Export an MPI to an ASCII string.
 *
 * \param X        The source MPI. This must point to an initialized MPI.
 * \param radix    The numeric base of the output string.
 * \param buf      The buffer to write the string to. This must be writable
 *                 buffer of length \p buflen Bytes.
 * \param buflen   The available size in Bytes of \p buf.
 * \param olen     The address at which to store the length of the string
 *                 written, including the  final \c NULL byte. This must
 *                 not be \c NULL.
 *
 * \note           You can call this function with `buflen == 0` to obtain the
 *                 minimum required buffer size in `*olen`.
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_BUFFER_TOO_SMALL if the target buffer \p buf
 *                 is too small to hold the value of \p X in the desired base.
 *                 In this case, `*olen` is nonetheless updated to contain the
 *                 size of \p buf required for a successful call.
 * \return         Another negative error code on different kinds of failure.
 */
int blcrypto_suite_mpi_write_string( const blcrypto_suite_mpi *X, int radix,
                              char *buf, size_t buflen, size_t *olen );

#if defined(BLCRYPTO_SUITE_FS_IO)
/**
 * \brief          Read an MPI from a line in an opened file.
 *
 * \param X        The destination MPI. This must point to an initialized MPI.
 * \param radix    The numeric base of the string representation used
 *                 in the source line.
 * \param fin      The input file handle to use. This must not be \c NULL.
 *
 * \note           On success, this function advances the file stream
 *                 to the end of the current line or to EOF.
 *
 *                 The function returns \c 0 on an empty line.
 *
 *                 Leading whitespaces are ignored, as is a
 *                 '0x' prefix for radix \c 16.
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_BUFFER_TOO_SMALL if the file read buffer
 *                 is too small.
 * \return         Another negative error code on failure.
 */
int blcrypto_suite_mpi_read_file( blcrypto_suite_mpi *X, int radix, FILE *fin );

/**
 * \brief          Export an MPI into an opened file.
 *
 * \param p        A string prefix to emit prior to the MPI data.
 *                 For example, this might be a label, or "0x" when
 *                 printing in base \c 16. This may be \c NULL if no prefix
 *                 is needed.
 * \param X        The source MPI. This must point to an initialized MPI.
 * \param radix    The numeric base to be used in the emitted string.
 * \param fout     The output file handle. This may be \c NULL, in which case
 *                 the output is written to \c stdout.
 *
 * \return         \c 0 if successful.
 * \return         A negative error code on failure.
 */
int blcrypto_suite_mpi_write_file( const char *p, const blcrypto_suite_mpi *X,
                            int radix, FILE *fout );
#endif /* BLCRYPTO_SUITE_FS_IO */

/**
 * \brief          Import an MPI from unsigned big endian binary data.
 *
 * \param X        The destination MPI. This must point to an initialized MPI.
 * \param buf      The input buffer. This must be a readable buffer of length
 *                 \p buflen Bytes.
 * \param buflen   The length of the input buffer \p p in Bytes.
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED if memory allocation failed.
 * \return         Another negative error code on different kinds of failure.
 */
int blcrypto_suite_mpi_read_binary( blcrypto_suite_mpi *X, const unsigned char *buf,
                             size_t buflen );

/**
 * \brief          Export an MPI into unsigned big endian binary data
 *                 of fixed size.
 *
 * \param X        The source MPI. This must point to an initialized MPI.
 * \param buf      The output buffer. This must be a writable buffer of length
 *                 \p buflen Bytes.
 * \param buflen   The size of the output buffer \p buf in Bytes.
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_BUFFER_TOO_SMALL if \p buf isn't
 *                 large enough to hold the value of \p X.
 * \return         Another negative error code on different kinds of failure.
 */
int blcrypto_suite_mpi_write_binary( const blcrypto_suite_mpi *X, unsigned char *buf,
                              size_t buflen );

/**
 * \brief          Perform a left-shift on an MPI: X <<= count
 *
 * \param X        The MPI to shift. This must point to an initialized MPI.
 * \param count    The number of bits to shift by.
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         Another negative error code on different kinds of failure.
 */
int blcrypto_suite_mpi_shift_l( blcrypto_suite_mpi *X, size_t count );

/**
 * \brief          Perform a right-shift on an MPI: X >>= count
 *
 * \param X        The MPI to shift. This must point to an initialized MPI.
 * \param count    The number of bits to shift by.
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         Another negative error code on different kinds of failure.
 */
int blcrypto_suite_mpi_shift_r( blcrypto_suite_mpi *X, size_t count );

/**
 * \brief          Compare the absolute values of two MPIs.
 *
 * \param X        The left-hand MPI. This must point to an initialized MPI.
 * \param Y        The right-hand MPI. This must point to an initialized MPI.
 *
 * \return         \c 1 if `|X|` is greater than `|Y|`.
 * \return         \c -1 if `|X|` is lesser than `|Y|`.
 * \return         \c 0 if `|X|` is equal to `|Y|`.
 */
int blcrypto_suite_mpi_cmp_abs( const blcrypto_suite_mpi *X, const blcrypto_suite_mpi *Y );

/**
 * \brief          Compare two MPIs.
 *
 * \param X        The left-hand MPI. This must point to an initialized MPI.
 * \param Y        The right-hand MPI. This must point to an initialized MPI.
 *
 * \return         \c 1 if \p X is greater than \p Y.
 * \return         \c -1 if \p X is lesser than \p Y.
 * \return         \c 0 if \p X is equal to \p Y.
 */
int blcrypto_suite_mpi_cmp_mpi( const blcrypto_suite_mpi *X, const blcrypto_suite_mpi *Y );

/**
 * \brief          Check if an MPI is less than the other in constant time.
 *
 * \param X        The left-hand MPI. This must point to an initialized MPI
 *                 with the same allocated length as Y.
 * \param Y        The right-hand MPI. This must point to an initialized MPI
 *                 with the same allocated length as X.
 * \param ret      The result of the comparison:
 *                 \c 1 if \p X is less than \p Y.
 *                 \c 0 if \p X is greater than or equal to \p Y.
 *
 * \return         0 on success.
 * \return         BLCRYPTO_SUITE_ERR_MPI_BAD_INPUT_DATA if the allocated length of
 *                 the two input MPIs is not the same.
 */
int blcrypto_suite_mpi_lt_mpi_ct( const blcrypto_suite_mpi *X, const blcrypto_suite_mpi *Y,
        unsigned *ret );

/**
 * \brief          Compare an MPI with an integer.
 *
 * \param X        The left-hand MPI. This must point to an initialized MPI.
 * \param z        The integer value to compare \p X to.
 *
 * \return         \c 1 if \p X is greater than \p z.
 * \return         \c -1 if \p X is lesser than \p z.
 * \return         \c 0 if \p X is equal to \p z.
 */
int blcrypto_suite_mpi_cmp_int( const blcrypto_suite_mpi *X, blcrypto_suite_mpi_sint z );

/**
 * \brief          Perform an unsigned addition of MPIs: X = |A| + |B|
 *
 * \param X        The destination MPI. This must point to an initialized MPI.
 * \param A        The first summand. This must point to an initialized MPI.
 * \param B        The second summand. This must point to an initialized MPI.
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         Another negative error code on different kinds of failure.
 */
int blcrypto_suite_mpi_add_abs( blcrypto_suite_mpi *X, const blcrypto_suite_mpi *A,
                         const blcrypto_suite_mpi *B );

/**
 * \brief          Perform an unsigned subtraction of MPIs: X = |A| - |B|
 *
 * \param X        The destination MPI. This must point to an initialized MPI.
 * \param A        The minuend. This must point to an initialized MPI.
 * \param B        The subtrahend. This must point to an initialized MPI.
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_NEGATIVE_VALUE if \p B is greater than \p A.
 * \return         Another negative error code on different kinds of failure.
 *
 */
int blcrypto_suite_mpi_sub_abs( blcrypto_suite_mpi *X, const blcrypto_suite_mpi *A,
                         const blcrypto_suite_mpi *B );

/**
 * \brief          Perform a signed addition of MPIs: X = A + B
 *
 * \param X        The destination MPI. This must point to an initialized MPI.
 * \param A        The first summand. This must point to an initialized MPI.
 * \param B        The second summand. This must point to an initialized MPI.
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         Another negative error code on different kinds of failure.
 */
int blcrypto_suite_mpi_add_mpi( blcrypto_suite_mpi *X, const blcrypto_suite_mpi *A,
                         const blcrypto_suite_mpi *B );

/**
 * \brief          Perform a signed subtraction of MPIs: X = A - B
 *
 * \param X        The destination MPI. This must point to an initialized MPI.
 * \param A        The minuend. This must point to an initialized MPI.
 * \param B        The subtrahend. This must point to an initialized MPI.
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         Another negative error code on different kinds of failure.
 */
int blcrypto_suite_mpi_sub_mpi( blcrypto_suite_mpi *X, const blcrypto_suite_mpi *A,
                         const blcrypto_suite_mpi *B );

/**
 * \brief          Perform a signed addition of an MPI and an integer: X = A + b
 *
 * \param X        The destination MPI. This must point to an initialized MPI.
 * \param A        The first summand. This must point to an initialized MPI.
 * \param b        The second summand.
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         Another negative error code on different kinds of failure.
 */
int blcrypto_suite_mpi_add_int( blcrypto_suite_mpi *X, const blcrypto_suite_mpi *A,
                         blcrypto_suite_mpi_sint b );

/**
 * \brief          Perform a signed subtraction of an MPI and an integer:
 *                 X = A - b
 *
 * \param X        The destination MPI. This must point to an initialized MPI.
 * \param A        The minuend. This must point to an initialized MPI.
 * \param b        The subtrahend.
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         Another negative error code on different kinds of failure.
 */
int blcrypto_suite_mpi_sub_int( blcrypto_suite_mpi *X, const blcrypto_suite_mpi *A,
                         blcrypto_suite_mpi_sint b );

/**
 * \brief          Perform a multiplication of two MPIs: X = A * B
 *
 * \param X        The destination MPI. This must point to an initialized MPI.
 * \param A        The first factor. This must point to an initialized MPI.
 * \param B        The second factor. This must point to an initialized MPI.
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         Another negative error code on different kinds of failure.
 *
 */
int blcrypto_suite_mpi_mul_mpi( blcrypto_suite_mpi *X, const blcrypto_suite_mpi *A,
                         const blcrypto_suite_mpi *B );

/**
 * \brief          Perform a multiplication of an MPI with an unsigned integer:
 *                 X = A * b
 *
 * \param X        The destination MPI. This must point to an initialized MPI.
 * \param A        The first factor. This must point to an initialized MPI.
 * \param b        The second factor.
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         Another negative error code on different kinds of failure.
 *
 */
int blcrypto_suite_mpi_mul_int( blcrypto_suite_mpi *X, const blcrypto_suite_mpi *A,
                         blcrypto_suite_mpi_uint b );

/**
 * \brief          Perform a division with remainder of two MPIs:
 *                 A = Q * B + R
 *
 * \param Q        The destination MPI for the quotient.
 *                 This may be \c NULL if the value of the
 *                 quotient is not needed.
 * \param R        The destination MPI for the remainder value.
 *                 This may be \c NULL if the value of the
 *                 remainder is not needed.
 * \param A        The dividend. This must point to an initialized MPi.
 * \param B        The divisor. This must point to an initialized MPI.
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED if memory allocation failed.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_DIVISION_BY_ZERO if \p B equals zero.
 * \return         Another negative error code on different kinds of failure.
 */
int blcrypto_suite_mpi_div_mpi( blcrypto_suite_mpi *Q, blcrypto_suite_mpi *R, const blcrypto_suite_mpi *A,
                         const blcrypto_suite_mpi *B );

/**
 * \brief          Perform a division with remainder of an MPI by an integer:
 *                 A = Q * b + R
 *
 * \param Q        The destination MPI for the quotient.
 *                 This may be \c NULL if the value of the
 *                 quotient is not needed.
 * \param R        The destination MPI for the remainder value.
 *                 This may be \c NULL if the value of the
 *                 remainder is not needed.
 * \param A        The dividend. This must point to an initialized MPi.
 * \param b        The divisor.
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED if memory allocation failed.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_DIVISION_BY_ZERO if \p b equals zero.
 * \return         Another negative error code on different kinds of failure.
 */
int blcrypto_suite_mpi_div_int( blcrypto_suite_mpi *Q, blcrypto_suite_mpi *R, const blcrypto_suite_mpi *A,
                         blcrypto_suite_mpi_sint b );

/**
 * \brief          Perform a modular reduction. R = A mod B
 *
 * \param R        The destination MPI for the residue value.
 *                 This must point to an initialized MPI.
 * \param A        The MPI to compute the residue of.
 *                 This must point to an initialized MPI.
 * \param B        The base of the modular reduction.
 *                 This must point to an initialized MPI.
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_DIVISION_BY_ZERO if \p B equals zero.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_NEGATIVE_VALUE if \p B is negative.
 * \return         Another negative error code on different kinds of failure.
 *
 */
int blcrypto_suite_mpi_mod_mpi( blcrypto_suite_mpi *R, const blcrypto_suite_mpi *A,
                         const blcrypto_suite_mpi *B );

/**
 * \brief          Perform a modular reduction with respect to an integer.
 *                 r = A mod b
 *
 * \param r        The address at which to store the residue.
 *                 This must not be \c NULL.
 * \param A        The MPI to compute the residue of.
 *                 This must point to an initialized MPi.
 * \param b        The integer base of the modular reduction.
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_DIVISION_BY_ZERO if \p b equals zero.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_NEGATIVE_VALUE if \p b is negative.
 * \return         Another negative error code on different kinds of failure.
 */
int blcrypto_suite_mpi_mod_int( blcrypto_suite_mpi_uint *r, const blcrypto_suite_mpi *A,
                         blcrypto_suite_mpi_sint b );

/**
 * \brief          Perform a sliding-window exponentiation: X = A^E mod N
 *
 * \param X        The destination MPI. This must point to an initialized MPI.
 * \param A        The base of the exponentiation.
 *                 This must point to an initialized MPI.
 * \param E        The exponent MPI. This must point to an initialized MPI.
 * \param N        The base for the modular reduction. This must point to an
 *                 initialized MPI.
 * \param _RR      A helper MPI depending solely on \p N which can be used to
 *                 speed-up multiple modular exponentiations for the same value
 *                 of \p N. This may be \c NULL. If it is not \c NULL, it must
 *                 point to an initialized MPI. If it hasn't been used after
 *                 the call to blcrypto_suite_mpi_init(), this function will compute
 *                 the helper value and store it in \p _RR for reuse on
 *                 subsequent calls to this function. Otherwise, the function
 *                 will assume that \p _RR holds the helper value set by a
 *                 previous call to blcrypto_suite_mpi_exp_mod(), and reuse it.
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_BAD_INPUT_DATA if \c N is negative or
 *                 even, or if \c E is negative.
 * \return         Another negative error code on different kinds of failures.
 *
 */
int blcrypto_suite_mpi_exp_mod( blcrypto_suite_mpi *X, const blcrypto_suite_mpi *A,
                         const blcrypto_suite_mpi *E, const blcrypto_suite_mpi *N,
                         blcrypto_suite_mpi *_RR );

/**
 * \brief          Fill an MPI with a number of random bytes.
 *
 * \param X        The destination MPI. This must point to an initialized MPI.
 * \param size     The number of random bytes to generate.
 * \param f_rng    The RNG function to use. This must not be \c NULL.
 * \param p_rng    The RNG parameter to be passed to \p f_rng. This may be
 *                 \c NULL if \p f_rng doesn't need a context argument.
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         Another negative error code on failure.
 *
 * \note           The bytes obtained from the RNG are interpreted
 *                 as a big-endian representation of an MPI; this can
 *                 be relevant in applications like deterministic ECDSA.
 */
int blcrypto_suite_mpi_fill_random( blcrypto_suite_mpi *X, size_t size,
                     int (*f_rng)(void *, unsigned char *, size_t),
                     void *p_rng );

/**
 * \brief          Compute the greatest common divisor: G = gcd(A, B)
 *
 * \param G        The destination MPI. This must point to an initialized MPI.
 * \param A        The first operand. This must point to an initialized MPI.
 * \param B        The second operand. This must point to an initialized MPI.
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         Another negative error code on different kinds of failure.
 */
int blcrypto_suite_mpi_gcd( blcrypto_suite_mpi *G, const blcrypto_suite_mpi *A,
                     const blcrypto_suite_mpi *B );

/**
 * \brief          Compute the modular inverse: X = A^-1 mod N
 *
 * \param X        The destination MPI. This must point to an initialized MPI.
 * \param A        The MPI to calculate the modular inverse of. This must point
 *                 to an initialized MPI.
 * \param N        The base of the modular inversion. This must point to an
 *                 initialized MPI.
 *
 * \return         \c 0 if successful.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_BAD_INPUT_DATA if \p N is less than
 *                 or equal to one.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_NOT_ACCEPTABLE if \p has no modular inverse
 *                 with respect to \p N.
 */
int blcrypto_suite_mpi_inv_mod( blcrypto_suite_mpi *X, const blcrypto_suite_mpi *A,
                         const blcrypto_suite_mpi *N );

#if !defined(BLCRYPTO_SUITE_DEPRECATED_REMOVED)
#if defined(BLCRYPTO_SUITE_DEPRECATED_WARNING)
#define BLCRYPTO_SUITE_DEPRECATED      __attribute__((deprecated))
#else
#define BLCRYPTO_SUITE_DEPRECATED
#endif
/**
 * \brief          Perform a Miller-Rabin primality test with error
 *                 probability of 2<sup>-80</sup>.
 *
 * \deprecated     Superseded by blcrypto_suite_mpi_is_prime_ext() which allows
 *                 specifying the number of Miller-Rabin rounds.
 *
 * \param X        The MPI to check for primality.
 *                 This must point to an initialized MPI.
 * \param f_rng    The RNG function to use. This must not be \c NULL.
 * \param p_rng    The RNG parameter to be passed to \p f_rng.
 *                 This may be \c NULL if \p f_rng doesn't use a
 *                 context parameter.
 *
 * \return         \c 0 if successful, i.e. \p X is probably prime.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_NOT_ACCEPTABLE if \p X is not prime.
 * \return         Another negative error code on other kinds of failure.
 */
BLCRYPTO_SUITE_DEPRECATED int blcrypto_suite_mpi_is_prime( const blcrypto_suite_mpi *X,
                          int (*f_rng)(void *, unsigned char *, size_t),
                          void *p_rng );
#undef BLCRYPTO_SUITE_DEPRECATED
#endif /* !BLCRYPTO_SUITE_DEPRECATED_REMOVED */

/**
 * \brief          Miller-Rabin primality test.
 *
 * \warning        If \p X is potentially generated by an adversary, for example
 *                 when validating cryptographic parameters that you didn't
 *                 generate yourself and that are supposed to be prime, then
 *                 \p rounds should be at least the half of the security
 *                 strength of the cryptographic algorithm. On the other hand,
 *                 if \p X is chosen uniformly or non-adversially (as is the
 *                 case when blcrypto_suite_mpi_gen_prime calls this function), then
 *                 \p rounds can be much lower.
 *
 * \param X        The MPI to check for primality.
 *                 This must point to an initialized MPI.
 * \param rounds   The number of bases to perform the Miller-Rabin primality
 *                 test for. The probability of returning 0 on a composite is
 *                 at most 2<sup>-2*\p rounds</sup>.
 * \param f_rng    The RNG function to use. This must not be \c NULL.
 * \param p_rng    The RNG parameter to be passed to \p f_rng.
 *                 This may be \c NULL if \p f_rng doesn't use
 *                 a context parameter.
 *
 * \return         \c 0 if successful, i.e. \p X is probably prime.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_NOT_ACCEPTABLE if \p X is not prime.
 * \return         Another negative error code on other kinds of failure.
 */
int blcrypto_suite_mpi_is_prime_ext( const blcrypto_suite_mpi *X, int rounds,
                              int (*f_rng)(void *, unsigned char *, size_t),
                              void *p_rng );
/**
 * \brief Flags for blcrypto_suite_mpi_gen_prime()
 *
 * Each of these flags is a constraint on the result X returned by
 * blcrypto_suite_mpi_gen_prime().
 */
typedef enum {
    BLCRYPTO_SUITE_MPI_GEN_PRIME_FLAG_DH =      0x0001, /**< (X-1)/2 is prime too */
    BLCRYPTO_SUITE_MPI_GEN_PRIME_FLAG_LOW_ERR = 0x0002, /**< lower error rate from 2<sup>-80</sup> to 2<sup>-128</sup> */
} blcrypto_suite_mpi_gen_prime_flag_t;

/**
 * \brief          Generate a prime number.
 *
 * \param X        The destination MPI to store the generated prime in.
 *                 This must point to an initialized MPi.
 * \param nbits    The required size of the destination MPI in bits.
 *                 This must be between \c 3 and #BLCRYPTO_SUITE_MPI_MAX_BITS.
 * \param flags    A mask of flags of type #blcrypto_suite_mpi_gen_prime_flag_t.
 * \param f_rng    The RNG function to use. This must not be \c NULL.
 * \param p_rng    The RNG parameter to be passed to \p f_rng.
 *                 This may be \c NULL if \p f_rng doesn't use
 *                 a context parameter.
 *
 * \return         \c 0 if successful, in which case \p X holds a
 *                 probably prime number.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         #BLCRYPTO_SUITE_ERR_MPI_BAD_INPUT_DATA if `nbits` is not between
 *                 \c 3 and #BLCRYPTO_SUITE_MPI_MAX_BITS.
 */
int blcrypto_suite_mpi_gen_prime( blcrypto_suite_mpi *X, size_t nbits, int flags,
                   int (*f_rng)(void *, unsigned char *, size_t),
                   void *p_rng );

#if defined(BLCRYPTO_SUITE_SELF_TEST)

/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 */
int blcrypto_suite_mpi_self_test( int verbose );

#endif /* BLCRYPTO_SUITE_SELF_TEST */

#ifdef __cplusplus
}
#endif

#endif /* bignum.h */
