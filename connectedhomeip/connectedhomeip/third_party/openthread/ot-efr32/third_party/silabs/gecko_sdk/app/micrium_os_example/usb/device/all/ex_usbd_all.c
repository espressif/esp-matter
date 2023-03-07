/***************************************************************************//**
 * @file
 * @brief USB Device Example
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
 ********************************************************************************************************
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>

#if (defined(RTOS_MODULE_USB_DEV_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/rtos_err.h>

#include  <usb/include/device/usbd_core.h>

#include  <ex_description.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                      USB DEVICE CONTROLLER NAME
 *
 * Note(s) : (1) usb0 is used by default. It may not be possible to use this one as device. Change it if
 *               needed.
 *******************************************************************************************************/

#if (!defined(EX_USBD_CTRLR_NAME))
#define  EX_USBD_CTRLR_NAME                           "usb0"    // TODO: Modify usb controller name.
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
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (defined(RTOS_MODULE_USB_DEV_HID_AVAIL) && defined(EX_USBD_HID_INIT_AVAIL))
void Ex_USBD_HID_Init(CPU_INT08U dev_nbr,
                      CPU_INT08U cfg_nbr_fs,
                      CPU_INT08U cfg_nbr_hs);
#endif

#if (defined(RTOS_MODULE_USB_DEV_CDC_AVAIL) &&  defined(RTOS_MODULE_USB_DEV_ACM_AVAIL) && defined(EX_USBD_CDC_ACM_INIT_AVAIL))
void Ex_USBD_ACM_SerialInit(CPU_INT08U dev_nbr,
                            CPU_INT08U cfg_nbr_fs,
                            CPU_INT08U cfg_nbr_hs);
#endif

#if (defined(RTOS_MODULE_USB_DEV_EEM_AVAIL) && defined(EX_USBD_CDC_EEM_INIT_AVAIL))
void Ex_USBD_CDC_EEM_Init(CPU_INT08U dev_nbr,
                          CPU_INT08U config_nbr_fs,
                          CPU_INT08U config_nbr_hs);
#endif

#if (defined(RTOS_MODULE_USB_DEV_VENDOR_AVAIL) && defined(EX_USBD_VENDOR_INIT_AVAIL))
void Ex_USBD_Vendor_Init(CPU_INT08U dev_nbr,
                         CPU_INT08U config_nbr_fs,
                         CPU_INT08U config_nbr_hs);
#endif

#if (defined(RTOS_MODULE_USB_DEV_MSC_AVAIL) && defined(EX_USBD_MSC_INIT_AVAIL))
void Ex_USBD_MSC_Init(CPU_INT08U dev_nbr,
                      CPU_INT08U cfg_nbr_fs,
                      CPU_INT08U cfg_nbr_hs);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                          LOCAL CONSTATNTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                             Ex_USBD_Init()
 *
 * @brief  Example of initialization of the USB device module. Also initializes the available
 *         classes.
 *******************************************************************************************************/
