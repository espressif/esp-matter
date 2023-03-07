/*
 *   Copyright (c) 2017, Qorvo Inc
 *
 *
 *   Declarations of the public functions and enumerations of gpBleDataRx.
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


#ifndef _GPBLEDATARX_H_
#define _GPBLEDATARX_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
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

/** @function pointer Ble_cbUnexpectedDataRx_t
*   @param connId
*/
typedef void (*Ble_cbUnexpectedDataRx_t) (Ble_IntConnId_t connId);


/*****************************************************************************
 *                    Component Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//Requests
void gpBle_DataRxReset(Bool firstTime);

void gpBle_DataRxOpenConnection(Ble_IntConnId_t connId);

void gpBle_DataRxInterceptUnexpectedPdus(Ble_IntConnId_t connId, Ble_cbUnexpectedDataRx_t func);

void gpBle_DataRxCloseConnection(Ble_IntConnId_t connId);

void gpBle_DataRxIndication(Ble_IntConnId_t connId, Ble_LLID_t llid, gpPd_Loh_t pdLoh);

void gpBle_DataRxConnEventDone(Ble_IntConnId_t connId);

void gpBle_HciDataRxConfirm(gpBle_RxBufferHandle_t bufferId);

//Indications
void gpBle_cbDataRxIndication(gpHci_ConnectionHandle_t connHandle, gpHci_PacketBoundaryFlag_t boundary, gpBle_RxBufferHandle_t bufferId, UInt16 dataLength, UInt8* pData);

/*****************************************************************************
 *                    Service Function Prototypes
 *****************************************************************************/


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_GPBLEDATARX_H_
