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
    ProvisionAttr_PanId,
    ProvisionAttr_ExtPanId,
    ProvisionAttr_Freq,
    ProvisionAttr_SensorChannelMask,
    ProvisionAttr_FFDAddr,
    ProvisionAttr_NtwkKey,
    ProvisionAttr_ProvState
} RemoteDisplay_ProvisionAttr_t;

typedef enum
{
    LightAttr_Light_OnOff,
    LightAttr_Target_Addr_Type,
    LightAttr_Target_Addr,
    LightAttr_Target_Endpoint
} RemoteDisplayLightAttr_t;

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
    /** Started as Zigbee Coordinator */
    RemoteDisplay_DevState_COORD_STARTING = 8,
    /** Started as Zigbee Coordinator */
    RemoteDisplay_DevState_DEV_ZB_COORD = 9,
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
typedef void (*RemoteDisplay_provisionConnectCb_t)(void);

//! \brief Callback function type for stopping a provisioned device
typedef void (*RemoteDisplay_provisionDisconnectCb_t)(void);

//! \brief Structure for sensor provisioning callbacks
typedef struct
{
    RemoteDisplay_setProvisioningAttrCb_t setProvisioningAttrCb;
    RemoteDisplay_getProvisioningAttrCb_t getProvisioningAttrCb;
    RemoteDisplay_provisionConnectCb_t provisionConnectCb;
    RemoteDisplay_provisionDisconnectCb_t provisionDisconnectCb;
} RemoteDisplay_clientProvisioningtCbs_t;

//! \brief Callback function type for getting the remote display attributes
typedef void (*RemoteDisplay_setLightAttrCb_t)(RemoteDisplayLightAttr_t LightAttr, void *const value, uint8_t len);

//! \brief Callback function type for getting the remote display attributes
typedef void (*RemoteDisplay_getLightAttrCb_t)(RemoteDisplayLightAttr_t LightAttr, void *value, uint8_t len);

//! \brief Structusre for sensor remote display callbacks
typedef struct
{
    RemoteDisplay_setLightAttrCb_t setLightAttrCb;
    RemoteDisplay_getLightAttrCb_t getLightAttrCb;
} RemoteDisplay_LightCbs_t;

//! \brief Structure for Zigbee network device info
typedef struct
{
    uint16_t devAddr;
    uint16_t parentAddr;
    uint8_t lightEndPoint;
    int8_t rssi;
} RemoteDisplay_zigbeeDeviceInfo_t;


//! \brief Structure for Zigbee network device info
typedef struct
{
    uint16_t devAddr;
    uint16_t parentAddr;
    uint8_t sensorData;
    int8_t rssi;
    uint8_t deviceStatus;
} RemoteDisplay_ti15_4StackDeviceInfo_t;

//! \brief Union for Device Info
union RemoteDisplay_DeviceInfo_t
{
    RemoteDisplay_zigbeeDeviceInfo_t zigbeeDeviceInfo;
    RemoteDisplay_ti15_4StackDeviceInfo_t ti15_4StackDeviceInfo;
};

//! \brief Structure for sensor provisioning callbacks
typedef struct
{
    RemoteDisplay_setProvisioningAttrCb_t setProvisioningAttrCb;
    RemoteDisplay_getProvisioningAttrCb_t getProvisioningAttrCb;

} RemoteDisplay_networkDeviceCbs_t;

//! \brief Callback function type for getting device info from zigbee coord
typedef void (*RemoteDisplay_networkDeviceCb_t)(uint16_t, union RemoteDisplay_DeviceInfo_t*);



/*********************************************************************
*   @fn RemoteDisplay_registerClientProvCbs
*
*   @brief Register the commissioning client callbacks
*/
extern void RemoteDisplay_registerClientProvCbs(RemoteDisplay_clientProvisioningtCbs_t clientCbs);

/*********************************************************************
*   @fn RemoteDisplay_registerclientProvCbs
*
*   @brief Register the commissioning client callbacks
*/
extern void RemoteDisplay_registerLightCbs(RemoteDisplay_LightCbs_t clientCbs);

/*********************************************************************
*  @fn RemoteDisplay_registerNetworkDeviceCb
*
*  @brief Register the clients network device callback
*/
extern void RemoteDisplay_registerNetworkDeviceCb(RemoteDisplay_networkDeviceCb_t networkDeviceCb);

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
 *  @fn      RemoteDisplay_updateLightProfData
 *
 *  @brief   Sets event to synchronize the Zigbee/BLE application light state.
 */
extern void RemoteDisplay_updateLightProfData();

/*********************************************************************
 *  @fn      RemoteDisplay_updateLightProfData
 *
 *  @brief   Sets event to synchronize the Zigbee/BLE application light state.
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
