/*
 *  Elliptic curve DSA
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
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_ECDSA_C)

#include "mbedtls/ecdsa.h"
#include "mbedtls/asn1write.h"

#include <string.h>

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdlib.h>
#define mbedtls_calloc calloc
#define mbedtls_free free
#endif

#include "mbedtls/platform_util.h"

/* Parameter validation macros based on platform_util.h */
#define ECDSA_VALIDATE_RET(cond) MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_ECP_BAD_INPUT_DATA)
#define ECDSA_VALIDATE(cond) MBEDTLS_INTERNAL_VALIDATE(cond)

#define ECDSA_RS_ECP NULL

#define ECDSA_BUDGET(ops) /* no-op; for compatibility */

#define ECDSA_RS_ENTER(SUB) (void)rs_ctx
#define ECDSA_RS_LEAVE(SUB) (void)rs_ctx

#define MBEDTLS_ECDSA_MPI_S_HAVE_OBJECT (155u)
#define MBEDTLS_ECDSA_MPI_N_HAVE_OBJECT (1u)

#if defined(MBEDTLS_ECDSA_SIGN_ALT)
static int mbedtls_ecdsa_verify_digest_len(const size_t blen, sss_algorithm_t *alg)
{
    int ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    switch (blen)
    {
        case 32:
            *alg = kAlgorithm_SSS_ECDSA_SHA256;
            ret  = 0;
            break;
        case 64:
            *alg = kAlgorithm_SSS_ECDSA_SHA512;
            ret  = 0;
            break;
        default:
            ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
            break;
    }
    return ret;
}

static int mbedtls_ecdsa_verify_digest_allign(const size_t digestLen,
                                              const size_t ecCoordinateSize,
                                              const uint8_t *digest,
                                              uint8_t allignedDigest[])
{
    if (digestLen < ecCoordinateSize)
    {
        size_t diff = ecCoordinateSize - digestLen;
        memcpy(&allignedDigest[diff], digest, digestLen);
    }
    else
    {
        memcpy(allignedDigest, digest, digestLen);
    }
    return 0;
}

/*
 * Compute ECDSA signature of a hashed message (SEC1 4.1.3)
 * Obviously, compared to SEC1 4.1.3, we skip step 4 (hash message)
 */
