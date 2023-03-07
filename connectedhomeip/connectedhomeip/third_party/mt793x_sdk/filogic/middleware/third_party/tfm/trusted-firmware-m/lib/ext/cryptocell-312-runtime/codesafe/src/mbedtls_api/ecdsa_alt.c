/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
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

#include "cc_ecc_internal.h"


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

#include "mbedtls_common.h"
#include "ecp_common.h"
#include "cc_pal_types.h"
#include "cc_pal_mem.h"
#include "cc_pal_abort.h"
#include "cc_common.h"
#include "cc_ecpki_error.h"

#if defined (ECP_SHORTWEIERSTRASS)

#include "cc_ecpki_ecdsa.h"
#include "cc_ecpki_build.h"

#endif
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

#if defined(MBEDTLS_ECDSA_VERIFY_ALT) || defined (MBEDTLS_ECDSA_SIGN_ALT)
static CCEcpkiHashOpMode_t message_size_to_hash_mode( size_t bytelen )
{
    CCEcpkiHashOpMode_t hash_mode;
    size_t wordlen = (bytelen / sizeof(uint32_t));

    switch( wordlen )
    {
        case CC_HASH_SHA1_DIGEST_SIZE_IN_WORDS:
            hash_mode = CC_ECPKI_AFTER_HASH_SHA1_mode;
            break;
        case CC_HASH_SHA224_DIGEST_SIZE_IN_WORDS:
            hash_mode = CC_ECPKI_AFTER_HASH_SHA224_mode;
            break;
        case CC_HASH_SHA256_DIGEST_SIZE_IN_WORDS:
            hash_mode = CC_ECPKI_AFTER_HASH_SHA256_mode;
            break;
        case CC_HASH_SHA384_DIGEST_SIZE_IN_WORDS:
            hash_mode = CC_ECPKI_AFTER_HASH_SHA384_mode;
            break;
        case CC_HASH_SHA512_DIGEST_SIZE_IN_WORDS:
            hash_mode = CC_ECPKI_AFTER_HASH_SHA512_mode;
            break;
        default:
            hash_mode = CC_ECPKI_AFTER_HASH_SHA512_mode;//use default value, to avoid checking the hash
    }

    return hash_mode;
}
#endif

#if defined(MBEDTLS_ECDSA_SIGN_ALT)
#if defined(ECP_SHORTWEIERSTRASS)

