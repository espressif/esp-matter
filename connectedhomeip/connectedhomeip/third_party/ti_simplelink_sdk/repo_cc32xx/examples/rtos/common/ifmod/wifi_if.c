/*
 * Copyright (C) 2016-2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*****************************************************************************

   Application Name     - Provisioning application
   Application Overview - This application demonstrates how to use 
                          the provisioning method
                        in order to establish connection to the AP.
                        The application
                        connects to an AP and ping's the gateway to
                        verify the connection.

   Application Details  - Refer to 'Provisioning' README.html

 *****************************************************************************/
//****************************************************************************
//
//! \addtogroup
//! @{
//
//****************************************************************************

/* Standard Include */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include "mqueue.h"
#include <pthread.h>
#include <time.h>
#include <assert.h>

/* TI-DRIVERS Header files */
#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/drivers/net/wifi/slwificonn.h>
#include <ti/drivers/net/wifi/slnetifwifi.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/apps/LED.h>
#include "utils_if.h"
#include "debug_if.h"
#include "wifi_if.h"

#undef DEBUG_IF_NAME
#undef DEBUG_IF_SEVERITY
#define DEBUG_IF_NAME       "WIFI"
#define DEBUG_IF_SEVERITY   WIFI_IF_DEBUG_LEVEL


/****************************************************************************
              GLOBAL VARIABLES
 ****************************************************************************/
static pthread_t gSpawnThread = (pthread_t)NULL;
static pthread_t gSlWifiConnThread = (pthread_t)NULL;
static pthread_t gSlExtProvThread = (pthread_t)NULL;
static bool      gIsSlStarted = false;
bool   gIsProvsioning = false;
bool   gIsExtProvsioning = false;
static ExtProv_start_f gfExtProvStart = NULL;
static ExtProv_stop_f  gfExtProvStop = NULL;
static void *gpExtProvHandle = NULL;
#ifdef WIFI_LED_HANDLE
extern LED_Handle WIFI_LED_HANDLE;
#endif

/*****************************************************************************
          SimpleLink Callback Functions :WLAN, NETAPP and GENERAL EVENTS
          are served here. They are used internally by SlWifiConn (as lib
          registration) and are available here only for debug prints.
 *****************************************************************************/

