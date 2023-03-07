/******************************************************************************

 @file tempsensor.c

 @brief Temperature Sensor example application

 Group: CMCU, LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2020-2021, Texas Instruments Incorporated
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

/******************************************************************************
 Includes
 *****************************************************************************/
#include <openthread/config.h>
#include <openthread-core-config.h>

/* Standard Library Header files */
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

/* POSIX Header files */
#include <time.h>

/* OpenThread public API Header files */
#include <openthread/coap.h>
#include <openthread/ip6.h>
#include <openthread/link.h>
#include <openthread/platform/uart.h>
#include <openthread/thread.h>

/* driverlib specific header */
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/aon_batmon.h)

/* POSIX Header files */
#include <sched.h>
#include <pthread.h>
#include <mqueue.h>
#include <otstack.h>
#include "task_config.h"
#include <tempsensor.h>

/* RTOS Header files */
#include <ti/drivers/GPIO.h>

/* OpenThread Internal/Example Header files */
#include "otsupport/otrtosapi.h"
#include "otsupport/otinstance.h"

/* Board Header files */
#include "ti_drivers_config.h"

#include "utils/code_utils.h"
#include <disp_utils.h>
#include "tiop_ui.h"

/* Private configuration Header files */
#include "tiop_config.h"

#ifdef USE_DMM
#ifdef BLE_START
#include "remote_display.h"
#include "provisioning_gatt_profile.h"
#endif
#include "ti_dmm_application_policy.h"
#endif

#if BOARD_DISPLAY_USE_UART
/* CUI Header file */
#include "cui.h"
#endif /* BOARD_DISPLAY_USE_UART */

#if (OPENTHREAD_CONFIG_COAP_API_ENABLE == 0)
#error "OPENTHREAD_CONFIG_COAP_API_ENABLE needs to be defined and set to 1"
#endif

/******************************************************************************
 Constants and definitions
 *****************************************************************************/

/* read attribute */
#define ATTR_READ     0x01
/* write attribute */
#define ATTR_WRITE    0x02
/* report attribute */
#define ATTR_REPORT   0x04

#if (OPENTHREAD_CONFIG_DHCP6_CLIENT_ENABLE && OPENTHREAD_ENABLE_DHCP6_MULTICAST_SOLICIT)

/*Expected value for tempsensor for 2 byte device type on DHCPv6 assigned address
Value is written in Little Endian format, so the expected value is read as 0x0002 */

#ifndef TIOP_TEMPSENSOR_DEIVCE_TYPE
#define TIOP_TEMPSENSOR_DEIVCE_TYPE            0x0200
#endif

/*Expected value for thermostat for 2 byte device type on DHCPv6 assigned address
Value is written in Little Endian format, so the expected value is read as 0x0002 */

#ifndef TIOP_THERMOSTAT_DEIVCE_TYPE
#define TIOP_THERMOSTAT_DEIVCE_TYPE            0x0100
#endif

/*Expected value for thermostat for 2 byte device ID on DHCPv6 assigned address
Value is written in Little Endian format, so the expected value is read as 0x0002*/

#ifndef TIOP_THERMOSTAT_DEIVCE_ID
#define TIOP_THERMOSTAT_DEIVCE_ID            0x0100
#endif

#endif

/* Reporting interval in milliseconds */
#ifndef USE_DMM
#ifndef TIOP_TEMPSENSOR_REPORTING_INTERVAL
#define TIOP_TEMPSENSOR_REPORTING_INTERVAL 10000
#endif
#else
static uint32_t reportingInterval = 2000;
#endif

/* Manually assigned address of thermostat */
#ifndef TIOP_THERMOSTAT_ADDRESS
#define TIOP_THERMOSTAT_ADDRESS            "ff03::1"
#endif

/* Default address to report temperature */
#ifndef TIOP_TEMPSENSOR_REPORTING_ADDRESS
#define TIOP_TEMPSENSOR_REPORTING_ADDRESS  "ff02::1"
#endif

#define DEFAULT_COAP_HEADER_TOKEN_LEN      2

/**
 * @brief Size of the message queue for `TempSensor_procQueue`
 *
 * There are 7/10 events that can be raised, it is unlikely that they will all be
 * raised at the same time. Add one buffer queue element.
 */
#if TIOP_CUI
#define TEMPSENSOR_PROC_QUEUE_MAX_MSG     (11)
#else
#define TEMPSENSOR_PROC_QUEUE_MAX_MSG     (8)
#endif /* TIOP_CUI */

/* coap attribute descriptor */
typedef struct
{
    const char*    uriPath; /* attribute URI */
    uint16_t       type;    /* type of resource: read only or read write */
    uint8_t*       pValue;  /* pointer to value of attribute state */

} attrDesc_t;


struct TempSensor_procQueueMsg
{
    TempSensor_evt evt;
};


/******************************************************************************
 Local variables
 *****************************************************************************/

/* Timer ID */
static timer_t reportTimerID;

/* Static IPv6 address assigned to the thermostat */
static otIp6Address thermostatAddress;

/* IPv6 address to send the reporting temperature to */
static otIp6Address reportingAddress;

/* Port to report the temperature to */
static uint16_t peerPort = OT_DEFAULT_COAP_PORT;

/* POSIX message queue for passing events to the application processing loop */
const  char  TempSensor_procQueueName[] = "ts_process";
static mqd_t TempSensor_procQueueDesc;

/* coap resource for the application */
static otCoapResource coapResource;

/* coap attribute state of the application */
static uint8_t attrTemperature[11] = "70";
static int temperatureValue = 70;

/* coap attribute descriptor for the application */
const attrDesc_t coapAttr = {
                             TEMPSENSOR_TEMP_URI,
                             (ATTR_READ|ATTR_REPORT),
                             attrTemperature,
};

static Button_Handle rightButtonHandle;

/* Holds the server setup state: 1 indicates CoAP server has been setup */
static bool serverSetup;

/* String variable for copying over app lines to CUI */
static char statusBuf[MAX_STATUS_LINE_VALUE_LEN];

/******************************************************************************
 Function Prototype
 *****************************************************************************/

/* Temperature Sensor processing thread. */
static void *TempSensor_task(void *arg0);
/* Timeout callback for reporting. */
static void reportingTimeoutCB(union sigval val);


/***
 * Referenced Externals
 */
#ifdef USE_DMM
static otPanId panId;
static otExtendedPanId extPanId;
static otChannelMask chanMask;
static uint8_t channel;
static otMasterKey masterKey;
static otNetworkName networkName;
static otShortAddress   shortAddr;
static otExtAddress     extAddress;
static otDeviceRole     role;
//Has the change mask for updating parameters
static uint32_t deltaMask;
void updateNetworkParams(void);
#ifdef BLE_START

