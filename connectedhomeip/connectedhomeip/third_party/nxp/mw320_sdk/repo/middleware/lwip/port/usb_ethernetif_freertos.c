/*
 * Copyright 2018,2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "netif/etharp.h"
#include "netif/ppp/pppoe.h"
#include "lwip/igmp.h"
#include "lwip/mld6.h"
#include "lwip/netifapi.h"
#if USE_RTOS && defined(FSL_RTOS_FREE_RTOS)
#include "FreeRTOS.h"
#include "event_groups.h"
#endif


#include "usb_host_config.h"
#include "usb_host.h"
#include "usb_host_cdc_rndis.h"
#include "usb_host_cdc.h"

#include "fsl_debug_console.h"
#include "board.h"

#include "usb_ethernetif.h"
#if (defined(FSL_FEATURE_SOC_SYSMPU_COUNT) && (FSL_FEATURE_SOC_SYSMPU_COUNT > 0U))
#include "fsl_sysmpu.h"
#endif /* FSL_FEATURE_SOC_SYSMPU_COUNT */

#include "board.h"

#if ((!USB_HOST_CONFIG_KHCI) && (!USB_HOST_CONFIG_EHCI) && (!USB_HOST_CONFIG_OHCI) && (!USB_HOST_CONFIG_IP3516HS))
#error Please enable USB_HOST_CONFIG_KHCI, USB_HOST_CONFIG_EHCI, USB_HOST_CONFIG_OHCI, or USB_HOST_CONFIG_IP3516HS in file usb_host_config.
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
  * Prototypes
  ******************************************************************************/
extern void USB_HostClockInit(void);
extern void USB_HostIsrEnable(void);
extern void USB_HostTaskFn(void *param);
/*******************************************************************************
  * Variables
  ******************************************************************************/
#define IFNAME0 'N'
#define IFNAME1 'X'
/*each g_RndisInstance should have its own's buffer*/
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) uint8_t g_SendMessage[RNDIS_CONTROL_MESSAGE];
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) uint8_t g_GetMessage[RNDIS_CONTROL_MESSAGE];
/*used for send/recv data from/to device rndis, the message length should be the sum of max frame packet size and usb rndis header*/
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) uint8_t g_OutPutBuffer[RNDIS_DATA_MESSAGE];
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) uint8_t g_InPutBuffer[RNDIS_DATA_MESSAGE];

usb_host_rndis_instance_struct_t g_RndisInstance = {0};

usb_host_handle g_HostHandle;


/*******************************************************************************
 * Code
 ******************************************************************************/
 /*!
 * @brief host cdc data transfer callback.
 *
 * This function is used as callback function for bulk in transfer .
 *
 * @param param    the host cdc instance pointer.
 * @param data     data buffer pointer.
 * @param dataLength data length.
 * @status         transfer result status.
 */

void USB_HostCdcRndisDataInCallback(void *param, uint8_t *data, uint32_t dataLength, usb_status_t status)
{
    usb_host_rndis_instance_struct_t * rndisInstance = (usb_host_rndis_instance_struct_t *)param;
     struct netif *netif = (struct netif *)rndisInstance->netif;
    if (status != kStatus_USB_Success)
    {
        if (status == kStatus_USB_TransferCancel)
        {
            /*usb_echo("cdc transfer cancel\r\n");*/
        }
        else
        {
            /**usb_echo("cdc in transfer retry\r\n");*/
        }
        rndisInstance->runState = kUSB_HostCdcRndisRunDataReceive;
    }
    else
    {

        struct pbuf *pbuf;
        if((dataLength > 0) && (NULL != data))
        {

            rndis_packet_msg_struct_t *temp = (rndis_packet_msg_struct_t *)data;
            pbuf= pbuf_alloc(PBUF_RAW, temp->dataLength, PBUF_POOL);
            if (pbuf)
            {
                
                temp->dataBuffer[temp->dataLength] = 0;
                pbuf->tot_len = temp->dataLength;
                pbuf->len = temp->dataLength;

                uint8_t *p = (uint8_t*)(&temp->dataOffset);
                memcpy(pbuf->payload, (p + temp->dataOffset), temp->dataLength);

                netif->input(pbuf, netif);   
            }
        }
        rndisInstance->runState = kUSB_HostCdcRndisRunDataReceive;
    }                               
     xEventGroupSetBits(g_RndisInstance.event_group, RNDIS_LWIP_INPUT);
    return;
}