//*****************************************************************************
//
//! \brief The Function Handles WLAN Events (used only for log messages)
//!
//! \param[in]  pWlanEvent - Pointer to WLAN Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
    static const char *Roles[] = {"STA","STA","AP","P2P"};
    static const char *WlanStatus[] = {"DISCONNECTED","SCANING","CONNECTING","CONNECTED"};

    switch(pWlanEvent->Id)
    {
    case SL_WLAN_EVENT_CONNECT:
        /* set the string terminate */
        pWlanEvent->Data.Connect.SsidName[pWlanEvent->Data.Connect.SsidLen] =
                '\0';
        LOG_INFO(" [Event] STA connected to AP "
                  "- BSSID:%.2x:%.2x:%.2x:%.2x:%.2x:%.2x, SSID:%s",
                pWlanEvent->Data.Connect.Bssid[0],
                pWlanEvent->Data.Connect.Bssid[1],
                pWlanEvent->Data.Connect.Bssid[2],
                pWlanEvent->Data.Connect.Bssid[3],
                pWlanEvent->Data.Connect.Bssid[4],
                pWlanEvent->Data.Connect.Bssid[5],
                pWlanEvent->Data.Connect.SsidName);
        break;

    case SL_WLAN_EVENT_DISCONNECT:
        LOG_INFO(" [Event] STA disconnected from AP (Reason Code = %d)",
                    pWlanEvent->Data.Disconnect.ReasonCode);
        break;

    case SL_WLAN_EVENT_STA_ADDED:
        LOG_DEBUG(" [Event] New STA Addeed (MAC Address:  %.2x:%.2x:%.2x:%.2x:%.2x)",
                pWlanEvent->Data.STAAdded.Mac[0],
                pWlanEvent->Data.STAAdded.Mac[1],
                pWlanEvent->Data.STAAdded.Mac[2],
                pWlanEvent->Data.STAAdded.Mac[3],
                pWlanEvent->Data.STAAdded.Mac[4],
                pWlanEvent->Data.STAAdded.Mac[5]);
        break;

    case SL_WLAN_EVENT_STA_REMOVED:
        LOG_DEBUG(" [Event] STA Removed (MAC Address: %.2x:%.2x:%.2x:%.2x:%.2x)",
                pWlanEvent->Data.STAAdded.Mac[0],
                pWlanEvent->Data.STAAdded.Mac[1],
                pWlanEvent->Data.STAAdded.Mac[2],
                pWlanEvent->Data.STAAdded.Mac[3],
                pWlanEvent->Data.STAAdded.Mac[4],
                pWlanEvent->Data.STAAdded.Mac[5]);
        break;

    case SL_WLAN_EVENT_PROVISIONING_PROFILE_ADDED:
        LOG_DEBUG(" [Provisioning] Profile Added: SSID: %s",
                    pWlanEvent->Data.ProvisioningProfileAdded.Ssid);
        if(pWlanEvent->Data.ProvisioningProfileAdded.ReservedLen > 0)
        {
            LOG_DEBUG(" [Provisioning] Profile Added: PrivateToken:%s",
                        pWlanEvent->Data.ProvisioningProfileAdded.Reserved);
        }
        break;

    case SL_WLAN_EVENT_PROVISIONING_STATUS:
    {
        switch(pWlanEvent->Data.ProvisioningStatus.ProvisioningStatus)
        {
        case SL_WLAN_PROVISIONING_GENERAL_ERROR:
        case SL_WLAN_PROVISIONING_ERROR_ABORT:
        case SL_WLAN_PROVISIONING_ERROR_ABORT_INVALID_PARAM:
        case SL_WLAN_PROVISIONING_ERROR_ABORT_HTTP_SERVER_DISABLED:
        case SL_WLAN_PROVISIONING_ERROR_ABORT_PROFILE_LIST_FULL:
        case SL_WLAN_PROVISIONING_ERROR_ABORT_PROVISIONING_ALREADY_STARTED:
            LOG_DEBUG(" [Provisioning] Provisioning Error status=%d",
                        pWlanEvent->Data.ProvisioningStatus.ProvisioningStatus);
            break;

        case SL_WLAN_PROVISIONING_CONFIRMATION_STATUS_FAIL_NETWORK_NOT_FOUND:
            LOG_DEBUG(" [Provisioning] Profile confirmation failed: "
                      "network not found");
            break;

        case SL_WLAN_PROVISIONING_CONFIRMATION_STATUS_FAIL_CONNECTION_FAILED:
            LOG_DEBUG(" [Provisioning] Profile confirmation failed:"
                      " Connection failed");
            break;

        case
        SL_WLAN_PROVISIONING_CONFIRMATION_STATUS_CONNECTION_SUCCESS_IP_NOT_ACQUIRED:
            LOG_DEBUG(" [Provisioning] Profile confirmation failed:"
                      " IP address not acquired");
            break;

        case SL_WLAN_PROVISIONING_CONFIRMATION_STATUS_SUCCESS_FEEDBACK_FAILED:
            LOG_DEBUG(" [Provisioning] Profile Confirmation failed "
                      " (Connection Success, feedback to Smartphone app failed)");
            break;

        case SL_WLAN_PROVISIONING_CONFIRMATION_STATUS_SUCCESS:
            LOG_DEBUG(" [Provisioning] Profile Confirmation Success!");
            break;

        case SL_WLAN_PROVISIONING_AUTO_STARTED:
            LOG_DEBUG(" [Provisioning] Auto-Provisioning Started");
            break;

        case SL_WLAN_PROVISIONING_STOPPED:
            LOG_DEBUG(" Provisioning stopped: Current Role: %s",
                        Roles[pWlanEvent->Data.ProvisioningStatus.Role]);
            if(ROLE_STA == pWlanEvent->Data.ProvisioningStatus.Role)
            {
                LOG_DEBUG("WLAN Status: %s",
                            WlanStatus[pWlanEvent->Data.ProvisioningStatus.
                                       WlanStatus]);

                if(SL_WLAN_STATUS_CONNECTED ==
                        pWlanEvent->Data.ProvisioningStatus.WlanStatus)
                {
                    LOG_DEBUG("Connected to SSID: %s",
                                pWlanEvent->Data.ProvisioningStatus.Ssid);
                }
            }
            break;

        case SL_WLAN_PROVISIONING_SMART_CONFIG_SYNCED:
            LOG_DEBUG(" [Provisioning] Smart Config Synced!");
            break;

        case SL_WLAN_PROVISIONING_SMART_CONFIG_SYNC_TIMEOUT:
            LOG_DEBUG(" [Provisioning] Smart Config Sync Timeout!");
            break;

        case SL_WLAN_PROVISIONING_CONFIRMATION_WLAN_CONNECT:
            LOG_DEBUG(
                    " [Provisioning] Profile confirmation: WLAN Connected!");
            break;

        case SL_WLAN_PROVISIONING_CONFIRMATION_IP_ACQUIRED:
            LOG_DEBUG(
                    " [Provisioning] Profile confirmation: IP Acquired!");
            break;

        case SL_WLAN_PROVISIONING_EXTERNAL_CONFIGURATION_READY:
            LOG_DEBUG(" [Provisioning] External configuration is ready! ");
            /* [External configuration]: External configuration is ready,
start the external configuration process.
        In case of using the external provisioning
enable the function below which will trigger StartExternalProvisioning() */
            break;

        default:
            LOG_ERROR(" [Provisioning] Unknown Provisioning Status: %d",
                    pWlanEvent->Data.ProvisioningStatus.ProvisioningStatus);
            break;
        }
    }
    break;

    default:
        LOG_ERROR(" [Event] - WlanEventHandler has received %d !!!!",
                    pWlanEvent->Id);
        break;
    }
}


