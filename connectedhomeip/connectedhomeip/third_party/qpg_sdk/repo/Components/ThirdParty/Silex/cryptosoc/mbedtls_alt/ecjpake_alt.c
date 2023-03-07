/*
 *  Elliptic curve J-PAKE
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
 * References in the code are to the Thread v1.0 Specification,
 * available to members of the Thread Group http://threadgroup.org/
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_ECJPAKE_C)
#include <sx_generic.h>


#include "mbedtls/ecjpake.h"
#include <sx_jpake_alg.h>
#include <sx_ecc_curves.h>
#include <ba414e_config.h>
#include <cryptodma.h>
#include <string.h>

#if defined(MBEDTLS_ECJPAKE_ALT)

#define MBEDTLS_ECJPAKE_POINT_BYTES (2 * MBEDTLS_ECJPAKE_MAX_BYTES)

/*
 * Convert a mbedtls_ecjpake_role to identifier string
 */
static const char * const ecjpake_id[] = {
    "client",
    "server"
};

#define ID_MINE     ( ecjpake_id[ ctx->role ] )
#define ID_PEER     ( ecjpake_id[ 1 - ctx->role ] )

/** Convert a mbedtls_ecp_point point into a block_t representation for cryptosoc */
static int convert_pt_ecp_cryptosoc(const mbedtls_ecp_point *pt, block_t sx_pt,
    int byteslen)
{
    int ret = 0;

    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &pt->X, sx_pt.addr, byteslen) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &pt->Y, sx_pt.addr + byteslen, byteslen) );
cleanup:
    return ret;
}

/** Convert a cryptosoc block_t point representation into a mbedtls_ecp_point */
static int convert_pt_cryptosoc_ecp(mbedtls_ecp_point *pt, block_t sx_pt,
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
        return sx_find_ecp_curve_flash(SX_ECP_DP_SECP192R1);
    case MBEDTLS_ECP_DP_SECP224R1:
        //return &sx_ecc_curve_p224;
        return sx_find_ecp_curve_flash(SX_ECP_DP_SECP224R1);
    case MBEDTLS_ECP_DP_SECP256R1:
        //return &sx_ecc_curve_p256;
        return sx_find_ecp_curve_flash(SX_ECP_DP_SECP256R1);
    case MBEDTLS_ECP_DP_SECP384R1:
        //return &sx_ecc_curve_p384;
        return sx_find_ecp_curve_flash(SX_ECP_DP_SECP384R1);
    case MBEDTLS_ECP_DP_SECP521R1:
        //return &sx_ecc_curve_p521;
        return sx_find_ecp_curve_flash(SX_ECP_DP_SECP521R1);
    case MBEDTLS_ECP_DP_CURVE25519:
        //return &sx_ecc_curve_curve25519;
        return sx_find_ecp_curve_flash(SX_ECP_DP_CURVE25519);
    default:
        return NULL;
    }
}

/*
 * Initialize context
 */
void mbedtls_ecjpake_init( mbedtls_ecjpake_context *ctx )
{
    if( ctx == NULL )
        return;

    ctx->hashalg = 0;
    mbedtls_ecp_group_init( &ctx->grp );
    ctx->point_format = MBEDTLS_ECP_PF_UNCOMPRESSED;

    mbedtls_ecp_point_init( &ctx->Xm1 );
    mbedtls_ecp_point_init( &ctx->Xm2 );
    mbedtls_ecp_point_init( &ctx->Xp1 );
    mbedtls_ecp_point_init( &ctx->Xp2 );
    mbedtls_ecp_point_init( &ctx->Xp  );

    mbedtls_mpi_init( &ctx->xm1 );
    mbedtls_mpi_init( &ctx->xm2 );
    mbedtls_mpi_init( &ctx->s   );
}

/*
 * Free context
 */
void mbedtls_ecjpake_free( mbedtls_ecjpake_context *ctx )
{
    if( ctx == NULL )
        return;

    ctx->hashalg = 0;
    mbedtls_ecp_group_free( &ctx->grp );

    mbedtls_ecp_point_free( &ctx->Xm1 );
    mbedtls_ecp_point_free( &ctx->Xm2 );
    mbedtls_ecp_point_free( &ctx->Xp1 );
    mbedtls_ecp_point_free( &ctx->Xp2 );
    mbedtls_ecp_point_free( &ctx->Xp  );

    mbedtls_mpi_free( &ctx->xm1 );
    mbedtls_mpi_free( &ctx->xm2 );
    mbedtls_mpi_free( &ctx->s   );
}

