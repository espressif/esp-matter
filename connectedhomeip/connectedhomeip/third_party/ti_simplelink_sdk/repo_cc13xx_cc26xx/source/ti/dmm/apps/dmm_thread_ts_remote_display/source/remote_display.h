/******************************************************************************

 @file  remote_display.h

 @brief This file contains the Remote Display BLE application
        definitions and prototypes.

 Group: WCS, BTS
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

#ifndef REMOTE_DISPLAY_H
#define REMOTE_DISPLAY_H

#include "ti_ble_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
    ProvisionAttr_PanId,
    ProvisionAttr_ExtPanId,
    ProvisionAttr_Freq,
    ProvisionAttr_SensorChannelMask,
    ProvisionAttr_FFDAddr,
    ProvisionAttr_NtwkKey,
    ProvisionAttr_NtwkName,
    ProvisionAttr_ShortAddr,
    ProvisionAttr_Role,
    ProvisionAttr_PSKd,
} RemoteDisplay_ProvisionAttr_t;

typedef enum
{
    SensorAttr_Temperature,
    SensorAttr_Report_Interval
} RemoteDisplay_SensorAttr_t;

/**
 * Local copy of the zstack_DevState enum located within zstack.h
 */
typedef enum
{
    /** Initialized - not started automatically */
    RemoteDisplay_DevState_HOLD = 0,
    /** Initialized - not connected to anything */
    RemoteDisplay_DevState_INIT = 1,
    /** Discovering PAN's to join */
    RemoteDisplay_DevState_NWK_DISC = 2,
    /** Joining a PAN */
    RemoteDisplay_DevState_NWK_JOINING = 3,
    /**
     * ReJoining a PAN in secure mode scanning in current channel,
     * only for end devices
     */
    RemoteDisplay_DevState_NWK_REJOIN_SEC_CURR_CHANNEL = 4,
    /** Joined but not yet authenticated by trust center */
    RemoteDisplay_DevState_END_DEVICE_UNAUTH = 5,
    /** Started as device after authentication */
    RemoteDisplay_DevState_DEV_END_DEVICE = 6,
    /** Device joined, authenticated and is a router */
    RemoteDisplay_DevState_DEV_ROUTER = 7,
    /** Device has lost information about its parent */
    RemoteDisplay_DevState_NWK_ORPHAN = 10,
    /** Device is sending KeepAlive message to its parent */
    RemoteDisplay_DevState_NWK_KA = 11,
    /** Device is waiting before trying to rejoin */
    RemoteDisplay_DevState_NWK_BACKOFF = 12,
    /**
     * ReJoining a PAN in secure mode scanning in all channels,
     * only for end devices
     */
    RemoteDisplay_DevState_NWK_REJOIN_SEC_ALL_CHANNEL = 13,
    /**
     * ReJoining a PAN in unsecure mode scanning in current channel,
     * only for end devices
     */
    RemoteDisplay_DevState_NWK_TC_REJOIN_CURR_CHANNEL = 14,
    /**
     * ReJoining a PAN in unsecure mode scanning in all channels,
     * only for end devices
     */
    RemoteDisplay_DevState_NWK_TC_REJOIN_ALL_CHANNEL = 15,
} RemoteDisplay_DevState;

//! \brief Callback function type for getting the provisioning attributes
typedef void (*RemoteDisplay_setProvisioningAttrCb_t)(RemoteDisplay_ProvisionAttr_t provisioningAttr, void *const value, uint8_t len);

//! \brief Callback function type for getting the provisioning attributes
typedef void (*RemoteDisplay_getProvisioningAttrCb_t)(RemoteDisplay_ProvisionAttr_t provisioningAttr, void *value, uint8_t len);

//! \brief Callback function type for starting provisioning of a device
typedef void (*RemoteDisplay_provisionConnectCb_t)(uint8_t connectType);

//! \brief Callback function type for resetting the device
typedef void (*RemoteDisplay_networkResetCb_t)(uint8_t resetType);

//! \brief Structure for sensor provisioning callbacks
typedef struct
{
    RemoteDisplay_setProvisioningAttrCb_t setProvisioningAttrCb;
    RemoteDisplay_getProvisioningAttrCb_t getProvisioningAttrCb;
    RemoteDisplay_provisionConnectCb_t provisionConnectCb;
    RemoteDisplay_networkResetCb_t networkResetCb;
} RemoteDisplay_clientProvisioningCbs_t;

//! \brief Callback function type for getting the remote display attributes
typedef void (*RemoteDisplay_setSensorAttrCb_t)(RemoteDisplay_SensorAttr_t SensorAttr, void *const value, uint8_t len);

//! \brief Callback function type for getting the remote display attributes
typedef void (*RemoteDisplay_getSensorAttrCb_t)(RemoteDisplay_SensorAttr_t SensorAttr, void *value, uint8_t len);

//! \brief Structure for sensor remote display callbacks
typedef struct
{
    RemoteDisplay_setSensorAttrCb_t setSensorAttrCb;
    RemoteDisplay_getSensorAttrCb_t getSensorAttrCb;
} RemoteDisplay_SensorCbs_t;



/*********************************************************************
*   @fn RemoteDisplay_registerClientProvCbs
*
*   @brief Register the commissioning client callbacks
*/
extern void RemoteDisplay_registerClientProvCbs(RemoteDisplay_clientProvisioningCbs_t clientCbs);
/*********************************************************************
*   @fn RemoteDisplay_registerSensorCbs
*
*   @brief Register the sensor callbacks
*/
extern void RemoteDisplay_registerSensorCbs(RemoteDisplay_SensorCbs_t sensorCbs);

/*********************************************************************
*  @fn RemoteDisplay_deviceUpdate
*
*  @brief Update device data
*/
extern void RemoteDisplay_deviceUpdate(uint16_t devAddr);

/*********************************************************************
 *  @fn      RemoteDisplay_createTask
 *
 *  @brief   Task creation function for the Remote Display Peripheral.
 */
extern void RemoteDisplay_createTask(void);

/*********************************************************************
 *  @fn      RemoteDisplay_updateJoinState
 *
 *  @brief   Sets the join state characteristic
 */
extern void RemoteDisplay_updateJoinState(RemoteDisplay_DevState state);

/*********************************************************************
 *  @fn      RemoteDisplay_updateSensorProfData
 *
 *  @brief   Sets event to synchronize the Thread/BLE application sensor data
 */
extern void RemoteDisplay_updateSensorProfData();

/*********************************************************************
 *  @fn      RemoteDisplay_updateProvProfData
 *
 *  @brief   Sets event to synchronize the Thread/BLE application provisioning data
 */
extern void RemoteDisplay_updateProvProfData();

/*********************************************************************
 *  @fn      RemoteDisplay_bleFastStateUpdateCb
 *
 *  @brief   Callback from BLE link layer to indicate a state change
 */
extern void RemoteDisplay_bleFastStateUpdateCb(uint32_t StackRole, uint32_t stackState);

/*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* REMOTE_DISPLAY_H */
