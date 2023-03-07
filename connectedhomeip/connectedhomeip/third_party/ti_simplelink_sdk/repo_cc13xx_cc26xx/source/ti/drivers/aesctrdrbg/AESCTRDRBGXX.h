/*
 * Copyright (c) 2019-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*!****************************************************************************
 *  @file       AESCTRDRBGXX.h
 *
 *  @brief      Generic AESCTRDRBG implementation based on the AESCTR driver
 *
 *  This file should only be included in the board file to fill the AESCTR_config
 *  struct.
 *
 *  # Use of AESCTR #
 *  This implementation uses the AESCTR driver to generate the random bitstream
 *  required to mutate the internal AESCTRDRBG state and provide random output
 *  bits. The driver will open an instance of the AESCTR driver based on the index
 *  specified in #AESCTRDRBGXX_HWAttrs:aesctrIndex. Mutual exclusion and hardware
 *  access are all handled by the AESCTR driver instance.
 *
 *  # Implementation Limitations
 *  - Only plaintext CryptoKeys are supported by this implementation.
 *
 *  # Runtime Parameter Validation #
 *  The driver implementation does not perform runtime checks for most input parameters.
 *  Only values that are likely to have a stochastic element to them are checked (such
 *  as whether a driver is already open). Higher input paramter validation coverage is
 *  achieved by turning on assertions when compiling the driver.
 */

#ifndef ti_drivers_aesctrdrbg_AESCTRDRBGXX__include
#define ti_drivers_aesctrdrbg_AESCTRDRBGXX__include

#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/AESCTRDRBG.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! @brief Define that specifies the maximum AES key length required
 *
 *  This define defines what the largest AES key length used in an application is.
 *  Since this implementation needs to support all AES key lengths by default,
 *  temporary buffers and the internal driver state are sized to accommodate AES-256.
 *  If only AES-128 is used in an application, the driver can be recompiled
 *  with a different #AESCTRDRBG_MAX_KEY_LENGTH to save RAM in the #AESCTRDRBGXX_Object
 *  and reducing stack size requirements.
 */
#ifndef AESCTRDRBG_MAX_KEY_LENGTH
    #define AESCTRDRBG_MAX_KEY_LENGTH AESCTRDRBG_AES_KEY_LENGTH_256
#endif

/*! @brief Define that specifies the maximum seed length used by the driver */
#define AESCTRDRBG_MAX_SEED_LENGTH (AESCTRDRBG_MAX_KEY_LENGTH + AESCTRDRBG_AES_BLOCK_SIZE_BYTES)

/*!
 *  @brief      AESCTRDRBGXX Hardware Attributes
 *
 *  AESCTR26XX hardware attributes should be included in the board file
 *  and pointed to by the AESCTR_config struct.
 */
typedef struct {
    uint_least8_t   aesctrIndex;    /*! Index into AESCTR_config array */
} AESCTRDRBGXX_HWAttrs;

/*!
 *  @brief      AESCTRDRBGXX Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct {
    uint8_t                         keyingMaterial[AESCTRDRBG_AES_KEY_LENGTH_256];
    uint8_t                         counter[AESCTRDRBG_AES_BLOCK_SIZE_BYTES];
    CryptoKey                       key;
    AESCTR_Handle                   ctrHandle;
    size_t                          seedLength;
    uint32_t                        reseedCounter;
    uint32_t                        reseedInterval;
    int_fast16_t                    returnStatus;
    bool                            isOpen;
    bool                            isInstantiated;
} AESCTRDRBGXX_Object;

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_aesctrdrbg_AESCTRDRBGXX__include */
