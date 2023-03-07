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

#ifndef _GPBLESECURITYCOPROCESSOR_H_
#define _GPBLESECURITYCOPROCESSOR_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpBle.h"
#include "gpBle_defs.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define BLE_SEC_INIT_VECTOR_LENGTH  8
#define BLE_SEC_INIT_VECTOR_PART_LENGTH  (BLE_SEC_INIT_VECTOR_LENGTH >> 1)

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Component Function Definitions
 *****************************************************************************/

void gpBle_SecurityCoprocessorReset(Bool firstReset);
gpHci_Result_t gpBle_SecurityCoprocessorCalculateSessionKey(Ble_IntConnId_t connId, UInt8* pLtk, UInt8* pSkd, UInt8* pIv);
gpHci_Result_t gpBle_SecurityCoprocessorCcmEncrypt(Ble_IntConnId_t connId, gpPd_Loh_t* pPdLoh);
gpHci_Result_t gpBle_SecurityCoprocessorCcmDecrypt(Ble_IntConnId_t connId, gpPd_Loh_t* pPdLoh);

/*****************************************************************************
 *                    Service Function Definitions
 *****************************************************************************/
gpHci_Result_t gpBle_AESEncrypt(UInt8* pInplaceBuffer, UInt8* pKey);
gpHci_Result_t gpBle_LeEncrypt(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_LeRand(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
#endif // _GPBLESECURITYCOPROCESSOR_H_