/*!
* @brief host cdc data transfer callback.
*
* This function is used as callback function for bulk out transfer .
*
* @param param    the host cdc instance pointer.
* @param data     data buffer pointer.
* @param dataLength data length.
* @status         transfer result status.
*/
void USB_HostCdcRndisDataOutCallback(void *param, uint8_t *data, uint32_t dataLength, usb_status_t status)
{
    /*usb_host_rndis_instance_struct_t *rndisInstance = (usb_host_rndis_instance_struct_t *)param;*/
    if (status != kStatus_USB_Success)
    {
        if (status == kStatus_USB_TransferCancel)
        {
            usb_echo("cdc transfer cancel\r\n");
        }
        else
        {
            /*usb_echo("cdc out transfer error\r\n");*/
        }
    }
    xEventGroupSetBits(g_RndisInstance.lwipoutput, RNDIS_LWIP_OUTPUT);

    return;
}



err_t USB_EthernetIfOutPut(struct netif *netif, struct pbuf *p)
{
    err_t    status;
    usb_host_rndis_instance_struct_t * rndisInstance = (usb_host_rndis_instance_struct_t *)netif->state;
    status = ERR_OK;
    if (rndisInstance->attach)
    {
        if(p->tot_len == p->len)
        {
            uint32_t total;
            total = p->tot_len;
            uint32_t transferDone = 0;

            while (total)
            {
                rndisInstance->dataSend = 1;
                if(total <= RNDIS_FRAME_MAX_FRAMELEN)
                {
                       
                    USB_HostRndisSendDataMsg(rndisInstance->classHandle, rndisInstance->outPutBuffer, RNDIS_FRAME_MAX_FRAMELEN, 0, 0, 0, 0, 0, ((uint8_t*)p->payload + transferDone),total, 
                                                USB_HostCdcRndisDataOutCallback, rndisInstance);
                    transferDone += total;
                    total = 0U;

                }
                else
                {
                    
                    USB_HostRndisSendDataMsg(rndisInstance->classHandle, rndisInstance->outPutBuffer, RNDIS_FRAME_MAX_FRAMELEN, 0, 0, 0, 0, 0, ((uint8_t*)p->payload + transferDone),RNDIS_FRAME_MAX_FRAMELEN, 
                                                USB_HostCdcRndisDataOutCallback, rndisInstance);
                    transferDone += RNDIS_FRAME_MAX_FRAMELEN;
                    total -=  RNDIS_FRAME_MAX_FRAMELEN;     
                }

            xEventGroupWaitBits(g_RndisInstance.lwipoutput, 
            RNDIS_LWIP_OUTPUT,       
            pdTRUE,         
            pdFALSE,        
            portMAX_DELAY); 

            }
        }
        else
        {
            if (p->tot_len < RNDIS_FRAME_MAX_FRAMELEN)
            {
                struct pbuf *temp;
                uint8_t *bufferAddr = rndisInstance->outPutBuffer;
                for (temp = p; temp != NULL; temp = temp->next)
                {

                    memcpy(bufferAddr, temp->payload, temp->len);
                    bufferAddr += temp->len;
                }

                USB_HostRndisSendDataMsg(rndisInstance->classHandle, rndisInstance->outPutBuffer, RNDIS_FRAME_MAX_FRAMELEN, 0, 0, 0, 0, 0, ((uint8_t*)&g_OutPutBuffer[0]),p->tot_len, 
                            USB_HostCdcRndisDataOutCallback, rndisInstance);
                xEventGroupWaitBits(g_RndisInstance.lwipoutput, 
                RNDIS_LWIP_OUTPUT,       
                pdTRUE,         
                pdFALSE,        
                portMAX_DELAY); 
             
            }
            else
            {
                /*TO DO*/
                return ERR_BUF;
            }
        }
    }
    else
    {
        status = ERR_CONN;
        usb_echo("USB Rndis device is not attached\r\n");
    }
    return status;
}



/*!
 * @brief host callback function.
 *
 * device attach/detach callback function.
 *
 * @param deviceHandle           device handle.
 * @param configurationHandle attached device's configuration descriptor information.
 * @param event_code           callback event code, please reference to enumeration host_event_t.
 *
 * @retval kStatus_USB_Success              The host is initialized successfully.
 * @retval kStatus_USB_NotSupported         The application don't support the configuration.
 */
