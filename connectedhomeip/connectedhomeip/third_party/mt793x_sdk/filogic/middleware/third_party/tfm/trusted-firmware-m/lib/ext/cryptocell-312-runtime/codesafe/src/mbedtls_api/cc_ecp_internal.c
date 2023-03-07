/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
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

#include "mbedtls/ecp.h"
#include "ecp_common.h"
#include "mbedtls_common.h"
#include "cc_pal_types.h"
#include "cc_pal_mem.h"
#include "cc_pal_abort.h"
#include "cc_common.h"
#include "cc_ecpki_error.h"
#if defined (ECP_MONTGOMERY)
#include "cc_ec_mont_api.h"
#endif
#if defined (ECP_SHORTWEIERSTRASS)
#include "pka_ec_wrst.h"
#include "cc_ecpki_kg.h"
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

static int ecc_conv_scalar_to_mpi( uint8_t * scalar, size_t scalarSize, mbedtls_mpi * X)
{
    CCError_t status;
    int ret;
    unsigned char * outArr = (unsigned char*)mbedtls_calloc(1, scalarSize);
    CC_UNUSED_PARAM(status); // Fix mps2 warning

    if (NULL == outArr)
    {
        CC_PAL_LOG_ERR("Error - failed to allocate memory\n");
        return MBEDTLS_ERR_ECP_ALLOC_FAILED;
    }
    if (CC_OK != (status = CC_CommonReverseMemcpy(outArr, scalar, scalarSize)))
    {
        mbedtls_free(outArr);
        CC_PAL_LOG_ERR("Error - failed to reverse memcpy, status = %d\n",status);
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }
    ret = mbedtls_mpi_read_binary(X, outArr, scalarSize);
    if (0 != ret)
    {
        mbedtls_free(outArr);
        CC_PAL_LOG_ERR("Error - failed to read binary to mpi, ret = %d\n",ret);
        return ret;
    }

    X->s = 1; /*unsigned*/

    mbedtls_free(outArr);

    return (0);
}

static int ecc_conv_mpi_to_scalar( const mbedtls_mpi * X, uint8_t *scalar, size_t *scalarSize)
{
    CCError_t  status;
    int ret;
    unsigned char * outArr = (unsigned char*)mbedtls_calloc(1, *scalarSize);
    CC_UNUSED_PARAM(status); // Fix mps2 warning

    if (NULL == outArr)
    {
        CC_PAL_LOG_ERR("Error - failed to allocate memory\n");
        return MBEDTLS_ERR_ECP_ALLOC_FAILED;
    }

    ret = mbedtls_mpi_write_binary(X, outArr, *scalarSize);
    if (0 != ret)
    {
        mbedtls_free(outArr);
        CC_PAL_LOG_ERR("Error - failed to write mpi to binary, ret = %d\n",ret);
        return ret;
    }

    if (CC_OK != (status = CC_CommonReverseMemcpy(scalar, outArr, *scalarSize)))
    {
        mbedtls_free(outArr);
        CC_PAL_LOG_ERR("Error - failed to reverse memcpy, status = %d\n",status);
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }
    *scalarSize = (X->n * sizeof(mbedtls_mpi_uint));

    mbedtls_free(outArr);

    return 0;
}

