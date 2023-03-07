/*
 * Copyright (c) 2020, Qorvo Inc
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Change$
 * $DateTime$
 */

/** @file "qvCHIP_KVS.h"
 *
 *  CHIP wrapper KVS API
 *
 *  Declarations of the KVS specific public functions and enumerations of qvCHIP.
*/

#ifndef _QVCHIP_FACTORYDATA_H_
#define _QVCHIP_FACTORYDATA_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include <stdint.h>

/*****************************************************************************
 *                    Enum Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/** @brief TLV Tag IDs for various factory data */
typedef enum {
    TAG_ID_END_MARKER = 0,
    /* DeviceAttestationCredentialsProvider */
    TAG_ID_CERTIFICATION_DECLARATION = 1,
    TAG_ID_FIRMWARE_INFORMATION = 2,
    TAG_ID_DEVICE_ATTESTATION_CERTIFICATE = 3,
    TAG_ID_PRODUCT_ATTESTATION_INTERMEDIATE_CERTIFICATE = 4,
    TAG_ID_DEVICE_ATTESTATION_PRIVATE_KEY = 5,
    TAG_ID_DEVICE_ATTESTATION_PUBLIC_KEY = 6,
    /* CommissionableDataProvider */
    TAG_ID_SETUP_DISCRIMINATOR = 15,
    TAG_ID_SPAKE2_ITERATION_COUNT = 16,
    TAG_ID_SPAKE2_SALT = 17,
    TAG_ID_SPAKE2_VERIFIER = 18,
    TAG_ID_SPAKE2_SETUP_PASSCODE = 19,
    /* DeviceInstanceInfoProvider */
    TAG_ID_VENDOR_NAME = 25,
    TAG_ID_VENDOR_ID = 26,
    TAG_ID_PRODUCT_NAME = 27,
    TAG_ID_PRODUCT_ID = 28,
    TAG_ID_SERIAL_NUMBER = 29,
    TAG_ID_MANUFACTURING_DATE = 30,
    TAG_ID_HARDWARE_VERSION = 31,
    TAG_ID_HARDWARE_VERSION_STRING = 32,
    TAG_ID_ROTATING_DEVICE_ID = 33,
    /* Platform specific */
    TAG_ID_ENABLE_KEY = 40
} qvCHIP_FactoryDataTagId_t;

/** @brief Expected TLV lengths for certain factory data */
typedef enum {
    CERTIFICATION_DECLARATION_LEN = 541,
    DEVICE_ATTESTATION_CERTIFICATE_LEN = 600,
    PRODUCT_ATTESTATION_INTERMEDIATE_CERTIFICATE_LEN = 600,
    DEVICE_ATTESTATION_PRIVATE_KEY_LEN = 32,
    DEVICE_ATTESTATION_PUBLIC_KEY_LEN = 65,
    SETUP_DISCRIMINATOR_LEN = 2,
    SPAKE2_ITERATION_COUNT_LEN = 4,
    SPAKE2_SALT_LEN = 32,
    SPAKE2_VERIFIER_LEN = 98,
    SETUP_PASSCODE_LEN = 4,
    VENDOR_ID_LEN = 2,
    PRODUCT_ID_LEN = 2,
    MANUFACTURING_DATE_LEN = 4,
    HARDWARE_VERSION_LEN = 2,
    ROTATING_DEVICE_ID_UNIQUE_ID_LEN = 32,
    ENABLE_KEY_LEN = 16
} qvCHIP_FactoryDataMaxLength_t;

/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 *                    NVM API
 *****************************************************************************/
/** @brief Get Factory data
 *  @param tag             ID of the tag in the TLV structure holding the data of interest
 *  @param dst             pointer to destination data
 *  @param buffer_size     size of the destination buffer
 *  @param out data_length the number of bytes written to the buffer
 *  @return                QV_STATUS_INVALID_ARGUMENT if data is NULL,
 *                         QV_STATUS_BUFFER_TOO_SMALL if the returned data is too large to fit the buffer,
 *                         QV_STATUS_INVALID_DATA if the factory data is missing,
 *                         QV_STATUS_NO_ERROR otherwise
*/
qvStatus_t qvCHIP_FactoryDataGetValue(qvCHIP_FactoryDataTagId_t tag, uint8_t* dst, uint32_t buffer_size, uint32_t* data_length);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_QVCHIP_FACTORYDATA_H_
