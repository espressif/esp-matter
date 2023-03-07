/*
 *  Elliptic curve DSA
 *
 *  Copyright The Mbed TLS Contributors
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
 */

/*
 * References:
 *
 * SEC1 http://www.secg.org/index.php?action=secg,docs_secg
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_ECDSA_C)

#include <nxLog_sss.h>
#include "fsl_sss_util_asn1_der.h"

#include "mbedtls/ecdsa.h"
#include "mbedtls/asn1write.h"

#include <string.h>

#if defined(MBEDTLS_ECDSA_DETERMINISTIC)
#include "mbedtls/hmac_drbg.h"
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdlib.h>
#define mbedtls_calloc calloc
#define mbedtls_free free
#endif

#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"

/* clang-format off */

/* Parameter validation macros based on platform_util.h */
#define ECDSA_VALIDATE_RET(cond) MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_ECP_BAD_INPUT_DATA)
#define ECDSA_VALIDATE(cond) MBEDTLS_INTERNAL_VALIDATE(cond)

#if defined(MBEDTLS_ECDSA_VERIFY_ALT)

/* Used for SSS object init */
static sss_key_store_t *ecdsa_verify_ssskeystore = NULL;

#if defined(MBEDTLS_ECP_RESTARTABLE)

/*
 * Sub-context for ecdsa_verify()
 */
struct mbedtls_ecdsa_restart_ver
{
    mbedtls_mpi u1, u2;     /* intermediate values  */
    enum {                  /* what to do next?     */
        ecdsa_ver_init = 0, /* getting started      */
        ecdsa_ver_muladd,   /* muladd step          */
    } state;
};

/*
 * Init verify restart sub-context
 */
static void ecdsa_restart_ver_init( mbedtls_ecdsa_restart_ver_ctx *ctx )
{
    mbedtls_mpi_init( &ctx->u1 );
    mbedtls_mpi_init( &ctx->u2 );
    ctx->state = ecdsa_ver_init;
}

/*
 * Free the components of a verify restart sub-context
 */
static void ecdsa_restart_ver_free( mbedtls_ecdsa_restart_ver_ctx *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_mpi_free( &ctx->u1 );
    mbedtls_mpi_free( &ctx->u2 );

    ecdsa_restart_ver_init( ctx );
}

/*
 * Sub-context for ecdsa_sign()
 */
struct mbedtls_ecdsa_restart_sig
{
    int sign_tries;
    int key_tries;
    mbedtls_mpi k;          /* per-signature random */
    mbedtls_mpi r;          /* r value              */
    enum {                  /* what to do next?     */
        ecdsa_sig_init = 0, /* getting started      */
        ecdsa_sig_mul,      /* doing ecp_mul()      */
        ecdsa_sig_modn,     /* mod N computations   */
    } state;
};

/*
 * Init verify sign sub-context
 */
static void ecdsa_restart_sig_init( mbedtls_ecdsa_restart_sig_ctx *ctx )
{
    ctx->sign_tries = 0;
    ctx->key_tries = 0;
    mbedtls_mpi_init( &ctx->k );
    mbedtls_mpi_init( &ctx->r );
    ctx->state = ecdsa_sig_init;
}

/*
 * Free the components of a sign restart sub-context
 */
static void ecdsa_restart_sig_free( mbedtls_ecdsa_restart_sig_ctx *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_mpi_free( &ctx->k );
    mbedtls_mpi_free( &ctx->r );
}

#if defined(MBEDTLS_ECDSA_DETERMINISTIC)
/*
 * Sub-context for ecdsa_sign_det()
 */
struct mbedtls_ecdsa_restart_det
{
    mbedtls_hmac_drbg_context rng_ctx;  /* DRBG state   */
    enum {                      /* what to do next?     */
        ecdsa_det_init = 0,     /* getting started      */
        ecdsa_det_sign,         /* make signature       */
    } state;
};

/*
 * Init verify sign_det sub-context
 */
static void ecdsa_restart_det_init( mbedtls_ecdsa_restart_det_ctx *ctx )
{
    mbedtls_hmac_drbg_init( &ctx->rng_ctx );
    ctx->state = ecdsa_det_init;
}

/*
 * Free the components of a sign_det restart sub-context
 */
static void ecdsa_restart_det_free( mbedtls_ecdsa_restart_det_ctx *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_hmac_drbg_free( &ctx->rng_ctx );

    ecdsa_restart_det_init( ctx );
}
#endif /* MBEDTLS_ECDSA_DETERMINISTIC */

#define ECDSA_RS_ECP    ( rs_ctx == NULL ? NULL : &rs_ctx->ecp )

