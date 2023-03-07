/*
 *  Elliptic curves over GF(p): generic functions
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
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
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */
/*
 * Copyright (C) 2018 Beerten Engineering
 * Copyright (C) 2018 Silex Inside
 *      hardware offload with CryptoSoc.
 */

/*
 * References:
 *
 * SEC1 http://www.secg.org/index.php?action=secg,docs_secg
 * GECC = Guide to Elliptic Curve Cryptography - Hankerson, Menezes, Vanstone
 * FIPS 186-3 http://csrc.nist.gov/publications/fips/fips186-3/fips_186-3.pdf
 * RFC 4492 for the related TLS structures and constants
 *
 * [Curve25519] http://cr.yp.to/ecdh/curve25519-20060209.pdf
 *
 * [2] CORON, Jean-S'ebastien. Resistance against differential power analysis
 *     for elliptic curve cryptosystems. In : Cryptographic Hardware and
 *     Embedded Systems. Springer Berlin Heidelberg, 1999. p. 292-302.
 *     <http://link.springer.com/chapter/10.1007/3-540-48059-5_25>
 *
 * [3] HEDABOU, Mustapha, PINEL, Pierre, et B'EN'ETEAU, Lucien. A comb method to
 *     render ECC resistant against Side Channel Attacks. IACR Cryptology
 *     ePrint Archive, 2004, vol. 2004, p. 342.
 *     <http://eprint.iacr.org/2004/342.pdf>
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_ECP_C)

#include "mbedtls/ecp.h"
#include "mbedtls/threading.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"

#include <string.h>

#if defined(MBEDTLS_ECP_ALT)

#define ECP_VALIDATE_RET( cond )    \
    if (!(cond)) { return MBEDTLS_ERR_ECP_BAD_INPUT_DATA; }

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdlib.h>
#include <stdio.h>
#define mbedtls_printf     printf
#define mbedtls_calloc    calloc
#define mbedtls_free       free
#endif

#include "mbedtls/ecp_internal.h"

#if ( defined(__ARMCC_VERSION) || defined(_MSC_VER) ) && \
    !defined(inline) && !defined(__cplusplus)
#define inline __inline
#endif

#include <stdbool.h>
#include <ba414e_config.h>
#include <sx_dh_alg.h>
#include <sx_ecc_curves.h>
#include <sx_ecc_keygen_alg.h>
#include <sx_primitives.h>
#include <sx_generic.h>

#if defined(MBEDTLS_ECDSA_VERIFY_ALT) || defined(MBEDTLS_ECDSA_SIGN_ALT)
#include <sx_ecdsa_alg.h>
#endif

#define ECC_MAX_OPERAND_SZ ECC_MAX_KEY_SIZE

/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}

#if defined(MBEDTLS_SELF_TEST)
/*
 * Counts of point addition and doubling, and field multiplications.
 * Used to test resistance of point multiplication to simple timing attacks.
 */
static unsigned long add_count, dbl_count, mul_count;
#endif

#if defined(MBEDTLS_ECP_DP_SECP192R1_ENABLED) ||   \
    defined(MBEDTLS_ECP_DP_SECP224R1_ENABLED) ||   \
    defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED) ||   \
    defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED) ||   \
    defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED) ||   \
    defined(MBEDTLS_ECP_DP_BP256R1_ENABLED)   ||   \
    defined(MBEDTLS_ECP_DP_BP384R1_ENABLED)   ||   \
    defined(MBEDTLS_ECP_DP_BP512R1_ENABLED)   ||   \
    defined(MBEDTLS_ECP_DP_SECP192K1_ENABLED) ||   \
    defined(MBEDTLS_ECP_DP_SECP224K1_ENABLED) ||   \
    defined(MBEDTLS_ECP_DP_SECP256K1_ENABLED)
#define ECP_SHORTWEIERSTRASS
#endif

#if defined(MBEDTLS_ECP_DP_CURVE25519_ENABLED)
#define ECP_MONTGOMERY
#endif

/*
 * Curve types: internal for now, might be exposed later
 */
typedef enum
{
    ECP_TYPE_NONE = 0,
    ECP_TYPE_SHORT_WEIERSTRASS,    /* y^2 = x^3 + a x + b      */
    ECP_TYPE_MONTGOMERY,           /* y^2 = x^3 + a x^2 + x    */
} ecp_curve_type;

/*
 * List of supported curves:
 *  - internal ID
 *  - TLS NamedCurve ID (RFC 4492 sec. 5.1.1, RFC 7071 sec. 2)
 *  - size in bits
 *  - readable name
 *
 * Curves are listed in order: largest curves first, and for a given size,
 * fastest curves first. This provides the default order for the SSL module.
 *
 * Reminder: update profiles in x509_crt.c when adding a new curves!
 */
static const mbedtls_ecp_curve_info ecp_supported_curves[] =
{
#if defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)
    { MBEDTLS_ECP_DP_SECP521R1,    25,     521,    "secp521r1"         },
#endif
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
    { MBEDTLS_ECP_DP_SECP384R1,    24,     384,    "secp384r1"         },
#endif
#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
    { MBEDTLS_ECP_DP_SECP256R1,    23,     256,    "secp256r1"         },
#endif
#if defined(MBEDTLS_ECP_DP_SECP224R1_ENABLED)
    { MBEDTLS_ECP_DP_SECP224R1,    21,     224,    "secp224r1"         },
#endif
#if defined(MBEDTLS_ECP_DP_SECP192R1_ENABLED)
    { MBEDTLS_ECP_DP_SECP192R1,    19,     192,    "secp192r1"         },
#endif
    { MBEDTLS_ECP_DP_NONE,          0,     0,      NULL                },
};

#define ECP_NB_CURVES   sizeof( ecp_supported_curves ) /    \
                        sizeof( ecp_supported_curves[0] )

static mbedtls_ecp_group_id ecp_supported_grp_id[ECP_NB_CURVES];


/*
 * List of supported curves and associated info
 */
const mbedtls_ecp_curve_info *mbedtls_ecp_curve_list( void )
{
    return( ecp_supported_curves );
}

/*
 * List of supported curves, group ID only
 */
const mbedtls_ecp_group_id *mbedtls_ecp_grp_id_list( void )
{
    static int init_done = 0;

    if( ! init_done )
    {
        size_t i = 0;
        const mbedtls_ecp_curve_info *curve_info;

        for( curve_info = mbedtls_ecp_curve_list();
             curve_info->grp_id != MBEDTLS_ECP_DP_NONE;
             curve_info++ )
        {
            ecp_supported_grp_id[i++] = curve_info->grp_id;
        }
        ecp_supported_grp_id[i] = MBEDTLS_ECP_DP_NONE;

        init_done = 1;
    }

    return( ecp_supported_grp_id );
}

/*
 * Get the curve info for the internal identifier
 */
const mbedtls_ecp_curve_info *mbedtls_ecp_curve_info_from_grp_id( mbedtls_ecp_group_id grp_id )
{
    const mbedtls_ecp_curve_info *curve_info;

    for( curve_info = mbedtls_ecp_curve_list();
         curve_info->grp_id != MBEDTLS_ECP_DP_NONE;
         curve_info++ )
    {
        if( curve_info->grp_id == grp_id )
            return( curve_info );
    }

    return( NULL );
}

/*
 * Get the curve info from the TLS identifier
 */
const mbedtls_ecp_curve_info *mbedtls_ecp_curve_info_from_tls_id( uint16_t tls_id )
{
    const mbedtls_ecp_curve_info *curve_info;

    for( curve_info = mbedtls_ecp_curve_list();
         curve_info->grp_id != MBEDTLS_ECP_DP_NONE;
         curve_info++ )
    {
        if( curve_info->tls_id == tls_id )
            return( curve_info );
    }

    return( NULL );
}

