/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "usb_host_config.h"
#include "usb_host.h"
#include "usb_host_cdc.h"
#include "fsl_device_registers.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/


#define RNDIS_DATA_MESSAGE  (RNDIS_FRAME_MAX_FRAMELEN + RNDIS_DAT_MSG_HEADER_SIZE + 1)
#define RNDIS_CONTROL_MESSAGE  (1024U)

#define RNDIS_RESPONSE_AVAILABLE  (8U)


#define RNDIS_LWIP_OUTPUT (0x01)
#define RNDIS_DEVICE_ATTACH (0x02)
#define RNDIS_CONTROL_MSG (0x04)
#define RNDIS_INTERRUPT_MSG (0x08)
#define RNDIS_LWIP_INPUT (0x10)
#define RNDIS_DEVICE_INIT_READY (0x20)
#define RNDIS_EVENT_MASK (0xff)
   
/*! @brief init information */
typedef struct ethernetifConfig
{
    uint8_t controllerId;
    void *privateData;
} ethernetifConfig_t;
/*! @brief host app device attach/detach status */
typedef enum HostAppState
{
    kStatus_DEV_Idle = 0, /*!< there is no device attach/detach */
    kStatus_DEV_Attached, /*!< device is attached */
    kStatus_DEV_Detached, /*!< device is detached */
} host_app_state;
typedef struct _usb_host_rndis_instance_struct
{
    usb_device_handle deviceHandle;
    usb_host_class_handle classHandle;
    usb_host_interface_handle controlInterfaceHandle;
    usb_host_interface_handle dataInterfaceHandle;
    usb_host_handle hostHandle;
#if defined(USB_STACK_FREERTOS)
    EventGroupHandle_t event_group;
    EventGroupHandle_t lwipoutput;
#endif
    uint8_t* sendMessage;
    uint8_t* getMessage;
    uint8_t* outPutBuffer;
    uint8_t* inPutBuffer;
    void* netif;
    usb_host_cdc_acm_state_struct_t state;
    uint8_t deviceState;
    uint8_t previousState;
    uint8_t runState;
    uint8_t previousRunState;
    volatile uint8_t interruptRunState;
    uint8_t runWaitState;
    uint8_t dataSend;
    uint8_t responseAvailable;
    volatile uint8_t attach;
    uint8_t pollingInSending;
} usb_host_rndis_instance_struct_t;


/*! @brief host app run status */
typedef enum HostCdcRndisRunState
{
    kUSB_HostCdcRndisRunIdle = 0,                             /*!< idle */
    kUSB_HostCdcRndisRunSetControlInterface,                  /*!< execute set interface code */
    kUSB_HostCdcRndisRunWaitSetControlInterface,              /*!< wait set interface done */
    kUSB_HostCdcRndisRunSetControlInterfaceDone,              /*!< set interface is done, execute next step */
    kUSB_HostCdcRndisRunSetDataInterface,                     /*!< execute set interface code */
    kUSB_HostCdcRndisRunWaitSetDataInterface,                 /*!< wait set interface done */
    kUSB_HostCdcRndisRunSetDataInterfaceDone,                 /*!< set interface is done, execute next step */
    kUSB_HostCdcRndisRunWaitInitMsg,                          /*!< wait init message done */
    kUSB_HostCdcRndisRunWaitInitMsgDone,                      /*!< init message done, execute next step */
    kUSB_HostCdcRndisRunWaitGetMACAddress,                      /*!< wait query message done */
    kUSB_HostCdcRndisRunWaitGetMACAddressDone,                  /*!< init query done, execute next step */
    kUSB_HostCdcRndisRunWaitMaxmumFrame,                      /*!< wait query message done */
    kUSB_HostCdcRndisRunWaitMaxmumFrameDone,                  /*!< init query done, execute next step */
    kUSB_HostCdcRndisRunWaitSetMsg,                       /*!< wait set message done */
    kUSB_HostCdcRndisRunWaitSetMsgDone,                   /*!< init query done, execute next step */
    kUSB_HostCdcRndisRunWaitDataSend,                     /*!< wait data send done */
    kUSB_HostCdcRndisRunDataSendDone,                     /*!< data send is done, execute next step  */
    kUSB_HostCdcRndisRunDataReceive,                         /*!< wait data receive done */
    kUSB_HostCdcRndisRunWaitDataReceived,                    /*!< wait data receive done */
    kUSB_HostCdcRndisRunDataReceivedDone,                   /*!< data receive is done, execute next step  */
    kUSB_HostCdcRndisRunWaitGetEncapsulatedCommand,        /*!< wait data receive done */
    kUSB_HostCdcRndisRunGetEncapsulatedResponseDone,        /*!< data receive is done, execute next step  */
    kUSB_HostCdcRndisRunInterruptRecvPrime,
    kUSB_HostCdcRndisRunInterruptRecvDone,
    kUSB_HostCdcRndisRunGetState,
    kUSB_HostCdcRndisRunGetStateDone

} host_cdc_rndis_run_state;
/*!
* @brief host cdc task function.
*
* This function implements the host mouse action, it is used to create task.
*
* @param param   the host cdc instance pointer.
*/
extern void USB_HosCdcRndisTask(void *param);

/*!
 * @brief host cdc callback function.
 *
 * This function should be called in the host callback function.
 *
 * @param deviceHandle           device handle.
 * @param configurationHandle attached device's configuration descriptor information.
 * @param event_code           callback event code, please reference to enumeration host_event_t.
 *
 * @retval kStatus_USB_Success              The host is initialized successfully.
 * @retval kStatus_USB_NotSupported         The configuration don't contain hid mouse interface.
 */
extern usb_status_t USB_HostCdcRndisEvent(usb_device_handle deviceHandle,
                                     usb_host_configuration_handle configurationHandle,
                                     uint32_t event_code);
extern err_t USB_EthernetIfInIt(struct netif *netif);
/**
 * This function should be called when a packet is ready to be read
 * from the interface. 
 * It is used by bare-metal applications.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void usb_ethernetif_input( struct netif *netif);
