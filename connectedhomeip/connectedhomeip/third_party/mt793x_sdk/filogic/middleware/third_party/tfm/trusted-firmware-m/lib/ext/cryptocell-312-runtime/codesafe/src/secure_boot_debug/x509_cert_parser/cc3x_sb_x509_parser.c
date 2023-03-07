/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "secureboot_basetypes.h"
#include "util_asn1_parser.h"
#include "util_x509_parser.h"
#include "sb_x509_error.h"
#include "sb_x509_cert_parser.h"
#include "cc3x_sb_x509_ext_parser.h"
#include "bootimagesverifier_error.h"
#include "cc_pal_x509_defs.h"
#include "bsv_error.h"
#include "cc_bitops.h"
#include "cc_pal_log.h"
#include "secdebug_defs.h"
#include "cc_pka_hw_plat_defs.h"
#include "common_cert_parser.h"
#include "secureboot_stage_defs.h"


#define SIZE_OF_CERT_ASN1_HEADER 4 // 0x30,0x82, MSB size, LSB size - not like CCSbCertAsn1Data_t, since we have 2 bytes of certificate size

/**
   @brief This function verifies the secure debug package size and returns the
   pointer to each of the certificates within the package.
  from the ASN.1 first package we read the first certificate size.
  We jump to the following certificate and read its size from the ASN.1 header.
  If the sum of both certificates smaller than the certificate package size,
  we assume we have key certificate as well. And set the certificate pointers
  according to the ASN.1 header sizes of each certificate.
  In any case we need to compare each certificate size and the certificate package
  total size with the maximum possible value of the certificate sizes.
 */