static int ecdsa_wrst_sign( mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
                const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
                int (*f_rng)(void *, unsigned char *, size_t), void *p_rng )
{
    int ret;
    CCEcpkiDomainID_t domainId;
    CCEcdsaSignUserContext_t* temp_context;
    CCRndContext_t      rnd_ctx;
    uint8_t temp_buf[ (CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS+1)*sizeof(uint32_t) ] = {0};
    CCEcpkiUserPrivKey_t* priv_key;
    const CCEcpkiDomain_t*  pDomain;
    CCEcpkiHashOpMode_t hash_mode;
    size_t signature_size;
    uint8_t * pSignature;
    size_t key_size, order_size;
    uint32_t status;

    ret = ecp_grp_id_to_domain_id( grp->id, &domainId );
    if (ret != 0)
    {
        return ret;
    }
    pDomain =  CC_EcpkiGetEcDomain( domainId );

    if (NULL == pDomain)
    {
        CC_PAL_LOG_ERR( "Error - domain id: %d is not supported\n",domainId );
        return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    rnd_ctx.rndGenerateVectFunc = f_rng;
    rnd_ctx.rndState = p_rng;

    order_size = CALC_FULL_BYTES ( grp->nbits );
    signature_size =  order_size*2;

    temp_context = (CCEcdsaSignUserContext_t *)mbedtls_calloc( 1, sizeof( CCEcdsaSignUserContext_t ) + signature_size + sizeof( CCEcpkiUserPrivKey_t ) );
    if ( temp_context == NULL )
    {
        CC_PAL_LOG_ERR( "Error - failed to alloc memory for the temp context\n" );
        return MBEDTLS_ERR_ECP_ALLOC_FAILED;
    }
    mbedtls_zeroize_internal( temp_context, signature_size + sizeof( CCEcdsaSignUserContext_t ) + sizeof( CCEcpkiUserPrivKey_t ) );

    key_size = mbedtls_mpi_size( d );

    hash_mode = message_size_to_hash_mode( blen );

    priv_key = (CCEcpkiUserPrivKey_t *)(temp_context + 1);

    pSignature = (uint8_t *)(priv_key + 1);

    ret = mbedtls_mpi_write_binary( d, temp_buf, key_size );

    if (ret !=0 )
    {
        CC_PAL_LOG_ERR( "Error - failed to convert d to binary\n" );
        goto end;
    }
    status = CC_EcpkiPrivKeyBuild( pDomain,
                             temp_buf,
                             mbedtls_mpi_size( d ),
                             priv_key );
    if ( status != CC_OK )
    {
        CC_PAL_LOG_ERR( "Error - failed to build private key, return code is: %d\n", status );
        ret = error_mapping_cc_to_mbedtls_ecc( status );
         goto end;
    }

    status =  CC_EcdsaSign( &rnd_ctx,
                         temp_context,
                         priv_key,
                         hash_mode,
                         (uint8_t*)buf,
                         blen,
                         pSignature,
                         &signature_size );

    if ( status != CC_OK )
    {
        CC_PAL_LOG_ERR( "Error - signing failed, return code is: %d\n", status );
        ret = error_mapping_cc_to_mbedtls_ecc( status );
        goto end;
    }

    ret = mbedtls_mpi_read_binary( r, pSignature, order_size );
    if ( ret != 0 )
    {
        CC_PAL_LOG_ERR( "Error - failed to convert r\n" );
        goto end;
    }
    ret = mbedtls_mpi_read_binary( s, pSignature + order_size , order_size );
    if ( ret != 0 )
    {
        CC_PAL_LOG_ERR( "Error - failed to convert s\n" );
        goto end;
    }

end:

       mbedtls_zeroize_internal( temp_context, signature_size + sizeof( CCEcdsaSignUserContext_t ) + sizeof( CCEcpkiUserPrivKey_t ) );
       mbedtls_free( temp_context );

       return ret;
}

#endif /*(ECP_SHORTWEIERSTRASS)*/

/*
 * Compute ECDSA signature of a hashed message (SEC1 4.1.3)
 * Obviously, compared to SEC1 4.1.3, we skip step 4 (hash message)
 * The only exception is for curve 25519-Edwareds where we do hash
 */
int mbedtls_ecdsa_sign( mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
                const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
                int (*f_rng)(void *, unsigned char *, size_t), void *p_rng )
{

    /* Input parameters validation */
    if (NULL == grp || NULL == r || NULL == s || NULL == d || NULL == buf)
    {
        CC_PAL_LOG_ERR("Error - NULL pointer exception\n");
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );

    }

#if defined(ECP_SHORTWEIERSTRASS)

    if( ecp_get_type( grp ) == ECP_TYPE_SHORT_WEIERSTRASS )
    {
        if(NULL == f_rng || NULL == p_rng)
        {
            CC_PAL_LOG_ERR("Error - NULL pointer exception\n");
            return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
        }
        return ecdsa_wrst_sign(grp, r, s, d, buf, blen, f_rng, p_rng);
    }
#endif /* ECP_SHORTWEIERSTRASS */
    return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
}
#endif /* MBEDTLS_ECDSA_SIGN_ALT */

#if defined(MBEDTLS_ECDSA_VERIFY_ALT)
#if defined(ECP_SHORTWEIERSTRASS)

