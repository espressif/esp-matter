/***************************************************************************//**
 * @file
 * @brief USB Host Example - USB Host module initialization
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 * This example shows how to initialize the USB Host module and how to add a Host Controller. It also
 * initializes all the available class drivers.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>

#if (defined(RTOS_MODULE_USB_HOST_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/rtos_err.h>

#include  <usb/include/host/usbh_core.h>

#include  <ex_description.h>

#ifdef RTOS_MODULE_USB_HOST_PBHCI_AVAIL
#include  <usb/include/host/usbh_pbhci.h>
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                      USB HOST CONTROLLER NAME
 *
 * Note(s) : (1) usb1 is used by default. It may not be possible to use this one as host. Change it if
 *               needed.
 *******************************************************************************************************/

#if (!defined(EX_USBH_CTRLR_NAME))
#define  EX_USBH_CTRLR_NAME                           "usb0"    // TODO: Modify usb controller name.
#endif

/********************************************************************************************************
 *                                               LOGGING
 *
 * Note(s) : (1) This example outputs information to the console via the function printf() via a macro
 *               called EX_TRACE(). This can be modified or disabled if printf() is not supported.
 *******************************************************************************************************/

#ifndef EX_TRACE
#include  <stdio.h>
#define  EX_TRACE(...)                                      printf(__VA_ARGS__)
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (defined(RTOS_MODULE_USB_HOST_AOAP_AVAIL) && defined(EX_USBH_AOAP_INIT_AVAIL))
void Ex_USBH_AOAP_Init(void);
#endif

#if (defined(RTOS_MODULE_USB_HOST_CDC_AVAIL) &&  defined(RTOS_MODULE_USB_HOST_ACM_AVAIL) && defined(EX_USBH_CDC_ACM_INIT_AVAIL))
void Ex_USBH_CDC_ACM_Init(void);
#endif

#if (defined(RTOS_MODULE_USB_HOST_HID_AVAIL) && defined(EX_USBH_HID_INIT_AVAIL))
void Ex_USBH_HID_Init(void);
#endif

#if (defined(RTOS_MODULE_USB_HOST_MSC_AVAIL) && defined(EX_USBH_MSC_INIT_AVAIL))
void Ex_USBH_MSC_Init(void);
#endif

#if (defined(RTOS_MODULE_USB_HOST_USB2SER_AVAIL) && defined(EX_USBH_USB2SER_INIT_AVAIL))
void Ex_USBH_USB2SER_Init(void);
#endif

CPU_BOOLEAN Ex_USBH_DevConnAccept(void);

CPU_BOOLEAN Ex_USBH_DevConfigAccept(void);

void Ex_USBH_DevConfig(USBH_DEV_HANDLE dev_handle,
                       CPU_INT08U      config_nbr,
                       RTOS_ERR        err);

void Ex_USBH_FnctConnFail(USBH_FNCT_HANDLE fnct_handle,
                          RTOS_ERR         err);

void Ex_USBH_DevConnFail(CPU_INT08U hub_addr,
                         CPU_INT08U port_nbr,
                         RTOS_ERR   err);