CCError_t CCCertSecDbgParse(uint32_t   *pDebugCertPkg,
                            uint32_t   certPkgSize,
                            BufferInfo32_t  *pKeyCert,         // out
                            BufferInfo32_t  *pEnablerCert,   // out
                            BufferInfo32_t  *pDeveloperCert) // out
{

        uint32_t rc = 0;
        CCSbCertAsn1Data_t asn1DataCert1;
        CCSbCertAsn1Data_t asn1DataCert2;
        uint32_t    cert2WordOffset;
        CCSbCertAsn1Data_t asn1DataCert3;
        uint32_t    cert3WordOffset;
        uint32_t  numOfCert = 0;
        uint32_t  *pCertPkg = pDebugCertPkg;
        uint32_t  *pCertFirst;
        uint32_t  *pCertSecond;
        uint32_t  *pCertThird;
        unsigned long endAddr = (unsigned long)pDebugCertPkg + certPkgSize;

        /* certificate offsets within package must be word aligned */
        // Get the first certificate size
        pCertFirst = pCertPkg;
        rc = UTIL_Asn1ReadItemVerifyTagFW((uint8_t **)&pCertPkg, &asn1DataCert1, CC_X509_CERT_SEQ_TAG_ID, (unsigned long)pCertPkg, endAddr);
        if (rc != CC_OK) {
                CC_PAL_LOG_ERR("Failed to UTIL_Asn1ReadItemVerifyTagFW 0x%x for cert1\n", rc);
                goto end_with_error;
        }

        /* Make sure no wrap around */
        if ((asn1DataCert1.itemSize + SIZE_OF_CERT_ASN1_HEADER) < asn1DataCert1.itemSize) {
                CC_PAL_LOG_ERR("illegal value %d\n", asn1DataCert1.itemSize);
                goto end_with_error;
        }

        /* Some sanity checks for certificate size*/
        if (((asn1DataCert1.itemSize + SIZE_OF_CERT_ASN1_HEADER) > certPkgSize) || ((asn1DataCert1.itemSize + SIZE_OF_CERT_ASN1_HEADER) > CC_SB_MAX_ENABLER_CERT_SIZE_IN_BYTES)) {
                CC_PAL_LOG_ERR("asn1Data.itemSize(0x%x) > certPkgSize(0x%x)\n", asn1DataCert1.itemSize, certPkgSize);
                goto end_with_error;
        }
        numOfCert++;

        /* Get the second certificate size */
        cert2WordOffset = CALC_32BIT_WORDS_FROM_BYTES(asn1DataCert1.itemSize);
        if ((cert2WordOffset * CC_32BIT_WORD_SIZE + SIZE_OF_CERT_ASN1_HEADER) > certPkgSize) {
                CC_PAL_LOG_ERR("cert2WordOffset(0x%x) > certPkgSize(0x%x)\n", cert2WordOffset * CC_32BIT_WORD_SIZE, certPkgSize);
                goto end_with_error;
        }
        pCertPkg += cert2WordOffset;
        pCertSecond = pCertPkg;
        rc = UTIL_Asn1ReadItemVerifyTagFW((uint8_t **)&pCertPkg, &asn1DataCert2, CC_X509_CERT_SEQ_TAG_ID, (unsigned long)pCertPkg, endAddr);
        if (rc != CC_OK) {
                CC_PAL_LOG_ERR("Failed to UTIL_Asn1ReadItemVerifyTagFW 0x%x for cert2\n", rc);
                goto end_with_error;
        }

        /* Verify no wrap around. */
        if ((asn1DataCert2.itemSize + SIZE_OF_CERT_ASN1_HEADER) < asn1DataCert2.itemSize) {
                CC_PAL_LOG_ERR("illegal value %d\n", asn1DataCert1.itemSize);
                goto end_with_error;
        }
        if ((asn1DataCert2.itemSize + SIZE_OF_CERT_ASN1_HEADER) > (certPkgSize - (cert2WordOffset * CC_32BIT_WORD_SIZE)) ||
            ((asn1DataCert2.itemSize + SIZE_OF_CERT_ASN1_HEADER) > CC_SB_MAX_ENABLER_CERT_SIZE_IN_BYTES)) { /* enabler certificate is the biggest certificate */
                CC_PAL_LOG_ERR("asn1DataCert2.itemSize(0x%x) > (certPkgSize(0x%x)-cert2WordOffset(0x%x))\n",
                               asn1DataCert2.itemSize, certPkgSize, cert2WordOffset);
                goto end_with_error;
        }
        numOfCert++;

        // Get the third certificate size - if the offset is outside of boundaries return (there are only 2 certificates).
        cert3WordOffset = CALC_32BIT_WORDS_FROM_BYTES(asn1DataCert2.itemSize) + cert2WordOffset;
        if (((cert3WordOffset * CC_32BIT_WORD_SIZE) + 2 * SIZE_OF_CERT_ASN1_HEADER) >= certPkgSize) {  // meaning only 2 certificates in chain
                goto end;
        }
        pCertPkg += (cert3WordOffset - cert2WordOffset);
        pCertThird = pCertPkg;
        rc = UTIL_Asn1ReadItemVerifyTagFW((uint8_t **)&pCertPkg, &asn1DataCert3, CC_X509_CERT_SEQ_TAG_ID, (unsigned long)pCertPkg, endAddr);
        if (rc != CC_OK) {
                CC_PAL_LOG_ERR("Failed to UTIL_Asn1ReadItemVerifyTagFW 0x%x for cert3\n", rc);
                goto end_with_error;
        }

        if ((asn1DataCert3.itemSize + SIZE_OF_CERT_ASN1_HEADER) < asn1DataCert3.itemSize) {
                CC_PAL_LOG_ERR("illegal value %d\n", asn1DataCert3.itemSize);
                goto end_with_error;
        }
        if ((asn1DataCert3.itemSize + SIZE_OF_CERT_ASN1_HEADER) > (certPkgSize - (cert3WordOffset * CC_32BIT_WORD_SIZE)) ||
            ((asn1DataCert3.itemSize + SIZE_OF_CERT_ASN1_HEADER) > CC_SB_MAX_ENABLER_CERT_SIZE_IN_BYTES)) {
                CC_PAL_LOG_ERR("asn1DataCert3.itemSize(0x%x) > (certPkgSize(0x%x)-cert3WordOffset(0x%x))\n",
                               asn1DataCert3.itemSize, certPkgSize, cert3WordOffset * CC_32BIT_WORD_SIZE);
                goto end_with_error;
        }
        numOfCert++;

end:
        if (numOfCert == 2) {
                pKeyCert->bufferSize = 0;
                pKeyCert->pBuffer = NULL;
                pEnablerCert->bufferSize = asn1DataCert1.itemSize;
                pEnablerCert->pBuffer = pCertFirst;
                pDeveloperCert->bufferSize = asn1DataCert2.itemSize;
                pDeveloperCert->pBuffer = pCertSecond;
        } else if (numOfCert == 3) {
                pKeyCert->bufferSize = asn1DataCert1.itemSize;
                pKeyCert->pBuffer = pCertFirst;
                pEnablerCert->bufferSize = asn1DataCert2.itemSize;
                pEnablerCert->pBuffer = pCertSecond;
                pDeveloperCert->bufferSize = asn1DataCert3.itemSize;
                pDeveloperCert->pBuffer = pCertThird;
        }
        return CC_OK;

end_with_error:
        pKeyCert->bufferSize = 0;
        pKeyCert->pBuffer = NULL;
        pEnablerCert->bufferSize = 0;
        pEnablerCert->pBuffer = NULL;
        pDeveloperCert->bufferSize = 0;
        pDeveloperCert->pBuffer = NULL;

        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;

}


/**
   @brief This function load sizeof(CCSbCertAsn1Data_t ) from flash and get the
   certificate size from it. Make sure size is within range
   (smaller than workspace size not including the required space for N, Np and signature).
   read the certificate according to size from header and copy the certificate content from Flash to RAM.
 */