/*!
 * @brief      Get the high byte of a uint16_t variable
 *
 * @param      a - uint16_t variable
 *
 * @return     high byte
 */
uint8_t Util_hiUint16(uint16_t a);

/*!
 * @brief      Get the low byte of a uint16_t variable
 *
 * @param      a - uint16_t variable
 *
 * @return     low byte
 */
uint8_t Util_loUint16(uint16_t a);

/*!
 * @brief      Build a uint16_t out of 2 uint8_t variables
 *
 * @param      loByte - low byte
 * @param      hiByte - high byte
 *
 * @return     combined uint16_t
 */
uint16_t Util_buildUint16(uint8_t loByte, uint8_t hiByte);

/*!
 * @brief      Build a uint32_t out of 4 uint8_t variables
 *
 * @param      byte0 - byte - 0
 * @param      byte1 - byte - 1
 * @param      byte2 - byte - 2
 * @param      byte3 - byte - 3
 *
 * @return     combined uint32_t
 */
uint32_t Util_buildUint32(uint8_t byte0, uint8_t byte1, uint8_t byte2,
                          uint8_t byte3);

/*!
 * @brief      Pulls 1 uint8_t out of a uint32_t
 *
 * @param      var - uint32_t variable
 * @param      byteNum - what byte to pull out (0-3)
 *
 * @return     uint8_t
 */
uint8_t Util_breakUint32(uint32_t var, int byteNum);
// Provisioning callback functions
static void provisionConnectCb(uint8_t connectType);
static void networkResetCb(uint8_t resetType);
static void setProvisioningCb(RemoteDisplay_ProvisionAttr_t provisioningAttr, void *const value, uint8_t len);
static void getProvisioningCb(RemoteDisplay_ProvisionAttr_t provisioningAttr, void *value, uint8_t len);
RemoteDisplay_clientProvisioningCbs_t ot_ProvisioningCbs =
{
 setProvisioningCb,
 getProvisioningCb,
 provisionConnectCb,
 networkResetCb
};
static void provisionConnectCb(uint8_t connectType)
{
    if(connectType == PROVPROFILE_JOIN){
        uiActionJoin(0);

    }else if(connectType == PROVPROFILE_ATTACH){
        uiActionAttach(0);
    }
}


static void networkResetCb(uint8_t resetType){
    if(resetType == PROVPROFILE_RESETDEVICE)
    {
        OtRtosApi_lock();
        otInstanceFactoryReset(OtInstance_get());
        OtRtosApi_unlock();
    }
    else if (resetType == PROVPROFILE_RESETNETWORK)
    {
        OtRtosApi_lock();
        otInstanceReset(OtInstance_get());
        OtRtosApi_unlock();
    }

}

/** @brief  Set provisioning callback functions
 *
 *  @param  ProvisionAttr_t  Remote display attribute value to set
 *  @param  value  pointer to data from remote dispaly application
 *  @param  len  length of data from remote display application
 */
static void setProvisioningCb(RemoteDisplay_ProvisionAttr_t provisioningAttr,
                              void *const value, uint8_t len)
{
    uint8_t *byteArr = (uint8_t *)value;

    switch(provisioningAttr)
    {
    case ProvisionAttr_PanId:
    {
        OtRtosApi_lock();
        otLinkSetPanId(OtInstance_get(), Util_buildUint16(byteArr[1], byteArr[0]));
        OtRtosApi_unlock();

        break;
    }
    case ProvisionAttr_SensorChannelMask:
    {

        OtRtosApi_lock();

        otLinkSetSupportedChannelMask(OtInstance_get(), Util_buildUint32(byteArr[3], byteArr[2],
                                                                         byteArr[1], byteArr[0]));
        OtRtosApi_unlock();

        break;
    }
    case ProvisionAttr_ExtPanId:
    {
        extPanId.m8[0] = ((uint8_t *)value)[0];
        extPanId.m8[1] = ((uint8_t *)value)[1];
        extPanId.m8[2] = ((uint8_t *)value)[2];
        extPanId.m8[3] = ((uint8_t *)value)[3];
        extPanId.m8[4] = ((uint8_t *)value)[4];
        extPanId.m8[5] = ((uint8_t *)value)[5];
        extPanId.m8[6] = ((uint8_t *)value)[6];
        extPanId.m8[7] = ((uint8_t *)value)[7];
        OtRtosApi_lock();
        otThreadSetExtendedPanId(OtInstance_get(),&extPanId);
        OtRtosApi_unlock();
        break;
    }
    case ProvisionAttr_Freq://channel
    {
        OtRtosApi_lock();
        otLinkSetChannel(OtInstance_get(),*(uint8_t *)value);
        OtRtosApi_unlock();
        break;
    }
    case ProvisionAttr_NtwkName:
    {
        for(int i = 0; i< PROVISIONING_NTWK_NAME_LEN; i++)
            networkName.m8[i] = ((uint8_t *)value)[i];
        OtRtosApi_lock();
        otThreadSetNetworkName(OtInstance_get(),networkName.m8);
        OtRtosApi_unlock();
        break;
    }
    case ProvisionAttr_NtwkKey:
    {
        masterKey.m8[0] = ((uint8_t *)value)[0];
        masterKey.m8[1] = ((uint8_t *)value)[1];
        masterKey.m8[2] = ((uint8_t *)value)[2];
        masterKey.m8[3] = ((uint8_t *)value)[3];
        masterKey.m8[4] = ((uint8_t *)value)[4];
        masterKey.m8[5] = ((uint8_t *)value)[5];
        masterKey.m8[6] = ((uint8_t *)value)[6];
        masterKey.m8[7] = ((uint8_t *)value)[7];
        OtRtosApi_lock();
        otThreadSetMasterKey(OtInstance_get(),&masterKey);
        OtRtosApi_unlock();
        break;
    }
    case ProvisionAttr_FFDAddr:
        //Not writable
    case ProvisionAttr_ShortAddr:
        //not writable
    case ProvisionAttr_Role:
        //not writable
    default:
        // Attribute not found
        break;
    }
}

/** @brief  Get provisioning callback functions
 *
 *  @param  ProvisionAttr_t  Remote display attribute value to set
 *
 *  @return  uint8_t  Current value of data present in 15.4 application
 */
