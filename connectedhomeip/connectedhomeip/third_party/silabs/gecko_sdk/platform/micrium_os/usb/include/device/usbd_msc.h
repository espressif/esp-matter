/***************************************************************************//**
 * @file
 * @brief USB Device MSC Class
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
 * @defgroup USBD_MSC USB Device MSC API
 * @ingroup USBD
 * @brief   USB Device MSC API
 *
 * @addtogroup USBD_MSC
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _USBD_MSC_H_
#define  _USBD_MSC_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <usb/include/device/usbd_core.h>
#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef const struct usbd_scsi_storage_api USBD_SCSI_STORAGE_API;

typedef struct usbd_scsi_lu_info {
  USBD_SCSI_STORAGE_API *SCSI_StorageAPI_Ptr;

  const CPU_CHAR        *VendIdPtr;
  const CPU_CHAR        *ProdIdPtr;
  CPU_INT32U            ProdRevLevel;
  CPU_BOOLEAN           IsRdOnly;
} USBD_SCSI_LU_INFO;

typedef const struct usbd_scsi_lu_fncts {
  void (*HostEjectEvent)(CPU_INT08U lu_nbr);
} USBD_SCSI_LU_FNCTS;

/****************************************************************************************************//**
 *                                       CONFIGURATION STRUCTURE
 * @{
 *******************************************************************************************************/

typedef struct usbd_msc_qty_cfg {
  CPU_INT08U ClassInstanceQty;                                  ///< Quantity of MSC class instacnes.
  CPU_INT08U ConfigQty;                                         ///< Qty of configs in which a class instance can be seen.

  CPU_INT08U LUN_Qty;                                           ///< Quantity of Logical Unit per class instance.
} USBD_MSC_QTY_CFG;

typedef struct usbd_msc_init_cfg {                              // ------------- MSC INIT CONFIGURATIONS --------------
  CPU_SIZE_T BufAlignOctets;                                    ///< Indicates desired mem alignment for internal buf.
  CPU_INT32U DataBufLen;                                        ///< Data Buffer Length, in octets.

  MEM_SEG    *MemSegPtr;                                        ///< Ptr to mem segment to use when allocating ctrl data.
  MEM_SEG    *MemSegBufPtr;                                     ///< Ptr to mem segment to use when allocating data buf.
} USBD_MSC_INIT_CFG;

///< @}

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

#define  USBD_MSC_LU_NBR_INVALID                        DEF_INT_08U_MAX_VAL

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

extern const USBD_MSC_INIT_CFG USBD_MSC_InitCfgDflt;            // USBD MSC class dflt configurations.

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
void USBD_MSC_ConfigureBufAlignOctets(CPU_SIZE_T buf_align_octets);

void USBD_MSC_ConfigureDataBufLen(CPU_INT32U data_buf_len);

void USBD_MSC_ConfigureMemSeg(MEM_SEG *p_mem_seg,
                              MEM_SEG *p_mem_seg_buf);
#endif

/********************************************************************************************************
 *                                               MSC FUNCTIONS
 *******************************************************************************************************/

void USBD_MSC_Init(const USBD_MSC_QTY_CFG *p_qty_cfg,
                   RTOS_ERR               *p_err);

CPU_INT08U USBD_MSC_Add(RTOS_TASK_CFG *p_msc_task_cfg,
                        RTOS_ERR      *p_err);

void USBD_MSC_TaskPrioSet(CPU_INT08U     class_nbr,
                          RTOS_TASK_PRIO prio,
                          RTOS_ERR       *p_err);

CPU_BOOLEAN USBD_MSC_ConfigAdd(CPU_INT08U class_nbr,
                               CPU_INT08U dev_nbr,
                               CPU_INT08U config_nbr,
                               RTOS_ERR   *p_err);

CPU_INT08U USBD_MSC_SCSI_LunAdd(CPU_INT08U         class_nbr,
                                USBD_SCSI_LU_INFO  *p_lu_info,
                                USBD_SCSI_LU_FNCTS *p_lu_fncts,
                                RTOS_ERR           *p_err);

void USBD_MSC_SCSI_LunAttach(CPU_INT08U class_nbr,
                             CPU_INT08U lu_nbr,
                             CPU_CHAR   *media_name,
                             RTOS_ERR   *p_err);

void USBD_MSC_SCSI_LunDetach(CPU_INT08U class_nbr,
                             CPU_INT08U lu_nbr,
                             RTOS_ERR   *p_err);

CPU_BOOLEAN USBD_MSC_IsConn(CPU_INT08U class_nbr);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                           MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
