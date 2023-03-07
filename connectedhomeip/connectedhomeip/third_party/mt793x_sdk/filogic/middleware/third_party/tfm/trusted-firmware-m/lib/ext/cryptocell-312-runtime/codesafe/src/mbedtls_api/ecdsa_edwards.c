/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_ECDSA_C)

#include "mbedtls/ecdsa.h"
#include "cc_ecc_internal.h"
#include "mbedtls_common.h"
#include "ecp_common.h"
#include "cc_pal_types.h"
#include "cc_pal_mem.h"
#include "cc_pal_abort.h"
#include "cc_common.h"
#include "cc_ecpki_error.h"
#include "ec_edw.h"

#include "cc_ec_edw_api.h"
#include "ec_edw_local.h"
#include "cc_ecpki_domain.h"
#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdlib.h>
#include <stdio.h>
#define mbedtls_printf     printf
#define mbedtls_calloc    calloc
#define mbedtls_free       free
#endif

#ifdef MBEDTLS_ECP_DP_CURVE25519_ENABLED

static int ecdsa_export_mpi_to_buff(const mbedtls_mpi *mpi, size_t *outBuffSize, unsigned char *buf, size_t buffSize)
{
    int ret;
    uint32_t i = 0;
    uint32_t actSize;
    uint32_t zeroFill;

    actSize = mbedtls_mpi_size(mpi);
    zeroFill = buffSize - actSize;
    i = 0;
    while (zeroFill) {
        buf[i] = 0;
        zeroFill--;
        i++;
    }
    ret = mbedtls_mpi_write_binary(mpi, &buf[i], actSize);
    if (ret != 0)
    {
        CC_PAL_LOG_ERR("Error - failed mbedtls_mpi_write_binary 0x%x\n", ret);
        return ret;
    }
    *outBuffSize = buffSize;
    return CC_OK;

}

/*
 * Generate key pair for Edwards ed25519 curve
 */
