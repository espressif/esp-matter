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
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#ifndef _CORDIO_BLE_HOST_H_
#define _CORDIO_BLE_HOST_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Initialize Cordio BLE stack */
void cordioBleHost_Init(void);

/** @brief Function to be called during the main loop to handle cordio stack tasks */
void cordioBleHost_OnIdle(void);

/** @brief Vendor specific function
 *
 *  @param timeFor15Dot4
 */
void cordioBleHost_SetVsdDualModeTimeFor15Dot4(UInt32 timeFor15Dot4);

/**
 * @brief Function for setting the Bluetooth Device Address
 *
 * @param bdAddr    Bluetooth Device Address to be set
 */
void cordioBleHost_SetDeviceAddress(const BtDeviceAddress_t* bdAddr);

/**
 * @brief Function for getting the Bluetooth Device Address
 *
 * @param bdAddr    Bluetooth Device Address returned through given pointer
 */
void cordioBleHost_GetDeviceAddress(BtDeviceAddress_t* bdAddr);

/**
 * @brief Function to decode Cordio message numbering to a log print
 *
 * @param msg_num   message to print
 */
void cordioBleHost_PrintEventString(UInt8 msg_num);

/**
 * @brief Function for setting the transmit power of the link controller
 *
 * @param  dBm  transmit power in dBm
 */
void cordioBleHost_SetTransmitPower(Int8 transmitPower);

/**
 * @brief Function for enabling VSD event notification from LL
 *
 * @param  event  Bit to be set to receive notifications in the host stack
 */
void cordioBleHost_SetEventNotificationBit(UInt8 event);

/**
 * @brief Function for enabling VSD event notification from LL
 *
 * @param  event  Bit to be reset to disable notifications in the host stack
 */
void cordioBleHost_ResetEventNotificationBit(UInt8 event);


#ifdef __cplusplus
}
#endif

#endif
