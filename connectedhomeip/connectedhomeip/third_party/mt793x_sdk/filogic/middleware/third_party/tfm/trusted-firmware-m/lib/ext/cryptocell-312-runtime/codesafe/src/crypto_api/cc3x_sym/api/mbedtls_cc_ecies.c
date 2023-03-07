/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/************* Include Files ****************/
#include "mbedtls_cc_ecies.h"

#include "cc_pal_mem.h"
#include "cc_pal_log.h"
#include "cc_pal_types.h"

#include "cc_ecpki_build.h"
#include "cc_ecpki_error.h"
#include "cc_ecpki_domain.h"
#include "cc_rnd_common.h"
#include "cc_ecpki_kg.h"
#include "ecp_common.h"
#include "ec_wrst.h"
#include "cc_kdf.h"
#include "mbedtls_cc_hkdf.h"
#include "mbedtls_common.h"

#include "mbedtls/platform.h"

/************************ Defines *************************************/

/************************ Enums ***************************************/

/************************ Typedefs ************************************/

/************************ Global Data *********************************/

/************* Private function prototype *****************************/
static CCKdfHashOpMode_t ecies_convert_mbed_to_cc_hkdf_hash_mode(mbedtls_hkdf_hashmode_t mode)
{
    switch (mode)
    {
        case CC_HKDF_HASH_SHA1_mode:
            return CC_KDF_HASH_SHA1_mode;
        case CC_HKDF_HASH_SHA224_mode:
            return CC_KDF_HASH_SHA224_mode;
        case CC_HKDF_HASH_SHA256_mode:
            return CC_KDF_HASH_SHA256_mode;
        case CC_HKDF_HASH_SHA384_mode:
            return CC_KDF_HASH_SHA384_mode;
        case CC_HKDF_HASH_SHA512_mode:
            return CC_KDF_HASH_SHA512_mode;
        default:
            return CC_KDF_HASH_OpModeLast;
    }
}

static int ecies_convert_mbed_to_cc_public_key(const mbedtls_ecp_group *pGrp, /*in*/
                                               const CCEcpkiDomain_t *pDomain,
                                               mbedtls_ecp_point *pPubKey, /*in*/
                                               CCEcpkiUserPublKey_t *pUserPublKey, /*out*/
                                               CCEciesTempData_t *tempBuff /*in*/)

{
    int error = 0;
    size_t keylen = 0;
    uint8_t *pBuf = NULL;
    CCEcpkiBuildTempData_t *pTempBuildBuff = NULL;
    size_t bufSize = 0;

    if (tempBuff == NULL)
    {
        CC_PAL_LOG_ERR("Error - tempBuff is NULL\n");
        error = CC_ECIES_INVALID_PTR;
        goto EXIT_ON_ERROR;
    }

    pTempBuildBuff = &tempBuff->tmp.buildTempbuff;
    pBuf = (uint8_t*)&tempBuff->ConvPublKey;
    bufSize = sizeof(tempBuff->ConvPublKey);

    /* write mpi to buf */
    if ((error = mbedtls_ecp_point_write_binary(pGrp,
                                                pPubKey,
                                                MBEDTLS_ECP_PF_UNCOMPRESSED,
                                                &keylen,
                                                pBuf,
                                                bufSize)) != 0)
    {
        CC_PAL_LOG_ERR("Error - failed to write mpi\n");
        goto EXIT_ON_ERROR;
    }

    if ((error = CC_EcpkiPublKeyBuildAndCheck(pDomain,
                                              pBuf,
                                              keylen,
                                              ECpublKeyFullCheck,
                                              pUserPublKey,
                                              pTempBuildBuff)) != CC_OK)
    {
        CC_PAL_LOG_ERR("Error - failed CC_EcpkiPublKeyBuildAndCheck\n");
        goto EXIT_ON_ERROR;
    }

EXIT_ON_ERROR:
    return error;

}

