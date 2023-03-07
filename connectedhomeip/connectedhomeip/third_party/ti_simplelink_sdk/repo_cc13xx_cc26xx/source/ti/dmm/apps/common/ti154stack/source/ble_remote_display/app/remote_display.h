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

#ifdef DMM_SENSOR
#include "jdllc.h"
#elif DMM_COLLECTOR
#include "cllc.h"
#endif

#define RemoteDisplay_JOIN_STATE_SYNC_LOSS 7

#define RemoteDisplay_JOIN_STATE_SYNC_LOSS  7

typedef enum
{
    ProvisionAttr_PanId,
    ProvisionAttr_ExtPanId,
    ProvisionAttr_Freq,
    ProvisionAttr_SensorChannelMask,
    ProvisionAttr_FFDAddr,
    ProvisionAttr_NtwkKey,
    ProvisionAttr_ProvState
} ProvisionAttr_t;

typedef enum
{
    RemoteDisplayAttr_ReportInterval,
    RemoteDisplayAttr_CollLed,
#ifdef DMM_SENSOR
    RemoteDisplayAttr_SensorData
#elif DMM_COLLECTOR
    RemoteDisplayAttr_SensorAddr,
    RemoteDisplayAttr_SensorDisassociate,
#endif
} RemoteDisplayAttr_t;

//! \brief Callback function type for getting the provisioning attributes
typedef void (*RemoteDisplay_setProvisioningAttrCb_t)(ProvisionAttr_t provisioningAttr, void *const value, uint8_t len);

//! \brief Callback function type for getting the provisioning attributes
typedef void (*RemoteDisplay_getProvisioningAttrCb_t)(ProvisionAttr_t provisioningAttr, void *value, uint8_t len);

#ifdef DMM_COLLECTOR
//! \brief Callback function type for starting a collector network
typedef void (*RemoteDisplay_provisionNetworkStartCb_t)(bool ntwkOpen);
#endif
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
#ifdef DMM_SENSOR
    RemoteDisplay_provisionDisconnectCb_t provisionDisconnectCb;
#endif
#ifdef  DMM_COLLECTOR
    RemoteDisplay_provisionNetworkStartCb_t provisionNtwkOpenCloseCb;
#endif
} RemoteDisplay_clientProvisioningtCbs_t;

//! \brief Callback function type for getting the remote display attributes
typedef void (*RemoteDisplay_setRDAttrCb_t)(RemoteDisplayAttr_t remoteDisplayAttr, void *const value, uint8_t len);

//! \brief Callback function type for getting the remote display attributes
typedef void (*RemoteDisplay_getRDAttrCb_t)(RemoteDisplayAttr_t remoteDisplayAttr, void *value, uint8_t len);

//! \brief Structure for sensor remote display callbacks
typedef struct
{
    RemoteDisplay_setRDAttrCb_t setRDAttrCb;
    RemoteDisplay_getRDAttrCb_t getRDAttrCb;
} RemoteDisplayCbs_t;

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

//! \brief Callback function type for getting device info from collector
typedef void (*RemoteDisplay_networkDeviceCb_t)(uint16_t, union RemoteDisplay_DeviceInfo_t*);

#ifdef DISPLAY_PER_STATS
//! \brief Structure for PER
typedef struct
{
    unsigned int successes;
    unsigned int failures;
    float per;
} RemoteDisplay_perData_t;
#endif

/*********************************************************************
*  @fn RemoteDisplay_registerClientProvCbs
*
*  @brief Register the commissioning client callbacks
*/
extern void RemoteDisplay_registerClientProvCbs(RemoteDisplay_clientProvisioningtCbs_t clientCbs);

/*********************************************************************
*  @fn RemoteDisplay_registerclientProvCbs
*
*  @brief Register the commissioning client callbacks
*/
extern void RemoteDisplay_registerRDCbs(RemoteDisplayCbs_t clientCbs);

/*********************************************************************
*  @fn RemoteDisplay_updateSensorData
*
*  @brief Update sensor data
*/
extern void RemoteDisplay_updateSensorData(void);

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
 * @brief   Task creation function for the Remote Display Peripheral.
 */
extern void RemoteDisplay_createTask(void);

/*********************************************************************
 *  @fn      RemoteDisplay_updateSensorJoinState
 *
 * @brief   Sets the sensors sensor reading characteristic
 */
#ifdef DMM_SENSOR
extern void RemoteDisplay_updateSensorJoinState(Jdllc_states_t state);
#elif DMM_COLLECTOR
extern void RemoteDisplay_updateCollectorJoinState(Cllc_states_t state);
#endif

/*********************************************************************
 *  @fn      RemoteDisplay_bleFastStateUpdateCb
 *
 * @brief   Callback from BLE link layer to indicate a state change
 */
extern void RemoteDisplay_bleFastStateUpdateCb(uint32_t StackRole, uint32_t stackState);

#ifdef FEATURE_SECURE_COMMISSIONING
/*********************************************************************
* @fn      RemoteDisplay_updateSmState
*
* @brief   Update the Remote Display SM state machine
*
* @param   smState - the SM State being set
*
* @return  None
*/
extern void RemoteDisplay_updateSmState(uint8_t pState);

/*********************************************************************
* @fn      RemoteDisplay_getBleAuthConnectionStatus
*
* @brief   Determine if the device currently has a bonded connection
*
* @return  bool - True if at least one bonded connection exists
*/
extern bool RemoteDisplay_getBleAuthConnectionStatus(void);

/*********************************************************************
* @fn      RemoteDisplay_checkSmAccessAllowed
*
* @brief   update the Connection Bond Status of a specific connection handle
*
* @param   connectionHandle - BLE connection handle to update
*
* @return  bool - True if the BLE connection has access to SM characteristics
*/
extern bool RemoteDisplay_checkSmAccessAllowed(uint16_t connectionHandle);
#endif /* FEATURE_SECURE_COMMISSIONING */

/*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* REMOTE_DISPLAY_H */
