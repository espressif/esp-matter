/***************************************************************************//**
 * @file
 * @brief USB General Operations
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/****************************************************************************************************//**
 * @defgroup USB_CONT USB Controller API
 * @addtogroup USB_CONT
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _USB_CTRLR_H_
#define  _USB_CTRLR_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <common/include/platform_mgr.h>

#include  <rtos_description.h>

#ifdef  RTOS_MODULE_USB_DEV_AVAIL
#include  <usb/include/device/usbd_core.h>
#endif

#ifdef  RTOS_MODULE_USB_HOST_AVAIL
#include  <usb/include/host/usbh_core.h>

#ifdef  RTOS_MODULE_USB_HOST_PBHCI_AVAIL
#include  <usb/include/host/usbh_pbhci.h>
#endif
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ---------------- USB CTRLR HW INFO -----------------
struct _rtos_usb_ctrlr_hw_info {
  PLATFORM_MGR_ITEM            Item;                            // Platform manager standard item structure.

#ifdef RTOS_MODULE_USB_DEV_AVAIL
  const USBD_DEV_CTRLR_HW_INFO *HW_InfoDevPtr;                  // Pointer to device specific hardware information.
#endif

#ifdef RTOS_MODULE_USB_HOST_AVAIL
  const USBH_HC_HW_INFO *HW_InfoHostPtr;                        // Pointer to host specific hardware information.
  const CPU_CHAR        *MainCtrlrName;                         // Name of main ctrlr if host controller is a companion.
#endif
};

/********************************************************************************************************
 *                                           FORWARD DECLARATIONS
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------ HW INFO SET UTILITY MACRO -------------
#if defined(RTOS_MODULE_USB_DEV_AVAIL) && defined(RTOS_MODULE_USB_HOST_AVAIL)
#define  USB_CTRLR_HW_INFO_SET(dev_ptr, host_ptr, main_ctrlr_name) \
  .HW_InfoDevPtr = (const  USBD_DEV_CTRLR_HW_INFO *)(dev_ptr),     \
  .HW_InfoHostPtr = (const  USBH_HC_HW_INFO *)(host_ptr),          \
  .MainCtrlrName = (main_ctrlr_name)
#elif defined(RTOS_MODULE_USB_DEV_AVAIL)
#define  USB_CTRLR_HW_INFO_SET(dev_ptr, host_ptr, main_ctrlr_name) \
  .HW_InfoDevPtr = (const  USBD_DEV_CTRLR_HW_INFO *)(dev_ptr),
#elif defined(RTOS_MODULE_USB_HOST_AVAIL)
#define  USB_CTRLR_HW_INFO_SET(dev_ptr, host_ptr, main_ctrlr_name) \
  .HW_InfoHostPtr = (const  USBH_HC_HW_INFO *)(host_ptr),          \
  .MainCtrlrName = (main_ctrlr_name)
#endif

//                                                                 ------ PLATFORM MGR ITEM CREATE AND REG MACRO ------
#if defined(RTOS_MODULE_USB_DEV_AVAIL) || defined(RTOS_MODULE_USB_HOST_AVAIL)
#define  USB_CTRLR_PLAT_MGR_ITEM_STD_CREATE_REG(name, dev_ptr, host_ptr) \
  static const struct _rtos_usb_ctrlr_hw_info _item = {                  \
    .Item.Type = PLATFORM_MGR_ITEM_TYPE_HW_INFO_USB,                     \
    .Item.StrID = (name),                                                \
    USB_CTRLR_HW_INFO_SET(dev_ptr, host_ptr, DEF_NULL) };                \
  PlatformMgrItemInitAdd((PLATFORM_MGR_ITEM *)&_item.Item)
#else
#define  USB_CTRLR_PLAT_MGR_ITEM_STD_CREATE_REG(name, dev_ptr, host_ptr)
#endif

/****************************************************************************************************//**
 *                                           USB_CTRLR_HW_INFO_REG()
 *
 * @brief    Registers a USB controller to the platform manager. The USB controller implements both
 *           device and host functionalities.
 *
 * @param    name        Unique name for the USB controller. It is recommended to follow the standard
 *                       "usbX" where X is a digit.
 *
 * @param    dev_ptr     Pointer to the USB device hardware information structure of type
 *                       USBD_DEV_CTRLR_HW_INFO.
 *
 * @param    host_ptr    Pointer to the USB host hardware information structure of type
 *                       USBH_HC_HW_INFO.
 *
 * @note     (1) This macro should normally be called from the BSP.
 *******************************************************************************************************/

//                                                                 --------- STD USB CTRLR HW INFO REG MACRO ----------
#if defined(RTOS_MODULE_USB_DEV_AVAIL) && defined(RTOS_MODULE_USB_HOST_AVAIL)
#define  USB_CTRLR_HW_INFO_REG(name, dev_ptr, host_ptr)                \
  do {                                                                 \
    if (((dev_ptr)->DrvInfoPtr != DEF_NULL)                            \
        || (((USBH_HC_HW_INFO *)(host_ptr))->BaseAddr != 0u)) {        \
      USB_CTRLR_PLAT_MGR_ITEM_STD_CREATE_REG(name, dev_ptr, host_ptr); \
    }                                                                  \
  } while (0)
