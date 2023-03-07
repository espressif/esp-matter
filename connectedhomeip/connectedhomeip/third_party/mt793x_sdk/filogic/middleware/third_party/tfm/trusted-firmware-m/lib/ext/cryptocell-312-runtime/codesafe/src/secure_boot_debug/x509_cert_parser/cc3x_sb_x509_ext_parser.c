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
#include "bsv_error.h"
#include "sb_x509_cert_parser.h"
#include "cc3x_sb_x509_ext_parser.h"
#include "sb_x509_error.h"
#include "bootimagesverifierx509_error.h"
#include "util_asn1_parser.h"
#include "util_base64.h"
#include "util_x509_parser.h"
#include "cc_pal_log.h"
//#include "bootimagesverifierx509_def.h"
#include "bootimagesverifier_def.h"
#include "cc_crypto_x509_defs.h"

/*!
 * @brief Parse certificate extension segment
 *
 * @param[in/out] ppCert    - pointer to X509 certificate as ASN.1 byte array
 * @param[in] certType      - certificate type
 * @param[in] cntNumOfImg   - number of images for content certificate (should be 0 for all else)
 * @param[out] pOutStr      - extension data structure according to certificate type
 * @param[in] pOutStrSize   - extension data structure max size
 * @param[in] maxCertSize   - max certficate size
 * @param[in] startAddress      - start address of certificate
 * @param[in] endAddress    - end address of certificate (the certificate pointer cannot exceed this address)
 *
 * @return uint32_t         - On success: the value CC_OK is returned,
 *                    On failure: a value from bsv_error.h
 */