#if defined(ECP_MONTGOMERY)
static int ecp_mont_mul( mbedtls_ecp_point *R,
             const mbedtls_mpi *m, const mbedtls_ecp_point *P )
{
    int ret;
    CCError_t rc;
    CCEcMontTempBuff_t *ecMontTempBuff;
    uint8_t resPoint[CC_EC_MONT_MOD_SIZE_IN_BYTES];
    uint8_t scalar[MBEDTLS_ECP_MAX_BYTES];
    uint8_t px[CC_EC_MONT_MOD_SIZE_IN_BYTES];
    size_t resPointSize;
    size_t scalarSize, pxSize;

    scalarSize = MBEDTLS_ECP_MAX_BYTES;
    pxSize = CC_EC_MONT_MOD_SIZE_IN_BYTES;
    resPointSize = CC_EC_MONT_MOD_SIZE_IN_BYTES;

    ret = ecc_conv_mpi_to_scalar(m, scalar, &scalarSize);
    if (ret != 0)
    {
       return ret;
    }

    mbedtls_zeroize_internal(px, CC_EC_MONT_MOD_SIZE_IN_BYTES);
    ret = ecc_conv_mpi_to_scalar(&P->X, px, &pxSize);
    if (ret != 0)
    {
        return ret;
    }

    ecMontTempBuff = mbedtls_calloc( 1, sizeof( CCEcMontTempBuff_t ));
    if (NULL == ecMontTempBuff)
    {
        CC_PAL_LOG_ERR("Error - failed to allocate memory\n");
        return MBEDTLS_ERR_ECP_ALLOC_FAILED;
    }

    rc = CC_EcMontScalarmult(resPoint, &resPointSize, scalar, scalarSize, px, CC_EC_MONT_MOD_SIZE_IN_BYTES, ecMontTempBuff);

    mbedtls_free(ecMontTempBuff);
    /*secure programing*/
    mbedtls_zeroize_internal(scalar,scalarSize);

    if (rc != CC_SUCCESS)
    {
        CC_PAL_LOG_ERR("Error - multiplication ended with result: %d\n", rc);
        return error_mapping_cc_to_mbedtls_ecc(rc);
    }
    /* prepare the output point R*/
    /* Y is not used in the result, and Z is 1*/
    ret =  mbedtls_mpi_lset( &R->Z, 1 );
    if (ret != 0)
    {
        CC_PAL_LOG_ERR("Error - could not set R.z\n");
        return MBEDTLS_ERR_ECP_ALLOC_FAILED;
    }
    mbedtls_mpi_free(&R->Y);
    ret = ecc_conv_scalar_to_mpi(resPoint, resPointSize, &R->X);
    if (ret != 0)
    {
        return ret;
    }
    return (0);

}
#endif /* ECP_MONTGOMERY */

#if defined(ECP_SHORTWEIERSTRASS)

static int ecp_wrst_mul( mbedtls_ecp_group *grp, mbedtls_ecp_point *R,
             const mbedtls_mpi *m, const mbedtls_ecp_point *P )
{
    int ret;
    CCError_t rc;
    void *tmpBuf;
    const CCEcpkiDomain_t *pDomain;
    CCEcpkiDomainID_t domainId;

    ret = ecp_grp_id_to_domain_id(grp->id, &domainId);
    if (ret != 0)
    {
        CC_PAL_LOG_ERR("Error - group id %d is not supported\n",grp->id);
        return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;

    }
    pDomain = CC_EcpkiGetEcDomain(domainId);
    if (NULL == pDomain)
    {
        CC_PAL_LOG_ERR("Error - domain id %d is not supported\n",domainId);
        return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    tmpBuf = mbedtls_calloc(1, sizeof(CCEcpkiKgTempData_t));
    if (NULL == tmpBuf)
    {
        CC_PAL_LOG_ERR("Error - failed to allocate memory for temporary buffer\n");
        return MBEDTLS_ERR_ECP_ALLOC_FAILED;
    }
    ret = mbedtls_mpi_grow(&R->X, CALC_FULL_32BIT_WORDS(pDomain->modSizeInBits));
    if (ret != 0)
    {
        CC_PAL_LOG_ERR("Error - failed to allocate memory for R\n");
        mbedtls_free(tmpBuf);
        return MBEDTLS_ERR_ECP_ALLOC_FAILED;
    }

    ret = mbedtls_mpi_grow(&R->Y, CALC_FULL_32BIT_WORDS(pDomain->modSizeInBits));
    if (ret != 0)
    {
        CC_PAL_LOG_ERR("Error - failed to allocate memory for R.x\n");
        mbedtls_free(tmpBuf);
        mbedtls_mpi_free(&R->X);
        return MBEDTLS_ERR_ECP_ALLOC_FAILED;
    }

    rc = PkaEcWrstScalarMult(pDomain, m->p, m->n, P->X.p, P->Y.p, R->X.p, R->Y.p, tmpBuf);
    mbedtls_free(tmpBuf);
    if (rc != CC_SUCCESS)
    {
        CC_PAL_LOG_ERR("Error - multiplication ended with result: %d\n", rc);
        return error_mapping_cc_to_mbedtls_ecc(rc);
    }

    ret = mbedtls_mpi_lset( &R->Z, 1 );
    if (ret != 0)
    {
        CC_PAL_LOG_ERR("Error - failed to allocate memory for R\n");
        mbedtls_mpi_free(&R->X);
        mbedtls_mpi_free(&R->Y);
        return MBEDTLS_ERR_ECP_ALLOC_FAILED;
    }
    return (0);
}
#endif /* ECP_SHORTWEIERSTRASS */

/*
 * Multiplication R = m * P
 */
int cc_ecp_mul( mbedtls_ecp_group *grp, mbedtls_ecp_point *R,
             const mbedtls_mpi *m, const mbedtls_ecp_point *P,
             int (*f_rng)(void *, unsigned char *, size_t), void *p_rng )
{
    int ret;

    CC_UNUSED_PARAM(f_rng);
    CC_UNUSED_PARAM(p_rng);

    /* Input parameters validation */
    if (NULL == grp || NULL == R || NULL == m || NULL == P )
    {
        CC_PAL_LOG_ERR("Error - NULL pointer exception\n");
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );

    }
    /* Common sanity checks */
    if( mbedtls_mpi_cmp_int( &P->Z, 1 ) != 0 )
    {
        CC_PAL_LOG_ERR("Error - trying to multiply the infinity point\n");
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
    }

    ret = mbedtls_ecp_check_privkey( grp, m );
    if (ret !=0 )
    {
        CC_PAL_LOG_ERR("Error - bad private key\n");
        return( ret );
    }
    ret = mbedtls_ecp_check_pubkey( grp, P );
    if (ret != 0)
    {
        CC_PAL_LOG_ERR("Error - bad public key\n");
        return( ret );
    }

#if defined(ECP_MONTGOMERY)
    if( ecp_get_type( grp ) == ECP_TYPE_25519 )
    {
        return ecp_mont_mul( R, m, P);
    }
#endif
#if defined(ECP_SHORTWEIERSTRASS)
    if( ecp_get_type( grp ) == ECP_TYPE_SHORT_WEIERSTRASS )
    {
        return ecp_wrst_mul( grp, R, m, P);
    }
#endif
    return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
}

