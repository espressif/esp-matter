/*
 * Copyright (c) 2020, Texas Instruments Incorporated
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
 *  @file       SHA2CC26X1.h
 *
 *  @brief      SHA2 driver implementation for the CC26X1 family
 *
 *  This file should only be included in the board file to fill the SHA2_config
 *  struct.
 *
 *  The CC26X1 family does not have a dedicated hardware crypto accelerator.
 *  As a result, there are several limitations and advantages over accelerator
 *  based implementations.
 *
 *  # Limitations #
 *  - Asynchronously canceling an operation is not possible. The APIs will
 *    always run to completion.
 *  - Invoking driver APIs from multiple contexts at the same time using the
 *    same handle is not permitted or guarded against.
 *
 *  # Advantages #
 *  - Since this is a software implementation that uses dependency injection,
 *    it is possible for multiple clients to simultaneously invoke driver APIs
 *    with their respective handles.
 *
 *  # Supported Digest Sizes #
 *
 *  The driver is built on top of a SHA256 software implementation stored in
 *  ROM. As a result, digest sizes other than 256 bits are not supported with
 *  this driver.
 *
 *  | SHA2_HashTypes Supported |
 *  |--------------------------|
 *  | SHA2_HASH_TYPE_256       |
 *
 */

#ifndef ti_drivers_sha2_SHA2CC26X1__include
#define ti_drivers_sha2_SHA2CC26X1__include

#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/SHA2.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/rom_sha256.h)

#ifdef __cplusplus
extern "C" {
#endif


/*!
 *  @brief Hardware-specific configuration attributes
 *
 *  SHA2CC26X1 hardware attributes are used in the board file by the
 *  #SHA2_Config struct.
 */
typedef struct {
    uint8_t dummy;
} SHA2CC26X1_HWAttrs;

/*! \cond Internal APIs */

/*
 *  SHACC26XX Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct {
    bool                            isOpen;
    SHA2_ReturnBehavior             returnBehavior;
    SHA2_CallbackFxn                callbackFxn;
    uint32_t                        hmacDigest[SHA2_DIGEST_LENGTH_BYTES_256 / sizeof(uint32_t)];
    SHA256_Workzone                 workzone;
} SHA2CC26X1_Object;

/*! \endcond */

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_sha2_SHA2CC26X1__include */
