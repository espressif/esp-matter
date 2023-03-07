/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "secureboot_basetypes.h"
#include "secureboot_general_hwdefs.h"
#include "bsv_defs.h"
#include "bsv_error.h"
#include "cc_pal_log.h"
#include "cc_otp_defs.h"
#include "bootimagesverifier_def.h"
#include "secureboot_stage_defs.h"
#include "secureboot_base_func.h"
#include "secureboot_base_swimgverify.h"
#include "bootimagesverifier_swcomp.h"
#include "bootimagesverifier_error.h"
#include "bootimagesverifier_parser.h"
#include "common_cert_parser.h"
#include "cc_crypto_boot_defs.h"
#include "nvm.h"
#include "secdebug_api.h"
#include "secdebug_defs.h"

const uint32_t certMagicNumber[CC_SB_MAX_CERT] = {
        /* No enum */           0,
        /*  CC_SB_KEY_CERT       */ CC_SB_KEY_CERT_MAGIC_NUMBER,
        /*  CC_SB_CONTENT_CERT   */ CC_SB_CONTENT_CERT_MAGIC_NUMBER,
        0,
        /*  CC_SB_ENABLER_CERT   */ CC_CERT_SEC_DEBUG_ENABLER_MAGIC,
        /*  CC_SB_DEVELOPER_CERT */ CC_CERT_SEC_DEBUG_DEVELOPER_MAGIC
};

const uint32_t certMainMaxSize[CC_SB_MAX_CERT] = {
        /* No enum */                 0,
        /*  CC_SB_KEY_CERT       */(CC_SB_MAX_KEY_CERT_SIZE_IN_BYTES - CC_SB_MAX_CERT_SIGN_SIZE_IN_BYTES),
        /*  CC_SB_CONTENT_CERT   */(CC_SB_MAX_CONTENT_CERT_SIZE_IN_BYTES - CC_SB_MAX_CERT_SIGN_SIZE_IN_BYTES),
        0,
        /*  CC_SB_ENABLER_CERT   */(CC_SB_MAX_ENABLER_CERT_SIZE_IN_BYTES - CC_SB_MAX_CERT_SIGN_SIZE_IN_BYTES),
        /*  CC_SB_DEVELOPER_CERT */(CC_SB_MAX_DEVELOPER_CERT_SIZE_IN_BYTES - CC_SB_MAX_CERT_SIGN_SIZE_IN_BYTES)
};


uint32_t hbkId2HashSizeWords[] = {
        /* CC_SB_HASH_BOOT_KEY_0_128B */  CC_BSV_128B_HASH_SIZE_IN_WORDS,
        /* CC_SB_HASH_BOOT_KEY_1_128B */  CC_BSV_128B_HASH_SIZE_IN_WORDS,
        /* CC_SB_HASH_BOOT_KEY_256B   */ CC_BSV_256B_HASH_SIZE_IN_WORDS,
};




static CCError_t verifyCertPubKeyAndSign(unsigned long hwBaseAddress,
                                         uint32_t    *pCert,
                                         size_t      certSize,
                                         CCSbCertInfo_t  *pSbCertInfo,
                                         workspaceInt_t  *pWorkspaceInt)
{
        CCError_t rc = CC_OK;
        uint32_t  expPubKeyHashSizeWords;
        CCHashResult_t    expPubKeyHash;

        if (pSbCertInfo->initDataFlag != CC_SB_FIRST_CERT_IN_CHAIN) { // not first certificate in chain
                expPubKeyHashSizeWords = sizeof(CCHashResult_t) / CC_32BIT_WORD_SIZE;
                UTIL_MemCopy((uint8_t *)expPubKeyHash, (uint8_t *)pSbCertInfo->pubKeyHash, expPubKeyHashSizeWords * CC_32BIT_WORD_SIZE);
        } else { // meaning this is first certificate in chain
                expPubKeyHashSizeWords = hbkId2HashSizeWords[pSbCertInfo->keyIndex];
                rc = NVM_ReadHASHPubKey(hwBaseAddress,
                                        pSbCertInfo->keyIndex,
                                        expPubKeyHash,
                                        expPubKeyHashSizeWords);
                if (rc != CC_OK) {
                        // if HBK not programed yet, skip HBK verify, but continue verifying the certificate
                        if (rc == CC_BOOT_IMG_VERIFIER_SKIP_PUBLIC_KEY_VERIFY) {
                                expPubKeyHashSizeWords = 0;
                        } else {
                                CC_PAL_LOG_ERR("Failed NVM_ReadHASHPubKey 0x%x", rc);
                                return rc;
                        }
                }
        }

        /* Verify  public key hash only if expectedSize > 0 */
        if (expPubKeyHashSizeWords > 0) {
                rc = CCSbCalcPublicKeyHASHAndCompare(hwBaseAddress,
                                                     (uint32_t *)(&pWorkspaceInt->pubKey.N[0]),
                                                     expPubKeyHash,
                                                     expPubKeyHashSizeWords * CC_32BIT_WORD_SIZE);
                if (rc != CC_OK) {
                        CC_PAL_LOG_ERR("CCSbCalcPublicKeyHASHAndCompare failed 0x%x\n", rc);
                        return rc;
                }
        }

        /* Verify the certificate signature */
        rc = CCSbVerifySignature(hwBaseAddress,
                                 pCert,
                                 (CCSbNParams_t *)(pWorkspaceInt->pubKey.N),
                                 (CCSbSignature_t *)&pWorkspaceInt->signature,
                                 certSize,
                                 RSA_PSS_3072);
        if (rc != CC_OK) {
                CC_PAL_LOG_ERR("CCSbVerifySignature failed 0x%x\n", rc);
                return rc;
        }

        return CC_OK;

}

