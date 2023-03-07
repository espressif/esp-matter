/******************************************************************************

 @file dmm_priority_bl_wsn.c

 @brief Dual Mode Manager Global Priority for 15.4 Collector and BLE Peripheral

 Group: WCS LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2017-2021, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

#include "dmm/dmm_policy.h"
#include "dmm/dmm_priority_ble_wsn.h"


/* BLE Activity */
typedef enum
{
    DMM_BLE_CONNECTION =0x07D0,     
    DMM_BLE_CON_EST =0x03E8,
    DMM_BLE_BROADCASTING =0x0BB8,
    DMM_BLE_OBSERVING =0x0FA0,
} DMMStackActivityBLE;


typedef enum
{
    DMM_WSN_RETRANSMIT =0x022B,       
    DMM_WSN_TRANSMIT =0x0309,		
    DMM_WSN_RECEIVE =0x0307,
} DMMStackActivityWSN;

/* Global Priority Table: BLE connection lower than WSN data */
StackActivity activityBLE_bleLwsnH[ACTIVITY_NUM_BLE*PRIORITY_NUM] =
{

    DMM_GLOBAL_PRIORITY(DMM_BLE_CONNECTION, DMM_StackPNormal, 80),
    DMM_GLOBAL_PRIORITY(DMM_BLE_CONNECTION, DMM_StackPHigh, 170),
    DMM_GLOBAL_PRIORITY(DMM_BLE_CONNECTION, DMM_StackPUrgent, 250),

    DMM_GLOBAL_PRIORITY(DMM_BLE_CON_EST, DMM_StackPNormal, 80),
    DMM_GLOBAL_PRIORITY(DMM_BLE_CON_EST, DMM_StackPHigh, 200),
    DMM_GLOBAL_PRIORITY(DMM_BLE_CON_EST, DMM_StackPUrgent, 220),

    DMM_GLOBAL_PRIORITY(DMM_BLE_BROADCASTING, DMM_StackPNormal, 70),
    DMM_GLOBAL_PRIORITY(DMM_BLE_BROADCASTING, DMM_StackPHigh, 160),
    DMM_GLOBAL_PRIORITY(DMM_BLE_BROADCASTING, DMM_StackPUrgent, 210),

    DMM_GLOBAL_PRIORITY(DMM_BLE_OBSERVING, DMM_StackPNormal, 70),
    DMM_GLOBAL_PRIORITY(DMM_BLE_OBSERVING, DMM_StackPHigh, 160),
    DMM_GLOBAL_PRIORITY(DMM_BLE_OBSERVING, DMM_StackPUrgent, 210),
};

StackActivity activityWSN_bleLwsnH[ACTIVITY_NUM_WSN*PRIORITY_NUM] =
{

    DMM_GLOBAL_PRIORITY(DMM_WSN_RETRANSMIT, DMM_StackPNormal, 90),
    DMM_GLOBAL_PRIORITY(DMM_WSN_RETRANSMIT, DMM_StackPHigh, 180),
    DMM_GLOBAL_PRIORITY(DMM_WSN_RETRANSMIT, DMM_StackPUrgent, 240),

    DMM_GLOBAL_PRIORITY(DMM_WSN_TRANSMIT, DMM_StackPNormal, 90),
    DMM_GLOBAL_PRIORITY(DMM_WSN_TRANSMIT, DMM_StackPHigh, 180),
    DMM_GLOBAL_PRIORITY(DMM_WSN_TRANSMIT, DMM_StackPUrgent, 240),

    DMM_GLOBAL_PRIORITY(DMM_WSN_RECEIVE, DMM_StackPNormal, 90),
    DMM_GLOBAL_PRIORITY(DMM_WSN_RECEIVE, DMM_StackPHigh, 180),
    DMM_GLOBAL_PRIORITY(DMM_WSN_RECEIVE, DMM_StackPUrgent, 240),
};


/* the order of stacks in policy table and global table must be the same */
GlobalTable globalPriorityTable_bleLwsnH[DMMPOLICY_NUM_STACKS] =
{
    {
        .globalTableArray =  activityBLE_bleLwsnH,
        .tableSize = (uint8_t)(ACTIVITY_NUM_BLE*PRIORITY_NUM),
        .stackRole = DMMPolicy_StackRole_BlePeripheral,
    },

    {
        .globalTableArray =  activityWSN_bleLwsnH,
        .tableSize = (uint8_t)(ACTIVITY_NUM_WSN*PRIORITY_NUM),
        .stackRole = DMMPolicy_StackRole_WsnNode,
    },
};
