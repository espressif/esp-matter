/******************************************************************************

 @file dmm_priority_ble_thread.c

 @brief Dual Mode Manager Global Priority for OpenThread Stack and BLE Stack

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
#include "dmm/dmm_priority_ble_thread.h"


/* BLE Activity */
typedef enum
{
    DMM_BLE_CONNECTION    = 0x07D0,     
    DMM_BLE_CON_EST       = 0x03E8,
    DMM_BLE_BROADCASTING  = 0x0BB8,
    DMM_BLE_OBSERVING     = 0x0FA0,
} DMMStackActivityBLE;

/* Thread Activity */
typedef enum
{
    DMM_THREAD_TX_DATA    = 0x0001,
    DMM_THREAD_TX_POLL    = 0x0002,
    DMM_THREAD_RX_POLL    = 0x0003,
    DMM_THREAD_RX_SCAN    = 0x0004,
    DMM_THREAD_RX_IDLE    = 0x0005
} DMMStackActivityThread;

/* Global Priority Table: BLE connection is lower than Thread data */
StackActivity activityBLE_bleLthreadH[ACTIVITY_NUM_BLE*PRIORITY_NUM] =
{
     DMM_GLOBAL_PRIORITY(DMM_BLE_CONNECTION, DMM_StackPNormal, 70),
     DMM_GLOBAL_PRIORITY(DMM_BLE_CONNECTION, DMM_StackPHigh, 170),
     DMM_GLOBAL_PRIORITY(DMM_BLE_CONNECTION, DMM_StackPUrgent, 250),

     DMM_GLOBAL_PRIORITY(DMM_BLE_CON_EST, DMM_StackPNormal, 70),
     DMM_GLOBAL_PRIORITY(DMM_BLE_CON_EST, DMM_StackPHigh, 195),
     DMM_GLOBAL_PRIORITY(DMM_BLE_CON_EST, DMM_StackPUrgent, 220),

     DMM_GLOBAL_PRIORITY(DMM_BLE_BROADCASTING, DMM_StackPNormal, 60),
     DMM_GLOBAL_PRIORITY(DMM_BLE_BROADCASTING, DMM_StackPHigh, 160),
     DMM_GLOBAL_PRIORITY(DMM_BLE_BROADCASTING, DMM_StackPUrgent, 210),

     DMM_GLOBAL_PRIORITY(DMM_BLE_OBSERVING, DMM_StackPNormal, 60),
     DMM_GLOBAL_PRIORITY(DMM_BLE_OBSERVING, DMM_StackPHigh, 160),
     DMM_GLOBAL_PRIORITY(DMM_BLE_OBSERVING, DMM_StackPUrgent, 210),
};


StackActivity activityThread_bleLthreadH[ACTIVITY_NUM_THREAD*PRIORITY_NUM] =
{
     DMM_GLOBAL_PRIORITY(DMM_THREAD_TX_DATA, DMM_StackPNormal, 150),
     DMM_GLOBAL_PRIORITY(DMM_THREAD_TX_DATA, DMM_StackPHigh, 200),
     DMM_GLOBAL_PRIORITY(DMM_THREAD_TX_DATA, DMM_StackPUrgent, 240),

     DMM_GLOBAL_PRIORITY(DMM_THREAD_TX_POLL, DMM_StackPNormal, 100),
     DMM_GLOBAL_PRIORITY(DMM_THREAD_TX_POLL, DMM_StackPHigh, 200),
     DMM_GLOBAL_PRIORITY(DMM_THREAD_TX_POLL, DMM_StackPUrgent, 230),

     DMM_GLOBAL_PRIORITY(DMM_THREAD_RX_POLL, DMM_StackPNormal, 100),
     DMM_GLOBAL_PRIORITY(DMM_THREAD_RX_POLL, DMM_StackPHigh, 200),
     DMM_GLOBAL_PRIORITY(DMM_THREAD_RX_POLL, DMM_StackPUrgent, 230),

     DMM_GLOBAL_PRIORITY(DMM_THREAD_RX_SCAN, DMM_StackPNormal, 90),
     DMM_GLOBAL_PRIORITY(DMM_THREAD_RX_SCAN, DMM_StackPHigh, 190),
     DMM_GLOBAL_PRIORITY(DMM_THREAD_RX_SCAN, DMM_StackPUrgent, 215),

     DMM_GLOBAL_PRIORITY(DMM_THREAD_RX_IDLE, DMM_StackPNormal, 80),
     DMM_GLOBAL_PRIORITY(DMM_THREAD_RX_IDLE, DMM_StackPHigh, 80),
     DMM_GLOBAL_PRIORITY(DMM_THREAD_RX_IDLE, DMM_StackPUrgent, 175),

};

/* the order of stacks in policy table and global table must be the same */
GlobalTable globalPriorityTable_bleLthreadH[DMMPOLICY_NUM_STACKS] =
{
     {  .globalTableArray =  activityBLE_bleLthreadH,
        .tableSize = (uint8_t)(ACTIVITY_NUM_BLE*PRIORITY_NUM),
        .stackRole = DMMPolicy_StackRole_BlePeripheral,
     },

     {  .globalTableArray =  activityThread_bleLthreadH,
        .tableSize = (uint8_t)(ACTIVITY_NUM_THREAD*PRIORITY_NUM),
        .stackRole = DMMPolicy_StackRole_threadFtd,
     },
};
