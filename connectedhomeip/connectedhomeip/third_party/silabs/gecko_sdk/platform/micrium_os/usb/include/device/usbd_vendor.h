/***************************************************************************//**
 * @file
 * @brief USB Device Vendor Class
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
 * @defgroup USBD_VENDOR USB Device Vendor API
 * @ingroup USBD
 * @brief   USB Device Vendor API
 *
 * @addtogroup USBD_VENDOR
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _USBD_VENDOR_H_
#define  _USBD_VENDOR_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <usb/include/device/usbd_core.h>
#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <common/include/lib_mem.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       VENDOR CLASS CONFIGURATION
 * @{
 *******************************************************************************************************/

typedef struct usbd_vendor_qty_cfg {                            // ------- VENDOR CLASS CONFIGURATION STRUCTURE -------
  CPU_INT08U ClassInstanceQty;                                  ///< Quantity of vendor class instacnes.
  CPU_INT08U ConfigQty;                                         ///< Qty of configs in which a class instance can be seen.
} USBD_VENDOR_QTY_CFG;

typedef struct usbd_vendor_init_cfg {                           // ------------ VENDOR INIT CONFIGURATIONS ------------
  CPU_INT08U MsExtPropertiesQty;                                ///< Quantity of Microsoft extended properties.
  MEM_SEG    *MemSegPtr;                                        ///< Ptr to mem segment to use when allocating ctrl data.
} USBD_VENDOR_INIT_CFG;

///< @}

//                                                                 App callback used for vendor-specific req.
typedef CPU_BOOLEAN (*USBD_VENDOR_REQ_FNCT)  (CPU_INT08U           class_nbr,
                                              CPU_INT08U           dev_nbr,
                                              const USBD_SETUP_REQ *p_setup_req);

//                                                                 App callback used for async comm.
typedef void (*USBD_VENDOR_ASYNC_FNCT)(CPU_INT08U class_nbr,
                                       void       *p_buf,
                                       CPU_INT32U buf_len,
                                       CPU_INT32U xfer_len,
                                       void       *p_callback_arg,
                                       RTOS_ERR   err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

extern const USBD_VENDOR_INIT_CFG USBD_Vendor_InitCfgDflt;      // USBD Vendor class dflt configurations.

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   CONFIGURATION OVERRIDE FUNCTIONS
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBD_Vendor_ConfigureMsExtPropertiesQty(CPU_INT08U ms_ext_properties_qty);

void USBD_Vendor_ConfigureMemSeg(MEM_SEG *p_mem_seg);
#endif

/********************************************************************************************************
 *                                           VENDOR FUNCTIONS
 *******************************************************************************************************/

void USBD_Vendor_Init(USBD_VENDOR_QTY_CFG *p_cfg,
                      RTOS_ERR            *p_err);

CPU_INT08U USBD_Vendor_Add(CPU_BOOLEAN          intr_en,
                           CPU_INT16U           interval,
                           USBD_VENDOR_REQ_FNCT req_callback,
                           RTOS_ERR             *p_err);

void USBD_Vendor_ConfigAdd(CPU_INT08U class_nbr,
                           CPU_INT08U dev_nbr,
                           CPU_INT08U config_nbr,
                           RTOS_ERR   *p_err);

CPU_BOOLEAN USBD_Vendor_IsConn(CPU_INT08U class_nbr);

#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)
void USBD_Vendor_MS_ExtPropertyAdd(CPU_INT08U       class_nbr,
                                   CPU_INT08U       property_type,
                                   const CPU_INT08U *p_property_name,
                                   CPU_INT16U       property_name_len,
                                   const CPU_INT08U *p_property,
                                   CPU_INT32U       property_len,
                                   RTOS_ERR         *p_err);
#endif

CPU_INT32U USBD_Vendor_Rd(CPU_INT08U class_nbr,
                          void       *p_buf,
                          CPU_INT32U buf_len,
                          CPU_INT16U timeout,
                          RTOS_ERR   *p_err);

CPU_INT32U USBD_Vendor_Wr(CPU_INT08U  class_nbr,
                          void        *p_buf,
                          CPU_INT32U  buf_len,
                          CPU_INT16U  timeout,
                          CPU_BOOLEAN end,
                          RTOS_ERR    *p_err);

void USBD_Vendor_RdAsync(CPU_INT08U             class_nbr,
                         void                   *p_buf,
                         CPU_INT32U             buf_len,
                         USBD_VENDOR_ASYNC_FNCT async_fnct,
                         void                   *p_async_arg,
                         RTOS_ERR               *p_err);

void USBD_Vendor_WrAsync(CPU_INT08U             class_nbr,
                         void                   *p_buf,
                         CPU_INT32U             buf_len,
                         USBD_VENDOR_ASYNC_FNCT async_fnct,
                         void                   *p_async_arg,
                         CPU_BOOLEAN            end,
                         RTOS_ERR               *p_err);

CPU_INT32U USBD_Vendor_IntrRd(CPU_INT08U class_nbr,
                              void       *p_buf,
                              CPU_INT32U buf_len,
                              CPU_INT16U timeout,
                              RTOS_ERR   *p_err);

CPU_INT32U USBD_Vendor_IntrWr(CPU_INT08U  class_nbr,
                              void        *p_buf,
                              CPU_INT32U  buf_len,
                              CPU_INT16U  timeout,
                              CPU_BOOLEAN end,
                              RTOS_ERR    *p_err);

void USBD_Vendor_IntrRdAsync(CPU_INT08U             class_nbr,
                             void                   *p_buf,
                             CPU_INT32U             buf_len,
                             USBD_VENDOR_ASYNC_FNCT async_fnct,
                             void                   *p_async_arg,
                             RTOS_ERR               *p_err);

void USBD_Vendor_IntrWrAsync(CPU_INT08U             class_nbr,
                             void                   *p_buf,
                             CPU_INT32U             buf_len,
                             USBD_VENDOR_ASYNC_FNCT async_fnct,
                             void                   *p_async_arg,
                             CPU_BOOLEAN            end,
                             RTOS_ERR               *p_err);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
