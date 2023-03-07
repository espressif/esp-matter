/*
 * Copyright (c) 2008-2016, GreenPeak Technologies
 * Copyright (c) 2017-2019, Qorvo Inc
 *
 * gpHal_MISC.h
 *
 * This file contains miscellaneous functions for GPIO and OTP functionality
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#ifndef _HAL_GP_MISC_H_
#define _HAL_GP_MISC_H_

/** @file gpHal_MISC.h
 *  @brief This file contains miscellaneous functions for GPIO and OTP functionality
*/

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gp_global.h"
#include "gpHal_reg.h"

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Returns a 8-bit random value.
 *
 * This function returns a 8-bit random value using samples of the GP chip radio I and Q signals.
 */
GP_API UInt8 gpHal_GetRandomSeed(void);

#if defined(GP_DIVERSITY_GPHAL_K8E)
/**
 * @brief fills a buffer with entropy from the QRNG.
 *
 * This function fills a buffer with entropy from the QRNG.
 */
GP_API void gpHal_GetQRNGRandomSeed(UInt8 size, UInt8* buffer);
#endif //defined(GP_DIVERSITY_GPHAL_K8D) || defined(GP_DIVERSITY_GPHAL_K8E)


#ifdef GP_DIVERSITY_GPHAL_DCDC_ENABLED


#define gpHal_DCDC_ResultSuccess 0
#define gpHal_DCDC_ResultTimeout 1


typedef UInt8 gpHal_DCDC_Result_t;

GP_API void gpHal_DCDCEnable(void);
GP_API Bool gpHalDCDC_IsActive(void);
#endif // GP_DIVERSITY_GPHAL_DCDC_ENABLED

#ifdef __cplusplus
}
#endif
#endif //_HAL_GP_MISC_H_


