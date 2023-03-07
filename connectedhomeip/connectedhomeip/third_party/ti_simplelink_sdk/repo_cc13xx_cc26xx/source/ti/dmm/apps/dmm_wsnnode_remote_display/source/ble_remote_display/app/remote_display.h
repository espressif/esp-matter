/******************************************************************************

 @file  remote_display.h

 @brief This file contains the Remote Display BLE application
        definitions and prototypes.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2013-2021, Texas Instruments Incorporated
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

#ifndef REMOTE_DISPLAY_H
#define REMOTE_DISPLAY_H

#include "ti_ble_config.h"

#ifdef __cplusplus
extern "C"
{
#endif


typedef enum
{
    RemoteDisplayAttr_NodeReportInterval,
    RemoteDisplayAttr_ConcLed,
    RemoteDisplayAttr_NodeData,
    RemoteDisplayAttr_NodeAddress,
} RemoteDisplayAttr_t;

//! \brief Callback function type for getting the remote display attributes
typedef void (*RemoteDisplay_setRDAttrCb_t)(RemoteDisplayAttr_t remoteDisplayAttr, void *const value, uint8_t len);

//! \brief Callback function type for getting the remote display attributes
typedef void (*RemoteDisplay_getRDAttrCb_t)(RemoteDisplayAttr_t remoteDisplayAttr, void *value, uint8_t len);


//! \brief Structure for node remote display callbacks
typedef struct
{
    RemoteDisplay_setRDAttrCb_t setRDAttrCb;
    RemoteDisplay_getRDAttrCb_t getRDAttrCb;
} RemoteDisplayCbs_t;

//! \brief Structure for node stats
typedef struct {
  uint16_t dataSendSuccess;
  uint16_t dataSendFail;
  uint16_t dataTxSchError;
  uint16_t ackRxAbort;
  uint16_t ackRxTimeout;
  uint16_t ackRxSchError;
} RemoteDisplay_nodeWsnStats_t;

/*********************************************************************

*@fn RemoteDisplay_registerclientProvCbs
*@brief Register the commissioning client callbacks
*/
extern void RemoteDisplay_registerRDCbs(RemoteDisplayCbs_t clientCbs);


/*********************************************************************
 *  @fn      RemoteDisplay_syncProvAttr
 *
 * @brief   Helper function to initialize all provisioning profile characteristic
 *          values based on the 15.4 application.
 */
extern void RemoteDisplay_syncRDAttr(void);

/*********************************************************************
 *  @fn      RemoteDisplay_createTask
 *
 * @brief   Task creation function for the Remote Display Peripheral.
 */
extern void RemoteDisplay_createTask(void);


/*********************************************************************
 *  @fn      RemoteDisplay_setNodeSensorReading
 *
 * @brief   Updates the sensor data within the remote display profile
 */
extern void RemoteDisplay_updateNodeData(void);

/*********************************************************************
 *  @fn      RemoteDisplay_setNodeSensorReading
 *
 * @brief   Sets the nodes sensor reading characteristic
 */
extern void RemoteDisplay_updateNodeWsnStats(RemoteDisplay_nodeWsnStats_t* stats);

/*********************************************************************
 *  @fn      RemoteDisplay_bleFastStateUpdateCb
 *
 * @brief   Callback from BLE link layer to indicate a state change
 */
extern void RemoteDisplay_bleFastStateUpdateCb(uint32_t StackRole, uint32_t stackState);

/*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* REMOTE_DISPLAY_H */
