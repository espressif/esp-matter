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
 *  @file       ECDHCC26X1.h
 *
 *  @brief      ECDH driver implementation for the CC26XX family
 *
 *  This file should only be included in the board file to fill the ECDH_config
 *  struct.
 *
 *  # Hardware and Implementation Details #
 *
 *  # Supported Curves #
 *
 *  The driver implementation supports the following curves for ECDH:
 *
 *  | Curves Supported |
 *  |------------------|
 *  | NISTP256         |
 *
 */

#ifndef ti_drivers_ecdh_ECDHCC26X1__include
#define ti_drivers_ecdh_ECDHCC26X1__include

#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/ECDH.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/rom_ecc.h)

#ifdef __cplusplus
extern "C" {
#endif

/*!
 *  @brief      ECDHCC26X1 Hardware Attributes
 *
 *  ECC26XX hardware attributes should be included in the board file
 *  and pointed to by the ECDH_config struct.
 */
typedef struct {
    uint8_t    dummy;
} ECDHCC26X1_HWAttrs;

/*!
 *  @brief      ECDHCC26X1 Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct {
    bool                            isOpen;
    ECDH_CallbackFxn                callbackFxn;
    ECDH_ReturnBehavior             returnBehavior;
    ECC_State                       eccState;
    uint32_t                        eccWorkZone[171];
} ECDHCC26X1_Object;

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_ecdh_ECDHCC26X1__include */
