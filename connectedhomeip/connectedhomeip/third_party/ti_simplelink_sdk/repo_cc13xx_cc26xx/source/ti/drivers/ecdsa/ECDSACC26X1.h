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
 *  @file       ECDSACC26X1.h
 *
 *  @brief      ECDSA driver implementation for the CC26X1 family
 *
 *  This file should only be included in the board file to fill the ECDSA_config
 *  struct.
 *
 *  # Supported Curves #
 *
 *  The driver implementation supports the following curves for ECDH:
 *
 *  | Curves Supported |
 *  |------------------|
 *  | NISTP256         |
 */

#ifndef ti_drivers_ecdsa_ECDSACC26X1__include
#define ti_drivers_ecdsa_ECDSACC26X1__include

#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/ECDSA.h>
#include <ti/drivers/TRNG.h>
#include <ti/drivers/trng/TRNGCC26XX.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/rom_ecc.h)

#ifdef __cplusplus
extern "C" {
#endif

/*!
 *  @brief      ECDSACC26X1 Hardware Attributes
 *
 *  ECDSACC26X1 hardware attributes should be included in the board file
 *  and pointed to by the ECDSA_config struct.
 */
typedef struct {
    /*! @brief TRNG Peripheral's interrupt priority.

        The CC26xx uses three of the priority bits, meaning ~0 has the same effect as (7 << 5).

        (7 << 5) will apply the lowest priority.

        (1 << 5) will apply the highest priority.

        Setting the priority to 0 is not supported by this driver.

        HWI's with priority 0 ignore the HWI dispatcher to support zero-latency interrupts, thus invalidating the critical sections in this driver.
    */
    uint8_t    trngIntPriority;
} ECDSACC26X1_HWAttrs;

/*!
 *  @brief      ECDSACC26X1 Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct {
    bool                            isOpen;
    ECDSA_CallbackFxn               callbackFxn;
    ECDSA_ReturnBehavior            returnBehavior;
    ECC_State                       eccState;
    uint32_t                        eccWorkZone[288];
} ECDSACC26X1_Object;

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_ecdsa_ECDSACC26X1__include */
