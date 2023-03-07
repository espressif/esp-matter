/*
 *  Elliptic curve Diffie-Hellman
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
 * Copyright 2019-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * References:
 *
 * SEC1 http://www.secg.org/index.php?action=secg,docs_secg
 * RFC 4492
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdlib.h>
#define mbedtls_calloc calloc
#define mbedtls_free free
#endif /* MBEDTLS_PLATFORM_C */

#if defined(MBEDTLS_ECDH_C)
#if defined(MBEDTLS_ECDH_ALT)
#include "mbedtls/ecdh.h"
#include "mbedtls/platform_util.h"

#include <string.h>

#include "fsl_debug_console.h"

/* Parameter validation macros based on platform_util.h */
#define ECDH_VALIDATE_RET(cond) MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_ECP_BAD_INPUT_DATA)
#define ECDH_VALIDATE(cond) MBEDTLS_INTERNAL_VALIDATE(cond)

#if defined(MBEDTLS_ECDH_LEGACY_CONTEXT)
typedef mbedtls_ecdh_context mbedtls_ecdh_context_mbed;
#endif

static mbedtls_ecp_group_id mbedtls_ecdh_grp_id(const mbedtls_ecdh_context *ctx)
{
#if defined(MBEDTLS_ECDH_LEGACY_CONTEXT)
    return (ctx->grp.id);
#else
    return (ctx->grp_id);
#endif
}

#if !defined(MBEDTLS_ECDH_GEN_PUBLIC_ALT)
/*
 * Generate public key (restartable version)
 *
 * Note: this internal function relies on its caller preserving the value of
 * the output parameter 'd' across continuation calls. This would not be
 * acceptable for a public function but is OK here as we control call sites.
 */
static int ecdh_gen_public_restartable(mbedtls_ecp_group *grp,
                                       mbedtls_mpi *d,
                                       mbedtls_ecp_point *Q,
                                       int (*f_rng)(void *, unsigned char *, size_t),
                                       void *p_rng,
                                       mbedtls_ecp_restart_ctx *rs_ctx)
{
    int ret;
    MBEDTLS_MPI_CHK(mbedtls_ecp_gen_privkey(grp, d, f_rng, p_rng));
    MBEDTLS_MPI_CHK(mbedtls_ecp_mul_restartable(grp, Q, d, &grp->G, f_rng, p_rng, rs_ctx));

cleanup:
    return (ret);
}

/*
 * Generate public key
 */
int mbedtls_ecdh_gen_public(mbedtls_ecp_group *grp,
                            mbedtls_mpi *d,
                            mbedtls_ecp_point *Q,
                            int (*f_rng)(void *, unsigned char *, size_t),
                            void *p_rng)
{
    ECDH_VALIDATE_RET(grp != NULL);
    ECDH_VALIDATE_RET(d != NULL);
    ECDH_VALIDATE_RET(Q != NULL);
    ECDH_VALIDATE_RET(f_rng != NULL);
    return (ecdh_gen_public_restartable(grp, d, Q, f_rng, p_rng, NULL));
}
#endif /* !MBEDTLS_ECDH_GEN_PUBLIC_ALT */

#if !defined(MBEDTLS_ECDH_COMPUTE_SHARED_ALT)
/*
 * Compute shared secret (SEC1 3.3.1)
 */
static int ecdh_compute_shared_restartable(mbedtls_ecp_group *grp,
                                           mbedtls_mpi *z,
                                           const mbedtls_ecp_point *Q,
                                           const mbedtls_mpi *d,
                                           int (*f_rng)(void *, unsigned char *, size_t),
                                           void *p_rng,
                                           mbedtls_ecp_restart_ctx *rs_ctx)
{
    int ret;
    mbedtls_ecp_point P;

    mbedtls_ecp_point_init(&P);

    MBEDTLS_MPI_CHK(mbedtls_ecp_mul_restartable(grp, &P, d, Q, f_rng, p_rng, rs_ctx));

    if (mbedtls_ecp_is_zero(&P))
    {
        ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
        goto cleanup;
    }

    MBEDTLS_MPI_CHK(mbedtls_mpi_copy(z, &P.X));

cleanup:
    mbedtls_ecp_point_free(&P);

    return (ret);
}

