/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */



#ifndef _CRYPTO_DRIVER_H
#define _CRYPTO_DRIVER_H

#ifdef __cplusplus
extern "C"
{
#endif


#include "secureboot_basetypes.h"
#include "bsv_crypto_api.h"


/*!
 * @brief This function gives the functionality of integrated hash
 *
 * @param[in] hwBaseAddress     - CryptoCell base address
 * @param[out] hashResult   - the HASH result.
 *
 */
#define SBROM_CryptoHash(wBaseAddress, inputDataAddr, dataSize, hashBuff)       \
    CC_BsvSHA256(hwBaseAddress, (uint8_t *)inputDataAddr, (size_t)dataSize, hashBuff);



#ifdef __cplusplus
}
#endif

#endif



