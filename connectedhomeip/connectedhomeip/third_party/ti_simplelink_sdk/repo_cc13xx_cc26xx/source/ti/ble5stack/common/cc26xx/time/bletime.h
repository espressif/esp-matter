/******************************************************************************

 @file  bletime.h

 @brief This file contains GATT service discovery and configuration definitions
        and prototypes for discovering
        Time services.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2014-2022, Texas Instruments Incorporated
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

#ifndef BLETIME_H
#define BLETIME_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */

// Time discovery states
enum
{
  DISC_IDLE = 0x00,                       // Idle state

  DISC_CURR_TIME_START = 0x10,            // Current time service
  DISC_CURR_TIME_SVC,                     // Discover service
  DISC_CURR_TIME_CHAR,                    // Discover all characteristics
  DISC_CURR_TIME_CT_TIME_CCCD,            // Discover CT time CCCD

  DISC_DST_CHG_START = 0x20,              // DST change service
  DISC_DST_CHG_SVC,                       // Discover service
  DISC_DST_CHG_CHAR,                      // Discover all characteristics

  DISC_REF_TIME_START = 0x30,             // Reference time service
  DISC_REF_TIME_SVC,                      // Discover service
  DISC_REF_TIME_CHAR,                     // Discover all characteristics

  DISC_NWA_START = 0x40,                  // NwA service
  DISC_NWA_SVC,                           // Discover service
  DISC_NWA_CHAR,                          // Discover all characteristics
  DISC_NWA_NWA_CCCD,                      // Discover NwA CCCD

  DISC_ALERT_NTF_START = 0x50,            // Alert notification service
  DISC_ALERT_NTF_SVC,                     // Discover service
  DISC_ALERT_NTF_CHAR,                    // Discover all characteristics
  DISC_ALERT_NTF_NEW_CCCD,                // Discover new alert CCCD
  DISC_ALERT_NTF_UNREAD_CCCD,             // Discover unread alert status CCCD

  DISC_BATT_START = 0x60,                 // Battery service
  DISC_BATT_SVC,                          // Discover service
  DISC_BATT_CHAR,                         // Discover all characteristics
  DISC_BATT_LVL_CCCD,                     // Discover battery level CCCD

  DISC_PAS_START = 0x70,                  // Phone alert status service
  DISC_PAS_SVC,                           // Discover service
  DISC_PAS_CHAR,                          // Discover all characteristics
  DISC_PAS_ALERT_CCCD,                    // Discover alert status CCCD
  DISC_PAS_RINGER_CCCD,                   // Discover ringer setting CCCD

  DISC_FAILED = 0xFF                      // Discovery failed
};

// Time handle cache indexes
enum
{
  HDL_CURR_TIME_CT_TIME_START,            // Current time start handle
  HDL_CURR_TIME_CT_TIME_END,              // Current time end handle
  HDL_CURR_TIME_CT_TIME_CCCD,             // Current time CCCD
  HDL_CURR_TIME_LOC_INFO,                 // Local time info
  HDL_CURR_TIME_REF_INFO,                 // Reference time info

  HDL_DST_CHG_TIME_DST,                   // Time with DST

  HDL_REF_TIME_UPD_CTRL,                  // Time update control point
  HDL_REF_TIME_UPD_STATE,                 // Time update state

  HDL_NWA_NWA_START,                      // NwA start handle
  HDL_NWA_NWA_END,                        // NwA end handle
  HDL_NWA_NWA_CCCD,                       // NwA CCCD

  HDL_ALERT_NTF_NEW_START,                // New alert start handle
  HDL_ALERT_NTF_NEW_END,                  // New alert end handle
  HDL_ALERT_NTF_UNREAD_START,             // Unread alert status start handle
  HDL_ALERT_NTF_UNREAD_END,               // Unread alert status end handle
  HDL_ALERT_NTF_CTRL,                     // Alert notification control point
  HDL_ALERT_NTF_NEW_CAT,                  // Supported New Alert Category
  HDL_ALERT_NTF_UNREAD_CAT,               // Supported Unread Alert Category
  HDL_ALERT_NTF_NEW_CCCD,                 // New alert CCCD
  HDL_ALERT_NTF_UNREAD_CCCD,              // Alert unread alert status CCCD

  HDL_BATT_LEVEL_START,                   // Battery level start handle
  HDL_BATT_LEVEL_END,                     // Battery level end handle
  HDL_BATT_LEVEL_CCCD,                    // Battery level CCCD

  HDL_PAS_ALERT_START,                    // Alert status start handle
  HDL_PAS_ALERT_END,                      // Alert status end handle
  HDL_PAS_RINGER_START,                   // Ringer setting start handle
  HDL_PAS_RINGER_END,                     // Ringer setting end handle
  HDL_PAS_CTRL,                           // Ringer control point
  HDL_PAS_ALERT_CCCD,                     // Alert status CCCD
  HDL_PAS_RINGER_CCCD,                    // Ringer setting CCCD

  HDL_CACHE_LEN
};

// Configuration states
#define TIME_CONFIG_START              0x00
#define TIME_CONFIG_CMPL               0xFF

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * GLOBAL
 */

// Connection handle
extern uint16_t Time_connHandle;

// Handle cache
extern uint16_t Time_handleCache[HDL_CACHE_LEN];

// Task ID
extern uint8_t Time_configDone;

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Time service discovery functions.
 */
extern uint8_t Time_discStart(void);
extern uint8_t Time_discGattMsg(uint8_t state, gattMsgEvent_t *pMsg);

/*
 * Time characteristic configuration functions.
 */
extern uint8_t Time_configNext(uint8_t state);
extern uint8_t Time_configGattMsg(uint8_t state, gattMsgEvent_t *pMsg);

/*
 * Time indication and notification handling functions.
 */
extern void Time_indGattMsg(gattMsgEvent_t *pMsg);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* BLETIME_H */