/*
 * Compute shared secret (SEC1 3.3.1)
 */
int mbedtls_ecdh_compute_shared(mbedtls_ecp_group *grp,
                                mbedtls_mpi *z,
                                const mbedtls_ecp_point *Q,
                                const mbedtls_mpi *d,
                                int (*f_rng)(void *, unsigned char *, size_t),
                                void *p_rng)
{
    ECDH_VALIDATE_RET(grp != NULL);
    ECDH_VALIDATE_RET(Q != NULL);
    ECDH_VALIDATE_RET(d != NULL);
    ECDH_VALIDATE_RET(z != NULL);
    return (ecdh_compute_shared_restartable(grp, z, Q, d, f_rng, p_rng, NULL));
}
#endif /* !MBEDTLS_ECDH_COMPUTE_SHARED_ALT */

static void ecdh_init_internal(mbedtls_ecdh_context_mbed *ctx)
{
    mbedtls_ecp_group_init(&ctx->grp);
    mbedtls_mpi_init(&ctx->d);
    mbedtls_ecp_point_init(&ctx->Q);
    mbedtls_ecp_point_init(&ctx->Qp);
    mbedtls_mpi_init(&ctx->z);
}

/*
 * Initialize context
 */
void mbedtls_ecdh_init(mbedtls_ecdh_context *ctx)
{
    ECDH_VALIDATE(ctx != NULL);

#if defined(MBEDTLS_ECDH_LEGACY_CONTEXT)
    ecdh_init_internal(ctx);
    mbedtls_ecp_point_init(&ctx->Vi);
    mbedtls_ecp_point_init(&ctx->Vf);
    mbedtls_mpi_init(&ctx->_d);
#else
    memset(ctx, 0, sizeof(mbedtls_ecdh_context));

    ctx->var = MBEDTLS_ECDH_VARIANT_NONE;
#endif
    ctx->point_format     = MBEDTLS_ECP_PF_UNCOMPRESSED;
    ctx->isKeyInitialized = false;
}

static int ecdh_setup_internal(mbedtls_ecdh_context_mbed *ctx, mbedtls_ecp_group_id grp_id)
{
    int ret;

    ret = mbedtls_ecp_group_load(&ctx->grp, grp_id);
    if (ret != 0)
    {
        return (MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE);
    }

    return (0);
}

/*
 * Setup context
 */
int mbedtls_ecdh_setup(mbedtls_ecdh_context *ctx, mbedtls_ecp_group_id grp_id)
{
    ECDH_VALIDATE_RET(ctx != NULL);

#if defined(MBEDTLS_ECDH_LEGACY_CONTEXT)
    return (ecdh_setup_internal(ctx, grp_id));
#else
    switch (grp_id)
    {
        default:
            ctx->point_format = MBEDTLS_ECP_PF_UNCOMPRESSED;
            ctx->var          = MBEDTLS_ECDH_VARIANT_MBEDTLS_2_0;
            ctx->grp_id       = grp_id;
            ecdh_init_internal(&ctx->ctx.mbed_ecdh);
            return (ecdh_setup_internal(&ctx->ctx.mbed_ecdh, grp_id));
    }
#endif
}

static void ecdh_free_internal(mbedtls_ecdh_context_mbed *ctx)
{
    mbedtls_ecp_group_free(&ctx->grp);
    mbedtls_mpi_free(&ctx->d);
    mbedtls_ecp_point_free(&ctx->Q);
    mbedtls_ecp_point_free(&ctx->Qp);
    mbedtls_mpi_free(&ctx->z);
}

/*
 * Free context
 */
void mbedtls_ecdh_free(mbedtls_ecdh_context *ctx)
{
    if (ctx == NULL)
        return;
    if (ctx->isKeyInitialized)
        sss_sscp_key_object_free(&ctx->key);

#if defined(MBEDTLS_ECDH_LEGACY_CONTEXT)
    mbedtls_ecp_point_free(&ctx->Vi);
    mbedtls_ecp_point_free(&ctx->Vf);
    mbedtls_mpi_free(&ctx->_d);
    ecdh_free_internal(ctx);
#else
    switch (ctx->var)
    {
        case MBEDTLS_ECDH_VARIANT_MBEDTLS_2_0:
            ecdh_free_internal(&ctx->ctx.mbed_ecdh);
            break;
        default:
            break;
    }

    ctx->point_format = MBEDTLS_ECP_PF_UNCOMPRESSED;
    ctx->var          = MBEDTLS_ECDH_VARIANT_NONE;
    ctx->grp_id       = MBEDTLS_ECP_DP_NONE;
#endif
}