static int ecdsa_wrst_verify( mbedtls_ecp_group *grp,
                  const unsigned char *buf, size_t blen,
                  const mbedtls_ecp_point *Q, const mbedtls_mpi *r, const mbedtls_mpi *s)
{
    CCEcpkiHashOpMode_t hash_mode;
    CCEcdsaVerifyUserContext_t* pTemp_context;
    CCEcpkiUserPublKey_t* pPub_key;
    size_t temp_size, order_size;
    uint8_t pTemp_buf[ (2*CC_ECPKI_MODUL_MAX_LENGTH_IN_WORDS+1)*sizeof(uint32_t) ] = {0};
    uint8_t * pSignature;
    CCEcpkiDomainID_t domainId;
    const CCEcpkiDomain_t*  pDomain;
    int ret;
    CCError_t status;
    uint32_t signature_size;

    order_size = CALC_FULL_BYTES ( grp->nbits );
    signature_size = order_size *2;


    hash_mode = message_size_to_hash_mode( blen );

    ret = ecp_grp_id_to_domain_id( grp->id, &domainId );
    if (ret != 0)
    {
        return ret;
    }
    pDomain =  CC_EcpkiGetEcDomain(domainId);

    if (NULL == pDomain)
    {
        CC_PAL_LOG_ERR("Error - domain id %d is not supported\n",domainId);
        return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }


    ret = mbedtls_ecp_point_write_binary( grp, Q, MBEDTLS_ECP_PF_UNCOMPRESSED,
                                           &temp_size, pTemp_buf, sizeof(pTemp_buf) );

    if (ret != 0)
    {
        CC_PAL_LOG_ERR("Error - converting Q\n");
        return ret;
    }

    pPub_key = (CCEcpkiUserPublKey_t*)mbedtls_calloc( 1, sizeof(CCEcpkiUserPublKey_t) + sizeof(CCEcdsaVerifyUserContext_t) + signature_size );
    if ( pPub_key == NULL )
    {
        CC_PAL_LOG_ERR("Error - cant allocate memory for pPub_key\n");
        return MBEDTLS_ERR_ECP_ALLOC_FAILED;
    }
    pTemp_context = (CCEcdsaVerifyUserContext_t*)(pPub_key + 1);
    pSignature = (uint8_t *)(pTemp_context + 1);

    status = CC_EcpkiPubKeyBuild(pDomain, pTemp_buf, temp_size, pPub_key);
    if (status != CC_OK)
    {
        ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
        CC_PAL_LOG_ERR("Error - building public key failed with return code 0x%08x\n", status);
        goto end;
    }

    ret = mbedtls_mpi_write_binary( r, pSignature, order_size );
    if ( ret != 0 )
    {
        CC_PAL_LOG_ERR("Error - converting r\n");
        goto end;
    }

    ret = mbedtls_mpi_write_binary( s, pSignature + order_size, order_size );
    if ( ret != 0 )
    {
        CC_PAL_LOG_ERR("Error - converting s\n");
        goto end;
    }


    status =  CC_EcdsaVerify ( pTemp_context,
                               pPub_key,
                               hash_mode,
                               pSignature,
                               signature_size,
                               (uint8_t*)buf,
                               blen );

    if ( status != CC_OK )
    {
        ret = error_mapping_cc_to_mbedtls_ecc(status);
        CC_PAL_LOG_ERR("Error - verification failed with return code 0x%08x\n", status);
    }

end:
    mbedtls_zeroize_internal(pPub_key, sizeof(CCEcpkiUserPublKey_t) + sizeof(CCEcdsaVerifyUserContext_t) + signature_size);
    mbedtls_free( pPub_key );
    return ret;
}
#endif /* ECP_SHORTWEIERSTRASS */



/*
 * Verify ECDSA signature of hashed message (SEC1 4.1.4)
 * Obviously, compared to SEC1 4.1.3, we skip step 2 (hash message)
 * The only exception is for curve 25519-Edwareds where we do hash
 */
int mbedtls_ecdsa_verify( mbedtls_ecp_group *grp,
                  const unsigned char *buf, size_t blen,
                  const mbedtls_ecp_point *Q, const mbedtls_mpi *r, const mbedtls_mpi *s)
{
        /* Input parameters validation */
    if (NULL == grp || NULL == r || NULL == s || NULL == Q || buf == NULL)
    {
        CC_PAL_LOG_ERR("Error - NULL pointer exception\n");
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );

    }

#if defined(ECP_SHORTWEIERSTRASS)

    if( ecp_get_type( grp ) == ECP_TYPE_SHORT_WEIERSTRASS )
    {
        return ecdsa_wrst_verify(grp, buf, blen, Q, r, s);
    }
#endif /* ECP_SHORTWEIERSTRASS */
    return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
}
#endif /* MBEDTLS_ECDSA_VERIFY_ALT */

#if defined(MBEDTLS_ECDSA_GENKEY_ALT)
/*
 * Generate key pair
 */
int mbedtls_ecdsa_genkey( mbedtls_ecdsa_context *ctx, mbedtls_ecp_group_id gid,
                  int (*f_rng)(void *, unsigned char *, size_t), void *p_rng )
{
    return( mbedtls_ecp_group_load( &ctx->grp, gid ) ||
            cc_ecp_gen_keypair( &ctx->grp, &ctx->d, &ctx->Q, f_rng, p_rng ) );
}
#endif /* MBEDTLS_ECDSA_GENKEY_ALT */

#endif /* defined(MBEDTLS_ECDSA_C) */