static void getProvisioningCb(RemoteDisplay_ProvisionAttr_t provisioningAttr, void *value, uint8_t len)
{
    switch(provisioningAttr)
    {
#ifndef USEOT
    case ProvisionAttr_ProvState:
    {
        *(uint8_t *)value = provState;
        break;
    }
#endif
    // The PAN ID and Channel mask are reversed in byte order below
    // to allow the Light Blue BLE phone application to parse this data properly.
    case ProvisionAttr_PanId:
    {
        ((uint8_t *)value)[0] = Util_hiUint16(panId);
        ((uint8_t *)value)[1] = Util_loUint16(panId);
        break;
    }
    case ProvisionAttr_SensorChannelMask:
    {
        ((uint8_t *)value)[0] = Util_breakUint32(chanMask, 3);
        ((uint8_t *)value)[1] = Util_breakUint32(chanMask, 2);
        ((uint8_t *)value)[2] = Util_breakUint32(chanMask, 1);
        ((uint8_t *)value)[3] = Util_breakUint32(chanMask, 0);
        break;
    }
    case ProvisionAttr_Freq://channel
    {
        *(uint8_t *)value = channel;
        break;
    }
    case ProvisionAttr_ExtPanId:
    {
        ((uint8_t *)value)[0] = extPanId.m8[0];
        ((uint8_t *)value)[1] = extPanId.m8[1];
        ((uint8_t *)value)[2] = extPanId.m8[2];
        ((uint8_t *)value)[3] = extPanId.m8[3];
        ((uint8_t *)value)[4] = extPanId.m8[4];
        ((uint8_t *)value)[5] = extPanId.m8[5];
        ((uint8_t *)value)[6] = extPanId.m8[6];
        ((uint8_t *)value)[7] = extPanId.m8[7];


        break;
    }
    case ProvisionAttr_FFDAddr://EUI64
    {
        ((uint8_t *)value)[0] = extAddress.m8[0];
        ((uint8_t *)value)[1] = extAddress.m8[1];
        ((uint8_t *)value)[2] = extAddress.m8[2];
        ((uint8_t *)value)[3] = extAddress.m8[3];
        ((uint8_t *)value)[4] = extAddress.m8[4];
        ((uint8_t *)value)[5] = extAddress.m8[5];
        ((uint8_t *)value)[6] = extAddress.m8[6];
        ((uint8_t *)value)[7] = extAddress.m8[7];
        break;
    }
    case ProvisionAttr_NtwkKey:
    {
        ((uint8_t *)value)[0] = masterKey.m8[0];
        ((uint8_t *)value)[1] = masterKey.m8[1];
        ((uint8_t *)value)[2] = masterKey.m8[2];
        ((uint8_t *)value)[3] = masterKey.m8[3];
        ((uint8_t *)value)[4] = masterKey.m8[4];
        ((uint8_t *)value)[5] = masterKey.m8[5];
        ((uint8_t *)value)[6] = masterKey.m8[6];
        ((uint8_t *)value)[7] = masterKey.m8[7];
        ((uint8_t *)value)[8] = masterKey.m8[8];
        ((uint8_t *)value)[9] = masterKey.m8[9];
        ((uint8_t *)value)[10] = masterKey.m8[10];
        ((uint8_t *)value)[11] = masterKey.m8[11];
        ((uint8_t *)value)[12] = masterKey.m8[12];
        ((uint8_t *)value)[13] = masterKey.m8[13];
        ((uint8_t *)value)[14] = masterKey.m8[14];
        ((uint8_t *)value)[15] = masterKey.m8[15];
        break;
    }
    case ProvisionAttr_NtwkName:
    {

        for(int i = 0; i< PROVPROFILE_NTWK_NAME_CHAR_LEN; i++)
            ((uint8_t *)value)[i] = networkName.m8[i];

        break;
    }
    case ProvisionAttr_PSKd:
    {
        for(int i = 0; i< PROVPROFILE_PROV_PSKD_CHAR_LEN; i++)
            ((uint8_t *)value)[i] = TIOP_CONFIG_PSKD[i];
        break;
    }
    case ProvisionAttr_ShortAddr:
    {
        ((uint8_t *)value)[0] = Util_hiUint16(shortAddr);
        ((uint8_t *)value)[1] = Util_loUint16(shortAddr);
        break;
    }
    case ProvisionAttr_Role:
    {
        *(uint8_t *)value = role;
        break;
    }
    default:
        // Attribute not found
        break;
    }
}

// Sensor callback functions
static void setSensorCb(RemoteDisplay_SensorAttr_t sensorAttr, void *const value, uint8_t len);
static void getSensorCb(RemoteDisplay_SensorAttr_t sensorAttr, void *value, uint8_t len);
RemoteDisplay_SensorCbs_t ot_SensorCbs =
{
 setSensorCb,
 getSensorCb
};

/** @brief  Set provisioning callback functions
 *
 *  @param  ProvisionAttr_t  Remote display attribute value to set
 *  @param  value  pointer to data from remote dispaly application
 *  @param  len  length of data from remote display application
 */
static void setSensorCb(RemoteDisplay_SensorAttr_t sensorAttr,
                        void *const value, uint8_t len)
{
    uint8_t *byteArr = (uint8_t *)value;

    switch(sensorAttr)
    {
    case SensorAttr_Report_Interval:
    {
        reportingInterval = Util_buildUint32(byteArr[3],byteArr[2],byteArr[1],byteArr[0]);
        TempSensor_postEvt(TempSensor_evtReportTemp);
        break;
    }
    case SensorAttr_Temperature:
        //not writable
    default:
        // Attribute not found
        break;
    }
}

/** @brief  Get provisioning callback functions
 *
 *  @param  ProvisionAttr_t  Remote display attribute value to set
 *
 *  @return  uint8_t  Current value of data present in 15.4 application
 */
static void getSensorCb(RemoteDisplay_SensorAttr_t sensorAttr, void *value, uint8_t len)
{
    switch(sensorAttr)
    {
    case SensorAttr_Temperature:
    {
        ((uint8_t *)value)[0] = attrTemperature[0];//res[0] = Util_loUint16((uint16_t)temperatureValue);
        ((uint8_t *)value)[1] = attrTemperature[1];//res[1] = Util_hiUint16((uint16_t)temperatureValue);

        break;
    }
    case SensorAttr_Report_Interval:
    {
        ((uint8_t *)value)[0] = Util_breakUint32(reportingInterval, 3);//reportingInterval
        ((uint8_t *)value)[1] = Util_breakUint32(reportingInterval, 2);
        ((uint8_t *)value)[2] = Util_breakUint32(reportingInterval, 1);
        ((uint8_t *)value)[3] = Util_breakUint32(reportingInterval, 0);
        break;
    }
    default:
        // Attribute not found
        break;
    }
}

#endif
#endif
/******************************************************************************
 Local Functions
 *****************************************************************************/

/**
 * @brief Configure the timer.
 *
 * @return None
 */
