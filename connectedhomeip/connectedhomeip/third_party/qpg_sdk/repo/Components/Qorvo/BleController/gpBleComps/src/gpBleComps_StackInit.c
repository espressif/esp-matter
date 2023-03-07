/*
 * Copyright (c) 2017, Qorvo Inc
 *
 * gpZproComps_StackInit.c
 *
 * The file contains the function that initializes all the base components that are used.
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


#define GP_COMPONENT_ID GP_COMPONENT_ID_BLECOMPS

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpBleComps.h"
#include "gpBle_defs.h"
#include "gpLog.h"

#ifdef GP_COMP_RTIL
#include "gpRtIl.h"
#endif //GP_COMP_RTIL

#ifdef GP_COMP_BLE
#include "gpBle.h"
#endif //GP_COMP_BLE

#ifdef GP_COMP_BLEADDRESSRESOLVER
#include "gpBleAddressResolver.h"
#endif //GP_COMP_BLEADDRESSRESOLVER


#ifdef GP_COMP_BLEADVERTISER
#include "gpBleAdvertiser.h"
#endif //GP_COMP_BLEADVERTISER

#ifdef GP_COMP_BLECONFIG
#include "gpBleConfig.h"
#endif //GP_COMP_BLECONFIG

#ifdef GP_COMP_BLEDATACHANNELRXQUEUE
#include "gpBleDataChannelRxQueue.h"
#endif //GP_COMP_BLEDATACHANNELRXQUEUE

#ifdef GP_COMP_BLEDATACHANNELTXQUEUE
#include "gpBleDataChannelTxQueue.h"
#endif //GP_COMP_BLEDATACHANNELTXQUEUE

#ifdef GP_COMP_BLEDATACOMMON
#include "gpBleDataCommon.h"
#endif //GP_COMP_BLEDATACOMMON

#ifdef GP_COMP_BLEDATARX
#include "gpBleDataRx.h"
#endif //GP_COMP_BLEDATARX

#ifdef GP_COMP_BLEDATATX
#include "gpBleDataTx.h"
#endif //GP_COMP_BLEDATATX


#ifdef GP_COMP_BLELLCP
#include "gpBleLlcp.h"
#endif //GP_COMP_BLELLCP

#ifdef GP_COMP_BLELLCPFRAMEWORK
#include "gpBleLlcpFramework.h"
#endif //GP_COMP_BLELLCPFRAMEWORK

#ifdef GP_COMP_BLELLCPPROCEDURES
#include "gpBleLlcpProcedures.h"
#endif //GP_COMP_BLELLCPPROCEDURES

#ifdef GP_COMP_BLESCANNER
#include "gpBleScanner.h"
#endif //GP_COMP_BLESCANNER

#ifdef GP_COMP_BLEPERADVSYNC
#include "gpBlePerAdvSync.h"
#endif //GP_COMP_BLEPERADVSYNC

#ifdef GP_COMP_BLEACTIVITYMANAGER
#include "gpBleActivityManager.h"
#endif //GP_COMP_BLEACTIVITYMANAGER

#ifdef GP_COMP_BLESECURITYCOPROCESSOR
#include "gpBleSecurityCoprocessor.h"
#endif //GP_COMP_BLESECURITYCOPROCESSOR

#ifdef GP_COMP_BLETEST
#include "gpBleTest.h"
#endif //GP_COMP_BLETEST

#ifdef GP_COMP_BLETESTMODE
#include "gpBleTestMode.h"
#endif //GP_COMP_BLETESTMODE


#ifdef GP_COMP_BLEDIRECTIONFINDING
#include "gpBleDirectionFinding.h"
#endif //GP_COMP_BLEDIRECTIONFINDING

#ifdef GP_COMP_BLEPERADVSYNCTRANSFER
#include "gpBlePerAdvSyncTransfer.h"
#endif //GP_COMP_BLEPERADVSYNCTRANSFER

#ifdef GP_COMP_BLECIGMANAGER
#include "gpBleCigManager.h"
#endif //GP_COMP_BLECIGMANAGER

#ifdef GP_COMP_BLEISOAL
#include "gpBleIsoal.h"
#endif //GP_COMP_BLEISOAL

#ifdef GP_COMP_BLEISODATAMUX
#include "gpBleIsoDataMux.h"
#endif //GP_COMP_BLEISODATAMUX

#ifdef GP_COMP_BLEISOTESTMODE
#include "gpBleIsoTestMode.h"
#endif //GP_COMP_BLEISOTESTMODE

#ifdef GP_COMP_BLEISOCHANNELMANAGER
#include "gpBleIsoChannelManager.h"
#endif //GP_COMP_BLEISOCHANNELMANAGER

#ifdef GP_COMP_BLECONNECTIONMANAGER
#include "gpBleConnectionManager.h"
#endif //GP_COMP_BLECONNECTIONMANAGER

#ifdef GP_COMP_HCI
#include "gpHci.h"
#endif //GP_COMP_HCI


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#ifdef GP_COMP_BLECOMPS

// fix for findbuildconflicts, when this flag is used in P1053

#endif //ifndef GP_COMP_BLECOMPS
/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

#ifndef GP_COMP_UNIT_TEST
static void Ble_TriggerResetDone(void);
#endif //GP_COMP_UNIT_TEST

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

#ifndef GP_COMP_UNIT_TEST
void Ble_TriggerResetDone(void)
{
    gpHci_EventCbPayload_t eventCb;
        // When done, we should trigger a command complete to the host
    eventCb.commandCompleteParams.opCode = gpHci_OpcodeNoOperation;
    eventCb.commandCompleteParams.result = gpHci_ResultSuccess;
    eventCb.commandCompleteParams.numHciCmdPackets = 0x01;

    // Make sure this is scheduled, giving other modules time to initialize
    gpBle_ScheduleEvent(0, gpHci_EventCode_CommandComplete, &eventCb);
}
#endif //GP_COMP_UNIT_TEST

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpBleComps_StackInit(void)
{
    gpHal_BleCallbacks_t bleCallbacks;

    MEMSET(&bleCallbacks, 0, sizeof(bleCallbacks));

    // Move to better location
#ifdef GP_COMP_RTIL
    gpRtIl_Init(&bleCallbacks);
#endif //GP_COMP_RTIL

#ifdef GP_COMP_BLE
    gpBle_Init(&bleCallbacks);
#endif //GP_COMP_BLE

#ifdef GP_COMP_BLEISODATAMUX
    gpBleIsoDataMux_Init();
#endif //GP_COMP_BLEISODATAMUX

#ifdef GP_COMP_HCI
    gpHci_Init();
#endif //GP_COMP_HCI

#ifdef GP_COMP_BLECONFIG
    // This one should be early in the chain, to clear event masks on reset (set to default)
    gpBleConfig_Init();
#endif //GP_COMP_BLECONFIG

    // Make sure LLCP is before adv/scan/init/activity manager, as this is the central service in the link layer
#ifdef GP_COMP_BLELLCP
    gpBleLlcp_Init(&bleCallbacks);
#endif //GP_COMP_BLELLCP

#ifdef GP_COMP_BLELLCPFRAMEWORK
    gpBleLlcpFramework_Init();
#endif //GP_COMP_BLELLCPFRAMEWORK


#ifdef GP_COMP_BLEADVERTISER
    gpBleAdvertiser_Init(&bleCallbacks);
#endif //GP_COMP_BLEADVERTISER

#ifdef GP_COMP_BLESCANNER
    gpBleScanner_Init(&bleCallbacks);
#endif //GP_COMP_BLESCANNER

#ifdef GP_COMP_BLEPERADVSYNC
    gpBlePeriodicAdvertisingSync_Init();
#endif // GP_COMP_BLEPERADVSYNC


#ifdef GP_COMP_BLEDATACOMMON
    gpBle_DataCommonInit();
#endif //GP_COMP_BLEDATACOMMON

#ifdef GP_COMP_BLEDATATX
    gpBle_DataTxInit();
#endif //GP_COMP_BLEDATATX

#ifdef GP_COMP_BLEDATARX
    //gpBle_DataRxInit(); empty
#endif //GP_COMP_BLEDATATX

#ifdef GP_COMP_BLEDATACHANNELTXQUEUE
    gpBle_DataTxQueueInit(&bleCallbacks);
#endif //GP_COMP_BLEDATACHANNELTXQUEUE

#ifdef GP_COMP_BLEDATACHANNELRXQUEUE
    gpBle_DataRxQueueInit(&bleCallbacks);
#endif //GP_COMP_BLEDATACHANNELRXQUEUE

#ifdef GP_COMP_BLELLCPPROCEDURES
    gpBleLlcpProcedures_Init(&bleCallbacks);
#endif // GP_COMP_BLELLCPPROCEDURES

    // Init other services
#ifdef GP_COMP_BLEADDRESSRESOLVER
    gpBleAddressResolver_Init();
#endif //GP_COMP_BLEADDRESSRESOLVER


#ifdef GP_COMP_BLECIGMANAGER
    gpBleCigManager_Init();
#endif //GP_COMP_BLECIGMANAGER

#ifdef GP_COMP_BLEACTIVITYMANAGER
    gpBleActivityManager_Init(&bleCallbacks);
#endif //GP_COMP_BLEACTIVITYMANAGER

#ifdef GP_COMP_BLESECURITYCOPROCESSOR
    //gpBle_SecurityCoprocessorInit(); empty
#endif //GP_COMP_BLESECURITYCOPROCESSOR

#ifdef GP_COMP_BLETEST
    gpBleTest_Init();
#endif //GP_COMP_BLETEST

#ifdef GP_COMP_BLETESTMODE
    gpBle_TestModeInit(&bleCallbacks);
#endif //GP_COMP_BLETESTMODE

#ifdef GP_COMP_BLEDIRECTIONFINDING
    gpBleDirectionFinding_Init();
#endif //GP_COMP_BLEDIRECTIONFINDING

#ifdef GP_COMP_BLEPERADVSYNCTRANSFER
    gpBlePerAdvSyncTransfer_Init();
#endif //GP_COMP_BLEPERADVSYNCTRANSFER

#ifdef GP_COMP_BLEISOAL
    gpBleIsoal_Init();
#endif //GP_COMP_BLEISOAL

#ifdef GP_COMP_BLEISOTESTMODE
    gpBleIsoTestMode_Init();
#endif //GP_COMP_BLEISOTESTMODE

#ifdef GP_COMP_BLEISOCHANNELMANAGER
    gpBleIsoChannelManager_Init();
#endif //GP_COMP_BLEISOCHANNELMANAGER

#ifdef GP_COMP_BLECONNECTIONMANAGER
    gpBleConnectionManager_Init();
#endif //GP_COMP_BLECONNECTIONMANAGER

    // Register callbacks to gpHal
    gpHal_BleRegisterCallbacks(&bleCallbacks);

    // Make sure they are all in a clean state
    gpBle_ResetServices(true);

#ifndef GP_COMP_UNIT_TEST
    Ble_TriggerResetDone();
#endif
}


//This function resets services in the order they were initialized: this may cause issues in case a function has
//dependencies on another procedure that may already have lost its context during the reset.
void gpBle_ResetServices(Bool firstReset)
{

#ifdef GP_COMP_RTIL
    gpRtIl_Reset();
#endif //GP_COMP_RTIL

#ifdef GP_COMP_HCI
    gpHci_CompReset();
#endif //GP_COMP_HCI

    // Make sure LLCP is before adv/scan/init/activity manager, as this is the central service in the link layer
#ifdef GP_COMP_BLELLCP
    gpBleLlcp_Reset(firstReset);
#endif //GP_COMP_BLELLCP

#ifdef GP_COMP_BLELLCPFRAMEWORK
    gpBleLlcpFramework_Reset(firstReset);
#endif //GP_COMP_BLELLCPFRAMEWORK

#ifdef GP_COMP_BLEADVERTISER
    gpBleAdvertiser_Reset(firstReset);
#endif //GP_COMP_BLEADVERTISER

#ifdef GP_COMP_BLEPERADVSYNC
    gpBlePeriodicAdvertisingSync_Reset();
#endif // GP_COMP_BLEPERADVSYNC

#ifdef GP_COMP_BLECONFIG
    // This one should be early in the chain, to clear event masks on reset (set to default)
    gpBleConfig_Reset(firstReset);
#endif //GP_COMP_BLECONFIG

#ifdef GP_COMP_BLESCANNER
    gpBleScanner_Reset(firstReset);
#endif //GP_COMP_BLESCANNER


#ifdef GP_COMP_BLEDATACOMMON
    gpBle_DataCommonReset(firstReset);
#endif //GP_COMP_BLEDATACOMMON

#ifdef GP_COMP_BLEDATATX
    gpBle_DataTxReset(firstReset);
#endif //GP_COMP_BLEDATATX

#ifdef GP_COMP_BLEDATARX
    gpBle_DataRxReset(firstReset);
#endif //GP_COMP_BLEDATATX

#ifdef GP_COMP_BLEDATACHANNELTXQUEUE
    gpBle_DataTxQueueReset(firstReset);
#endif //GP_COMP_BLEDATACHANNELTXQUEUE

#ifdef GP_COMP_BLEDATACHANNELRXQUEUE
    gpBle_DataRxQueueReset(firstReset);
#endif //GP_COMP_BLEDATACHANNELRXQUEUE

#ifdef GP_COMP_BLELLCPPROCEDURES
    gpBleLlcpProcedures_Reset(firstReset);
#endif // GP_COMP_BLELLCPPROCEDURES

    // Init other services
#ifdef GP_COMP_BLEADDRESSRESOLVER
    gpBleAddressResolver_Reset(firstReset);
#endif //GP_COMP_BLEADDRESSRESOLVER

    // Init other services

#ifdef GP_COMP_BLECIGMANAGER
    gpBleCigManager_Reset(firstReset);
#endif //GP_COMP_BLECIGMANAGER

#ifdef GP_COMP_BLEACTIVITYMANAGER
    gpBleActivityManager_Reset(firstReset);
#endif //GP_COMP_BLEACTIVITYMANAGER

#ifdef GP_COMP_BLESECURITYCOPROCESSOR
    gpBle_SecurityCoprocessorReset(firstReset);
#endif //GP_COMP_BLESECURITYCOPROCESSOR

#ifdef GP_COMP_BLETEST
    gpBleTest_ResetRequest();
#endif //GP_COMP_BLETEST

#ifdef GP_COMP_BLETESTMODE
    gpBle_TestModeReset(firstReset);
#endif //GP_COMP_BLETESTMODE

#ifdef GP_COMP_BLEDIRECTIONFINDING
    gpBleDirectionFinding_Reset(firstReset);
#endif //GP_COMP_BLEDIRECTIONFINDING


#ifdef GP_COMP_BLEPERADVSYNCTRANSFER
    gpBlePerAdvSyncTransfer_Reset();
#endif //GP_COMP_BLEPERADVSYNCTRANSFER

#ifdef GP_COMP_BLEISOAL
    gpBleIsoal_Reset(firstReset);
#endif //GP_COMP_BLEISOAL

#ifdef GP_COMP_BLEISODATAMUX
    gpBleIsoDataMux_Reset(firstReset);
#endif //GP_COMP_BLEISODATAMUX

#ifdef GP_COMP_BLEISOTESTMODE
    gpBleIsoTestMode_Reset(firstReset);
#endif //GP_COMP_BLEISOTESTMODE

#ifdef GP_COMP_BLEISOCHANNELMANAGER
    gpBleIsoChannelManager_Reset(firstReset);
#endif //GP_COMP_BLEISOCHANNELMANAGER

#ifdef GP_COMP_BLECONNECTIONMANAGER
    gpBleConnectionManager_Reset(firstReset);
#endif //GP_COMP_BLECONNECTIONMANAGER

#ifdef GP_COMP_BLE
    // Reset gpBle block (named different to avoid conflict with gpBle_Reset function that resets the complete LL).
    gpBle_ResetBlock(firstReset);
#endif //GP_COMP_BLE
}

void gpBleComps_ResetLinkLayer(void)
{
    gpBle_ResetServices(false);
}