/*
 * Setup context
 */
int mbedtls_ecjpake_setup( mbedtls_ecjpake_context *ctx,
                           mbedtls_ecjpake_role role,
                           mbedtls_md_type_t hash,
                           mbedtls_ecp_group_id curve,
                           const unsigned char *secret,
                           size_t len )
{
    int ret;
    uint32_t opsize;

    ctx->role = role;

    switch (hash ) {
    case MBEDTLS_MD_SHA1:
        ctx->hashalg = e_SHA1;
        break;
    case MBEDTLS_MD_SHA224:
        ctx->hashalg = e_SHA224;
        break;
    case MBEDTLS_MD_SHA256:
        ctx->hashalg = e_SHA256;
        break;
    case MBEDTLS_MD_SHA384:
        ctx->hashalg = e_SHA384;
        break;
    case MBEDTLS_MD_SHA512:
        ctx->hashalg = e_SHA512;
        break;
    default:
        return( MBEDTLS_ERR_MD_FEATURE_UNAVAILABLE );
    }
    /* Only allow SHA256 as it's the only hash algorithm to be validated
     * and the hash is hardcoded to SHA256 in parts of JPAKE CryptoSoC.*/
    if (ctx->hashalg != e_SHA256)
        return ( MBEDTLS_ERR_MD_FEATURE_UNAVAILABLE );

    MBEDTLS_MPI_CHK( mbedtls_ecp_group_load( &ctx->grp, curve ) );

    MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &ctx->s, secret, len ) );

    ctx->curve = find_ecp_curve( curve );
    if ( !ctx->curve )
        ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    opsize = sx_ecc_curve_bytesize(ctx->curve);
    if (opsize > MBEDTLS_ECJPAKE_MAX_BYTES)
        ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    if (len > opsize)
        ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;

cleanup:
    if( ret != 0 )
        mbedtls_ecjpake_free( ctx );

    return( ret );
}

/*
 * Check if context is ready for use
 */
int mbedtls_ecjpake_check( const mbedtls_ecjpake_context *ctx )
{
    if( ctx->hashalg != e_SHA256 ||
        ctx->grp.id == MBEDTLS_ECP_DP_NONE ||
        ctx->s.p == NULL ||
        ctx->curve == NULL )
    {
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
    }

    return( 0 );
}


/*
 * Parse a ECShnorrZKP (7.4.2.2.2) and verify it (7.4.2.3.3)
 */
static int ecjpake_zkp_read( const mbedtls_ecjpake_context *ctx,
                             const mbedtls_ecp_point *G,
                             const mbedtls_ecp_point *X,
                             const char *id,
                             const unsigned char **p,
                             const unsigned char *end )
{
    int ret;
    mbedtls_ecp_point V;
    size_t r_len;

    mbedtls_ecp_point_init( &V );

    /*
     * struct {
     *     ECPoint V;
     *     opaque r<1..2^8-1>;
     * } ECSchnorrZKP;
     */
    if( end < *p )
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
    MBEDTLS_MPI_CHK( mbedtls_ecp_tls_read_point( &ctx->grp, &V, p, end - *p ) );

    if( end < *p || (size_t)( end - *p ) < 1 )
    {
        ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
        goto cleanup;
    }

    r_len = *(*p)++;

    if( end < *p || (size_t)( end - *p ) < r_len )
    {
        ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
        goto cleanup;
    }

    block_t r_blk = BLOCK_T_CONV(*p, r_len);
    *p += r_len;

    /*
     * Verification
     */
    if (!r_len)
        return MBEDTLS_ERR_ECP_INVALID_KEY;

    uint32_t opsize = sx_ecc_curve_bytesize(ctx->curve);
    uint32_t status;
    uint8_t Gmem[MBEDTLS_ECJPAKE_POINT_BYTES] ;
    block_t G_blk = BLOCK_T_CONV(Gmem, 2*opsize);
    uint8_t Vmem[MBEDTLS_ECJPAKE_POINT_BYTES] ;
    block_t V_blk = BLOCK_T_CONV(Vmem, 2*opsize);
    uint8_t Xmem[MBEDTLS_ECJPAKE_POINT_BYTES] ;
    block_t X_blk = BLOCK_T_CONV(Xmem, 2*opsize);
    block_t id_blk = BLOCK_T_CONV(id, strlen(id));

    MBEDTLS_MPI_CHK( convert_pt_ecp_cryptosoc(G, G_blk, opsize) );
    MBEDTLS_MPI_CHK( convert_pt_ecp_cryptosoc(X, X_blk, opsize) );
    MBEDTLS_MPI_CHK( convert_pt_ecp_cryptosoc(&V, V_blk, opsize) );

    sx_enable_clock();
    ba414e_set_command(0, opsize, BA414E_BIGEND, 0);
    ret = ba414e_load_curve(ctx->curve->params, opsize, BA414E_BIGEND, 0);
    if (ret) {
        ret = MBEDTLS_ERR_ECP_HW_ACCEL_FAILED;
        goto cleanup;
    }
    status = jpake_hash_verify_zkp(G_blk, ~0, X_blk, V_blk, r_blk, id_blk,
                                   ctx->curve->command, opsize);

    sx_disable_clock();

    if (status == CRYPTOLIB_CRYPTO_ERR)
        ret = MBEDTLS_ERR_ECP_HW_ACCEL_FAILED;
    else if (status == CRYPTOLIB_INVALID_POINT)
        ret = MBEDTLS_ERR_ECP_INVALID_KEY;
    else if (status)
        ret = MBEDTLS_ERR_ECP_VERIFY_FAILED;

cleanup:
    mbedtls_ecp_point_free( &V );

    return( ret );
}



