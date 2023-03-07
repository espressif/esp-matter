/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define CC_PAL_LOG_CUR_COMPONENT CC_LOG_MASK_SECURE_BOOT

/************* Include Files ****************/

#include "secureboot_basetypes.h"
#include "secureboot_stage_defs.h"
#include "secureboot_defs.h"
#include "cc_crypto_x509_defs.h"
#include "secureboot_base_func.h"
#include "sb_x509_error.h"
#include "sb_x509_cert_parser.h"
#include "util_asn1_parser.h"
#include "util_base64.h"
#include "util_x509_parser.h"
#include "cc_pal_log.h"
#include "cc_crypto_boot_defs.h"
#ifdef CC_SB_SUPPORT_IOT
#include "secureboot_stage_defs.h"
#else
#include "sbrom_bsv_api.h"
#include "bootimagesverifierx509_def.h"
#include "sbrom_bsv_error.h"
#include "sb_x509_ext_parser.h"
#endif

/*!
 * @brief Parse and validate TBS header certificate
 *
 * @param[in] ppAsn1Cert    - pointer to X509 certificate as ASN.1 byte array
 * @param[in] certMaxSize   - certificate max size (according to certificate type)
 * @param[out] pSignCertSize    - certificate TBS size, used for signature
 * @param[out] pTbsStartOffset - certificate TBS start offset, used for signature
 * @param[out] pOutPubKey       - certificate public key modulus (exponent is constant)
 *
 * @return uint32_t         - On success: the value CC_OK is returned,
 *                    On failure: a value from sbrom_bsv_error.h
 */