/* The function validates the certificate header - Magic number , type and version. */
static CCError_t CCCertValidateHeader(CCSbCertHeader_t *pCertHeader,
                                      CCSbCertTypes_t *pCertType)
{
        uint32_t expVersion;
        CCSbCertTypes_t certType = CC_SB_MAX_CERT;
        uint32_t i = 0;

        /* Verify Magic number, and get certificate type out of it */
        /*---------------------*/
        if (pCertHeader->magicNumber == 0) {
                CC_PAL_LOG_ERR("certificate magic number is incorrect \n");
                return CC_BOOT_IMG_VERIFIER_INCORRECT_CERT_TYPE;
        }

        for (i = CC_SB_MIN_CERT + 1; i < CC_SB_MAX_CERT; i++) {
                if (pCertHeader->magicNumber == certMagicNumber[i]) {
                        certType = (CCSbCertTypes_t)i;
                        break;
                }
        }

        if ((certType & *pCertType) == 0) {
                CC_PAL_LOG_ERR("certificate type is incorrect %d exp 0x%x\n", certType, *pCertType);
                return CC_BOOT_IMG_VERIFIER_INCORRECT_CERT_TYPE;
        }

        /* Verify certificate version */
        /*----------------------------*/
        expVersion = (CC_SB_CERT_VERSION_MAJOR << CERT_VERSION_MAJOR_BIT_SHIFT) | CC_SB_CERT_VERSION_MINOR;
        if (pCertHeader->certVersion != expVersion) {
                CC_PAL_LOG_ERR("Certificate version incorrect, expVersion 0x%x, pCertHeader->certVersion 0x%x\n",
                               expVersion, pCertHeader->certVersion);
                return CC_BOOT_IMG_VERIFIER_CERT_VERSION_NUM_INCORRECT;
        }

        // set the actual certificate type
        *pCertType = certType;
        return CC_OK;

}



/**
   @brief This function is basic verification for all secure boot/debug certificates.
   it verifies type, size, public key and signature.
   Return pointers to certificate proprietary header, and body.
   Workspace should be clear when function returns
        call CCCertFieldsParse() - according to certificate type(x509 or not),
                copy public key, Np and signature to workspace,
                and returns pointers to certificate proprietary header, and body.
        call CCCertValidateHeader(), and verify cert type (as expected) and size (according to type).
        If expected public key hash is NULL, call CC_BsvPubKeyHashGet() with HBK type defined in certificate to get OTP HBK
        Call verifyCertPubKeyAndSign() To verify public key and certificate signature.
                Public key is verified against the expected value, and N and Np and signature resides on workspace.

 */