void Ex_USBH_DevDisconn(USBH_DEV_HANDLE dev_handle);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static USBH_EVENT_FNCTS Ex_USBH_EventFncts = {
  .DevConnAccept = Ex_USBH_DevConnAccept,
  .DevConfigAccept = Ex_USBH_DevConfigAccept,
  .DevConfig = Ex_USBH_DevConfig,
  .FnctConnFail = Ex_USBH_FnctConnFail,
  .DevConnFail = Ex_USBH_DevConnFail,
  .DevResume = DEF_NULL,                                        // Not yet supported. For futur considerations.
  .DevDisconn = Ex_USBH_DevDisconn
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                          LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                             Ex_USBH_Init()
 *
 * @brief  Example of initialization of the USB host module. Also initializes the available class
 *         drivers.
 *******************************************************************************************************/
void Ex_USBH_Init(void)
{
  RTOS_ERR err;

  USBH_ConfigureEventFncts(&Ex_USBH_EventFncts);

  USBH_Init(1u, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

#ifdef RTOS_MODULE_USB_HOST_PBHCI_AVAIL
  USBH_PBHCI_Init(&err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
#endif

  (void)USBH_HC_Add(EX_USBH_CTRLR_NAME,
                    DEF_NULL,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

#if (defined(RTOS_MODULE_USB_HOST_AOAP_AVAIL) && defined(EX_USBH_AOAP_INIT_AVAIL))
  Ex_USBH_AOAP_Init();
#endif

#if (defined(RTOS_MODULE_USB_HOST_CDC_AVAIL) &&  defined(RTOS_MODULE_USB_HOST_ACM_AVAIL) && defined(EX_USBH_CDC_ACM_INIT_AVAIL))
  Ex_USBH_CDC_ACM_Init();
#endif

#if (defined(RTOS_MODULE_USB_HOST_HID_AVAIL) && defined(EX_USBH_HID_INIT_AVAIL))
  Ex_USBH_HID_Init();
#endif

#if (defined(RTOS_MODULE_USB_HOST_MSC_AVAIL) && defined(EX_USBH_MSC_INIT_AVAIL))
  Ex_USBH_MSC_Init();
#endif

#if (defined(RTOS_MODULE_USB_HOST_USB2SER_AVAIL) && defined(EX_USBH_USB2SER_INIT_AVAIL))
  Ex_USBH_USB2SER_Init();
#endif
}

/****************************************************************************************************//**
 *                                             Ex_USBH_Start()
 *
 * @brief  Example of host controller start.
 *******************************************************************************************************/
void Ex_USBH_Start(void)
{
  USBH_HC_HANDLE hc_handle;
  RTOS_ERR       err;

  hc_handle = USBH_HC_HandleGetFromName(EX_USBH_CTRLR_NAME);

  USBH_HC_Start(hc_handle, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                         Ex_USBH_DevConnAccept()
 *
 * @brief  Accepts a device connection.
 *
 * @return  DEF_YES, Device connection accepted by application.
 *          DEF_NO,  Device connection declined by application.
 *
 * @note   (1) Basic information on the connected device can be retrieved within this function via the
 *             standard USB Host dev API using the special handle value: USBH_DEV_HANDLE_NOTIFICATION.
 *
 * @note   (2) If device connection is declined, the device will be disconnected.
 *******************************************************************************************************/
CPU_BOOLEAN Ex_USBH_DevConnAccept(void)
{
  return (DEF_YES);
}

/****************************************************************************************************//**
 *                                        Ex_USBH_DevConfigAccept()
 *
 * @brief  Accepts a device configuration.
 *
 * @return  DEF_YES, Device configuration accepted by application.
 *          DEF_NO,  Device configuration declined by application.
 *
 * @note   (1) Basic information on the connected device can be retrieved within this function via the
 *             standard USB Host dev API using the special handle value: USBH_DEV_HANDLE_NOTIFICATION.
 *
 * @note   (2) If configuration is declined, the device will remain in addressed state. The
 *             application is free to call the function USBH_DevConfigSet() later on for this device.
 *******************************************************************************************************/
CPU_BOOLEAN Ex_USBH_DevConfigAccept(void)
{
  return (DEF_YES);
}

/****************************************************************************************************//**
 *                                           Ex_USBH_DevConfig()
 *
 * @brief  Notifies application that a device is now in the configured state and ready for
 *         communication.
 *
 * @param  dev_handle  Handle to USB device.
 *
 * @param  config_nbr  Configuration number that was set.
 *
 * @param  err         Error code. If error code is not RTOS_ERR_NONE, device configuration failed.
 *******************************************************************************************************/
void Ex_USBH_DevConfig(USBH_DEV_HANDLE dev_handle,
                       CPU_INT08U      config_nbr,
                       RTOS_ERR        err)
{
  PP_UNUSED_PARAM(dev_handle);
  PP_UNUSED_PARAM(config_nbr);

  if (err.Code == RTOS_ERR_NONE) {
    EX_TRACE("Device configured.\r\n");
  } else {
    EX_TRACE("Device configuration failed with error: %s.\r\n", RTOS_ERR_DESC_STR_GET(err.Code));
  }
}

/****************************************************************************************************//**
 *                                         Ex_USBH_FnctConnFail()
 *
 * @brief  Notifies that one of the device's function connection has failed.
 *
 * @param  fnct_handle  Handle to the function.
 *
 * @param  err          Error code.
 *
 * @note   (1) If the function is part of a composite device, the device connection may succeed if
 *             at least one function connects successfuly.
 *******************************************************************************************************/
void Ex_USBH_FnctConnFail(USBH_FNCT_HANDLE fnct_handle,
                          RTOS_ERR         err)
{
  PP_UNUSED_PARAM(fnct_handle);
#if (RTOS_ERR_CFG_STR_EN == DEF_DISABLED)
  PP_UNUSED_PARAM(err);
#endif

  EX_TRACE("Function connection failed with err: %s\r\n", RTOS_ERR_DESC_STR_GET(err.Code));
}

/****************************************************************************************************//**
 *                                          Ex_USBH_DevConnFail()
 *
 * @brief  Notifies that a device connection has failed.
 *
 * @param  hub_addr  Hub address on which the device was connected.
 *
 * @param  port_nbr  Hub port number on which the device was connected.
 *
 * @param  err       Error code.
 *******************************************************************************************************/
void Ex_USBH_DevConnFail(CPU_INT08U hub_addr,
                         CPU_INT08U port_nbr,
                         RTOS_ERR   err)
{
  PP_UNUSED_PARAM(hub_addr);
  PP_UNUSED_PARAM(port_nbr);
#if (RTOS_ERR_CFG_STR_EN == DEF_DISABLED)
  PP_UNUSED_PARAM(err);
#endif

  EX_TRACE("Device connection failed with err: %s\r\n", RTOS_ERR_DESC_STR_GET(err.Code));
}

/****************************************************************************************************//**
 *                                          Ex_USBH_DevDisconn()
 *
 * @brief  Notifies a device disconnection.
 *
 * @param  dev_handle  Handle to the USB device.
 *******************************************************************************************************/
void Ex_USBH_DevDisconn(USBH_DEV_HANDLE dev_handle)
{
  PP_UNUSED_PARAM(dev_handle);

  EX_TRACE("Device disconnected\r\n");
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_USB_HOST_AVAIL