usb_status_t USB_HostEvent(usb_device_handle deviceHandle,
                           usb_host_configuration_handle configurationHandle,
                           uint32_t event_code)
{
    usb_status_t status;
    status = kStatus_USB_Success;

    switch (event_code)
    {
        case kUSB_HostEventAttach:
            status = USB_HostCdcRndisEvent(deviceHandle, configurationHandle, event_code);
            break;
        case kUSB_HostEventNotSupported:
            status = USB_HostCdcRndisEvent(deviceHandle, configurationHandle, event_code);
            usb_echo("device not supported.\r\n");
            break;
        case kUSB_HostEventEnumerationDone:
            status = USB_HostCdcRndisEvent(deviceHandle, configurationHandle, event_code);
            break;
        case kUSB_HostEventDetach:
            status = USB_HostCdcRndisEvent(deviceHandle, configurationHandle, event_code);
            break;
        default:
            break;
    }
    return status;
}

void USB_HostTask(void *hostHandle)
{
    while (1)
    {
        USB_HostTaskFn(g_HostHandle);
    }
}
void USB_HostApplicationTask(void *param)
{

    while (1)
    {
        xEventGroupWaitBits(g_RndisInstance.event_group,    /* The event group handle. */
                                 RNDIS_DEVICE_ATTACH | RNDIS_CONTROL_MSG | RNDIS_INTERRUPT_MSG |
                                 RNDIS_LWIP_INPUT,        /* The bit pattern the event group is waiting for. */
                                 pdTRUE,         /* BIT_n will be cleared automatically. */
                                 pdFALSE,        /* Don't wait for both bits, either bit unblock task. */
                                 portMAX_DELAY); /* Block indefinitely to wait for the condition to be met. */
        
        USB_HosCdcRndisTask(&g_RndisInstance);
    }
}
static void USB_HostApplicationInit(uint8_t controllerId, struct netif *netif)

{

    usb_status_t status = kStatus_USB_Success;
    
    USB_HostClockInit();
    
    g_RndisInstance.event_group = xEventGroupCreate();
    g_RndisInstance.lwipoutput = xEventGroupCreate();

    status = USB_HostInit(controllerId, &g_HostHandle, USB_HostEvent);
    if (status != kStatus_USB_Success)
    {
        usb_echo("host init error\r\n");
        return;
    }
    usb_echo("  host init.\r\n");
    netif->state = (void *)&g_RndisInstance;
    g_RndisInstance.netif = (void*)netif;
    g_RndisInstance.hostHandle = g_HostHandle;
    g_RndisInstance.sendMessage = &g_SendMessage[0];
    g_RndisInstance.getMessage = &g_GetMessage[0];
    g_RndisInstance.outPutBuffer = &g_OutPutBuffer[0];
    g_RndisInstance.inPutBuffer = &g_InPutBuffer[0];
    USB_HostIsrEnable();

    if (xTaskCreate(USB_HostTask, "usb host task", 2000L / sizeof(portSTACK_TYPE), g_HostHandle, DEFAULT_THREAD_PRIO + 3, NULL) != pdPASS)
    {
        usb_echo("create host task error\r\n");
    }

    if (xTaskCreate(USB_HostApplicationTask, "App task", 2000L / sizeof(portSTACK_TYPE), NULL, DEFAULT_THREAD_PRIO + 2, NULL) != pdPASS)
    {
        usb_echo("create cdc task error\r\n");
    }
    xEventGroupWaitBits(g_RndisInstance.event_group,    /* The event group handle. */
                         RNDIS_DEVICE_INIT_READY,        /* The bit pattern the event group is waiting for. */
                         pdTRUE,         /* BIT_n will be cleared automatically. */
                         pdFALSE,        /* Don't wait for both bits, either bit unblock task. */
                         portMAX_DELAY); /* Block indefinitely to wait for the condition to be met. */
    while (!g_RndisInstance.attach)
    {
    }
    
}

err_t USB_EthernetIfInIt(struct netif *netif)
{
    err_t    status;
    status = ERR_IF;
    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
#if LWIP_IPV4
    netif->output = etharp_output;
#endif
    netif->linkoutput = USB_EthernetIfOutPut;
    /* set MAC hardware address length */
    netif->hwaddr_len = ETH_HWADDR_LEN;
    ethernetifConfig_t *config;
    if(netif->state)
    {
        config = (ethernetifConfig_t *)netif->state;

        USB_HostApplicationInit(config->controllerId, netif);
        /*USB enet card is ready*/
        netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
        status = ERR_OK;
    }
    
    return status;
}


