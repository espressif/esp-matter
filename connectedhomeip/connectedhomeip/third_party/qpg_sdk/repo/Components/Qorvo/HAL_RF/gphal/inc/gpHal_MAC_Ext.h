/*
 * Copyright (c) 2012-2014, 2016, GreenPeak Technologies
 * Copyright (c) 2017, 2019, Qorvo Inc
 *
 * gpHal_MAC_Ext.h
 *   This file contains all the functions needed for Extra MAC functionality.
 *
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



#ifndef _GPHAL_MAC_EXT_H_
#define _GPHAL_MAC_EXT_H_

/** @file gpHal_MAC_Ext.h
 *  @brief This file contains all the extra functionality for MAC.
*/


/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gp_global.h"
#include "gpHal_MAC.h"
#include "gpPd.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Purges packet from TX queue.
 *
 *  This function purges a packet currently in the TX queue using the pd handle of the packet.
 *  When the handle is non-existent or the packet is already transmitted an error is returned.
 *
 *  Possible results are:
 *          - gpHal_ResultSuccess
 *          - gpHal_ResultInvalidRequest (Invalid pd handle given)
 *          - gpHal_ResultBusy           (Packet already sent)
 *
 *  @param pdHandle  Unique pd handle given to packet at request time.
*/
gpHal_Result_t gpHal_PurgeRequest(gpPd_Handle_t pdHandle);

#ifdef GP_HAL_DIVERSITY_MACFILT_PATTERN_MATCH
/**
 *  @brief Hook to the application that determines if the data contained in the
 *         Pd is "custom data" or regular data
 *
 *  @param pdLoh   Contains info to retrieve the Tx data from the gpPd module
 *  @return        gpHal_ResultSuccess on custom data
 *                 gpHal_ResultInvalidRequest on regular data
 *
 *  @details       Using the gpPd API one can verify the contents of the Tx data
 *                 e.g. gpPd_ReadByte() or gpPd_ReadByteStream()
 */
GP_API gpHal_Result_t gpHal_cbIsCustomData(gpPd_Loh_t pdLoh);

/**
 * @brief Registers the callback for a CUSTOM data confirm.
 *
 * This function registers the callback for a CustomDataConfirm. The callback will be executed on a DataConfirm interrupt, if the data is transmitted from the custom stack.
 * This DataConfirm will be given after a DataRequest is finished.
 *
 * The Primitive interrupt needs to be enabled.
 *
 * @param callback The pointer to the callback function.
*/
GP_API void gpHal_RegisterCustomDataConfirmCallback(gpHal_DataConfirmCallback_t callback);

/**
 * @brief Registers the callback for CUSTOM data indication.
 *
 * This function registers the CustomDataIndication callback. The callback will be executed on a DataIndication interrupt, when pattern matches for custom data
 * This DataIndication will be given if a packet is received.
 *
 * The Primitive interrupt needs to be enabled.
 *
 * @param callback The pointer to the callback function.
*/
GP_API void gpHal_RegisterCustomDataIndicationCallback(gpHal_DataIndicationCallback_t callback);

/**
 * @brief Start matching all incoming radio packets to the supplied pattern.
 *        In case the pattern matches the first 2 bytes of the received data,
 *        it will bypass the MAC handling completely and offer the callback
 *        registered by gpHal_RegisterCustomDataIndicationCallback().
 */
GP_API void gpHal_EnableCustomMac(UInt8 pattern[2]);

/**
 * @brief Stop pattern matching for custom MAC implementations
 */
GP_API void gpHal_DisableCustomMac();
#endif /* GP_HAL_DIVERSITY_MACFILT_PATTERN_MATCH */

#ifdef __cplusplus
}
#endif
#endif  /* _GPHAL_MAC_EXT_H_ */
