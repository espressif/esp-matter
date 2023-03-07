/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _COMMON_CRYPTO_ENCODE_H
#define _COMMON_CRYPTO_ENCODE_H

#include <stdint.h>

#define CC_COMMON_CALC_BASE64_ENCODE_SIZE(origSize) ((((origSize+2)/3)*4)+1)
#define CC_COMMON_CALC_BASE64_MAX_DECODE_SIZE(encodedSize) ((encodedSize*3)/4)  /* max size in case no padding to encoded buffer */

/**
* @brief performs base64-encode
*
* @param[in] pBuff             - the buffer to encode
* @param[in] buffLen           - input buffer length
* @param[in/out] pEncBuffLen   - encoded buffer length
* @param[out] pEncBuff         - encoded buffer
*/
/*********************************************************/
int32_t CC_CommonBase64Encode(uint8_t *pBuff,
                uint32_t  buffLen,
                uint8_t *pEncBuff,
                uint32_t *pEecBuffLen);


/**
* @brief performs base64-decode
*
* @param[in] pEncBuff          - base64-encoded buffer
* @param[in] encBuffLen        - input buffer length
* @param[in/out] pDecBuffLen   - decoded buffer length
* @param[out] pDecBuff         - decoded buffer
*/
/*********************************************************/
int32_t CC_CommonBase64Decode(uint8_t *pEncBuff,
                uint32_t  encBuffLen,
                uint8_t *pDecBuff,
                uint32_t *pDecBuffLen);



#endif
