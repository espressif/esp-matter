/*
 *   Copyright (c) 2015-2016, GreenPeak Technologies
 *   Copyright (c) 2017, Qorvo Inc
 *
 *   This software is owned by Qorvo Inc
 *   and protected under applicable copyright laws.
 *   It is delivered under the terms of the license
 *   and is intended and supplied for use solely and
 *   exclusively with products manufactured by
 *   Qorvo Inc.
 *
 *
 *   THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 *   CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 *   IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 *   LIMITED TO, IMPLIED WARRANTIES OF
 *   MERCHANTABILITY AND FITNESS FOR A
 *   PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *   QORVO INC. SHALL NOT, IN ANY
 *   CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 *   INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 *   FOR ANY REASON WHATSOEVER.
 *
 *   $Header$
 *   $Change$
 *   $DateTime$
 */


#ifndef _GPSECUREBOOT_H_
#define _GPSECUREBOOT_H_


/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"

/*****************************************************************************
 *                    Enum Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_SECUREBOOT_RMA_ACTION_BULK_ERASE       (0x80DF55BALU)
#define GP_SECUREBOOT_RMA_ACTION_OPEN_MSI         (0xBD10EDE9LU)

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

/**
 *
 *  @brief Authenticate an arbitrary image in flash with metadata taken from extended user license
 *
 *  This function authenticates a flash image, based on information from the extended user license located
 *  @ startAddressImage + licenseOffset
 *
 *  @param  startAddressImage   the full flash address of the beginning of the area is stored. Used in conjuction with the offsets in the
 *  extended user license to calculate the correct flash address of the sections.
 *  @param  licenseOffset       The offset where the loaded + extended user license starts
 *
 *  @return                     True if image is authentic (matches the signature), False if not
 */

Bool gpSecureBoot_AuthenticateImage(UInt32 startAddressImage, UInt32 licenseOffset);

#if defined(GP_COMP_EXTSTORAGE)
Bool gpSecureBoot_ExtStorage_AuthenticateImage(UInt32 startAddressImage, UInt32 licenseOffset);
#endif

/*
 *
 * @brief given a arbitrary flash address, check if RMA mode needs to be activated
 *
 * @param rmaTokenAddress   address of the location to check on RMA conditions
 *
 * @param rmaAction         check which action should be executed:
 *                          - Bulk Erase Flash -> GP_SECUREBOOT_RMA_ACTION_BULK_ERASE
 *                          - Open MSI         -> GP_SECUREBOOT_RMA_ACTION_OPEN_MSI
 *
 * @return True if RMA mode should be executed, if False, not.
 *
 */
Bool gpSecureBoot_CheckRMAMode(UInt32 rmaTokenAddress, UInt32 rmaAction);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_GPSECUREBOOT_H_