/*!
 * @brief host cdc interrupt transfer callback.
 *
 * This function is used as callback function for interrupt transfer . Interrupt transfer is used to implement
 * asynchronous notification of UART status as pstn sepc. This callback suppose the device will return SerialState
 * notification. If there is need to suppose other notification ,please refer pstn spec 6.5 and cdc spec6.3.
 * @param param    the host cdc instance pointer.
 * @param data     data buffer pointer.
 * @param dataLength data length.
 * @status         transfer result status.
 */
void USB_HostCdcRndisInterruptCallback(void *param, uint8_t *data, uint32_t dataLength, usb_status_t status)
{
    /*usb_host_cdc_acm_state_struct_t *state = (usb_host_cdc_acm_state_struct_t *)data;*/
    usb_host_rndis_instance_struct_t *rndisInstance = (usb_host_rndis_instance_struct_t *)param;
    
    if (status != kStatus_USB_Success)
    {
        if (status == kStatus_USB_TransferCancel)
        {
            usb_echo("cdc transfer cancel\r\n");
        }
        else
        {
            usb_echo("cdc control transfer error\r\n");
        }
    }
    else
    { /*more information about SerialState ,please pstn spec 6.5.4 */
       /* usb_echo("get serial state value = %d\r\n", state->bmstate);*/
        rndisInstance->responseAvailable = 1;
        rndisInstance->interruptRunState = kUSB_HostCdcRndisRunInterruptRecvDone;
    }
    xEventGroupSetBits(g_RndisInstance.event_group, RNDIS_INTERRUPT_MSG);
}

/*!
 * @brief host cdc rndis control transfer callback.
 *
 * This function is used as callback function for control transfer .
 *
 * @param param    the host cdc rndis instance pointer.
 * @param data     data buffer pointer.
 * @param dataLength data length.
 * @status         transfer result status.
 */
void USB_HostCdcRndisControlCallback(void *param, uint8_t *data, uint32_t dataLength, usb_status_t status)
{
    usb_host_rndis_instance_struct_t *rndisInstance = (usb_host_rndis_instance_struct_t *)param;

    struct netif *netif = (struct netif *)rndisInstance->netif;
    if (status != kStatus_USB_Success)
    {
        usb_echo("data transfer error = %d\r\n",status);
        return;
    }
    
    if (rndisInstance->runWaitState == kUSB_HostCdcRndisRunWaitSetControlInterface)
    {
        rndisInstance->runState = kUSB_HostCdcRndisRunSetControlInterfaceDone;
    }
    else if (rndisInstance->runWaitState == kUSB_HostCdcRndisRunWaitSetDataInterface)
    {
        rndisInstance->runState = kUSB_HostCdcRndisRunSetDataInterfaceDone;
    }
    else if (rndisInstance->runWaitState == kUSB_HostCdcRndisRunWaitInitMsg)
    {
        rndisInstance->runState = kUSB_HostCdcRndisRunWaitInitMsgDone;
    }
    else if (rndisInstance->runWaitState == kUSB_HostCdcRndisRunWaitMaxmumFrame)
    {
        rndisInstance->runState = kUSB_HostCdcRndisRunWaitMaxmumFrameDone;
    }
    else if (rndisInstance->runWaitState == kUSB_HostCdcRndisRunWaitGetMACAddress)
    {
        rndisInstance->runState = kUSB_HostCdcRndisRunWaitGetMACAddressDone;
    }
    else if (rndisInstance->runWaitState == kUSB_HostCdcRndisRunWaitSetMsg)
    {
        rndisInstance->runState = kUSB_HostCdcRndisRunWaitSetMsgDone;
    }
    else if (rndisInstance->runWaitState == kUSB_HostCdcRndisRunWaitGetEncapsulatedCommand)
    {
        rndisInstance->runState = kUSB_HostCdcRndisRunGetEncapsulatedResponseDone;
        if (rndisInstance->previousRunState == kUSB_HostCdcRndisRunWaitInitMsgDone)
        {
           rndisInstance->runState = kUSB_HostCdcRndisRunWaitMaxmumFrame;
                  
        }
        else if (rndisInstance->previousRunState == kUSB_HostCdcRndisRunWaitMaxmumFrameDone)
        {
            rndisInstance->runState = kUSB_HostCdcRndisRunWaitGetMACAddress;

            rndis_query_cmplt_struct_t * msg = (rndis_query_cmplt_struct_t *)data;
            if(REMOTE_NDIS_QUERY_CMPLT == msg->messageType)
            {
                netif->mtu = USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(((uint8_t*)&msg->requestID + msg->informationBufferOffset));
            }
            else
            {
                /*set default value if device doesn't send query complete message*/
                netif->mtu = 1500U;
            }
        }
        else if (rndisInstance->previousRunState == kUSB_HostCdcRndisRunWaitGetMACAddressDone)
        {
            rndisInstance->runState = kUSB_HostCdcRndisRunWaitSetMsg;
            rndis_query_cmplt_struct_t * msg = (rndis_query_cmplt_struct_t *)data;

            netif->hwaddr_len = NETIF_MAX_HWADDR_LEN;

            memcpy(netif->hwaddr,(((uint8_t*)&msg->requestID + msg->informationBufferOffset)), NETIF_MAX_HWADDR_LEN);
        }
        else if (rndisInstance->previousRunState == kUSB_HostCdcRndisRunWaitSetMsgDone)
        {
            rndisInstance->runState = kUSB_HostCdcRndisRunGetState;
        }
        rndisInstance->previousRunState = kUSB_HostCdcRndisRunIdle;
    }
    else
    {
    }
    xEventGroupSetBits(g_RndisInstance.event_group, RNDIS_CONTROL_MSG);
}


