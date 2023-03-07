/*
 *   Copyright (c) 2017, Qorvo Inc
 *
 *
 *   Declarations of the public functions and enumerations of gpBleDataTx.
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


#ifndef _GPBLEDATATX_H_
#define _GPBLEDATATX_H_

/// @file "gpBleDataTx.h"
/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpPd.h"
#include "gpHci_Includes.h"
#include "gpBle.h"

#include "gpBle_defs.h"

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

//Requests
void gpBle_DataTxInit(void);

void gpBleDataTx_cbDataConfirm(Ble_IntConnId_t connId);

void gpBle_TxResourceAvailableInd(Ble_IntConnId_t connId);

void gpBle_DataTxOpenConnection(Ble_IntConnId_t connId);

void gpBle_DataTxSetConnectionPause(Ble_IntConnId_t connId, Bool pause);

void gpBle_DataTxCloseConnection(Ble_IntConnId_t connId);

void gpBle_DataTxReset(Bool firstReset);

#if !(defined(GP_BLE_DIVERSITY_OPTIMIZE_MEMCPY))
void gpBle_DataTxRequest(gpHci_ConnectionHandle_t connHandle, UInt16 dataLength, UInt8* pData);
#else
void gpBle_DataTxRequest(gpHci_ConnectionHandle_t connHandle, UInt16 dataLength, UInt8* pData, void* pConn, void* pWsfBuff);
#endif //!(defined(GP_BLE_DIVERSITY_OPTIMIZE_MEMCPY))

Bool gpBle_DataTxIsDataInBuffers(Ble_IntConnId_t connId);

//Indications
void Ble_SendHciNumberOfCompletedPacketsEvent(gpHci_ConnectionHandle_t connHandle);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_GPBLEDATATX_H_