//*****************************************************************************
//
//! \brief This function handles General Events
//!
//! \param[in]  pDevEvent - Pointer to General Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
    /* Most of the general errors are not FATAL are are to be handled
appropriately by the application */
    LOG_DEBUG("[GENERAL EVENT] - ID=[%d] Sender=[%d]\n\n",
                pDevEvent->Data.Error.Code,
                pDevEvent->Data.Error.Source);
}

//*****************************************************************************
//
//! \brief The Function Handles the Fatal errors
//!
//! \param[in]  slFatalErrorEvent - Pointer to Fatal Error Event info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkFatalErrorEventHandler(SlDeviceFatal_t *slFatalErrorEvent)
{

    switch (slFatalErrorEvent->Id)
    {
    case SL_DEVICE_EVENT_FATAL_DEVICE_ABORT:
    {
        LOG_ERROR("FATAL ERROR: Abort NWP event detected: "
                "AbortType=%d, AbortData=0x%x\n\r",
                slFatalErrorEvent->Data.DeviceAssert.Code,
                slFatalErrorEvent->Data.DeviceAssert.Value);
    }
    break;

    case SL_DEVICE_EVENT_FATAL_DRIVER_ABORT:
    {
        LOG_ERROR("FATAL ERROR: Driver Abort detected\n\r");
    }
    break;

    case SL_DEVICE_EVENT_FATAL_NO_CMD_ACK:
    {
        LOG_ERROR("FATAL ERROR: No Cmd Ack detected "
                "[cmd opcode = 0x%x]\n\r",
                slFatalErrorEvent->Data.NoCmdAck.Code);
    }
    break;

    case SL_DEVICE_EVENT_FATAL_SYNC_LOSS:
    {
        LOG_ERROR("FATAL ERROR: Sync loss detected\n\r");
    }
    break;

    case SL_DEVICE_EVENT_FATAL_CMD_TIMEOUT:
    {
        LOG_ERROR("FATAL ERROR: Async event timeout detected "
                "[event opcode = 0x%x]\n\r",
                slFatalErrorEvent->Data.CmdTimeout.Code);
    }
    break;

    default:
        LOG_ERROR("FATAL ERROR: Unspecified error detected\n\r");
        break;
    }
}

