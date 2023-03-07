/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef  _CC_PAL_FIPS_H
#define  _CC_PAL_FIPS_H

/*!
@file
@brief This file contains definitions that are used by the FIPS related APIs. The implementation of these functions
need to be replaced according to the Platform and TEE_OS.
*/

#include "cc_pal_types_plat.h"
#include "cc_fips.h"
#include "cc_fips_defs.h"

/**
 * @brief This function purpose is to get the FIPS state.
 *
 *
 * @return Zero on success.
 * @return A non-zero value on failure.
 */
CCError_t CC_PalFipsGetState(CCFipsState_t *pFipsState);


/**
 * @brief This function purpose is to get the FIPS Error.
 *
 *
 * @return Zero on success.
 * @return A non-zero value on failure.
 */
CCError_t CC_PalFipsGetError(CCFipsError_t *pFipsError);


/**
 * @brief This function purpose is to get the FIPS trace.
 *
 *
 * @return Zero on success.
 * @return A non-zero value on failure.
 */
CCError_t CC_PalFipsGetTrace(CCFipsTrace_t *pFipsTrace);


/**
 * @brief This function purpose is to set the FIPS state.
 *
 *
 * @return Zero on success.
 * @return A non-zero value on failure.
 */
CCError_t CC_PalFipsSetState(CCFipsState_t fipsState);


/**
 * @brief This function purpose is to set the FIPS error.
 *
 *
 * @return Zero on success.
 * @return A non-zero value on failure.
 */
CCError_t CC_PalFipsSetError(CCFipsError_t fipsError);


/**
 * @brief This function purpose is to set the FIPS trace.
 *
 *
 * @return Zero on success.
 * @return A non-zero value on failure.
 */
CCError_t CC_PalFipsSetTrace(CCFipsTrace_t fipsTrace);


/**
 * @brief This function purpose is to wait for FIPS interrupt.
 *      After GPR0 (==FIPS) interrupt is detected, clear the interrupt in ICR,
 *      and call CC_FipsIrqHandle
 *
 *
 * @return Zero on success.
 * @return A non-zero value on failure.
 */
CCError_t CC_PalFipsWaitForReeStatus(void);

/**
 * @brief This function purpose is to stop waiting for REE FIPS interrupt.
 *      since TEE lib is terminating
 *
 *
 * @return Zero on success.
 * @return A non-zero value on failure.
 */
CCError_t CC_PalFipsStopWaitingRee(void);

#endif  // _CC_PAL_FIPS_H