uint32_t CCCertLoadCertificate(CCSbFlashReadFunc flashRead_func,
                               void *userContext,
                               CCAddr_t certAddress,
                               uint32_t *pCert,
                               uint32_t *pCertBufferWordSize)
{
        uint32_t rc = 0;
        CCSbCertAsn1Data_t asn1DataCert1;
        uint8_t *plCert = (uint8_t *)pCert;
        uint32_t certSizeFullWords;

        /* Verify that the certificate buffer size is big enough to contain the header */
        if (*pCertBufferWordSize < (SIZE_OF_CERT_ASN1_HEADER / CC_32BIT_WORD_SIZE)) {
                CC_PAL_LOG_ERR("certificate buff size too small to contain certificate header\n");
                return CC_BOOT_IMG_VERIFIER_WORKSPACE_SIZE_TOO_SMALL;
        }

        /* Read the certificate header from the Flash */
        rc = flashRead_func(certAddress,
                            plCert,
                            SIZE_OF_CERT_ASN1_HEADER,
                            userContext);
        if (rc != CC_OK) {
                CC_PAL_LOG_ERR("failed flashRead_func for certificate header\n");
                return rc;
        }

        rc = UTIL_Asn1ReadItemVerifyTagFW((uint8_t **)&plCert,
                                          &asn1DataCert1,
                                          CC_X509_CERT_SEQ_TAG_ID,
                                          (unsigned long)plCert,
                                          (unsigned long)plCert + SIZE_OF_CERT_ASN1_HEADER);
        if (rc != CC_OK) {
                CC_PAL_LOG_ERR("Failed to UTIL_Asn1ReadItemVerifyTagFW 0x%x for cert header\n", rc);
                return rc;
        }

        certSizeFullWords = ALIGN_TO_4BYTES(asn1DataCert1.itemSize);

        /* Verify no wrap around */
        if ((*pCertBufferWordSize) * CC_32BIT_WORD_SIZE - SIZE_OF_CERT_ASN1_HEADER > (*pCertBufferWordSize) * CC_32BIT_WORD_SIZE) {
                CC_PAL_LOG_ERR("Certificate size too big\n");
                return CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
        }
        /* Make sure certificate size is within range */
        if (certSizeFullWords > ((*pCertBufferWordSize) * CC_32BIT_WORD_SIZE - SIZE_OF_CERT_ASN1_HEADER)) {
                CC_PAL_LOG_ERR("Certificate size too big\n");
                return CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
        }

        /* according to the header read the additional certificate buffer -
          * not including the non-signed part in case of content certificate */
        rc = flashRead_func(certAddress + SIZE_OF_CERT_ASN1_HEADER,
                            (uint8_t *)plCert,
                            asn1DataCert1.itemSize,
                            userContext);
        if (rc != CC_OK) {
                CC_PAL_LOG_ERR("failed flashRead_func for certificate\n");
                return rc;
        }

        *pCertBufferWordSize = ((certSizeFullWords + SIZE_OF_CERT_ASN1_HEADER) / CC_32BIT_WORD_SIZE);

        return CC_OK;

}


/**
   @brief This function
   call SB_X509_VerifyCertTbsHeader() to verify the X509 header and get the user Data and public key.
   Copy the public key into workspace
   Call SB_X509_ParseCertExtensions() to get pointers for:
      Proprietary header pointer
      Copy Np into workspace following N
      Proprietary Certificate body pointer
   Call UTIL_X509GetSignature(), and copy the signature into workspace, after Np.
 */
