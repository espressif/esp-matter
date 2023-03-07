/*
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
 */

/** @file "gpBleLlcpProcedures.h"
 *
 *  Declarations of the public functions and enumerations of gpBleLlcpProcedures.
*/

#ifndef _GPBLELLCPPROCEDURES_H_
#define _GPBLELLCPPROCEDURES_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpBleLlcpFramework.h"
#include "gpPd.h"
#include "gpHal_Ble.h"
#include "gpHal.h"
#include "gpEncryption.h"
#include "gpHci_Includes.h"
#include "gpBle.h"
#include "gpBleLlcp.h"
#include "gpBleActivityManager.h"


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
void gpBleLlcpProcedures_Init(gpHal_BleCallbacks_t* pCallbacks);

void gpBleLlcpProcedures_Reset(Bool firstTime);

void Ble_LlcpProcedureUpdateResetConnection(Ble_LlcpLinkContext_t* pContext);

gpHci_Result_t gpBleLlcpProcedures_TriggerAnchorMove(Ble_IntConnId_t connId, UInt16 offset);

gpHal_BleChannelMapHandle_t Ble_LlcpGetLatestChannelMapHandle(gpHal_BleChannelMapHandle_t masterChannelMapHandle);

void gpBleLlcpProcedures_ControllerTriggeredPhyUpdate(Ble_IntConnId_t connId, gpHci_PhyMask_t txPhys, gpHci_PhyMask_t rxPhys);

#if defined(GP_DIVERSITY_BLE_MASTER)
gpHci_Result_t gpBle_SetNewMasterChannelMap(gpHci_ChannelMap_t* pChannelMap);
Bool gpBle_IsMasterChannelMapUpdateInProgress(void);
#endif //defined(GP_DIVERSITY_BLE_MASTER)

gpHci_Result_t gpBle_LeLongTermKeyRequestReply(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);

gpHci_Result_t gpBle_LeLongTermKeyRequestNegativeReply(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);

gpHci_Result_t gpBle_LeConnectionUpdate(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);






//Indications

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_GPBLELLCPPROCEDURES_H_

