/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef  _CC_UTIL_CMAC_H
#define  _CC_UTIL_CMAC_H

#include "cc_util_int_defs.h"

/*! Defines the CMAC result buffer. */
typedef uint8_t CCUtilAesCmacResult_t[CC_UTIL_AES_CMAC_RESULT_SIZE_IN_BYTES];

CCUtilError_t UtilCmacBuildDataForDerivation( const uint8_t               *pLabel,
                                             size_t                      labelSize,
                                             const uint8_t               *pContextData,
                                             size_t                      contextSize,
                                             uint8_t            *pDataIn,
                                             size_t                 *pDataInSize,
                                             size_t                      derivedKeySize);
/*!
 * This function is used to generate bytes stream for key derivation purposes.
 * The function gets an input data and can use use one of the following keys: KDR/Session/userKey.
 *
 * @param[in] keyType       - UTIL_USER_KEY / UTIL_ROOT_KEY
 * @param[in] pUserKey      - A pointer to the user's key buffer (case of CC_UTIL_USER_KEY).
 * @param[in] pDataIn       - A pointer to input buffer.
 * @param[in] dataInSize    - Size of data in bytes.
 * @param[out] pCmacResult  - A pointer to output buffer 16 bytes array.
 *
 * @return CC_UTIL_OK on success, otherwise failure
 *
 */
CCUtilError_t UtilCmacDeriveKey(UtilKeyType_t       keyType,
                CCAesUserKeyData_t      *pUserKey,
                uint8_t         *pDataIn,
                size_t                  dataInSize,
                CCUtilAesCmacResult_t   pCmacResult);


#endif /* _CC_UTIL_CMAC_H */
