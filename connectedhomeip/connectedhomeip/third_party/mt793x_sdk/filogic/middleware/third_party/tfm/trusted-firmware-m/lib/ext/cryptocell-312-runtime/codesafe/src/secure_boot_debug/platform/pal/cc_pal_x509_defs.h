/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
@file
@brief This file contains user-defined functions, definitions and TBS header struct.
*/

#ifndef _CC_PAL_X509_DEFS_H
#define _CC_PAL_X509_DEFS_H



#ifdef __cplusplus
extern "C"
{
#endif

#include "cc_crypto_x509_common_defs.h"

#define CC_CERT_USER_DATA_KEY_OFFSET        0
#define CC_CERT_USER_DATA_PRIM_OFFSET       1
#define CC_CERT_USER_DATA_SECOND_OFFSET     2

/*!
@brief This function checks validity period and should be implemented by customer.
       It receives start and end validity period as input. It also receives an indication flag for each period. If the flag is not 1,
       the value of current period was not defined by the user.

@return CC_OK On success.
@return A non-zero value from sbrom_bsv_error.h on failure.
*/
CCError_t CC_PalVerifyCertValidity(char *pNotBeforeStr, /*!< [in] Pointer to the start period string. */
    uint32_t notBeforeStrSize, /*!< [in] Size of the start period string. */
    uint8_t notBeforeStrFlag, /*!< [in] Start period definition flag indication. */
    char *pNotAfterStr,/*!< [in] Pointer to the end period string. */
    uint32_t notAfterStrSize,/*!< [in] Size of the end period string. */
    uint8_t notAfterStrFlag); /*!< [in] Start period definition Flag indication. */



/*! x509 Certificate user's data. This data is outputed after the certificate passed validation */
typedef struct {
    uint8_t   setSerialNum;                                   /*! Definition flag of certificate serial number. */
    uint32_t  serialNum;                                      /*! Value of  certificate serial number. */
    uint8_t   setIssuerName;                                  /*! Definition flag of certificate issuer name. */
    char      IssuerName[X509_ISSUER_NAME_MAX_STRING_SIZE+1];   /*! String of certificate issuer name. */
    uint8_t   setSubjectName;                                 /*! Definition flag of certificate subject name. */
    char      SubjectName[X509_SUBJECT_NAME_MAX_STRING_SIZE+1]; /*! String of certificate subject name. */
    uint8_t   setNotBeforeStr;                                /*! Definition flag of start validity period.  */
    char      NotBeforeStr[X509_VALIDITY_PERIOD_MAX_STRING_SIZE+1]; /*! String of start validity period. */
    uint8_t   setNotAfterStr;                                     /*! Definition flag of end validity period.  */
    char      NotAfterStr[X509_VALIDITY_PERIOD_MAX_STRING_SIZE+1]; /*! String of end validity period. */
#ifdef CC_SB_CERT_USER_DATA_EXT
    uint8_t   userData[X509_USER_DATA_MAX_SIZE_BYTES*3];           /*! Byte array containing the user's data from the certificate, only valid if the ROM was
                                       compiled with CC_CONFIG_SB_CERT_USER_DATA_EXT = 1.
                                       This structure is used by secure debug and secure boot. In case of secure debug
                                       there are 3 buffers of user's data: key, primary debug, secondary debug*/
#endif
}CCX509CertHeaderInfo_t;


#ifdef __cplusplus
}
#endif

#endif