static int ecies_convert_mbed_to_cc_private_key(const CCEcpkiDomain_t *pDomain,
                                                mbedtls_mpi *pPrivKey, /*in*/
                                                CCEcpkiUserPrivKey_t *pUserPrivKey /*out*/,
                                                CCEciesTempData_t *tempBuff)
{
    int error = 0;
    size_t n = 0;
    CCEcpkiBuildTempData_t *pTempBuildBuff = NULL;
    if (tempBuff == NULL)
    {
        CC_PAL_LOG_ERR("Error - tempBuff is NULL\n");
        error = CC_ECIES_INVALID_PTR;
        goto EXIT_ON_ERROR;
    }

    pTempBuildBuff = &tempBuff->tmp.buildTempbuff;
    n = mbedtls_mpi_size( pPrivKey );

    if (n > sizeof(*pTempBuildBuff))
    {
        CC_PAL_LOG_ERR("Error - n[%u] latger pTempBuildBuff struct[%u]\n", n, sizeof(*pTempBuildBuff));
        error = CC_ECIES_INVALID_TEMP_DATA_SIZE_ERROR;
        goto EXIT_ON_ERROR;
    }
    /* write mpi to buf */
    if ((error = mbedtls_mpi_write_binary(pPrivKey, (uint8_t*)pTempBuildBuff, n)) != 0)
    {
        CC_PAL_LOG_ERR("Error - failed to write mpi\n");
        goto EXIT_ON_ERROR;
    }

    if ((error = CC_EcpkiPrivKeyBuild(pDomain, (uint8_t*)pTempBuildBuff, n, pUserPrivKey)) != CC_OK)
    {
        CC_PAL_LOG_ERR("Error - failed to CC_EcpkiPrivKeyBuild n[%u]\n", n);
        goto EXIT_ON_ERROR;
    }

EXIT_ON_ERROR:
    return error;

}

static void ecies_free_keys(CCEcpkiUserPrivKey_t *a, CCEcpkiUserPublKey_t *b)
{
    if (a != NULL)
    {
        CC_PalMemSetZero(a, sizeof(*a));
        mbedtls_free(a);
    }

    if (b != NULL)
    {
        CC_PalMemSetZero(b, sizeof(*b));
        mbedtls_free(b);
    }
}