/*
 * Get the curve info from the name
 */
const mbedtls_ecp_curve_info *mbedtls_ecp_curve_info_from_name( const char *name )
{
    const mbedtls_ecp_curve_info *curve_info;

    for( curve_info = mbedtls_ecp_curve_list();
         curve_info->grp_id != MBEDTLS_ECP_DP_NONE;
         curve_info++ )
    {
        if( strcmp( curve_info->name, name ) == 0 )
            return( curve_info );
    }

    return( NULL );
}

/*
 * Get the type of a curve
 */
static inline ecp_curve_type ecp_get_type( const mbedtls_ecp_group *grp )
{
    if( grp->G.X.p == NULL )
        return( ECP_TYPE_NONE );
    if( grp->G.Y.p == NULL )
        return( ECP_TYPE_MONTGOMERY );
    else
        return( ECP_TYPE_SHORT_WEIERSTRASS );
}

/** Convert a mbedtls_ecp_point point into a block_t representation for cryptosoc */
static int convert_pt_ecp_cryptosoc_local(const mbedtls_ecp_point *pt, block_t sx_pt,
    int byteslen)
{
    int ret = 0;

    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &pt->X, sx_pt.addr, byteslen) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &pt->Y, sx_pt.addr + byteslen, byteslen) );
cleanup:
    return ret;
}

/** Convert a cryptosoc block_t point representation into a mbedtls_ecp_point */
static int convert_pt_cryptosoc_ecp_local(mbedtls_ecp_point *pt, block_t sx_pt,
    int byteslen)
{
    int ret = 0;

    MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &pt->X, sx_pt.addr, byteslen) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &pt->Y, sx_pt.addr + byteslen, byteslen) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_lset( &pt->Z, 1) );
cleanup:
    return ret;
}

static const sx_ecc_curve_t * find_ecp_curve(mbedtls_ecp_group_id id)
{
    switch (id) {
    case MBEDTLS_ECP_DP_SECP192R1:
        //return &sx_ecc_curve_p192;
        return sx_find_ecp_curve(SX_ECP_DP_SECP192R1);
    case MBEDTLS_ECP_DP_SECP224R1:
        //return &sx_ecc_curve_p224;
        return sx_find_ecp_curve(SX_ECP_DP_SECP224R1);
    case MBEDTLS_ECP_DP_SECP256R1:
        //return &sx_ecc_curve_p256;
        return sx_find_ecp_curve(SX_ECP_DP_SECP256R1);
    case MBEDTLS_ECP_DP_SECP384R1:
        //return &sx_ecc_curve_p384;
        return sx_find_ecp_curve(SX_ECP_DP_SECP384R1);
    case MBEDTLS_ECP_DP_SECP521R1:
        //return &sx_ecc_curve_p521;
        return sx_find_ecp_curve(SX_ECP_DP_SECP521R1);
    case MBEDTLS_ECP_DP_CURVE25519:
        //return &sx_ecc_curve_curve25519;
        return sx_find_ecp_curve(SX_ECP_DP_CURVE25519);
    default:
        return NULL;
    }
}

/** Multiply and add ECC Weierstrass points
 *
 * result = scalar_m * point1 + scalar_n * point2
 */
uint32_t sx_ecc_pt_muladd(block_t curve, block_t scalar_m, block_t point1,
    block_t scalar_n, block_t point2, block_t result, uint32_t size, uint32_t curve_flags)
{
   uint32_t error;

    sx_enable_clock();

    /* first multiply 'point1' with the 'scalar_m' */
    ba414e_set_command(BA414E_OPTYPE_ECC_POINT_MULT, size, BA414E_BIGEND, curve_flags);
    ba414e_set_config(BA414E_MEMLOC_12, BA414E_MEMLOC_14, BA414E_MEMLOC_6, 0x0);
    // Load curve parameters
    error = ba414e_load_curve(curve, size, BA414E_BIGEND, 1);
    if (error)
        return CRYPTOLIB_CRYPTO_ERR;
    // Location 14 -> scalar_m
    mem2CryptoRAM_rev(scalar_m, size, BA414E_MEMLOC_14);
    // Location 12 -> point1 x, Location 13 -> point1 y
    point2CryptoRAM_rev(point1, size, BA414E_MEMLOC_12);
    // Start BA414E
    error = ba414e_start_wait_status();
    if (error)
        return CRYPTOLIB_CRYPTO_ERR;

    /* next multiply 'point2' with the 'scalar_m' */
    // Already done: ba414e_set_command(BA414E_OPTYPE_ECC_POINT_MULT, size, BA414E_BIGEND, curve_flags);
    ba414e_set_config(BA414E_MEMLOC_12, BA414E_MEMLOC_14, BA414E_MEMLOC_8, 0x0);
    // Location 14 -> scalar_n
    mem2CryptoRAM_rev(scalar_n, size, BA414E_MEMLOC_14);
    // Location 12 -> point2 x, Location 13 -> point2 y
    point2CryptoRAM_rev(point2, size, BA414E_MEMLOC_12);
    // Start BA414E
    error = ba414e_start_wait_status();
    if (error)
        return CRYPTOLIB_CRYPTO_ERR;

    /* Add the results of the 2 products*/
    ba414e_set_command(BA414E_OPTYPE_ECC_POINT_ADD, size, BA414E_BIGEND, curve_flags);
    ba414e_set_config(BA414E_MEMLOC_6, BA414E_MEMLOC_8, BA414E_MEMLOC_10, 0x0);
    // Start BA414E
    error = ba414e_start_wait_status();
    if (error)
        return CRYPTOLIB_CRYPTO_ERR;

    // Fetch the results
    CryptoRAM2point_rev(result, size, BA414E_MEMLOC_10);

    sx_disable_clock();

   return CRYPTOLIB_SUCCESS;
}


/*
 * Initialize (the components of) a point
 */
void mbedtls_ecp_point_init( mbedtls_ecp_point *pt )
{
    if( pt == NULL )
        return;

    mbedtls_mpi_init( &pt->X );
    mbedtls_mpi_init( &pt->Y );
    mbedtls_mpi_init( &pt->Z );
}

/*
 * Initialize (the components of) a group
 */
void mbedtls_ecp_group_init( mbedtls_ecp_group *grp )
{
    if( grp == NULL )
        return;

    memset( grp, 0, sizeof( mbedtls_ecp_group ) );
}

/*
 * Initialize (the components of) a key pair
 */
void mbedtls_ecp_keypair_init( mbedtls_ecp_keypair *key )
{
    if( key == NULL )
        return;

    mbedtls_ecp_group_init( &key->grp );
    mbedtls_mpi_init( &key->d );
    mbedtls_ecp_point_init( &key->Q );
}

/*
 * Unallocate (the components of) a point
 */
void mbedtls_ecp_point_free( mbedtls_ecp_point *pt )
{
    if( pt == NULL )
        return;

    mbedtls_mpi_free( &( pt->X ) );
    mbedtls_mpi_free( &( pt->Y ) );
    mbedtls_mpi_free( &( pt->Z ) );
}

/*
 * Unallocate (the components of) a group
 */
void mbedtls_ecp_group_free( mbedtls_ecp_group *grp )
{
    if( grp == NULL )
        return;
    if( grp->h != 1 )
    {
        mbedtls_mpi_free( &grp->P );
        mbedtls_mpi_free( &grp->A );
        mbedtls_mpi_free( &grp->B );
        mbedtls_ecp_point_free( &grp->G );
        mbedtls_mpi_free( &grp->N );
    }
    mbedtls_zeroize( grp, sizeof( mbedtls_ecp_group ) );
}

