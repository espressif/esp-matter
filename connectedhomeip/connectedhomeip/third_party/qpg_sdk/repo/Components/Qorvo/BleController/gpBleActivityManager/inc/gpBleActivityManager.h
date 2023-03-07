/*
 * Copyright (c) 2015, GreenPeak Technologies
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

#ifndef _GPBLEACTIVITYMANAGER_H_
#define _GPBLEACTIVITYMANAGER_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpBle.h"
#include "gpBle_defs.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define BLEACTIVITYMANAGER_DURATION_UNIT_INVALID           0xFFFF

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef struct {
    UInt16 intervalMin;
    UInt16 intervalMax;
} gpBleActivityManager_IntervalMinMax_t;

typedef struct {
    UInt32 firstActivityTs;
    UInt16 interval;
} gpBleActivityManager_Timing_t;

typedef struct {
    UInt16 scanInterval;
    UInt16 scanWindow;
} gpBleActivityManager_ScanInput_t;

typedef struct {
    UInt32 firstScanTs;
} gpBleActivityManager_ScanOutput_t;

typedef struct {
    UInt32 firstVirtConnEventTs;
    UInt16 connInterval;
    UInt16 latency;
    UInt16 timeout;
    gpHal_BleChannelMapHandle_t channelMapHandle;
    UInt8 channelMap[BLE_CHANNEL_MAP_SIZE];
} gpBleActivityManager_ConnOutput_t;

typedef struct {
    UInt32 currentEventTs;
    UInt16 intervalMin;
    UInt16 intervalMax;
    UInt16 currentLatency;
    UInt16 currentInterval;
    UInt8 preferredPeriodicity;
} gpBleActivityManager_ConnUpdateInput_t;

#define gpBleActivityManager_NoCollision    0
#define gpBleActivityManager_Collision        1
typedef  UInt8 gpBleActivityManager_CollisionResult_t;

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void gpBleActivityManager_Init(gpHal_BleCallbacks_t* pCallbacks);
void gpBleActivityManager_Reset(Bool firstReset);
void gpBleActivityManager_OpenConnection(Ble_IntConnId_t connId, UInt32 firstConnEventTs);

gpHci_Result_t gpBleActivityManager_StartAdvertising(gpBleActivityManager_IntervalMinMax_t* pAdvIntParams, gpBleActivityManager_Timing_t* pAdvTimingParams, Bool highDutyCycleDirected, Bool singleEvent);
gpHci_Result_t gpBleActivityManager_StartScanning(gpBleActivityManager_ScanInput_t* pScanIntParams, gpBleActivityManager_ScanOutput_t* pScanTimingParams);
gpHci_Result_t gpBleActivityManager_StartInitScanning(gpBleActivityManager_ScanInput_t* pParams, gpBleActivityManager_Timing_t* pTiming);
gpHci_Result_t gpBleActivityManager_AddVirtualConnection(Ble_IntConnId_t connId, gpBleActivityManager_IntervalMinMax_t* pInterval, gpBleActivityManager_Timing_t* pTiming);
gpHci_Result_t gpBleActivityManager_UpdateMasterConnection(Ble_IntConnId_t connId, gpBleActivityManager_ConnUpdateInput_t* pIn, gpBleActivityManager_Timing_t* pActivityTiming);
void gpBleActivityManager_AddIncomingActivity(Ble_ActivityId_t activityId, UInt16 interval, UInt16 durationUnit625, UInt32 firstActivityTs);
void gpBleActivityManager_UpdateSlaveConnection(Ble_IntConnId_t connId, UInt16 interval);
gpHci_Result_t gpBleActivityManager_GetPreferredAnchorPoint(Ble_IntConnId_t connId, gpBleActivityManager_ConnUpdateInput_t* pIn, gpBleActivityManager_Timing_t* pAnchor);


Bool gpBleActivityManager_IsActivityRegistered(Ble_ActivityId_t activityId);

void gpBleActivityManager_UnregisterActivity(Ble_ActivityId_t activityId);

void gpBleActivityManager_CloseConnection(Ble_IntConnId_t connId);

UInt8 gpBleActivityManager_GetNrOfActivities(void);
UInt16 gpBleActivityManager_GetPreferredInterval(void);
void gpBleActivityManager_ConnectionEventDone(Ble_IntConnId_t connId);
Bool gpBleActivityManager_IsActivityEstablished(Ble_ActivityId_t activityId);


UInt32 gpBleActivityManager_GetNextActivityTs(Ble_ActivityId_t activityId);
gpBleActivityManager_CollisionResult_t gpBleActivityManager_CheckForCollision(UInt32 *proposedDelay, UInt32 windowStart, UInt32 windowEnd);


void gpBleActivityManager_EstablishMasterConnection(Ble_IntConnId_t virtualConnId, Ble_IntConnId_t masterConnId, UInt32 currentEventTimeUs);
void gpBleActivityManager_StopSupervisionTimeout(Ble_IntConnId_t connId);

#ifdef __cplusplus
}
#endif
#endif //_GPBLEACTIVITYMANAGER_H_
