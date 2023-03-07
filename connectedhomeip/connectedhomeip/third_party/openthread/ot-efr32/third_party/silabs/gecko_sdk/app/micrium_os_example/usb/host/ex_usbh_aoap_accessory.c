/***************************************************************************//**
 * @file
 * @brief USB Host Example - Android Open Accessory Protocol
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
 * The AOAP class driver is meant to be used with devices running Google's Android operating system that
 * supports accessory mode v1.0 or above.
 *
 * This example application sets the Accessory strings and will be notified once an Android device is
 * connected.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>

#if (defined(RTOS_MODULE_USB_HOST_AOAP_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/rtos_err.h>

#include  <usb/include/host/usbh_core.h>
#include  <usb/include/host/usbh_aoap.h>

#include  <ex_description.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

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

static void *Ex_USBH_AOAP_AccessoryConn(USBH_DEV_HANDLE       dev_handle,
                                        USBH_FNCT_HANDLE      fnct_handle,
                                        USBH_AOAP_FNCT_HANDLE aoap_fnct_handle);

static void Ex_USBH_AOAP_AccessoryDisconn(USBH_AOAP_FNCT_HANDLE aoap_fnct_handle,
                                          void                  *p_arg);

/********************************************************************************************************
 ********************************************************************************************************
 *                                          LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 AOAP application functions structure.
static const USBH_AOAP_APP_FNCTS Ex_USBH_AOAP_AccessoryAppFncts = {
  Ex_USBH_AOAP_AccessoryConn,
  Ex_USBH_AOAP_AccessoryDisconn
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           Ex_USBH_AOAP_Init()
 *
 * @brief  Example of initialization of Android Accessory class driver.
 *******************************************************************************************************/
void Ex_USBH_AOAP_Init(void)
{
  USBH_AOAP_STR_CFG cfg_str_aoap;
  RTOS_ERR          err;

  // TODO: Modify strings to match your Android application.
  cfg_str_aoap.AccStrManufacturer = "Micrium inc";
  cfg_str_aoap.AccStrManufacturerLen = 11u;
  cfg_str_aoap.AccStrModel = "AOAP demo";
  cfg_str_aoap.AccStrModelLen = 9u;
  cfg_str_aoap.AccStrDescription = "AOAP application for demonstration purposes";
  cfg_str_aoap.AccStrDescriptionLen = 43u;
  cfg_str_aoap.AccStrVersion = "1.0";
  cfg_str_aoap.AccStrVersionLen = 3u;
  cfg_str_aoap.AccStrURI = "www.micrium.com";
  cfg_str_aoap.AccStrURI_Len = 15u;
  cfg_str_aoap.AccStrSerial = "123456789";
  cfg_str_aoap.AccStrSerialLen = 9u;

  USBH_AOAP_Init(&cfg_str_aoap,
                 &Ex_USBH_AOAP_AccessoryAppFncts,
                 &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                      Ex_USBH_AOAP_AccessoryConn()
 *
 * @brief  An Android accessory function has been connected.
 *
 * @param  dev_handle        Handle on USB device.
 *
 * @param  fnct_handle       Handle on USB function.
 *
 * @param  aoap_fnct_handle  Handle on Android accessory class function handle.
 *
 * @return  Pointer to application specific data associated to Android accessory function.
 *******************************************************************************************************/
static void *Ex_USBH_AOAP_AccessoryConn(USBH_DEV_HANDLE       dev_handle,
                                        USBH_FNCT_HANDLE      fnct_handle,
                                        USBH_AOAP_FNCT_HANDLE aoap_fnct_handle)
{
  EX_TRACE("AOAP example: Function connected\r\n");

  PP_UNUSED_PARAM(dev_handle);
  PP_UNUSED_PARAM(fnct_handle);
  PP_UNUSED_PARAM(aoap_fnct_handle);

  // A this point, the connected Android device has been switched to Accessory mode.

  /* TODO: A task can be posted to communicate with your Android accessory application.
   *       No protocol is defined for this. You can use the functions USBH_AOAP_AccDataTx()
   *       and USBH_AOAP_AccDataRx() that send/receive raw data.
   */

  return (DEF_NULL);                                            // No data associated to device.
}

/****************************************************************************************************//**
 *                                     Ex_USBH_AOAP_AccessoryDisconn()
 *
 * @brief  An Android accessory function has been disconnected.
 *
 * @param  aoap_fnct_handle  Handle on Android accessory class function handle.
 *
 * @param  p_arg             Pointer to application specific data associated to Android
 *                           accessory function returned to Ex_USBH_AOAP_AccessoryConn().
 *******************************************************************************************************/
static void Ex_USBH_AOAP_AccessoryDisconn(USBH_AOAP_FNCT_HANDLE aoap_fnct_handle,
                                          void                  *p_arg)
{
  PP_UNUSED_PARAM(aoap_fnct_handle);
  PP_UNUSED_PARAM(p_arg);

  EX_TRACE("AOAP example: Function disconnected\r\n");
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_USB_HOST_AOAP_AVAIL