/* Utility macro for checking and updating ops budget */
#define ECDSA_BUDGET( ops )   \
    MBEDTLS_MPI_CHK( mbedtls_ecp_check_budget( grp, ECDSA_RS_ECP, ops ) );

/* Call this when entering a function that needs its own sub-context */
#define ECDSA_RS_ENTER( SUB )   do {                                 \
    /* reset ops count for this call if top-level */                 \
    if( rs_ctx != NULL && rs_ctx->ecp.depth++ == 0 )                 \
        rs_ctx->ecp.ops_done = 0;                                    \
                                                                     \
    /* set up our own sub-context if needed */                       \
    if( mbedtls_ecp_restart_is_enabled() &&                          \
        rs_ctx != NULL && rs_ctx->SUB == NULL )                      \
    {                                                                \
        rs_ctx->SUB = mbedtls_calloc( 1, sizeof( *rs_ctx->SUB ) );   \
        if( rs_ctx->SUB == NULL )                                    \
            return( MBEDTLS_ERR_ECP_ALLOC_FAILED );                  \
                                                                     \
        ecdsa_restart_## SUB ##_init( rs_ctx->SUB );                 \
    }                                                                \
} while( 0 )

/* Call this when leaving a function that needs its own sub-context */
#define ECDSA_RS_LEAVE( SUB )   do {                                 \
    /* clear our sub-context when not in progress (done or error) */ \
    if( rs_ctx != NULL && rs_ctx->SUB != NULL &&                     \
        ret != MBEDTLS_ERR_ECP_IN_PROGRESS )                         \
    {                                                                \
        ecdsa_restart_## SUB ##_free( rs_ctx->SUB );                 \
        mbedtls_free( rs_ctx->SUB );                                 \
        rs_ctx->SUB = NULL;                                          \
    }                                                                \
                                                                     \
    if( rs_ctx != NULL )                                             \
        rs_ctx->ecp.depth--;                                         \
} while( 0 )

#else /* MBEDTLS_ECP_RESTARTABLE */

#define ECDSA_RS_ECP    NULL

#define ECDSA_BUDGET( ops )   /* no-op; for compatibility */

#define ECDSA_RS_ENTER( SUB )   (void) rs_ctx
#define ECDSA_RS_LEAVE( SUB )   (void) rs_ctx

#endif /* MBEDTLS_ECP_RESTARTABLE */

/*
 * Derive a suitable integer for group grp from a buffer of length len
 * SEC1 4.1.3 step 5 aka SEC1 4.1.4 step 3
 */
static int derive_mpi( const mbedtls_ecp_group *grp, mbedtls_mpi *x,
                       const unsigned char *buf, size_t blen )
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    size_t n_size = ( grp->nbits + 7 ) / 8;
    size_t use_size = blen > n_size ? n_size : blen;

    MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( x, buf, use_size ) );
    if( use_size * 8 > grp->nbits )
        MBEDTLS_MPI_CHK( mbedtls_mpi_shift_r( x, use_size * 8 - grp->nbits ) );

    /* While at it, reduce modulo N */
    if( mbedtls_mpi_cmp_mpi( x, &grp->N ) >= 0 )
        MBEDTLS_MPI_CHK( mbedtls_mpi_sub_mpi( x, x, &grp->N ) );

cleanup:
    return (ret);
}

/*
* For software rollback
*/
/*
* Verify ECDSA signature of hashed message (SEC1 4.1.4)
* Obviously, compared to SEC1 4.1.3, we skip step 2 (hash message)
*/
static int ecdsa_verify_restartable_o(mbedtls_ecp_group *grp,
                                     const unsigned char *buf, size_t blen,
                                     const mbedtls_ecp_point *Q,
                                     const mbedtls_mpi *r, const mbedtls_mpi *s,
                                     mbedtls_ecdsa_restart_ctx *rs_ctx )
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    mbedtls_mpi e, s_inv, u1, u2;
    mbedtls_ecp_point R;
    mbedtls_mpi *pu1 = &u1, *pu2 = &u2;

    mbedtls_ecp_point_init( &R );
    mbedtls_mpi_init( &e ); mbedtls_mpi_init( &s_inv );
    mbedtls_mpi_init( &u1 ); mbedtls_mpi_init( &u2 );

    /* Fail cleanly on curves such as Curve25519 that can't be used for ECDSA */
    if( ! mbedtls_ecdsa_can_do( grp->id ) || grp->N.p == NULL )
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );

    ECDSA_RS_ENTER( ver );