void Ex_USBD_Init(void)
{
  CPU_INT08U       config_nbr_fs;
  CPU_INT08U       config_nbr_hs = USBD_CONFIG_NBR_NONE;
  CPU_INT08U       dev_nbr;
  RTOS_ERR         err;
  USBD_QTY_CFG     cfg_qty_usbd;
  USBD_DEV_CFG     cfg_usbd_dev;
  USBD_DEV_DRV_CFG cfg_usbd_dev_drv;

  //                                                               ---------------- INITIALIZING USBD -----------------
  cfg_qty_usbd.DevQty = 1u;
  cfg_qty_usbd.ConfigQty = 2u;
  cfg_qty_usbd.IF_Qty = 10u;
  cfg_qty_usbd.IF_AltQty = 10u;
  cfg_qty_usbd.IF_GrpQty = 2u;
  cfg_qty_usbd.EP_DescQty = 10u;
  cfg_qty_usbd.URB_ExtraQty = 10u;
  cfg_qty_usbd.StrQty = 15u;
  cfg_qty_usbd.EP_OpenQty = 10u;

  USBD_Init(&cfg_qty_usbd, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               -------------- ADDING DFLT DEV CTRLR ---------------
#if (defined(RTOS_MODULE_USB_DEV_CDC_AVAIL) &&  defined(RTOS_MODULE_USB_DEV_ACM_AVAIL) && defined(EX_USBD_CDC_ACM_INIT_AVAIL))
  cfg_usbd_dev.ProductID = 0x1234u;
#elif (defined(RTOS_MODULE_USB_DEV_VENDOR_AVAIL) && defined(EX_USBD_VENDOR_INIT_AVAIL))
  cfg_usbd_dev.ProductID = 0x1001u;
#else
  cfg_usbd_dev.ProductID = 0x0001u;
#endif
  cfg_usbd_dev.VendorID = 0xFFFEu;
  cfg_usbd_dev.DeviceBCD = 0x0100u;
  cfg_usbd_dev.ManufacturerStrPtr = "Micrium inc";
  cfg_usbd_dev.ProductStrPtr = "Product";
  cfg_usbd_dev.SerialNbrStrPtr = "1234567890ABCDEF";
  cfg_usbd_dev.LangId = USBD_LANG_ID_ENGLISH_US;

  cfg_usbd_dev_drv.EP_OpenQty = 10u;
  cfg_usbd_dev_drv.URB_ExtraQty = 0u;

  dev_nbr = USBD_DevAdd(EX_USBD_CTRLR_NAME,
                        DEF_NULL,
                        &cfg_usbd_dev,
                        &cfg_usbd_dev_drv,
                        DEF_NULL,                               // No bus functions specified.
                        &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               Adding full-speed configuration...
  config_nbr_fs = USBD_ConfigAdd(dev_nbr,
                                 DEF_BIT_NONE,                  // TODO: Add any attribute related to configuration.
                                 100u,                          // TODO: Adjust max power draw by dev from host.
                                 USBD_DEV_SPD_FULL,
                                 "Full-Speed config",
                                 &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

#if (USBD_CFG_HS_EN == DEF_ENABLED)
  //                                                               Adding high-speed configuration...
  config_nbr_hs = USBD_ConfigAdd(dev_nbr,
                                 DEF_BIT_NONE,                  // TODO: Add any attribute related to configuration.
                                 100u,                          // TODO: Adjust max power draw by dev from host.
                                 USBD_DEV_SPD_HIGH,
                                 "High-Speed config",
                                 &err);
  if (err.Code == RTOS_ERR_NONE) {
    //                                                             Indicating what is other speed config counterpart...
    USBD_ConfigOtherSpeed(dev_nbr,
                          config_nbr_fs,
                          config_nbr_hs,
                          &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  } else {
    EX_TRACE("Warning - Unable to add High-speed configuration");
  }
#endif

#if (defined(RTOS_MODULE_USB_DEV_CDC_AVAIL) &&  defined(RTOS_MODULE_USB_DEV_ACM_AVAIL) && defined(EX_USBD_CDC_ACM_INIT_AVAIL))
  Ex_USBD_ACM_SerialInit(dev_nbr,
                         config_nbr_fs,
                         config_nbr_hs);
#endif

#if (defined(RTOS_MODULE_USB_DEV_EEM_AVAIL) && defined(EX_USBD_CDC_EEM_INIT_AVAIL))
  Ex_USBD_CDC_EEM_Init(dev_nbr,
                       config_nbr_fs,
                       config_nbr_hs);
#endif

#if (defined(RTOS_MODULE_USB_DEV_HID_AVAIL) && defined(EX_USBD_HID_INIT_AVAIL))
  Ex_USBD_HID_Init(dev_nbr,
                   config_nbr_fs,
                   config_nbr_hs);
#endif

#if (defined(RTOS_MODULE_USB_DEV_MSC_AVAIL) && defined(EX_USBD_MSC_INIT_AVAIL))
  Ex_USBD_MSC_Init(dev_nbr,
                   config_nbr_fs,
                   config_nbr_hs);
#endif

#if (defined(RTOS_MODULE_USB_DEV_VENDOR_AVAIL) && defined(EX_USBD_VENDOR_INIT_AVAIL))
  Ex_USBD_Vendor_Init(dev_nbr,
                      config_nbr_fs,
                      config_nbr_hs);
#endif
}

/****************************************************************************************************//**
 *                                             Ex_USBD_Start()
 *
 * @brief  Example of USB device controller start.
 *******************************************************************************************************/
void Ex_USBD_Start(void)
{
  CPU_INT08U dev_nbr;
  RTOS_ERR   err;

  dev_nbr = USBD_DevNbrGetFromName(EX_USBD_CTRLR_NAME);

  USBD_DevStart(dev_nbr, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_USB_DEV_AVAIL