/*
 * Unallocate (the components of) a key pair
 */
void mbedtls_ecp_keypair_free( mbedtls_ecp_keypair *key )
{
    if( key == NULL )
        return;

    mbedtls_ecp_group_free( &key->grp );
    mbedtls_mpi_free( &key->d );
    mbedtls_ecp_point_free( &key->Q );
}

/*
 * Copy the contents of a point
 */
int mbedtls_ecp_copy( mbedtls_ecp_point *P, const mbedtls_ecp_point *Q )
{
    int ret;

    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &P->X, &Q->X ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &P->Y, &Q->Y ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &P->Z, &Q->Z ) );

cleanup:
    return( ret );
}

/*
 * Copy the contents of a group object
 */
int mbedtls_ecp_group_copy( mbedtls_ecp_group *dst, const mbedtls_ecp_group *src )
{
    return mbedtls_ecp_group_load( dst, src->id );
}

/*
 * Set point to zero
 */
int mbedtls_ecp_set_zero( mbedtls_ecp_point *pt )
{
    int ret;

    MBEDTLS_MPI_CHK( mbedtls_mpi_lset( &pt->X , 1 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_lset( &pt->Y , 1 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_lset( &pt->Z , 0 ) );

cleanup:
    return( ret );
}

/*
 * Tell if a point is zero
 */
int mbedtls_ecp_is_zero( mbedtls_ecp_point *pt )
{
    return( mbedtls_mpi_cmp_int( &pt->Z, 0 ) == 0 );
}

/*
 * Compare two points lazyly
 */
int mbedtls_ecp_point_cmp( const mbedtls_ecp_point *P,
                           const mbedtls_ecp_point *Q )
{
    if( mbedtls_mpi_cmp_mpi( &P->X, &Q->X ) == 0 &&
        mbedtls_mpi_cmp_mpi( &P->Y, &Q->Y ) == 0 &&
        mbedtls_mpi_cmp_mpi( &P->Z, &Q->Z ) == 0 )
    {
        return( 0 );
    }

    return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
}

/*
 * Import a non-zero point from ASCII strings
 */
int mbedtls_ecp_point_read_string( mbedtls_ecp_point *P, int radix,
                           const char *x, const char *y )
{
    int ret;

    MBEDTLS_MPI_CHK( mbedtls_mpi_read_string( &P->X, radix, x ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_read_string( &P->Y, radix, y ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_lset( &P->Z, 1 ) );

cleanup:
    return( ret );
}

/*
 * Export a point into unsigned binary data (SEC1 2.3.3)
 */
int mbedtls_ecp_point_write_binary( const mbedtls_ecp_group *grp, const mbedtls_ecp_point *P,
                            int format, size_t *olen,
                            unsigned char *buf, size_t buflen )
{
    int ret = 0;
    size_t plen;

    if( format != MBEDTLS_ECP_PF_UNCOMPRESSED &&
        format != MBEDTLS_ECP_PF_COMPRESSED )
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );

    /*
     * Common case: P == 0
     */
    if( mbedtls_mpi_cmp_int( &P->Z, 0 ) == 0 )
    {
        if( buflen < 1 )
            return( MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL );

        buf[0] = 0x00;
        *olen = 1;

        return( 0 );
    }

    plen = mbedtls_mpi_size( &grp->P );

    if( format == MBEDTLS_ECP_PF_UNCOMPRESSED )
    {
        *olen = 2 * plen + 1;

        if( buflen < *olen )
            return( MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL );

        buf[0] = 0x04;
        MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &P->X, buf + 1, plen ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &P->Y, buf + 1 + plen, plen ) );
    }
    else if( format == MBEDTLS_ECP_PF_COMPRESSED )
    {
        *olen = plen + 1;

        if( buflen < *olen )
            return( MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL );

        buf[0] = 0x02 + mbedtls_mpi_get_bit( &P->Y, 0 );
        MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &P->X, buf + 1, plen ) );
    }

cleanup:
    return( ret );
}

/*
 * Import a point from unsigned binary data (SEC1 2.3.4)
 */
int mbedtls_ecp_point_read_binary( const mbedtls_ecp_group *grp, mbedtls_ecp_point *pt,
                           const unsigned char *buf, size_t ilen )
{
    int ret;
    size_t plen;

    if( ilen < 1 )
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );

    if( buf[0] == 0x00 )
    {
        if( ilen == 1 )
            return( mbedtls_ecp_set_zero( pt ) );
        else
            return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
    }

    plen = mbedtls_mpi_size( &grp->P );

    if( buf[0] != 0x04 )
        return( MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE );

    if( ilen != 2 * plen + 1 )
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );

    MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &pt->X, buf + 1, plen ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &pt->Y, buf + 1 + plen, plen ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_lset( &pt->Z, 1 ) );

cleanup:
    return( ret );
}

/*
 * Import a point from a TLS ECPoint record (RFC 4492)
 *      struct {
 *          opaque point <1..2^8-1>;
 *      } ECPoint;
 */
int mbedtls_ecp_tls_read_point( const mbedtls_ecp_group *grp, mbedtls_ecp_point *pt,
                        const unsigned char **buf, size_t buf_len )
{
    unsigned char data_len;
    const unsigned char *buf_start;

    /*
     * We must have at least two bytes (1 for length, at least one for data)
     */
    if( buf_len < 2 )
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );

    data_len = *(*buf)++;
    if( data_len < 1 || data_len > buf_len - 1 )
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );

    /*
     * Save buffer start for read_binary and update buf
     */
    buf_start = *buf;
    *buf += data_len;

    return mbedtls_ecp_point_read_binary( grp, pt, buf_start, data_len );
}

/*
 * Export a point as a TLS ECPoint record (RFC 4492)
 *      struct {
 *          opaque point <1..2^8-1>;
 *      } ECPoint;
 */
int mbedtls_ecp_tls_write_point( const mbedtls_ecp_group *grp, const mbedtls_ecp_point *pt,
                         int format, size_t *olen,
                         unsigned char *buf, size_t blen )
{
    int ret;

    /*
     * buffer length must be at least one, for our length byte
     */
    if( blen < 1 )
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );

    if( ( ret = mbedtls_ecp_point_write_binary( grp, pt, format,
                    olen, buf + 1, blen - 1) ) != 0 )
        return( ret );

    /*
     * write length to the first byte and update total length
     */
    buf[0] = (unsigned char) *olen;
    ++*olen;

    return( 0 );
}

/*
 * Read a group id from an ECParameters record (RFC 4492) and convert it to
 * mbedtls_ecp_group_id.
 */
int mbedtls_ecp_tls_read_group_id( mbedtls_ecp_group_id *grp,
                                   const unsigned char **buf, size_t len )
{
    uint16_t tls_id;
    const mbedtls_ecp_curve_info *curve_info;
    ECP_VALIDATE_RET( grp  != NULL );
    ECP_VALIDATE_RET( buf  != NULL );
    ECP_VALIDATE_RET( *buf != NULL );

    /*
     * We expect at least three bytes (see below)
     */
    if( len < 3 )
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );

    /*
     * First byte is curve_type; only named_curve is handled
     */
    if( *(*buf)++ != MBEDTLS_ECP_TLS_NAMED_CURVE )
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );

    /*
     * Next two bytes are the namedcurve value
     */
    tls_id = *(*buf)++;
    tls_id <<= 8;
    tls_id |= *(*buf)++;

    if( ( curve_info = mbedtls_ecp_curve_info_from_tls_id( tls_id ) ) == NULL )
        return( MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE );

    *grp = curve_info->grp_id;

    return( 0 );
}