//*****************************************************************************
//
//! \brief This function handles network events such as IP acquisition, IP
//!           leased, IP released etc (used only for log messages)
//!
//! \param[in]  pNetAppEvent - Pointer to NetApp Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
    switch(pNetAppEvent->Id)
    {
    case SL_NETAPP_EVENT_IPV4_ACQUIRED:
         LOG_INFO("[NETAPP EVENT] IP Acquired: IP=%d.%d.%d.%d , "
                 "Gateway=%d.%d.%d.%d",
                 SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Ip,3),
                 SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Ip,2),
                 SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Ip,1),
                 SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Ip,0),
                 SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Gateway,3),
                 SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Gateway,2),
                 SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Gateway,1),
                 SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Gateway,0));
         break;
    case SL_NETAPP_EVENT_IPV6_ACQUIRED:
         LOG_DEBUG("[NETAPP EVENT] IPV6 Acquired");
         break;

    case SL_NETAPP_EVENT_DHCPV4_LEASED:

        LOG_DEBUG("[NETAPP Event] IP Leased: %d.%d.%d.%d",
                    SL_IPV4_BYTE(pNetAppEvent->Data.IpLeased.IpAddress,3),
                    SL_IPV4_BYTE(pNetAppEvent->Data.IpLeased.IpAddress,2),
                    SL_IPV4_BYTE(pNetAppEvent->Data.IpLeased.IpAddress,1),
                    SL_IPV4_BYTE(pNetAppEvent->Data.IpLeased.IpAddress,0));
        break;

    case SL_NETAPP_EVENT_IPV4_LOST:
         LOG_DEBUG("[NETAPP EVENT] IPV4 Lost");
         break;

    case SL_NETAPP_EVENT_IPV6_LOST:
         LOG_DEBUG("[NETAPP EVENT] IPV6 Lost");
         break;

    case SL_NETAPP_EVENT_DHCP_IPV4_ACQUIRE_TIMEOUT:
        break;


    default:
        LOG_ERROR("[NETAPP EVENT] Unhandled event [0x%x] ",
                    pNetAppEvent->Id);
        break;
    }
}

//*****************************************************************************
//                 Local Functions
//*****************************************************************************

//*****************************************************************************
//
//! \brief  Create a (pthread) task
//!
//*****************************************************************************



//*****************************************************************************
//
//! \brief  Configure static profile based on hard coded setting or
//!         a configuration file
//!
//*****************************************************************************
int ConfigureLocalNetwork()
{
    int retVal = 0;
    uint8_t *pBuf = NULL;
    char *pSSID = NULL;
    SlWlanSecParams_t  secParams = {SL_WLAN_SEC_TYPE_OPEN, NULL, 0};
    if (AP_SSID != NULL)
    {
        /* AP_SSID is not NULL - it will be set as a static profile
         * This should be used in development
         */
        pSSID = AP_SSID;
        if(AP_PASSWORD != NULL)
        {
            secParams.Type = SL_WLAN_SEC_TYPE_WPA_WPA2;
            secParams.Key = (int8_t*)AP_PASSWORD;
            secParams.KeyLen = strlen((char*)secParams.Key);
        }
    }
    else if(AP_CFG_FILENAME != NULL)
    {
        int len;
        /* Read Network credential from a file (if exists) */
        pBuf  = (uint8_t *)malloc(AP_CFG_MAX_SIZE);
        if(pBuf)
        {
            len = FILE_read((int8_t*)AP_CFG_FILENAME, AP_CFG_MAX_SIZE, pBuf, AP_CFG_TOKEN);
            if(len > 0)
            {
                int i;
                pSSID = (char*)pBuf;
                for(i=0; i<len; i++)
                {
                    if(pBuf[i] == ' ')
                    {
                        pBuf[i] = 0;
                        if(i < len-1)
                        {
                            secParams.Type = SL_WLAN_SEC_TYPE_WPA_WPA2;
                            secParams.Key = (int8_t*)&pBuf[i+1];
                            secParams.KeyLen = len-(i+1);
                            break;
                        }
                    }
                }
                pBuf[len] = 0;
            }
        }
    }
    if(pSSID)
    {
        uint16_t ssidLen = strlen(pSSID);
        retVal = SlWifiConn_addProfile(pSSID, ssidLen, NULL, &secParams, NULL, 15, SLWIFICONN_PROFILE_FLAG_NON_PERSISTENT);
    }
    if(pBuf)
        free(pBuf);

    return retVal;
}

