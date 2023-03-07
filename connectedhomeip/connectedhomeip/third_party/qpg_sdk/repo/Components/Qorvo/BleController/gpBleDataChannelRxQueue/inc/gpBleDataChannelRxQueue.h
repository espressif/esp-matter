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

#ifndef _GPBLEDATACHANNELRXQUEUE_H_
#define _GPBLEDATACHANNELRXQUEUE_H_

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

/*****************************************************************************
 *                    Component Function Definitions
 *****************************************************************************/

void gpBle_DataRxQueueInit(gpHal_BleCallbacks_t* pCallbacks);
void gpBle_DataRxQueueReset(Bool firstReset);
void gpBle_DataRxQueueOpenConnection(Ble_IntConnId_t connId);
void gpBle_DataRxQueueCloseConnection(Ble_IntConnId_t connId);
UInt16 gpBle_DataRxQueueReadAuthPayloadTo(Ble_IntConnId_t connId);
void gpBle_DataRxQueueWriteAuthPayloadTo(Ble_IntConnId_t connId, UInt16 authPayloadToUnit);
void gpBle_DataRxQueueEnableAuthPayloadTo(Ble_IntConnId_t connId, Bool enable);
void gpBle_DataRxQueueEnableDecryption(Ble_IntConnId_t connId, Bool enable);
void gpBle_DataRxQueueSetFlowCtrl(UInt16 connMask);
UInt16 gpBle_DataRxQueueGetFlowCtrl(void);

#endif //_GPBLEDATACHANNELRXQUEUE_H_