/*
 * Generate ZKP (7.4.2.3.2) and write it as ECSchnorrZKP (7.4.2.2.2)
 */
static int ecjpake_zkp_write( const mbedtls_ecjpake_context *ctx,
                              const mbedtls_ecp_point *G,
                              const mbedtls_mpi *x,
                              const mbedtls_ecp_point *X,
                              const char *id,
                              unsigned char **p,
                              const unsigned char *end,
                              int (*f_rng)(void *, unsigned char *, size_t),
                              void *p_rng )
{
    int ret;
    mbedtls_ecp_point V;
    mbedtls_mpi v;
    size_t len;
    uint32_t opsize = sx_ecc_curve_bytesize(ctx->curve);

    if( end < *p )
        return( MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL );

    mbedtls_ecp_point_init( &V );
    mbedtls_mpi_init( &v );

    /* Compute signature */
    MBEDTLS_MPI_CHK( mbedtls_ecp_gen_keypair_base( (mbedtls_ecp_group *)&ctx->grp,
                                                   G, &v, &V, f_rng, p_rng ) );
    /* Write it out */
    MBEDTLS_MPI_CHK( mbedtls_ecp_tls_write_point( &ctx->grp, &V,
                ctx->point_format, &len, *p, end - *p ) );
    *p += len;
    if( *p + opsize > end )
        return( MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL );

    /* ZKP based on x, X, v, V */
    *(*p)++ = (unsigned char)( opsize & 0xFF );

    uint8_t vmem[MBEDTLS_ECJPAKE_MAX_BYTES];
    block_t v_blk = BLOCK_T_CONV(vmem, opsize);
    uint8_t Gmem[MBEDTLS_ECJPAKE_POINT_BYTES];
    block_t G_blk = BLOCK_T_CONV(Gmem, 2*opsize);
    uint8_t Vmem[MBEDTLS_ECJPAKE_POINT_BYTES];
    block_t V_blk = BLOCK_T_CONV(Vmem, 2*opsize);
    uint8_t xmem[MBEDTLS_ECJPAKE_MAX_BYTES];
    block_t x_blk = BLOCK_T_CONV(xmem, opsize);
    uint8_t Xmem[MBEDTLS_ECJPAKE_POINT_BYTES];
    block_t X_blk = BLOCK_T_CONV(Xmem, 2*opsize);
    block_t r_blk = BLOCK_T_CONV(*p, opsize);
    block_t userid_blk = BLOCK_T_CONV(id, strlen(id));

    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary(&v, v_blk.addr, opsize) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary(x, x_blk.addr, opsize) );
    MBEDTLS_MPI_CHK( convert_pt_ecp_cryptosoc(G, G_blk, opsize) );
    MBEDTLS_MPI_CHK( convert_pt_ecp_cryptosoc(X, X_blk, opsize) );
    MBEDTLS_MPI_CHK( convert_pt_ecp_cryptosoc(&V, V_blk, opsize) );

    sx_enable_clock();
    ret = ba414e_load_curve(ctx->curve->params, opsize, BA414E_BIGEND, 0);
    if (ret) {
        ret = MBEDTLS_ERR_ECP_HW_ACCEL_FAILED;
        goto cleanup;
    }

    ret = jpake_hash_create_zkp(G_blk, x_blk, X_blk, v_blk, V_blk,
                            userid_blk, r_blk,
                            ctx->curve->command, opsize);
    sx_disable_clock();

    if (ret)
        ret = MBEDTLS_ERR_ECP_HW_ACCEL_FAILED;
    *p += opsize;

