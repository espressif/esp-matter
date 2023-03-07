/*
 * Copyright (c) 2017, Qorvo Inc
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by\
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

#ifndef _GPBLEDATACHANNELTXQUEUE_H_
#define _GPBLEDATACHANNELTXQUEUE_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpBle_defs.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

#define Ble_DataChannelTxQueueCallerLlcp       0x00
#define Ble_DataChannelTxQueueCallerData       0x01
typedef UInt8 Ble_DataChannelTxQueueCaller_t;

typedef struct {
    Bool includeCte;
    UInt8 cteDurationUnit; // units of 8us
    UInt8 cteType;  // create typedef?
} gpBleData_CteOptions_t;

/*****************************************************************************
 *                    Component Function Definitions
 *****************************************************************************/

// Data channel TX Queue handler
void gpBle_DataTxQueueInit(gpHal_BleCallbacks_t* pCallbacks);
void gpBle_DataTxQueueReset(Bool firstReset);
gpHci_Result_t gpBle_DataTxQueueRequest(Ble_IntConnId_t connId, gpPd_Loh_t pdLoh, Ble_LLID_t llid, gpBleData_CteOptions_t* pCteOptions);
void gpBle_DataTxQueueOpenConnection(Ble_IntConnId_t connId);
void gpBle_DataTxQueueEnableEncryption(Ble_IntConnId_t connId, Bool enable);
void gpBle_DataTxQueueCloseConnection(Ble_IntConnId_t connId);
gpPd_Handle_t gpBle_DataTxQueueAllocatePd(Ble_IntConnId_t connId, Ble_DataChannelTxQueueCaller_t caller);
void gpBle_DataTxQueueRegisterEmptyQueueCallback(Ble_IntConnId_t connId, Ble_EmptyQueueCallback_t callback);
void gpBle_DataTxQueueUnregisterEmptyQueueCallback(Ble_IntConnId_t connId);
void gpBle_SetConnectionBandwidthControl(Ble_IntConnId_t connId, UInt16 newMaxCELengthUnit, UInt16 newIntervalUnit);
void gpBle_cbDataTxQueueResourceAvailable(Ble_IntConnId_t connId);
#endif //_GPBLEDATACHANNELTXQUEUE_H_