#if defined(ECP_MONTGOMERY)
static int ecp_mont_gen_keypair_base(
                     const mbedtls_ecp_point *G, /* can be NULL in order to use the curve generator*/
                     mbedtls_mpi *d, mbedtls_ecp_point *Q,
                     int (*f_rng)(void *, unsigned char *, size_t),
                     void *p_rng )
{
    int ret;
    CCError_t rc;
    CCEcMontTempBuff_t *ecMontTempBuff;
    uint8_t resPoint[CC_EC_MONT_MOD_SIZE_IN_BYTES];
    uint8_t scalar[MBEDTLS_ECP_MAX_BYTES];
    uint8_t px[CC_EC_MONT_MOD_SIZE_IN_BYTES];
    size_t resPointSize;
    size_t scalarSize, pxSize;
    CCRndContext_t *pRndContext;

    scalarSize = MBEDTLS_ECP_MAX_BYTES;
    pxSize = CC_EC_MONT_MOD_SIZE_IN_BYTES;
    resPointSize = CC_EC_MONT_MOD_SIZE_IN_BYTES;


    if (G != NULL) /* Base point was supplied by application*/
    {
        mbedtls_zeroize_internal(px, CC_EC_MONT_MOD_SIZE_IN_BYTES);
        ret = ecc_conv_mpi_to_scalar(&G->X, px, &pxSize);
        if (ret != 0)
        {
            return ret;
        }
    }

    ecMontTempBuff = mbedtls_calloc( 1, (sizeof( CCEcMontTempBuff_t ) + sizeof( CCRndContext_t )));
    if (NULL == ecMontTempBuff)
    {
        CC_PAL_LOG_ERR("Error - failed to allocate memory\n");
        return MBEDTLS_ERR_ECP_ALLOC_FAILED;
    }

    pRndContext = (CCRndContext_t *)(ecMontTempBuff + 1);
    pRndContext->rndGenerateVectFunc = (CCRndGenerateVectWorkFunc_t)f_rng;
    pRndContext->rndState = p_rng;

    if (G != NULL)
    {
        rc = CC_EcMontKeyPairBase(resPoint, &resPointSize, scalar, &scalarSize, px, CC_EC_MONT_MOD_SIZE_IN_BYTES, pRndContext, ecMontTempBuff);
    }
    else
    {
        rc = CC_EcMontKeyPair(resPoint, &resPointSize, scalar, &scalarSize, pRndContext, ecMontTempBuff);
    }

    mbedtls_free(ecMontTempBuff);

    if (rc != CC_SUCCESS)
    {
        CC_PAL_LOG_ERR("Error - keypair generation ended with result: %d\n", rc);
        return error_mapping_cc_to_mbedtls_ecc(rc);
    }

    ret = ecc_conv_scalar_to_mpi(scalar, scalarSize, d);
    /* secure programing */
    mbedtls_zeroize_internal(scalar, scalarSize);
    if (ret != 0)
    {
        goto END;
    }
    /* prepare the output point Q*/
    /* Y is not used in the result, and Z is 1*/
    ret =  mbedtls_mpi_lset( &Q->Z, 1 );
    if (ret != 0)
    {
        CC_PAL_LOG_ERR("Error - could not set Q.z\n");
        goto END;
    }
    mbedtls_mpi_free(&Q->Y);
    ret = ecc_conv_scalar_to_mpi(resPoint, resPointSize, &Q->X);
    if (ret != 0)
    {
        goto END;
    }

    /* see [Curve25519] page 5 */
    ret = mbedtls_mpi_set_bit(d, 0, 0);
    if (ret != 0)
    {
        goto END;
    }
    ret = mbedtls_mpi_set_bit(d, 1, 0);
    if (ret != 0)
    {
        goto END;
    }
    ret = mbedtls_mpi_set_bit(d, 2, 0);
    if (ret != 0)
    {
        goto END;
    }
    ret = mbedtls_mpi_set_bit(d, 255, 0);
    if (ret != 0)
    {
        goto END;
    }
    ret = mbedtls_mpi_set_bit(d, 254, 1);
    if (ret != 0)
    {
        goto END;
    }

    return (0);

    END:
        mbedtls_ecp_point_free(Q);
        mbedtls_mpi_free(d);
        return ret;

}
#endif /* ECP_MONTGOMERY */
#if defined(ECP_SHORTWEIERSTRASS)