cleanup:
    mbedtls_ecp_point_free( &V );
    mbedtls_mpi_free( &v );

    return( ret );
}

/*
 * Parse a ECJPAKEKeyKP (7.4.2.2.1) and check proof
 * Output: verified public key X
 */
static int ecjpake_kkp_read( const mbedtls_ecjpake_context *ctx,
                             const mbedtls_ecp_point *G,
                             mbedtls_ecp_point *X,
                             const char *id,
                             const unsigned char **p,
                             const unsigned char *end )
{
    int ret;

    if( end < *p )
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );

    /*
     * struct {
     *     ECPoint X;
     *     ECSchnorrZKP zkp;
     * } ECJPAKEKeyKP;
     */
    MBEDTLS_MPI_CHK( mbedtls_ecp_tls_read_point( &ctx->grp, X, p, end - *p ) );
    if( mbedtls_ecp_is_zero( X ) )
    {
        ret = MBEDTLS_ERR_ECP_INVALID_KEY;
        goto cleanup;
    }

    MBEDTLS_MPI_CHK( ecjpake_zkp_read( ctx, G, X, id, p, end ) );

cleanup:
    return( ret );
}

/*
 * Generate an ECJPAKEKeyKP
 * Output: the serialized structure, plus private/public key pair
 */
static int ecjpake_kkp_write( const mbedtls_ecjpake_context *ctx,
                              const mbedtls_ecp_point *G,
                              mbedtls_mpi *x,
                              mbedtls_ecp_point *X,
                              const char *id,
                              unsigned char **p,
                              const unsigned char *end,
                              int (*f_rng)(void *, unsigned char *, size_t),
                              void *p_rng )
{
    int ret;
    size_t len;

    if( end < *p )
        return( MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL );

    /* Generate key (7.4.2.3.1) and write it out */
    MBEDTLS_MPI_CHK( mbedtls_ecp_gen_keypair_base(
        (mbedtls_ecp_group*) &ctx->grp, G, x, X,
        f_rng, p_rng ) );
    MBEDTLS_MPI_CHK( mbedtls_ecp_tls_write_point( &ctx->grp, X,
                ctx->point_format, &len, *p, end - *p ) );
    *p += len;
    /* Generate and write proof */
    MBEDTLS_MPI_CHK( ecjpake_zkp_write( ctx, G, x, X, id,
                                        p, end, f_rng, p_rng ) );

cleanup:
    return( ret );
}

/*
 * Read a ECJPAKEKeyKPPairList (7.4.2.3) and check proofs
 * Ouputs: verified peer public keys Xa, Xb
 */
static int ecjpake_kkpp_read( const mbedtls_ecjpake_context *ctx,
                              const mbedtls_ecp_point *G,
                              mbedtls_ecp_point *Xa,
                              mbedtls_ecp_point *Xb,
                              const char *id,
                              const unsigned char *buf,
                              size_t len )
{
    int ret;
    const unsigned char *p = buf;
    const unsigned char *end = buf + len;

    /*
     * struct {
     *     ECJPAKEKeyKP ecjpake_key_kp_pair_list[2];
     * } ECJPAKEKeyKPPairList;
     */
    MBEDTLS_MPI_CHK( ecjpake_kkp_read( ctx, G, Xa, id, &p, end ) );
    MBEDTLS_MPI_CHK( ecjpake_kkp_read( ctx, G, Xb, id, &p, end ) );

    if( p != end )
        ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;

cleanup:
    return( ret );
}