/*
 * Set a group from an ECParameters record (RFC 4492)
 */
int mbedtls_ecp_tls_read_group( mbedtls_ecp_group *grp,
                                const unsigned char **buf, size_t len )
{
    int ret; //MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    mbedtls_ecp_group_id grp_id;
    ECP_VALIDATE_RET( grp  != NULL );
    ECP_VALIDATE_RET( buf  != NULL );
    ECP_VALIDATE_RET( *buf != NULL );

    if( ( ret = mbedtls_ecp_tls_read_group_id( &grp_id, buf, len ) ) != 0 )
        return( ret );

    return( mbedtls_ecp_group_load( grp, grp_id ) );
}

/*
 * Write the ECParameters record corresponding to a group (RFC 4492)
 */
int mbedtls_ecp_tls_write_group( const mbedtls_ecp_group *grp, size_t *olen,
                         unsigned char *buf, size_t blen )
{
    const mbedtls_ecp_curve_info *curve_info;

    if( ( curve_info = mbedtls_ecp_curve_info_from_grp_id( grp->id ) ) == NULL )
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );

    /*
     * We are going to write 3 bytes (see below)
     */
    *olen = 3;
    if( blen < *olen )
        return( MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL );

    /*
     * First byte is curve_type, always named_curve
     */
    *buf++ = MBEDTLS_ECP_TLS_NAMED_CURVE;

    /*
     * Next two bytes are the namedcurve value
     */
    buf[0] = curve_info->tls_id >> 8;
    buf[1] = curve_info->tls_id & 0xFF;

    return( 0 );
}


#if defined(ECP_SHORTWEIERSTRASS)


#endif /* ECP_SHORTWEIERSTRASS */

#if defined(ECP_MONTGOMERY)

static void swapbytes(uint8_t *buf, size_t len)
{
    size_t i;
    uint8_t t;
    for(i = 0; i < len/2; i++) {
        t = buf[i];
        buf[i] = buf[len-i-1];
        buf[len-i-1] = t;
    }
}

/*
 * Multiplication with Montgomery ladder in x/z coordinates,
 * for curves in Montgomery form
 */
static int ecp_mul_mxz( mbedtls_ecp_group *grp, mbedtls_ecp_point *R,
                        const mbedtls_mpi *m, const mbedtls_ecp_point *P )
{
    uint32_t status;
    int ret = 0;
    uint8_t point[ECC_MAX_OPERAND_SZ];
    block_t point_blk = BLOCK_T_CONV(point, sizeof(point));
    uint8_t scalar[ECC_MAX_OPERAND_SZ];
    block_t scalar_blk = BLOCK_T_CONV(scalar, sizeof(scalar));
    uint8_t result[ECC_MAX_OPERAND_SZ];
    block_t result_blk = BLOCK_T_CONV(result, sizeof(result));
    const sx_ecc_curve_t *curve = sx_find_ecp_curve(SX_ECP_DP_CURVE25519); //&sx_ecc_curve_curve25519;
    int opsize;

    if (grp->id != MBEDTLS_ECP_DP_CURVE25519)
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;

    opsize = sx_ecc_curve_bytesize(curve);
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &P->X, point, opsize) );
    swapbytes(point, opsize); // convert from big endian to little endian
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary(m, scalar, opsize) );
    swapbytes(scalar, opsize); // convert from big endian to little endian
    sx_enable_clock();
    status = ecc_montgomery_mult(curve->params, curve->command,
                                opsize,
                                scalar_blk, point_blk, result_blk);
    sx_disable_clock();
    if (status != CRYPTOLIB_SUCCESS)
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA; // TODO error code ?

    swapbytes(result, opsize); // convert from little endian to big endian
    MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &R->X, result, opsize) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_lset( &R->Z, 1) );

cleanup:
    return ret;
}

#endif /* ECP_MONTGOMERY */

#if defined(ECP_SHORTWEIERSTRASS)
static int ecp_mul_comb( mbedtls_ecp_group *grp, mbedtls_ecp_point *R,
             const mbedtls_mpi *m, const mbedtls_ecp_point *P )
{
    uint32_t ret = CRYPTOLIB_INVALID_PARAM;
    uint8_t m_bytes[ECC_MAX_OPERAND_SZ];
    block_t m_blk = BLOCK_T_CONV(m_bytes, sizeof(m_bytes));
    uint8_t p[ECC_MAX_OPERAND_SZ * 2];
    block_t p_blk = BLOCK_T_CONV(p, sizeof(p));
    uint8_t r[ECC_MAX_OPERAND_SZ * 2];
    block_t r_blk = BLOCK_T_CONV(r, sizeof(r));
    const sx_ecc_curve_t *curve = find_ecp_curve(grp->id);
    uint32_t klen;

    if (!curve)
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;

    klen = sx_ecc_curve_bytesize(curve);
    MBEDTLS_MPI_CHK( convert_pt_ecp_cryptosoc_local(P, p_blk, klen) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary(m, m_bytes, klen) );
    // dh_common_key_ecdh() is equivalent to a ECC point multiplication
    sx_enable_clock();
    ret = dh_common_key_ecdh(curve->params, m_blk, p_blk, r_blk, klen, curve->command);
    sx_disable_clock();
    if (ret)
        return MBEDTLS_ERR_ECP_HW_ACCEL_FAILED;
    MBEDTLS_MPI_CHK( convert_pt_cryptosoc_ecp_local(R, r_blk, klen) );

cleanup:
    return ret;
}
#endif

/*
 * Multiplication R = m * P
 */
int mbedtls_ecp_mul( mbedtls_ecp_group *grp, mbedtls_ecp_point *R,
             const mbedtls_mpi *m, const mbedtls_ecp_point *P,
             int (*f_rng)(void *, unsigned char *, size_t), void *p_rng )
{
    uint32_t ret = CRYPTOLIB_INVALID_PARAM;

    (void)f_rng;
    (void)p_rng;

    /* Common sanity checks */
    if( mbedtls_mpi_cmp_int( &P->Z, 1 ) != 0 )
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
    if( ( ret = mbedtls_ecp_check_privkey( grp, m ) ) != 0 ||
        ( ret = mbedtls_ecp_check_pubkey( grp, P ) ) != 0 )
        return( ret );

    #if defined(ECP_MONTGOMERY)
    if( ecp_get_type( grp ) == ECP_TYPE_MONTGOMERY )
        ret = ecp_mul_mxz( grp, R, m, P );

    #endif
    #if defined(ECP_SHORTWEIERSTRASS)
        if( ecp_get_type( grp ) == ECP_TYPE_SHORT_WEIERSTRASS )
            ret = ecp_mul_comb( grp, R, m, P );
    #endif

    return (ret == CRYPTOLIB_SUCCESS) ? 0 : MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
}