#if defined(MBEDTLS_ECP_RESTARTABLE)
    if( rs_ctx != NULL && rs_ctx->ver != NULL )
    {
        /* redirect to our context */
        pu1 = &rs_ctx->ver->u1;
        pu2 = &rs_ctx->ver->u2;

        /* jump to current step */
        if( rs_ctx->ver->state == ecdsa_ver_muladd )
            goto muladd;
    }
#endif /* MBEDTLS_ECP_RESTARTABLE */

    /*
     * Step 1: make sure r and s are in range 1..n-1
     */
    if( mbedtls_mpi_cmp_int( r, 1 ) < 0 || mbedtls_mpi_cmp_mpi( r, &grp->N ) >= 0 ||
        mbedtls_mpi_cmp_int( s, 1 ) < 0 || mbedtls_mpi_cmp_mpi( s, &grp->N ) >= 0 )
    {
        ret = MBEDTLS_ERR_ECP_VERIFY_FAILED;
        goto cleanup;
    }

    /*
     * Step 3: derive MPI from hashed message
     */
    MBEDTLS_MPI_CHK( derive_mpi( grp, &e, buf, blen ) );

    /*
     * Step 4: u1 = e / s mod n, u2 = r / s mod n
     */
    ECDSA_BUDGET( MBEDTLS_ECP_OPS_CHK + MBEDTLS_ECP_OPS_INV + 2 );

    MBEDTLS_MPI_CHK( mbedtls_mpi_inv_mod( &s_inv, s, &grp->N ) );

    MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( pu1, &e, &s_inv ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_mod_mpi( pu1, pu1, &grp->N ) );

    MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( pu2, r, &s_inv ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_mod_mpi( pu2, pu2, &grp->N ) );

#if defined(MBEDTLS_ECP_RESTARTABLE)
    if( rs_ctx != NULL && rs_ctx->ver != NULL )
        rs_ctx->ver->state = ecdsa_ver_muladd;

muladd:
#endif
    /*
     * Step 5: R = u1 G + u2 Q
     */
    MBEDTLS_MPI_CHK( mbedtls_ecp_muladd_restartable( grp,
                     &R, pu1, &grp->G, pu2, Q, ECDSA_RS_ECP ) );

    if( mbedtls_ecp_is_zero( &R ) )
    {
        ret = MBEDTLS_ERR_ECP_VERIFY_FAILED;
        goto cleanup;
    }

    /*
     * Step 6: convert xR to an integer (no-op)
     * Step 7: reduce xR mod n (gives v)
     */
    MBEDTLS_MPI_CHK( mbedtls_mpi_mod_mpi( &R.X, &R.X, &grp->N ) );

    /*
     * Step 8: check if v (that is, R.X) is equal to r
     */
    if( mbedtls_mpi_cmp_mpi( &R.X, r ) != 0 )
    {
        ret = MBEDTLS_ERR_ECP_VERIFY_FAILED;
        goto cleanup;
    }

cleanup:
    mbedtls_ecp_point_free( &R );
    mbedtls_mpi_free( &e ); mbedtls_mpi_free( &s_inv );
    mbedtls_mpi_free( &u1 ); mbedtls_mpi_free( &u2 );

    ECDSA_RS_LEAVE( ver );

    return( ret );
}

/*
* Verify ECDSA signature of hashed message
*/
int mbedtls_ecdsa_verify_o(mbedtls_ecp_group *grp,
                          const unsigned char *buf, size_t blen,
                          const mbedtls_ecp_point *Q,
                          const mbedtls_mpi *r,
                          const mbedtls_mpi *s)
{
    ECDSA_VALIDATE_RET( grp != NULL );
    ECDSA_VALIDATE_RET( Q   != NULL );
    ECDSA_VALIDATE_RET( r   != NULL );
    ECDSA_VALIDATE_RET( s   != NULL );
    ECDSA_VALIDATE_RET( buf != NULL || blen == 0 );

    return (ecdsa_verify_restartable_o(grp, buf, blen, Q, r, s, NULL));
}

void sss_mbedtls_set_sss_keystore(sss_key_store_t *ssskeystore)
{
    ecdsa_verify_ssskeystore = ssskeystore;
}

/*
 * Verify ECDSA signature of hashed message
 */