/*
 * Generate a ECJPAKEKeyKPPairList
 * Outputs: the serialized structure, plus two private/public key pairs
 */
static int ecjpake_kkpp_write( const mbedtls_ecjpake_context *ctx,
                               const mbedtls_ecp_point *G,
                               mbedtls_mpi *xm1,
                               mbedtls_ecp_point *Xa,
                               mbedtls_mpi *xm2,
                               mbedtls_ecp_point *Xb,
                               const char *id,
                               unsigned char *buf,
                               size_t len,
                               size_t *olen,
                               int (*f_rng)(void *, unsigned char *, size_t),
                               void *p_rng )
{
    int ret;
    unsigned char *p = buf;
    const unsigned char *end = buf + len;

    MBEDTLS_MPI_CHK( ecjpake_kkp_write( ctx, G, xm1, Xa, id,
                &p, end, f_rng, p_rng ) );
    MBEDTLS_MPI_CHK( ecjpake_kkp_write( ctx, G, xm2, Xb, id,
                &p, end, f_rng, p_rng ) );

    *olen = p - buf;

cleanup:
    return( ret );
}

/*
 * Read and process the first round message
 */
int mbedtls_ecjpake_read_round_one( mbedtls_ecjpake_context *ctx,
                                    const unsigned char *buf,
                                    size_t len )
{
    return( ecjpake_kkpp_read( ctx,
                               &ctx->grp.G,
                               &ctx->Xp1, &ctx->Xp2, ID_PEER,
                               buf, len ) );
}

/*
 * Generate and write the first round message
 */
int mbedtls_ecjpake_write_round_one( mbedtls_ecjpake_context *ctx,
                            unsigned char *buf, size_t len, size_t *olen,
                            int (*f_rng)(void *, unsigned char *, size_t),
                            void *p_rng )
{
    return( ecjpake_kkpp_write( ctx,
                                &ctx->grp.G,
                                &ctx->xm1, &ctx->Xm1, &ctx->xm2, &ctx->Xm2,
                                ID_MINE, buf, len, olen, f_rng, p_rng ) );
}

/*
 * Read and process second round message (C: 7.4.2.5, S: 7.4.2.6)
 */
int mbedtls_ecjpake_read_round_two( mbedtls_ecjpake_context *ctx,
                                            const unsigned char *buf,
                                            size_t len )
{
    int ret;
    const unsigned char *p = buf;
    const unsigned char *end = buf + len;
    mbedtls_ecp_group grp;
    mbedtls_ecp_point G;    /* C: GB, S: GA */

    mbedtls_ecp_group_init( &grp );
    mbedtls_ecp_point_init( &G );

    /*
     * Server: GA = X3  + X4  + X1      (7.4.2.6.1)
     * Client: GB = X1  + X2  + X3      (7.4.2.5.1)
     * Unified: G = Xm1 + Xm2 + Xp1
     * We need that before parsing in order to check Xp as we read it
     */
    uint32_t opsize = sx_ecc_curve_bytesize(ctx->curve);
    uint8_t X1mem[MBEDTLS_ECJPAKE_POINT_BYTES];
    block_t X1 = BLOCK_T_CONV(X1mem, opsize * 2);
    uint8_t X2mem[MBEDTLS_ECJPAKE_POINT_BYTES];
    block_t X2 = BLOCK_T_CONV(X2mem, opsize * 2);
    uint8_t X3mem[MBEDTLS_ECJPAKE_POINT_BYTES];
    block_t X3 = BLOCK_T_CONV(X3mem, opsize * 2);
    uint8_t Gmem[MBEDTLS_ECJPAKE_POINT_BYTES];
    block_t G_blk = BLOCK_T_CONV(Gmem, opsize * 2);

    MBEDTLS_MPI_CHK( convert_pt_ecp_cryptosoc(&ctx->Xm1, X1, opsize) );
    MBEDTLS_MPI_CHK( convert_pt_ecp_cryptosoc(&ctx->Xm2, X2, opsize) );
    MBEDTLS_MPI_CHK( convert_pt_ecp_cryptosoc(&ctx->Xp1, X3, opsize) );

    sx_enable_clock();

    ret = jpake_3point_add(ctx->curve->params, X2, X3, X1, G_blk,
                           ctx->curve->command, opsize);
    sx_disable_clock();
    if (ret) {
        ret = MBEDTLS_ERR_ECP_HW_ACCEL_FAILED;
        goto cleanup;
    }
    MBEDTLS_MPI_CHK( convert_pt_cryptosoc_ecp(&G, G_blk, opsize) );

    /*
     * struct {
     *     ECParameters curve_params;   // only client reading server msg
     *     ECJPAKEKeyKP ecjpake_key_kp;
     * } Client/ServerECJPAKEParams;
     */
    if( ctx->role == MBEDTLS_ECJPAKE_CLIENT )
    {
        MBEDTLS_MPI_CHK( mbedtls_ecp_tls_read_group( &grp, &p, len ) );
        if( grp.id != ctx->grp.id )
        {
            ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
            goto cleanup;
        }
    }

    MBEDTLS_MPI_CHK( ecjpake_kkp_read( ctx,
                            &G, &ctx->Xp, ID_PEER, &p, end ) );

    if( p != end )
    {
        ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
        goto cleanup;
    }

cleanup:
    mbedtls_ecp_group_free( &grp );
    mbedtls_ecp_point_free( &G );

    return( ret );
}