static int ecdh_make_params_internal(mbedtls_ecdh_context_mbed *ctx,
                                     size_t *olen,
                                     int point_format,
                                     unsigned char *buf,
                                     size_t blen,
                                     int (*f_rng)(void *, unsigned char *, size_t),
                                     void *p_rng,
                                     int restart_enabled)
{
    int ret;
    size_t grp_len, pt_len;

    if (ctx->grp.pbits == 0)
        return (MBEDTLS_ERR_ECP_BAD_INPUT_DATA);

    (void)restart_enabled;

    if ((ret = mbedtls_ecdh_gen_public(&ctx->grp, &ctx->d, &ctx->Q, f_rng, p_rng)) != 0)
        return (ret);

    if ((ret = mbedtls_ecp_tls_write_group(&ctx->grp, &grp_len, buf, blen)) != 0)
        return (ret);

    buf += grp_len;
    blen -= grp_len;

    if ((ret = mbedtls_ecp_tls_write_point(&ctx->grp, &ctx->Q, point_format, &pt_len, buf, blen)) != 0)
        return (ret);

    *olen = grp_len + pt_len;
    return (0);
}

/*
 * Setup and write the ServerKeyExhange parameters (RFC 4492)
 *      struct {
 *          ECParameters    curve_params;
 *          ECPoint         public;
 *      } ServerECDHParams;
 */
int mbedtls_ecdh_make_params(mbedtls_ecdh_context *ctx,
                             size_t *olen,
                             unsigned char *buf,
                             size_t blen,
                             int (*f_rng)(void *, unsigned char *, size_t),
                             void *p_rng)
{
    int restart_enabled = 0;
    ECDH_VALIDATE_RET(ctx != NULL);
    ECDH_VALIDATE_RET(olen != NULL);
    ECDH_VALIDATE_RET(buf != NULL);
    ECDH_VALIDATE_RET(f_rng != NULL);

    (void)restart_enabled;

#if defined(MBEDTLS_ECDH_LEGACY_CONTEXT)
    return (ecdh_make_params_internal(ctx, olen, ctx->point_format, buf, blen, f_rng, p_rng, restart_enabled));
#else
    switch (ctx->var)
    {
        case MBEDTLS_ECDH_VARIANT_MBEDTLS_2_0:
            return (ecdh_make_params_internal(&ctx->ctx.mbed_ecdh, olen, ctx->point_format, buf, blen, f_rng, p_rng,
                                              restart_enabled));
        default:
            return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }
#endif
}

static int ecdh_read_params_internal(mbedtls_ecdh_context_mbed *ctx,
                                     const unsigned char **buf,
                                     const unsigned char *end)
{
    return (mbedtls_ecp_tls_read_point(&ctx->grp, &ctx->Qp, buf, end - *buf));
}

/*
 * Read the ServerKeyExhange parameters (RFC 4492)
 *      struct {
 *          ECParameters    curve_params;
 *          ECPoint         public;
 *      } ServerECDHParams;
 */
int mbedtls_ecdh_read_params(mbedtls_ecdh_context *ctx, const unsigned char **buf, const unsigned char *end)
{
    int ret;
    mbedtls_ecp_group_id grp_id;
    ECDH_VALIDATE_RET(ctx != NULL);
    ECDH_VALIDATE_RET(buf != NULL);
    ECDH_VALIDATE_RET(*buf != NULL);
    ECDH_VALIDATE_RET(end != NULL);

    if ((ret = mbedtls_ecp_tls_read_group_id(&grp_id, buf, end - *buf)) != 0)
        return (ret);

    if ((ret = mbedtls_ecdh_setup(ctx, grp_id)) != 0)
        return (ret);

#if defined(MBEDTLS_ECDH_LEGACY_CONTEXT)
    return (ecdh_read_params_internal(ctx, buf, end));
#else
    switch (ctx->var)
    {
        case MBEDTLS_ECDH_VARIANT_MBEDTLS_2_0:
            return (ecdh_read_params_internal(&ctx->ctx.mbed_ecdh, buf, end));
        default:
            return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }
#endif
}

