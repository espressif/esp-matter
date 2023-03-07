/*
 * Copyright (c) 2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
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

#ifndef _HAL_GP_DPI_H_
#define _HAL_GP_DPI_H_

/** @file gpHal_DPI.h
 *  @brief This file contains functions for Deep Packet Inspection
*/

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gp_global.h"

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
 /**
 * @brief This function prepares the DPI block for further configuration
*/
void gpHal_DpiPrepareForConfig(void);

/**
 * @brief Returns true if enabling without conflicts is allowed
*/
Bool gpHal_DpiCheckPreConditions(void);

/**
 * @brief Returns true if DPI is running
*/
Bool gpHal_DpiIsDpiRunning(void);

/**
 * @brief Enable the DPI block - only call when gpHal_DpiCheckPreConditions() returned true
*/
gpHal_Result_t gpHal_DpiEnable(void);

/**
 * @brief Disable the DPI block
*/
gpHal_Result_t gpHal_DpiDisable(void);

/**
 * @brief Add a pattern to be checked by DPI
*/
void gpHal_DpiAddPattern(UInt8* pPattr, UInt8 length);

/**
 * @brief Add a device to be checked by DPI
*/
void gpHal_DpiAddDevice(UInt32* pFrameCnt, UInt16* pShortAddr, MACAddress_t* pLongAddr, UInt8* pSecKey);

#ifdef __cplusplus
}
#endif

#endif //_HAL_GP_DPI_H_


