/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define CC_PAL_LOG_CUR_COMPONENT CC_LOG_MASK_SECURE_BOOT

/************* Include Files ****************/

#include "secureboot_basetypes.h"
#include "secureboot_stage_defs.h"
#include "util_asn1_parser.h"
#include "util_x509_parser.h"
#include "secureboot_error.h"
#include "sb_x509_error.h"
#include "rsa_bsv.h"
#include "cc_pal_log.h"
#include "cc_pka_hw_plat_defs.h"


/************************ Defines ******************************/


/************************ Enums ******************************/


/************************ Typedefs ******************************/


/************************ Global Data ******************************/
const uint8_t *certType2SubjectNames[CC_X509_CERT_TYPE_MAX] = {
  /*CC_X509_CERT_TYPE_MIN      */   (uint8_t *)NULL,
  /*CC_X509_CERT_TYPE_KEY      */   (uint8_t *)CC_X509_CERT_KEY_CERT,
  /*CC_X509_CERT_TYPE_CONTENT  */   (uint8_t *)CC_X509_CERT_CNT_CERT,
  /*CC_X509_CERT_TYPE_ENABLER_DBG */    (uint8_t *)CC_X509_CERT_ENABLER_CERT,
  /*CC_X509_CERT_TYPE_DEVELOPER_DBG */  (uint8_t *)CC_X509_CERT_DEVELOPER_CERT
};

/************* Private function prototype ****************/


/************************ Private Functions ******************************/
/* the following function verify the ASN1 tags sequences in case of strings (Issuer name and subject name) */
CCError_t UTIL_X509VerifyStr(uint8_t **pCert, uint32_t *dataSize, unsigned long startAddress, unsigned long endAddress)
{
    CCError_t error = CC_OK;
    CCSbCertAsn1Data_t asn1Data;

    /* read SEQ */
    error = UTIL_Asn1ReadItemVerifyTagFW(pCert, &asn1Data, CC_X509_CERT_SEQ_TAG_ID, startAddress, endAddress);
    if (error != CC_OK) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    /* read SET */
    error = UTIL_Asn1ReadItemVerifyTagFW(pCert, &asn1Data, CC_X509_CERT_SET_OF_TAG_ID, startAddress, endAddress);
    if (error != CC_OK) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    /* read SEQ */
    error = UTIL_Asn1ReadItemVerifyTagFW(pCert, &asn1Data, CC_X509_CERT_SEQ_TAG_ID, startAddress, endAddress);
    if (error != CC_OK) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    /* OBJ ID */
    error = UTIL_Asn1ReadItemVerifyTagFW(pCert, &asn1Data, CC_X509_CERT_OBJ_IDENTIFIER_TAG_ID, startAddress, endAddress);
    if (error != CC_OK) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }

    UTIL_ASN1_GET_NEXT_ITEM_RET(*pCert, asn1Data.itemSize, startAddress, endAddress)

    /* PRINT STR ID */
    error = UTIL_Asn1ReadItemVerifyTagFW(pCert, &asn1Data, CC_X509_CERT_PRNT_STR_TAG_ID, startAddress, endAddress);
    if (error != CC_OK) {
        error = UTIL_Asn1ReadItemVerifyTagFW(pCert, &asn1Data, CC_X509_CERT_UTF8_TAG_ID, startAddress, endAddress);
        if (error != CC_OK) {
            return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
        }
    }
    *dataSize = asn1Data.itemSize;

    return error;
}