int mbedtls_ecp_muladd( mbedtls_ecp_group *grp, mbedtls_ecp_point *R,
             const mbedtls_mpi *m, const mbedtls_ecp_point *P,
             const mbedtls_mpi *n, const mbedtls_ecp_point *Q )
{
    uint32_t status;
    int ret = 0;
    uint8_t p_bytes[ECC_MAX_OPERAND_SZ * 2];
    uint8_t q_bytes[ECC_MAX_OPERAND_SZ * 2];
    uint8_t m_bytes[ECC_MAX_OPERAND_SZ];
    uint8_t n_bytes[ECC_MAX_OPERAND_SZ];
    uint8_t r_bytes[ECC_MAX_OPERAND_SZ * 2];
    block_t p_blk = BLOCK_T_CONV(p_bytes, sizeof(p_bytes));
    block_t q_blk = BLOCK_T_CONV(q_bytes, sizeof(q_bytes));
    block_t m_blk = BLOCK_T_CONV(m_bytes, sizeof(m_bytes));
    block_t n_blk = BLOCK_T_CONV(n_bytes, sizeof(n_bytes));
    block_t r_blk = BLOCK_T_CONV(r_bytes, sizeof(r_bytes));
    uint32_t klen;
    const sx_ecc_curve_t *curve;

    if( ecp_get_type( grp ) != ECP_TYPE_SHORT_WEIERSTRASS )
        return( MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE );
    curve = find_ecp_curve(grp->id);
    if (!curve)
        return( MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE );

    if( ( status = mbedtls_ecp_check_privkey( grp, m ) ) != 0 )
        return( status );
    if (( status = mbedtls_ecp_check_pubkey( grp, P ) ) != 0 )
        return( status );
    if( ( status = mbedtls_ecp_check_privkey( grp, n ) ) != 0 )
        return( status );
    if (( status = mbedtls_ecp_check_pubkey( grp, Q ) ) != 0 )
        return( status );

    klen = sx_ecc_curve_bytesize(curve);
    MBEDTLS_MPI_CHK( convert_pt_ecp_cryptosoc_local(P, p_blk, klen) );
    MBEDTLS_MPI_CHK( convert_pt_ecp_cryptosoc_local(Q, q_blk, klen) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( m, m_bytes, klen) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( n, n_bytes, klen) );
    status = sx_ecc_pt_muladd(curve->params, m_blk, p_blk,
        n_blk, q_blk, r_blk, klen, curve->command);
    if (status != CRYPTOLIB_SUCCESS)
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    MBEDTLS_MPI_CHK( convert_pt_cryptosoc_ecp_local(R, r_blk, klen) );

cleanup:
    return ret;
}


#if defined(ECP_MONTGOMERY)
/*
 * Check validity of a public key for Montgomery curves with x-only schemes
 */
static int ecp_check_pubkey_mx( const mbedtls_ecp_group *grp, const mbedtls_ecp_point *pt )
{
    /* [Curve25519 p. 5] Just check X is the correct number of bytes */
    if( mbedtls_mpi_size( &pt->X ) > ( grp->nbits + 7 ) / 8 )
        return( MBEDTLS_ERR_ECP_INVALID_KEY );

    return( 0 );
}
#endif /* ECP_MONTGOMERY */




/*
 * Check that a point is valid as a public key
 */
int mbedtls_ecp_check_pubkey( const mbedtls_ecp_group *grp, const mbedtls_ecp_point *pt )
{
    uint32_t status;
    uint8_t keymaterial[ECC_MAX_OPERAND_SZ * 2];
    block_t pubkey_blk = BLOCK_T_CONV(keymaterial, sizeof(keymaterial));
    uint32_t klen;
    const sx_ecc_curve_t *curve;

    /* Must use affine coordinates */
    if( mbedtls_mpi_cmp_int( &pt->Z, 1 ) != 0 )
        return( MBEDTLS_ERR_ECP_INVALID_KEY );

#if defined(ECP_MONTGOMERY)
    if( ecp_get_type( grp ) == ECP_TYPE_MONTGOMERY ) {
        return( ecp_check_pubkey_mx( grp, pt ) );
    }
#endif
#if defined(ECP_SHORTWEIERSTRASS)
    if( ecp_get_type( grp ) != ECP_TYPE_SHORT_WEIERSTRASS )
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
#endif

    /* pt coordinates must be normalized for our checks */
    if( mbedtls_mpi_cmp_int( &pt->X, 0 ) < 0 ||
        mbedtls_mpi_cmp_int( &pt->Y, 0 ) < 0 ||
        mbedtls_mpi_cmp_mpi( &pt->X, &grp->P ) >= 0 ||
        mbedtls_mpi_cmp_mpi( &pt->Y, &grp->P ) >= 0 )
        return( MBEDTLS_ERR_ECP_INVALID_KEY );

    curve = find_ecp_curve(grp->id);
    if (!curve)
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    klen = sx_ecc_curve_bytesize(curve);

    convert_pt_ecp_cryptosoc_local(pt, pubkey_blk, klen);

    sx_enable_clock();

    status = ecc_validate_key(curve->params, pubkey_blk, klen, curve->command);

    sx_disable_clock();

    if (status)
        return MBEDTLS_ERR_ECP_INVALID_KEY;
    return 0;
}

/*
 * Check that an mbedtls_mpi is valid as a private key
 */
int mbedtls_ecp_check_privkey( const mbedtls_ecp_group *grp, const mbedtls_mpi *d )
{
#if defined(ECP_MONTGOMERY)
    if( ecp_get_type( grp ) == ECP_TYPE_MONTGOMERY )
    {
        /* see [Curve25519] page 5 */
        if( mbedtls_mpi_get_bit( d, 0 ) != 0 ||
            mbedtls_mpi_get_bit( d, 1 ) != 0 ||
            mbedtls_mpi_get_bit( d, 2 ) != 0 ||
            mbedtls_mpi_bitlen( d ) - 1 != grp->nbits ) /* mbedtls_mpi_bitlen is one-based! */
            return( MBEDTLS_ERR_ECP_INVALID_KEY );
        else
            return( 0 );
    }
#endif /* ECP_MONTGOMERY */
#if defined(ECP_SHORTWEIERSTRASS)
    if( ecp_get_type( grp ) == ECP_TYPE_SHORT_WEIERSTRASS )
    {
        /* see SEC1 3.2 */
        if( mbedtls_mpi_cmp_int( d, 1 ) < 0 ||
            mbedtls_mpi_cmp_mpi( d, &grp->N ) >= 0 )
            return( MBEDTLS_ERR_ECP_INVALID_KEY );
        else
            return( 0 );
    }
#endif /* ECP_SHORTWEIERSTRASS */

    return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
}

/*
 * Generate a keypair with configurable base point
 */
int mbedtls_ecp_gen_keypair_base( mbedtls_ecp_group *grp,
                     const mbedtls_ecp_point *G,
                     mbedtls_mpi *d, mbedtls_ecp_point *Q,
                     int (*f_rng)(void *, unsigned char *, size_t),
                     void *p_rng )
{
    int ret;
    size_t n_size = ( grp->nbits + 7 ) / 8;

#if defined(ECP_MONTGOMERY)
    if( ecp_get_type( grp ) == ECP_TYPE_MONTGOMERY )
    {
        /* [M225] page 5 */
        size_t b;

        do {
            MBEDTLS_MPI_CHK( mbedtls_mpi_fill_random( d, n_size, f_rng, p_rng ) );
        } while( mbedtls_mpi_bitlen( d ) == 0);

        /* Make sure the most significant bit is nbits */
        b = mbedtls_mpi_bitlen( d ) - 1; /* mbedtls_mpi_bitlen is one-based */
        if( b > grp->nbits )
            MBEDTLS_MPI_CHK( mbedtls_mpi_shift_r( d, b - grp->nbits ) );
        else
            MBEDTLS_MPI_CHK( mbedtls_mpi_set_bit( d, grp->nbits, 1 ) );

        /* Make sure the last three bits are unset */
        MBEDTLS_MPI_CHK( mbedtls_mpi_set_bit( d, 0, 0 ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_set_bit( d, 1, 0 ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_set_bit( d, 2, 0 ) );
    }
    else
#endif /* ECP_MONTGOMERY */
#if defined(ECP_SHORTWEIERSTRASS)
    if( ecp_get_type( grp ) == ECP_TYPE_SHORT_WEIERSTRASS )
    {
        /* SEC1 3.2.1: Generate d such that 1 <= n < N */
        int count = 0;
        unsigned char rnd[MBEDTLS_ECP_MAX_BYTES];

        /*
         * Match the procedure given in RFC 6979 (deterministic ECDSA):
         * - use the same byte ordering;
         * - keep the leftmost nbits bits of the generated octet string;
         * - try until result is in the desired range.
         * This also avoids any biais, which is especially important for ECDSA.
         */
        do
        {
            MBEDTLS_MPI_CHK( f_rng( p_rng, rnd, n_size ) );
            MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( d, rnd, n_size ) );
            MBEDTLS_MPI_CHK( mbedtls_mpi_shift_r( d, 8 * n_size - grp->nbits ) );

            /*
             * Each try has at worst a probability 1/2 of failing (the msb has
             * a probability 1/2 of being 0, and then the result will be < N),
             * so after 30 tries failure probability is a most 2**(-30).
             *
             * For most curves, 1 try is enough with overwhelming probability,
             * since N starts with a lot of 1s in binary, but some curves
             * such as secp224k1 are actually very close to the worst case.
             */
            if( ++count > 30 )
                return( MBEDTLS_ERR_ECP_RANDOM_FAILED );
        }
        while( mbedtls_mpi_cmp_int( d, 1 ) < 0 ||
               mbedtls_mpi_cmp_mpi( d, &grp->N ) >= 0 );
    }
    else
#endif /* ECP_SHORTWEIERSTRASS */
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );

cleanup:
    if( ret != 0 )
        return( ret );

    return( mbedtls_ecp_mul( grp, Q, d, G, f_rng, p_rng ) );
}