uint32_t CCCertFieldsParse(BufferInfo32_t  *pCertInfo,
                           BufferInfo32_t  *pWorkspaceInfo,
                           CertFieldsInfo_t  *pCertFields,
                           uint32_t **ppCertStartSign,
                           uint32_t *pCertSignedSize,
                           BufferInfo32_t  *pX509HeaderInfo)
{
        uint32_t rc = 0;
        uint32_t  certSignedSize = 0;
        uint32_t    certStartOffest;
        CCSbCertHeader_t *lpCertHeader;
        uint8_t *lpNp;
        CCSbSignature_t *lpSignature;
        unsigned long startAddr = (unsigned long)pCertInfo->pBuffer;
        unsigned long endAddr = (unsigned long)pCertInfo->pBuffer + pCertInfo->bufferSize + sizeof(CCSbCertAsn1Data_t);
        CCX509CertHeaderInfo_t   *pX509UserData = NULL;
        uint8_t     *pX509Cert = (uint8_t *)pCertInfo->pBuffer;
        workspaceInt_t  *lpWorkspaceInt;

        if ((pWorkspaceInfo == NULL) ||
            (pWorkspaceInfo->pBuffer == NULL) ||
            (pWorkspaceInfo->bufferSize < sizeof(workspaceInt_t))) {
                CC_PAL_LOG_ERR("workspace and or sizes illegal\n");
                return CC_BSV_ILLEGAL_INPUT_PARAM_ERR;
        }

        if (startAddr > endAddr) {  /* Verify no overlap */
                CC_PAL_LOG_ERR("buffer overlap detected \n");
                return CC_BSV_ILLEGAL_INPUT_PARAM_ERR;
        }
        lpWorkspaceInt = (workspaceInt_t  *)(pWorkspaceInfo->pBuffer);

        if (pX509HeaderInfo != NULL) {
                if (((pX509HeaderInfo->pBuffer == NULL) && (pX509HeaderInfo->bufferSize != 0)) ||
                    ((pX509HeaderInfo->pBuffer != NULL) && (pX509HeaderInfo->bufferSize < sizeof(CCX509CertHeaderInfo_t)))) {
                        CC_PAL_LOG_ERR("workspace and or sizes illegal\n");
                        return CC_BSV_ILLEGAL_INPUT_PARAM_ERR;
                }
                pX509UserData = (CCX509CertHeaderInfo_t *)pX509HeaderInfo->pBuffer;
        }

        rc = SB_X509_VerifyCertTbsHeader(&pX509Cert,
                                         pCertInfo->bufferSize,
                                         &certSignedSize,
                                         &certStartOffest,
                                         (CCSbNParams_t *)&(lpWorkspaceInt->pubKey),
                                         pX509UserData,
                                         startAddr,
                                         endAddr);

        if ((rc != CC_OK) ||
            (certSignedSize > pCertInfo->bufferSize - SB_CERT_RSA_KEY_SIZE_IN_BYTES)) {
                CC_PAL_LOG_ERR("Failed SB_X509_VerifyCertTbsHeader 0x%x\n", rc);
                goto error;
        }

        *ppCertStartSign = pCertInfo->pBuffer + 1;
        *pCertSignedSize = certSignedSize;

        /* Copy the proprietary header from the extension,
           copy Np from the extension,
           get the pointer to the certificate main */
        rc = SB_X509_ParseCertExtensions(&pX509Cert,
                                         certSignedSize,
                                         &lpCertHeader,
                                         &lpNp,
                                         (uint8_t **)&pCertFields->pCertBody,
                                         &pCertFields->certBodySize,
                                         startAddr,
                                         endAddr);
        if ((rc != CC_OK) ||
            (pCertFields->certBodySize > certSignedSize)) {
                CC_PAL_LOG_ERR("Failed SB_X509_ParseCertExtensions 0x%x, or bodySize 0x%x too big 0x%x\n", rc, pCertFields->certBodySize, certSignedSize);
                goto error;
        }

        UTIL_MemCopy((uint8_t *)&pCertFields->certHeader, (uint8_t *)lpCertHeader, sizeof(CCSbCertHeader_t));
        UTIL_MemCopy((uint8_t *)&(lpWorkspaceInt->pubKey.Np), lpNp, RSA_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_BYTES);


        lpSignature = (CCSbSignature_t *)&(lpWorkspaceInt->signature);
        rc = UTIL_X509GetSignature(&pX509Cert, lpSignature, startAddr, endAddr);
        if (rc != CC_OK) {
                CC_PAL_LOG_ERR("Failed UTIL_X509GetSignature 0x%x\n", rc);
                goto error;
        }

        return CC_OK;
error:
        UTIL_MemSet((uint8_t *)pCertFields, 0, sizeof(CertFieldsInfo_t));
        *pCertSignedSize = 0;

        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
}

uint32_t CCCertGetUnsignedDataOffset(uint32_t *pCert,
                     uint32_t *pUnsignedDataOffset)
{
        CCError_t rc = CC_OK;
        uint8_t *plCert = (uint8_t *)pCert;
        CCSbCertAsn1Data_t asn1DataCert1;
        uint32_t certSizeFullWords;

        if ((pCert == NULL)||(pUnsignedDataOffset == NULL)){
        return CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
    }

        rc = UTIL_Asn1ReadItemVerifyTagFW((uint8_t **)&plCert,
                                          &asn1DataCert1,
                                          CC_X509_CERT_SEQ_TAG_ID,
                                          (unsigned long)plCert,
                                          (unsigned long)plCert + SIZE_OF_CERT_ASN1_HEADER);
        if (rc != CC_OK) {
                CC_PAL_LOG_ERR("Failed to UTIL_Asn1ReadItemVerifyTagFW 0x%x for cert header\n", rc);
                return rc;
        }

        certSizeFullWords = ALIGN_TO_4BYTES(asn1DataCert1.itemSize);
        *pUnsignedDataOffset = ((certSizeFullWords + SIZE_OF_CERT_ASN1_HEADER) / CC_32BIT_WORD_SIZE);

        return CC_OK;
}