/************************ Public Functions ****************************/
CCError_t mbedtls_ecies_kem_encrypt_full(mbedtls_ecp_group *pGrp,
                                         mbedtls_ecp_point *pRecipUzPublKey,
                                         CCKdfDerivFuncMode_t kdfDerivMode,
                                         mbedtls_hkdf_hashmode_t kdfHashMode,
                                         uint32_t isSingleHashMode,
                                         mbedtls_ecp_point *pExtEphUzPublicKey,
                                         mbedtls_mpi *pExtEphUzPrivateKey,
                                         uint8_t *pSecrKey,
                                         size_t secrKeySize,
                                         uint8_t *pCipherData,
                                         size_t *pCipherDataSize,
                                         void *pBuff,
                                         size_t buffLen,
                                         int (*f_rng)(void *, unsigned char *, size_t),
                                         void *p_rng)
{

    /* LOCAL DECLARATIONS */
    CCError_t error = CC_OK;

    /*  pointer to EC domain  */
    const CCEcpkiDomain_t *pDomain;
    CCEcpkiDomainID_t domain_id;

    uint32_t modSizeInBytes, kdfDataSize;
    uint8_t *pKdfData;


    /* pointers to ephemeral key pair, which should be used */
    CCEcpkiUserPublKey_t *pCcReciptUzPublKey = NULL;
    CCEcpkiUserPrivKey_t *pEphUzPrivKey = NULL;
    CCEcpkiUserPublKey_t *pEphUzPublKey = NULL;
    CCEcpkiPublKey_t *pRecipPublKey = NULL;
    CCEcpkiPrivKey_t *pEphPrivKey = NULL;

    CCEciesTempData_t *pTempBuff = NULL;
    CCRndContext_t *pRndContext = NULL;

    /* Initialize Error */
    error = CC_OK;

    /* .........    checking the validity of input parameters  .......... */
    /* Note: pRndContext and pFipsCtx will be checked in called functions */
    /* ------------------------------------------------------------------- */

    /* check the validity of the user static private key */
    if (pRecipUzPublKey == NULL)
        return CC_ECIES_INVALID_PUBL_KEY_PTR_ERROR;

    /* check KDF and HASH modes */
    if (kdfDerivMode != CC_KDF_ISO18033_KDF1_DerivMode && kdfDerivMode != CC_KDF_ISO18033_KDF2_DerivMode)
        return CC_ECIES_INVALID_KDF_DERIV_MODE_ERROR;

    /* check HASH mode */
    if (kdfHashMode >= CC_HKDF_HASH_NumOfModes)
        return CC_ECIES_INVALID_KDF_HASH_MODE_ERROR;

    /* check the pointer to the buffer for secret key output */
    if (pSecrKey == NULL)
        return CC_ECIES_INVALID_SECRET_KEY_PTR_ERROR;

    /* check the size of secret key to be generated */
    if (secrKeySize == 0)
        return CC_ECIES_INVALID_SECRET_KEY_SIZE_ERROR;

    /* checking the buffer for cipher text output */
    if (pCipherData == NULL)
        return CC_ECIES_INVALID_CIPHER_DATA_PTR_ERROR;

    if (pCipherDataSize == NULL)
        return CC_ECIES_INVALID_CIPHER_DATA_SIZE_PTR_ERROR;

    if ((pExtEphUzPublicKey != NULL) ^ (pExtEphUzPrivateKey != NULL))
        return CC_ECIES_INVALID_CIPHER_DATA_SIZE_PTR_ERROR;

    /* checking the temp buffer pointer  */
    if (pBuff == NULL)
        return CC_ECIES_INVALID_TEMP_DATA_PTR_ERROR;

    if (buffLen < sizeof(CCEciesTempData_t))
        return CC_ECIES_INVALID_TEMP_DATA_PTR_ERROR;


    /* ..  initializtions  and other checking   .... */
    /* --------------------------------------------- */

    /* convert grp_id to domain ID */
    if ((error = ecp_grp_id_to_domain_id(pGrp->id, &domain_id)) != 0)
    {
        CC_PAL_LOG_ERR("Error - failed to ecp_grp_id_to_domain_id grpId[%u]. error[0x%08x]\n", pGrp->id, error);
        error = CC_ECPKI_INVALID_DOMAIN_ID_ERROR;
        goto End;
    }

    /* check EC Domain ID */
    if (domain_id >= CC_ECPKI_DomainID_OffMode)
    {
        CC_PAL_LOG_ERR("Error - domain_id[%u] larger than CC_ECPKI_DomainID_OffMode[%u]\n", domain_id, CC_ECPKI_DomainID_OffMode);
        error = CC_ECPKI_INVALID_DOMAIN_ID_ERROR;
        goto End;
    }

    /* get domain from id */
    pDomain = CC_EcpkiGetEcDomain(domain_id);
    if (pDomain == NULL)
    {
        CC_PAL_LOG_ERR("Error - failed to get pDomain by id[%u]\n", domain_id);
        error = CC_ECPKI_INVALID_DOMAIN_ID_ERROR;
        goto End;
    }

    /* convert mbedtls keys to cc structs */
    pTempBuff = (CCEciesTempData_t *)pBuff;

    pCcReciptUzPublKey = mbedtls_calloc(1, sizeof(*pCcReciptUzPublKey));
    if (pCcReciptUzPublKey == NULL)
    {
        CC_PAL_LOG_ERR("Error - pCcReciptUzPublKey failed to allocate\n");
        error = CC_ECIES_INVALID_PTR;
        goto End;
    }

    if ((error = ecies_convert_mbed_to_cc_public_key(pGrp, pDomain, pRecipUzPublKey, pCcReciptUzPublKey, pTempBuff)) != 0)
    {
        CC_PAL_LOG_ERR("Error - failed to ecp_convert_mbed_to_cc_public_key receipt public key. error[0x%08x]\n", error);
        goto End;
    }

     /* derive and check domainID from recipient Public Key */
    pRecipPublKey = (CCEcpkiPublKey_t*) &pCcReciptUzPublKey->PublKeyDbBuff;

    /* modulus size */
    modSizeInBytes = CALC_FULL_BYTES(pDomain->modSizeInBits);

    /* check cipher output buffer size */
    if (*pCipherDataSize < 2 * modSizeInBytes + 1)
    {
        CC_PAL_LOG_ERR("Error - size mismatch dataSize[%u] keyDataSize[%u]\n", *pCipherDataSize, 2 * modSizeInBytes + 1);
        error = CC_ECIES_INVALID_CIPHER_DATA_SIZE_ERROR;
        goto End;
    }

    if (pExtEphUzPublicKey == NULL && pExtEphUzPrivateKey == NULL)
    {
        pRndContext = mbedtls_calloc(1, sizeof(CCRndContext_t));
        if (pRndContext == NULL)
        {
            CC_PAL_LOG_ERR("Error - failed to allocate CCRndContext_t\n");
            error = CC_ECIES_INVALID_PTR;
            goto End;
        }

        if (f_rng == NULL)
        {
            CC_PAL_LOG_ERR("Error - f_rng is NULL\n");
            error = CC_ECIES_INVALID_PTR;
            goto End;
        }

        pRndContext->rndGenerateVectFunc = (CCRndGenerateVectWorkFunc_t)f_rng;
        pRndContext->rndState = p_rng;

        /* use internal genrated ephemeral Key Pair */
        error = CC_EcpkiKeyPairGenerate(pRndContext, /*in/out*/
                                        pDomain,
                                        &pTempBuff->PrivKey,
                                        &pTempBuff->PublKey,
                                        &pTempBuff->tmp.KgTempBuff,
                                        NULL);

        CC_PalMemSetZero(pRndContext, sizeof(*pRndContext));
        mbedtls_free(pRndContext);

        if (error != CC_OK)
            goto End;

        pEphUzPrivKey = &pTempBuff->PrivKey;    // r
        pEphUzPublKey = &pTempBuff->PublKey;    // g~ = r*G
    }
    else
    {
        pEphUzPrivKey = &pTempBuff->PrivKey;
        pEphUzPublKey = &pTempBuff->PublKey;

        /* use external ephemeral Key Pair */
        if ((error = ecies_convert_mbed_to_cc_private_key(pDomain, pExtEphUzPrivateKey, pEphUzPrivKey, pTempBuff)) != 0)
        {
            CC_PAL_LOG_ERR("Error - failed to ecies_convert_mbed_to_cc_private_key. error[0x%08x]\n", error);
            ecies_free_keys(pEphUzPrivKey, pEphUzPublKey);
            goto End;
        }

        if ((error = ecies_convert_mbed_to_cc_public_key(pGrp, pDomain, pExtEphUzPublicKey, pEphUzPublKey, pTempBuff)) != 0)
        {
            CC_PAL_LOG_ERR("Error - failed to ecies_convert_mbed_to_cc_public_key. error[0x%08x]\n", error);
            ecies_free_keys(pEphUzPrivKey, pEphUzPublKey);
            goto End;
        }
    }

    /* convert ephemeral public key to standard form */
    error = CC_EcpkiPubKeyExport(pEphUzPublKey, /*in*/
                                 CC_EC_PointUncompressed, /*in*/
                                 (uint8_t*) &pTempBuff->zz, /*out*/
                                 pCipherDataSize); /*in/out*//* Number of Bytes that were copied to zz*/
    if (error != CC_OK)
    {
        CC_PAL_LOG_ERR("Error - failed to CC_EcpkiPubKeyExport. error[0x%08x]\n", error);
        goto End;
    }

    /* call  EcWrstDhDeriveSharedSecret function to calculate the Secret Value */
    /* --------------------------------------------------------------- */        // h~ = r*h
    pEphPrivKey = (CCEcpkiPrivKey_t*) &pEphUzPrivKey->PrivKeyDbBuff;
    error = EcWrstDhDeriveSharedSecret(pRecipPublKey, /*in*/
                                       (CCEcpkiPrivKey_t *) (pEphPrivKey->PrivKey), /*in*/
                                       &((uint8_t*) &pTempBuff->zz)[*pCipherDataSize], /*out*//* Next available space in zz where the (SV X coordinate of multiplication) will be stored*/
                                       &pTempBuff->tmp.DhTempBuff); /*in*/
    if (error != CC_OK)
    {
        CC_PAL_LOG_ERR("Error - failed to EcWrstDhDeriveSharedSecret. error[0x%08x]\n", error);
        goto End;
    }

    /* derive the Keying Data from the Secret Value by calling the KDF function    */
    /* --------------------------------------------------------------------------- */

    /* set pointer  and size of input data for KDF function */
    if (isSingleHashMode == CC_TRUE)
    { /* z = X */
        pKdfData = &((uint8_t*) &pTempBuff->zz)[*pCipherDataSize];
        kdfDataSize = modSizeInBytes;
    }
    else
    { /*z = C0 || X*/
        pKdfData = (uint8_t*) &pTempBuff->zz;
        kdfDataSize = *pCipherDataSize + modSizeInBytes;
    }

    /*derive the Keying Data */
    error = CC_KdfKeyDerivFunc(pKdfData/*ZZSecret*/,
                               kdfDataSize/*ZZSecretSize*/,
                               NULL/*&otherInfo*/,
                               ecies_convert_mbed_to_cc_hkdf_hash_mode(kdfHashMode),
                               kdfDerivMode,
                               pSecrKey,
                               secrKeySize);
    if (error != CC_OK)
    {
        CC_PAL_LOG_ERR("Error - failed to CC_KdfKeyDerivFunc. error[0x%08x]\n", error);
        goto End;
    }

    /* Output of the Cipher Data (C0) = Ephemeral Public Key */
    CC_PalMemCopy(pCipherData, pTempBuff->zz, *pCipherDataSize);

End:

    if (error != CC_OK)
    {
        *pCipherDataSize = 0;
    }

    ecies_free_keys(NULL, pCcReciptUzPublKey);

    /* clean temp buffers */
    CC_PalMemSetZero(pBuff, buffLen);

    return error;

}/* END OF CC_EciesKemEncrypt */