/*
 * Generate key pair, wrapper for conventional base point
 */
int mbedtls_ecp_gen_keypair( mbedtls_ecp_group *grp,
                             mbedtls_mpi *d, mbedtls_ecp_point *Q,
                             int (*f_rng)(void *, unsigned char *, size_t),
                             void *p_rng )
{
    return( mbedtls_ecp_gen_keypair_base( grp, &grp->G, d, Q, f_rng, p_rng ) );
}

/*
 * Generate a keypair, prettier wrapper
 */
int mbedtls_ecp_gen_key( mbedtls_ecp_group_id grp_id, mbedtls_ecp_keypair *key,
                int (*f_rng)(void *, unsigned char *, size_t), void *p_rng )
{
    int ret;

    if( ( ret = mbedtls_ecp_group_load( &key->grp, grp_id ) ) != 0 )
        return( ret );

    return( mbedtls_ecp_gen_keypair( &key->grp, &key->d, &key->Q, f_rng, p_rng ) );
}

/*
 * Check a public-private key pair
 */
int mbedtls_ecp_check_pub_priv( const mbedtls_ecp_keypair *pub, const mbedtls_ecp_keypair *prv )
{
    int ret;
    mbedtls_ecp_point Q;
    mbedtls_ecp_group grp;

    if( pub->grp.id == MBEDTLS_ECP_DP_NONE ||
        pub->grp.id != prv->grp.id ||
        mbedtls_mpi_cmp_mpi( &pub->Q.X, &prv->Q.X ) ||
        mbedtls_mpi_cmp_mpi( &pub->Q.Y, &prv->Q.Y ) ||
        mbedtls_mpi_cmp_mpi( &pub->Q.Z, &prv->Q.Z ) )
    {
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
    }

    mbedtls_ecp_point_init( &Q );
    mbedtls_ecp_group_init( &grp );

    /* mbedtls_ecp_mul() needs a non-const group... */
    mbedtls_ecp_group_copy( &grp, &prv->grp );

    /* Also checks d is valid */
    MBEDTLS_MPI_CHK( mbedtls_ecp_mul( &grp, &Q, &prv->d, &prv->grp.G, NULL, NULL ) );

    if( mbedtls_mpi_cmp_mpi( &Q.X, &prv->Q.X ) ||
        mbedtls_mpi_cmp_mpi( &Q.Y, &prv->Q.Y ) ||
        mbedtls_mpi_cmp_mpi( &Q.Z, &prv->Q.Z ) )
    {
        ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
        goto cleanup;
    }

cleanup:
    mbedtls_ecp_point_free( &Q );
    mbedtls_ecp_group_free( &grp );

    return( ret );
}


#if defined(MBEDTLS_ECP_DP_CURVE25519_ENABLED)
/*
 * Specialized function for creating the Curve25519 group
 */