//*****************************************************************************
//
//! \brief  Thread context for the SlWifiConn
//!
//! \note   The SlWifiConn_pocess only returns when the module is destoryed
//!         (see WIFI_IF_deinit)
//!
//*****************************************************************************
static void *SlWifiConnTask(void *pvParameters)
{
    void* retVal = SlWifiConn_process(pvParameters);
    pthread_exit(NULL);
    return retVal;
}


//*****************************************************************************
//
//! \brief  Thread context for the external provisioning method (e.g. WAC)
//!
//! \note   The start_f should return upon
//!
//*****************************************************************************
static void *WifiExtProvisioingTask(void* pvParameters)
{
    if (gfExtProvStart)
    {
        gfExtProvStart(gpExtProvHandle);
    }
    pthread_detach(pthread_self());
    pthread_exit(NULL);
    return NULL;
}


//*****************************************************************************
//
//! \brief  SlWifiConn Event Handler
//!
//*****************************************************************************
static void SlWifiConnEventHandler(WifiConnEventId_e eventId , WifiConnEventData_u *pData)
{
    uint8_t  simpleLinkMac[SL_MAC_ADDR_LEN];
    uint16_t macAddressLen;

    switch(eventId)
    {
    case WifiConnEvent_POWERED_UP:
    {
        LOG_INFO("[SlWifiConnEventHandler] POWERED_UP ");
        macAddressLen = sizeof(simpleLinkMac);
        sl_NetCfgGet(SL_NETCFG_MAC_ADDRESS_GET,NULL,&macAddressLen,
                     (unsigned char *)simpleLinkMac);
        LOG_INFO("  MAC address: %x:%x:%x:%x:%x:%x",
                    simpleLinkMac[0],
                    simpleLinkMac[1],
                    simpleLinkMac[2],
                    simpleLinkMac[3],
                    simpleLinkMac[4],
                    simpleLinkMac[5]                            );
    }
    break;
    case WifiConnEvent_POWERED_DOWN:
        LOG_INFO("[SlWifiConnEventHandler] POWERED DOWN "_CLR_RESET_);
        break;
    case WifiConnEvent_PROVISIONING_STARTED:
        gIsProvsioning = 1;
        LOG_INFO("[SlWifiConnEventHandler] PROVISIONING STARTED !\n\r"
                    "      mode=%d (0-AP, 1-SC, 2-AP+SC, 3-AP+SC+EXT)" _CLR_RESET_, pData->provisioningCmd);
#ifdef WIFI_LED_HANDLE
        LED_startBlinking(WIFI_LED_HANDLE, 500, LED_BLINK_FOREVER);
#endif
        break;
    case WifiConnEvent_PROVISIONING_STOPPED:
        gIsProvsioning = 0;
        LOG_INFO("[SlWifiConnEventHandler] PROVISIONING_STOPPED !\n\r"
                    "      status = %d (0-SUCCESS, 1-FAILED, 2-STOPPED)"_CLR_RESET_, pData->status);
#ifdef WIFI_LED_HANDLE
        LED_stopBlinking(WIFI_LED_HANDLE);
#endif
        break;
    case WifiConnEvent_EXTERNAL_PROVISIONING_START_REQ:
        gIsExtProvsioning = 1;
        if(gfExtProvStart)
        {
            gSlExtProvThread =  OS_createTask(EXT_PROV_TASK_PRIORITY, EXT_PROV_STACK_SIZE, WifiExtProvisioingTask, NULL, OS_TASK_FLAG_DETACHED);
            assert(gSlExtProvThread);
        }
        LOG_INFO("[SlWifiConnEventHandler] START EXT PROVISIONING !"_CLR_RESET_);
        break;
    case WifiConnEvent_EXTERNAL_PROVISIONING_STOP_REQ:
        gIsExtProvsioning = 0;
        if(gfExtProvStop)
        {
            gfExtProvStop(gpExtProvHandle);
        }
        LOG_INFO("[SlWifiConnEventHandler] STOP EXT PROVISIONING !"_CLR_RESET_);
        break;
    default:
        LOG_INFO("[SlWifiConnEventHandler] UNKNOWN EVENT "_CLR_RESET_);
    }
}


