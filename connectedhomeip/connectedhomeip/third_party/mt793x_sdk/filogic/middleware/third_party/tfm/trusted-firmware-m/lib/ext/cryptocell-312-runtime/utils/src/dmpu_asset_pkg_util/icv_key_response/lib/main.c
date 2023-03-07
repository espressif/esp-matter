/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <openssl/objects.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/bn.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include "common_util_log.h"
#include "common_crypto_asym.h"
#include "common_crypto_sym.h"
#include "common_rsa_keypair_util.h"
#include "common_util_files.h"
#include "cc_production_asset.h"
#include "dmpu_utils.h"

static int verifyCert(CCOemRequestCert_t * pCertStruct)
{
        int rc = 0;
        unsigned int csrBuffSize = 0;

        if (pCertStruct == NULL){
                return 1;
        }

        /* verify cert header */
        if (pCertStruct->token != DMPU_OEM_KEY_REQ_TOKEN) {
                UTIL_LOG_ERR("Ilegal token in cert 0x%x\n", pCertStruct->token);
                return 1;
        }
        if (pCertStruct->version != DMPU_OEM_KEY_REQ_VERSION) {
                UTIL_LOG_ERR("Ilegal version in cert 0x%x\n", pCertStruct->version);
                return 1;
        }
        if (pCertStruct->size != sizeof(CCOemRequestCert_t) - SB_CERT_RSA_KEY_SIZE_IN_BYTES) {
                UTIL_LOG_ERR("Ilegal length in cert 0x%x\n", pCertStruct->size);
                return 1;
        }

        /* verify certificate signatire */
        rc = CC_CommonRsaVerify(RSA_USE_PKCS_21_VERSION,                    /* RSA version */
                                  pCertStruct->oemMainPubKey,                            /* public key to verify with */
                                  (char *)pCertStruct,                           /* data */
                                  pCertStruct->size,   /* data size */
                                  pCertStruct->signature);                            /* signature */
        if (rc != 0) {
                UTIL_LOG_ERR( "failed to CC_CommonRsaVerify() for certificate, rc %d\n", rc);
                return 1;
        }
        UTIL_LOG_ERR("verifyCert: OK\n");
        return 0;
}


/**
* @brief Build the ICV key response package using openSSL
*        library
*
* @param[in]  encKeyBuff & encKeyBuffSize & pKeyPwdFileName - the encryptes Kpicv key
*           assetId - Asset ID, used for Kprov derivation
*           asset & assetSize - The Asset to generate the package for
*               pPkgFileName - OUT - the asset package binary
*               file name
* @param[out] None
*
*/
/*********************************************************/
int generateIcvKeyRespPkg(char *encKeyBuff, uint32_t encKeyBuffSize,
    char *pKeyPwdFileName,
    uint8_t *pCertbuff, uint32_t certbuffSize,
    char *pPkgFileName)
{
    int rc = 0;
    uint8_t i = 0;
        CCOemRequestCert_t  *pCertStruct;
    uint8_t keyRtl[PROD_KEY_RTL_KEY_SIZE + AES_BLOCK_SIZE] = {0};  // Adding AES_BLOCK_SIZE to avoid over-run
        uint8_t hbkBuff[HASH_SHA256_DIGEST_SIZE_IN_BYTES];
        uint8_t keyTmp[PROD_KEY_TMP_KEY_SIZE] = {0};
        uint8_t     encKeyTmp[SB_CERT_RSA_KEY_SIZE_IN_BYTES] = {0};

    // Verify Inputs
    if ( (pPkgFileName == NULL) ||
             (encKeyBuff == NULL) ||
             (encKeyBuffSize != (PROD_KEY_RTL_KEY_SIZE+ AES_BLOCK_SIZE)) ||
             (pCertbuff == NULL) ||
             (certbuffSize !=  sizeof (CCOemRequestCert_t))) {
        UTIL_LOG_ERR( "Invalid inputs\n");
        return 1;
    }
        pCertStruct = (CCOemRequestCert_t  *)pCertbuff;

    InitOpenSsl();

        // First, verify the certificate
        rc = verifyCert(pCertStruct);
    if (rc != 0) {
        UTIL_LOG_ERR( "failed to verifyCert() for Krtl, rc %d\n", rc);
        rc = 1;
        goto end_func;
    }

    // Decrypt Krtl
    rc = CC_CommonAesCbcDecrypt(pKeyPwdFileName, encKeyBuff, encKeyBuffSize, keyRtl);
    if (rc != 0) {
        UTIL_LOG_ERR( "failed to CC_CommonAesCbcDecrypt() for Krtl, rc %d\n", rc);
        rc = 1;
        goto end_func;
    }

        // Calculate HBK from oem main public key hash
        rc = CC_CommonCalcHash((uint8_t *)&pCertStruct->oemMainPubKey,
                               SB_CERT_RSA_KEY_SIZE_IN_BYTES+NP_SIZE_IN_BYTES,
                               hbkBuff,
                               HASH_SHA256_DIGEST_SIZE_IN_BYTES);
    if (rc != 0) {
        UTIL_LOG_ERR( "failed to CC_CommonCalcHash() for Krtl, rc %d\n", rc);
        rc = 1;
        goto end_func;
    }

        // Calculate Ktmp = cmac(Krtl, 0x01 || OEM label  || 0x0 || HBK(only 16 bytes) || 0x80)
    rc = AesCmacKeyDerivation(keyRtl, PROD_KEY_RTL_KEY_SIZE,
                PROD_OEM_KEY_TMP_LABEL,  PROD_KEY_TMP_LABEL_SIZE,
                (uint8_t *)hbkBuff, PROD_KEY_TMP_CONTEXT_SIZE,
                keyTmp, PROD_KEY_TMP_KEY_SIZE);
    if (rc != 0) {
        UTIL_LOG_ERR( "failed to AesCmacKeyDerivation() for Ktmp, rc %d\n", rc);
        rc = 1;
        goto end_func;
    }

        // Encrypt the OEM key with the dedicated key pair
        rc = CC_CommonRsaEncrypt(RSA_USE_PKCS_21_VERSION,
                            (uint8_t *)&pCertStruct->oemEncPubKey,
              keyTmp, PROD_KEY_TMP_KEY_SIZE,
              encKeyTmp) ;
    if (rc != 0) {
        UTIL_LOG_ERR( "failed to CC_CommonRsaEncrypt() for Ktmp, rc %d\n", rc);
        rc = 1;
        goto end_func;
    }


    // Writie the encrypted key  into bin file
    rc = CC_CommonUtilCopyBuffToBinFile(pPkgFileName, encKeyTmp, SB_CERT_RSA_KEY_SIZE_IN_BYTES);
    if (rc != 0) {
        UTIL_LOG_ERR( "failed to CC_CommonUtilCopyBuffToBinFile(), rc %d\n", rc);
        rc = 1;
        goto end_func;
    }

end_func:
    CloseOpenSsl();
    UTIL_LOG_ERR( "End rc %d\n", rc);
    return rc;
}