#elif defined(RTOS_MODULE_USB_DEV_AVAIL)
#define  USB_CTRLR_HW_INFO_REG(name, dev_ptr, host_ptr)                \
  do {                                                                 \
    if ((dev_ptr)->DrvInfoPtr != DEF_NULL) {                           \
      USB_CTRLR_PLAT_MGR_ITEM_STD_CREATE_REG(name, dev_ptr, host_ptr); \
    }                                                                  \
  } while (0)
#elif defined(RTOS_MODULE_USB_HOST_AVAIL)
#define  USB_CTRLR_HW_INFO_REG(name, dev_ptr, host_ptr)                \
  do {                                                                 \
    if (((USBH_HC_HW_INFO *)(host_ptr))->BaseAddr != 0u) {             \
      USB_CTRLR_PLAT_MGR_ITEM_STD_CREATE_REG(name, dev_ptr, host_ptr); \
    }                                                                  \
  } while (0)
#else
#define  USB_CTRLR_HW_INFO_REG(name, dev_ptr, host_ptr)
#endif

/****************************************************************************************************//**
 *                                       USB_CTRLR_HW_INFO_DEV_ONLY_REG()
 *
 * @brief    Registers a USB Device controller to the platform manager.
 *
 * @param    name        Unique name for the USB device controller. It is recommended to follow the
 *                       standard "usbX" where X is a digit.
 *
 * @param    dev_ptr     Pointer to the USB device hardware information strucutre of type
 *                       USBD_DEV_CTRLR_HW_INFO.
 *
 * @note     (1) This macro should normally be called from the BSP.
 *******************************************************************************************************/

//                                                                 ------------ DEV ONLY HW INFO REG MACRO ------------
#if defined(RTOS_MODULE_USB_DEV_AVAIL)
#define  USB_CTRLR_HW_INFO_DEV_ONLY_REG(name, dev_ptr)                 \
  do {                                                                 \
    if ((dev_ptr)->DrvInfoPtr != DEF_NULL) {                           \
      USB_CTRLR_PLAT_MGR_ITEM_STD_CREATE_REG(name, dev_ptr, DEF_NULL); \
    }                                                                  \
  } while (0)
#else
#define  USB_CTRLR_HW_INFO_DEV_ONLY_REG(name, dev_ptr)
#endif

/****************************************************************************************************//**
 *                                       USB_CTRLR_HW_INFO_HOST_ONLY_REG()
 *
 * @brief    Registers a USB Host controller to the platform manager.
 *
 * @param    name        Unique name for the USB host controller. It is recommended to follow the
 *                       standard "usbX" where X is a digit.
 *
 * @param    host_ptr    Pointer to the USB host hardware information strucutre of type
 *                       USBH_HC_HW_INFO.
 *
 * @note     (1) This macro should normally be called from the BSP.
 *******************************************************************************************************/

//                                                                 ----------- HOST ONLY HW INFO REG MACRO ------------
#if defined(RTOS_MODULE_USB_HOST_AVAIL)
#define  USB_CTRLR_HW_INFO_HOST_ONLY_REG(name, host_ptr)                \
  do {                                                                  \
    if (((USBH_HC_HW_INFO *)(host_ptr))->BaseAddr != 0u) {              \
      USB_CTRLR_PLAT_MGR_ITEM_STD_CREATE_REG(name, DEF_NULL, host_ptr); \
    }                                                                   \
  } while (0)
#else
#define  USB_CTRLR_HW_INFO_HOST_ONLY_REG(name, host_ptr)
#endif

/****************************************************************************************************//**
 *                                   USB_CTRLR_HW_INFO_HOST_COMPANION_REG()
 *
 * @brief    Registers a USB Host companion controller to the platform manager.
 *
 * @param    name                Unique name for the USB host controller. It is recommended to follow
 *                               the standard "usbX" where X is a digit.
 *
 * @param    main_ctrlr_name     The name of the main USB controller for which this controller is a
 *                               companion.
 *
 * @param    host_ptr            Pointer to the USB host hardware information strucutre of type
 *                               USBH_HC_HW_INFO.
 *
 * @note     (1) This macro should normally be called from the BSP.
 *******************************************************************************************************/

//                                                                 --------- HOST COMPANION HW INFO REG MACRO ---------
#if defined(RTOS_MODULE_USB_HOST_AVAIL)
#define  USB_CTRLR_HW_INFO_HOST_COMPANION_REG(name, main_ctrlr_name, host_ptr) \
  do {                                                                         \
    if (((USBH_HC_HW_INFO *)(host_ptr))->BaseAddr != 0u) {                     \
      static const struct _rtos_usb_ctrlr_hw_info _item = {                    \
        .Item.Type = PLATFORM_MGR_ITEM_TYPE_HW_INFO_USB_HOST_COMPANION,        \
        .Item.StrID = (name),                                                  \
        USB_CTRLR_HW_INFO_SET(DEF_NULL, host_ptr, main_ctrlr_name) };          \
      PlatformMgrItemInitAdd((PLATFORM_MGR_ITEM *)&_item.Item);                \
    }                                                                          \
  } while (0)
#else
#define  USB_CTRLR_HW_INFO_HOST_COMPANION_REG(name, main_ctrlr_name, host_ptr)
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                         MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