static int ecp_wrst_gen_keypair_base( mbedtls_ecp_group *grp,
                     const mbedtls_ecp_point *G, /* can be NULL in order to use the curve generator*/
                     mbedtls_mpi *d, mbedtls_ecp_point *Q,
                     int (*f_rng)(void *, unsigned char *, size_t),
                     void *p_rng )
{
    int ret;
    CCError_t rc;
    const CCEcpkiDomain_t *pDomain;
    CCEcpkiDomainID_t domainId;
    CCRndContext_t *pRndContext;
    CCEcpkiUserPrivKey_t        *pUserPrivKey;
    CCEcpkiUserPublKey_t        *pUserPublKey;
    CCEcpkiPrivKey_t            *pPrivKey;
    CCEcpkiPublKey_t            *pPublicKey;

    CCEcpkiKgTempData_t         *pTempBuff = NULL;

    ret = ecp_grp_id_to_domain_id(grp->id, &domainId);
    if (ret != 0)
    {
        CC_PAL_LOG_ERR("Error - group id %d is not supported\n",grp->id);
        return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;

    }
    pDomain = CC_EcpkiGetEcDomain(domainId);
    if (NULL == pDomain)
    {
        CC_PAL_LOG_ERR("Error - domain id %d is not supported\n",domainId);
        return MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    pTempBuff = mbedtls_calloc(1, (sizeof( CCEcpkiKgTempData_t ) + sizeof( CCRndContext_t ) + sizeof( CCEcpkiUserPrivKey_t ) + sizeof( CCEcpkiUserPublKey_t )));
    if (NULL == pTempBuff)
    {
        CC_PAL_LOG_ERR("Error - failed to allocate memory for temporary buffer\n");
        return MBEDTLS_ERR_ECP_ALLOC_FAILED;
    }
    pRndContext = (CCRndContext_t *)(pTempBuff + 1);
    pUserPrivKey = (CCEcpkiUserPrivKey_t *)(pRndContext + 1);
    pUserPublKey = (CCEcpkiUserPublKey_t *)(pUserPrivKey + 1);

    pRndContext->rndGenerateVectFunc = (CCRndGenerateVectWorkFunc_t)f_rng;
    pRndContext->rndState = p_rng;

    ret = mbedtls_mpi_grow(&Q->X, CALC_FULL_32BIT_WORDS(pDomain->modSizeInBits));
    if (ret != 0)
    {
        CC_PAL_LOG_ERR("Error - failed to allocate memory for R\n");
        goto END;
    }
    Q->X.s = 1; /*unsigned*/

    ret = mbedtls_mpi_grow(&Q->Y, CALC_FULL_32BIT_WORDS(pDomain->modSizeInBits));
    if (ret != 0)
    {
        CC_PAL_LOG_ERR("Error - failed to allocate memory for R.x\n");
        goto END;
    }
    Q->Y.s = 1; /*unsigned*/

    ret = mbedtls_mpi_grow(d, CALC_FULL_32BIT_WORDS(pDomain->modSizeInBits));
    if (ret != 0)
    {
        CC_PAL_LOG_ERR("Error - failed to allocate memory for R.d\n");
        goto END;
    }

    d->s = 1; /*unsigned*/

    if (G != NULL) /* Base point was supplied by the application*/
    {
        rc = CC_EcpkiKeyPairGenerateBase(pRndContext, pDomain, G->X.p, G->Y.p, pUserPrivKey, pUserPublKey, pTempBuff, NULL);
    }
    else
    {
        rc = CC_EcpkiKeyPairGenerate(pRndContext, pDomain, pUserPrivKey, pUserPublKey, pTempBuff, NULL);
    }
    if (rc != CC_SUCCESS)
    {
        CC_PAL_LOG_ERR("Error - Key generation ended with result: %d\n", rc);
        ret =  error_mapping_cc_to_mbedtls_ecc(rc);
        goto END;
    }

    pPrivKey = (CCEcpkiPrivKey_t *)pUserPrivKey->PrivKeyDbBuff;
    pPublicKey = (CCEcpkiPublKey_t *)pUserPublKey->PublKeyDbBuff;
    CC_PalMemCopy(d->p, pPrivKey->PrivKey, CALC_FULL_BYTES(pDomain->modSizeInBits));
    CC_PalMemCopy(Q->X.p, pPublicKey->x, CALC_FULL_BYTES(pDomain->modSizeInBits));
    CC_PalMemCopy(Q->Y.p, pPublicKey->y, CALC_FULL_BYTES(pDomain->modSizeInBits));

    ret = mbedtls_mpi_lset( &Q->Z, 1 );
    if (ret != 0)
    {
        CC_PAL_LOG_ERR("Error - failed to allocate memory for R\n");
        goto END;
    }
    ret = (0);
    goto SUCCESS;

    END:

        mbedtls_ecp_point_free(Q);
        mbedtls_mpi_free(d);
    SUCCESS:
        mbedtls_zeroize_internal(pTempBuff, (sizeof( CCEcpkiKgTempData_t ) + sizeof( CCRndContext_t ) + sizeof( CCEcpkiUserPrivKey_t ) + sizeof( CCEcpkiUserPublKey_t )));
        mbedtls_free(pTempBuff);
        return ret;
}
#endif /* ECP_SHORTWEIERSTRASS */
/*
 * Generate a keypair with configurable base point
 */
int cc_ecp_gen_keypair_base( mbedtls_ecp_group *grp,
                     const mbedtls_ecp_point *G,
                     mbedtls_mpi *d, mbedtls_ecp_point *Q,
                     int (*f_rng)(void *, unsigned char *, size_t),
                     void *p_rng )
{

    /* Input parameters validation */
    if (NULL == grp || NULL == Q || NULL == d || NULL == G || f_rng == NULL || p_rng == NULL)
    {
        CC_PAL_LOG_ERR("Error - NULL pointer exception\n");
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );

    }
#if defined(ECP_MONTGOMERY)
    if( ecp_get_type( grp ) == ECP_TYPE_25519 )
    {
        return ecp_mont_gen_keypair_base(G, d, Q, f_rng, p_rng);
    }
#endif /* ECP_MONTGOMERY */

#if defined(ECP_SHORTWEIERSTRASS)
    if( ecp_get_type( grp ) == ECP_TYPE_SHORT_WEIERSTRASS )
    {
        return ecp_wrst_gen_keypair_base(grp, G, d, Q, f_rng, p_rng);
    }
#endif /* ECP_SHORTWEIERSTRASS */
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
}
#endif /*(MBEDTLS_ECP_C)*/