static void configureReportingTimer(void)
{
    struct sigevent event =
    {
     .sigev_notify_function = reportingTimeoutCB,
     .sigev_notify          = SIGEV_SIGNAL,
    };

    timer_create(CLOCK_MONOTONIC, &event, &reportTimerID);
}

/**
 * @brief Starts the reporting timer.
 *
 * @param timeout Time in milliseconds.
 *
 * Should be called after the reporting timer has been created, and after the
 * global address has been registered.
 */
static void startReportingTimer(uint32_t timeout)
{
    struct itimerspec newTime  = {0};
    struct itimerspec zeroTime = {0};
    struct itimerspec currTime;

    newTime.it_value.tv_sec  = (timeout / 1000U);
    newTime.it_value.tv_nsec = ((timeout % 1000U) * 1000000U);

    /* Disarm timer if currently armed */
    timer_gettime(reportTimerID, &currTime);
    if ((currTime.it_value.tv_sec != 0) || (currTime.it_value.tv_nsec != 0))
    {
        timer_settime(reportTimerID, 0, &zeroTime, NULL);
    }

    /* Arm timer */
    timer_settime(reportTimerID, 0, &newTime, NULL);
}

/**
 * @brief Callback function registered with the Coap server.
 *        Processes the coap request from the clients.
 *
 * @param val Argument passed by the clock if set up.
 *
 * @return None
 */
static void reportingTimeoutCB(union sigval val)
{
    TempSensor_postEvt(TempSensor_evtReportTemp);

    (void) val;
}

/**
 * @brief Reports the temperature to another coap device.
 *
 * @return None
 */
static void tempSensorReport(void)
{
    otError error = OT_ERROR_NONE;
    otMessage *requestMessage;
    otMessageInfo messageInfo;
    otInstance *instance = OtInstance_get();
    int32_t celsiusTemp;
#if (OPENTHREAD_CONFIG_DHCP6_CLIENT_ENABLE && OPENTHREAD_ENABLE_DHCP6_MULTICAST_SOLICIT)
    otIp6Address targetThermstatAddress;
    OtRtosApi_lock();
    const otNetifAddress *ip_address = otIp6GetUnicastAddresses(instance);
    OtRtosApi_unlock();
    for (const otNetifAddress *addr = ip_address; addr; addr = addr->mNext)
    {
        if(addr->mAddress.mFields.m16[6]==TIOP_TEMPSENSOR_DEIVCE_TYPE)
        {
            targetThermstatAddress = addr->mAddress;
            targetThermstatAddress.mFields.m16[6] = TIOP_THERMOSTAT_DEIVCE_TYPE;
            targetThermstatAddress.mFields.m16[7] = TIOP_THERMOSTAT_DEIVCE_ID;
            reportingAddress = targetThermstatAddress;
        };
    };
#endif

    /* make sure there is a new temperature reading otherwise just report the previous temperature */
    if(AONBatMonNewTempMeasureReady())
    {
        /* Read the temperature in degrees C from the internal temp sensor */
        celsiusTemp = AONBatMonTemperatureGetDegC();

        /* convert temp to Fahrenheit */
        temperatureValue = (int)((celsiusTemp * 9) / 5) + 32;
        /* convert temperature to string attribute */
        snprintf((char*)attrTemperature, sizeof(attrTemperature), "%d",
                 temperatureValue);
#ifdef USE_DMM
#ifdef BLE_START
        RemoteDisplay_updateSensorProfData();
#endif
#endif
    }

    /* print the reported value to the terminal */
    DISPUTILS_SERIALPRINTF(0, 0, "Reporting Temperature:");
    DISPUTILS_SERIALPRINTF(0, 0, (char*)attrTemperature);


    OtRtosApi_lock();
    requestMessage = otCoapNewMessage(instance, NULL);
    otEXPECT_ACTION(requestMessage != NULL, error = OT_ERROR_NO_BUFS);

    otCoapMessageInit(requestMessage, OT_COAP_TYPE_NON_CONFIRMABLE, OT_COAP_CODE_POST);
    otCoapMessageGenerateToken(requestMessage, DEFAULT_COAP_HEADER_TOKEN_LEN);
    error = otCoapMessageAppendUriPathOptions(requestMessage,
                                              THERMOSTAT_TEMP_URI);
    OtRtosApi_unlock();
    otEXPECT(OT_ERROR_NONE == error);

    OtRtosApi_lock();
    otCoapMessageSetPayloadMarker(requestMessage);
    OtRtosApi_unlock();

    OtRtosApi_lock();
    error = otMessageAppend(requestMessage, attrTemperature,
                            strlen((const char*) attrTemperature));
    OtRtosApi_unlock();
    otEXPECT(OT_ERROR_NONE == error);

    memset(&messageInfo, 0, sizeof(messageInfo));
    messageInfo.mPeerAddr = reportingAddress;
    messageInfo.mPeerPort = peerPort;

    OtRtosApi_lock();
    error = otCoapSendRequest(instance, requestMessage, &messageInfo, NULL,
                              NULL);
    OtRtosApi_unlock();

    /* Restart the clock */
#ifndef USE_DMM
    startReportingTimer(TIOP_TEMPSENSOR_REPORTING_INTERVAL);
#else
    startReportingTimer(reportingInterval);
#endif
    exit:

    if(error != OT_ERROR_NONE && requestMessage != NULL)
    {
        OtRtosApi_lock();
        otMessageFree(requestMessage);
        OtRtosApi_unlock();
    }
}

/**
 * @brief Callback function registered with the Coap server.
 *        Processes the coap request from the clients.
 *
 * @param  aContext      A pointer to the context information.
 * @param  aMessage      A pointer to the message.
 * @param  aMessageInfo  A pointer to the message info.
 *
 * @return None
 */
static void coapHandleServer(void *aContext, otMessage *aMessage,
                             const otMessageInfo *aMessageInfo)
{
    otError error = OT_ERROR_NONE;
    otMessage *responseMessage;
    otCoapCode responseCode = OT_COAP_CODE_CHANGED;
    otCoapCode messageCode = otCoapMessageGetCode(aMessage);

    responseMessage = otCoapNewMessage((otInstance*)aContext, NULL);
    otEXPECT_ACTION(responseMessage != NULL, error = OT_ERROR_NO_BUFS);

    otCoapMessageInitResponse(responseMessage, aMessage, OT_COAP_TYPE_ACKNOWLEDGMENT, responseCode);
    otCoapMessageSetToken(responseMessage, otCoapMessageGetToken(aMessage),
                          otCoapMessageGetTokenLength(aMessage));
    otCoapMessageSetPayloadMarker(responseMessage);

    if(OT_COAP_CODE_GET == messageCode)
    {
        error = otMessageAppend(responseMessage, attrTemperature,
                                strlen((const char*) attrTemperature));
        otEXPECT(OT_ERROR_NONE == error);

        error = otCoapSendResponse((otInstance*)aContext, responseMessage,
                                   aMessageInfo);
        otEXPECT(OT_ERROR_NONE == error);
    }

    exit:

    if (error != OT_ERROR_NONE && responseMessage != NULL)
    {
        otMessageFree(responseMessage);
    }
}