static int ecp_use_curve25519( mbedtls_ecp_group *grp )
{
    int ret;

    /* Actually ( A + 2 ) / 4 */
    MBEDTLS_MPI_CHK( mbedtls_mpi_read_string( &grp->A, 16, "01DB42" ) );

    /* P = 2^255 - 19 */
    MBEDTLS_MPI_CHK( mbedtls_mpi_lset( &grp->P, 1 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_shift_l( &grp->P, 255 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_sub_int( &grp->P, &grp->P, 19 ) );
    grp->pbits = mbedtls_mpi_bitlen( &grp->P );

    /* Y intentionaly not set, since we use x/z coordinates.
     * This is used as a marker to identify Montgomery curves! */
    MBEDTLS_MPI_CHK( mbedtls_mpi_lset( &grp->G.X, 9 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_lset( &grp->G.Z, 1 ) );
    mbedtls_mpi_free( &grp->G.Y );

    /* Actually, the required msb for private keys */
    grp->nbits = 254;

cleanup:
    if( ret != 0 )
        mbedtls_ecp_group_free( grp );

    return( ret );
}
#endif /* MBEDTLS_ECP_DP_CURVE25519_ENABLED */


/*
 * Make group available from embedded constants
 */
int mbedtls_ecp_group_load( mbedtls_ecp_group *grp, mbedtls_ecp_group_id id )
{
    const sx_ecc_curve_t *curve;
    size_t opsize;

    mbedtls_ecp_group_free( grp );
    curve = find_ecp_curve(id);
    if (!curve) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    mbedtls_ecp_group_init(grp);

    grp->id = id;
    opsize = (curve->bitsize + 7)/ 8;

    #if defined(MBEDTLS_ECP_DP_CURVE25519_ENABLED)
    if (grp->id == MBEDTLS_ECP_DP_CURVE25519) {
            return( ecp_use_curve25519( grp ) );
    }
    #endif /* MBEDTLS_ECP_DP_CURVE25519_ENABLED */

    /* Cryptosoc library uses a large big endian representation. But mbedtls
     * mpi uses an array on mpi_uint (typically 32, 64 or 128 bits)
     * which have CPU endianness.
     *
     * Future improvement: keep and reuse mpi representations
     */
    mbedtls_mpi_init(&grp->P);
    mbedtls_mpi_read_binary( &grp->P, curve->params.addr + 0 * opsize, opsize);
    mbedtls_mpi_init(&grp->A);
    mbedtls_mpi_read_binary( &grp->A, curve->params.addr + 4 * opsize, opsize);
    mbedtls_mpi_init(&grp->B);
    mbedtls_mpi_read_binary( &grp->B, curve->params.addr + 5 * opsize, opsize);
    mbedtls_mpi_init(&grp->N);
    mbedtls_mpi_read_binary( &grp->N, curve->params.addr + 1 * opsize, opsize);
    mbedtls_ecp_point_init(&grp->G);
    mbedtls_mpi_read_binary( &grp->G.X, curve->params.addr + 2 * opsize, opsize);
    mbedtls_mpi_read_binary( &grp->G.Y, curve->params.addr + 3 * opsize, opsize);
    mbedtls_mpi_lset( &grp->G.Z, 1);

    grp->pbits = mbedtls_mpi_bitlen( &grp->P );
    grp->nbits = mbedtls_mpi_bitlen( &grp->N );

    return 0;
}

#if defined(MBEDTLS_SELF_TEST)

/*
 * Checkup routine
 */
int mbedtls_ecp_self_test( int verbose )
{
    int ret;
    size_t i;
    mbedtls_ecp_group grp;
    mbedtls_ecp_point R, P;
    mbedtls_mpi m;
    unsigned long add_c_prev, dbl_c_prev, mul_c_prev;
    /* exponents especially adapted for secp192r1 */
    const char *exponents[] =
    {
        "000000000000000000000000000000000000000000000001", /* one */
        "FFFFFFFFFFFFFFFFFFFFFFFF99DEF836146BC9B1B4D22830", /* N - 1 */
        "5EA6F389A38B8BC81E767753B15AA5569E1782E30ABE7D25", /* random */
        "400000000000000000000000000000000000000000000000", /* one and zeros */
        "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", /* all ones */
        "555555555555555555555555555555555555555555555555", /* 101010... */
    };

    mbedtls_ecp_group_init( &grp );
    mbedtls_ecp_point_init( &R );
    mbedtls_ecp_point_init( &P );
    mbedtls_mpi_init( &m );

    /* Use secp192r1 if available, or any available curve */
#if defined(MBEDTLS_ECP_DP_SECP192R1_ENABLED)
    MBEDTLS_MPI_CHK( mbedtls_ecp_group_load( &grp, MBEDTLS_ECP_DP_SECP192R1 ) );
#else
    MBEDTLS_MPI_CHK( mbedtls_ecp_group_load( &grp, mbedtls_ecp_curve_list()->grp_id ) );
#endif

    if( verbose != 0 )
        mbedtls_printf( "  ECP test #1 (constant op_count, base point G): " );

    /* Do a dummy multiplication first to trigger precomputation */
    MBEDTLS_MPI_CHK( mbedtls_mpi_lset( &m, 2 ) );
    MBEDTLS_MPI_CHK( mbedtls_ecp_mul( &grp, &P, &m, &grp.G, NULL, NULL ) );

    add_count = 0;
    dbl_count = 0;
    mul_count = 0;
    MBEDTLS_MPI_CHK( mbedtls_mpi_read_string( &m, 16, exponents[0] ) );
    MBEDTLS_MPI_CHK( mbedtls_ecp_mul( &grp, &R, &m, &grp.G, NULL, NULL ) );

    for( i = 1; i < sizeof( exponents ) / sizeof( exponents[0] ); i++ )
    {
        add_c_prev = add_count;
        dbl_c_prev = dbl_count;
        mul_c_prev = mul_count;
        add_count = 0;
        dbl_count = 0;
        mul_count = 0;

        MBEDTLS_MPI_CHK( mbedtls_mpi_read_string( &m, 16, exponents[i] ) );
        MBEDTLS_MPI_CHK( mbedtls_ecp_mul( &grp, &R, &m, &grp.G, NULL, NULL ) );

        if( add_count != add_c_prev ||
            dbl_count != dbl_c_prev ||
            mul_count != mul_c_prev )
        {
            if( verbose != 0 )
                mbedtls_printf( "failed (%u)\n", (unsigned int) i );

            ret = 1;
            goto cleanup;
        }
    }

    if( verbose != 0 )
        mbedtls_printf( "passed\n" );

    if( verbose != 0 )
        mbedtls_printf( "  ECP test #2 (constant op_count, other point): " );
    /* We computed P = 2G last time, use it */

    add_count = 0;
    dbl_count = 0;
    mul_count = 0;
    MBEDTLS_MPI_CHK( mbedtls_mpi_read_string( &m, 16, exponents[0] ) );
    MBEDTLS_MPI_CHK( mbedtls_ecp_mul( &grp, &R, &m, &P, NULL, NULL ) );

    for( i = 1; i < sizeof( exponents ) / sizeof( exponents[0] ); i++ )
    {
        add_c_prev = add_count;
        dbl_c_prev = dbl_count;
        mul_c_prev = mul_count;
        add_count = 0;
        dbl_count = 0;
        mul_count = 0;

        MBEDTLS_MPI_CHK( mbedtls_mpi_read_string( &m, 16, exponents[i] ) );
        MBEDTLS_MPI_CHK( mbedtls_ecp_mul( &grp, &R, &m, &P, NULL, NULL ) );

        if( add_count != add_c_prev ||
            dbl_count != dbl_c_prev ||
            mul_count != mul_c_prev )
        {
            if( verbose != 0 )
                mbedtls_printf( "failed (%u)\n", (unsigned int) i );

            ret = 1;
            goto cleanup;
        }
    }

    if( verbose != 0 )
        mbedtls_printf( "passed\n" );

cleanup:

    if( ret < 0 && verbose != 0 )
        mbedtls_printf( "Unexpected error, return code = %08X\n", ret );

    mbedtls_ecp_group_free( &grp );
    mbedtls_ecp_point_free( &R );
    mbedtls_ecp_point_free( &P );
    mbedtls_mpi_free( &m );

    if( verbose != 0 )
        mbedtls_printf( "\n" );

    return( ret );
}

#endif /* MBEDTLS_SELF_TEST */


#if defined(MBEDTLS_ECDSA_VERIFY_ALT)

int mbedtls_ecdsa_verify( mbedtls_ecp_group *grp,
                  const unsigned char *buf, size_t blen,
                  const mbedtls_ecp_point *Q, const mbedtls_mpi *r, const mbedtls_mpi *s)
{
    const sx_ecc_curve_t *curve;
    uint32_t opsize;
    uint32_t dgst_local_len;
    int ret;
    uint8_t digest[ECC_MAX_KEY_SIZE];
    block_t digest_blk = BLOCK_T_CONV(buf, blen);
    uint8_t extra_bits;
    uint8_t keymaterial[ECC_MAX_OPERAND_SZ * 2];
    block_t pubkey_blk = BLOCK_T_CONV(keymaterial, sizeof(keymaterial));
    uint8_t signature[ECC_MAX_OPERAND_SZ * 2];
    block_t signature_blk = BLOCK_T_CONV(signature, sizeof(keymaterial));

    /* Fail cleanly on curves such as Curve25519 that can't be used for ECDSA */
    if( grp->N.p == NULL )
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );

    curve = find_ecp_curve(grp->id);
    if (!curve) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }
    opsize = sx_ecc_curve_bytesize(curve);

    /* make sure r and s are in range 1..n-1 */
    if( mbedtls_mpi_cmp_int( r, 1 ) < 0 || mbedtls_mpi_cmp_mpi( r, &grp->N ) >= 0 ||
        mbedtls_mpi_cmp_int( s, 1 ) < 0 || mbedtls_mpi_cmp_mpi( s, &grp->N ) >= 0 )
    {
        return MBEDTLS_ERR_ECP_VERIFY_FAILED;
    }

    /* No need to check validity of Q now.
     * ecdsa_signature_verification_digest() will check validity of Q with
     * hardware acceleration. */

    /* Define digest size, shorten as needed.
     * When the curve is smaller than the digest, take only
     * the most significant bytes. When the curve is larger than the digest,
     * leading zeroes will be inserted within ecdsa_signature_verification() */
    dgst_local_len = SX_MIN( opsize, blen );
    digest_blk.len = dgst_local_len;

    sx_enable_clock();

    /* Bitshift if needed, for curve smaller than digest and with order N not
     * on bytes boundaries */
    extra_bits = sx_ecc_curve_bitsize(curve) & 0x7;
    if (extra_bits && blen*8 > sx_ecc_curve_bitsize(curve)) {
        memcpy(digest, buf, dgst_local_len);
        unsigned int shift = 8 - extra_bits;
        uint8_t prev, val;
        int i;
        prev = 0;
        for(i = 0; i < dgst_local_len; i++) {
            val = ((buf[i] >> shift)&0xFF) | prev;
            prev = buf[i] << (8-shift);
            digest[i] = val;
        }
        digest_blk.addr = digest;
    }

    /* convert point Q into cryptolib block_t */
    convert_pt_ecp_cryptosoc_local(Q, pubkey_blk, opsize);
    pubkey_blk.len = opsize * 2;

    /* convert r and s big MPI numbers into one cryptolib block_t */
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( r, signature_blk.addr, opsize) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( s, signature_blk.addr + opsize, opsize) );
    signature_blk.len = opsize * 2;

    /* perform signature verification */
    ret = ecdsa_signature_verification_digest(curve,
                                              digest_blk,
                                              pubkey_blk,
                                              signature_blk);
    sx_disable_clock();

    return( (ret == CRYPTOLIB_SUCCESS) ? 0 : MBEDTLS_ERR_ECP_VERIFY_FAILED);

cleanup:
    return MBEDTLS_ERR_ECP_VERIFY_FAILED;
}

