/***************************************************************************//**
 * @file
 * @brief USB Host - Android Open Accessory Protocol (AOAP)
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
 * @defgroup USBH_AOAP USB Host AOAP API
 * @ingroup USBH
 * @brief   USB Host AOAP API
 *
 * @addtogroup USBH_AOAP
 * @{
 ********************************************************************************************************
 * @note             (1) The Android Open Accessory Protocol (AOAP) class driver follows the AOAP
 *                       specification available from Android's website here:
 *                       https://source.android.com/accessories/aoa.html
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _USBH_AOAP_H_
#define  _USBH_AOAP_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

#include  <usb/include/host/usbh_class.h>
#include  <usb/include/host/usbh_core_utils.h>
#include  <usb/include/host/usbh_core_opt_def.h>
#include  <usb/include/host/usbh_core_handle.h>

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

/********************************************************************************************************
 *                                       USB-TO-SERIAL FUNCTION HANDLE
 *******************************************************************************************************/

typedef USBH_CLASS_FNCT_HANDLE USBH_AOAP_FNCT_HANDLE;

/********************************************************************************************************
 *                                       APP NOTIFICATION FUNCTIONS
 *******************************************************************************************************/

typedef const struct usbh_aoap_app_fncts {
  //                                                               An AOAP function has been connected.
  void *(*Conn)(USBH_DEV_HANDLE dev_handle,
                USBH_FNCT_HANDLE fnct_handle,
                USBH_AOAP_FNCT_HANDLE aoap_fnct_handle);

  //                                                               An AOAP function has been disconn'd.
  void (*Disconn)(USBH_AOAP_FNCT_HANDLE aoap_fnct_handle,
                  void                  *p_arg);
} USBH_AOAP_APP_FNCTS;

/****************************************************************************************************//**
 *                                       AOAP CONFIGURATION STRUCTURES
 *
 * @note     (1) String must be null terminated. Storage must be persistent.
 * @{
 *******************************************************************************************************/

typedef struct usbh_aoap_str_cfg {                              // ---------- USB HOST AOAP CLASS STRING CFG ----------
  const CPU_CHAR *AccStrManufacturer;                           ///< Ptr to str of accessory manufacturer. See note (1).
  CPU_INT08U     AccStrManufacturerLen;                         ///< Len to str of accessory manufacturer.
  const CPU_CHAR *AccStrModel;                                  ///< Ptr to str of accessory model. See note (1).
  CPU_INT08U     AccStrModelLen;                                ///< Len to str of accessory model.
  const CPU_CHAR *AccStrDescription;                            ///< Ptr to str of accessory description. See note (1).
  CPU_INT08U     AccStrDescriptionLen;                          ///< Len to str of accessory description.
  const CPU_CHAR *AccStrVersion;                                ///< Ptr to str of accessory version. See note (1).
  CPU_INT08U     AccStrVersionLen;                              ///< Len to str of accessory version.
  const CPU_CHAR *AccStrURI;                                    ///< Ptr to str of accessory URI. See note (1).
  CPU_INT08U     AccStrURI_Len;                                 ///< Len to str of accessory URI.
  const CPU_CHAR *AccStrSerial;                                 ///< Ptr to str of accessory serial. See note (1).
  CPU_INT08U     AccStrSerialLen;                               ///< Len to str of accessory serial.
} USBH_AOAP_STR_CFG;

typedef struct usbh_aoap_cfg_optimize_spd {                     // ----------- AOAP CLASS OPTIMIZE SPD CFG ------------
  CPU_INT08U FnctQty;                                           ///< Qty of AOAP functions.
} USBH_AOAP_CFG_OPTIMIZE_SPD;

typedef struct usbh_aoap_cfg_init_alloc {                       // ------------ AOAP CLASS INIT ALLOC CFG -------------
  CPU_INT08U FnctQty;                                           ///< Qty of AOAP functions.
} USBH_AOAP_CFG_INIT_ALLOC;

typedef struct usbh_aoap_init_cfg {                             // ------------- AOAP INIT CONFIGURATIONS -------------
  CPU_SIZE_T                 BufAlignOctets;                    ///< Indicates desired mem alignment for internal buf.
  MEM_SEG                    *MemSegPtr;                        ///< Ptr to mem segment to use when allocating ctrl data.
  MEM_SEG                    *MemSegBufPtr;                     ///< Ptr to mem segment to use when allocating data buf.

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  USBH_AOAP_CFG_OPTIMIZE_SPD OptimizeSpd;                       ///< Configurations for optimize speed mode.
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  USBH_AOAP_CFG_INIT_ALLOC InitAlloc;                           ///< Configurations for allocations at init mode.
#endif
} USBH_AOAP_INIT_CFG;

///< @}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

extern const USBH_AOAP_INIT_CFG USBH_AOAP_InitCfgDflt;          // USBH AOAP class dflt configurations.

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
void USBH_AOAP_ConfigureBufAlignOctets(CPU_SIZE_T buf_align_octets);

void USBH_AOAP_ConfigureMemSeg(MEM_SEG *p_mem_seg,
                               MEM_SEG *p_mem_seg_buf);

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
void USBH_AOAP_ConfigureOptimizeSpdCfg(const USBH_AOAP_CFG_OPTIMIZE_SPD *p_optimize_spd_cfg);
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
void USBH_AOAP_ConfigureInitAllocCfg(const USBH_AOAP_CFG_INIT_ALLOC *p_init_alloc_cfg);
#endif
#endif

/********************************************************************************************************
 *                                               AOAP FUNCTIONS
 *******************************************************************************************************/

void USBH_AOAP_Init(USBH_AOAP_STR_CFG   *p_str_cfg,
                    USBH_AOAP_APP_FNCTS *p_app_fncts,
                    RTOS_ERR            *p_err);

void USBH_AOAP_StdReqTimeoutSet(CPU_INT32U std_req_timeout_ms,
                                RTOS_ERR   *p_err);

USBH_DEV_HANDLE USBH_AOAP_DevHandleGet(USBH_AOAP_FNCT_HANDLE aoap_fnct_handle,
                                       RTOS_ERR              *p_err);

CPU_INT32U USBH_AOAP_AccDataRx(USBH_AOAP_FNCT_HANDLE aoap_fnct_handle,
                               CPU_INT08U            *p_buf,
                               CPU_INT32U            buf_len,
                               CPU_INT32U            timeout,
                               RTOS_ERR              *p_err);

CPU_INT32U USBH_AOAP_AccDataTx(USBH_AOAP_FNCT_HANDLE aoap_fnct_handle,
                               CPU_INT08U            *p_buf,
                               CPU_INT32U            buf_len,
                               CPU_INT32U            timeout,
                               RTOS_ERR              *p_err);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
