/*
 * Copyright (c) 2013-2014, 2016, GreenPeak Technologies
 * Copyright (c) 2017-2018, Qorvo Inc
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



#ifndef _GPHAL_DATAPENDING_H_
#define _GPHAL_DATAPENDING_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "gpHal_RomCode_DP.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

/** @file gpHal_DP.h
 *  @brief This file contains all the functions needed for DataPending functionality.
*/

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gp_global.h"

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/** @enum gpHal_AddressMode_t */
//@{
/** @brief No Address */
#define gpHal_AddressModeNoAddress                  0
/** @brief Reserved */
#define gpHal_AddressModeReserved                   1
/** @brief Short (i.e. 16-bit) address */
#define gpHal_AddressModeShortAddress               2
/** @brief Extended (i.e. 8-byte) address */
#define gpHal_AddressModeExtendedAddress            3
/** @typedef gpHal_AddressMode_t
    @brief Selection of the address mode
*/
typedef UInt8 gpHal_AddressMode_t;
//@}

/** @union gpHal_RfAddress_t */
typedef union {
/** The short address (2 bytes).*/
    UInt16  Short;
/** The extended address (MAC address), 8 bytes.*/
    MACAddress_t Extended;
} gpHal_RfAddress_t;

/** @union gpHal_AddressInfo_t */
typedef struct {
    gpHal_RfAddress_t address;
    UInt16 panId;
    gpHal_AddressMode_t  addressMode;
} gpHal_AddressInfo_t;

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpHal_CodeJumpTableFlash_Defs_DP.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */

gpHal_Result_t gpHal_DpClearEntries(UInt8 id);

/** @brief Add an entry to the list of addresses for which a data packet is pending transission
*
*   @param  pAddressInfo  The address for which a data packet is pending
*   @param  id            The stack id which has the data packet pending
*   @return result        The return parameter indicating success or the failure code
*
*/
gpHal_Result_t gpHal_DpAddEntry(gpHal_AddressInfo_t* pAddressInfo, UInt8 id);
/** @brief Remove an entry from the list of addresses for which a data packet is pending transission
*
*   @param  pAddressInfo  The address for which a data packet is no longer pending
*   @param  id            The stack id which had the data packet pending
*   @return result        The return parameter indicating success or the failure code
*
*/
gpHal_Result_t gpHal_DpRemoveEntry(gpHal_AddressInfo_t* pAddressInfo, UInt8 id);

/** @brief Check if there are datapending entries in the list
*
*   @return result        A boolean indicating if any stack has pending data packets in the list.
*
*/
Bool gpHal_DPEntriesPending(void);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

#ifdef __cplusplus
}
#endif

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif  /* _GPHAL_DATAPENDING_H_ */