//*****************************************************************************
//
//! \brief  WiFi (SimpleLink driver and SlWifiConn) initialization
//!
//*****************************************************************************
int WIFI_IF_init()
{
    int retVal = 0;

    if(!gIsSlStarted)
    {
        /* Simplelink Spawn Thread init (should be done before SlWifiConn i initiated */
        gSpawnThread =  OS_createTask(SL_SPAWN_TASK_PRIORITY, SL_SPAWN_STACK_SIZE, sl_Task, NULL, OS_TASK_FLAG_DETACHED);
        assert(gSpawnThread);
        gIsSlStarted = true;
    }
    /* SlWifiConn init  */
    retVal = SlWifiConn_init(SlWifiConnEventHandler);
    assert(retVal == 0);

    /* SlWifiConn Thread init */
    gSlWifiConnThread =  OS_createTask(WIFI_CONN_TASK_PRIORITY, WIFI_CONN_STACK_SIZE, SlWifiConnTask, NULL, OS_TASK_FLAG_DETACHED);
    assert(gSlWifiConnThread);

    /* Provisioning Settings */
    if (PROVISIONING_MODE != WifiProvMode_OFF)
    {
        uint32_t flags = 0;
#if FORCE_PROVISIONING
        flags = SLWIFICONN_PROV_FLAG_FORCE_PROVISIONING;
#endif
        retVal = SlWifiConn_enableProvisioning(PROVISIONING_MODE, PROVISIONING_CMD, flags);
    }

    /* Adding logging capabilities to the SlWifiConn */
#if (SLWIFICONN_DEBUG_ENABLE)
    if(DEBUG_IF_SEVERITY <= E_DEBUG)
    {
        SlWifiConn_registerDebugCallback((SlWifiConn_Debug_f)Report);
        assert(retVal == 0);
    }
#endif

    /* Static Network Profile Settings */
   if(!FORCE_PROVISIONING)
   {
       ConfigureLocalNetwork();
   }

   /* Updating SlWifiConn Settings */
   {
       WifiConnSettings_t wifiConnSettings;
       SlWifiConn_getConfiguration(&wifiConnSettings);
       if(PROVISIONING_TIMEOUT)
       {
           wifiConnSettings.provisioningTimeout = PROVISIONING_TIMEOUT;
       }
       if(PROVISIONING_AP_PASSWORD != NULL)
       {
           wifiConnSettings.provisioningAP_secType = SL_WLAN_SEC_TYPE_WPA_WPA2;
           wifiConnSettings.provisioningAP_password = PROVISIONING_AP_PASSWORD;
       }
       if(PROVISIONING_SC_KEY != NULL)
       {
           wifiConnSettings.provisioningSC_key = PROVISIONING_SC_KEY;
       }
       wifiConnSettings.connectTimeout = 10; 
       wifiConnSettings.ipTimeout = 10;
       SlWifiConn_setConfiguration(&wifiConnSettings);
   }

   return retVal;
}

//*****************************************************************************
//
//! \brief  Registration of External Provsioning Callbacks
//!
//*****************************************************************************
int WIFI_IF_registerExtProvCallbacks(ExtProv_start_f fStart, ExtProv_stop_f fStop, void *pHandle)
{
    gfExtProvStart = fStart;
    gfExtProvStop = fStop;
    gpExtProvHandle = pHandle;
    return 0;
}

//*****************************************************************************
//
//! \brief  A request for NWP reset
//!
//*****************************************************************************
int WIFI_IF_reset()
{
    return SlWifiConn_reset();
}

//*****************************************************************************
//
//! \brief  Free SlWifiConn resources (including the module's thread)
//!
//*****************************************************************************
int WIFI_IF_deinit()
{
    int retVal = SlWifiConn_deinit();
    if(retVal == 0)
    {
        void *ret;
        retVal = pthread_join(gSlWifiConnThread, &ret);
    }
    return retVal;
}