CCError_t CCCommonCertVerify(unsigned long   hwBaseAddress,
                             BufferInfo32_t  *pCertInfo,
                             CertFieldsInfo_t  *pCertFields,  // in/out
                             CCSbCertInfo_t  *pSbCertInfo,   //in/out
                             BufferInfo32_t  *pWorkspaceInfo,
                             BufferInfo32_t  *pX509HeaderInfo) //in/out
{
        uint32_t        rc = 0;
        uint32_t        certSignedSize = 0;
        keyCertFlags_t  certFlag;
        uint32_t    *pCertStartSign;

        if ((pWorkspaceInfo == NULL) ||
            (pWorkspaceInfo->pBuffer == NULL) ||
            (pWorkspaceInfo->bufferSize < sizeof(workspaceInt_t)) ||
            (!IS_ALIGNED(pWorkspaceInfo->bufferSize, sizeof(uint32_t))) ||
            (!IS_ALIGNED(sizeof(workspaceInt_t), sizeof(uint32_t)))) {
                CC_PAL_LOG_ERR("workspace and or sizes illegal\n");
                return CC_BSV_ILLEGAL_INPUT_PARAM_ERR;
        }

        /* Parse the certificate fields to get pointers to the certificate internals */
        /*---------------------------------------------------------------------------*/
        rc = CCCertFieldsParse(pCertInfo,
                               pWorkspaceInfo,
                               pCertFields,
                               &pCertStartSign,
                               &certSignedSize,
                               pX509HeaderInfo);
        if (rc != CC_OK) {
                CC_PAL_LOG_ERR("Failed CCCertFieldsParse 0x%x\n", rc);
                goto end_with_error;
        }
        /* Verify Magic number, and version. returns the certificate type */
        /*----------------------------------------------------------------*/
        rc = CCCertValidateHeader(&pCertFields->certHeader, &pCertFields->certType);
        if (rc != CC_OK) {
                CC_PAL_LOG_ERR("Failed CCCertValidateHeader 0x%x\n", rc);
                goto end_with_error;
        }

        // Verify certificate size. no need to verify the type again
        if (((pCertFields->certType == CC_SB_KEY_CERT) ||
             (pCertFields->certType == CC_SB_CONTENT_CERT) ||
             (pCertFields->certType == CC_SB_ENABLER_CERT) ||
             (pCertFields->certType ==  CC_SB_DEVELOPER_CERT)) &&
            (certSignedSize > certMainMaxSize[pCertFields->certType])) {
                CC_PAL_LOG_ERR("certSignedSize too big 0x%x for cert %d\n", certSignedSize, pCertFields->certType);
                rc = CC_BOOT_IMG_VERIFIER_INCORRECT_CERT_TYPE;
                goto end_with_error;
        }

        if (pSbCertInfo->initDataFlag == CC_SB_FIRST_CERT_IN_CHAIN) { // Verify the first HBK in chain
                certFlag.flagsWord = pCertFields->certHeader.certFlags;
                pSbCertInfo->keyIndex = (CCSbPubKeyIndexType_t)(certFlag.flagsBits.hbkId);
                if (pSbCertInfo->keyIndex > CC_SB_HASH_BOOT_KEY_256B) {
                        CC_PAL_LOG_ERR("invalid hbkId %d", pSbCertInfo->keyIndex);
                        rc = CC_BOOT_IMG_VERIFIER_ILLEGAL_HBK_IDX;
                        goto end_with_error;
                }
        }


        /* Verify certificate public key and it's signature,  pCertStartSign is word aligned for propritery and x509*/
        /*--------------------------------------------------*/
        rc = verifyCertPubKeyAndSign(hwBaseAddress,
                                     (uint32_t *)pCertStartSign,
                                     certSignedSize,
                                     pSbCertInfo,
                                     (workspaceInt_t  *)pWorkspaceInfo->pBuffer);
        if (rc != CC_OK) {
                CC_PAL_LOG_ERR("verifyCertPubKeyAndSign failed 0x%X\n", rc);
                goto end_with_error;
        }

        goto end;

end_with_error:
        UTIL_MemSet((uint8_t *)pCertFields, 0, sizeof(CertFieldsInfo_t));
end:
        UTIL_MemSet((uint8_t *)pWorkspaceInfo->pBuffer, 0, pWorkspaceInfo->bufferSize);
        return rc;
}


/**
   @brief This function verifies key certificate specific fields
        The header flags, NV counter according to HBK type
        Return next certificate public key hash.
 */
uint32_t CCCommonKeyCertVerify(unsigned long   hwBaseAddress,
                               uint32_t certFlags,
                               uint8_t *pCertMain,
                               CCSbCertInfo_t *pCertPkgInfo)
{
        uint32_t       rc = 0;
        keyCertFlags_t  keyFlag;
        KeyCertMain_t certMain;

        keyFlag.flagsWord = certFlags;

        if (pCertPkgInfo->initDataFlag == CC_SB_FIRST_CERT_IN_CHAIN) {
                pCertPkgInfo->keyIndex = (CCSbPubKeyIndexType_t)(keyFlag.flagsBits.hbkId);
        }

        /* Copy non-aligned certFields.pCertBody into aligned struct */
        UTIL_MemCopy((uint8_t *)&certMain, (uint8_t *)pCertMain, sizeof(KeyCertMain_t));

        /* Verify that the SW version is valid */
        rc = CCSbVerifyNvCounter(hwBaseAddress, certMain.swVer, pCertPkgInfo);
        if (rc != CC_OK) {
                CC_PAL_LOG_ERR("CCSbVerifyNvCounter failed 0x%X\n", rc);
                return rc;
        }
        if (pCertPkgInfo->initDataFlag == CC_SB_FIRST_CERT_IN_CHAIN) {
                pCertPkgInfo->activeMinSwVersionVal = certMain.swVer;
        }

        /* Set function output values */
        UTIL_MemCopy((uint8_t *)pCertPkgInfo->pubKeyHash, (uint8_t *)certMain.nextPubKeyHash, sizeof(CCHashResult_t));

        return CC_OK;
}

