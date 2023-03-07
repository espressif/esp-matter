/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 *   Bluetooth Low Energy
 *   Declarations of the public functions and enumerations of gpBle.
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


#ifndef _GPBLEONNECTIONS_H_
#define _GPBLEONNECTIONS_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpHci_Includes.h"
#include "gpBle.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

// Preamble bytes
#define GPBLE_PREAMBLE_BYTE_ODD         0x55
#define GPBLE_PREAMBLE_BYTE_EVEN        0xAA
#define GPBLE_PREAMBLE_BYTE_CODED       0x3C

// Preamble sizes
#define GPBLE_PREAMBLE_SIZE_1MBIT       1
#define GPBLE_PREAMBLE_SIZE_2MBIT       2
#define GPBLE_PREAMBLE_SIZE_CODED       10

#define GPBLE_CONNECTION_HANDLE_MAX     0xEFF

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef Bool (*gpBle_ConnectionsHandleInUse_t) (gpHci_ConnectionHandle_t connectionHandle);
typedef gpHci_ConnectionHandle_t (*gpBle_AllocateHandle_t)  (void);
typedef void (*gpBle_Establish_t)  (gpHci_ConnectionHandle_t connHandle, gpHci_Result_t result);
typedef void (*gpBle_Disconnect_t) (gpHci_ConnectionHandle_t connHandle, gpHci_Result_t reason);

typedef struct {
    gpBle_AllocateHandle_t  cbAllocateHandle;
    gpBle_Establish_t       cbEstablish;
    gpBle_Disconnect_t      cbDisconnect;
} gpBle_ConnectionCallbacks_t;

/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/

void gpBle_ConnectionsInit(void);
void gpBle_ConnectionsReset(Bool firstReset);
void gpBle_RegisterConnectionCallbacks(const gpBle_ConnectionCallbacks_t* pCallbacks);
UInt8 gpBle_GetPreambleSymbol(gpHci_Phy_t phy, gpBle_AccessAddress_t accessAddress);
gpHci_ConnectionHandle_t gpBle_AllocateHciConnectionHandle(void);
void gpBle_EstablishConnection(gpHci_ConnectionHandle_t connHandle, gpHci_Result_t result);
void gpBle_StopConnection(gpHci_ConnectionHandle_t connHandle, gpHci_Result_t reason);

#endif //_GPBLE_CONNECTIONS_H_