CCError_t UTIL_X509VerifyIssuerName(uint8_t *pCert, uint32_t size)
{
    CCError_t error = CC_OK;

    error = UTIL_MemCmp(pCert, (uint8_t*)CC_X509_CERT_ISSUER_NAME, size);
    if (error != CC_TRUE){
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    return CC_OK;
}


/* the following function verify the subject name */
CCError_t UTIL_X509VerifySubjectName(uint8_t *pCert, CCX509CertType_t certType, uint32_t subNameSize)
{
    CCError_t error = CC_OK;
    /* validate inputs */
    if ((NULL == pCert) ||
        ((certType >= CC_X509_CERT_TYPE_MAX) || (certType <= CC_X509_CERT_TYPE_MIN))) {
        CC_PAL_LOG_ERR("Invalid inputs\n");
        return CC_SB_X509_CERT_INV_PARAM;
    }

    error = UTIL_MemCmp(pCert,(uint8_t*)certType2SubjectNames[certType],subNameSize);
    if(error != CC_TRUE)
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;

    return CC_OK;
}


/* the following function verify the ASN1 tags sequences in case of strings (Issuer name and subject name) */
CCError_t UTIL_X509VerifyPubKey(uint8_t **pCert, CCSbNParams_t *pParamsN, unsigned long startAddress, unsigned long endAddress)
{
    CCError_t error = CC_OK;
    CCSbCertAsn1Data_t asn1Data;
    uint8_t objId[] = CC_X509_CERT_RSASSAENC_ID;
    uint8_t eVal[] = X509_RSA_E_VAL_IN_BYTES;

    /* read SEQ */
    error = UTIL_Asn1ReadItemVerifyTagFW(pCert, &asn1Data, CC_X509_CERT_SEQ_TAG_ID, startAddress, endAddress);
    if (error != CC_OK)
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    /* read SEQ */
    error = UTIL_Asn1ReadItemVerifyTagFW(pCert, &asn1Data, CC_X509_CERT_SEQ_TAG_ID, startAddress, endAddress);
    if (error != CC_OK)
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    /* OBJ ID */
    error = UTIL_Asn1ReadItemVerifyTagFW(pCert, &asn1Data, CC_X509_CERT_OBJ_IDENTIFIER_TAG_ID, startAddress, endAddress);
    if ((error != CC_OK) || (asn1Data.itemSize != sizeof(objId))) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    /* verify ID */
    if ((error = UTIL_MemCmp(*pCert, objId, asn1Data.itemSize)) != CC_TRUE){
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    UTIL_ASN1_GET_NEXT_ITEM_RET(*pCert, asn1Data.itemSize, startAddress, endAddress);

    /* read NULL */
    error = UTIL_Asn1ReadItemVerifyTagFW(pCert, &asn1Data, CC_X509_CERT_NULL_TAG_ID, startAddress, endAddress);
    if (error != CC_OK)
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    UTIL_ASN1_GET_NEXT_ITEM_RET(*pCert, asn1Data.itemSize, startAddress, endAddress);
    /* BIT Str */
    error = UTIL_Asn1ReadItemVerifyTag(*pCert, &asn1Data, CC_X509_CERT_BIT_STR_TAG_ID);
    if (error != CC_OK)
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    UTIL_ASN1_GET_NEXT_ITEM_RET(*pCert, asn1Data.index+1, startAddress, endAddress);//add 1 for unused bits
    /* SEQ */
    error = UTIL_Asn1ReadItemVerifyTagFW(pCert, &asn1Data, CC_X509_CERT_SEQ_TAG_ID, startAddress, endAddress);
    if (error != CC_OK)
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    /* read INT and copy the N into buffer */
    error = UTIL_Asn1ReadItemVerifyTag(*pCert, &asn1Data, CC_X509_CERT_INT_TAG_ID);
    if (error != CC_OK)
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    if (asn1Data.itemSize != SB_CERT_RSA_KEY_SIZE_IN_BYTES){
        if (asn1Data.itemSize != (SB_CERT_RSA_KEY_SIZE_IN_BYTES + 1)){
            return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
        }
        asn1Data.index ++;
    }
    UTIL_ASN1_GET_NEXT_ITEM_RET(*pCert, asn1Data.index, startAddress, endAddress);
    UTIL_MemCopy((uint8_t*)pParamsN->N, *pCert, SB_CERT_RSA_KEY_SIZE_IN_BYTES);
    UTIL_ASN1_GET_NEXT_ITEM_RET(*pCert, asn1Data.itemSize-1, startAddress, endAddress);

    /* Verify E is the expected constant */
    error = UTIL_Asn1ReadItemVerifyTagFW(pCert, &asn1Data, CC_X509_CERT_INT_TAG_ID, startAddress, endAddress);
    if (error != CC_OK)
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;

    if (asn1Data.itemSize != sizeof(eVal)) /* verify the size of E is correct */
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;

    if ((error = UTIL_MemCmp(*pCert, eVal, asn1Data.itemSize)) != CC_TRUE){
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    UTIL_ASN1_GET_NEXT_ITEM_RET(*pCert, asn1Data.itemSize, startAddress, endAddress);

    return CC_OK;
}

/* the following function retrieves the signature */
CCError_t UTIL_X509GetSignature(uint8_t **pCert, CCSbSignature_t *signatureP, unsigned long startAddress, unsigned long endAddress)
{
    CCError_t error = CC_OK;
    CCSbCertAsn1Data_t asn1Data;
    uint8_t objId[] = CC_X509_CERT_SHA256RSAPSS_ID;
    uint8_t objSha256Id[] = CC_X509_CERT_SHA256_ID;
    uint8_t objMgf1Id[] = CC_X509_CERT_MGF1_ID;

    /* read SEQ */
    error = UTIL_Asn1ReadItemVerifyTagFW(pCert, &asn1Data, CC_X509_CERT_SEQ_TAG_ID, startAddress, endAddress);
    if (error != CC_OK) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    /* OBJ ID */
    error = UTIL_Asn1ReadItemVerifyTagFW(pCert, &asn1Data, CC_X509_CERT_OBJ_IDENTIFIER_TAG_ID, startAddress, endAddress);
    if (error != CC_OK) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    if (asn1Data.itemSize != sizeof(objId)) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    /* verify ID */
    if ((error = UTIL_MemCmp(*pCert, objId, asn1Data.itemSize)) != CC_TRUE){
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    UTIL_ASN1_GET_NEXT_ITEM_RET(*pCert, asn1Data.itemSize, startAddress, endAddress);

    /* verify sha256 + PSS + mgf1 attributes signature */
    error = UTIL_Asn1ReadItemVerifyTagFW(pCert, &asn1Data, CC_X509_CERT_SEQ_TAG_ID, startAddress, endAddress);
    if (error != CC_OK) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    error = UTIL_Asn1ReadItemVerifyTagFW(pCert, &asn1Data, CC_X509_CERT_CTX_SPEC_TAG_ID, startAddress, endAddress);
    if (error != CC_OK) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    error = UTIL_Asn1ReadItemVerifyTagFW(pCert, &asn1Data, CC_X509_CERT_SEQ_TAG_ID, startAddress, endAddress);
    if (error != CC_OK) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    /* verify sha256 */
    error = UTIL_Asn1ReadItemVerifyTagFW(pCert, &asn1Data, CC_X509_CERT_OBJ_IDENTIFIER_TAG_ID, startAddress, endAddress);
    if (error != CC_OK) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    if (asn1Data.itemSize != sizeof(objSha256Id)) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    if ((error = UTIL_MemCmp(*pCert, objSha256Id, asn1Data.itemSize)) != CC_TRUE){
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    UTIL_ASN1_GET_NEXT_ITEM_RET(*pCert, asn1Data.itemSize, startAddress, endAddress);
    /* verify mgf1 + sha256 */
    error = UTIL_Asn1ReadItemVerifyTagFW(pCert, &asn1Data, CC_X509_CERT_CTX_SPEC_TAG1_ID, startAddress, endAddress);
    if (error != CC_OK) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    error = UTIL_Asn1ReadItemVerifyTagFW(pCert, &asn1Data, CC_X509_CERT_SEQ_TAG_ID, startAddress, endAddress);
    if (error != CC_OK) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    /* verify mgf1 */
    error = UTIL_Asn1ReadItemVerifyTagFW(pCert, &asn1Data, CC_X509_CERT_OBJ_IDENTIFIER_TAG_ID, startAddress, endAddress);
    if (error != CC_OK) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    if (asn1Data.itemSize != sizeof(objMgf1Id)) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    if ((error = UTIL_MemCmp(*pCert, objMgf1Id, asn1Data.itemSize)) != CC_TRUE){
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }

    UTIL_ASN1_GET_NEXT_ITEM_RET(*pCert, asn1Data.itemSize, startAddress, endAddress);
    error = UTIL_Asn1ReadItemVerifyTagFW(pCert, &asn1Data, CC_X509_CERT_SEQ_TAG_ID, startAddress, endAddress);
    if (error != CC_OK) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    /* verify sha256 */
    error = UTIL_Asn1ReadItemVerifyTagFW(pCert, &asn1Data, CC_X509_CERT_OBJ_IDENTIFIER_TAG_ID, startAddress, endAddress);
    if (error != CC_OK) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    if (asn1Data.itemSize != sizeof(objSha256Id)) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    if ((error = UTIL_MemCmp(*pCert, objSha256Id, asn1Data.itemSize)) != CC_TRUE){
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    UTIL_ASN1_GET_NEXT_ITEM_RET(*pCert, asn1Data.itemSize, startAddress, endAddress);

    /* verify last special tag size */
    error = UTIL_Asn1ReadItemVerifyTagFW(pCert, &asn1Data, CC_X509_CERT_CTX_SPEC_TAG2_ID, startAddress, endAddress);
    if (error != CC_OK) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    if (asn1Data.itemSize != CC_X509_CERT_CTX_SPEC_TAG2_SIZE){
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    UTIL_ASN1_GET_NEXT_ITEM_RET(*pCert, asn1Data.itemSize, startAddress, endAddress);
    /* BIT Str */
    error = UTIL_Asn1ReadItemVerifyTag(*pCert, &asn1Data, CC_X509_CERT_BIT_STR_TAG_ID);
    if (error != CC_OK) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    if (asn1Data.itemSize != (SB_CERT_RSA_KEY_SIZE_IN_BYTES + 1)){
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    UTIL_ASN1_GET_NEXT_ITEM_RET(*pCert, asn1Data.index + 1, startAddress, endAddress);//add 1 for unused bits
    // copy the signature as reversed buffer
    UTIL_ReverseMemCopy((uint8_t*)signatureP->sig, *pCert, SB_CERT_RSA_KEY_SIZE_IN_BYTES);
    UTIL_ASN1_GET_NEXT_ITEM_RET(*pCert, asn1Data.itemSize-1, startAddress, endAddress);//add 1 for unused bits

    return error;
}
