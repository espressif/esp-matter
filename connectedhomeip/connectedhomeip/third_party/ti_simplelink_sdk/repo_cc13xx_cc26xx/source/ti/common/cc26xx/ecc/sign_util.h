/******************************************************************************

 @file  sign_util.h

 @brief This module contains utility functions for sign verification

 Group: CMCU
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2012-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

#ifndef SIGN_UTIL_H
#define SIGN_UTIL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 *                                          Includes
 */

#include "oad_image_header.h"
#include "crc32.h"


/*********************************************************************
 * GLOBAL VARIABLES
 */
/* ECC ROM global window size and workzone buffer. */
#if !defined(DeviceFamily_CC26X2) && !defined(DeviceFamily_CC13X2) && !defined(DeviceFamily_CC13X2X7) && !defined(DeviceFamily_CC26X2X7)
extern uint32_t *eccRom_workzone;
#else
extern uint8_t eccRom_windowSize;
extern uint32_t *eccRom_workzone;

/* ECC ROM global parameters */
extern uint32_t  *eccRom_param_p;
extern uint32_t  *eccRom_param_r;
extern uint32_t  *eccRom_param_a;
extern uint32_t  *eccRom_param_b;
extern uint32_t  *eccRom_param_Gx;
extern uint32_t  *eccRom_param_Gy;

/* NIST P-256 Curves in ROM
 Note: these are actually strings*/
extern uint32_t NIST_Curve_P256_p;
extern uint32_t NIST_Curve_P256_r;
extern uint32_t NIST_Curve_P256_a;
extern uint32_t NIST_Curve_P256_b;
extern uint32_t NIST_Curve_P256_Gx;
extern uint32_t NIST_Curve_P256_Gy;
#endif /* !DeviceFamily_CC26X2 && !DeviceFamily_CC13X2 && !DeviceFamily_CC13X2X7 && !DeviceFamily_CC26X2X7*/

/*********************************************************************
 * MACROS
 */

/*! Length of Sign */
#define SECURE_FW_SIGN_LEN 32

/*!< Success Return Code      */
#define SECURE_FW_STATUS_SUCCESS                              0
/*! Failure return code */
#define SECURE_FW_STATUS_FAIL                                 1
/*!< Illegal parameter        */
#define SECURE_FW_STATUS_ILLEGAL_PARAM                       -1
/*! Invalid ECC Signature         */
#define SECURE_FW_ECC_STATUS_INVALID_SIGNATURE             0x5A
/*! ECC Signature Successfully Verified  */
#define SECURE_FW_ECC_STATUS_VALID_SIGNATURE               0xA5

/* ECC Window Size.  Determines speed and workzone size of ECC operations.
 Recommended setting is 3. */
#define SECURE_FW_ECC_WINDOW_SIZE                3

/* Key size in uint32_t blocks */
#define SECURE_FW_ECC_UINT32_BLK_LEN(len)        (((len) + 3) / 4)

/* Offset value for number of bytes occupied by length field */
#define SECURE_FW_ECC_KEY_OFFSET                 4

/* Offset of Key Length field */
#define SECURE_FW_ECC_KEY_LEN_OFFSET             0

/* Total buffer size */
#define SECURE_FW_ECC_BUF_TOTAL_LEN(len)         ((len) + SECURE_FW_ECC_KEY_OFFSET)

/*!
 * ECC Workzone length in bytes for NIST P-256 key and shared secret generation.
 * For use with ECC Window Size 3 only.  Used to store intermediary values in
 * ECC calculations.
 */
#define BIM_ECC_NIST_P256_WORKZONE_LEN_IN_BYTES          1100

#ifndef SECURE_FW_ECC_NIST_P256_WORKZONE_LEN_IN_BYTES
    #define SECURE_FW_ECC_NIST_P256_WORKZONE_LEN_IN_BYTES (BIM_ECC_NIST_P256_WORKZONE_LEN_IN_BYTES)
#endif

/*!
 * ECC key length in bytes for NIST P-256 keys.
 */
#define SECURE_FW_ECC_NIST_P256_KEY_LEN_IN_BYTES                32

/* Cert element length for ECC- size of secure_fw_cert_element_t*/
#define SECURE_CERT_LENGTH    sizeof(certElement_t)
#define SECURE_CERT_OPTIONS   0x0000
#define SECURE_SIGN_TYPE          1

/*******************************************************************************
 * Typedefs
 */


/*! ECC public key pair */
typedef struct {
  uint8_t pubKeyX[32];
  uint8_t pubKeyY[32];
} eccKey_t;

/*! Cert Payload */
typedef union {
  eccKey_t eccKey;
  uint8_t  aesKey[2];
} certPayload_t;

/*! Cert Element */
TYPEDEF_STRUCT_PACKED
{
  uint8_t  version;
  uint8_t  len;
  uint16_t options;
  uint8_t signerInfo[8];
  certPayload_t certPayload;
} certElement_t;





/*! ECC NIST Curve Parameters */
typedef struct ECCROMCC26XX_CurveParams
{
    /*!<  Length in bytes of curve parameters and keys */
    uint8_t         keyLen;
    /*!<  Length in bytes of workzone to allocate      */
    uint16_t        workzoneLen;
    /*!<  Window size of operation                     */
    uint8_t         windowSize;
    /*!<  ECC Curve Parameter P                        */
    uint32_t        *param_p;
    /*!<  ECC Curve Parameter R                        */
    uint32_t        *param_r;
    /*!<  ECC Curve Parameter A                        */
    uint32_t        *param_a;
    /*!<  ECC Curve Parameter B                        */
    uint32_t        *param_b;
    /*!<  ECC Curve Parameter Gx                       */
    uint32_t        *param_gx;
    /*!<  ECC Curve Parameter Gy                       */
    uint32_t        *param_gy;
} ECCROMCC26XX_CurveParams;

typedef struct ECCROMCC26XX_Params {
    ECCROMCC26XX_CurveParams curve;   /*!< ECC Curve Parameters   */
    int8_t                   status;  /*!< stored return status   */
} ECCROMCC26XX_Params;

void eccInit(ECCROMCC26XX_Params *pParams);

extern uint8_t verifyCertElement(uint8_t *signerInfo);
extern uint8_t *computeSha2Hash(uint32_t imgStartAddr, uint8_t *SHABuff,
                                uint16_t SHABuffLen, bool useExtFl);

extern uint8_t  bimVerifyImage_ecc(const uint8_t *publicKey_x, const uint8_t *publicKey_y,
                                uint8_t *hash, uint8_t *sign1, uint8_t *sign2,
                                uint32_t *eccWorkzone, uint8_t *tempWorkzone);


#ifdef __cplusplus
}
#endif

#endif /* SIGN_UTIL_H */
