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

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_USB_DEV_AVAIL) || defined(RTOS_MODULE_USB_HOST_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define   MICRIUM_SOURCE
#define   USB_MODULE
#include  <cpu/include/cpu.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <common/include/platform_mgr.h>
#include  <rtos_description.h>

#include  <common/source/platform_mgr/platform_mgr_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <usb/include/usb_ctrlr.h>
#include  <usb/source/usb_ctrlr_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RTOS_MODULE_CUR                                 RTOS_CFG_MODULE_USBH | RTOS_CFG_MODULE_USBD

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           PRIVATE FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USB_CtrlrTypeGet()
 *
 * @brief    Get USB controller type.
 *
 * @param    name    Name of the USB controller.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
 *                   this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_FOUND
 *
 * @return   USB_CTRLR_TYPE_STD,            if USB controller type is standard.
 *           USB_CTRLR_TYPE_HOST_COMPANION, if USB controller is a companion of another USB host
 *                                           controller.
 *******************************************************************************************************/
USB_CTRLR_TYPE USB_CtrlrTypeGet(const CPU_CHAR *name,
                                RTOS_ERR       *p_err)
{
  PLATFORM_MGR_ITEM *p_item;
  USB_CTRLR_TYPE    type;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, USB_CTRLR_TYPE_NONE);

  p_item = PlatformMgrItemGetByName((CPU_CHAR *)name, p_err);

  type = (p_item->Type == PLATFORM_MGR_ITEM_TYPE_HW_INFO_USB) ? USB_CTRLR_TYPE_STD : USB_CTRLR_TYPE_HOST_COMPANION;

  return (type);
}

/****************************************************************************************************//**
 *                                           USB_CtrlrDevHwInfoGet()
 *
 * @brief    Get device controller hardware information for given USB controller.
 *
 * @param    name    Name of the USB controller.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
 *                   this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_FOUND
 *
 * @return   Device controller hardware information.
 *******************************************************************************************************/

#ifdef RTOS_MODULE_USB_DEV_AVAIL
USBD_DEV_CTRLR_HW_INFO *USB_CtrlrDevHwInfoGet(const CPU_CHAR *name,
                                              RTOS_ERR       *p_err)
{
  struct _rtos_usb_ctrlr_hw_info *p_ctrlr;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);

  p_ctrlr = (struct _rtos_usb_ctrlr_hw_info *)PlatformMgrItemGetByName((CPU_CHAR *)name,
                                                                       p_err);

  return ((USBD_DEV_CTRLR_HW_INFO *)p_ctrlr->HW_InfoDevPtr);
}
#endif

/****************************************************************************************************//**
 *                                           USB_CtrlrHostHwInfoGet()
 *
 * @brief    Get host controller hardware information for given USB controller.
 *
 * @param    name    Name of the USB controller.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
 *                   this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_FOUND
 *
 * @return   Host controller hardware information.
 *******************************************************************************************************/

#ifdef RTOS_MODULE_USB_HOST_AVAIL
USBH_HC_HW_INFO *USB_CtrlrHostHwInfoGet(const CPU_CHAR *name,
                                        CPU_CHAR       **p_main_ctrlr_name,
                                        RTOS_ERR       *p_err)
{
  PLATFORM_MGR_ITEM *p_item;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);

  p_item = PlatformMgrItemGetByName((CPU_CHAR *)name, p_err);

  if ((p_item->Type == PLATFORM_MGR_ITEM_TYPE_HW_INFO_USB_HOST_COMPANION)
      && (p_main_ctrlr_name != DEF_NULL)) {
    *p_main_ctrlr_name = (CPU_CHAR *)((struct _rtos_usb_ctrlr_hw_info *)p_item)->MainCtrlrName;
  }

  return ((USBH_HC_HW_INFO *)((struct _rtos_usb_ctrlr_hw_info *)p_item)->HW_InfoHostPtr);
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_DEV_AVAIL) || defined(RTOS_MODULE_USB_HOST_AVAIL))