CCError_t mbedtls_ecies_kem_decrypt(mbedtls_ecp_group *pGrp,
                                    mbedtls_mpi *pRecipUzPrivKey,
                                    CCKdfDerivFuncMode_t kdfDerivMode,
                                    mbedtls_hkdf_hashmode_t kdfHashMode,
                                    uint32_t isSingleHashMode,
                                    uint8_t *pCipherData,
                                    size_t cipherDataSize,
                                    uint8_t *pSecrKey,
                                    size_t secrKeySize,
                                    void *pBuff,
                                    size_t buffLen)
{

    /* LOCAL DECLARATIONS */

    CCError_t error = CC_OK;

    /*  pointer to EC domain  */
    const CCEcpkiDomain_t *pDomain;
    CCEcpkiDomainID_t domain_id;

    uint32_t modSizeInBytes, kdfDataSize;
    uint8_t *pKdfData;

    CCEcpkiPrivKey_t *pRecipPrivKey = NULL;
    CCEcpkiUserPrivKey_t *pRecipUserPrivKey;

    CCEciesTempData_t *pTempBuff = NULL;

    /* ................. checking the validity of input parameters  .......... */
    /* ----------------------------------------------------------------------- */

    /* check the validity of the recipient's private key */
    if (pRecipUzPrivKey == NULL)
        return CC_ECIES_INVALID_PRIV_KEY_PTR_ERROR;

    /*check KDF and HASH modes */
    if (kdfDerivMode != CC_KDF_ISO18033_KDF1_DerivMode && kdfDerivMode != CC_KDF_ISO18033_KDF2_DerivMode)
        return CC_ECIES_INVALID_KDF_DERIV_MODE_ERROR;

    if (kdfHashMode >= CC_HKDF_HASH_NumOfModes) {
        return CC_ECIES_INVALID_KDF_HASH_MODE_ERROR;
    }

    /* check the pointer to the buffer for secret key output */
    if (pSecrKey == NULL)
        return CC_ECIES_INVALID_SECRET_KEY_PTR_ERROR;

    /* checking the size of secret key to be generated */
    if (secrKeySize == 0)
        return CC_ECIES_INVALID_SECRET_KEY_SIZE_ERROR;

    /* check the buffer for cipher text output */
    if (pCipherData == NULL)
        return CC_ECIES_INVALID_CIPHER_DATA_PTR_ERROR;

    /* checking the temp buffer pointer  */
    if (pBuff == NULL)
        return CC_ECIES_INVALID_TEMP_DATA_PTR_ERROR;

    /* check the buffer is bug enough */
    if (buffLen < MBEDTLS_ECIES_MIN_BUFF_LEN_BYTES)
    {
        CC_PAL_LOG_ERR("Error - buffLen[%u] < MBEDTLS_ECIES_MIN_BUFF_LEN_BYTES[%u]\n", buffLen, MBEDTLS_ECIES_MIN_BUFF_LEN_BYTES);
        return CC_ECIES_INVALID_TEMP_DATA_PTR_ERROR;
    }

    /* ..  initializations  and other checking   .... */
    /* ---------------------------------------------- */

    /* convert grp_id to domain ID */
    if ((error = ecp_grp_id_to_domain_id(pGrp->id, &domain_id)) != 0)
    {
        CC_PAL_LOG_ERR("Error - failed to ecp_grp_id_to_domain_id grpId[%u]. error[0x%08x]\n", pGrp->id, error);
        error = CC_ECPKI_INVALID_DOMAIN_ID_ERROR;
        goto End;
    }

    /* check EC Domain ID */
    if (domain_id >= CC_ECPKI_DomainID_OffMode)
    {
        CC_PAL_LOG_ERR("Error - domain_id[%u] larger than CC_ECPKI_DomainID_OffMode[%u]\n", domain_id, CC_ECPKI_DomainID_OffMode);
        error = CC_ECPKI_INVALID_DOMAIN_ID_ERROR;
        goto End;
    }

    /* get domain from id */
    pDomain = CC_EcpkiGetEcDomain(domain_id);
    if (pDomain == NULL)
    {
        CC_PAL_LOG_ERR("Error - failed to get pDomain by id[%u]\n", domain_id);
        error = CC_ECPKI_INVALID_DOMAIN_ID_ERROR;
        goto End;
    }

    /* convert mbedtls keys to cc structs */
    pTempBuff = (CCEciesTempData_t *)pBuff;
    pRecipUserPrivKey = &pTempBuff->PrivKey;

    if ((error = ecies_convert_mbed_to_cc_private_key(pDomain, pRecipUzPrivKey, pRecipUserPrivKey, pTempBuff)) != 0)
    {
        CC_PAL_LOG_ERR("Error - failed to ecp_convert_mbed_to_cc_public_key reciept public key. error[0x%08x]\n", error);
        goto End;
    }

    /* derive domainID from recipient Private Key */
    pRecipPrivKey = (CCEcpkiPrivKey_t*) &(pRecipUserPrivKey->PrivKeyDbBuff);

    /* modulus size */
    modSizeInBytes = CALC_FULL_BYTES(pDomain->modSizeInBits);

    /* partially check the cipher data C0 (ephemeral public key)
     and initialize appropriate CC Key structure */
    error = CC_EcpkiPublKeyBuildAndCheck(pDomain, /*in*/
                                         pCipherData, /*in - ephem. publ.key data*/
                                         cipherDataSize, /*in*/
                                         ECpublKeyPartlyCheck, /*in*/
                                         &pTempBuff->PublKey, /*out*/
                                         &pTempBuff->tmp.buildTempbuff); /*in*/
    if (error != CC_OK)
        goto End;

    /* call  EcWrstDhDeriveSharedSecret function to calculate the Secret Value SV */
    /* ----------------------------------------------------------------- */
    error = EcWrstDhDeriveSharedSecret((CCEcpkiPublKey_t*) &pTempBuff->PublKey.PublKeyDbBuff, /*in*/
                                       (CCEcpkiPrivKey_t *) pRecipPrivKey->PrivKey, /*in*/
                                       &((uint8_t*) &pTempBuff->zz)[cipherDataSize], /*out*/
                                       &pTempBuff->tmp.DhTempBuff); /*in*/

    if (error != CC_OK)
        goto End;

    /* set pointer  and size of input data for KDF function */
    if (isSingleHashMode == CC_TRUE)
    { /* zz = SV */
        pKdfData = &((uint8_t*) &pTempBuff->zz)[cipherDataSize];
        kdfDataSize = modSizeInBytes;
    }
    else
    { /* zz = C0 || SV */
        pKdfData = (uint8_t*) &pTempBuff->zz;
        kdfDataSize = cipherDataSize + modSizeInBytes;
        /* set cipherData  C0 in the temp buffer zz */
        CC_PalMemCopy((uint8_t* )&pTempBuff->zz, pCipherData, cipherDataSize);
    }

    /* derive the Keying Data from the ZZ Value by calling the KDF function */
    /* -----------------------------------------------------------------  - */
    error = CC_KdfKeyDerivFunc(pKdfData/*ZZSecret*/,
                               kdfDataSize/*ZZSecretSize*/,
                               NULL/*&otherInfo*/,
                               ecies_convert_mbed_to_cc_hkdf_hash_mode(kdfHashMode),
                               kdfDerivMode,
                               pSecrKey,
                               secrKeySize);
End:
    /* clean temp buffers */
    CC_PalMemSetZero(pBuff, buffLen);

    return error;

}
