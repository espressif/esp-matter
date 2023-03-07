/***************************************************************************//**
 * @file
 * @brief USB Host - Communications Device Class (CDC)
 *        Abstract Control Model (ACM)
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
 * @defgroup USBH_ACM USB Host ACM API
 * @ingroup USBH
 * @brief   USB Host ACM API
 *
 * @addtogroup USBH_ACM
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _USBH_ACM_H_
#define  _USBH_ACM_H_

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

#include  <usb/include/host/usbh_cdc.h>
#include  <usb/include/host/usbh_core_handle.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  USBH_ACM_STD_REQ_TIMEOUT_DFLT                5000u

/********************************************************************************************************
 *                                   CDC ACM FUNCTION CAPABILITIES BITS
 *******************************************************************************************************/

#define  USBH_ACM_CAP_CALL_MGMT_HANDLE              DEF_BIT_00  // CDC ACM fnct handles call mgmt.
#define  USBH_ACM_CAP_CALL_MGMT_DCI                 DEF_BIT_01  // CDC ACM fnct can send/recv call mgmt info over DCI.
#define  USBH_ACM_CAP_COMM_FEATURE                  DEF_BIT_02  // CDC ACM fnct supports Set/Get/ClrCommFeature req.
#define  USBH_ACM_CAP_LINE_CODING                   DEF_BIT_03  // CDC ACM fnct supports Set/GetLineCoding and SetCtrlLineState req.
#define  USBH_ACM_CAP_SEND_BREAK                    DEF_BIT_04  // CDC ACM fnct supports SendBreak req.
#define  USBH_ACM_CAP_NET_CONN                      DEF_BIT_05  // CDC ACM fnct supports NetConn notification.

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       CDC ACM FUNCTION HANDLE
 *
 * Note(s) : (1) CDC ACM function handle contains CDC ACM function index.
 *******************************************************************************************************/

typedef USBH_CLASS_FNCT_HANDLE USBH_ACM_FNCT_HANDLE;

/********************************************************************************************************
 *                                   CDC ACM APP NOTIFICATION FUNCTIONS
 *******************************************************************************************************/

typedef const struct usbh_acm_app_fncts {
  //                                                               Indicates a CDC ACM function has been connected.
  void *(*Conn)(USBH_DEV_HANDLE dev_handle,
                USBH_FNCT_HANDLE fnct_handle,
                USBH_CDC_FNCT_HANDLE cdc_handle,
                USBH_ACM_FNCT_HANDLE acm_handle);

  //                                                               Indicates a CDC ACM function has been disconnected.
  void (*Disconn)(USBH_ACM_FNCT_HANDLE acm_handle,
                  void                 *p_arg);

  //                                                               Indicates a network connection.
  void (*NetConn)(USBH_ACM_FNCT_HANDLE acm_handle,
                  void                 *p_arg,
                  CPU_BOOLEAN          is_conn);

  //                                                               Response available.
  void (*RespAvail)(USBH_ACM_FNCT_HANDLE acm_handle,
                    void                 *p_arg);

  //                                                               Indicates a change in CDC ACM function serial state.
  void (*SerialStateChng)(USBH_ACM_FNCT_HANDLE acm_handle,
                          void                 *p_arg,
                          CPU_INT08U           serial_state);
} USBH_ACM_APP_FNCTS;

/********************************************************************************************************
 *                                       CDC ACM ASYNCHRONOUS FUNCTION
 *******************************************************************************************************/

typedef void (*USBH_ACM_ASYNC_FNCT) (USBH_ACM_FNCT_HANDLE acm_handle,
                                     CPU_INT08U           *p_buf,
                                     CPU_INT32U           buf_len,
                                     CPU_INT32U           xfer_len,
                                     void                 *p_arg,
                                     RTOS_ERR             err);

/********************************************************************************************************
 *                                   CDC ACM CONFIGURATION STRUCTURES
 *
 * Note(s) : (1) Can be DEF_NULL if USBH_CFG_OPTIMIZE_SPD_EN is set to DEF_DISABLED
 *
 *           (2) Can be DEF_NULL if USBH_CFG_INIT_ALLOC_EN is set to DEF_DISABLED
 *******************************************************************************************************/

//                                                                 USB HOST ACM SUBCLASS OPTIMIZE SPD CONFIGURATIONS -
typedef struct usbh_acm_cfg_optimize_spd {
  CPU_INT08U FnctQty;                                           // Quantity of CDC ACM functions.
} USBH_ACM_CFG_OPTIMIZE_SPD;

//                                                                 - USB HOST ACM SUBCLASS INIT ALLOC CONFIGURATIONS --
typedef struct usbh_acm_cfg_init_alloc {
  CPU_INT08U FnctQty;                                           // Quantity of CDC ACM functions.
  CPU_INT08U AsyncXferQty;                                      // Quantity of asynchronous transfers.
} USBH_ACM_CFG_INIT_ALLOC;