CCError_t SB_X509_VerifyCertTbsHeader(uint8_t    **ppAsn1Cert,
                      uint32_t      certMaxSize,
                      uint32_t      *pSignCertSize,
                      uint32_t      *pTbsStartOffset,
                      CCSbNParams_t     *pOutPubKey,
                      CCX509CertHeaderInfo_t *pOutCertHeaderInfo,
                      unsigned long startAddress,
                      unsigned long endAddress)
{
    uint32_t    rc = 0;
    uint32_t    strSize = 0;
    CCSbCertAsn1Data_t asn1Data;
    uint8_t     algId[] = CC_X509_CERT_SHA256RSAPSS_ID;
    uint8_t objSha256Id[] = CC_X509_CERT_SHA256_ID;
    uint8_t objMgf1Id[] = CC_X509_CERT_MGF1_ID;
    uint32_t         notBeforeStrSize = 0;
    uint32_t         notAfterStrSize  = 0;

    /* validate inputs */
    if ((NULL == ppAsn1Cert) ||
        (NULL == pOutPubKey) ||
        (NULL == pSignCertSize) ||
        (NULL == pTbsStartOffset) ||
        (certMaxSize == 0)) {
        CC_PAL_LOG_ERR("Invalid inputs\n");
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }

    /* set default output values */
    *pSignCertSize = 0;
    *pTbsStartOffset = 0;
    UTIL_MemSet((uint8_t *)pOutPubKey, 0, sizeof(CCSbNParams_t));

    /* 1. get certificate size , validate tag + size */
    rc = UTIL_Asn1ReadItemVerifyTagFW(ppAsn1Cert, &asn1Data, CC_X509_CERT_SEQ_TAG_ID, startAddress, endAddress);
    if (rc != CC_OK) {
        CC_PAL_LOG_ERR("Failed to UTIL_Asn1ReadItemVerifyTagFW 0x%x for CC_X509_CERT_SEQ_TAG_ID\n", rc);
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }

    if (asn1Data.itemSize > certMaxSize){
        CC_PAL_LOG_ERR("asn1Data.itemSize > certTypeMaxSize[certType]\n");
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }

    /* certificate signature is on all TBS */
    *pTbsStartOffset = asn1Data.index;

    /* 2. get TBS size - no need to verify size */
    rc = UTIL_Asn1ReadItemVerifyTagFW(ppAsn1Cert, &asn1Data, CC_X509_CERT_SEQ_TAG_ID, startAddress, endAddress);
    if (rc != CC_OK) {
        CC_PAL_LOG_ERR("Failed to UTIL_Asn1ReadItemVerifyTagFW 0x%x for CC_X509_CERT_SEQ_TAG_ID\n", rc);
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }

    /* certificate signature is on all TBS */
    *pSignCertSize = asn1Data.itemSize + asn1Data.index;
    if (*pSignCertSize > certMaxSize ){ /* the size of the certificate to be verified cannot be bigger than certMaxSize*/
        CC_PAL_LOG_ERR("asn1Data.itemSize > certTypeMaxSize\n");
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }

    /* 3. get version and verify it is v3 */
    rc = UTIL_Asn1ReadItemVerifyTagFW(ppAsn1Cert, &asn1Data, CC_X509_CERT_CTX_SPEC_TAG_ID, startAddress, endAddress);
    if (rc != CC_OK) {
        CC_PAL_LOG_ERR("Failed to UTIL_Asn1ReadItemVerifyTagFW 0x%x for CC_X509_CERT_CTX_SPEC_TAG_ID\n", rc);
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    rc = UTIL_Asn1ReadItemVerifyTagFW(ppAsn1Cert, &asn1Data, CC_X509_CERT_INT_TAG_ID, startAddress, endAddress);
    if (rc != CC_OK) {
        CC_PAL_LOG_ERR("Failed to UTIL_Asn1ReadItemVerifyTagFW 0x%x for CC_X509_CERT_INT_TAG_ID\n", rc);
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }

    if (**ppAsn1Cert != CC_X509_CERT_VERSION ){
        CC_PAL_LOG_ERR("Ilegal certificate version 0x%x\n", **ppAsn1Cert);
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }

    UTIL_ASN1_GET_NEXT_ITEM_RET(*ppAsn1Cert, asn1Data.itemSize, startAddress, endAddress);


    /* 4. get the serial number */
    rc = UTIL_Asn1ReadItemVerifyTagFW(ppAsn1Cert, &asn1Data, CC_X509_CERT_INT_TAG_ID, startAddress, endAddress);
    if (rc != CC_OK) {
        CC_PAL_LOG_ERR("Failed to CC_X509_CERT_INT_TAG_ID for serial number\n");
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    if (pOutCertHeaderInfo != NULL){
        UTIL_MemCopy((uint8_t*)&(pOutCertHeaderInfo->serialNum), (uint8_t*)*ppAsn1Cert, sizeof(uint32_t));
#ifndef BIG__ENDIAN
        UTIL_ReverseBuff((uint8_t*)&(pOutCertHeaderInfo->serialNum),sizeof(uint32_t));
#endif
        pOutCertHeaderInfo->setSerialNum = 1;
    }

    UTIL_ASN1_GET_NEXT_ITEM_RET(*ppAsn1Cert, asn1Data.itemSize, startAddress, endAddress);

    /* 5. get the alg id and verify it */
    rc = UTIL_Asn1ReadItemVerifyTagFW(ppAsn1Cert, &asn1Data, CC_X509_CERT_SEQ_TAG_ID, startAddress, endAddress);
    if (rc != CC_OK) {
        CC_PAL_LOG_ERR("Failed to CC_X509_CERT_SEQ_TAG_ID for algId\n");
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    rc = UTIL_Asn1ReadItemVerifyTagFW(ppAsn1Cert, &asn1Data, CC_X509_CERT_OBJ_IDENTIFIER_TAG_ID, startAddress, endAddress);
    if (rc != CC_OK || asn1Data.itemSize != sizeof(algId)) {
        CC_PAL_LOG_ERR("Failed to CC_X509_CERT_OBJ_IDENTIFIER_TAG_ID for algId\n");
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    if ((rc = UTIL_MemCmp(*ppAsn1Cert, algId, sizeof(algId))) != CC_TRUE) {
        CC_PAL_LOG_ERR("Failed to UTIL_MemCmp algId\n");
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    UTIL_ASN1_GET_NEXT_ITEM_RET(*ppAsn1Cert, asn1Data.itemSize, startAddress, endAddress);

    /* verify sha256 + PSS + mgf1 attributes signature */
    rc = UTIL_Asn1ReadItemVerifyTagFW(ppAsn1Cert, &asn1Data, CC_X509_CERT_SEQ_TAG_ID, startAddress, endAddress);
    if (rc != CC_OK) {
        CC_PAL_LOG_ERR("Failed to CC_X509_CERT_SEQ_TAG_ID for PSS\n");
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    rc = UTIL_Asn1ReadItemVerifyTagFW(ppAsn1Cert, &asn1Data, CC_X509_CERT_CTX_SPEC_TAG_ID, startAddress, endAddress);
    if (rc != CC_OK) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    rc = UTIL_Asn1ReadItemVerifyTagFW(ppAsn1Cert, &asn1Data, CC_X509_CERT_SEQ_TAG_ID, startAddress, endAddress);
    if (rc != CC_OK) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    /* verify sha256 */
    rc = UTIL_Asn1ReadItemVerifyTagFW(ppAsn1Cert, &asn1Data, CC_X509_CERT_OBJ_IDENTIFIER_TAG_ID, startAddress, endAddress);
    if ((rc != CC_OK) || (asn1Data.itemSize != sizeof(objSha256Id))) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    if ((rc = UTIL_MemCmp(*ppAsn1Cert, objSha256Id, sizeof(objSha256Id))) != CC_TRUE){
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    UTIL_ASN1_GET_NEXT_ITEM_RET(*ppAsn1Cert, asn1Data.itemSize, startAddress, endAddress);
    /* verify mgf1 + sha256 */
    rc = UTIL_Asn1ReadItemVerifyTagFW(ppAsn1Cert, &asn1Data, CC_X509_CERT_CTX_SPEC_TAG1_ID, startAddress, endAddress);
    if (rc != CC_OK) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    rc = UTIL_Asn1ReadItemVerifyTagFW(ppAsn1Cert, &asn1Data, CC_X509_CERT_SEQ_TAG_ID, startAddress, endAddress);
    if (rc != CC_OK) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    /* verify mgf1 */
    rc = UTIL_Asn1ReadItemVerifyTagFW(ppAsn1Cert, &asn1Data, CC_X509_CERT_OBJ_IDENTIFIER_TAG_ID, startAddress, endAddress);
    if ((rc != CC_OK) || (asn1Data.itemSize != sizeof(objMgf1Id))) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    if ((rc = UTIL_MemCmp(*ppAsn1Cert, objMgf1Id, sizeof(objMgf1Id))) != CC_TRUE){
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    UTIL_ASN1_GET_NEXT_ITEM_RET(*ppAsn1Cert, asn1Data.itemSize, startAddress, endAddress);
    rc = UTIL_Asn1ReadItemVerifyTagFW(ppAsn1Cert, &asn1Data, CC_X509_CERT_SEQ_TAG_ID, startAddress, endAddress);
    if (rc != CC_OK) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    /* verify sha256 */
    rc = UTIL_Asn1ReadItemVerifyTagFW(ppAsn1Cert, &asn1Data, CC_X509_CERT_OBJ_IDENTIFIER_TAG_ID, startAddress, endAddress);
    if ((rc != CC_OK) || (asn1Data.itemSize != sizeof(objSha256Id))) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    if ((rc = UTIL_MemCmp(*ppAsn1Cert, objSha256Id, sizeof(objSha256Id))) != CC_TRUE){
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    UTIL_ASN1_GET_NEXT_ITEM_RET(*ppAsn1Cert, asn1Data.itemSize, startAddress, endAddress);

    /* verify last special tag size */
    rc = UTIL_Asn1ReadItemVerifyTagFW(ppAsn1Cert, &asn1Data, CC_X509_CERT_CTX_SPEC_TAG2_ID, startAddress, endAddress);
    if (rc != CC_OK) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    if (asn1Data.itemSize != CC_X509_CERT_CTX_SPEC_TAG2_SIZE){
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    UTIL_ASN1_GET_NEXT_ITEM_RET(*ppAsn1Cert, asn1Data.itemSize, startAddress, endAddress);

    /* 6. get the issuer name and verify it */
    rc = UTIL_X509VerifyStr(ppAsn1Cert, &strSize, startAddress, endAddress);
    if (rc != CC_OK || strSize > X509_ISSUER_NAME_MAX_STRING_SIZE ) {
        CC_PAL_LOG_ERR("Failed to UTIL_X509VerifyStr for issuer Name\n");
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    if (pOutCertHeaderInfo != NULL){
        if (strSize>0){
            UTIL_MemCopy((uint8_t*)pOutCertHeaderInfo->IssuerName, (uint8_t*)*ppAsn1Cert, strSize);
            pOutCertHeaderInfo->IssuerName[strSize]=0;
            pOutCertHeaderInfo->setIssuerName = 1;
        } else {
            pOutCertHeaderInfo->setIssuerName = 0;
            return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
        }
    }

    UTIL_ASN1_GET_NEXT_ITEM_RET(*ppAsn1Cert, strSize, startAddress, endAddress);

    /* 7. skip over the validity period */
    rc = UTIL_Asn1ReadItemVerifyTagFW(ppAsn1Cert, &asn1Data, CC_X509_CERT_SEQ_TAG_ID, startAddress, endAddress);
    if (rc != CC_OK) {
        CC_PAL_LOG_ERR("Failed to CC_X509_CERT_SEQ_TAG_ID for vallidity period\n");
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    rc = UTIL_Asn1ReadItemVerifyTagFW(ppAsn1Cert, &asn1Data, CC_X509_CERT_UTC_TIME_TAG_ID, startAddress, endAddress);
    if (rc != CC_OK || asn1Data.itemSize > X509_VALIDITY_PERIOD_MAX_STRING_SIZE) {
        CC_PAL_LOG_ERR("Failed to CC_X509_CERT_UTC_TIME_TAG_ID for notBefore\n");
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }

    if (pOutCertHeaderInfo != NULL) {
        if ((asn1Data.itemSize>0) && (asn1Data.itemSize<sizeof(pOutCertHeaderInfo->NotBeforeStr))){
            UTIL_MemCopy((uint8_t*)pOutCertHeaderInfo->NotBeforeStr, (uint8_t*)*ppAsn1Cert, asn1Data.itemSize);
            pOutCertHeaderInfo->NotBeforeStr[asn1Data.itemSize] = 0;
            notBeforeStrSize = asn1Data.itemSize;
            pOutCertHeaderInfo->setNotBeforeStr = 1;
        } else {
            pOutCertHeaderInfo->setNotBeforeStr = 0;
        }
    }

    UTIL_ASN1_GET_NEXT_ITEM_RET(*ppAsn1Cert, asn1Data.itemSize, startAddress, endAddress);

    rc = UTIL_Asn1ReadItemVerifyTagFW(ppAsn1Cert, &asn1Data, CC_X509_CERT_UTC_TIME_TAG_ID, startAddress, endAddress);
    if (rc != CC_OK) {
        rc = UTIL_Asn1ReadItemVerifyTagFW(ppAsn1Cert, &asn1Data, CC_X509_CERT_GENERALIZED_TIME_TAG_ID, startAddress, endAddress);
        if (rc != CC_OK || asn1Data.itemSize > X509_VALIDITY_PERIOD_MAX_STRING_SIZE) {
            CC_PAL_LOG_ERR("Failed to CC_X509_CERT_GENERALIZED_TIME_TAG_ID for notAfter\n");
            return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
        }
    }
    if (pOutCertHeaderInfo != NULL) {
        if ((asn1Data.itemSize>0) && (asn1Data.itemSize<sizeof(pOutCertHeaderInfo->NotAfterStr))){
            UTIL_MemCopy((uint8_t*)pOutCertHeaderInfo->NotAfterStr, (uint8_t*)*ppAsn1Cert, asn1Data.itemSize);
            pOutCertHeaderInfo->NotAfterStr[asn1Data.itemSize] = 0;
            notAfterStrSize = asn1Data.itemSize;
            pOutCertHeaderInfo->setNotAfterStr = 1;
        } else {
            pOutCertHeaderInfo->setNotAfterStr = 0;
        }

        rc = CC_PalVerifyCertValidity(pOutCertHeaderInfo->NotBeforeStr,
            notBeforeStrSize,
            pOutCertHeaderInfo->setNotBeforeStr,
            pOutCertHeaderInfo->NotAfterStr,
            notAfterStrSize,
            pOutCertHeaderInfo->setNotAfterStr);

            if (rc != CC_OK) {
                CC_PAL_LOG_ERR("Failed to verify certificate validity\n");
                return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
            }
    }

    UTIL_ASN1_GET_NEXT_ITEM_RET(*ppAsn1Cert, asn1Data.itemSize, startAddress, endAddress);

    /* 8. get the subject name and verify it */
    rc = UTIL_X509VerifyStr(ppAsn1Cert, &strSize, startAddress, endAddress);
    if (rc != CC_OK || strSize > X509_SUBJECT_NAME_MAX_STRING_SIZE) {
        CC_PAL_LOG_ERR("Failed to UTIL_X509VerifyStr for subject Name\n");
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }

    if (pOutCertHeaderInfo != NULL){
             UTIL_MemCopy((uint8_t*)pOutCertHeaderInfo->SubjectName, (uint8_t*)*ppAsn1Cert, strSize);
             pOutCertHeaderInfo->SubjectName[strSize]=0;
             pOutCertHeaderInfo->setSubjectName = 1;
    }
    UTIL_ASN1_GET_NEXT_ITEM_RET(*ppAsn1Cert, strSize, startAddress, endAddress);

    /* 9. get the pub key */
    rc = UTIL_X509VerifyPubKey(ppAsn1Cert, pOutPubKey, startAddress, endAddress);
    if (rc != CC_OK) {
        CC_PAL_LOG_ERR("Failed to UTIL_X509VerifyPubKey\n");
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }


    return CC_OK;
}


/*!
 * @brief Parse and validate certificate package header
 *
 * @param[in] pPkgHeader    - pointer to debug certificate package
 * @param[in] pkgType       - certificate package type
 * @param[out] pDbg2CertInfo    - secondary debug certificate offset and size
 * @param[out] pHbkFormat   - HBK format for comparison with OTP
 *
 * @return uint32_t         - On success: the value CC_OK is returned,
 *                    On failure: a value from sbrom_bsv_error.h
 */
CCError_t SB_X509_ParseCertPkg(CCX509PkgHeader_t    *pPkgHeader,
                  CCX509CertType_t      pkgType,
                  CCX509CertInfo_t      *pDbg2CertInfo,
                  CCSbPubKeyIndexType_t     *pHbkFormat)
{
    uint32_t            hbkFormat = 0;

    /* verify Inputs */
    if ((NULL == pPkgHeader) ||
        (NULL == pDbg2CertInfo) ||
        (NULL == pHbkFormat) ||
        ((pkgType >= CC_X509_CERT_TYPE_MAX) || (pkgType <= CC_X509_CERT_TYPE_MIN))) {
        CC_PAL_LOG_ERR("Invalid inputs\n");
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    /* set default outputs */
    pDbg2CertInfo->certInfoWord = 0;
    *pHbkFormat = CC_SB_HASH_BOOT_NOT_USED;

    /* verify certificate package Token */
    if (pPkgHeader->pkgToken != CC_X509_CERT_PKG_TOKEN) {
        CC_PAL_LOG_ERR("Invalid Debug package Token 0x%X\n", pPkgHeader->pkgToken);
        return CC_SB_X509_CERT_ILLEGAL_TOKEN;
    }
    /* verify certificate package Version */
    if (pPkgHeader->pkgVer != CC_X509_CERT_PKG_VERSION) {
        CC_PAL_LOG_ERR("Invalid Debug package Version 0x%X\n", pPkgHeader->pkgVer);
        return CC_SB_X509_CERT_ILLEGAL_VERSION;
    }
    /* verify certificate package certificate type */
    if (pPkgHeader->pkgFlags.pkgFlagsBits.certType != pkgType) {
        CC_PAL_LOG_ERR("Invalid Debug package type 0x%X\n", pPkgHeader->pkgFlags.pkgFlagsBits.certType);
        return CC_SB_X509_CERT_ILLEGAL_CERT_TYPE;
    }
    /* verify certificate package HBK format */
    hbkFormat = pPkgHeader->pkgFlags.pkgFlagsBits.hbkType;
    if ((hbkFormat != CC_SB_HASH_BOOT_KEY_0_128B) &&
        (hbkFormat != CC_SB_HASH_BOOT_KEY_1_128B) &&
        (hbkFormat != CC_SB_HASH_BOOT_KEY_256B) &&
        ((hbkFormat == CC_SB_HASH_BOOT_NOT_USED) && (pkgType != CC_X509_CERT_TYPE_CONTENT))){
        CC_PAL_LOG_ERR("Invalid Debug package HBK type 0x%X\n", hbkFormat);
        return CC_SB_X509_CERT_INV_PARAM;
    }
    *pHbkFormat = (CCSbPubKeyIndexType_t)hbkFormat;

    /* get certificate offset and size */
    pDbg2CertInfo->certInfoWord = pPkgHeader->certInfo.certInfoWord;


    return CC_OK;
}

