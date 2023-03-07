/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _CC_PM_DEFS_H_
#define _CC_PM_DEFS_H_

/*!
@file
@defgroup cc_pm_defs CryptoCell power management macroes
@brief This file contains power management definitions.
@{
@ingroup cryptocell_api

*/

#ifdef __cplusplus
extern "C"
{
#endif

#include "cc_pal_pm.h"

/*! Get ARM Cerberus status. Return number of active registered CC operations */
#define CC_STATUS_GET   CC_PalPowerSaveModeStatus()

/*! Notify ARM Cerberus is active. */
#define CC_IS_WAKE  CC_PalPowerSaveModeSelect(CC_FALSE)

/*! Notify ARM Cerberus is idle. */
#define CC_IS_IDLE  CC_PalPowerSaveModeSelect(CC_TRUE)


#ifdef __cplusplus
}
#endif
/**
@}
 */
#endif /*_CC_PM_DEFS_H_*/