static int ecdsa_sign_restartable(mbedtls_ecp_group *grp,
                                  mbedtls_mpi *r,
                                  mbedtls_mpi *s,
                                  const mbedtls_mpi *d,
                                  const unsigned char *buf,
                                  size_t blen,
                                  int (*f_rng)(void *, unsigned char *, size_t),
                                  void *p_rng,
                                  mbedtls_ecdsa_restart_ctx *rs_ctx)
{
    int ret = 0;
    sss_sscp_asymmetric_t asyc;
    sss_algorithm_t alg;
    size_t bufLen           = (blen + 7u) / 8u;
    size_t coordinateLen    = (grp->pbits + 7u) / 8u;
    size_t signatureSize    = 2 * coordinateLen;
    uint8_t *signature      = mbedtls_calloc(signatureSize, sizeof(uint8_t));
    uint8_t *allignedDigest = mbedtls_calloc(coordinateLen, sizeof(uint8_t));

    /* Fail cleanly on curves such as Curve25519 that can't be used for ECDSA */
    if (grp->N.p == NULL)
        return (MBEDTLS_ERR_ECP_BAD_INPUT_DATA);
    CRYPTO_InitHardware();
    if (d->s != MBEDTLS_ECDSA_MPI_S_HAVE_OBJECT)
    {
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
    else if (d->n != MBEDTLS_ECDSA_MPI_N_HAVE_OBJECT)
    {
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
    else if ((ret = mbedtls_ecdsa_verify_digest_len(bufLen, &alg)) != 0)
    {
    }
    else if (mbedtls_ecdsa_verify_digest_allign(bufLen, coordinateLen, (const uint8_t *)buf, allignedDigest) != 0)
    {
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
    else if (sss_sscp_asymmetric_context_init(&asyc, &g_sssSession, (sss_sscp_object_t *)d->p, alg, kMode_SSS_Sign) !=
             kStatus_SSS_Success)
    {
    }
    else if (sss_sscp_asymmetric_sign_digest(&asyc, allignedDigest, coordinateLen, signature, &signatureSize) !=
             kStatus_SSS_Success)
    {
    }
    else if ((ret = mbedtls_mpi_read_binary(r, signature, coordinateLen)) != 0)
    {
    }
    else if ((ret = mbedtls_mpi_read_binary(s, &signature[coordinateLen], coordinateLen)) != 0)
    {
    }
    else
    {
        ret = 0;
    }
    sss_sscp_asymmetric_context_free(&asyc);
    mbedtls_free(allignedDigest);
    mbedtls_free(signature);
    return (ret);
}

#if defined(MBEDTLS_ECDSA_ALT)
/*
 * Initialize context
 */
void mbedtls_ecdsa_init(mbedtls_ecdsa_context *ctx)
{
    ECDSA_VALIDATE(ctx != NULL);
    ctx->isKeyInitialized = false;
    mbedtls_ecp_keypair_init((mbedtls_ecp_keypair *)ctx);
}

/*
 * Free context
 */
void mbedtls_ecdsa_free(mbedtls_ecdsa_context *ctx)
{
    if (ctx == NULL)
        return;
    if (ctx->isKeyInitialized)
        sss_sscp_key_object_free(&ctx->key);
    mbedtls_ecp_keypair_free((mbedtls_ecp_keypair *)ctx);
}
#endif /* MBEDTLS_ECDSA_ALT */

#if defined(MBEDTLS_ECDSA_GENKEY_ALT)
/*
 * Generate key pair
 */
int mbedtls_ecdsa_genkey(mbedtls_ecdsa_context *ctx,
                         mbedtls_ecp_group_id gid,
                         int (*f_rng)(void *, unsigned char *, size_t),
                         void *p_rng)
{
    int ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    ECDSA_VALIDATE_RET(ctx != NULL);
    ret = mbedtls_ecp_group_load(&ctx->grp, gid);
    if (ret != 0)
        return (ret);
    size_t keyLen     = (ctx->grp.pbits + 7u) / 8u;
    size_t keyBitsLen = ctx->grp.pbits;
    size_t keySize    = 3 * keyLen;
    uint8_t *pubKey   = mbedtls_calloc(keySize, sizeof(uint8_t));
    CRYPTO_InitHardware();
    if (ctx->isKeyInitialized == false)
    {
        if (sss_sscp_key_object_init(&ctx->key, &g_keyStore) != kStatus_SSS_Success)
        {
            sss_sscp_key_object_free(&ctx->key);
            return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
        }
        /* Allocate key handle */
        else if (sss_sscp_key_object_allocate_handle(&ctx->key, 0u, kSSS_KeyPart_Pair, kSSS_CipherType_EC_NIST_P,
                                                     3 * keyLen, 0xF0u) != kStatus_SSS_Success)
        {
            sss_sscp_key_object_free(&ctx->key);
            return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
        }
        else
        {
            ctx->isKeyInitialized = true;
        }
    }
    if (sss_sscp_key_store_generate_key(&g_keyStore, &ctx->key, keyBitsLen, NULL) != kStatus_SSS_Success)
    {
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
    else if (sss_sscp_key_store_get_key(&g_keyStore, &ctx->key, pubKey, &keySize, &keyBitsLen, NULL) !=
             kStatus_SSS_Success)
    {
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
    else if ((ret = mbedtls_mpi_read_binary(&ctx->Q.X, pubKey, keyLen)) != 0)
    {
    }
    else if ((ret = mbedtls_mpi_read_binary(&ctx->Q.Y, &pubKey[keyLen], keyLen)) != 0)
    {
    }
    else if ((ret = mbedtls_mpi_lset(&ctx->Q.Z, 1)) != 0)
    {
    }
    else
    {
        ctx->d.s = MBEDTLS_ECDSA_MPI_S_HAVE_OBJECT;
        ctx->d.n = MBEDTLS_ECDSA_MPI_N_HAVE_OBJECT;
        ctx->d.p = (mbedtls_mpi_uint *)&ctx->key;
        ret      = 0;
    }
    mbedtls_platform_zeroize(pubKey, keySize);
    mbedtls_free(pubKey);
    return 0;
}
#endif /* MBEDTLS_ECDSA_GENKEY_ALT */

/*
 * Compute ECDSA signature of a hashed message
 */
int mbedtls_ecdsa_sign(mbedtls_ecp_group *grp,
                       mbedtls_mpi *r,
                       mbedtls_mpi *s,
                       const mbedtls_mpi *d,
                       const unsigned char *buf,
                       size_t blen,
                       int (*f_rng)(void *, unsigned char *, size_t),
                       void *p_rng)
{
    ECDSA_VALIDATE_RET(grp != NULL);
    ECDSA_VALIDATE_RET(r != NULL);
    ECDSA_VALIDATE_RET(s != NULL);
    ECDSA_VALIDATE_RET(d != NULL);
    ECDSA_VALIDATE_RET(f_rng != NULL);
    ECDSA_VALIDATE_RET(buf != NULL || blen == 0);

    return (ecdsa_sign_restartable(grp, r, s, d, buf, blen, f_rng, p_rng, NULL));
}
#endif /* MBEDTLS_ECDSA_SIGN_ALT */

#if defined(MBEDTLS_ECDSA_VERIFY_ALT)
/*
 * Verify ECDSA signature of hashed message (SEC1 4.1.4)
 * Obviously, compared to SEC1 4.1.3, we skip step 2 (hash message)
 */
static int ecdsa_verify_restartable(mbedtls_ecp_group *grp,
                                    const unsigned char *buf,
                                    size_t blen,
                                    const mbedtls_ecp_point *Q,
                                    const mbedtls_mpi *r,
                                    const mbedtls_mpi *s,
                                    mbedtls_ecdsa_restart_ctx *rs_ctx)
{
    int ret;
    /* Fail cleanly on curves such as Curve25519 that can't be used for ECDSA */
    if (grp->N.p == NULL)
        return (MBEDTLS_ERR_ECP_BAD_INPUT_DATA);
    size_t coordinateLen     = (grp->pbits + 7u) / 8u;
    size_t coordinateBitsLen = grp->pbits;
    size_t keySize           = 3 * coordinateLen;
    size_t bufLen            = (blen + 7u) / 8u;
    uint8_t *pubKey          = mbedtls_calloc(keySize, sizeof(uint8_t));
    sss_sscp_object_t ecdsaPublic;
    sss_sscp_asymmetric_t asyc;
    sss_algorithm_t alg;
    uint8_t *allignedDigest = mbedtls_calloc(coordinateLen, sizeof(uint8_t));
    CRYPTO_InitHardware();
    if ((ret = mbedtls_ecdsa_verify_digest_len(bufLen, &alg)) != 0)
    {
    }
    else if (mbedtls_ecdsa_verify_digest_allign(bufLen, coordinateLen, (const uint8_t *)buf, allignedDigest) != 0)
    {
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
    else if ((ret = mbedtls_mpi_write_binary(&Q->X, pubKey, coordinateLen)) != 0)
    {
    }
    else if ((ret = mbedtls_mpi_write_binary(&Q->Y, &pubKey[coordinateLen], coordinateLen)) != 0)
    {
    }
    else if (sss_sscp_key_object_init(&ecdsaPublic, &g_keyStore) != kStatus_SSS_Success)
    {
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
    /* Allocate key handle */
    else if (sss_sscp_key_object_allocate_handle(&ecdsaPublic, 0u, kSSS_KeyPart_Pair, kSSS_CipherType_EC_NIST_P,
                                                 keySize, 0xF0u) != kStatus_SSS_Success)
    {
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
    else if (sss_sscp_key_store_set_key(&g_keyStore, &ecdsaPublic, (const uint8_t *)pubKey, keySize, coordinateBitsLen,
                                        NULL) != kStatus_SSS_Success)
    {
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
    else if ((ret = mbedtls_mpi_write_binary(r, pubKey, coordinateLen)) != 0)
    {
    }
    else if ((ret = mbedtls_mpi_write_binary(s, &pubKey[coordinateLen], coordinateLen)) != 0)
    {
    }
    else if (sss_sscp_asymmetric_context_init(&asyc, &g_sssSession, &ecdsaPublic, alg, kMode_SSS_Verify) !=
             kStatus_SSS_Success)
    {
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
    else if (sss_sscp_asymmetric_verify_digest(&asyc, allignedDigest, coordinateLen, pubKey, coordinateLen * 2u) !=
             kStatus_SSS_Success)
    {
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
    else
    {
        ret = 0;
    }
    sss_sscp_key_object_free(&ecdsaPublic);
    sss_sscp_asymmetric_context_free(&asyc);
    mbedtls_platform_zeroize(pubKey, keySize);
    mbedtls_free(pubKey);
    mbedtls_platform_zeroize(allignedDigest, coordinateLen);
    mbedtls_free(allignedDigest);
    return ret;
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
    ECDSA_VALIDATE_RET(grp != NULL);
    ECDSA_VALIDATE_RET(Q != NULL);
    ECDSA_VALIDATE_RET(r != NULL);
    ECDSA_VALIDATE_RET(s != NULL);
    ECDSA_VALIDATE_RET(buf != NULL || blen == 0);

    return (ecdsa_verify_restartable(grp, buf, blen, Q, r, s, NULL));
}
#endif /* MBEDTLS_ECDSA_VERIFY_ALT */

#endif /* MBEDTLS_ECDSA_C */