/*!
 * @brief host rndis task function.
 *
 * This function implements the host cdc action, it is used to create task.
 *
 * @param param   the host rndis instance pointer.
 */
void USB_HosCdcRndisTask(void *param)
{

    usb_status_t status = kStatus_USB_Success;
    usb_host_rndis_instance_struct_t *rndisInstance = (usb_host_rndis_instance_struct_t *)param;
    uint32_t filtertype;
    struct netif *netif ;
    /* device state changes */
    if (rndisInstance->deviceState != rndisInstance->previousState)
    {
        rndisInstance->previousState = rndisInstance->deviceState;
        switch (rndisInstance->deviceState)
        {
            case kStatus_DEV_Idle:
                break;
            case kStatus_DEV_Attached:
                rndisInstance->runState = kUSB_HostCdcRndisRunSetControlInterface;
                status = USB_HostCdcInit(rndisInstance->deviceHandle, &rndisInstance->classHandle);
                usb_echo("rndis device attached\r\n");
                break;
            case kStatus_DEV_Detached:
                rndisInstance->deviceState = kStatus_DEV_Idle;
                rndisInstance->runState = kUSB_HostCdcRndisRunIdle;
                USB_HostCdcDeinit(rndisInstance->deviceHandle, rndisInstance->classHandle);
                rndisInstance->dataInterfaceHandle = NULL;
                rndisInstance->classHandle = NULL;
                rndisInstance->controlInterfaceHandle = NULL;
                rndisInstance->deviceHandle = NULL;
                rndisInstance->interruptRunState = kUSB_HostCdcRndisRunIdle;
                usb_echo("rndis device detached\r\n");
                break;
            default:
                break;
        }
    }

    /* run state */
    switch (rndisInstance->runState)
    {
        case kUSB_HostCdcRndisRunIdle:
            break;
        case kUSB_HostCdcRndisRunSetControlInterface:
            rndisInstance->runWaitState = kUSB_HostCdcRndisRunWaitSetControlInterface;
            rndisInstance->runState = kUSB_HostCdcRndisRunIdle;
            if (USB_HostCdcSetControlInterface(rndisInstance->classHandle, rndisInstance->controlInterfaceHandle, 0,
                                               USB_HostCdcRndisControlCallback, rndisInstance) != kStatus_USB_Success)
            {
                usb_echo("set control interface error\r\n");
            }
            break;
        case kUSB_HostCdcRndisRunSetControlInterfaceDone:
            rndisInstance->runWaitState = kUSB_HostCdcRndisRunWaitSetDataInterface;
            rndisInstance->runState = kUSB_HostCdcRndisRunIdle;
            if (USB_HostCdcSetDataInterface(rndisInstance->classHandle, rndisInstance->dataInterfaceHandle, 0,
                                            USB_HostCdcRndisControlCallback,rndisInstance) != kStatus_USB_Success)
            {
                usb_echo("set data interface error\r\n");
            }
            break;
        case kUSB_HostCdcRndisRunSetDataInterfaceDone:
            rndisInstance->runWaitState = kUSB_HostCdcRndisRunWaitInitMsg;
            rndisInstance->runState = kUSB_HostCdcRndisRunIdle;
            if(USB_HostRndisInitMsg(rndisInstance->classHandle,rndisInstance->sendMessage, RNDIS_CONTROL_MESSAGE,
                                                        USB_HostCdcRndisControlCallback, rndisInstance))
            {
                usb_echo("Error in Init message\r\n");
            }
            break;
        case kUSB_HostCdcRndisRunWaitInitMsgDone:

            if (rndisInstance->interruptRunState == kUSB_HostCdcRndisRunIdle)
            {
                rndisInstance->interruptRunState = kUSB_HostCdcRndisRunInterruptRecvPrime;
                if (USB_HostCdcInterruptRecv(rndisInstance->classHandle, (uint8_t *)&rndisInstance->state,
                                             RNDIS_RESPONSE_AVAILABLE, USB_HostCdcRndisInterruptCallback,
                                             rndisInstance) != kStatus_USB_Success)
                {
                    usb_echo("Error in USB_HostCdcInterruptRecv: %x\r\n", status);
                }
            }
            else if (rndisInstance->interruptRunState == kUSB_HostCdcRndisRunInterruptRecvDone)
            {     
                rndisInstance->interruptRunState = kUSB_HostCdcRndisRunIdle;
                /*remember the previous set encapsulated command*/
                rndisInstance->previousRunState = kUSB_HostCdcRndisRunWaitInitMsgDone;
                rndisInstance->runWaitState = kUSB_HostCdcRndisRunWaitGetEncapsulatedCommand;
                rndisInstance->runState = kUSB_HostCdcRndisRunIdle;

                if(USB_HostCdcGetEncapsulatedResponse(rndisInstance->classHandle, rndisInstance->getMessage, 
                                                                                RNDIS_CONTROL_MESSAGE, USB_HostCdcRndisControlCallback, rndisInstance))
                {
                   usb_echo("Error in Init message\r\n");
                }
            }
            break;
        case kUSB_HostCdcRndisRunWaitMaxmumFrame:
            rndisInstance->previousRunState = kUSB_HostCdcRndisRunWaitMaxmumFrame;
            rndisInstance->runWaitState = kUSB_HostCdcRndisRunWaitMaxmumFrame;
            rndisInstance->runState = kUSB_HostCdcRndisRunIdle;

            if(USB_HostRndisQueryMsg(rndisInstance->classHandle, OID_GEN_MAXIMUM_FRAME_SIZE, rndisInstance->sendMessage, 
                                                                                      RNDIS_CONTROL_MESSAGE, 0, 0, NULL, USB_HostCdcRndisControlCallback, rndisInstance))
            {
                usb_echo("Error in Init message\r\n");
            }
            break;
      
        case kUSB_HostCdcRndisRunWaitMaxmumFrameDone:

            if (rndisInstance->interruptRunState == kUSB_HostCdcRndisRunIdle)
            {
                rndisInstance->interruptRunState = kUSB_HostCdcRndisRunInterruptRecvPrime;
                if (USB_HostCdcInterruptRecv(rndisInstance->classHandle, (uint8_t *)&rndisInstance->state,
                                             RNDIS_RESPONSE_AVAILABLE, USB_HostCdcRndisInterruptCallback,
                                             rndisInstance) != kStatus_USB_Success)
                {
                    usb_echo("Error in USB_HostCdcInterruptRecv: %x\r\n", status);
                }
            }
            else if (rndisInstance->interruptRunState == kUSB_HostCdcRndisRunInterruptRecvDone)
            {
                rndisInstance->interruptRunState = kUSB_HostCdcRndisRunIdle;
                rndisInstance->previousRunState = kUSB_HostCdcRndisRunWaitMaxmumFrameDone;
                rndisInstance->runWaitState = kUSB_HostCdcRndisRunWaitGetEncapsulatedCommand;
                rndisInstance->runState = kUSB_HostCdcRndisRunIdle;

                if(USB_HostCdcGetEncapsulatedResponse(rndisInstance->classHandle, rndisInstance->getMessage, 
                                                                                          RNDIS_CONTROL_MESSAGE, USB_HostCdcRndisControlCallback, rndisInstance))
                {
                    usb_echo("Error in Init message\r\n");
                }
            }
            break;
        case kUSB_HostCdcRndisRunWaitGetMACAddress:
            rndisInstance->previousRunState = kUSB_HostCdcRndisRunWaitGetMACAddress;
            rndisInstance->runWaitState = kUSB_HostCdcRndisRunWaitGetMACAddress;
            rndisInstance->runState = kUSB_HostCdcRndisRunIdle;

            if(USB_HostRndisQueryMsg(rndisInstance->classHandle, OID_802_3_CURRENT_ADDRESS,rndisInstance->sendMessage, 
                                                                                      RNDIS_CONTROL_MESSAGE, 0, 0, NULL, USB_HostCdcRndisControlCallback, rndisInstance))
            {
                usb_echo("Error in Init message\r\n");
            }
            break;
      
        case kUSB_HostCdcRndisRunWaitGetMACAddressDone:
            if (rndisInstance->interruptRunState  == kUSB_HostCdcRndisRunIdle)
            {
                rndisInstance->interruptRunState = kUSB_HostCdcRndisRunInterruptRecvPrime;
                rndisInstance->interruptRunState = kUSB_HostCdcRndisRunInterruptRecvPrime;
                if (USB_HostCdcInterruptRecv(rndisInstance->classHandle, (uint8_t *)&rndisInstance->state,
                                           RNDIS_RESPONSE_AVAILABLE, USB_HostCdcRndisInterruptCallback,
                                           rndisInstance) != kStatus_USB_Success)
                {
                    usb_echo("Error in USB_HostCdcInterruptRecv: %x\r\n", status);
                }

            }
            else if (rndisInstance->interruptRunState == kUSB_HostCdcRndisRunInterruptRecvDone)
            {
                rndisInstance->interruptRunState = kUSB_HostCdcRndisRunIdle;
                rndisInstance->responseAvailable = 0;
                rndisInstance->previousRunState = kUSB_HostCdcRndisRunWaitGetMACAddressDone;
                rndisInstance->runWaitState = kUSB_HostCdcRndisRunWaitGetEncapsulatedCommand;
                rndisInstance->runState = kUSB_HostCdcRndisRunIdle;

                if(USB_HostCdcGetEncapsulatedResponse(rndisInstance->classHandle, rndisInstance->getMessage, 
                                                                                          RNDIS_CONTROL_MESSAGE, USB_HostCdcRndisControlCallback, rndisInstance))
                {
                    usb_echo("Error in Init message\r\n");
                }
            }
            break;
        case kUSB_HostCdcRndisRunWaitSetMsg:
            rndisInstance->previousRunState = kUSB_HostCdcRndisRunWaitSetMsg;
            rndisInstance->runWaitState = kUSB_HostCdcRndisRunWaitSetMsg;
            rndisInstance->runState = kUSB_HostCdcRndisRunIdle;
            filtertype = NDIS_PACKET_TYPE_DIRECTED;
            if(USB_HostRndisSetMsg(rndisInstance->classHandle, OID_GEN_CURRENT_PACKET_FILTER, rndisInstance->sendMessage, RNDIS_CONTROL_MESSAGE, 20U, (sizeof(filtertype)),&filtertype,
                                                                                          USB_HostCdcRndisControlCallback, rndisInstance))
            {
                usb_echo("Error in set message\r\n");
            }
            break;
        case kUSB_HostCdcRndisRunWaitSetMsgDone:
            if (rndisInstance->interruptRunState  == kUSB_HostCdcRndisRunIdle)
            {
                rndisInstance->interruptRunState = kUSB_HostCdcRndisRunInterruptRecvPrime;
                rndisInstance->interruptRunState = kUSB_HostCdcRndisRunInterruptRecvPrime;
                if (USB_HostCdcInterruptRecv(rndisInstance->classHandle, (uint8_t *)&rndisInstance->state,
                                       RNDIS_RESPONSE_AVAILABLE, USB_HostCdcRndisInterruptCallback,
                                       rndisInstance) != kStatus_USB_Success)
                {
                    usb_echo("Error in USB_HostCdcInterruptRecv: %x\r\n", status);
                }
            }
            else if (rndisInstance->interruptRunState == kUSB_HostCdcRndisRunInterruptRecvDone)
            {
                rndisInstance->responseAvailable = 0;
                rndisInstance->previousRunState = kUSB_HostCdcRndisRunWaitSetMsgDone;
                rndisInstance->runWaitState = kUSB_HostCdcRndisRunWaitGetEncapsulatedCommand;
                rndisInstance->runState = kUSB_HostCdcRndisRunIdle;

                if(USB_HostCdcGetEncapsulatedResponse(rndisInstance->classHandle, rndisInstance->getMessage, 
                                                                  RNDIS_CONTROL_MESSAGE, USB_HostCdcRndisControlCallback, rndisInstance))
                {
                    usb_echo("Error in Init message\r\n");
                }
            }
          break;
        case kUSB_HostCdcRndisRunGetState:
            rndisInstance->attach = 1;
            netif = (struct netif *)rndisInstance->netif;
           
            xEventGroupSetBits(g_RndisInstance.event_group, RNDIS_DEVICE_INIT_READY);
            netifapi_netif_set_link_up(netif);
        case kUSB_HostCdcRndisRunDataReceive:
            rndisInstance->runState = kUSB_HostCdcRndisRunIdle;
            USB_HostRndisRecvDataMsg(rndisInstance->classHandle, rndisInstance->inPutBuffer, RNDIS_DATA_MESSAGE, 
                                                            USB_HostCdcRndisDataInCallback, rndisInstance);
            break;
        default:
            break;
    }
}

