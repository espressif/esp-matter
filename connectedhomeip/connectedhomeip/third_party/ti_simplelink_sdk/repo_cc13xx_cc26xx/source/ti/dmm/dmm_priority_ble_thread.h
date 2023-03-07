/******************************************************************************

 @file dmm_priority_ble_zigee.h

 @brief Global Priority DMM use cases

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
 /*!****************************************************************************
 *  @file  dmm_priority_ble_thread.h
 *
 *  @brief      Global Priority Table
 *
 *  DMM enables devices to run multiple wireless protocol stacks concurrently.
 *  The DMMSch is to enable concurrent operation of multiple stacks with minimized conflicts 
 *  so that it does not cause significant performance degradation.  
 *  The DMMSch uses Application Level, Stack Level information, and Global Priority Table (GPT) when scheduling a command. 
 *  Stack Level information (provided by stack) is embedded in each RF command and it includes:
 *  - Start Type, Start Time, AllowDelay, Priority, Activity, etc.  
 *  Application Level information (provided by User via the Policy Table) includes:
 *  - Application State Name, Weight, AppliedActivity, Pause, etc.   
 *  Global Priority Table (GPT)
 *  - GPT defines relative priorities of the two stacks
 *  - GPT consists of three parameters: Stack Activity, Priority of the activity (Normal, High, Urgent), and Global Priority Number
 *  - GPT input: Stack Activity  + Priority of the activity (Normal, High, Urgent)
 *  - GPT output: Global Priority Number
 *  Final Priority = GPT (Stack Activity  + Priority of the activity) +  .weight (in the Policy Table)
 *  
 *  # GPT for BLE Stack + Thread Stack #
 *  ___________________________________________________________________________________________________
 *  | BLE Activity + Priority        | 15.4 Activity + Priority         | GTP value
 *  ____________________________________________________________________________________________________
 *  | Connect             - Normal   |                                  | 75
 *  ____________________________________________________________________________________________________
 *
 *  # GPT APIs are defined in the dmm_policy.c#
 *    - DMMPolicy_getGlobalPriority(): Get the global activity based on stack activity.
 *    - DMMPolicy_getDefaultPriority(): Get the default priority of the stack
 *    - DMMPolicy_getGPTStatus():  check if the global priority table is available
 *
 *
 * ********************************************************************************/
#ifndef dmm_priority_ble_thread__H_
#define dmm_priority_ble_thread__H_

#include "dmm_policy.h"

//! \brief The number of activities 
#define ACTIVITY_NUM_BLE                        4
#define ACTIVITY_NUM_THREAD                     5

extern GlobalTable globalPriorityTable_bleLthreadH[DMMPOLICY_NUM_STACKS];

#endif //dmm_priority_ble_thread__H_