/**
   @brief This function   verifies content certificate specific fields
        Verifies certificate flags, NV counter according to HBK type
        Call CCCertValidateSWComps()
        Call CCSbSetNvCounter()
 *      The function uses the workspace  for loading the
 *      non-signed certificate part and  if required to load and
 *      verify the images.
 */
uint32_t CCCommonContentCertVerify(CCSbFlashReadFunc flashReadFunc,
                                   void *userContext,
                                   unsigned long hwBaseAddress,
                                   CCAddr_t certStoreAddress,
                                   CCSbCertInfo_t *certPkgInfo,
                                   uint32_t certFlags,
                                   uint8_t  *pCertMain,
                                   BufferInfo32_t  *pWorkspaceInfo)
{
        CCError_t rc = CC_OK;
        CCSbCertParserSwCompsInfo_t swImagesData;
        /* Content additional data is always word aligned*/
        uint32_t *pSwImagesAddData;
        uint32_t sizeOfNonSignedCert = 0;
        uint32_t numOfImages = 0;
        CCSbCertFlags_t flags;
        uint32_t  swVer;


        /* 1. Get the number of sw components from the header flags field */
        flags.flagsWord = certFlags;
        numOfImages = flags.flagsBits.numOfSwCmp;
        if ((numOfImages > CC_SB_MAX_NUM_OF_IMAGES) ||
            (numOfImages == 0)) {
                return CC_BOOT_IMG_VERIFIER_ILLEGAL_NUM_OF_IMAGES;
        }

        /* 2. Load the extended data (unsigned data), in this stage the certificate is already verified. */
        sizeOfNonSignedCert = numOfImages * SW_REC_NONE_SIGNED_DATA_SIZE_IN_BYTES;
        if (sizeOfNonSignedCert > pWorkspaceInfo->bufferSize) {
                return CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
        }

        /* Read the Non-signed part of the certificate package from the Flash, and place it right after the certificate. */
        rc = flashReadFunc(certStoreAddress, (uint8_t *)pWorkspaceInfo->pBuffer, sizeOfNonSignedCert, userContext);
        if (rc != CC_OK) {
                CC_PAL_LOG_ERR("failed flashRead_func for Non-signed part\n");
                return rc;
        }
        pSwImagesAddData = pWorkspaceInfo->pBuffer;
        pWorkspaceInfo->bufferSize -= sizeOfNonSignedCert;
        pWorkspaceInfo->pBuffer += (sizeOfNonSignedCert / CC_32BIT_WORD_SIZE) + 1;

        /* 3. verify sw version
           Copy 4 bytes instead of accessing teh struct field, since  pCertMain is not word aligned */
        UTIL_MemCopy((uint8_t *)&swVer, (uint8_t *)pCertMain, sizeof(uint32_t));
        rc = CCSbVerifyNvCounter(hwBaseAddress, swVer, certPkgInfo);
        if (rc != CC_OK) {
                CC_PAL_LOG_ERR("CCSbVerifyNvCounter failed\n");
                return rc;
        }

        /* 4. load and verify sw comps */
        swImagesData.swCodeEncType = (CCswCodeEncType_t)(flags.flagsBits.swCodeEncType);
        swImagesData.swLoadVerifyScheme = (CCswLoadVerifyScheme_t)(flags.flagsBits.swLoadVerifyScheme);
        swImagesData.swCryptoType = (CCswCryptoType_t)(flags.flagsBits.swCryptoType);
        swImagesData.numOfSwComps = (flags.flagsBits.numOfSwCmp);

        /* move the pointers for nonce and images by sizeof bytes instead of using struct fiels, since  pCertMain is not word aligned */
        UTIL_MemCopy((uint8_t *)swImagesData.nonce, (uint8_t *)(((unsigned long)pCertMain) + sizeof(uint32_t)), sizeof(CCSbNonce_t));
        swImagesData.pSwCompsData = (uint8_t *)(((unsigned long)pCertMain) + sizeof(uint32_t) + sizeof(CCSbNonce_t));

        rc = CCCertValidateSWComps(flashReadFunc,
                                   userContext,
                                   hwBaseAddress,
                                   certPkgInfo->keyIndex,
                                   &swImagesData,
                                   pSwImagesAddData,
                                   pWorkspaceInfo->pBuffer,
                                   pWorkspaceInfo->bufferSize);
        if (rc != CC_OK) {
                CC_PAL_LOG_ERR("CCCertValidateSWComps failed\n");
                return rc;
        }

        /* 5. Assuming there is only one content certificate in the chain, */
        /*    Set the sw version in the OTP (if required)  */
        rc = CCSbSetNvCounter(hwBaseAddress, certPkgInfo);
        if (rc != CC_OK) {
                CC_PAL_LOG_ERR("CCSbSetNvCounter failed\n");
                return rc;
        }
        return CC_OK;
}