static int ecdh_get_params_internal(mbedtls_ecdh_context_mbed *ctx,
                                    const mbedtls_ecp_keypair *key,
                                    mbedtls_ecdh_side side)
{
    int ret;

    /* If it's not our key, just import the public part as Qp */
    if (side == MBEDTLS_ECDH_THEIRS)
        return (mbedtls_ecp_copy(&ctx->Qp, &key->Q));

    /* Our key: import public (as Q) and private parts */
    if (side != MBEDTLS_ECDH_OURS)
        return (MBEDTLS_ERR_ECP_BAD_INPUT_DATA);

    if ((ret = mbedtls_ecp_copy(&ctx->Q, &key->Q)) != 0 || (ret = mbedtls_mpi_copy(&ctx->d, &key->d)) != 0)
        return (ret);

    return (0);
}

/*
 * Get parameters from a keypair
 */
int mbedtls_ecdh_get_params(mbedtls_ecdh_context *ctx, const mbedtls_ecp_keypair *key, mbedtls_ecdh_side side)
{
    int ret;
    ECDH_VALIDATE_RET(ctx != NULL);
    ECDH_VALIDATE_RET(key != NULL);
    ECDH_VALIDATE_RET(side == MBEDTLS_ECDH_OURS || side == MBEDTLS_ECDH_THEIRS);

    if (mbedtls_ecdh_grp_id(ctx) == MBEDTLS_ECP_DP_NONE)
    {
        /* This is the first call to get_params(). Set up the context
         * for use with the group. */
        if ((ret = mbedtls_ecdh_setup(ctx, key->grp.id)) != 0)
            return (ret);
    }
    else
    {
        /* This is not the first call to get_params(). Check that the
         * current key's group is the same as the context's, which was set
         * from the first key's group. */
        if (mbedtls_ecdh_grp_id(ctx) != key->grp.id)
            return (MBEDTLS_ERR_ECP_BAD_INPUT_DATA);
    }

#if defined(MBEDTLS_ECDH_LEGACY_CONTEXT)
    return (ecdh_get_params_internal(ctx, key, side));
#else
    switch (ctx->var)
    {
        case MBEDTLS_ECDH_VARIANT_MBEDTLS_2_0:
            return (ecdh_get_params_internal(&ctx->ctx.mbed_ecdh, key, side));
        default:
            return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }
#endif
}

/*
 * Setup and export the client public value
 */