/**
 * @brief sets up the application coap server.
 *
 * @param aInstance A pointer to the context information.
 * @param attr      Attribute data
 *
 * @return OT_ERROR_NONE if successful, else error code
 */
static otError setupCoapServer(otInstance *aInstance, const attrDesc_t *attr)
{
    otError error = OT_ERROR_NONE;

    OtRtosApi_lock();
    error = otCoapStart(aInstance, OT_DEFAULT_COAP_PORT);
    OtRtosApi_unlock();
    otEXPECT(OT_ERROR_NONE == error);

    if(attr->type & (ATTR_READ | ATTR_WRITE))
    {
        coapResource.mHandler = &coapHandleServer;
        coapResource.mUriPath = (const char*)attr->uriPath;
        coapResource.mContext = aInstance;

        OtRtosApi_lock();
        error = otCoapAddResource(aInstance, &(coapResource));
        OtRtosApi_unlock();
        otEXPECT(OT_ERROR_NONE == error);
    }

    exit:
    return error;
}

/**
 * @brief Handles the key press events.
 *
 * @param _buttonHandle identifies which keys were pressed
 * @param _buttonEvents identifies the event that occurred on the key
 * @return None
 */
void processKeyChangeCB(Button_Handle _buttonHandle, Button_EventMask _buttonEvents)
{
    if (_buttonHandle == rightButtonHandle && _buttonEvents & Button_EV_CLICKED)
    {
        TempSensor_postEvt(TempSensor_evtKeyRight);
    }
}

/**
 * @brief Processes the OT stack events
 *
 * @param event             event identifier
 * @param aContext          context pointer for the event
 *
 * @return None
 */
static void processOtStackEvents(uint8_t event, void *aContext)
{
#ifdef USE_DMM
#else
    (void) aContext;
#endif
    switch (event)
    {
    case OT_STACK_EVENT_NWK_JOINED:
    {
        TempSensor_postEvt(TempSensor_evtNwkJoined);
        break;
    }

    case OT_STACK_EVENT_NWK_JOINED_FAILURE:
    {
        TempSensor_postEvt(TempSensor_evtNwkJoinFailure);
        break;
    }

    case OT_STACK_EVENT_NWK_DATA_CHANGED:
    {
        TempSensor_postEvt(TempSensor_evtNwkSetup);
        break;
    }

    case OT_STACK_EVENT_DEV_ROLE_CHANGED:
    {
        TempSensor_postEvt(TempSensor_evtDevRoleChanged);
        break;
    }
#ifdef USE_DMM
    case OT_STACK_EVENT_NWK_PARAMS_CHANGED:
    {
        deltaMask = *(uint32_t *)aContext;
        TempSensor_postEvt(TempSensor_evtNwkParamUpdate);
        break;
    }
#endif

    default:
    {
        // do nothing
        break;
    }
    }
}

/**
 * @brief Processes the events.
 *
 * @return None
 */
