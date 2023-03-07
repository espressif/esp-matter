/*
 * Copyright (c) 2021, Qorvo Inc
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
 * $Header$
 * $Change$
 * $DateTime$
 */

/** @file "gpUpgrade_Hash.h"
 *
 *  Upgrade functionality
 *
 *  Declarations of the private functions within gpUpgrade for hashing.
*/

#ifndef _GPUPGRADE_HASH_H_
#define _GPUPGRADE_HASH_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpUpgrade.h"
#include "gpUpgrade_defs.h"

/*****************************************************************************
 *                    Enum Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void gpUpgrade_HashPartialCrc(UInt32 * crcVal, UInt32 address, UInt32 totalSize);

#if defined(GP_DIVERSITY_APP_LICENSE_BASED_BOOT)
/** @brief This function calculates a CRC over an entire image
*
*   @param userLicenseAddress   The user license location with sections
*/
UInt32 gpUpgrade_HashImageCrc(UInt32 userLicenseAddress);

#else
/** @brief This function calculates a CRC over an entire image
*
*   @param swUpgTab             The software upgrade table with offset, size and flags
*/
UInt32 gpUpgrade_HashImageCrc(gpUpgrade_Table_t swUpgTab);
#endif // GP_DIVERSITY_APP_LICENSE_BASED_BOOT

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_GPUPGRADE_FLASH_H_
