/***************************************************************************//**
 * @file
 * @brief CAN
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
 * @defgroup CAN CAN API
 * @brief CAN API
 *
 * @defgroup CAN_BUS CAN Bus API
 * @ingroup CAN
 * @brief CAN Bus API
 *
 * @addtogroup CAN_BUS
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef _CAN_BUS_H_
#define _CAN_BUS_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                             INCLUDES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/platform_mgr.h>

#include  <em_can.h>
#include  <em_gpio.h>
#include  <em_cmu.h>
#include  <em_timer.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                              DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          CAN_CTRLR_REG()
 *
 * @brief    Registers a CAN controller to the platform manager.
 *
 * @param    name        Unique name for the CAN controller. It is recommended to follow the standard
 *                       "canX",  where X is a digit.
 *
 * @param    p_drv_info  Pointer to the CAN Bus driver hardware information structure
 *                       of type CAN_CTRLR_DRV_INFO.
 *
 * @note     (1) This macro should normally be called from the BSP.
 *******************************************************************************************************/

#define  CAN_CTRLR_REG(name, p_drv_info)                      \
  do {                                                        \
    static const struct can_ctrlr_pm_item _item = {           \
      .Item.Type = PLATFORM_MGR_ITEM_TYPE_HW_INFO_CAN,        \
      .Item.StrID = (name),                                   \
      .BusDrvInfoPtr = (void *)(p_drv_info)                   \
    };                                                        \
    PlatformMgrItemInitAdd((PLATFORM_MGR_ITEM *)&_item.Item); \
  } while (0)

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/*
 ********************************************************************************************************
 *                                          Handle to CAN bus
 *******************************************************************************************************/

typedef struct can_bus_handle *CAN_BUS_HANDLE;

/*
 ********************************************************************************************************
 *                                  CAN controller hardware information
 *******************************************************************************************************/

typedef struct can_ctrlr_hw_info {
  CPU_ADDR   BaseAddr;                                          ///< Controller's registers base address.
  const void *InfoExtPtr;                                       ///< Extended (driver specific) hardware information.
  CPU_INT08U IF_Rx;                                             ///< BSP Rx interface.
  CPU_INT08U IF_Tx;                                             ///< BSP Tx interface.
} CAN_CTRLR_HW_INFO;

/*
 ********************************************************************************************************
 *                                          BSP API structure
 *******************************************************************************************************/

typedef struct can_ctrlr_bsp_api {
  void (*Open)(void);                                           ///< BSP open function pointer.

  void (*Close)(void);                                          ///< BSP close function pointer.

  void (*IntCtrl)(CAN_BUS_HANDLE bus_handle);                   ///< BSP interrupt control pointer.

  void (*TmrCfg)(CPU_INT32U tmr_period);                        ///< BSP timer configuration pointer.
} CAN_CTRLR_BSP_API;

/*
 ********************************************************************************************************
 *                                       CAN controller information
 *******************************************************************************************************/

typedef struct can_ctrlr_drv_info {
  CAN_CTRLR_HW_INFO       HW_Info;                              ///< Hardware information structure.
  const CAN_CTRLR_BSP_API *BSP_API_Ptr;                         ///< Pointer to BSP API structure.
} CAN_CTRLR_DRV_INFO;

/****************************************************************************************************//**
 *                                 CAN controller platform manager item
 *
 * @note     (1) This structure should ONLY be instantiated via the macro CAN_CTRLR_REG().
 *******************************************************************************************************/

struct can_ctrlr_pm_item {
  PLATFORM_MGR_ITEM Item;                                       ///< Platform manager item.
  void              *BusDrvInfoPtr;                             ///< Pointer to bus driver information structure.
};

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _CAN_BUS_H_