#endif

#if defined(MBEDTLS_ECDSA_SIGN_ALT)


#define CRYPTOLIB_ECDSA_RETRY  9999

static uint32_t sx_ecdsa_attempt_signature(const sx_ecc_curve_t *curve,
    block_t rnd_blk,
    block_t signature)
{
    uint32_t status;
    uint32_t opsize = sx_ecc_curve_bytesize(curve);


    mem2CryptoRAM_rev(rnd_blk, opsize, BA414E_MEMLOC_7);

    /* ECDSA signature generation */
    ba414e_set_command(BA414E_OPTYPE_ECDSA_SIGN_GEN, opsize, BA414E_BIGEND, curve->command);
    status = ba414e_start_wait_status();

    if (status & ~BA414E_STS_SINV_MASK) {
        // Unexpected error
        return CRYPTOLIB_INVALID_SIGN_ERR;
    } else if (status) {
        // Not invertable bit (BA414E_STS_SINV_MASK) set => retry
        return CRYPTOLIB_ECDSA_RETRY;
    }

    // Fetch the results
    CryptoRAM2point_rev(signature, opsize, BA414E_MEMLOC_10);

    return CRYPTOLIB_SUCCESS;
}

/** shift bits to the right in place */
static void sx_bignum_rshift(char *rnd, uint32_t size, int rshift)
{
    char prev = 0;

    if (!rshift)
        return;

    for (int i=0; i < size; i++) {
        char val;
        val = rnd[i];
        rnd[i] = (val >> rshift) | prev;
        prev = (val << (8 - rshift)) & 0xFF;
    }
}

/** Compare 2 big numbers (a, b).
 *
 * Compare 2 big endian numbers represented as 2 arrays of bytes.
 * The return value:
 *    < 0 when a < b
 *    == 0 when a == b
 *    > 0 when a > b
 *
 * Runs as much as possible in constant time.
 */
static int sx_bignum_compare(uint8_t *a, uint8_t *b, uint32_t opsize)
{
    int i;
    int cmp = 0;

    a += opsize;
    b += opsize;
    for (i = 0; i < opsize; i++) {
        int ncmp = (int)*a - (int)*b;
        cmp = ncmp ? ncmp : cmp;
        b--;
        a--;
    }
    return cmp;
}

/** Return true is the big number is zero, false otherwise.
 *
 * Runs in constant time in order to not leak secrets.
 **/
static bool sx_bignum_is_zero(uint8_t *bn, uint32_t opsize)
{
    int r = 0;
    for (; opsize; opsize--) {
        r |= *bn;
        bn++;
    }
    return !r;
}

int mbedtls_ecdsa_sign( mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
                const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
                int (*f_rng)(void *, unsigned char *, size_t), void *p_rng )
{
    const sx_ecc_curve_t *curve;
    uint32_t opsize;
    uint32_t dgst_local_len;
    int ret;
    uint8_t digest[ECC_MAX_KEY_SIZE] ;
    block_t digest_blk = BLOCK_T_CONV(buf, blen);
    uint8_t extra_bits;
    uint8_t keymaterial[ECC_MAX_OPERAND_SZ] ;
    block_t privkey_blk = BLOCK_T_CONV(keymaterial, sizeof(keymaterial));
    uint8_t signature[ECC_MAX_OPERAND_SZ * 2] ;
    block_t signature_blk = BLOCK_T_CONV(signature, sizeof(signature));
    unsigned char rnd[ECC_MAX_KEY_SIZE];
    int retries = 0;
    block_t rnd_blk = BLOCK_T_CONV(rnd, sizeof(rnd));


    /* Fail cleanly on curves such as Curve25519 that can't be used for ECDSA */
    if( grp->N.p == NULL )
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );

    curve = find_ecp_curve(grp->id);
    if (!curve) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }
    opsize = sx_ecc_curve_bytesize(curve);

    /* Define digest size, shorten as needed.
     * When the curve is smaller than the digest, take only
     * the most significant bytes. When the curve is larger than the digest,
     * leading zeroes will be inserted within ecdsa_signature_verification() */
    dgst_local_len = SX_MIN( opsize, blen );
    digest_blk.len = dgst_local_len;

    /* Bitshift if needed, for curve smaller than digest and with order N not
     * on bytes boundaries */
    extra_bits = sx_ecc_curve_bitsize(curve) & 0x7;
    if (extra_bits && blen*8 > sx_ecc_curve_bitsize(curve)) {
        memcpy(digest, buf, dgst_local_len);
        unsigned int shift = 8 - extra_bits;
        uint8_t prev, val;
        int i;
        prev = 0;
        for(i = 0; i < dgst_local_len; i++) {
            val = ((buf[i] >> shift)&0xFF) | prev;
            prev = buf[i] << (8-shift);
            digest[i] = val;
        }
        digest_blk.addr = digest;
    }

    /* Make sure d is in range 1..n-1 */
    if( mbedtls_mpi_cmp_int( d, 1 ) < 0 || mbedtls_mpi_cmp_mpi( d, &grp->N ) >= 0 )
        return( MBEDTLS_ERR_ECP_INVALID_KEY );

    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( d, privkey_blk.addr, opsize) );

    rnd_blk.len = opsize;

    sx_enable_clock();
    sx_ecdsa_signature_configure(curve, digest_blk, privkey_blk);
    sx_disable_clock();

    do {
        f_rng(p_rng, rnd, opsize);
        if (extra_bits)
            sx_bignum_rshift((char*)rnd, opsize, 8 - extra_bits);
        ret = CRYPTOLIB_ECDSA_RETRY;
        /* if rnd is in the range [1, n-1] => attempt signature */
        if (!sx_bignum_is_zero(rnd, opsize)
            && sx_bignum_compare(rnd, curve->params.addr + opsize, opsize) < 0) {
            sx_enable_clock();
            ret = sx_ecdsa_attempt_signature(curve, rnd_blk, signature_blk);
            sx_disable_clock();
        }
    } while ((ret == CRYPTOLIB_ECDSA_RETRY) && retries++ < 30);

    if (ret != CRYPTOLIB_SUCCESS)
    {
        return MBEDTLS_ERR_ECP_RANDOM_FAILED;
    }
    ret = 0;
    MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( r, signature_blk.addr, opsize) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( s, signature_blk.addr + opsize, opsize) );

cleanup:
    return( ret );
}


#endif

#endif /* MBEDTLS_ECP_ALT */

#endif /* MBEDTLS_ECP_C */