int mbedtls_ecdsa_verify(mbedtls_ecp_group *grp,
    const unsigned char *buf,
    size_t blen,
    const mbedtls_ecp_point *Q,
    const mbedtls_mpi *r,
    const mbedtls_mpi *s)
{
    int ret = 1;
    ECDSA_VALIDATE_RET(grp != NULL);
    ECDSA_VALIDATE_RET(Q != NULL);
    ECDSA_VALIDATE_RET(r != NULL);
    ECDSA_VALIDATE_RET(s != NULL);
    ECDSA_VALIDATE_RET(buf != NULL || blen == 0);

    if (ecdsa_verify_ssskeystore != NULL) {
        sss_cipher_type_t cipherType = kSSS_CipherType_NONE;
        sss_object_t sssKeyObject    = {
            0,
        };
        sss_status_t status;
        size_t keyBitLen       = 0;
        uint8_t publickey[170] = {
            0,
        };
        size_t publickeylen          = 0;
        size_t rawPublickeylen       = 0;
        unsigned char signature[150] = {
            0,
        };
        size_t sigLen            = 0;
        unsigned char rs_buf[80] = {
            0,
        };
        size_t rs_buf_len = 0;
        sss_algorithm_t algorithm;
        sss_asymmetric_t asymVerifyCtx;

        /*
        *  Create the signature
        *  Signature = {
        *  0x30, Remaining Length, Tag, R_length, R, Tag, S_length, S }
        */

        /* Set totoal length */
        signature[sigLen++] = 0x30;
        signature[sigLen++] = (unsigned char)(4 + mbedtls_mpi_size(r) + mbedtls_mpi_size(s));
        /* 4 ==> Tag + Lengthn + Tag + Length */

        /* Set R */
        rs_buf_len = mbedtls_mpi_size(r);
        ret        = mbedtls_mpi_write_binary(r, rs_buf, rs_buf_len);
        if (ret != 0) {
            return ret;
        }

        signature[sigLen++] = 0x02;
        if ((rs_buf[0] & 0x80)) {
            signature[sigLen++] = (unsigned char)(rs_buf_len + 1);
            signature[sigLen++] = 0x00;
            /* Increment total length */
            signature[1] += 1;
        }
        else {
            signature[sigLen++] = (unsigned char)rs_buf_len;
        }

        if ((sizeof(signature) - sigLen) < rs_buf_len) {
            return -1;
        }
        memcpy(&signature[sigLen], rs_buf, rs_buf_len);
        sigLen += rs_buf_len;

        /* Set S */
        rs_buf_len = mbedtls_mpi_size(s);
        ret        = mbedtls_mpi_write_binary(s, rs_buf, rs_buf_len);
        if (ret != 0) {
            return ret;
        }

        signature[sigLen++] = 0x02;
        if ((rs_buf[0] & 0x80)) {
            signature[sigLen++] = (unsigned char)(rs_buf_len + 1);
            signature[sigLen++] = 0x00;
            /* Increment total length */
            signature[1] += 1;
        }
        else {
            signature[sigLen++] = (unsigned char)rs_buf_len;
        }

        if ((sizeof(signature) - sigLen) < rs_buf_len) {
            return -1;
        }
        memcpy(&signature[sigLen], rs_buf, rs_buf_len);
        sigLen += rs_buf_len;

        switch (grp->id) {
        case MBEDTLS_ECP_DP_SECP192R1:
            memcpy(publickey, gecc_der_header_nist192, der_ecc_nistp192_header_len);
            publickeylen = der_ecc_nistp192_header_len;
            cipherType   = kSSS_CipherType_EC_NIST_P;
            keyBitLen    = 192;
            break;
        case MBEDTLS_ECP_DP_SECP224R1:
            memcpy(publickey, gecc_der_header_nist224, der_ecc_nistp224_header_len);
            publickeylen = der_ecc_nistp224_header_len;
            cipherType   = kSSS_CipherType_EC_NIST_P;
            keyBitLen    = 224;
            break;
        case MBEDTLS_ECP_DP_SECP256R1:
            memcpy(publickey, gecc_der_header_nist256, der_ecc_nistp256_header_len);
            publickeylen = der_ecc_nistp256_header_len;
            cipherType   = kSSS_CipherType_EC_NIST_P;
            keyBitLen    = 256;
            break;
        case MBEDTLS_ECP_DP_SECP384R1:
            memcpy(publickey, gecc_der_header_nist384, der_ecc_nistp384_header_len);
            publickeylen = der_ecc_nistp384_header_len;
            cipherType   = kSSS_CipherType_EC_NIST_P;
            keyBitLen    = 384;
            break;
        case MBEDTLS_ECP_DP_SECP521R1:
            memcpy(publickey, gecc_der_header_nist521, der_ecc_nistp521_header_len);
            publickeylen = der_ecc_nistp521_header_len;
            cipherType   = kSSS_CipherType_EC_NIST_P;
            keyBitLen    = 521;
            break;
        case MBEDTLS_ECP_DP_BP256R1:
            memcpy(publickey, gecc_der_header_bp256, der_ecc_bp256_header_len);
            publickeylen = der_ecc_bp256_header_len;
            cipherType   = kSSS_CipherType_EC_BRAINPOOL;
            keyBitLen    = 256;
            break;
        case MBEDTLS_ECP_DP_BP384R1:
            memcpy(publickey, gecc_der_header_bp384, der_ecc_bp384_header_len);
            publickeylen = der_ecc_bp384_header_len;
            cipherType   = kSSS_CipherType_EC_BRAINPOOL;
            keyBitLen    = 384;
            break;
        case MBEDTLS_ECP_DP_BP512R1:
            memcpy(publickey, gecc_der_header_bp512, der_ecc_bp512_header_len);
            publickeylen = der_ecc_bp512_header_len;
            cipherType   = kSSS_CipherType_EC_BRAINPOOL;
            keyBitLen    = 512;
            break;
        case MBEDTLS_ECP_DP_SECP192K1:
            memcpy(publickey, gecc_der_header_192k, der_ecc_192k_header_len);
            publickeylen = der_ecc_192k_header_len;
            cipherType   = kSSS_CipherType_EC_NIST_K;
            keyBitLen    = 192;
            break;
        case MBEDTLS_ECP_DP_SECP224K1:
            memcpy(publickey, gecc_der_header_224k, der_ecc_224k_header_len);
            publickeylen = der_ecc_224k_header_len;
            cipherType   = kSSS_CipherType_EC_NIST_K;
            keyBitLen    = 224;
            break;
        case MBEDTLS_ECP_DP_SECP256K1:
            memcpy(publickey, gecc_der_header_256k, der_ecc_256k_header_len);
            publickeylen = der_ecc_256k_header_len;
            cipherType   = kSSS_CipherType_EC_NIST_K;
            keyBitLen    = 256;
            break;
        default:
            /* Rollback to verification on host if SE is not initialised */
            return mbedtls_ecdsa_verify_o(grp, buf, blen, Q, r, s);
        }

        ret = mbedtls_ecp_point_write_binary(
            grp, Q, 0, &rawPublickeylen, &publickey[publickeylen], (sizeof(publickey) - publickeylen));
        if (ret != 0) {
            return ret;
        }
        publickeylen += rawPublickeylen;

        status = sss_key_object_init(&sssKeyObject, ecdsa_verify_ssskeystore);
        if (status != kStatus_SSS_Success) {
            return 1;
        }

        status = sss_key_object_allocate_handle(
            &sssKeyObject, (__LINE__), kSSS_KeyPart_Public, cipherType, publickeylen, kKeyObject_Mode_Transient);
        if (status != kStatus_SSS_Success) {
            return 1;
        }

        status =
            sss_key_store_set_key(ecdsa_verify_ssskeystore, &sssKeyObject, publickey, publickeylen, keyBitLen, NULL, 0);
        if (status != kStatus_SSS_Success) {
            return 1;
        }

        switch (blen) {
        case 20:
            algorithm = kAlgorithm_SSS_SHA1;
            break;
        case 28:
            algorithm = kAlgorithm_SSS_SHA224;
            break;
        case 32:
            algorithm = kAlgorithm_SSS_SHA256;
            break;
        case 48:
            algorithm = kAlgorithm_SSS_SHA384;
            break;
        case 64:
            algorithm = kAlgorithm_SSS_SHA512;
            break;
        default:
            return 1;
        }

        status = sss_asymmetric_context_init(
            &asymVerifyCtx, ecdsa_verify_ssskeystore->session, &sssKeyObject, algorithm, kMode_SSS_Verify);
        if (status != kStatus_SSS_Success) {
            return 1;
        }

        LOG_D("Verify using sss_asymmetric_verify_digest \n");
        status = sss_asymmetric_verify_digest(&asymVerifyCtx, (uint8_t *)buf, blen, (uint8_t *)signature, sigLen);
        if (status != kStatus_SSS_Success) {
            return 1;
        }

        status = sss_key_store_erase_key(ecdsa_verify_ssskeystore, &sssKeyObject);
        if (status != kStatus_SSS_Success) {
            return 1;
        }

        return 0;
    }
    else {
        /* Rollback to verification on host if SE is not initialised */
        return mbedtls_ecdsa_verify_o(grp, buf, blen, Q, r, s);
    }
}
#endif /* !MBEDTLS_ECDSA_VERIFY_ALT */

/* clang-format on */

#endif /* MBEDTLS_ECDSA_C */