int mbedtls_ecdsa_genkey_edwards( mbedtls_ecdsa_context *ctx, mbedtls_ecp_group_id gid,
                                  int (*f_rng)(void *, unsigned char *, size_t), void *p_rng )
{
    mbedtls_ecp_group *grp;
    mbedtls_mpi *d;
    mbedtls_ecp_point *Q;
    int ret = 0;
    CCError_t rc = 0;
    const CCEcEdwDomain_t *pDomain;
    uint32_t    *pFuncTemp;
    uint32_t    ecEdwOrderSizeBytes;
    uint8_t     *pPrivKey;  // size is 2*ecEdwOrderSizeBytes
    uint32_t    privKeySizeBytes;
    uint8_t     *pPublicKey;  // size is ecEdwOrderSizeBytes
    uint32_t    pubKeySizeBytes;
    CCEcEdwTempBuff_t     *pTempBuff = NULL; // size is 3*ecEdwOrderSizeBytes
    uint8_t *pSeed;

    /* Verify inputs */
    if ((ctx == NULL ) ||
            (f_rng == NULL) ||
            (p_rng == NULL) ||
            (gid != MBEDTLS_ECP_DP_CURVE25519 ))
    {
        CC_PAL_LOG_ERR("Failed check params");
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    grp = &ctx->grp;
    d = &ctx->d;
    Q = &ctx->Q;

    pDomain = EcEdwGetDomain25519();
    if (NULL == pDomain)
    {
        CC_PAL_LOG_ERR("Failed invalid domain");
        return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    ecEdwOrderSizeBytes = CALC_FULL_BYTES(pDomain->ecOrdSizeInBits);
    privKeySizeBytes = 2*ecEdwOrderSizeBytes;
    pubKeySizeBytes = CALC_FULL_BYTES(pDomain->ecModSizeInBits);
    pTempBuff = mbedtls_calloc(1, ((ecEdwOrderSizeBytes + sizeof(CCEcEdwTempBuff_t) + privKeySizeBytes + pubKeySizeBytes)));
    if (NULL == pTempBuff)
    {
        CC_PAL_LOG_ERR("Error - failed to allocate memory for temporary buffer\n");
        return MBEDTLS_ERR_ECP_ALLOC_FAILED;
    }

    pSeed = (uint8_t*)pTempBuff;
    pFuncTemp = (uint32_t*)pTempBuff + CALC_32BIT_WORDS_FROM_BYTES(ecEdwOrderSizeBytes);
    pPrivKey =  (uint8_t*)pFuncTemp + sizeof( CCEcEdwTempBuff_t );
    pPublicKey = pPrivKey + privKeySizeBytes;

    /* generate random seed */
    rc = f_rng((void *)p_rng, (unsigned char *)pSeed, (size_t)ecEdwOrderSizeBytes);
    if (rc) {
        ret = error_mapping_cc_to_mbedtls_ecc(rc);
        CC_PAL_LOG_ERR("Failed to generate random seed");
        goto END;
    }

    /* generate key pair */
    rc = EcEdwSeedKeyPair(pPublicKey, pPrivKey,
                          pSeed, pDomain,
                          (uint32_t*)pFuncTemp);
    if (rc != CC_SUCCESS)
    {
        CC_PAL_LOG_ERR("Error - Key generation ended with result: 0x%x\n", rc);
        ret =  error_mapping_cc_to_mbedtls_ecc(rc);
        goto END;
    }
     ret = mbedtls_mpi_read_binary(&Q->Y, pPublicKey, CALC_FULL_BYTES(pDomain->ecModSizeInBits));
    if (ret != 0)
    {
         CC_PAL_LOG_ERR("Error - failed to allocate memory for Q->Y\n");
        goto END;
    }
    Q->Y.s = 1; /*unsigned*/

    ret = mbedtls_mpi_read_binary(d, pPrivKey, CALC_FULL_BYTES(2*pDomain->ecModSizeInBits));
    if (ret != 0)
    {
         CC_PAL_LOG_ERR("Error - failed to allocate memory for Q->Y\n");
        goto END;
    }
    d->s = 1; /*unsigned*/


    /* Set the group curve order used by sign & verify functions */
    grp->nbits = pDomain->ecModSizeInBits;  // 253
    ret = (0);
    goto SUCCESS;

    END:

    mbedtls_ecp_point_free(Q);
    mbedtls_mpi_free(d);
    SUCCESS:
    mbedtls_zeroize_internal(pTempBuff, (ecEdwOrderSizeBytes + sizeof(CCEcEdwTempBuff_t) + privKeySizeBytes + pubKeySizeBytes));
    mbedtls_free(pTempBuff);
    return ret;

}


int mbedtls_ecdsa_sign_edwards( mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
                                const mbedtls_mpi *d, const unsigned char *buf, size_t blen)
{
    int ret;
    CCEcEdwTempBuff_t *cc_temp_buff;
    uint8_t temp_buf[ (CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS+1)*sizeof(uint32_t) ] = {0};
    const CCEcEdwDomain_t*  pDomain;
    size_t signature_size;
    uint8_t * pSignature;
    size_t key_size, order_size;
    uint32_t status;

    CC_UNUSED_PARAM(grp);
    /* Verify inputs */
    if ((r == NULL) ||
        (s == NULL) ||
        (d == NULL) ||
        ((buf == NULL) ^ (blen == 0)))
    {
        CC_PAL_LOG_ERR("Failed check params");
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    pDomain =  EcEdwGetDomain25519();
    if (NULL == pDomain)
    {
        return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    order_size = CALC_FULL_BYTES (pDomain->ecModSizeInBits);
    signature_size = order_size * 2 ;

    cc_temp_buff = (CCEcEdwTempBuff_t *)mbedtls_calloc(1, sizeof(CCEcEdwTempBuff_t) + signature_size );
    if ( cc_temp_buff == NULL )
    {
        CC_PAL_LOG_ERR("Error - failed to alloc memory for the cc_temp_buff\n");
        return MBEDTLS_ERR_ECP_ALLOC_FAILED;
    }
    mbedtls_zeroize_internal(cc_temp_buff, signature_size + sizeof(CCEcEdwTempBuff_t));

    pSignature = (uint8_t *)(cc_temp_buff + 1);

    ret = ecdsa_export_mpi_to_buff(d, &key_size, temp_buf, order_size * 2);

    if (ret !=0 )
    {
        CC_PAL_LOG_ERR("Error - failed to convert d to binary\n");
        goto end;
    }

    status = CC_EcEdwSign(pSignature,
                          &signature_size,
                          (uint8_t*)buf,
                          blen,
                          temp_buf,
                          key_size,
                          cc_temp_buff);
    if (( status != CC_OK ) ||
            (signature_size != order_size * 2))
    {
        CC_PAL_LOG_ERR("Error - Edwards signing failed, return code is: %d\n", status);
        ret = error_mapping_cc_to_mbedtls_ecc(status);
        goto end;
    }

    ret = mbedtls_mpi_read_binary( r, pSignature, order_size );
    if ( ret != 0 )
    {
        CC_PAL_LOG_ERR("Error - failed to convert r\n");
        goto end;
    }

    ret = mbedtls_mpi_read_binary( s, pSignature + order_size, order_size );
    if ( ret != 0 )
    {
        CC_PAL_LOG_ERR("Error - failed to convert s\n");
        goto end;
    }

    end:

    mbedtls_zeroize_internal(cc_temp_buff, signature_size + sizeof(CCEcEdwTempBuff_t) );
    mbedtls_free( cc_temp_buff );

    return ret;
}


int mbedtls_ecdsa_verify_edwards(mbedtls_ecp_group *grp, const unsigned char *buf, size_t blen,
                                 const mbedtls_ecp_point *Q, const mbedtls_mpi *r, const mbedtls_mpi *s)
{
    const CCEcEdwDomain_t*  pDomain;
    CCEcEdwTempBuff_t* p_cc_temp_buf;
    size_t pub_key_size;
    uint8_t pub_key_buf[ (2*CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS+1)*CC_32BIT_WORD_SIZE ] = {0};
    uint8_t * p_signature;
    int ret;
    CCError_t status;

    size_t order_size = 0;
    size_t tmp_size = 0;
    uint32_t signature_size = 0 ;

    CC_UNUSED_PARAM(grp);

    /* Verify inputs */
    if (((buf == NULL) ^ (blen == 0)) ||
            (Q == NULL) ||
            (r == NULL) ||
            (s == NULL))
    {
        CC_PAL_LOG_ERR("Failed check params");
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    pDomain =  EcEdwGetDomain25519();
    if (NULL == pDomain)
    {
        return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    order_size = CALC_FULL_BYTES (pDomain->ecModSizeInBits);
    signature_size = order_size*2 ;

    /* Only the Y coordinate is required */
    pub_key_size = order_size;
    ret = ecdsa_export_mpi_to_buff( &Q->Y, &pub_key_size, pub_key_buf, order_size );
    if ((ret != 0) ||
        (pub_key_size != order_size))
    {
        CC_PAL_LOG_ERR("Error - converting Q\n");
        return ret;
    }

    p_cc_temp_buf = (CCEcEdwTempBuff_t*)mbedtls_calloc( 1, sizeof(CCEcEdwTempBuff_t) + signature_size );
    if ( p_cc_temp_buf == NULL )
    {
        CC_PAL_LOG_ERR("Error - cant allocate memory for CC temp buf\n");
        return MBEDTLS_ERR_ECP_ALLOC_FAILED;
    }

    p_signature = (uint8_t *)(p_cc_temp_buf + 1);
    tmp_size = order_size;

    ret = ecdsa_export_mpi_to_buff( r, &tmp_size, p_signature, order_size );
    if ((ret != 0) ||
        (tmp_size != order_size))
    {
        CC_PAL_LOG_ERR("Error - converting r\n");
        goto end;
    }

   ret = ecdsa_export_mpi_to_buff( s, &tmp_size, p_signature + order_size, order_size );
    if ((ret != 0) ||
        (tmp_size != order_size))
    {
        CC_PAL_LOG_ERR("Error - converting s\n");
        goto end;
    }


    status =  CC_EcEdwVerify ( p_signature,
                               signature_size,
                               pub_key_buf,
                               pub_key_size,
                               (uint8_t*)buf,
                               blen,
                               p_cc_temp_buf);

    if ( status != CC_OK )
    {
        ret = error_mapping_cc_to_mbedtls_ecc(status);
        CC_PAL_LOG_ERR("Error - verification failed with return code 0x%08x\n", status);
    }

    end:
    mbedtls_zeroize_internal(p_cc_temp_buf, sizeof(CCEcEdwTempBuff_t) + signature_size);
    mbedtls_free( p_cc_temp_buf );
    return ret;
}


int mbedtls_ecdsa_public_key_read_edwards( mbedtls_ecp_point *Q,
                                           unsigned char *buf, size_t blen )
{
    const CCEcEdwDomain_t *pDomain;
    int ret;

    if ((Q == NULL) ||
            (buf == NULL) ||
            (blen == 0)) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }
    pDomain = EcEdwGetDomain25519();
    if (NULL == pDomain) {
        return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    if (blen != CALC_FULL_BYTES(pDomain->ecModSizeInBits)) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    ret = mbedtls_mpi_read_binary(&Q->Y, buf, blen);
    if (ret != 0)
    {
        CC_PAL_LOG_ERR("Error - failed to allocate memory for Q->Y\n");
        return ret;
    }
    Q->Y.s = 1; /*unsigned*/

    return CC_OK;

}


int mbedtls_ecdsa_public_key_write_edwards( const mbedtls_ecp_point *Q,
                                            size_t *olen,
                                            unsigned char *buf, size_t blen )
{
    int ret;
    uint32_t keySize;
    const CCEcEdwDomain_t *pDomain;

    if ((Q == NULL) ||
            (olen == NULL) ||
            (buf == NULL) ||
            (blen == 0)) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }
    pDomain = EcEdwGetDomain25519();
    if (NULL == pDomain) {
        return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }
    keySize = CALC_FULL_BYTES(pDomain->ecModSizeInBits);
    if (blen < keySize)
    {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }
    ret = ecdsa_export_mpi_to_buff(&Q->Y, olen, buf, keySize);
    if (ret != 0)
    {
        CC_PAL_LOG_ERR("Error - failed to allocate memory for Q->Y\n");
        return ret;
    }

    return CC_OK;
}


#endif  /* MBEDTLS_ECP_DP_CURVE25519_ENABLED */
#endif /* defined(MBEDTLS_ECDSA_C) */