static void processEvent(TempSensor_evt event)
{
    switch (event)
    {
    case TempSensor_evtReportTemp:
    {
        /* perform activity related to the report event. */
#if TIOP_CUI
#ifdef USE_DMM


#else
        snprintf(statusBuf, sizeof(statusBuf), "[" CUI_COLOR_RED "Temperature" CUI_COLOR_RESET "] " CUI_COLOR_WHITE "%s" CUI_COLOR_RESET " [" CUI_COLOR_RED "Poll Period" CUI_COLOR_RESET "] " CUI_COLOR_WHITE "%d", (char*)attrTemperature, TIOP_CONFIG_POLL_PERIOD);
        tiopCUIUpdateApp((char*)statusBuf);
#endif
#endif /* TIOP_CUI */
        DISPUTILS_SERIALPRINTF( 0, 0, "Report Event received");
        tempSensorReport();
#ifdef USE_DMM
        snprintf(statusBuf, sizeof(statusBuf), "[" CUI_COLOR_RED "Temperature" CUI_COLOR_RESET "] " CUI_COLOR_WHITE "%s" CUI_COLOR_RESET " [" CUI_COLOR_RED "Report Interval" CUI_COLOR_RESET "] " CUI_COLOR_WHITE "%d", (char*)attrTemperature, reportingInterval);
        tiopCUIUpdateApp((char*)statusBuf);
#endif
        break;
    }

    case TempSensor_evtNwkSetup:
    {
        if (false == serverSetup)
        {
            serverSetup = true;
            (void)setupCoapServer(OtInstance_get(), &coapAttr);

            DISPUTILS_SERIALPRINTF(1, 0, "CoAP server setup done");
#ifdef TIOP_POWER_DATA_ACK
            startReportingTimer(TIOP_TEMPSENSOR_REPORTING_INTERVAL);
#else
            reportingAddress = thermostatAddress;
            TempSensor_postEvt(TempSensor_evtAddressValid);
#endif
        }
        break;
    }

    case TempSensor_evtKeyRight:
    {
        if ((!otDatasetIsCommissioned(OtInstance_get())) &&
                (OtStack_joinState() != OT_STACK_EVENT_NWK_JOIN_IN_PROGRESS))
        {
#if TIOP_CUI
            tiopCUIUpdateConnStatus(CUI_conn_joining);
#endif /* TIOP_CUI */
            DISPUTILS_SERIALPRINTF(1, 0, "Joining Nwk ...");
            DMMPolicy_updateStackState(DMMPolicy_StackRole_threadMtd, DMMPOLICY_THREAD_LINK_EST);

            OtStack_joinConfiguredNetwork();
        }
        break;
    }

    case TempSensor_evtNwkJoined://This is called when the join succeeds, and then it needs to actually attach. Keep provisioning mode on until attaching is done
    {
        DISPUTILS_SERIALPRINTF( 1, 0, "Joined Nwk");

        (void)OtStack_setupNetwork();
#if TIOP_CUI
#ifdef USE_DMM
//Leaving the network update code to update stuff as needed
#else
        OtRtosApi_lock();
        tiopCUIUpdatePANID(otLinkGetPanId(OtInstance_get()));
        tiopCUIUpdateChannel(otLinkGetChannel(OtInstance_get()));
        tiopCUIUpdateShortAddr(otLinkGetShortAddress(OtInstance_get()));
        tiopCUIUpdateNwkName(otThreadGetNetworkName(OtInstance_get()));
        tiopCUIUpdateMasterkey(*(otThreadGetMasterKey(OtInstance_get())));
        tiopCUIUpdateExtPANID(*(otThreadGetExtendedPanId(OtInstance_get())));
        OtRtosApi_unlock();
#endif
#endif /* TIOP_CUI */
        break;
    }

    case TempSensor_evtNwkJoinFailure://This is called when the join process fails. It goes here, and does nothing with the role callback
    {
#if TIOP_CUI
        tiopCUIUpdateConnStatus(CUI_conn_join_fail);
#endif /* TIOP_CUI */
#ifdef USE_DMM
        DMMPolicy_updateStackState(DMMPolicy_StackRole_threadMtd, DMMPOLICY_THREAD_IDLE);
#endif
        DISPUTILS_SERIALPRINTF(1, 0, "Join Failure");
        break;
    }

    case TempSensor_evtAddressValid:
    {
#ifndef USE_DMM
        startReportingTimer(TIOP_TEMPSENSOR_REPORTING_INTERVAL);
#else
        startReportingTimer(reportingInterval);
#endif
        break;
    }

#if TIOP_CUI
    case TempSensor_evtProcessMenuUpdate:
    {
        CUI_processMenuUpdate();
        break;
    }
#ifdef USE_DMM
    case TempSensor_evtNwkParamUpdate:{
        updateNetworkParams();
        break;
    }
#endif
    case TempSensor_evtNwkAttach://This is called when the attach function is requested by the CUI or RD. Here the provisioning status is set until it times out or succeeds later
    {
        DISPUTILS_LCDPRINTF(1, 0, "Joining Nwk ...");
        tiopCUIUpdateConnStatus(CUI_conn_joining);

        DMMPolicy_updateStackState(DMMPolicy_StackRole_threadMtd, DMMPOLICY_THREAD_LINK_EST);
        (void)OtStack_setupInterfaceAndNetwork();
        break;
    }

    case TempSensor_evtNwkJoin://This is called when the join function is requested by the CUI or RD. Here the provisioning status is set until it fails early or succeeds later
    {
        if ((!otDatasetIsCommissioned(OtInstance_get())) &&
                (OtStack_joinState() != OT_STACK_EVENT_NWK_JOIN_IN_PROGRESS))
        {
            DISPUTILS_LCDPRINTF(1, 0, "Joining Nwk ...");
            tiopCUIUpdateConnStatus(CUI_conn_joining);
            DMMPolicy_updateStackState(DMMPolicy_StackRole_threadMtd, DMMPOLICY_THREAD_LINK_EST);
            OtStack_joinConfiguredNetwork();
            break;
        }
    }
#endif /* TIOP_CUI */

    case TempSensor_evtDevRoleChanged:
    {
        OtRtosApi_lock();
#ifdef USE_DMM
        role = otThreadGetDeviceRole(OtInstance_get());     //Just set the role, updateNetworkParams will use updated values to set its stuff
#else
        otDeviceRole role = otThreadGetDeviceRole(OtInstance_get());
#endif
        OtRtosApi_unlock();

#ifndef TIOP_POWER_MEASUREMENT

        /* This function handles LEDs as well as CUI updates. In future
         * it's role may be reduced and need to be folded into the
         * following `#if TIOP_CUI`.
         */
        tiopCUIUpdateRole(role);

#if TIOP_CUI
#ifdef USE_DMM
        if(role == OT_DEVICE_ROLE_DISABLED){
            tiopCUIUpdateConnStatus(CUI_conn_disabled);
            DMMPolicy_updateStackState(DMMPolicy_StackRole_threadMtd, DMMPOLICY_THREAD_IDLE);

#ifdef BLE_START
            ProvisioningProfile_SetProvisionLock(false);
#endif
            RemoteDisplay_updateProvProfData();//outside just so it will be set to detached, so the user knows it is in the process of connecting

        }
        else if (role == OT_DEVICE_ROLE_DETACHED){//This means it's currently trying to connect
        }
        else//If it has successfully connected and all parameters have been synchronized. Both attach and join will call this in the end
        {
            tiopCUIUpdateConnStatus(CUI_conn_joined);
            DMMPolicy_updateStackState(DMMPolicy_StackRole_threadMtd, DMMPOLICY_THREAD_DATA);
#ifdef BLE_START
            ProvisioningProfile_SetProvisionLock(true);
#endif
            //Leave network parameter updating to the network parameter update function
        }
#else

        switch (role)
        {
        case OT_DEVICE_ROLE_DISABLED:
        case OT_DEVICE_ROLE_DETACHED:
            break;

        case OT_DEVICE_ROLE_CHILD:
        case OT_DEVICE_ROLE_ROUTER:
        case OT_DEVICE_ROLE_LEADER:
        {


            tiopCUIUpdateConnStatus(CUI_conn_joined);
            DMMPolicy_updateStackState(DMMPolicy_StackRole_threadMtd, DMMPOLICY_THREAD_DATA);

            OtRtosApi_lock();
            tiopCUIUpdatePANID(otLinkGetPanId(OtInstance_get()));
            tiopCUIUpdateChannel(otLinkGetChannel(OtInstance_get()));
            tiopCUIUpdateShortAddr(otLinkGetShortAddress(OtInstance_get()));
            tiopCUIUpdateNwkName(otThreadGetNetworkName(OtInstance_get()));
            tiopCUIUpdateMasterkey(*(otThreadGetMasterKey(OtInstance_get())));
            tiopCUIUpdateExtPANID(*(otThreadGetExtendedPanId(OtInstance_get())));
            OtRtosApi_unlock();
            break;
        }

        default:
        {
            break;
        }
        }
#endif
#else
#ifdef USE_DMM
        updateNetworkParams();
#endif /* USE_DMM */
#endif /* TIOP_CUI */
#endif /* !TIOP_POWER_MEASUREMENT */
        break;
    }
    default:
    {
        break;
    }
    }
}

/******************************************************************************
 External Functions
 *****************************************************************************/

/**
 * Documented in openthread/platform/uart.h.
 */
void otPlatUartReceived(const uint8_t *aBuf, uint16_t aBufLength)
{
    (void)aBuf;
    (void)aBufLength;
    /* Do nothing. */
}

/**
 * Documented in openthread/platform/uart.h.
 */
void otPlatUartSendDone(void)
{
    /* Do nothing. */
}

/*
 * Documented in tempsensor.h
 */
