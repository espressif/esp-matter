/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#define CC_PAL_LOG_CUR_COMPONENT CC_LOG_MASK_SECURE_BOOT

/************* Include Files ****************/

#include "secureboot_basetypes.h"
#include "util.h"
#include "secureboot_error.h"
#include "sb_x509_error.h"
#include "util_asn1_parser.h"
#include "cc_pal_log.h"
#include "bootimagesverifier_def.h"

/************************ Defines ******************************/

/************************ Enums ******************************/


/************************ Typedefs ******************************/


/************************ Global Data ******************************/


/************* Private function prototype ****************/


/************************ Private Functions ******************************/
/* The function reads the size of the following item */
static CCError_t UTIL_Asn1ReadItemLength(uint8_t *pInStr, uint32_t *itemLen, uint8_t *index)
{
    uint8_t currVal = 0;
    uint32_t i = 0;

    currVal = *pInStr;

    /* Parsing Item's length according to X.690 Section 8.1.3 */
    if (currVal < 0x80){
        *itemLen = currVal;
        *index = *index + 1;
    }
    else {
        currVal &= 0x7F;
        if ((currVal == 0) || (currVal > sizeof(uint32_t))){
            return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
        }
        else {
            pInStr++;
            /* read the size according to number of bytes */
            *itemLen = 0;
            for (i=0 ; i<currVal ; i++){
                *itemLen = (*itemLen << 8) + (*pInStr++);
            }
            /* update the size of bytes */
            *index = *index + currVal + 1;
        }
    }
    if (*itemLen > CC_SB_MAX_CERT_SIZE_IN_BYTES) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }

    return CC_OK;
}

/* The function reads the ASN1 tag + size and returns it */
CCError_t UTIL_Asn1ReadItemVerifyTag(uint8_t *pInStr, CCSbCertAsn1Data_t *pAsn1Data, uint8_t tag)
{
    CCError_t  error = CC_OK;

    /* Read item id + size */
    pAsn1Data->tagId = *pInStr++;
    if (pAsn1Data->tagId != tag) {
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    pAsn1Data->index = 1;
    error = UTIL_Asn1ReadItemLength(pInStr, &(pAsn1Data->itemSize), &(pAsn1Data->index));

    return error;
}

/* The function reads the ASN1 tag + size and returns it */
CCError_t UTIL_Asn1ReadItemVerifyTagFW(uint8_t **ppInStr, CCSbCertAsn1Data_t *pAsn1Data, uint8_t tag, unsigned long startAddress, unsigned long endAddress)
{
    CCError_t  error = CC_OK;
    uint8_t *tempCertPtr = *ppInStr;

    /* Read item id + size */
    pAsn1Data->tagId = *tempCertPtr++;

    if (pAsn1Data->tagId != tag) {
        CC_PAL_LOG_WARN("Invalid tag 0x%x, expected 0x%x\n", pAsn1Data->tagId, tag);
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;
    }
    pAsn1Data->index = 1;
    error = UTIL_Asn1ReadItemLength(tempCertPtr, &(pAsn1Data->itemSize), &(pAsn1Data->index));
    if (error != CC_OK) {
        CC_PAL_LOG_WARN("Failed UTIL_Asn1ReadItemLength 0x%x\n", error);
        return error;
    }

    *ppInStr = *ppInStr + pAsn1Data->index;
    /* verify that index does not increment the cert pointer to invalid address (beyond the certificate boundaries) */
    UTILS_ASN1_CERT_VERIFY_PTR_RET((unsigned long)*ppInStr, startAddress, endAddress);

    return error;
}