usb_status_t USB_HostCdcRndisEvent(usb_device_handle deviceHandle,
                              usb_host_configuration_handle configurationHandle,
                              uint32_t event_code)
{
    usb_status_t status;
    uint8_t id;
    usb_host_configuration_t *configuration;
    uint8_t interface_index;
    usb_host_interface_t *hostInterface;
    uint32_t info_value;
    struct netif *netif;
    status = kStatus_USB_Success;

    switch (event_code)
    {
        case kUSB_HostEventAttach:
            /* judge whether is configurationHandle supported */
            configuration = (usb_host_configuration_t *)configurationHandle;

            for (interface_index = 0; interface_index < configuration->interfaceCount; ++interface_index)
            {
                hostInterface = &configuration->interfaceList[interface_index];
                id = hostInterface->interfaceDesc->bInterfaceClass;
                if (id != USB_HOST_CDC_RNDIS_CLASS_CODE)
                {
                    continue;
                }
                id = hostInterface->interfaceDesc->bInterfaceSubClass;
                if (id != USB_HOST_CDC_RNDIS_SUBCLASS_CODE)
                {
                    continue;
                }
                id = hostInterface->interfaceDesc->bInterfaceProtocol;
                if (id != USB_HOST_CDC_RNDIS_PROTOCOL_CODE)
                {
                    continue;
                }
                else
                {
                    /* the interface is supported by the application */
                    g_RndisInstance.controlInterfaceHandle = hostInterface;

                }
            }
            for (interface_index = 0; interface_index < configuration->interfaceCount; ++interface_index)
            {
                hostInterface = &configuration->interfaceList[interface_index];
                id = hostInterface->interfaceDesc->bInterfaceClass;

                if (id != USB_HOST_CDC_DATA_CLASS_CODE)
                {
                    continue;
                }
                id = hostInterface->interfaceDesc->bInterfaceSubClass;
                if (id != USB_HOST_CDC_DATA_SUBCLASS_CODE)
                {
                    continue;
                }
                id = hostInterface->interfaceDesc->bInterfaceProtocol;
                if (id != USB_HOST_CDC_DATA_PROTOCOL_CODE)
                {
                    continue;
                }
                else
                {
                    g_RndisInstance.dataInterfaceHandle = hostInterface;

                }
            }
            g_RndisInstance.deviceHandle = deviceHandle;
            if ((NULL != g_RndisInstance.dataInterfaceHandle) && (NULL != g_RndisInstance.controlInterfaceHandle))
            {
                status = kStatus_USB_Success;
            }
            else
            {
                status = kStatus_USB_NotSupported;
            }
            break;

        case kUSB_HostEventNotSupported:
            usb_echo("  the usb tethering featue is not enabled, please turn on usb tethering in mobile phone\r\n ");
            break;

        case kUSB_HostEventEnumerationDone:
            if (g_RndisInstance.deviceState == kStatus_DEV_Idle)
            {
                if ((g_RndisInstance.deviceHandle != NULL) && (g_RndisInstance.dataInterfaceHandle != NULL) && (g_RndisInstance.controlInterfaceHandle != NULL))
                {
                    g_RndisInstance.deviceState = kStatus_DEV_Attached;
  
                    USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDevicePID, &info_value);
                    usb_echo("device cdc attached:\r\npid=0x%x", info_value);
                    USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceVID, &info_value);
                    usb_echo("vid=0x%x ", info_value);
                    USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceAddress, &info_value);
                    usb_echo("address=%d\r\n", info_value);

                    xEventGroupSetBits(g_RndisInstance.event_group, RNDIS_DEVICE_ATTACH);
                }
            }
            else
            {
                usb_echo("not idle rndis instance\r\n");
            }
            break;

        case kUSB_HostEventDetach:
            if (g_RndisInstance.deviceState != kStatus_DEV_Idle)
            {

                g_RndisInstance.attach = 0;
                netif = (struct netif *)g_RndisInstance.netif;
                netifapi_netif_set_link_down(netif);
                g_RndisInstance.deviceState = kStatus_DEV_Detached;
            }
            break;

        default:
            break;
    }
    return status;
}