void TempSensor_postEvt(TempSensor_evt event)
{
    struct TempSensor_procQueueMsg msg;
    int                            ret;
    msg.evt = event;
    ret = mq_send(TempSensor_procQueueDesc, (const char *)&msg, sizeof(msg), 0);
    assert(0 == ret);
    (void)ret;
}

#if TIOP_CUI
/**
 * documented in tiop_ui.h
 */
void processMenuUpdateFn(void)
{
    TempSensor_postEvt(TempSensor_evtProcessMenuUpdate);
}

/**
 * documented in tiop_ui.h
 */
void tiopCUINwkReset(const char _input, char* _pLines[3], CUI_cursorInfo_t* _pCurInfo)
{
    if (CUI_ITEM_INTERCEPT_START == _input)
    {
        OtRtosApi_lock();
        otInstanceFactoryReset(OtInstance_get());
        OtRtosApi_unlock();
        strncpy(_pLines[2], "Resetting, Please wait...", MAX_MENU_LINE_LEN);
    }
}

/**
 * documented in tiop_ui.h
 */
void tiopCUIReset(const char _input, char* _pLines[3], CUI_cursorInfo_t* _pCurInfo)
{
    if (CUI_ITEM_INTERCEPT_START == _input)
    {
        OtRtosApi_lock();
        otInstanceReset(OtInstance_get());
        OtRtosApi_unlock();
        strncpy(_pLines[2], "Resetting, Please wait...", MAX_MENU_LINE_LEN);
    }
}

/**
 * documented in tiop_ui.h
 */
void uiActionAttach(const int32_t _itemEntry)
{
    TempSensor_postEvt(TempSensor_evtNwkAttach);
}

/**
 * documented in tiop_ui.h
 */
void uiActionJoin(const int32_t _itemEntry)
{
    TempSensor_postEvt(TempSensor_evtNwkJoin);
}
#endif /* TIOP_CUI */

#ifdef USE_DMM

/*!
 Build a uint16_t out of 2 uint8_t variables

 */
uint16_t Util_buildUint16(uint8_t loByte, uint8_t hiByte)
{
    return((uint16_t)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)));
}

/*!
 Get the high byte of a uint16_t variable

 */
uint8_t Util_hiUint16(uint16_t a)
{
    return((a >> 8) & 0xFF);
}

/*!
 Get the low byte of a uint16_t variable

 */
uint8_t Util_loUint16(uint16_t a)
{
    return((a) & 0xFF);
}
/*!
 Build a uint32_t out of 4 uint8_t variables

 */
uint32_t Util_buildUint32(uint8_t byte0, uint8_t byte1, uint8_t byte2,
                          uint8_t byte3)
{
    return((uint32_t)((uint32_t)((byte0) & 0x00FF) +
            ((uint32_t)((byte1) & 0x00FF) << 8) +
            ((uint32_t)((byte2) & 0x00FF) << 16) +
            ((uint32_t)((byte3) & 0x00FF) << 24)));
}

/*!
 Pull 1 uint8_t out of a uint32_t

 */
uint8_t Util_breakUint32(uint32_t var, int byteNum)
{
    return(uint8_t)((uint32_t)(((var) >> ((byteNum) * 8)) & 0x00FF));
}

/*
 * Upon reading the message to update parameters, will update as appropriate
 */
void updateNetworkParams(){

    OtRtosApi_lock();
    if(deltaMask & OT_CHANGED_THREAD_CHANNEL){
        channel = otLinkGetChannel(OtInstance_get());
        tiopCUIUpdateChannel(channel);
    }
    if(deltaMask & OT_CHANGED_THREAD_PANID ){
        panId = otLinkGetPanId(OtInstance_get());
        tiopCUIUpdatePANID(panId);
    }
    if(deltaMask & OT_CHANGED_THREAD_EXT_PANID ){
        extPanId = *(otThreadGetExtendedPanId(OtInstance_get()));
        tiopCUIUpdateExtPANID(extPanId);
    }
    if(deltaMask & OT_CHANGED_THREAD_NETWORK_NAME ){
        strncpy(networkName.m8,otThreadGetNetworkName(OtInstance_get()),sizeof(networkName.m8));
        tiopCUIUpdateNwkName(networkName.m8);
    }
    if(deltaMask & OT_CHANGED_MASTER_KEY ){
        masterKey = *(otThreadGetMasterKey(OtInstance_get()));
        tiopCUIUpdateMasterkey(masterKey);
    }
    if(deltaMask & OT_CHANGED_SUPPORTED_CHANNEL_MASK ){
        chanMask = otLinkGetSupportedChannelMask(OtInstance_get());
        tiopCUIUpdateChannelMask(chanMask);
    }

    if(deltaMask & (OT_CHANGED_THREAD_RLOC_ADDED | OT_CHANGED_THREAD_RLOC_REMOVED | OT_CHANGED_THREAD_LL_ADDR)){
        shortAddr = otLinkGetShortAddress(OtInstance_get());
        tiopCUIUpdateShortAddr(shortAddr);
    }

    OtRtosApi_unlock();
#ifdef BLE_START
    RemoteDisplay_updateProvProfData();
#endif
}
void setReportInterval(uint32_t interval){
    reportingInterval = interval;
    TempSensor_postEvt(TempSensor_evtReportTemp);
}
uint32_t getReportInterval(){
    return reportingInterval;
}

#endif
/*
 * Documented in task_config.h.
 */
void TempSensor_taskCreate(void)
{
    pthread_t           thread;
    pthread_attr_t      pAttrs;
    struct sched_param  priParam;
    int                 retc;

    retc = pthread_attr_init(&pAttrs);
    assert(retc == 0);

    retc = pthread_attr_setdetachstate(&pAttrs, PTHREAD_CREATE_DETACHED);
    assert(retc == 0);

    priParam.sched_priority = TASK_CONFIG_TEMPSENSOR_TASK_PRIORITY;
    retc = pthread_attr_setschedparam(&pAttrs, &priParam);
    assert(retc == 0);

    retc |= pthread_attr_setstacksize(&pAttrs, TASK_CONFIG_TEMPSENSOR_TASK_STACK_SIZE);
    assert(retc == 0);

    retc = pthread_create(&thread, &pAttrs, TempSensor_task, NULL);
    assert(retc == 0);

    retc = pthread_attr_destroy(&pAttrs);
    assert(retc == 0);

#ifdef USE_DMM
#ifdef BLE_START
    RemoteDisplay_registerClientProvCbs(ot_ProvisioningCbs);
    RemoteDisplay_registerSensorCbs(ot_SensorCbs);
#endif
#endif

    (void)retc;
}


/**
 *  Temp Sensor processing thread.
 */