/*
 * Generate and write the second round message (S: 7.4.2.5, C: 7.4.2.6)
 */
int mbedtls_ecjpake_write_round_two( mbedtls_ecjpake_context *ctx,
                            unsigned char *buf, size_t len, size_t *olen,
                            int (*f_rng)(void *, unsigned char *, size_t),
                            void *p_rng )
{
    int ret;
    mbedtls_ecp_point G;    /* C: GA, S: GB */
    mbedtls_ecp_point Xm;   /* C: Xc, S: Xs */
    mbedtls_mpi xm;         /* C: xc, S: xs */
    unsigned char *p = buf;
    const unsigned char *end = buf + len;
    size_t ec_len;

    mbedtls_ecp_point_init( &G );
    mbedtls_ecp_point_init( &Xm );
    mbedtls_mpi_init( &xm );

    /*
     * First generate private/public key pair (S: 7.4.2.5.1, C: 7.4.2.6.1)
     *
     * Client:  GA = X1  + X3  + X4  | xs = x2  * s | Xc = xc * GA
     * Server:  GB = X3  + X1  + X2  | xs = x4  * s | Xs = xs * GB
     * Unified: G  = Xm1 + Xp1 + Xp2 | xm = xm2 * s | Xm = xm * G
     */
    uint32_t opsize = sx_ecc_curve_bytesize(ctx->curve);

    uint8_t X1mem[MBEDTLS_ECJPAKE_POINT_BYTES];
    block_t X1 = BLOCK_T_CONV(X1mem, opsize * 2);
    uint8_t X3mem[MBEDTLS_ECJPAKE_POINT_BYTES];
    block_t X3 = BLOCK_T_CONV(X3mem, opsize * 2);
    uint8_t X4mem[MBEDTLS_ECJPAKE_POINT_BYTES];
    block_t X4 = BLOCK_T_CONV(X4mem, opsize * 2);
    uint8_t pwdmem[MBEDTLS_ECJPAKE_MAX_BYTES];
    block_t pwd_x2s_blk = BLOCK_T_CONV(pwdmem, opsize);
    uint8_t x2mem[MBEDTLS_ECJPAKE_MAX_BYTES];
    block_t x2_blk = BLOCK_T_CONV(x2mem, opsize);
    uint8_t Gmem[MBEDTLS_ECJPAKE_POINT_BYTES];
    block_t G_blk = BLOCK_T_CONV(Gmem, opsize * 2);
    uint8_t Amem[MBEDTLS_ECJPAKE_POINT_BYTES];
    block_t A_blk = BLOCK_T_CONV(Amem, opsize * 2);

    MBEDTLS_MPI_CHK( convert_pt_ecp_cryptosoc(&ctx->Xm1, X1, opsize) );
    MBEDTLS_MPI_CHK( convert_pt_ecp_cryptosoc(&ctx->Xp1, X3, opsize) );
    MBEDTLS_MPI_CHK( convert_pt_ecp_cryptosoc(&ctx->Xp2, X4, opsize) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary(&ctx->s, pwd_x2s_blk.addr, opsize) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary(&ctx->xm2, x2_blk.addr, opsize) );
    sx_enable_clock();

    ret = jpake_round2_compute_points(ctx->curve->params, x2_blk, X1, X3, X4,
        pwd_x2s_blk, G_blk, A_blk, ctx->curve->command, opsize);
    sx_disable_clock();

    if (ret) {
        ret = MBEDTLS_ERR_ECP_HW_ACCEL_FAILED;
        goto cleanup;
    }
    MBEDTLS_MPI_CHK( convert_pt_cryptosoc_ecp(&G, G_blk, opsize) );
    MBEDTLS_MPI_CHK( convert_pt_cryptosoc_ecp(&Xm, A_blk, opsize) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary(&xm, pwd_x2s_blk.addr, opsize) );

    /*
     * Now write things out
     *
     * struct {
     *     ECParameters curve_params;   // only server writing its message
     *     ECJPAKEKeyKP ecjpake_key_kp;
     * } Client/ServerECJPAKEParams;
     */
    if( ctx->role == MBEDTLS_ECJPAKE_SERVER )
    {
        if( end < p )
        {
            ret = MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL;
            goto cleanup;
        }
        MBEDTLS_MPI_CHK( mbedtls_ecp_tls_write_group( &ctx->grp, &ec_len,
                                                      p, end - p ) );
        p += ec_len;
    }

    if( end < p )
    {
        ret = MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL;
        goto cleanup;
    }
    MBEDTLS_MPI_CHK( mbedtls_ecp_tls_write_point( &ctx->grp, &Xm,
                         ctx->point_format, &ec_len, p, end - p ) );
    p += ec_len;

    MBEDTLS_MPI_CHK( ecjpake_zkp_write( ctx,
                                        &G, &xm, &Xm, ID_MINE,
                                        &p, end, f_rng, p_rng ) );

    *olen = p - buf;

cleanup:
    mbedtls_ecp_point_free( &G );
    mbedtls_ecp_point_free( &Xm );
    mbedtls_mpi_free( &xm );

    return( ret );
}

