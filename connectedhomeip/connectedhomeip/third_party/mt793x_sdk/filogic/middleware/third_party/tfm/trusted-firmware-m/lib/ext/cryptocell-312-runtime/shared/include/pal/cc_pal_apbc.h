/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_pal_apbc
 @{
 */

/*!
 @file
 @brief This file contains the definitions and APIs for APB-C implementation.

 This is a placeholder for platform-specific APB-C implementation.
*/

#ifndef _CC_PAL_APBC_H
#define _CC_PAL_APBC_H

/*!
 @brief This function initiates an atomic counter.

 @return Void.
 */
void CC_PalApbcCntrInit(void);

/*!
 @brief This function returns the number of APB-C access operations.

 @return The value of the atomic counter.
 */
int32_t CC_PalApbcCntrValue(void);

/*!
 @brief This function updates the atomic counter on each call to APB-C access.

 On each call to APB-C access, the counter is increased. At the end of each
 operation, the counter is decreased.

 @return \c 0 on success.
 @return A non-zero value on failure.
 */
CCError_t CC_PalApbcModeSelect(
               /*! [in] Determines the APB-C mode: TRUE (APB-C start access).
               FALSE (APB-C finish access). */
               CCBool isApbcInc
               );


/*!
 @}
 */
#endif