void *TempSensor_task(void *arg0)
{
    struct mq_attr attr;
#if !TIOP_CONFIG_SET_NW_ID
    bool           commissioned;
#endif /* !TIOP_CONFIG_SET_NW_ID */
    mqd_t          procQueueLoopDesc;

    attr.mq_curmsgs = 0;
    attr.mq_flags   = 0;
    attr.mq_maxmsg  = TEMPSENSOR_PROC_QUEUE_MAX_MSG;
    attr.mq_msgsize = sizeof(struct TempSensor_procQueueMsg);

    /* Open the processing queue in non-blocking mode for the notify
     * callback functions
     */
    TempSensor_procQueueDesc = mq_open(TempSensor_procQueueName,
                                       (O_WRONLY | O_NONBLOCK | O_CREAT),
                                       0, &attr);

    /* Open the processing queue in blocking read mode for the process loop */
    procQueueLoopDesc = mq_open(TempSensor_procQueueName, O_RDONLY, 0, NULL);

#if ((BOARD_DISPLAY_USE_UART || BOARD_DISPLAY_USE_LCD) && !TIOP_CUI)

    DispUtils_open();
#endif
    AONBatMonEnable();

    OtStack_taskCreate();

    OtStack_registerCallback(processOtStackEvents);

    /* Set the poll period, as NVS does not store poll period */
    OtRtosApi_lock();
    otLinkSetPollPeriod(OtInstance_get(), TIOP_CONFIG_POLL_PERIOD);
    OtRtosApi_unlock();
#ifdef USE_DMM

    snprintf(statusBuf, sizeof(statusBuf), "[" CUI_COLOR_RED "Temperature" CUI_COLOR_RESET "] " CUI_COLOR_WHITE "%s" CUI_COLOR_RESET " [" CUI_COLOR_RED "Report Interval" CUI_COLOR_RESET "] " CUI_COLOR_WHITE "%d", (char*)attrTemperature, reportingInterval);
#else
    snprintf(statusBuf, sizeof(statusBuf), "[" CUI_COLOR_RED "Temperature" CUI_COLOR_RESET "] " CUI_COLOR_WHITE "%s" CUI_COLOR_RESET " [" CUI_COLOR_RED "Poll Period" CUI_COLOR_RESET "] " CUI_COLOR_WHITE "%d", (char*)attrTemperature, TIOP_CONFIG_POLL_PERIOD);
#endif
    tiopCUIInit((char*)statusBuf, &rightButtonHandle);

    DISPUTILS_SERIALPRINTF(0, 0, "Temperature Sensor init!");

#if !TIOP_CONFIG_SET_NW_ID
    OtRtosApi_lock();
    commissioned = otDatasetIsCommissioned(OtInstance_get());
    OtRtosApi_unlock();

    if (false == commissioned)
    {
        otExtAddress extAddress;

        OtRtosApi_lock();
        otLinkGetFactoryAssignedIeeeEui64(OtInstance_get(), &extAddress);
        OtRtosApi_unlock();

        DISPUTILS_SERIALPRINTF(2, 0, "pskd: %s", TIOP_CONFIG_PSKD);
        DISPUTILS_SERIALPRINTF(3, 0, "EUI64: 0x%02x%02x%02x%02x%02x%02x%02x%02x",
                               extAddress.m8[0], extAddress.m8[1], extAddress.m8[2],
                               extAddress.m8[3], extAddress.m8[4], extAddress.m8[5],
                               extAddress.m8[6], extAddress.m8[7]);
    }
    else
    {
        //if it gets here, that means it's going to try and attach to the network of it's last saved parameters
        DMMPolicy_updateStackState(DMMPolicy_StackRole_threadMtd, DMMPOLICY_THREAD_LINK_EST);
        OtStack_setupInterfaceAndNetwork();
    }
#else
    DMMPolicy_updateStackState(DMMPolicy_StackRole_threadMtd, DMMPOLICY_THREAD_LINK_EST);

    OtStack_setupInterfaceAndNetwork();
#endif /* !TIOP_CONFIG_SET_NW_ID */

    OtRtosApi_lock();
    otIp6AddressFromString(TIOP_THERMOSTAT_ADDRESS, &thermostatAddress);
    otIp6AddressFromString(TIOP_TEMPSENSOR_REPORTING_ADDRESS, &reportingAddress);
    OtRtosApi_unlock();

#ifdef USE_DMM

    //Initialization using default parameters


    OtRtosApi_lock();
    otLinkGetFactoryAssignedIeeeEui64(OtInstance_get(), &extAddress);
    panId = otLinkGetPanId(OtInstance_get());
    channel = otLinkGetChannel(OtInstance_get());
    shortAddr = otLinkGetShortAddress(OtInstance_get());
    chanMask = otLinkGetSupportedChannelMask((OtInstance_get()));
    OtRtosApi_unlock();

    for(int i=0; i< sizeof(networkName.m8); i++){
        if(!TIOP_CONFIG_NETWORK_NAME[i])
            break;
        networkName.m8[i] = TIOP_CONFIG_NETWORK_NAME[i];
    }

    /* Set master key using a byte-swap assignment */
    for (int i = 0; i < OT_MASTER_KEY_SIZE/2; i++)
    {
        masterKey.m8[OT_MASTER_KEY_SIZE - i - 1] =
                ((TIOP_CONFIG_MASTER_KEY_L >> (8*i)) & 0xFF);
    }
    for (int i = OT_MASTER_KEY_SIZE/2; i < OT_MASTER_KEY_SIZE; i++)
    {
        masterKey.m8[OT_MASTER_KEY_SIZE - i - 1] =
                ((TIOP_CONFIG_MASTER_KEY_U >> (8*(i-(OT_MASTER_KEY_SIZE/2)))) & 0xFF);
    }
    /* Set extended PAN ID using a byte-swap assignment */
    for (int i = 0; i < OT_EXT_PAN_ID_SIZE; i++)
    {
        extPanId.m8[OT_EXT_PAN_ID_SIZE - i - 1] =
                ((TIOP_CONFIG_EXT_PAN_ID >> (8*i)) & 0xFF);
    }
#ifdef BLE_START
    RemoteDisplay_updateProvProfData();
#endif
    OtStack_setupStateChange();

#endif

    configureReportingTimer();

    /* process events */
    while (true)
    {

        struct TempSensor_procQueueMsg msg;
        ssize_t                        ret;

        ret = mq_receive(procQueueLoopDesc, (char *)&msg, sizeof(msg), NULL);



        /* priorities are ignored */
        if (ret < 0 || ret != sizeof(msg))
        {
            /* there was an error on receive or we did not receive the full message */
            continue;
        }

        processEvent(msg.evt);
    }
}