CCError_t SB_X509_ParseCertExtensions(uint8_t       **ppCert,
                        uint32_t    certSize,
                    CCSbCertHeader_t **ppCertPropHeader,
                        uint8_t     **ppNp,
                    uint8_t     **ppCertBody,
                    uint32_t    *pCertBodySize,
                    unsigned long   startAddress,
                    unsigned long   endAddress)
{
    CCError_t rc = CC_OK;
    CCSbCertAsn1Data_t asn1Data;
    uint32_t nextBuffOffset = 0;
    uint32_t extNum = 0;
    uint8_t extVal = 0;
    uint8_t **ppExtBuff;
    uint32_t extBuffSize = 0;


    /* validate inputs */
    if ((ppCert == NULL) ||
        (certSize == 0) ||
        (ppCertPropHeader == NULL) ||
        (ppNp == NULL) ||
        (ppCertBody == NULL) ||
        (pCertBodySize == NULL)){
         CC_PAL_LOG_ERR("Invalid inputs\n");
        return CC_BSV_ILLEGAL_INPUT_PARAM_ERR;
    }

    /* parse certificate extension header */
    /* the first tag is always the extension tag */
    rc = UTIL_Asn1ReadItemVerifyTagFW(ppCert, &asn1Data, CC_X509_CERT_CTX_EXT_TAG_ID, startAddress, endAddress);
    if (rc != CC_OK) {
         CC_PAL_LOG_ERR("1. Failed to read CC_X509_CERT_CTX_EXT_TAG_ID \n");
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    /* read SEQ */
    rc = UTIL_Asn1ReadItemVerifyTagFW(ppCert, &asn1Data, CC_X509_CERT_SEQ_TAG_ID, startAddress, endAddress);
    if (rc != CC_OK) {
         CC_PAL_LOG_ERR("2. Failed to read CC_X509_CERT_SEQ_TAG_ID \n");
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }

    for (extNum=0; extNum < CC3X_X509_CERT_EXT_NUMBER && nextBuffOffset < certSize; extNum++){
        /* read SEQ */
        rc = UTIL_Asn1ReadItemVerifyTagFW(ppCert, &asn1Data, CC_X509_CERT_SEQ_TAG_ID, startAddress, endAddress);
        if (rc != CC_OK) {
             CC_PAL_LOG_ERR("3. Failed to read CC_X509_CERT_SEQ_TAG_ID\n");
            return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
        }
        nextBuffOffset += asn1Data.itemSize;
        /* read OBJ ID */
        rc = UTIL_Asn1ReadItemVerifyTagFW(ppCert, &asn1Data, CC_X509_CERT_OBJ_IDENTIFIER_TAG_ID, startAddress, endAddress);
        if (rc != CC_OK) {
             CC_PAL_LOG_ERR("3. Failed to read CC_X509_CERT_SEQ_TAG_ID\n");
            return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
        }
        /* the last byte of the identifier , identifies the extension field */
        if (asn1Data.itemSize == 0){
            return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
        }
        extVal = *((*ppCert) + asn1Data.itemSize -1);
        UTIL_ASN1_GET_NEXT_ITEM_RET(*ppCert, asn1Data.itemSize, startAddress, endAddress);

        rc = UTIL_Asn1ReadItemVerifyTagFW(ppCert, &asn1Data, CC_X509_CERT_BOOL_TAG_ID, startAddress, endAddress);
        if (rc != CC_OK) {
             CC_PAL_LOG_ERR("5. Failed to read CC_X509_CERT_BOOL_TAG_ID\n");
            return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
        }
        UTIL_ASN1_GET_NEXT_ITEM_RET(*ppCert, asn1Data.itemSize, startAddress, endAddress);

        switch(extNum) {
        case 0:
            if (extVal != CC_X509_ID_EXT_PROPRIETARY_HEADER) {
                 CC_PAL_LOG_ERR("Illegal extension %d, expected Header\n", extVal);
                return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
            }

            ppExtBuff = (uint8_t **)ppCertPropHeader;
            extBuffSize = sizeof(CCSbCertHeader_t);
            break;
        case 1:
            if (extVal != CC_X509_ID_EXT_PUB_KEY_NP) {
                 CC_PAL_LOG_ERR("Illegal extension %d, expected Np\n", extVal);
                return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
            }
            ppExtBuff = (uint8_t **)ppNp;
            extBuffSize = RSA_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_BYTES;
            break;
        case 2:
            if ((extVal != CC_X509_ID_EXT_KEY_CERT_MAIN_VAL) &&
                (extVal != CC_X509_ID_EXT_CONTENT_CERT_MAIN_VAL) &&
                (extVal != CC_X509_ID_EXT_ENABLER_CERT_MAIN_VAL) &&
                (extVal != CC_X509_ID_EXT_DEVELOPER_CERT_MAIN_VAL)) {
                 CC_PAL_LOG_ERR("Illegal extension %d, expected key, content, enabler, developer\n", extVal);
                return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
            }
            ppExtBuff = (uint8_t **)ppCertBody;
            extBuffSize = *pCertBodySize;
            break;
        default:
            CC_PAL_LOG_ERR("Invalid loop %d\n", extNum);
            return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
        }

        rc = UTIL_Asn1ReadItemVerifyTagFW(ppCert, &asn1Data, CC_X509_CERT_OCT_STR_TAG_ID, startAddress, endAddress);
        if (rc != CC_OK) {
            CC_PAL_LOG_ERR("Failed to verify CC_X509_CERT_OCT_STR_TAG_ID, rc 0x%X\n", rc);
            return rc;
        }
        /* in case we have leading 0 before buffer */
        if ((asn1Data.itemSize > extBuffSize) && (**ppCert == 0)) {
            UTIL_ASN1_GET_NEXT_ITEM_RET(*ppCert, 1, startAddress, endAddress);
            asn1Data.itemSize--;
        } else if (asn1Data.itemSize > extBuffSize) {
             CC_PAL_LOG_ERR("Invalid inputs > extBuffSize 0x%x\n", extBuffSize);
            return CC_BSV_ILLEGAL_INPUT_PARAM_ERR;
        }

        *ppExtBuff = (uint8_t*)*ppCert;
        if (extNum == 2) {
            *pCertBodySize = asn1Data.itemSize;
        }

        UTIL_ASN1_GET_NEXT_ITEM_RET(*ppCert, asn1Data.itemSize, startAddress, endAddress);
    }

    return CC_OK;
}

