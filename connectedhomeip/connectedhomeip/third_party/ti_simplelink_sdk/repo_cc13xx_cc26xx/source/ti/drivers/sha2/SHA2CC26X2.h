/*
 * Copyright (c) 2017-2021, Texas Instruments Incorporated
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
/** ============================================================================
 *  @file       SHA2CC26X2.h
 *
 *  @brief      SHA2 driver implementation for the CC26X2 family
 *
 *  This file should only be included in the board file to fill the SHA2_config
 *  struct.
 *
 *  The CC26X2 family has a dedicated hardware crypto accelerator. It is capable
 *  of multiple AES block cipher modes of operation as well as SHA2 operations.
 *  Only one operation can be carried out on the accelerator at a time. Mutual
 *  exclusion is implemented at the driver level and coordinated between all
 *  drivers relying on the accelerator. It is transparent to the application
 *  and only noted ensure sensible access timeouts are set.
 *
 *  The driver implementation does not perform runtime checks for most input
 *  parameters.
 *  Only values that are likely to have a stochastic element to them are
 *  checked (such as whether a driver is already open). Higher input parameter
 *  validation coverage is achieved by turning on assertions when compiling
 *  the driver.
 *
 */

#ifndef ti_drivers_sha2_SHA2CC26X2__include
#define ti_drivers_sha2_SHA2CC26X2__include

#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/SHA2.h>

#ifdef __cplusplus
extern "C" {
#endif


/*!
 *  @brief Hardware-specific configuration attributes
 *
 *  SHA2CC26X2 hardware attributes are used in the board file by the
 *  #SHA2_Config struct.
 */
typedef struct {
    /*!< Hardware interrupt priority of the Hash accelerator.
     *
     * The CC26XX provides 8 interrupt priority levels encoded in three bits:
     *
     * Value        | Description
     * ------------ | -----------------------
     * (~0)         | Special value: always lowest priority across all OS kernels.
     * (7 << 5)     | Priority level 7: lowest, but rather use ~0 instead.
     * ..           | ..
     * (0 << 5)     | Priority level 0: highest, not supported by this driver
     *
     * Hardware interrupts with priority level 0 ignore the hardware interrupt dispatcher
     * for minimum latency. This is not supported by this driver.
     */
    uint8_t    intPriority;
} SHA2CC26X2_HWAttrs;


/*! \cond Internal APIs */

#define SHA2CC26X2_MAX_BLOCK_SIZE_BYTES    (SHA2_BLOCK_SIZE_BYTES_512)
#define SHA2CC26X2_MAX_DIGEST_LENGTH_BYTES (SHA2_DIGEST_LENGTH_BYTES_512)

/*!
 *  @brief SHACC26XX Object
 *
 *  \note The application must not access any member variables of this structure!
 */
typedef struct {
    bool                            isOpen;
    volatile bool                   operationInProgress;
    bool                            operationCanceled;
    volatile uint8_t                retainAccessCounter;
    SHA2_ReturnBehavior             returnBehavior;
    int_fast16_t                    returnStatus;
    uint32_t                        accessTimeout;
    SHA2_CallbackFxn                callbackFxn;
    SHA2_HashType                   hashType;
    uint16_t                        bytesInBuffer;
    uint32_t                        bytesProcessed;
    uint32_t                        digest[SHA2CC26X2_MAX_DIGEST_LENGTH_BYTES / sizeof(uint32_t)];
    uint32_t                        hmacDigest[SHA2CC26X2_MAX_DIGEST_LENGTH_BYTES / sizeof(uint32_t)];
    uint8_t                         buffer[SHA2CC26X2_MAX_BLOCK_SIZE_BYTES];
} SHA2CC26X2_Object;

/*! \endcond */

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_sha2_SHA2CC26X2__include */
