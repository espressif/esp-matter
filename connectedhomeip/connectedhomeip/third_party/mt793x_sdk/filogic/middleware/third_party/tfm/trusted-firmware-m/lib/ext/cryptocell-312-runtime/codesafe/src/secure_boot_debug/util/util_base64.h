/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */



#ifndef UTIL_BASE64_H
#define UTIL_BASE64_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief This function converts pem base64 encoded string to char string
 *
 *
 * @param[in] pInStr - PEM base64 string
 * @param[in] inSize - size of given string
 * @param[out] pOutStr - output string decoded
 * @param[in/out] outSize - the output buffer size (in MAX size out actual size)
 *
 * @return CCError_t - On success the value CC_OK is returned,
 *         on failure - a value from bootimagesverifierx509_error.h
 */
CCError_t UTIL_ConvertPemStrToCharStr(uint8_t *pInStr, uint32_t inSize,
                      uint8_t *pOutStr, uint32_t *outSize);


#ifdef __cplusplus
}
#endif

#endif



