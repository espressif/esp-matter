/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _COMMON_UTIL_FILES_H
#define _COMMON_UTIL_FILES_H


#include <stdint.h>

#ifdef WIN32
#define UTILEXPORT_C __declspec(dllexport)
#else
#define UTILEXPORT_C
#endif

#define UTIL_MAX_FILE_NAME  256

/**
 * @brief This function
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return uint8_t -

 */
UTILEXPORT_C int32_t CC_CommonUtilCopyDataFromRawTextFile (uint8_t *fileName, uint8_t *outBuff, uint32_t *outBuffLen);

/**
 * @brief This function
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return uint8_t -

 */
UTILEXPORT_C int32_t CC_CommonUtilCopyDataFromTextFile (uint8_t *fileName, uint8_t *outBuff, uint32_t *outBuffLen);

/**
 * @brief This function
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return uint8_t -

 */
UTILEXPORT_C int32_t CC_CommonUtilCopyDataFromBinFile (uint8_t *fileName, uint8_t *outBuff, uint32_t *outBuffLen);

/**
 * @brief This function copies a buffer to a file
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return uint8_t -

 */
UTILEXPORT_C int32_t CC_CommonUtilCopyBuffToBinFile (uint8_t *fileName, uint8_t *inBuff, uint32_t inBuffLen);


/**
 * @brief The function reads the pwd file name gets the pwd and returns it
 *
 * @param[in] pPwdFileName - file name of the password
 * @param[out] pwd - passphrase data
 *
 */
/*********************************************************/
int32_t CC_CommonGetPassphrase(int8_t *pPwdFileName, uint8_t **pwd);

#endif