/*
 * Derive PMS (7.4.2.7 / 7.4.2.8)
 */
int mbedtls_ecjpake_derive_secret( mbedtls_ecjpake_context *ctx,
                            unsigned char *buf, size_t len, size_t *olen,
                            int (*f_rng)(void *, unsigned char *, size_t),
                            void *p_rng )
{
    uint32_t ret;
    uint32_t opsize = sx_ecc_curve_bytesize(ctx->curve);
    block_t session_key = BLOCK_T_CONV(buf, len);
    uint8_t x2buf[MBEDTLS_ECJPAKE_MAX_BYTES];
    block_t x2_blk = BLOCK_T_CONV(x2buf, opsize);
    uint8_t X4buf[MBEDTLS_ECJPAKE_POINT_BYTES];
    block_t X4_blk = BLOCK_T_CONV(X4buf, opsize * 2);
    uint8_t Bbuf[MBEDTLS_ECJPAKE_POINT_BYTES];
    block_t B_blk = BLOCK_T_CONV(Bbuf, opsize * 2);
    uint8_t pwdbuf[MBEDTLS_ECJPAKE_MAX_BYTES];
    block_t pwd_blk = BLOCK_T_CONV(pwdbuf, opsize);

    *olen = (size_t)sx_hash_get_digest_size( (sx_hash_fct_t)ctx->hashalg );
    if( len < *olen )
        return( MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL );

    MBEDTLS_MPI_CHK( convert_pt_ecp_cryptosoc(&ctx->Xp2, X4_blk, opsize) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary(&ctx->xm2, x2buf, opsize) );
    MBEDTLS_MPI_CHK( convert_pt_ecp_cryptosoc(&ctx->Xp, B_blk, opsize) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary(&ctx->s, pwdbuf, opsize) );

    sx_enable_clock();
    ret = jpake_generate_session_key_blk(ctx->curve->params,
        pwd_blk, x2_blk, X4_blk, B_blk, (sx_hash_fct_t)ctx->hashalg,
        session_key, ctx->curve->command, opsize);
    sx_disable_clock();

    if (ret)
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;

cleanup:
    return( ret );
}

#undef ID_MINE
#undef ID_PEER

#endif /* MBEDTLS_ECJPAKE_ALT */

#endif /* MBEDTLS_ECJPAKE_C */