int mbedtls_ecdh_make_public(mbedtls_ecdh_context *ctx,
                             size_t *olen,
                             unsigned char *buf,
                             size_t blen,
                             int (*f_rng)(void *, unsigned char *, size_t),
                             void *p_rng)
{
    int ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    ECDH_VALIDATE_RET(ctx != NULL);
    size_t coordinateLen     = (ctx->grp.pbits + 7u) / 8u;
    size_t coordinateBitsLen = ctx->grp.pbits;
    size_t keySize           = 2 * coordinateLen;
    uint8_t *pubKey          = mbedtls_calloc(keySize, sizeof(uint8_t));
    CRYPTO_InitHardware();
    if (ctx->isKeyInitialized == false)
    {
        if (sss_sscp_key_object_init(&ctx->key, &g_keyStore) != kStatus_SSS_Success)
        {
            sss_sscp_key_object_free(&ctx->key);
            return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
        }
        /* Allovate key handle */
        else if (sss_sscp_key_object_allocate_handle(&ctx->key, 0u, kSSS_KeyPart_Pair, kSSS_CipherType_EC_NIST_P,
                                                     3 * coordinateLen, 0xF0u) != kStatus_SSS_Success)
        {
            sss_sscp_key_object_free(&ctx->key);
            return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
        }
        else
        {
            ctx->isKeyInitialized = true;
        }
    }
    if (sss_sscp_key_store_generate_key(&g_keyStore, &ctx->key, coordinateBitsLen, NULL) != kStatus_SSS_Success)
    {
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
    else if (sss_sscp_key_store_get_key(&g_keyStore, &ctx->key, pubKey, &keySize, &coordinateBitsLen, NULL) !=
             kStatus_SSS_Success)
    {
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
    else if ((ret = mbedtls_mpi_read_binary(&ctx->Q.X, pubKey, coordinateLen)) != 0)
    {
    }
    else if ((ret = mbedtls_mpi_read_binary(&ctx->Q.Y, &pubKey[coordinateLen], coordinateLen)) != 0)
    {
    }
    else if ((ret = mbedtls_mpi_lset(&ctx->Q.Z, 1)) != 0)
    {
    }
    else
    {
        ret = 0;
    }
    mbedtls_platform_zeroize(pubKey, keySize);
    mbedtls_free(pubKey);
    return ret;
}

static int ecdh_read_public_internal(mbedtls_ecdh_context_mbed *ctx, const unsigned char *buf, size_t blen)
{
    int ret;
    const unsigned char *p = buf;

    if ((ret = mbedtls_ecp_tls_read_point(&ctx->grp, &ctx->Qp, &p, blen)) != 0)
        return (ret);

    if ((size_t)(p - buf) != blen)
        return (MBEDTLS_ERR_ECP_BAD_INPUT_DATA);

    return (0);
}

/*
 * Parse and import the client's public value
 */
int mbedtls_ecdh_read_public(mbedtls_ecdh_context *ctx, const unsigned char *buf, size_t blen)
{
    ECDH_VALIDATE_RET(ctx != NULL);
    ECDH_VALIDATE_RET(buf != NULL);

#if defined(MBEDTLS_ECDH_LEGACY_CONTEXT)
    return (ecdh_read_public_internal(ctx, buf, blen));
#else
    switch (ctx->var)
    {
        case MBEDTLS_ECDH_VARIANT_MBEDTLS_2_0:
            return (ecdh_read_public_internal(&ctx->ctx.mbed_ecdh, buf, blen));
        default:
            return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }
#endif
}

/*
 * Derive and export the shared secret
 */
int mbedtls_ecdh_calc_secret(mbedtls_ecdh_context *ctx,
                             size_t *olen,
                             unsigned char *buf,
                             size_t blen,
                             int (*f_rng)(void *, unsigned char *, size_t),
                             void *p_rng)
{
    int ret = 0;
    ECDH_VALIDATE_RET(ctx != NULL);

    sss_sscp_derive_key_t dCtx;
    size_t coordinateLen     = (ctx->grp.pbits + 7u) / 8u;
    size_t coordinateBitsLen = ctx->grp.pbits;
    size_t keySize           = 3 * coordinateLen;
    uint8_t *pubKey          = mbedtls_calloc(keySize, sizeof(uint8_t));
    CRYPTO_InitHardware();
    if (sss_sscp_key_object_init(&ctx->peerPublicKey, &g_keyStore) != kStatus_SSS_Success)
    {
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
    else if (sss_sscp_key_object_allocate_handle(&ctx->peerPublicKey, 1u, kSSS_KeyPart_Pair, kSSS_CipherType_EC_NIST_P,
                                                 keySize, 0xF0u) != kStatus_SSS_Success)
    {
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
    else if ((ret = mbedtls_mpi_write_binary(&ctx->Qp.X, pubKey, coordinateLen)) != 0)
    {
    }
    else if ((ret = mbedtls_mpi_write_binary(&ctx->Qp.Y, &pubKey[coordinateLen], coordinateLen)) != 0)
    {
    }
    else if (sss_sscp_key_store_set_key(&g_keyStore, &ctx->peerPublicKey, (const uint8_t *)pubKey, keySize,
                                        coordinateBitsLen, NULL) != kStatus_SSS_Success)
    {
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
    else if (sss_sscp_key_object_init(&ctx->sharedSecret, &g_keyStore) != kStatus_SSS_Success)
    {
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
    if (sss_sscp_key_object_allocate_handle(&ctx->sharedSecret, 2u, kSSS_KeyPart_Default, kSSS_CipherType_AES,
                                            coordinateLen, 0xFFu) != kStatus_SSS_Success)
    {
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
    if (sss_sscp_derive_key_context_init(&dCtx, &g_sssSession, &ctx->key, kAlgorithm_SSS_ECDH,
                                         kMode_SSS_ComputeSharedSecret) != kStatus_SSS_Success)
    {
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
    else if (sss_sscp_asymmetric_dh_derive_key(&dCtx, &ctx->peerPublicKey, &ctx->sharedSecret) != kStatus_SSS_Success)
    {
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
    else if (sss_sscp_key_store_get_key(&g_keyStore, &ctx->sharedSecret, pubKey, &coordinateLen, &coordinateBitsLen,
                                        NULL) != kStatus_SSS_Success)
    {
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
    else if ((ret = mbedtls_mpi_read_binary(&ctx->z, pubKey, coordinateLen)) != 0)
    {
    }
    else
    {
        ret = 0;
    }
    sss_sscp_derive_key_context_free(&dCtx);
    sss_sscp_key_object_free(&ctx->peerPublicKey);
    sss_sscp_key_object_free(&ctx->sharedSecret);

    mbedtls_platform_zeroize(pubKey, keySize);
    mbedtls_free(pubKey);
    return ret;
}

/* test suite functions*/
#if defined(MBEDTLS_SELF_TEST)
static int ecdh_calc_secret_internal(mbedtls_ecdh_context_mbed *ctx,
                                     size_t *olen,
                                     unsigned char *buf,
                                     size_t blen,
                                     int (*f_rng)(void *, unsigned char *, size_t),
                                     void *p_rng,
                                     int restart_enabled)
{
    int ret;
    if (ctx == NULL || ctx->grp.pbits == 0)
        return (MBEDTLS_ERR_ECP_BAD_INPUT_DATA);
    (void)restart_enabled;
    if ((ret = mbedtls_ecdh_compute_shared(&ctx->grp, &ctx->z, &ctx->Qp, &ctx->d, f_rng, p_rng)) != 0)
    {
        return (ret);
    }

    if (mbedtls_mpi_size(&ctx->z) > blen)
        return (MBEDTLS_ERR_ECP_BAD_INPUT_DATA);

    *olen = ctx->grp.pbits / 8 + ((ctx->grp.pbits % 8) != 0);
    return mbedtls_mpi_write_binary(&ctx->z, buf, *olen);
}

/*
 * Derive and export the shared secret
 */
int mbedtls_ecdh_calc_secret_sw(mbedtls_ecdh_context *ctx,
                                size_t *olen,
                                unsigned char *buf,
                                size_t blen,
                                int (*f_rng)(void *, unsigned char *, size_t),
                                void *p_rng)
{
    int restart_enabled = 0;
    ECDH_VALIDATE_RET(ctx != NULL);
    ECDH_VALIDATE_RET(olen != NULL);
    ECDH_VALIDATE_RET(buf != NULL);

#if defined(MBEDTLS_ECDH_LEGACY_CONTEXT)
    return (ecdh_calc_secret_internal(ctx, olen, buf, blen, f_rng, p_rng, restart_enabled));
#else
    switch (ctx->var)
    {
        case MBEDTLS_ECDH_VARIANT_MBEDTLS_2_0:
            return (ecdh_calc_secret_internal(&ctx->ctx.mbed_ecdh, olen, buf, blen, f_rng, p_rng, restart_enabled));
        default:
            return (MBEDTLS_ERR_ECP_BAD_INPUT_DATA);
    }
#endif
}

static int ecdh_make_public_internal(mbedtls_ecdh_context_mbed *ctx,
                                     size_t *olen,
                                     int point_format,
                                     unsigned char *buf,
                                     size_t blen,
                                     int (*f_rng)(void *, unsigned char *, size_t),
                                     void *p_rng,
                                     int restart_enabled)
{
    int ret;

    if (ctx->grp.pbits == 0)
        return (MBEDTLS_ERR_ECP_BAD_INPUT_DATA);

    (void)restart_enabled;

    if ((ret = mbedtls_ecdh_gen_public(&ctx->grp, &ctx->d, &ctx->Q, f_rng, p_rng)) != 0)
        return (ret);

    return mbedtls_ecp_tls_write_point(&ctx->grp, &ctx->Q, point_format, olen, buf, blen);
}

/*
 * Setup and export the client public value
 */
int mbedtls_ecdh_make_public_sw(mbedtls_ecdh_context *ctx,
                                size_t *olen,
                                unsigned char *buf,
                                size_t blen,
                                int (*f_rng)(void *, unsigned char *, size_t),
                                void *p_rng)
{
    int restart_enabled = 0;
    ECDH_VALIDATE_RET(ctx != NULL);
    ECDH_VALIDATE_RET(olen != NULL);
    ECDH_VALIDATE_RET(buf != NULL);
    ECDH_VALIDATE_RET(f_rng != NULL);

#if defined(MBEDTLS_ECDH_LEGACY_CONTEXT)
    return (ecdh_make_public_internal(ctx, olen, ctx->point_format, buf, blen, f_rng, p_rng, restart_enabled));
#else
    switch (ctx->var)
    {
        case MBEDTLS_ECDH_VARIANT_MBEDTLS_2_0:
            return (ecdh_make_public_internal(&ctx->ctx.mbed_ecdh, olen, ctx->point_format, buf, blen, f_rng, p_rng,
                                              restart_enabled));
        default:
            return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }
#endif
}

static int myrand(void *rng_state, unsigned char *output, size_t len)
{
    size_t use_len;
    int rnd;

    if (rng_state != NULL)
        rng_state = NULL;

    while (len > 0)
    {
        use_len = len;
        if (use_len > sizeof(int))
            use_len = sizeof(int);

        rnd = rand();
        memcpy(output, &rnd, use_len);
        output += use_len;
        len -= use_len;
    }

    return (0);
}

int mbedtls_ecdh_self_test(int verbose)
{
    int ret;
    uint8_t buf[100];
    mbedtls_ecdh_context ecdhClient, ecdhServer;
    const mbedtls_ecp_curve_info *curve_info = mbedtls_ecp_curve_list();
    size_t olen;

    for (uint32_t i = 0; curve_info->grp_id != MBEDTLS_ECP_DP_NONE; curve_info++, i++)
    {
        if (verbose != 0)
            mbedtls_printf("  ECDH %s - #%u: ", curve_info->name, i + 1);
        mbedtls_ecdh_init(&ecdhClient);
        mbedtls_ecdh_init(&ecdhServer);

        if (mbedtls_ecp_group_load(&ecdhClient.grp, curve_info->grp_id) != 0 ||
            mbedtls_ecdh_make_public(&ecdhClient, &olen, buf, sizeof(buf), myrand, NULL) != 0)
        {
            if (verbose != 0)
                mbedtls_printf("failed\n");
            continue;
        }
        if (mbedtls_ecp_group_load(&ecdhServer.grp, curve_info->grp_id) != 0 ||
            mbedtls_ecdh_make_public_sw(&ecdhServer, &olen, buf, sizeof(buf), myrand, NULL) != 0)
        {
            if (verbose != 0)
                mbedtls_printf("failed\n");
            continue;
        }

        mbedtls_ecp_copy(&ecdhServer.Qp, &ecdhClient.Q);
        mbedtls_ecp_copy(&ecdhClient.Qp, &ecdhServer.Q);

        ret = mbedtls_ecdh_calc_secret(&ecdhClient, &olen, buf, sizeof(buf), myrand, NULL);
        ret = mbedtls_ecdh_calc_secret_sw(&ecdhServer, &olen, buf, sizeof(buf), myrand, NULL);

        if (ret != 0 || memcmp(ecdhClient.z.p, ecdhServer.z.p, sizeof(mbedtls_mpi_uint) * ecdhClient.z.n) != 0)
        {
            if (verbose != 0)
                mbedtls_printf("failed\n");

            return (1);
        }
        mbedtls_ecdh_free(&ecdhServer);
        mbedtls_ecdh_free(&ecdhClient);

        if (verbose != 0)
            mbedtls_printf("passed\n");

        if (verbose != 0)
            mbedtls_printf("\n");
    }

    return (0);
}
#endif /* MBEDTLS_SELF_TEST */
#endif /*#if defined(MBEDTLS_ECDH_ALT) */
#endif /* MBEDTLS_ECDH_C */
