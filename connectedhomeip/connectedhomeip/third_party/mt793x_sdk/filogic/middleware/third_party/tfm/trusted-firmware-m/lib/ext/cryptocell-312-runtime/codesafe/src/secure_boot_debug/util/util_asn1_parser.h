/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */



#ifndef UTIL_ASN1_PARSER_H
#define UTIL_ASN1_PARSER_H

#ifdef __cplusplus
extern "C"
{
#endif

/* ASN1 data structure */
typedef struct
{
    uint8_t  tagId;
    uint32_t itemSize;
    uint8_t  index;

}CCSbCertAsn1Data_t;

#define UTILS_ASN1_CERT_VERIFY_PTR_RET(address, startAddress, endAddress){ \
    /* If current address is bigger than endAddress, or if there was a wrapAround and the address is smaller than the address we started with */ \
    if ((address > endAddress) || (address < startAddress)){ \
        CC_PAL_LOG_ERR("Certificate pointer is beyond the allowed limit: addr 0x%lx, start 0x%lx, end 0x%lx\n", address, startAddress, endAddress);\
        return CC_SB_X509_CERT_PARSE_ILLEGAL_VAL;\
    }\
}\


#define UTIL_ASN1_GET_NEXT_ITEM_RET(pAsn1buff, size, startAddress, endAddress){ \
    (pAsn1buff += size); \
    UTILS_ASN1_CERT_VERIFY_PTR_RET((unsigned long)pAsn1buff, startAddress, endAddress); \
}\

/**
 * @brief This function reads ASN1 string and verify its tag
 *
 *
 * @param[in] pInStr - the ASN1 string to read from
 * @param[in] pAsn1Data - output the asn1 fields
 * @param[in] tag - tag to comapre to
 *
 * @return CCError_t - On success the value CC_OK is returned,
 *         on failure - a value from bootimagesverifierx509_error.h
 */
CCError_t UTIL_Asn1ReadItemVerifyTag(uint8_t *pInStr, CCSbCertAsn1Data_t *pAsn1Data, uint8_t tag);

/**
 * @brief This function reads ASN1 string, verify its tag and fw the str pointer
 *
 *
 * @param[in] ppInStr - the ASN1 string to read from
 * @param[in] pAsn1Data - output the asn1 fields
 * @param[in] tag - tag to comapre to
 *
 * @return CCError_t - On success the value CC_OK is returned,
 *         on failure - a value from bootimagesverifierx509_error.h
 */
CCError_t UTIL_Asn1ReadItemVerifyTagFW(uint8_t **ppInStr, CCSbCertAsn1Data_t *pAsn1Data, uint8_t tag,
                       unsigned long startAddress, unsigned long endAddress);

#ifdef __cplusplus
}
#endif

#endif



