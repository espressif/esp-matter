/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_pal_pm
 @{
 */

/*!
 @file
 @brief This file contains the definitions and APIs for power-management
 implementation.

 This is a placeholder for platform-specific power management implementation.
 The module should be updated whether CryptoCell is active or not,
 to notify the external PMU when CryptoCell might be powered down.
 */

#ifndef _CC_PAL_PM_H
#define _CC_PAL_PM_H


/*
******** Function pointer definitions **********
*/


/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/*!
 @brief This function initiates an atomic counter.

 @return Void.
 */
void CC_PalPowerSaveModeInit(void);

/*!
 @brief This function returns the number of active registered CryptoCell
 operations.

 @return The value of the atomic counter.
 */
int32_t CC_PalPowerSaveModeStatus(void);

/*!
 @brief This function updates the atomic counter on each call to CryptoCell.

 On each call to CryptoCell, the counter is increased. At the end of each operation
 the counter is decreased.
 Once the counter is zero, an external callback is called.

 @return \c 0 on success.
 @return A non-zero value on failure.
 */
CCError_t CC_PalPowerSaveModeSelect(
        /*! [in] TRUE: CryptoCell is active. FALSE: CryptoCell is idle. */
        CCBool isPowerSaveMode
        );


/*!
 @}
 */
#endif