//                                                                 ------------- ACM INIT CONFIGURATIONS --------------
typedef struct usbh_acm_init_cfg {
  MEM_SEG                   *MemSegPtr;                         // Ptr to mem segment to use when allocating ctrl data.

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  USBH_ACM_CFG_OPTIMIZE_SPD OptimizeSpd;                        // Configurations for optimize speed mode.
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  USBH_ACM_CFG_INIT_ALLOC InitAlloc;                            // Configurations for allocations at init mode.
#endif
} USBH_ACM_INIT_CFG;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

extern const USBH_ACM_INIT_CFG USBH_ACM_InitCfgDflt;            // USBH ACM class dflt configurations.

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
void USBH_ACM_ConfigureMemSeg(MEM_SEG *p_mem_seg);

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
void USBH_ACM_ConfigureOptimizeSpdCfg(const USBH_ACM_CFG_OPTIMIZE_SPD *p_optimize_spd_cfg);
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
void USBH_ACM_ConfigureInitAllocCfg(const USBH_ACM_CFG_INIT_ALLOC *p_init_alloc_cfg);
#endif
#endif

/********************************************************************************************************
 *                                               ACM FUNCTIONS
 *******************************************************************************************************/

void USBH_ACM_Init(USBH_ACM_APP_FNCTS *p_acm_app_fncts,
                   RTOS_ERR           *p_err);

void USBH_ACM_StdReqTimeoutSet(CPU_INT32U std_req_timeout_ms,
                               RTOS_ERR   *p_err);

CPU_INT08U USBH_ACM_CapabilitiesGet(USBH_ACM_FNCT_HANDLE acm_handle,
                                    RTOS_ERR             *p_err);

CPU_INT16U USBH_ACM_EncapsulatedCmdTx(USBH_ACM_FNCT_HANDLE acm_handle,
                                      CPU_INT08U           *p_buf,
                                      CPU_INT16U           buf_len,
                                      CPU_INT32U           timeout,
                                      RTOS_ERR             *p_err);

CPU_INT16U USBH_ACM_EncapsulatedRespRx(USBH_ACM_FNCT_HANDLE acm_handle,
                                       CPU_INT08U           *p_buf,
                                       CPU_INT16U           buf_len,
                                       CPU_INT32U           timeout,
                                       RTOS_ERR             *p_err);

void USBH_ACM_CommFeatureSet(USBH_ACM_FNCT_HANDLE acm_handle,
                             CPU_INT08U           feature,
                             CPU_INT16U           data,
                             CPU_INT32U           timeout,
                             RTOS_ERR             *p_err);

CPU_INT16U USBH_ACM_CommFeatureGet(USBH_ACM_FNCT_HANDLE acm_handle,
                                   CPU_INT08U           feature,
                                   CPU_INT32U           timeout,
                                   RTOS_ERR             *p_err);

void USBH_ACM_CommFeatureClr(USBH_ACM_FNCT_HANDLE acm_handle,
                             CPU_INT08U           feature,
                             CPU_INT32U           timeout,
                             RTOS_ERR             *p_err);

void USBH_ACM_LineCodingSet(USBH_ACM_FNCT_HANDLE acm_handle,
                            USBH_CDC_LINECODING  *p_line_coding,
                            CPU_INT32U           timeout,
                            RTOS_ERR             *p_err);

void USBH_ACM_LineCodingGet(USBH_ACM_FNCT_HANDLE acm_handle,
                            USBH_CDC_LINECODING  *p_line_coding,
                            CPU_INT32U           timeout,
                            RTOS_ERR             *p_err);

void USBH_ACM_CtrlLineStateSet(USBH_ACM_FNCT_HANDLE acm_handle,
                               CPU_INT16U           ctrl_signal,
                               CPU_INT32U           timeout,
                               RTOS_ERR             *p_err);

void USBH_ACM_BrkSend(USBH_ACM_FNCT_HANDLE acm_handle,
                      CPU_INT16U           dur,
                      CPU_INT32U           timeout,
                      RTOS_ERR             *p_err);

void USBH_ACM_RxAsync(USBH_ACM_FNCT_HANDLE acm_handle,
                      CPU_INT08U           *p_buf,
                      CPU_INT32U           buf_len,
                      USBH_ACM_ASYNC_FNCT  async_fnct,
                      void                 *p_async_arg,
                      RTOS_ERR             *p_err);

void USBH_ACM_TxAsync(USBH_ACM_FNCT_HANDLE acm_handle,
                      CPU_INT08U           *p_buf,
                      CPU_INT32U           buf_len,
                      USBH_ACM_ASYNC_FNCT  async_fnct,
                      void                 *p_async_arg,
                      RTOS_ERR             *p_err);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                           MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
