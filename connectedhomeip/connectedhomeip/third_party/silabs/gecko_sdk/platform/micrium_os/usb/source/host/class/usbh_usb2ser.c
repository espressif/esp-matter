/***************************************************************************//**
 * @file
 * @brief USB Host - USB-To-Serial Class
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

#if (defined(RTOS_MODULE_USB_HOST_USB2SER_AVAIL))

#if (!defined(RTOS_MODULE_USB_HOST_AVAIL))

#error USB HOST USB2SER class requires USB Host core. Make sure it is part of your project and that \
  RTOS_MODULE_USB_HOST_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define   USBH_USB2SER_MODULE
#define   MICRIUM_SOURCE

#include  <em_core.h>

#include  <cpu/include/cpu.h>

#include  <common/include/lib_str.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/source/op_lock/op_lock_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>
#include  <rtos_description.h>

#include  <usb/include/host/usbh_usb2ser.h>
#include  <usb/include/host/usbh_core_dev.h>
#include  <usb/include/host/usbh_core_fnct.h>
#include  <usb/include/host/usbh_core_ep.h>
#include  <usb/include/host/usbh_core_handle.h>

#include  <usb/source/host/core/usbh_core_priv.h>
#include  <usb/source/host/class/usbh_usb2ser_priv.h>
#include  <usb/source/host/cmd/usbh_cmd_priv.h>
#include  <usb/source/host/class/usbh_class_core_priv.h>
#include  <usb/source/host/class/usbh_class_ep_priv.h>
#include  <usb/source/host/class/usbh_class_common_priv.h>

#ifdef RTOS_MODULE_USB_HOST_USB2SER_FTDI_AVAIL
#include  <usb/include/host/usbh_usb2ser_ftdi.h>
#endif

#ifdef RTOS_MODULE_USB_HOST_USB2SER_SILABS_AVAIL
#include  <usb/include/host/usbh_usb2ser_silabs.h>
#endif

#ifdef RTOS_MODULE_USB_HOST_USB2SER_PROLIFIC_AVAIL
#include  <usb/include/host/usbh_usb2ser_prolific.h>
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (USBH, CLASS, USB2SER)
#define  RTOS_MODULE_CUR                                 RTOS_CFG_MODULE_USBH

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       DEFAULT CONFIGURATIONS
 *******************************************************************************************************/

const USBH_USB2SER_ADAPTER_DRV_ENTRY USBH_USB2SER_DrvEntryTblDfltCfg[] = {
#ifdef RTOS_MODULE_USB_HOST_USB2SER_FTDI_AVAIL
  { .DrvPtr = &USBH_USB2SER_FTDI_Drv, .CustomIdTbl = DEF_NULL, .CustomIdTblLen = 0u },
#endif

#ifdef RTOS_MODULE_USB_HOST_USB2SER_SILABS_AVAIL
  { .DrvPtr = &USBH_USB2SER_SILABS_Drv, .CustomIdTbl = DEF_NULL, .CustomIdTblLen = 0u },
#endif

#ifdef RTOS_MODULE_USB_HOST_USB2SER_PROLIFIC_AVAIL
  { .DrvPtr = &USBH_USB2SER_PROLIFIC_Drv, .CustomIdTbl = DEF_NULL, .CustomIdTblLen = 0u },
#endif

  { .DrvPtr = DEF_NULL, .CustomIdTbl = DEF_NULL, .CustomIdTblLen = 0u },
};

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const USBH_USB2SER_INIT_CFG USBH_USB2SER_InitCfgDflt = {
  .BufAlignOctets = LIB_MEM_BUF_ALIGN_AUTO,
  .HS_En = DEF_ENABLED,
  .RxBufQty = 1u,

  .DevDrvEntryTbl = (USBH_USB2SER_ADAPTER_DRV_ENTRY *)USBH_USB2SER_DrvEntryTblDfltCfg,

  .MemSegPtr = DEF_NULL,
  .MemSegBufPtr = DEF_NULL,

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  .OptimizeSpd = { .FnctQty = 1u },
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  .InitAlloc = { .FnctQty = 1u, .TxAsyncXferQty = 2u }
#endif
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           ADAPTER DRIVER ENTRY
 *******************************************************************************************************/

typedef struct usbh_usb2ser_adapter_drv_entry_mgmt {
  USBH_USB2SER_ADAPTER_DRV *DrvPtr;

  USBH_USB2SER_APP_ID      *CustomIdTbl;                        // Table assumed to be persistent.
  CPU_INT08U               CustomIdTblLen;

  KAL_LOCK_HANDLE          LockHandle;
} USBH_USB2SER_ADAPTER_DRV_ENTRY_MGMT;

/********************************************************************************************************
 *                                       USB2SER FUNCTION STRUCT
 *******************************************************************************************************/

typedef struct usbh_usb2ser_fnct_data {
  USBH_USB2SER_ADAPTER_DRV_ENTRY_MGMT *AdapterDrvEntryMgmtPtr;  // Pointer to adapter driver entry.
  void                                *AllocAdapterFnctPtr;     // Pointer to allocated mem for adapter drv fnct data.

  CPU_INT08U                          **RxBufPtrTbl;            // Rx buf pool.

  USBH_USB2SER_SERIAL_STATUS          StatusCur;                // Current serial status.

  void                                *AppFnctsArgPtr;          // Ptr to app fcnts arg.

  USBH_USB2SER_FNCT_ADAPTER_DRV_DATA  AdapterDrvData;           // Adapter driver specific data.

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_DISABLED)
  struct usbh_usb2ser_fnct_data       *NextPtr;                 // Ptr to next USB-to-serial fnct struct.
#endif
} USBH_USB2SER_FNCT_DATA;

/********************************************************************************************************
 *                                               ASYNC TX DATA
 *******************************************************************************************************/

typedef struct usbh_usb2ser_async_tx_data {
  USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *AdapterDataPtr;           // Ptr to USB2SER fnct adapter drv data.

  USBH_USB2SER_ASYNC_TX_FNCT         TxCmplFnct;                // App function to call on xfer completion.
  void                               *TxCmplArgPtr;             // App function arg.
} USBH_USB2SER_ASYNC_TX_DATA;

/********************************************************************************************************
 *                                       USB2SER CLASS DATA STRUCT
 *******************************************************************************************************/

typedef struct usbh_usb2ser {
  USBH_CLASS_FNCT_POOL                FnctPool;                 // Pool of usb2ser functions.

  MEM_DYN_POOL                        TxAsyncXferPool;          // Pool of Tx async xfer.

  USBH_USB2SER_APP_FNCTS              *AppFnctsPtr;             // Ptr to app notification fncts.

  USBH_USB2SER_ADAPTER_DRV_ENTRY_MGMT *AdapterDrvEntryMgmtTbl;  // Pointer to table of driver entries.
  CPU_INT08U                          AdapterDrvEntryTblLen;    // Length of  table of driver entries.

  CPU_INT16U                          MaxAdapterDrvFnctLen;     // Max len of drv data       among all adapter drivers.
  CPU_INT32U                          MaxAdapterCtrlBufLen;     // Max len of drv ctrl   buf among all adapter drivers.
  CPU_INT32U                          MaxAdapterStatusBufLen;   // Max len of drv status buf among all adapter drivers.
  CPU_INT32U                          MaxAdapterRxBufLen;       // Max len of drv rx     buf among all adapter drivers.

  void                                *TempAdapterFnctPtr;      // Pointer to temporary adapter driver function data.
} USBH_USB2SER;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static USBH_USB2SER *USBH_USB2SER_Ptr = DEF_NULL;

CPU_INT32U USBH_USB2SER_ReqTimeout = USBH_USB2SER_STD_REQ_TIMEOUT_DFLT;

/********************************************************************************************************
 *                                           CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
USBH_USB2SER_INIT_CFG USBH_USB2SER_InitCfg = {
  .BufAlignOctets = LIB_MEM_BUF_ALIGN_AUTO,
  .HS_En = DEF_ENABLED,
  .RxBufQty = 1u,

  .DevDrvEntryTbl = (USBH_USB2SER_ADAPTER_DRV_ENTRY *)USBH_USB2SER_DrvEntryTblDfltCfg,

  .MemSegPtr = DEF_NULL,
  .MemSegBufPtr = DEF_NULL,

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  .OptimizeSpd = { .FnctQty = 0u },
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  .InitAlloc = { .FnctQty = 0u, .TxAsyncXferQty = 0u }
#endif
};
#else
extern USBH_USB2SER_INIT_CFG USBH_USB2SER_InitCfg;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 -------------- CLASS DRIVER FUNCTION ---------------
static CPU_BOOLEAN USBH_USB2SER_ProbeFnct(USBH_DEV_HANDLE  dev_handle,
                                          USBH_FNCT_HANDLE fnct_handle,
                                          CPU_INT08U       class_code,
                                          void             **pp_class_fnct,
                                          RTOS_ERR         *p_err);

static CPU_BOOLEAN USBH_USB2SER_EP_Probe(void        *p_class_fnct,
                                         CPU_INT08U  if_ix,
                                         CPU_INT08U  ep_type,
                                         CPU_BOOLEAN ep_dir_in);

static void USBH_USB2SER_EP_Open(void           *p_class_fnct,
                                 USBH_EP_HANDLE ep_handle,
                                 CPU_INT08U     if_ix,
                                 CPU_INT08U     ep_type,
                                 CPU_BOOLEAN    ep_dir_in);

static void USBH_USB2SER_EP_Close(void           *p_class_fnct,
                                  USBH_EP_HANDLE ep_handle,
                                  CPU_INT08U     if_ix);

static void USBH_USB2SER_Conn(void *p_class_fnct);

static void USBH_USB2SER_IF_AltSet(void       *p_class_fnct,
                                   CPU_INT08U if_ix);

static void USBH_USB2SER_Disconn(void *p_class_fnct);

static void USBH_USB2SER_Suspend(void *p_class_fnct);

static void USBH_USB2SER_Resume(void *p_class_fnct);

#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
static void USBH_USB2SER_UnInit(void);
#endif

#ifdef RTOS_MODULE_COMMON_SHELL_AVAIL
static void USBH_USB2SER_TraceDump(void                *p_class_fnct,
                                   CPU_INT32U          opt,
                                   USBH_CMD_TRACE_FNCT trace_fnct);
#endif

//                                                                 ---------------- INTERNAL FUNCTIONS ----------------
static void USBH_USB2SER_ModemCtrlApply(USBH_USB2SER_FNCT_DATA *p_usb2ser_fnct_data,
                                        CPU_INT08U             dtr_en,
                                        CPU_INT08U             dtr_set,
                                        CPU_INT08U             rts_en,
                                        CPU_INT08U             rts_set,
                                        RTOS_ERR               *p_err);

static CPU_BOOLEAN USBH_USB2SER_FnctAllocCallback(void *p_class_fnct_data);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           USB2SER CLASS DRIVER
 ********************************************************************************************************
 *******************************************************************************************************/

static USBH_CLASS_DRV USBH_USB2SER_ClassDrv = {
  DEF_NULL,
  USBH_USB2SER_ProbeFnct,
  USBH_USB2SER_EP_Probe,
  USBH_USB2SER_EP_Open,
  USBH_USB2SER_EP_Close,
  USBH_USB2SER_Conn,
  USBH_USB2SER_IF_AltSet,
  USBH_USB2SER_Disconn,
  USBH_USB2SER_Suspend,
  USBH_USB2SER_Resume,
#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
  USBH_USB2SER_UnInit,
#else
  DEF_NULL,
#endif
#ifdef RTOS_MODULE_COMMON_SHELL_AVAIL
  USBH_USB2SER_TraceDump,
#endif
  (CPU_CHAR *)"USB-To-Serial",
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                   USBH_USB2SER_ConfigureBufAlignOctets()
 *
 * @brief    Configures the alignment of the internal buffers.
 *
 * @param    buf_align_octets    Buffer alignment, in octets.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the USB2SER class is initialized via the
 *               USBH_USB2SER_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_USB2SER_ConfigureBufAlignOctets(CPU_SIZE_T buf_align_octets)
{
  RTOS_ASSERT_CRITICAL((USBH_USB2SER_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_USB2SER_InitCfg.BufAlignOctets = buf_align_octets;
}
#endif

/****************************************************************************************************//**
 *                                       USBH_USB2SER_ConfigureHS_En()
 *
 * @brief    Configures the support for high-speed adapter devices.
 *
 * @param    hs_en   DEF_ENABLED,  if support for high-speed adapter devcies is enabled.
 *                   DEF_DISABLED, if support for high-speed adapter devcies is disabled.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the USB2SER class is initialized via the
 *               USBH_USB2SER_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_USB2SER_ConfigureHS_En(CPU_BOOLEAN hs_en)
{
  RTOS_ASSERT_CRITICAL((USBH_USB2SER_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_USB2SER_InitCfg.HS_En = hs_en;
}
#endif

/****************************************************************************************************//**
 *                                       USBH_USB2SER_ConfigureRxBufQty()
 *
 * @brief    Configures the quantity of receive buffers per adapter.
 *
 * @param    rx_buf_qty  Quantity of receive buffers.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the USB2SER class is initialized via the
 *               USBH_USB2SER_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_USB2SER_ConfigureRxBufQty(CPU_INT08U rx_buf_qty)
{
  RTOS_ASSERT_CRITICAL((USBH_USB2SER_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_USB2SER_InitCfg.RxBufQty = rx_buf_qty;
}
#endif

/****************************************************************************************************//**
 *                                   USBH_USB2SER_ConfigureDrvEntryTbl()
 *
 * @brief    Configures the adapter drivers table.
 *
 * @param    p_tbl   Pointer to table that contains the adapter driver entries. Table MUST be
 *                   null-terminated.
 *                   [Content MUST be persistent]
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the USB2SER class is initialized via the
 *               USBH_USB2SER_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_USB2SER_ConfigureDrvEntryTbl(USBH_USB2SER_ADAPTER_DRV_ENTRY *p_tbl)
{
  RTOS_ASSERT_CRITICAL((USBH_USB2SER_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_USB2SER_InitCfg.DevDrvEntryTbl = p_tbl;
}
#endif

/****************************************************************************************************//**
 *                                       USBH_USB2SER_ConfigureMemSeg()
 *
 * @brief    Configures the memory segment to use when allocating control data and buffers.
 *
 * @param    p_mem_seg       Pointer to memory segment to use when allocating control data.
 *                           Can be the same segment used for p_mem_seg_buf.
 *                           DEF_NULL means general purpose heap segment.
 *
 * @param    p_mem_seg_buf   Pointer to memory segment to use when allocating data buffers.
 *                           Can be the same segment used for p_mem_seg.
 *                           DEF_NULL means general purpose heap segment.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the USB2SER class is initialized via the
 *               USBH_USB2SER_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_USB2SER_ConfigureMemSeg(MEM_SEG *p_mem_seg,
                                  MEM_SEG *p_mem_seg_buf)
{
  RTOS_ASSERT_CRITICAL((USBH_USB2SER_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_USB2SER_InitCfg.MemSegPtr = p_mem_seg;
  USBH_USB2SER_InitCfg.MemSegBufPtr = p_mem_seg_buf;
}
#endif

/****************************************************************************************************//**
 *                                   USBH_USB2SER_ConfigureOptimizeSpdCfg()
 *
 * @brief    Sets the configurations required when optimize speed mode is enabled.
 *
 * @param    p_optimize_spd_cfg  Pointer to the structure containing the configurations for the
 *                               optimize speed mode.
 *
 * @note     (1) This function MUST be called before the USB2SER class is initialized via the
 *               USBH_USB2SER_Init() function.
 *
 * @note     (2) This function MUST be called when the USBH_CFG_OPTIMIZE_SPD_EN configuration is set
 *               to DEF_ENABLED.
 *******************************************************************************************************/

#if ((RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED) \
  && (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED))
void USBH_USB2SER_ConfigureOptimizeSpdCfg(const USBH_USB2SER_CFG_OPTIMIZE_SPD *p_optimize_spd_cfg)
{
  RTOS_ASSERT_CRITICAL((USBH_USB2SER_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_USB2SER_InitCfg.OptimizeSpd = *p_optimize_spd_cfg;
}
#endif

/****************************************************************************************************//**
 *                                   USBH_USB2SER_ConfigureInitAllocCfg()
 *
 * @brief    Sets the configurations required when allocation at initialization mode is enabled.
 *
 * @param    p_init_alloc_cfg    Pointer to the structure containing the configurations for the
 *                               allocation at initialization mode.
 *
 * @note     (1) This function MUST be called before the USB2SER class is initialized via the
 *               USBH_USB2SER_Init() function.
 *
 * @note     (2) This function MUST be called when the USBH_CFG_INIT_ALLOC_EN configuration is set
 *               to DEF_ENABLED.
 *******************************************************************************************************/

#if ((RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED) \
  && (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED))
void USBH_USB2SER_ConfigureInitAllocCfg(const USBH_USB2SER_CFG_INIT_ALLOC *p_init_alloc_cfg)
{
  RTOS_ASSERT_CRITICAL((USBH_USB2SER_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_USB2SER_InitCfg.InitAlloc = *p_init_alloc_cfg;
}
#endif

/****************************************************************************************************//**
 *                                           USBH_USB2SER_Init()
 *
 * @brief    Initializes the USB-to-serial Class.
 *
 * @param    p_app_fncts     Pointer to the callback functions structure that will be used to notify
 *                           application of change in USB to Serial device status.
 *                           [Content MUST be persistent]
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                               - RTOS_ERR_SEG_OVF
 *                               - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                               - RTOS_ERR_POOL_EMPTY
 *                               - RTOS_ERR_NOT_AVAIL
 *******************************************************************************************************/
void USBH_USB2SER_Init(USBH_USB2SER_APP_FNCTS *p_app_fncts,
                       RTOS_ERR               *p_err)
{
  CPU_INT08U                     cnt;
  CPU_INT08U                     class_fnct_qty;
  CPU_INT08U                     adapter_drv_entry_tbl_len;
  USBH_USB2SER_ADAPTER_DRV_ENTRY *p_adapter_drv_entry_tbl;

  if (USBH_USB2SER_Ptr != DEF_NULL) {                           // Chk if USB2SER already init.
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    return;
  }

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  RTOS_ASSERT_DBG_ERR_SET((USBH_USB2SER_InitCfg.OptimizeSpd.FnctQty != 0u), *p_err, RTOS_ERR_NOT_READY,; );
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  RTOS_ASSERT_DBG_ERR_SET((USBH_USB2SER_InitCfg.InitAlloc.FnctQty != 0u), *p_err, RTOS_ERR_NOT_READY,; );
#endif

#if ((USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED) \
  && (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED))
  RTOS_ASSERT_DBG_ERR_SET((USBH_USB2SER_InitCfg.OptimizeSpd.FnctQty == USBH_USB2SER_InitCfg.InitAlloc.FnctQty), *p_err, RTOS_ERR_INVALID_CFG,; );
#endif

  //                                                               At least one adapter driver needed.
  RTOS_ASSERT_DBG_ERR_SET((USBH_USB2SER_InitCfg.DevDrvEntryTbl != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  //                                                               Determine length of driver adapter entry table.
  adapter_drv_entry_tbl_len = 0u;
  p_adapter_drv_entry_tbl = USBH_USB2SER_InitCfg.DevDrvEntryTbl;
  while (p_adapter_drv_entry_tbl[adapter_drv_entry_tbl_len].DrvPtr != DEF_NULL) {
    adapter_drv_entry_tbl_len++;
  }
  //                                                               AdapterDrvEntryTblLen cannot be 0.
  RTOS_ASSERT_DBG_ERR_SET((adapter_drv_entry_tbl_len != 0), *p_err, RTOS_ERR_INVALID_ARG,; );

  //                                                               Ensure all adapter driver entries are valid.
  for (cnt = 0u; cnt < adapter_drv_entry_tbl_len; cnt++) {
    RTOS_ASSERT_DBG_ERR_SET(((p_adapter_drv_entry_tbl[cnt].DrvPtr->DevAdapterDrvPtr != DEF_NULL)
                             && (p_adapter_drv_entry_tbl[cnt].DrvPtr->ClassDrvPtr != DEF_NULL)), *p_err, RTOS_ERR_INVALID_ARG,; );
  }

  //                                                               Alloc USB2SER class data.
  USBH_USB2SER_Ptr = (USBH_USB2SER *)Mem_SegAlloc("USBH - USB2SER Data",
                                                  USBH_USB2SER_InitCfg.MemSegPtr,
                                                  sizeof(USBH_USB2SER),
                                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Copy adapter driver entry table.
  USBH_USB2SER_Ptr->AdapterDrvEntryMgmtTbl = (USBH_USB2SER_ADAPTER_DRV_ENTRY_MGMT *)Mem_SegAlloc("USBH - USB2SER adapter drv entry tbl",
                                                                                                 USBH_USB2SER_InitCfg.MemSegPtr,
                                                                                                 sizeof(USBH_USB2SER_ADAPTER_DRV_ENTRY_MGMT) * adapter_drv_entry_tbl_len,
                                                                                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBH_USB2SER_Ptr->AdapterDrvEntryTblLen = adapter_drv_entry_tbl_len;
  USBH_USB2SER_Ptr->AppFnctsPtr = p_app_fncts;
  USBH_USB2SER_Ptr->MaxAdapterDrvFnctLen = 0u;
  USBH_USB2SER_Ptr->MaxAdapterCtrlBufLen = 0u;
  USBH_USB2SER_Ptr->MaxAdapterStatusBufLen = 0u;
  USBH_USB2SER_Ptr->MaxAdapterRxBufLen = 0u;

  for (cnt = 0u; cnt < adapter_drv_entry_tbl_len; cnt++) {      // Browse through all drv entries.
    USBH_USB2SER_ADAPTER_DRV_ENTRY      *p_cur_drv_entry = &p_adapter_drv_entry_tbl[cnt];
    USBH_USB2SER_ADAPTER_DRV_ENTRY_MGMT *p_cur_drv_entry_mgmt = &USBH_USB2SER_Ptr->AdapterDrvEntryMgmtTbl[cnt];
    USBH_USB2SER_DEV_ADAPTER_DRV        *p_cur_adapter_drv = p_cur_drv_entry->DrvPtr->DevAdapterDrvPtr;

    //                                                             Retrieve max adapter drv fnct/rx buf len.
    USBH_USB2SER_Ptr->MaxAdapterDrvFnctLen = DEF_MAX(USBH_USB2SER_Ptr->MaxAdapterDrvFnctLen,
                                                     p_cur_adapter_drv->FnctLen);

    USBH_USB2SER_Ptr->MaxAdapterCtrlBufLen = DEF_MAX(USBH_USB2SER_Ptr->MaxAdapterCtrlBufLen,
                                                     p_cur_adapter_drv->BufLenCtrl);

    USBH_USB2SER_Ptr->MaxAdapterStatusBufLen = DEF_MAX(USBH_USB2SER_Ptr->MaxAdapterStatusBufLen,
                                                       p_cur_adapter_drv->BufLenStatus);

    USBH_USB2SER_Ptr->MaxAdapterRxBufLen = DEF_MAX(USBH_USB2SER_Ptr->MaxAdapterRxBufLen,
                                                   p_cur_adapter_drv->BufLenRxFS);

    if (USBH_USB2SER_InitCfg.HS_En == DEF_ENABLED) {
      USBH_USB2SER_Ptr->MaxAdapterRxBufLen = DEF_MAX(USBH_USB2SER_Ptr->MaxAdapterRxBufLen,
                                                     p_cur_adapter_drv->BufLenRxHS);
    }

    p_cur_drv_entry_mgmt->DrvPtr = p_cur_drv_entry->DrvPtr;
    p_cur_drv_entry_mgmt->CustomIdTbl = p_cur_drv_entry->CustomIdTbl;
    p_cur_drv_entry_mgmt->CustomIdTblLen = p_cur_drv_entry->CustomIdTblLen;
    p_cur_drv_entry_mgmt->LockHandle = KAL_LockCreate("USBH - USB2SER adapter drv lock",
                                                      DEF_NULL,
                                                      p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    //                                                             Initialize adapter driver.
    p_cur_adapter_drv->Init(USBH_USB2SER_InitCfg.MemSegPtr,
                            p_cur_drv_entry->CustomIdTbl,
                            p_cur_drv_entry->CustomIdTblLen,
                            p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  class_fnct_qty = USBH_USB2SER_InitCfg.InitAlloc.FnctQty;
#elif (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  class_fnct_qty = USBH_USB2SER_InitCfg.OptimizeSpd.FnctQty;
#else
  class_fnct_qty = USBH_CLASS_FNCT_QTY_UNLIMITED;
#endif

  USBH_ClassFnctPoolCreate("USBH - USB2SER class Fnct",
                           &USBH_USB2SER_Ptr->FnctPool,
                           USBH_USB2SER_InitCfg.MemSegPtr,
                           sizeof(USBH_USB2SER_FNCT_DATA),
                           class_fnct_qty,
                           USBH_USB2SER_FnctAllocCallback,
                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBH_USB2SER_Ptr->TempAdapterFnctPtr = Mem_SegAlloc("USBH - USB2SER temp adapter fnct",
                                                      USBH_USB2SER_InitCfg.MemSegPtr,
                                                      (USBH_USB2SER_Ptr->MaxAdapterDrvFnctLen * sizeof(CPU_INT08U)),
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Allocate pool of asynchronous xfers.
  Mem_DynPoolCreate("USBH - USB2SER Tx async xfer pool",
                    &USBH_USB2SER_Ptr->TxAsyncXferPool,
                    USBH_USB2SER_InitCfg.MemSegPtr,
                    sizeof(USBH_USB2SER_ASYNC_TX_DATA),
                    sizeof(CPU_ALIGN),
#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
                    USBH_USB2SER_InitCfg.InitAlloc.TxAsyncXferQty,
                    USBH_USB2SER_InitCfg.InitAlloc.TxAsyncXferQty,
#else
                    0u,
                    LIB_MEM_BLK_QTY_UNLIMITED,
#endif
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBH_ClassDrvReg(&USBH_USB2SER_ClassDrv,
                   p_err);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_StdReqTimeoutSet()
 *
 * @brief    Assigns a new timeout delay for the USB2SER standard requests.
 *
 * @param    std_req_timeout_ms  New timeout, in milliseconds.
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error
 *                               code(s) from this function:
 *                                   - RTOS_ERR_NONE
 *******************************************************************************************************/
void USBH_USB2SER_StdReqTimeoutSet(CPU_INT32U std_req_timeout_ms,
                                   RTOS_ERR   *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  USBH_USB2SER_ReqTimeout = std_req_timeout_ms;
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_DevHandleGet()
 *
 * @brief    Retrieves the device handle associated to the USB-to-serial adapter.
 *
 * @param    usb2ser_fnct_handle     Handle to the USB-to-serial function.
 *
 * @param    p_err                   Pointer to the variable that will receive one of these return error codes
 *                                   from this function :
 *                                       - RTOS_ERR_NONE
 *                                       - RTOS_ERR_ABORT
 *                                       - RTOS_ERR_INVALID_HANDLE
 *                                       - RTOS_ERR_TIMEOUT
 *                                       - RTOS_ERR_OS_OBJ_DEL
 *                                       - RTOS_ERR_NOT_AVAIL
 *
 * @return   Device handle.
 *******************************************************************************************************/
USBH_DEV_HANDLE USBH_USB2SER_DevHandleGet(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                                          RTOS_ERR                 *p_err)
{
  USBH_DEV_HANDLE        dev_handle;
  USBH_USB2SER_FNCT_DATA *p_usb2ser_fnct_data;
  CPU_INT32U             std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, USBH_DEV_HANDLE_INVALID);

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                                                                        DEF_NO,
                                                                        std_req_timeout,
                                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBH_DEV_HANDLE_INVALID);
  }

  dev_handle = p_usb2ser_fnct_data->AdapterDrvData.DevHandle;

  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                        DEF_NO);

  return (dev_handle);
}

/****************************************************************************************************//**
 *                                           USBH_USB2SER_PortNbrGet()
 *
 * @brief    Retrieves the port number associated with the USB-to-serial function handle.
 *
 * @param    usb2ser_fnct_handle     Handle to the USB-to-serial function.
 *
 * @param    p_err                   Pointer to the variable that will receive one of these return error codes
 *                                   from this function :
 *                                       - RTOS_ERR_NONE
 *                                       - RTOS_ERR_ABORT
 *                                       - RTOS_ERR_INVALID_HANDLE
 *                                       - RTOS_ERR_WOULD_BLOCK
 *                                       - RTOS_ERR_TIMEOUT
 *                                       - RTOS_ERR_OS_SCHED_LOCKED
 *                                       - RTOS_ERR_OS_OBJ_DEL
 *                                       - RTOS_ERR_IS_OWNER
 *                                       - RTOS_ERR_WOULD_OVF
 *                                       - RTOS_ERR_NOT_AVAIL
 *
 * @return   Port number.
 *******************************************************************************************************/
CPU_INT08U USBH_USB2SER_PortNbrGet(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                                   RTOS_ERR                 *p_err)
{
  CPU_INT08U                   port_nbr = 0u;
  USBH_USB2SER_FNCT_DATA       *p_usb2ser_fnct_data;
  USBH_USB2SER_DEV_ADAPTER_DRV *p_dev_adapter_drv;
  RTOS_ERR                     release_err;
  CPU_INT32U                   std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                                                                        DEF_NO,
                                                                        std_req_timeout,
                                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  p_dev_adapter_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->DevAdapterDrvPtr;

  KAL_LockAcquire(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  KAL_OPT_PEND_NONE,
                  std_req_timeout,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  port_nbr = p_dev_adapter_drv->PortNbrGet(&p_usb2ser_fnct_data->AdapterDrvData,
                                           p_err);

  KAL_LockRelease(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  &release_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(release_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, 0u);

end_rel:
  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                        DEF_NO);

  return (port_nbr);
}

/****************************************************************************************************//**
 *                                           USBH_USB2SER_Reset()
 *
 * @brief    Purges the buffers on the device.
 *
 * @param    usb2ser_fnct_handle     Handle to USB-to-serial function.
 *
 * @param    sel                     Type of reset.
 *                                       - USBH_USB2SER_RESET_SEL_TX
 *                                       - USBH_USB2SER_RESET_SEL_RX
 *                                       - USBH_USB2SER_RESET_SEL_ALL
 *
 * @param    p_err                   Pointer to the variable that will receive one of these return error codes
 *                                   from this function :
 *                                       - RTOS_ERR_NONE
 *                                       - RTOS_ERR_ABORT
 *                                       - RTOS_ERR_INVALID_HANDLE
 *                                       - RTOS_ERR_WOULD_BLOCK
 *                                       - RTOS_ERR_TIMEOUT
 *                                       - RTOS_ERR_OS_SCHED_LOCKED
 *                                       - RTOS_ERR_OS_OBJ_DEL
 *                                       - RTOS_ERR_IS_OWNER
 *                                       - RTOS_ERR_WOULD_OVF
 *                                       - RTOS_ERR_NOT_AVAIL
 *******************************************************************************************************/
void USBH_USB2SER_Reset(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                        USBH_USB2SER_RESET_SEL   sel,
                        RTOS_ERR                 *p_err)
{
  USBH_USB2SER_FNCT_DATA       *p_usb2ser_fnct_data;
  USBH_USB2SER_DEV_ADAPTER_DRV *p_dev_adapter_drv;
  RTOS_ERR                     release_err;
  CPU_INT32U                   std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                                                                        DEF_NO,
                                                                        std_req_timeout,
                                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_dev_adapter_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->DevAdapterDrvPtr;

  KAL_LockAcquire(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  KAL_OPT_PEND_NONE,
                  std_req_timeout,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  p_dev_adapter_drv->Reset(&p_usb2ser_fnct_data->AdapterDrvData,
                           sel,
                           p_err);

  KAL_LockRelease(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  &release_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(release_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

end_rel:
  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_BaudRateSet()
 *
 * @brief    Sets the baud rate of the communication port.
 *
 * @param    usb2ser_fnct_handle     Handle to the USB-to-serial function.
 *
 * @param    baudrate                Baud rate to configure (in bauds/second).
 *
 * @param    p_err                   Pointer to the variable that will receive one of these return error codes
 *                                   from this function :
 *                                       - RTOS_ERR_NONE
 *                                       - RTOS_ERR_ABORT
 *                                       - RTOS_ERR_INVALID_HANDLE
 *                                       - RTOS_ERR_WOULD_BLOCK
 *                                       - RTOS_ERR_TIMEOUT
 *                                       - RTOS_ERR_OS_SCHED_LOCKED
 *                                       - RTOS_ERR_OS_OBJ_DEL
 *                                       - RTOS_ERR_IS_OWNER
 *                                       - RTOS_ERR_WOULD_OVF
 *                                       - RTOS_ERR_NOT_AVAIL
 *******************************************************************************************************/
void USBH_USB2SER_BaudRateSet(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                              CPU_INT32U               baudrate,
                              RTOS_ERR                 *p_err)
{
  CPU_INT32U                   baudrate_cur;
  CPU_INT32U                   std_req_timeout;
  USBH_USB2SER_FNCT_DATA       *p_usb2ser_fnct_data;
  USBH_USB2SER_DEV_ADAPTER_DRV *p_dev_adapter_drv;
  RTOS_ERR                     release_err;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                                                                        DEF_NO,
                                                                        std_req_timeout,
                                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_dev_adapter_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->DevAdapterDrvPtr;

  KAL_LockAcquire(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  KAL_OPT_PEND_NONE,
                  std_req_timeout,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  //                                                               Ensure baudrate different than what about to set.
  baudrate_cur = p_dev_adapter_drv->BaudRateGet(&p_usb2ser_fnct_data->AdapterDrvData,
                                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_unlock_rel;
  }

  if (baudrate_cur == baudrate) {
    goto end_unlock_rel;
  }

  p_dev_adapter_drv->BaudRateSet(&p_usb2ser_fnct_data->AdapterDrvData,
                                 baudrate,
                                 p_err);

end_unlock_rel:
  KAL_LockRelease(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  &release_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(release_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

end_rel:
  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_BaudRateGet()
 *
 * @brief    Gets the current baud rate of the communication port.
 *
 * @param    usb2ser_fnct_handle     Handle to the USB-to-serial function.
 *
 * @param    p_err                   Pointer to the variable that will receive one of these return error codes
 *                                   from this function :
 *                                       - RTOS_ERR_NONE
 *                                       - RTOS_ERR_ABORT
 *                                       - RTOS_ERR_INVALID_HANDLE
 *                                       - RTOS_ERR_WOULD_BLOCK
 *                                       - RTOS_ERR_TIMEOUT
 *                                       - RTOS_ERR_OS_SCHED_LOCKED
 *                                       - RTOS_ERR_OS_OBJ_DEL
 *                                       - RTOS_ERR_IS_OWNER
 *                                       - RTOS_ERR_WOULD_OVF
 *                                       - RTOS_ERR_NOT_AVAIL
 *
 * @return   Current baud rate in bauds/sec.
 *******************************************************************************************************/
CPU_INT32U USBH_USB2SER_BaudRateGet(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                                    RTOS_ERR                 *p_err)
{
  CPU_INT32U                   baudrate_cur = 0u;
  CPU_INT32U                   std_req_timeout;
  USBH_USB2SER_FNCT_DATA       *p_usb2ser_fnct_data;
  USBH_USB2SER_DEV_ADAPTER_DRV *p_dev_adapter_drv;
  RTOS_ERR                     release_err;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                                                                        DEF_NO,
                                                                        std_req_timeout,
                                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  p_dev_adapter_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->DevAdapterDrvPtr;

  KAL_LockAcquire(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  KAL_OPT_PEND_NONE,
                  std_req_timeout,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  baudrate_cur = p_dev_adapter_drv->BaudRateGet(&p_usb2ser_fnct_data->AdapterDrvData,
                                                p_err);

  KAL_LockRelease(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  &release_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(release_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, 0u);

end_rel:
  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                        DEF_NO);

  return (baudrate_cur);
}

/****************************************************************************************************//**
 *                                           USBH_USB2SER_DataSet()
 *
 * @brief    Sets the data characteristics of the communication port.
 *
 * @param    usb2ser_fnct_handle     Handle to the USB-to-serial function.
 *
 * @param    data_size               Number of data bits.
 *
 * @param    parity                  Define the Parity to use, as follows:
 *                                       - USBH_USB2SER_PARITY_NONE,     Do not use the parity bit.
 *                                       - USBH_USB2SER_PARITY_ODD,      Use odd        parity bit.
 *                                       - USBH_USB2SER_PARITY_EVEN,     Use even       parity bit.
 *                                       - USBH_USB2SER_PARITY_MARK,     Use mark       parity bit.
 *                                       - USBH_USB2SER_PARITY_SPACE     Use space      parity bit.
 *
 * @param    stop_bits               Define the Number of stop bits, as follows:
 *                                       - USBH_USB2SER_STOP_BITS_1,     Use 1   stop bit.
 *                                       - USBH_USB2SER_STOP_BITS_1_5,   Use 1.5 stop bit.
 *                                       - USBH_USB2SER_STOP_BITS_2      Use 2   stop bits.
 *
 * @param    p_err                   Pointer to the variable that will receive one of these return error codes
 *                                   from this function :
 *                                       - RTOS_ERR_NONE
 *                                       - RTOS_ERR_ABORT
 *                                       - RTOS_ERR_INVALID_HANDLE
 *                                       - RTOS_ERR_WOULD_BLOCK
 *                                       - RTOS_ERR_TIMEOUT
 *                                       - RTOS_ERR_OS_SCHED_LOCKED
 *                                       - RTOS_ERR_OS_OBJ_DEL
 *                                       - RTOS_ERR_IS_OWNER
 *                                       - RTOS_ERR_WOULD_OVF
 *                                       - RTOS_ERR_NOT_AVAIL
 *******************************************************************************************************/
void USBH_USB2SER_DataSet(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                          CPU_INT08U               data_size,
                          USBH_USB2SER_PARITY      parity,
                          USBH_USB2SER_STOP_BITS   stop_bits,
                          RTOS_ERR                 *p_err)
{
  CPU_INT08U                   data_size_cur;
  USBH_USB2SER_FNCT_DATA       *p_usb2ser_fnct_data;
  USBH_USB2SER_PARITY          parity_cur;
  USBH_USB2SER_STOP_BITS       stop_bits_cur;
  USBH_USB2SER_DEV_ADAPTER_DRV *p_dev_adapter_drv;
  RTOS_ERR                     release_err;
  CPU_INT32U                   std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                                                                        DEF_NO,
                                                                        std_req_timeout,
                                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_dev_adapter_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->DevAdapterDrvPtr;

  KAL_LockAcquire(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  KAL_OPT_PEND_NONE,
                  std_req_timeout,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  //                                                               Ensure data different than what about to set.
  p_dev_adapter_drv->DataGet(&p_usb2ser_fnct_data->AdapterDrvData,
                             &data_size_cur,
                             &parity_cur,
                             &stop_bits_cur,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_unlock_rel;
  }

  if ((data_size_cur == data_size)
      && (parity_cur == parity)
      && (stop_bits_cur == stop_bits)) {
    goto end_unlock_rel;
  }

  p_dev_adapter_drv->DataSet(&p_usb2ser_fnct_data->AdapterDrvData,
                             data_size,
                             parity,
                             stop_bits,
                             p_err);

end_unlock_rel:
  KAL_LockRelease(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  &release_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(release_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

end_rel:
  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                           USBH_USB2SER_DataGet()
 *
 * @brief    Gets the data characteristics of the communication port.
 *
 * @param    usb2ser_fnct_handle     Handle to the USB-to-serial function.
 *
 * @param    p_data_size             Pointer to the variable that will receive the number of data bits.
 *
 * @param    p_parity                Pointer to the variable that will receive the parity check used, as
 *                                   follows:
 *                                       - USBH_USB2SER_PARITY_NONE,     Do not use the parity bit.
 *                                       - USBH_USB2SER_PARITY_ODD,      Use odd        parity bit.
 *                                       - USBH_USB2SER_PARITY_EVEN,     Use even       parity bit.
 *                                       - USBH_USB2SER_PARITY_MARK,     Use mark       parity bit.
 *                                       - USBH_USB2SER_PARITY_SPACE     Use space      parity bit.
 *
 * @param    p_stop_bits             Pointer to the variable that will receive the number of stop bits, as
 *                                   follows:
 *                                       - USBH_USB2SER_STOP_BITS_1,     Use 1   stop bit.
 *                                       - USBH_USB2SER_STOP_BITS_1_5,   Use 1.5 stop bit.
 *                                       - USBH_USB2SER_STOP_BITS_2      Use 2   stop bits.
 *
 * @param    p_err                   Pointer to the variable that will receive one of these return error codes
 *                                   from this function :
 *                                       - RTOS_ERR_NONE
 *                                       - RTOS_ERR_ABORT
 *                                       - RTOS_ERR_INVALID_HANDLE
 *                                       - RTOS_ERR_WOULD_BLOCK
 *                                       - RTOS_ERR_TIMEOUT
 *                                       - RTOS_ERR_OS_SCHED_LOCKED
 *                                       - RTOS_ERR_OS_OBJ_DEL
 *                                       - RTOS_ERR_IS_OWNER
 *                                       - RTOS_ERR_WOULD_OVF
 *                                       - RTOS_ERR_NOT_AVAIL
 *******************************************************************************************************/
void USBH_USB2SER_DataGet(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                          CPU_INT08U               *p_data_size,
                          USBH_USB2SER_PARITY      *p_parity,
                          USBH_USB2SER_STOP_BITS   *p_stop_bits,
                          RTOS_ERR                 *p_err)
{
  USBH_USB2SER_FNCT_DATA       *p_usb2ser_fnct_data;
  USBH_USB2SER_DEV_ADAPTER_DRV *p_dev_adapter_drv;
  RTOS_ERR                     release_err;
  CPU_INT32U                   std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                                                                        DEF_NO,
                                                                        std_req_timeout,
                                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_dev_adapter_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->DevAdapterDrvPtr;

  KAL_LockAcquire(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  KAL_OPT_PEND_NONE,
                  std_req_timeout,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  p_dev_adapter_drv->DataGet(&p_usb2ser_fnct_data->AdapterDrvData,
                             p_data_size,
                             p_parity,
                             p_stop_bits,
                             p_err);

  KAL_LockRelease(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  &release_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(release_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

end_rel:
  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_BreakSignalSet()
 *
 * @brief    Sets the break signal.
 *
 * @param    usb2ser_fnct_handle     Handle to the USB-to-serial function.
 *
 * @param    set                     Boolean that indicates if the break signal should be set or cleared.
 *
 * @param    p_err                   Pointer to the variable that will receive one of these return error codes
 *                                   from this function :
 *                                       - RTOS_ERR_NONE
 *                                       - RTOS_ERR_ABORT
 *                                       - RTOS_ERR_INVALID_HANDLE
 *                                       - RTOS_ERR_WOULD_BLOCK
 *                                       - RTOS_ERR_TIMEOUT
 *                                       - RTOS_ERR_OS_SCHED_LOCKED
 *                                       - RTOS_ERR_OS_OBJ_DEL
 *                                       - RTOS_ERR_IS_OWNER
 *                                       - RTOS_ERR_WOULD_OVF
 *                                       - RTOS_ERR_NOT_AVAIL
 *******************************************************************************************************/
void USBH_USB2SER_BreakSignalSet(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                                 CPU_BOOLEAN              set,
                                 RTOS_ERR                 *p_err)
{
  USBH_USB2SER_FNCT_DATA       *p_usb2ser_fnct_data;
  USBH_USB2SER_DEV_ADAPTER_DRV *p_dev_adapter_drv;
  RTOS_ERR                     release_err;
  CPU_INT32U                   std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                                                                        DEF_NO,
                                                                        std_req_timeout,
                                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_dev_adapter_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->DevAdapterDrvPtr;

  KAL_LockAcquire(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  KAL_OPT_PEND_NONE,
                  std_req_timeout,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  p_dev_adapter_drv->BrkSignalSet(&p_usb2ser_fnct_data->AdapterDrvData,
                                  set,
                                  p_err);

  KAL_LockRelease(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  &release_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(release_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

end_rel:
  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_ModemDTR_Set()
 *
 * @brief    Controls the modem DTR pin on a port.
 *
 * @param    usb2ser_fnct_handle     Handle to USB-to-serial function.
 *
 * @param    set                     Sets or clears the Data Terminal Ready (DTR) pin.
 *
 * @param    p_err                   Pointer to the variable that will receive one of these return error codes
 *                                   from this function :
 *                                       - RTOS_ERR_NONE
 *                                       - RTOS_ERR_ABORT
 *                                       - RTOS_ERR_INVALID_HANDLE
 *                                       - RTOS_ERR_WOULD_BLOCK
 *                                       - RTOS_ERR_TIMEOUT
 *                                       - RTOS_ERR_OS_SCHED_LOCKED
 *                                       - RTOS_ERR_OS_OBJ_DEL
 *                                       - RTOS_ERR_IS_OWNER
 *                                       - RTOS_ERR_WOULD_OVF
 *                                       - RTOS_ERR_NOT_AVAIL
 *                                       - RTOS_ERR_USB2SER_FLOW_CTRL_EN
 *
 * @note     (1) If the current flow control is set to DTR/DSR, the DTR pin cannot be set manually.
 *******************************************************************************************************/
void USBH_USB2SER_ModemDTR_Set(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                               CPU_BOOLEAN              set,
                               RTOS_ERR                 *p_err)
{
  USBH_USB2SER_FNCT_DATA             *p_usb2ser_fnct_data;
  USBH_USB2SER_DEV_ADAPTER_DRV       *p_dev_adapter_drv;
  USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL flow_ctrl_protocol_cur;
  RTOS_ERR                           release_err;
  CPU_INT32U                         std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                                                                        DEF_NO,
                                                                        std_req_timeout,
                                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_dev_adapter_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->DevAdapterDrvPtr;

  KAL_LockAcquire(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  KAL_OPT_PEND_NONE,
                  std_req_timeout,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  //                                                               Ensure DTR/DSR flow ctrl not set.
  flow_ctrl_protocol_cur = p_dev_adapter_drv->HW_FlowCtrlGet(&p_usb2ser_fnct_data->AdapterDrvData,
                                                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_unlock_rel;
  }

  if (flow_ctrl_protocol_cur == USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_DTR_DSR) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_USB2SER_FLOW_CTRL_EN);
    goto end_unlock_rel;
  }

  USBH_USB2SER_ModemCtrlApply(p_usb2ser_fnct_data,
                              DEF_ENABLED,
                              set,
                              0xFFu,
                              0xFFu,
                              p_err);

end_unlock_rel:
  KAL_LockRelease(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  &release_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(release_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

end_rel:
  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_ModemDTR_Get()
 *
 * @brief    Gets the DTR pin state on a port.
 *
 * @param    usb2ser_fnct_handle     Handle to the USB-to-serial function.
 *
 * @param    p_err                   Pointer to the variable that will receive one of these return error codes
 *                                   from this function :
 *                                       - RTOS_ERR_NONE
 *                                       - RTOS_ERR_ABORT
 *                                       - RTOS_ERR_INVALID_HANDLE
 *                                       - RTOS_ERR_WOULD_BLOCK
 *                                       - RTOS_ERR_TIMEOUT
 *                                       - RTOS_ERR_OS_SCHED_LOCKED
 *                                       - RTOS_ERR_OS_OBJ_DEL
 *                                       - RTOS_ERR_IS_OWNER
 *                                       - RTOS_ERR_WOULD_OVF
 *                                       - RTOS_ERR_NOT_AVAIL
 *                                       - RTOS_ERR_USB2SER_FLOW_CTRL_EN
 *
 * @return   DTR pin state.
 *******************************************************************************************************/
CPU_BOOLEAN USBH_USB2SER_ModemDTR_Get(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                                      RTOS_ERR                 *p_err)
{
  CPU_BOOLEAN                        dtr_set_cur = DEF_CLR;
  USBH_USB2SER_FNCT_DATA             *p_usb2ser_fnct_data;
  USBH_USB2SER_DEV_ADAPTER_DRV       *p_dev_adapter_drv;
  USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL flow_ctrl_protocol_cur;
  RTOS_ERR                           release_err;
  CPU_INT32U                         std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_CLR);

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                                                                        DEF_NO,
                                                                        std_req_timeout,
                                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_CLR);
  }

  p_dev_adapter_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->DevAdapterDrvPtr;

  KAL_LockAcquire(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  KAL_OPT_PEND_NONE,
                  std_req_timeout,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  //                                                               Ensure DTR/DSR flow ctrl not set.
  flow_ctrl_protocol_cur = p_dev_adapter_drv->HW_FlowCtrlGet(&p_usb2ser_fnct_data->AdapterDrvData,
                                                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_unlock_rel;
  }

  if (flow_ctrl_protocol_cur == USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_DTR_DSR) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_USB2SER_FLOW_CTRL_EN);
    goto end_unlock_rel;
  }

  p_dev_adapter_drv->ModemCtrlGet(&p_usb2ser_fnct_data->AdapterDrvData,
                                  DEF_NULL,
                                  &dtr_set_cur,
                                  DEF_NULL,
                                  DEF_NULL,
                                  p_err);
end_unlock_rel:
  KAL_LockRelease(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  &release_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(release_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, 0u);

end_rel:
  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                        DEF_NO);

  return (dtr_set_cur);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_ModemRTS_Set()
 *
 * @brief    Controls the modem RTS pin on a port.
 *
 * @param    usb2ser_fnct_handle     Handle to the USB-to-serial function.
 *
 * @param    set                     Sets or clears the Ready To Send (RTS) pin.
 *
 * @param    p_err                   Pointer to the variable that will receive one of these return error codes
 *                                   from this function :
 *                                       - RTOS_ERR_NONE
 *                                       - RTOS_ERR_ABORT
 *                                       - RTOS_ERR_INVALID_HANDLE
 *                                       - RTOS_ERR_WOULD_BLOCK
 *                                       - RTOS_ERR_TIMEOUT
 *                                       - RTOS_ERR_OS_SCHED_LOCKED
 *                                       - RTOS_ERR_OS_OBJ_DEL
 *                                       - RTOS_ERR_IS_OWNER
 *                                       - RTOS_ERR_WOULD_OVF
 *                                       - RTOS_ERR_NOT_AVAIL
 *                                       - RTOS_ERR_USB2SER_FLOW_CTRL_EN
 *
 * @note     (1) If the current flow control is set to RTS/CTS, the RTS pin cannot be set.
 *******************************************************************************************************/
void USBH_USB2SER_ModemRTS_Set(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                               CPU_BOOLEAN              set,
                               RTOS_ERR                 *p_err)
{
  USBH_USB2SER_FNCT_DATA             *p_usb2ser_fnct_data;
  USBH_USB2SER_DEV_ADAPTER_DRV       *p_dev_adapter_drv;
  USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL flow_ctrl_protocol_cur;
  RTOS_ERR                           release_err;
  CPU_INT32U                         std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                                                                        DEF_NO,
                                                                        std_req_timeout,
                                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_dev_adapter_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->DevAdapterDrvPtr;

  KAL_LockAcquire(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  KAL_OPT_PEND_NONE,
                  std_req_timeout,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }
  //                                                               Ensure RTS/CTS flow ctrl not set.
  flow_ctrl_protocol_cur = p_dev_adapter_drv->HW_FlowCtrlGet(&p_usb2ser_fnct_data->AdapterDrvData,
                                                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_unlock_rel;
  }

  if (flow_ctrl_protocol_cur == USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_RTS_CTS) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_USB2SER_FLOW_CTRL_EN);
    goto end_unlock_rel;
  }

  USBH_USB2SER_ModemCtrlApply(p_usb2ser_fnct_data,
                              0xFFu,
                              0xFFu,
                              DEF_ENABLED,
                              set,
                              p_err);

end_unlock_rel:
  KAL_LockRelease(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  &release_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(release_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

end_rel:
  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_ModemRTS_Get()
 *
 * @brief    Gets the RTS pin state on a port.
 *
 * @param    usb2ser_fnct_handle     Handle to USB-to-serial function.
 *
 * @param    p_err                   Pointer to the variable that will receive the return error code from this function :
 *                                       - RTOS_ERR_NONE
 *                                       - RTOS_ERR_ABORT
 *                                       - RTOS_ERR_INVALID_HANDLE
 *                                       - RTOS_ERR_WOULD_BLOCK
 *                                       - RTOS_ERR_TIMEOUT
 *                                       - RTOS_ERR_OS_SCHED_LOCKED
 *                                       - RTOS_ERR_OS_OBJ_DEL
 *                                       - RTOS_ERR_IS_OWNER
 *                                       - RTOS_ERR_WOULD_OVF
 *                                       - RTOS_ERR_NOT_AVAIL
 *                                       - RTOS_ERR_USB2SER_FLOW_CTRL_EN
 *
 * @return   RTS pin status.
 *******************************************************************************************************/
CPU_BOOLEAN USBH_USB2SER_ModemRTS_Get(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                                      RTOS_ERR                 *p_err)
{
  CPU_BOOLEAN                        rts_set_cur = DEF_CLR;
  USBH_USB2SER_FNCT_DATA             *p_usb2ser_fnct_data;
  USBH_USB2SER_DEV_ADAPTER_DRV       *p_dev_adapter_drv;
  USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL flow_ctrl_protocol_cur;
  RTOS_ERR                           release_err;
  CPU_INT32U                         std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_CLR);

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                                                                        DEF_NO,
                                                                        std_req_timeout,
                                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_CLR);
  }

  p_dev_adapter_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->DevAdapterDrvPtr;

  KAL_LockAcquire(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  KAL_OPT_PEND_NONE,
                  std_req_timeout,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  //                                                               Ensure RTS/CTS flow ctrl not set.
  flow_ctrl_protocol_cur = p_dev_adapter_drv->HW_FlowCtrlGet(&p_usb2ser_fnct_data->AdapterDrvData,
                                                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_unlock_rel;
  }

  if (flow_ctrl_protocol_cur == USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_RTS_CTS) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_USB2SER_FLOW_CTRL_EN);
    goto end_unlock_rel;
  }

  p_dev_adapter_drv->ModemCtrlGet(&p_usb2ser_fnct_data->AdapterDrvData,
                                  DEF_NULL,
                                  DEF_NULL,
                                  DEF_NULL,
                                  &rts_set_cur,
                                  p_err);

end_unlock_rel:
  KAL_LockRelease(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  &release_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(release_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, 0u);

end_rel:
  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                        DEF_NO);

  return (rts_set_cur);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_HW_FlowCtrlSet()
 *
 * @brief    Sets the hardware flow control protocol on the serial port.
 *
 * @param    usb2ser_fnct_handle     Handle to the USB-to-serial function.
 *
 * @param    protocol                Defines which hardware flow control protocol to use, as follows:
 *                                       - USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_RTS_CTS,   Use RTS/CTS protocol.
 *                                       - USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_DTR_DSR,   Use DTR/DSR protocol.
 *                                       - USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_NONE       No HW protocol.
 *
 * @param    p_err                   Pointer to the variable that will receive one of these return error codes
 *                                   from this function :
 *                                       - RTOS_ERR_NONE
 *                                       - RTOS_ERR_ABORT
 *                                       - RTOS_ERR_INVALID_HANDLE
 *                                       - RTOS_ERR_WOULD_BLOCK
 *                                       - RTOS_ERR_TIMEOUT
 *                                       - RTOS_ERR_OS_SCHED_LOCKED
 *                                       - RTOS_ERR_OS_OBJ_DEL
 *                                       - RTOS_ERR_IS_OWNER
 *                                       - RTOS_ERR_WOULD_OVF
 *                                       - RTOS_ERR_NOT_AVAIL
 *******************************************************************************************************/
void USBH_USB2SER_HW_FlowCtrlSet(USBH_USB2SER_FNCT_HANDLE           usb2ser_fnct_handle,
                                 USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL protocol,
                                 RTOS_ERR                           *p_err)
{
  USBH_USB2SER_FNCT_DATA             *p_usb2ser_fnct_data;
  USBH_USB2SER_DEV_ADAPTER_DRV       *p_dev_adapter_drv;
  USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL protocol_cur;
  RTOS_ERR                           release_err;
  CPU_INT32U                         std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                                                                        DEF_NO,
                                                                        std_req_timeout,
                                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_dev_adapter_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->DevAdapterDrvPtr;

  KAL_LockAcquire(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  KAL_OPT_PEND_NONE,
                  std_req_timeout,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  protocol_cur = p_dev_adapter_drv->HW_FlowCtrlGet(&p_usb2ser_fnct_data->AdapterDrvData,
                                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_unlock_rel;
  }

  if (protocol_cur == protocol) {
    goto end_unlock_rel;
  }

  USBH_USB2SER_ModemCtrlApply(p_usb2ser_fnct_data,              // Ensure modem ctrl disabled.
                              (protocol == USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_DTR_DSR) ? DEF_DISABLED : 0xFFu,
                              0xFFu,
                              (protocol == USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_RTS_CTS) ? DEF_DISABLED : 0xFFu,
                              0xFFu,
                              p_err);

  p_dev_adapter_drv->HW_FlowCtrlSet(&p_usb2ser_fnct_data->AdapterDrvData,
                                    protocol,
                                    p_err);

end_unlock_rel:
  KAL_LockRelease(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  &release_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(release_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

end_rel:
  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_HW_FlowCtrlGet()
 *
 * @brief    Gets the hardware flow control protocol on the serial port.
 *
 * @param    usb2ser_fnct_handle     Handle to the USB-to-serial function.
 *
 * @param    p_err                   Pointer to the variable that will receive one of these return error codes
 *                                   from this function :
 *                                       - RTOS_ERR_NONE
 *                                       - RTOS_ERR_ABORT
 *                                       - RTOS_ERR_INVALID_HANDLE
 *                                       - RTOS_ERR_WOULD_BLOCK
 *                                       - RTOS_ERR_TIMEOUT
 *                                       - RTOS_ERR_OS_SCHED_LOCKED
 *                                       - RTOS_ERR_OS_OBJ_DEL
 *                                       - RTOS_ERR_IS_OWNER
 *                                       - RTOS_ERR_WOULD_OVF
 *                                       - RTOS_ERR_NOT_AVAIL
 *
 * @return   Current hardware flow control protocol.
 *******************************************************************************************************/
USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL USBH_USB2SER_HW_FlowCtrlGet(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                                                               RTOS_ERR                 *p_err)
{
  USBH_USB2SER_FNCT_DATA             *p_usb2ser_fnct_data;
  USBH_USB2SER_DEV_ADAPTER_DRV       *p_dev_adapter_drv;
  USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL protocol_cur = USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_NONE;
  RTOS_ERR                           release_err;
  CPU_INT32U                         std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, protocol_cur);

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                                                                        DEF_NO,
                                                                        std_req_timeout,
                                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (protocol_cur);
  }

  p_dev_adapter_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->DevAdapterDrvPtr;

  KAL_LockAcquire(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  KAL_OPT_PEND_NONE,
                  std_req_timeout,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  protocol_cur = p_dev_adapter_drv->HW_FlowCtrlGet(&p_usb2ser_fnct_data->AdapterDrvData,
                                                   p_err);

  KAL_LockRelease(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  &release_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(release_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_NONE);

end_rel:
  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                        DEF_NO);

  return (protocol_cur);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_SW_FlowCtrlSet()
 *
 * @brief    Sets the software flow control on the serial port.
 *
 * @param    usb2ser_fnct_handle     Handle to the USB-to-serial function.
 *
 * @param    en                      Boolean that indicates if the software flow control should be enabled.
 *
 * @param    xon_char                Xon  character to use.
 *
 * @param    xoff_char               Xoff character to use.
 *
 * @param    p_err                   Pointer to the variable that will receive the return error code from this function :
 *                                       - RTOS_ERR_NONE
 *                                       - RTOS_ERR_ABORT
 *                                       - RTOS_ERR_INVALID_HANDLE
 *                                       - RTOS_ERR_WOULD_BLOCK
 *                                       - RTOS_ERR_TIMEOUT
 *                                       - RTOS_ERR_OS_SCHED_LOCKED
 *                                       - RTOS_ERR_OS_OBJ_DEL
 *                                       - RTOS_ERR_IS_OWNER
 *                                       - RTOS_ERR_WOULD_OVF
 *                                       - RTOS_ERR_NOT_AVAIL
 *******************************************************************************************************/
void USBH_USB2SER_SW_FlowCtrlSet(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                                 CPU_BOOLEAN              en,
                                 CPU_INT08U               xon_char,
                                 CPU_INT08U               xoff_char,
                                 RTOS_ERR                 *p_err)
{
  CPU_INT08U                   xon_char_cur;
  CPU_INT08U                   xoff_char_cur;
  CPU_BOOLEAN                  sw_flow_ctrl_en_cur;
  USBH_USB2SER_FNCT_DATA       *p_usb2ser_fnct_data;
  USBH_USB2SER_DEV_ADAPTER_DRV *p_dev_adapter_drv;
  RTOS_ERR                     release_err;
  CPU_INT32U                   std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET(((en == DEF_DISABLED)
                           || (xon_char != xoff_char)), *p_err, RTOS_ERR_INVALID_ARG,; );

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                                                                        DEF_NO,
                                                                        std_req_timeout,
                                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_dev_adapter_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->DevAdapterDrvPtr;

  KAL_LockAcquire(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  KAL_OPT_PEND_NONE,
                  std_req_timeout,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  sw_flow_ctrl_en_cur = p_dev_adapter_drv->SW_FlowCtrlGet(&p_usb2ser_fnct_data->AdapterDrvData,
                                                          &xon_char_cur,
                                                          &xoff_char_cur,
                                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_unlock_rel;
  }

  if ((en == sw_flow_ctrl_en_cur)
      && (xon_char == xon_char_cur)
      && (xoff_char == xoff_char_cur)) {
    goto end_unlock_rel;
  }

  p_dev_adapter_drv->SW_FlowCtrlSet(&p_usb2ser_fnct_data->AdapterDrvData,
                                    en,
                                    xon_char,
                                    xoff_char,
                                    p_err);

end_unlock_rel:
  KAL_LockRelease(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  &release_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(release_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

end_rel:
  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_SW_FlowCtrlGet()
 *
 * @brief    Gets the software flow control state on the serial port.
 *
 * @param    usb2ser_fnct_handle     Handle to the USB-to-serial function.
 *
 * @param    p_xon_char              Pointer to the variable that will receive the Xon  character.
 *
 * @param    p_xoff_char             Pointer to the variable that will receive the Xoff character.
 *
 * @param    p_err                   Pointer to the variable that will receive one of these return error codes
 *                                   from this function :
 *                                       - RTOS_ERR_NONE
 *                                       - RTOS_ERR_ABORT
 *                                       - RTOS_ERR_INVALID_HANDLE
 *                                       - RTOS_ERR_WOULD_BLOCK
 *                                       - RTOS_ERR_TIMEOUT
 *                                       - RTOS_ERR_OS_SCHED_LOCKED
 *                                       - RTOS_ERR_OS_OBJ_DEL
 *                                       - RTOS_ERR_IS_OWNER
 *                                       - RTOS_ERR_WOULD_OVF
 *                                       - RTOS_ERR_NOT_AVAIL
 *
 * @return   State of the software flow control.
 *******************************************************************************************************/
CPU_BOOLEAN USBH_USB2SER_SW_FlowCtrlGet(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                                        CPU_INT08U               *p_xon_char,
                                        CPU_INT08U               *p_xoff_char,
                                        RTOS_ERR                 *p_err)
{
  CPU_BOOLEAN                  sw_flow_ctrl_en_cur = DEF_DISABLED;
  USBH_USB2SER_FNCT_DATA       *p_usb2ser_fnct_data;
  USBH_USB2SER_DEV_ADAPTER_DRV *p_dev_adapter_drv;
  RTOS_ERR                     release_err;
  CPU_INT32U                   std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, sw_flow_ctrl_en_cur);

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                                                                        DEF_NO,
                                                                        std_req_timeout,
                                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (sw_flow_ctrl_en_cur);
  }

  p_dev_adapter_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->DevAdapterDrvPtr;

  KAL_LockAcquire(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  KAL_OPT_PEND_NONE,
                  std_req_timeout,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  sw_flow_ctrl_en_cur = p_dev_adapter_drv->SW_FlowCtrlGet(&p_usb2ser_fnct_data->AdapterDrvData,
                                                          p_xon_char,
                                                          p_xoff_char,
                                                          p_err);

  KAL_LockRelease(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  &release_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(release_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, 0u);

end_rel:
  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                        DEF_NO);

  return (sw_flow_ctrl_en_cur);
}

/****************************************************************************************************//**
 *                                           USBH_USB2SER_StatusGet()
 *
 * @brief    Retrieves the current serial status.
 *
 * @param    usb2ser_fnct_handle     Handle to the USB-to-serial function.
 *
 * @param    p_err                   Pointer to the variable that will receive one of these return error codes
 *                                   from this function :
 *                                       - RTOS_ERR_NONE
 *                                       - RTOS_ERR_ABORT
 *                                       - RTOS_ERR_INVALID_HANDLE
 *                                       - RTOS_ERR_TIMEOUT
 *                                       - RTOS_ERR_OS_OBJ_DEL
 *                                       - RTOS_ERR_NOT_AVAIL
 *
 * @return   Current serial status.
 *
 * @note     (1) The following masks determine if part of the modem status is set, as follows:
 *               - USBH_USB2SER_MODEM_STATUS_CTS
 *               - USBH_USB2SER_MODEM_STATUS_DSR
 *               - USBH_USB2SER_MODEM_STATUS_RING
 *               - USBH_USB2SER_MODEM_STATUS_CARRIER
 *
 * @note     (2) The following masks may be used to determine if part of the line status is set, as
 *           follows:
 *               - USBH_USB2SER_LINE_STATUS_RX_OVERFLOW_ERR
 *               - USBH_USB2SER_LINE_STATUS_PARITY_ERR
 *               - USBH_USB2SER_LINE_STATUS_FRAMING_ERR
 *               - USBH_USB2SER_LINE_STATUS_BRK_INT
 *******************************************************************************************************/
USBH_USB2SER_SERIAL_STATUS USBH_USB2SER_StatusGet(USBH_USB2SER_FNCT_HANDLE usb2ser_fnct_handle,
                                                  RTOS_ERR                 *p_err)
{
  USBH_USB2SER_FNCT_DATA     *p_usb2ser_fnct_data;
  USBH_USB2SER_SERIAL_STATUS status = { DEF_BIT_NONE, DEF_BIT_NONE };
  CPU_INT32U                 std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, status);

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                                                                        DEF_NO,
                                                                        std_req_timeout,
                                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (status);
  }

  status = p_usb2ser_fnct_data->StatusCur;

  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                        DEF_NO);

  return (status);
}

/****************************************************************************************************//**
 *                                           USBH_USB2SER_TxAsync()
 *
 * @brief    Sends the data on the serial port. This function is non-blocking.
 *
 * @param    usb2ser_fnct_handle     Handle to the USB-to-serial function.
 *
 * @param    p_buf                   Pointer to the buffer of data that will be sent.
 *
 * @param    buf_len                 Buffer length in bytes.
 *
 * @param    tx_cmpl_notify          Function that will be invoked upon completion of transmit operation.
 *
 * @param    p_arg                   Pointer to the argument that will be passed as parameter of 'tx_cmpl_notify'.
 *
 * @param    p_err                   Pointer to the variable that will receive one of these return error codes
 *                                   from this function :
 *                                       - RTOS_ERR_NONE
 *                                       - RTOS_ERR_ABORT
 *                                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                       - RTOS_ERR_POOL_EMPTY
 *                                       - RTOS_ERR_WOULD_BLOCK
 *                                       - RTOS_ERR_OS_SCHED_LOCKED
 *                                       - RTOS_ERR_IS_OWNER
 *                                       - RTOS_ERR_INVALID_HANDLE
 *                                       - RTOS_ERR_SEG_OVF
 *                                       - RTOS_ERR_TIMEOUT
 *                                       - RTOS_ERR_OS_OBJ_DEL
 *                                       - RTOS_ERR_NOT_AVAIL
 *                                       - RTOS_ERR_WOULD_OVF
 *******************************************************************************************************/
void USBH_USB2SER_TxAsync(USBH_USB2SER_FNCT_HANDLE   usb2ser_fnct_handle,
                          CPU_INT08U                 *p_buf,
                          CPU_INT32U                 buf_len,
                          USBH_USB2SER_ASYNC_TX_FNCT tx_cmpl_notify,
                          void                       *p_arg,
                          RTOS_ERR                   *p_err)
{
  USBH_USB2SER_FNCT_DATA       *p_usb2ser_fnct_data;
  USBH_USB2SER_ASYNC_TX_DATA   *p_tx_data;
  USBH_USB2SER_DEV_ADAPTER_DRV *p_dev_adapter_drv;
  RTOS_ERR                     release_err;
  CPU_INT32U                   std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG(tx_cmpl_notify != DEF_NULL, RTOS_ERR_INVALID_ARG,; );

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                                                                        DEF_YES,
                                                                        std_req_timeout,
                                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_tx_data = (USBH_USB2SER_ASYNC_TX_DATA *)Mem_DynPoolBlkGet(&USBH_USB2SER_Ptr->TxAsyncXferPool,
                                                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Failed to allocated async transfer block."));
    goto end_rel;
  }

  p_tx_data->AdapterDataPtr = &p_usb2ser_fnct_data->AdapterDrvData;
  p_tx_data->TxCmplFnct = tx_cmpl_notify;
  p_tx_data->TxCmplArgPtr = p_arg;

  p_dev_adapter_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->DevAdapterDrvPtr;

  KAL_LockAcquire(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  KAL_OPT_PEND_NONE,
                  std_req_timeout,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  p_dev_adapter_drv->TxAsync(&p_usb2ser_fnct_data->AdapterDrvData,
                             p_buf,
                             buf_len,
                             (void *)p_tx_data,
                             p_err);

  KAL_LockRelease(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  &release_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(release_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

end_rel:
  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)usb2ser_fnct_handle,
                        DEF_YES);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                       ADAPTER DRIVER FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       USBH_USB2SER_FnctAsyncRxCmpl()
 *
 * @brief    Asynchronous data receive completion function.
 *
 * @param    p_adapter_drv_data  Pointer to the adapter driver data.
 *
 * @param    p_buf               Pointer to the buffer.
 *
 * @param    p_buf_data          Pointer to the beggining of data in buffer.
 *
 * @param    buf_len             Buffer length in bytes.
 *
 * @param    data_len            Length of received data, in bytes.
 *
 * @param    err                 Error code of transfe.
 *******************************************************************************************************/
void USBH_USB2SER_FnctAsyncRxCmpl(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                  CPU_INT08U                         *p_buf,
                                  CPU_INT08U                         *p_buf_data,
                                  CPU_INT32U                         buf_len,
                                  CPU_INT32U                         data_len,
                                  RTOS_ERR                           err)
{
  USBH_USB2SER_FNCT_DATA       *p_usb2ser_fnct_data;
  USBH_USB2SER_DEV_ADAPTER_DRV *p_dev_adapter_drv;
  RTOS_ERR                     err_usbh;
  RTOS_ERR                     release_err;
  CPU_INT32U                   std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        (USBH_CLASS_FNCT_HANDLE)p_adapter_drv_data->Handle,
                                                                        DEF_NO,
                                                                        std_req_timeout,
                                                                        &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring USB2SER class fnct -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

  if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
    if ((data_len > 0u)
        && (USBH_USB2SER_Ptr->AppFnctsPtr != DEF_NULL)
        && (USBH_USB2SER_Ptr->AppFnctsPtr->DataRxd != DEF_NULL)) {
      void *p_app_fncts_arg = p_usb2ser_fnct_data->AppFnctsArgPtr;

      //                                                           Notify app data was received.
      USBH_USB2SER_Ptr->AppFnctsPtr->DataRxd(p_adapter_drv_data->Handle,
                                             p_app_fncts_arg,
                                             p_buf_data,
                                             data_len);
    }
  } else if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_ABORT) {
    LOG_ERR(("On rx completion -> ", RTOS_ERR_LOG_ARG_GET(err)));
  }

  //                                                               Re-submit buffer to adapter driver.
  p_dev_adapter_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->DevAdapterDrvPtr;

  KAL_LockAcquire(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  KAL_OPT_PEND_NONE,
                  std_req_timeout,
                  &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  p_dev_adapter_drv->RxAsync(p_adapter_drv_data,
                             p_buf,
                             buf_len,
                             &err_usbh);
  if ((RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE)
      && (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_INVALID_EP_STATE)) {
    LOG_ERR(("Submitting rx buffer -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
  }

  KAL_LockRelease(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  &release_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(release_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

end_rel:
  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)p_adapter_drv_data->Handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_FnctAsyncTxCmpl()
 *
 * @brief    Asynchronous data transfer completion function.
 *
 * @param    dev_handle  Handle to device.
 *
 * @param    ep_handle   Handle to endpoint.
 *
 * @param    p_buf       Pointer to the transmit buffer.
 *
 * @param    buf_len     Buffer length in octets.
 *
 * @param    xfer_len    Number of octets transferred.
 *
 * @param    p_arg       Asynchronous function argument.
 *
 * @param    err         Status of transfer.
 *******************************************************************************************************/
void USBH_USB2SER_FnctAsyncTxCmpl(USBH_DEV_HANDLE dev_handle,
                                  USBH_EP_HANDLE  ep_handle,
                                  CPU_INT08U      *p_buf,
                                  CPU_INT32U      buf_len,
                                  CPU_INT32U      xfer_len,
                                  void            *p_arg,
                                  RTOS_ERR        err)
{
  void                       *p_async_tx_cmpl_arg;
  RTOS_ERR                   err_lib;
  USBH_USB2SER_FNCT_HANDLE   usb2ser_fnct_handle;
  USBH_USB2SER_ASYNC_TX_DATA *p_async_tx_data = (USBH_USB2SER_ASYNC_TX_DATA *)p_arg;
  USBH_USB2SER_ASYNC_TX_FNCT async_tx_cmpl_fnct;

  (void)&dev_handle;
  (void)&ep_handle;

  async_tx_cmpl_fnct = p_async_tx_data->TxCmplFnct;
  p_async_tx_cmpl_arg = p_async_tx_data->TxCmplArgPtr;
  usb2ser_fnct_handle = p_async_tx_data->AdapterDataPtr->Handle;

  Mem_DynPoolBlkFree(&USBH_USB2SER_Ptr->TxAsyncXferPool,
                     p_arg,
                     &err_lib);
  if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
    LOG_ERR(("Freeing tx async data -> ", RTOS_ERR_LOG_ARG_GET(err_lib)));
  }

  async_tx_cmpl_fnct(usb2ser_fnct_handle,
                     p_async_tx_cmpl_arg,
                     p_buf,
                     buf_len,
                     xfer_len,
                     err);
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_FnctStatusUpdate()
 *
 * @brief    Updates current serial port status.
 *
 * @param    p_adapter_drv_data  Pointer to the adapter driver data.
 *
 * @param    status              Current serial status.
 *******************************************************************************************************/
void USBH_USB2SER_FnctStatusUpdate(USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data,
                                   USBH_USB2SER_SERIAL_STATUS         status)
{
  USBH_USB2SER_FNCT_DATA *p_usb2ser_fnct_data;
  RTOS_ERR               err_usbh;
  CPU_INT32U             std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        (USBH_CLASS_FNCT_HANDLE)p_adapter_drv_data->Handle,
                                                                        DEF_NO,
                                                                        std_req_timeout,
                                                                        &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring USB2SER class fnct -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

  if ((status.Line != p_usb2ser_fnct_data->StatusCur.Line)
      || (status.Modem != p_usb2ser_fnct_data->StatusCur.Modem)) {
    void *p_app_fnct_arg = p_usb2ser_fnct_data->AppFnctsArgPtr;

    p_usb2ser_fnct_data->StatusCur = status;

    if ((USBH_USB2SER_Ptr->AppFnctsPtr != DEF_NULL)
        && (USBH_USB2SER_Ptr->AppFnctsPtr->SerialStatusChng != DEF_NULL)) {
      USBH_USB2SER_Ptr->AppFnctsPtr->SerialStatusChng(p_adapter_drv_data->Handle,
                                                      p_app_fnct_arg,
                                                      status);
    }
  }

  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)p_adapter_drv_data->Handle,
                        DEF_NO);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTION
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBH_USB2SER_ProbeFnct()
 *
 * @brief    Probe class to see if function connected is a USB-to-serial adapter and that it is
 *           supported by any of the registered adapter driver.
 *
 * @param    dev_handle      Handle to device.
 *
 * @param    fnct_handle     Handle to function.
 *
 * @param    class_code      Function class code.
 *
 * @param    pp_class_fnct   Pointer that will receive the internal structure of the function.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this
 *                           function.
 *
 * @return   DEF_OK,   if fnct has been recognized as USB-To-Serial,
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_USB2SER_ProbeFnct(USBH_DEV_HANDLE  dev_handle,
                                          USBH_FNCT_HANDLE fnct_handle,
                                          CPU_INT08U       class_code,
                                          void             **pp_class_fnct,
                                          RTOS_ERR         *p_err)
{
  CPU_BOOLEAN                         ok = DEF_FAIL;
  CPU_INT08U                          cnt;
  CPU_INT16U                          fnct_len = 0u;
  void                                *p_class_fnct = DEF_NULL;
  USBH_DEV_SPD                        dev_spd;
  USBH_CLASS_FNCT_HANDLE              class_fnct_handle = USBH_CLASS_FNCT_HANDLE_INVALID;
  USBH_USB2SER_ADAPTER_DRV_ENTRY_MGMT *p_usb2ser_drv_entry = DEF_NULL;
  USBH_USB2SER_FNCT_DATA              *p_usb2ser_fnct_data = DEF_NULL;
  USBH_USB2SER_FNCT_ADAPTER_DRV_DATA  *p_adapter_drv_data;

  //                                                               Find adapter drv that can handle connected dev.
  for (cnt = 0u; cnt < USBH_USB2SER_Ptr->AdapterDrvEntryTblLen; cnt++) {
    USBH_CLASS_DRV *p_fnct_class_drv;

    p_usb2ser_drv_entry = &USBH_USB2SER_Ptr->AdapterDrvEntryMgmtTbl[cnt];
    fnct_len = p_usb2ser_drv_entry->DrvPtr->DevAdapterDrvPtr->FnctLen;
    p_fnct_class_drv = p_usb2ser_drv_entry->DrvPtr->ClassDrvPtr;

    if (fnct_len != 0u) {
      p_class_fnct = USBH_USB2SER_Ptr->TempAdapterFnctPtr;
    }

    if (p_fnct_class_drv->ProbeFnct != DEF_NULL) {
      ok = p_fnct_class_drv->ProbeFnct(dev_handle,
                                       fnct_handle,
                                       class_code,
                                       &p_class_fnct,
                                       p_err);
      if (ok == DEF_OK) {
        break;
      }
    }
  }

  if (ok == DEF_FAIL) {                                         // No adapter drv found.
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    p_usb2ser_drv_entry = DEF_NULL;

    goto end_free;
  }

  if (USBH_USB2SER_InitCfg.HS_En == DEF_DISABLED) {             // Ensure dev is FS if HS disabled.
    dev_spd = USBH_DevSpdGet(dev_handle, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto end_free;
    }

    if (dev_spd == USBH_DEV_SPD_HIGH) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
      goto end_free;
    }
  }

  class_fnct_handle = USBH_ClassFnctAlloc(&USBH_USB2SER_Ptr->FnctPool,
                                          dev_handle,
                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_USB2SER_FNCT_ALLOC);
    goto end_free;
  }

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        class_fnct_handle,
                                                                        DEF_YES,
                                                                        OP_LOCK_TIMEOUT_INFINITE,
                                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_free;
  }

  if (fnct_len != 0u) {
    Mem_Copy(p_usb2ser_fnct_data->AllocAdapterFnctPtr,
             USBH_USB2SER_Ptr->TempAdapterFnctPtr,
             USBH_USB2SER_Ptr->MaxAdapterDrvFnctLen);

    p_class_fnct = p_usb2ser_fnct_data->AllocAdapterFnctPtr;
  }

  p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr = p_usb2ser_drv_entry;
  p_usb2ser_fnct_data->StatusCur.Line = 0xFFu;
  p_usb2ser_fnct_data->StatusCur.Modem = 0xFFu;

  p_adapter_drv_data = &p_usb2ser_fnct_data->AdapterDrvData;
  p_adapter_drv_data->DataPtr = p_class_fnct;
  p_adapter_drv_data->DevHandle = dev_handle;
  p_adapter_drv_data->FnctHandle = fnct_handle;
  p_adapter_drv_data->Handle = (USBH_USB2SER_FNCT_HANDLE)class_fnct_handle;

  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);

  *pp_class_fnct = (void *)(CPU_ADDR)class_fnct_handle;

  return (ok);

end_free:
  if (p_usb2ser_drv_entry != DEF_NULL) {
    p_usb2ser_drv_entry->DrvPtr->ClassDrvPtr->Disconn((void *)(CPU_ADDR)class_fnct_handle);
  }

  if (class_fnct_handle != USBH_CLASS_FNCT_HANDLE_INVALID) {
    RTOS_ERR err_free;

    USBH_ClassFnctFree(&USBH_USB2SER_Ptr->FnctPool,
                       class_fnct_handle,
                       &err_free);
    if (RTOS_ERR_CODE_GET(err_free) != RTOS_ERR_NONE) {
      LOG_ERR(("Freeing USB2SER class fnct -> ", RTOS_ERR_LOG_ARG_GET(err_free)));
    }
  }

  return (ok);
}

/****************************************************************************************************//**
 *                                           USBH_USB2SER_EP_Probe()
 *
 * @brief    Probes USB-to-serial class driver before opening endpoint to ensure class driver needs
 *           it.
 *
 * @param    p_class_fnct    Pointer to the internal USB-To-Serial function structure.
 *
 * @param    if_ix           Endpoint's interface index.
 *
 * @param    ep_type         Type of endpoint.
 *
 * @param    ep_dir_in       Direction of endpoint.
 *
 * @return   DEF_YES, if endpoint is needed.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_USB2SER_EP_Probe(void        *p_class_fnct,
                                         CPU_INT08U  if_ix,
                                         CPU_INT08U  ep_type,
                                         CPU_BOOLEAN ep_dir_in)
{
  CPU_BOOLEAN            ep_needed = DEF_YES;
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  USBH_USB2SER_FNCT_DATA *p_usb2ser_fnct_data;
  USBH_CLASS_DRV         *p_fnct_class_drv;
  RTOS_ERR               err_usbh;

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        class_fnct_handle,
                                                                        DEF_YES,
                                                                        OP_LOCK_TIMEOUT_INFINITE,
                                                                        &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring USB2SER class fnct -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return (DEF_NO);
  }

  p_fnct_class_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->ClassDrvPtr;
  if (p_fnct_class_drv->EP_Probe != DEF_NULL) {
    ep_needed = p_fnct_class_drv->EP_Probe(p_usb2ser_fnct_data->AdapterDrvData.DataPtr,
                                           ep_type,
                                           ep_dir_in,
                                           if_ix);
  }

  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);

  return (ep_needed);
}

/****************************************************************************************************//**
 *                                           USBH_USB2SER_EP_Open()
 *
 * @brief    Notify USB-To-Serial class that endpoint has been opened.
 *
 * @param    p_class_fnct    Pointer to the internal USB-To-Serial function structure.
 *
 * @param    ep_handle       Handle to endpoint.
 *
 * @param    if_ix           Endpoint's interface index.
 *
 * @param    ep_type         Type of endpoint.
 *
 * @param    ep_dir_in       Direction of endpoint.
 *******************************************************************************************************/
static void USBH_USB2SER_EP_Open(void           *p_class_fnct,
                                 USBH_EP_HANDLE ep_handle,
                                 CPU_INT08U     if_ix,
                                 CPU_INT08U     ep_type,
                                 CPU_BOOLEAN    ep_dir_in)
{
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  USBH_USB2SER_FNCT_DATA *p_usb2ser_fnct_data;
  USBH_CLASS_DRV         *p_fnct_class_drv;
  RTOS_ERR               err_usbh;

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        class_fnct_handle,
                                                                        DEF_YES,
                                                                        OP_LOCK_TIMEOUT_INFINITE,
                                                                        &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring USB2SER class fnct -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

  p_fnct_class_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->ClassDrvPtr;
  if (p_fnct_class_drv->EP_Open != DEF_NULL) {
    p_fnct_class_drv->EP_Open(p_usb2ser_fnct_data->AdapterDrvData.DataPtr,
                              ep_handle,
                              if_ix,
                              ep_type,
                              ep_dir_in);
  }

  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);
}

/****************************************************************************************************//**
 *                                           USBH_USB2SER_EP_Close()
 *
 * @brief    Notify USB-To-Serial class that endpoint has been closed.
 *
 * @param    p_class_fnct    Pointer to the internal USB-To-Serial function structure.
 *
 * @param    ep_handle       Handle to endpoint.
 *
 * @param    if_ix           Endpoint's interface index.
 *******************************************************************************************************/
static void USBH_USB2SER_EP_Close(void           *p_class_fnct,
                                  USBH_EP_HANDLE ep_handle,
                                  CPU_INT08U     if_ix)
{
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  USBH_USB2SER_FNCT_DATA *p_usb2ser_fnct_data;
  USBH_CLASS_DRV         *p_fnct_class_drv;
  RTOS_ERR               err_usbh;

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        class_fnct_handle,
                                                                        DEF_YES,
                                                                        OP_LOCK_TIMEOUT_INFINITE,
                                                                        &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring USB2SER class fnct -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

  p_fnct_class_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->ClassDrvPtr;
  if (p_fnct_class_drv->EP_Close != DEF_NULL) {
    p_fnct_class_drv->EP_Close(p_usb2ser_fnct_data->AdapterDrvData.DataPtr,
                               ep_handle,
                               if_ix);
  }

  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);
}

/****************************************************************************************************//**
 *                                           USBH_USB2SER_Conn()
 *
 * @brief    Notify USB-To-Serial class of a connection.
 *
 * @param    p_class_fnct    Pointer to the internal USB-To-Serial function structure.
 *******************************************************************************************************/
static void USBH_USB2SER_Conn(void *p_class_fnct)
{
  USBH_CLASS_FNCT_HANDLE             class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  USBH_USB2SER_FNCT_DATA             *p_usb2ser_fnct_data;
  USBH_CLASS_DRV                     *p_fnct_class_drv;
  USBH_DEV_HANDLE                    dev_handle = USBH_DEV_HANDLE_INVALID;
  USBH_FNCT_HANDLE                   fnct_handle = USBH_FNCT_HANDLE_INVALID;
  USBH_USB2SER_DEV_ADAPTER_DRV       *p_dev_adapter_drv;
  USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_data;
  RTOS_ERR                           err;
  CPU_INT32U                         std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        class_fnct_handle,
                                                                        DEF_YES,
                                                                        std_req_timeout,
                                                                        &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring USB2SER class fnct -> ", RTOS_ERR_LOG_ARG_GET(err)));
    return;
  }

  p_fnct_class_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->ClassDrvPtr;
  p_dev_adapter_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->DevAdapterDrvPtr;
  p_adapter_data = &p_usb2ser_fnct_data->AdapterDrvData;
  if (p_fnct_class_drv->Conn != DEF_NULL) {
    p_fnct_class_drv->Conn(p_adapter_data->DataPtr);
  }

  KAL_LockAcquire(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  KAL_OPT_PEND_NONE,
                  std_req_timeout,
                  &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  if (p_dev_adapter_drv->Conn != DEF_NULL) {
    p_dev_adapter_drv->Conn(p_adapter_data);
  }

  p_dev_adapter_drv->Reset(p_adapter_data,                      // Purge all buffers on adapter.
                           USBH_USB2SER_RESET_SEL_ALL,
                           &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Resetting device -> ", RTOS_ERR_LOG_ARG_GET(err)));
  }

  KAL_LockRelease(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  &err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  dev_handle = p_adapter_data->DevHandle;
  fnct_handle = p_adapter_data->FnctHandle;

end_rel:
  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);

  if ((USBH_USB2SER_Ptr->AppFnctsPtr != DEF_NULL)
      && (USBH_USB2SER_Ptr->AppFnctsPtr->Conn != DEF_NULL)
      && (dev_handle != USBH_DEV_HANDLE_INVALID)
      && (fnct_handle != USBH_FNCT_HANDLE_INVALID)) {
    p_usb2ser_fnct_data->AppFnctsArgPtr = USBH_USB2SER_Ptr->AppFnctsPtr->Conn(dev_handle,
                                                                              fnct_handle,
                                                                              (USBH_USB2SER_FNCT_HANDLE)class_fnct_handle);
  }
}

/****************************************************************************************************//**
 *                                           USBH_USB2SER_IF_AltSet()
 *
 * @brief    Notify USB-To-Serial class that alternate interface was set.
 *
 * @param    p_class_fnct    Pointer to the internal USB-To-Serial function structure.
 *
 * @param    if_ix           Interface index.
 *******************************************************************************************************/
static void USBH_USB2SER_IF_AltSet(void       *p_class_fnct,
                                   CPU_INT08U if_ix)
{
  CPU_INT08U                         cnt;
  CPU_INT16U                         max_xfer_len;
  USBH_DEV_SPD                       spd;
  USBH_CLASS_FNCT_HANDLE             class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  USBH_USB2SER_FNCT_DATA             *p_usb2ser_fnct_data;
  USBH_CLASS_DRV                     *p_fnct_class_drv;
  USBH_USB2SER_DEV_ADAPTER_DRV       *p_dev_adapter_drv;
  USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_data;
  RTOS_ERR                           err;
  RTOS_ERR                           release_err;
  CPU_INT32U                         std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        class_fnct_handle,
                                                                        DEF_NO,
                                                                        std_req_timeout,
                                                                        &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring USB2SER class fnct -> ", RTOS_ERR_LOG_ARG_GET(err)));
    return;
  }

  p_fnct_class_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->ClassDrvPtr;
  p_dev_adapter_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->DevAdapterDrvPtr;
  p_adapter_data = &p_usb2ser_fnct_data->AdapterDrvData;
  if (p_fnct_class_drv->IF_AltSet != DEF_NULL) {
    p_fnct_class_drv->IF_AltSet(p_adapter_data->DataPtr, if_ix);
  }

  spd = USBH_DevSpdGet(p_adapter_data->DevHandle,
                       &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Retrieving dev spd -> ", RTOS_ERR_LOG_ARG_GET(err)));
    return;
  }

  max_xfer_len = ((spd == USBH_DEV_SPD_HIGH) ? p_dev_adapter_drv->BufLenRxHS : p_dev_adapter_drv->BufLenRxFS);

  KAL_LockAcquire(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  KAL_OPT_PEND_NONE,
                  std_req_timeout,
                  &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  for (cnt = 0u; cnt < USBH_USB2SER_InitCfg.RxBufQty; cnt++) {
    p_dev_adapter_drv->RxAsync(p_adapter_data,
                               p_usb2ser_fnct_data->RxBufPtrTbl[cnt],
                               max_xfer_len,
                               &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      LOG_ERR(("Initiating data reception -> ", RTOS_ERR_LOG_ARG_GET(err)));
    }
  }

  KAL_LockRelease(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  &release_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(release_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

end_rel:
  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                           USBH_USB2SER_Disconn()
 *
 * @brief    Notify USB-To-Serial class of a disconnection.
 *
 * @param    p_class_fnct    Pointer to the internal USB-To-Serial function structure.
 *******************************************************************************************************/
static void USBH_USB2SER_Disconn(void *p_class_fnct)
{
  void                   *p_app_fnct_arg;
  void                   *p_adapter_drv_arg;
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  USBH_USB2SER_FNCT_DATA *p_usb2ser_fnct_data;
  USBH_CLASS_DRV         *p_fnct_class_drv;
  RTOS_ERR               err_usbh;
  CPU_INT32U             std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_USB2SER_ReqTimeout;
  CORE_EXIT_ATOMIC();

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        class_fnct_handle,
                                                                        DEF_YES,
                                                                        std_req_timeout,
                                                                        &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring USB2SER class fnct -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

  p_fnct_class_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->ClassDrvPtr;
  p_adapter_drv_arg = p_usb2ser_fnct_data->AdapterDrvData.DataPtr;
  p_app_fnct_arg = p_usb2ser_fnct_data->AppFnctsArgPtr;

  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);

  if ((USBH_USB2SER_Ptr->AppFnctsPtr != DEF_NULL)
      && (USBH_USB2SER_Ptr->AppFnctsPtr->Disconn != DEF_NULL)) {
    USBH_USB2SER_Ptr->AppFnctsPtr->Disconn((USBH_USB2SER_FNCT_HANDLE)class_fnct_handle,
                                           p_app_fnct_arg);
  }

  KAL_LockAcquire(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  KAL_OPT_PEND_NONE,
                  std_req_timeout,
                  &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  if (p_fnct_class_drv->Disconn != DEF_NULL) {
    p_fnct_class_drv->Disconn(p_adapter_drv_arg);
  }

  KAL_LockRelease(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->LockHandle,
                  &err_usbh);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err_usbh) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

end_rel:
  USBH_ClassFnctFree(&USBH_USB2SER_Ptr->FnctPool,
                     class_fnct_handle,
                     &err_usbh);
}

/****************************************************************************************************//**
 *                                           USBH_USB2SER_Suspend()
 *
 * @brief    Suspends USB-To-Serial function.
 *
 * @param    p_class_fnct    Pointer to the internal USB-To-Serial function structure.
 *******************************************************************************************************/
static void USBH_USB2SER_Suspend(void *p_class_fnct)
{
  USBH_CLASS_FNCT_HANDLE             class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  USBH_USB2SER_FNCT_DATA             *p_usb2ser_fnct_data;
  USBH_CLASS_DRV                     *p_fnct_class_drv;
  USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_data;
  RTOS_ERR                           err_usbh;

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        class_fnct_handle,
                                                                        DEF_YES,
                                                                        OP_LOCK_TIMEOUT_INFINITE,
                                                                        &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring USB2SER class fnct -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

  p_fnct_class_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->ClassDrvPtr;
  p_adapter_data = &p_usb2ser_fnct_data->AdapterDrvData;
  if (p_fnct_class_drv->Suspend != DEF_NULL) {
    p_fnct_class_drv->Suspend(p_adapter_data);
  }

  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);
}

/****************************************************************************************************//**
 *                                           USBH_USB2SER_Resume()
 *
 * @brief    Resumes USB-To-Serial function.
 *
 * @param    p_class_fnct    Pointer to the internal USB-To-Serial function structure.
 *******************************************************************************************************/
static void USBH_USB2SER_Resume(void *p_class_fnct)
{
  USBH_CLASS_FNCT_HANDLE             class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  USBH_USB2SER_FNCT_DATA             *p_usb2ser_fnct_data;
  USBH_CLASS_DRV                     *p_fnct_class_drv;
  USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_data;
  RTOS_ERR                           err_usbh;

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        class_fnct_handle,
                                                                        DEF_YES,
                                                                        OP_LOCK_TIMEOUT_INFINITE,
                                                                        &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring USB2SER class fnct -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

  p_fnct_class_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->ClassDrvPtr;
  p_adapter_data = &p_usb2ser_fnct_data->AdapterDrvData;
  if (p_fnct_class_drv->Resume != DEF_NULL) {
    p_fnct_class_drv->Resume(p_adapter_data);
  }

  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);
}

/****************************************************************************************************//**
 *                                           USBH_USB2SER_UnInit()
 *
 * @brief    Un-initializes USB-To-Serial class and all its function classes.
 *******************************************************************************************************/

#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
static void USBH_USB2SER_UnInit(void)
{
  USBH_CLASS_DRV *p_fnct_class_drv;
  CPU_INT08U     cnt;
  RTOS_ERR       err_usbh;

  //                                                               Iterate through all fnct classes.
  for (cnt = 0u; cnt < USBH_USB2SER_Ptr->AdapterDrvEntryTblLen; cnt++) {
    p_fnct_class_drv = USBH_USB2SER_Ptr->AdapterDrvEntryMgmtTbl[cnt].DrvPtr->ClassDrvPtr;

    if (p_fnct_class_drv->UnInit != DEF_NULL) {
      p_fnct_class_drv->UnInit();
    }
  }

  USBH_ClassFnctPoolDel(&USBH_USB2SER_Ptr->FnctPool,
                        &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Deleting USB2SER fnct pool -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
  }

  USBH_USB2SER_Ptr = DEF_NULL;
}
#endif

/****************************************************************************************************//**
 *                                           USBH_USB2SER_TraceDump()
 *
 * @brief    Output class specific debug information on function.
 *
 * @param    p_class_fnct    Pointer to the internal USB-To-Serial function structure.
 *
 * @param    opt             Trace dump options.
 *
 * @param    trace_fnct      Function to call to output traces.
 *******************************************************************************************************/

#ifdef RTOS_MODULE_COMMON_SHELL_AVAIL
static void USBH_USB2SER_TraceDump(void                *p_class_fnct,
                                   CPU_INT32U          opt,
                                   USBH_CMD_TRACE_FNCT trace_fnct)
{
  CPU_BOOLEAN                        sw_en;
  CPU_CHAR                           str[8u];
  CPU_INT08U                         data_size;
  CPU_INT32U                         baudrate;
  USBH_CLASS_FNCT_HANDLE             class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  USBH_CLASS_DRV                     *p_adapter_class_drv;
  USBH_USB2SER_DEV_ADAPTER_DRV       *p_adapter_dev_drv;
  USBH_USB2SER_FNCT_DATA             *p_usb2ser_fnct_data;
  USBH_USB2SER_PARITY                parity;
  USBH_USB2SER_STOP_BITS             stop_bits;
  USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL protocol;
  RTOS_ERR                           err_usbh;

  p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)USBH_ClassFnctAcquire(&USBH_USB2SER_Ptr->FnctPool,
                                                                        class_fnct_handle,
                                                                        DEF_YES,
                                                                        USBH_USB2SER_ReqTimeout,
                                                                        &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring USB2SER class fnct -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

  p_adapter_class_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->ClassDrvPtr;
  p_adapter_dev_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->DevAdapterDrvPtr;

  trace_fnct("    ----- USB Host (USB-To-Serial specific debug information) ------\r\n");
  trace_fnct("    | BaudRate  | Data | Stop | Parity | Flow ctrl                 |\r\n");
  trace_fnct("    | ");

  baudrate = p_adapter_dev_drv->BaudRateGet(&p_usb2ser_fnct_data->AdapterDrvData,
                                            &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    return;
  }

  p_adapter_dev_drv->DataGet(&p_usb2ser_fnct_data->AdapterDrvData,
                             &data_size,
                             &parity,
                             &stop_bits,
                             &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Output baud rate.
  (void)Str_FmtNbr_Int32U(baudrate,
                          8u,
                          DEF_NBR_BASE_DEC,
                          ' ',
                          DEF_NO,
                          DEF_YES,
                          &str[0u]);
  trace_fnct(str);
  trace_fnct("  |  ");

  //                                                               Output data size.
  (void)Str_FmtNbr_Int32U(data_size,
                          2u,
                          DEF_NBR_BASE_DEC,
                          ' ',
                          DEF_NO,
                          DEF_YES,
                          &str[0u]);
  trace_fnct(str);
  trace_fnct("  | ");

  //                                                               Output qty of stop bits.
  switch (stop_bits) {
    case USBH_USB2SER_STOP_BITS_1:
      trace_fnct("1    | ");
      break;

    case USBH_USB2SER_STOP_BITS_1_5:
      trace_fnct("1.5  | ");
      break;

    case USBH_USB2SER_STOP_BITS_2:
      trace_fnct("2    | ");
      break;

    default:
      trace_fnct("???  | ");
      break;
  }

  //                                                               Output parity.
  switch (parity) {
    case USBH_USB2SER_PARITY_NONE:
      trace_fnct("None   | ");
      break;

    case USBH_USB2SER_PARITY_ODD:
      trace_fnct("Odd    | ");
      break;

    case USBH_USB2SER_PARITY_EVEN:
      trace_fnct("Even   | ");
      break;

    case USBH_USB2SER_PARITY_MARK:
      trace_fnct("Mark   | ");
      break;

    case USBH_USB2SER_PARITY_SPACE:
      trace_fnct("Space  | ");
      break;

    default:
      trace_fnct("????   | ");
      break;
  }

  protocol = p_adapter_dev_drv->HW_FlowCtrlGet(&p_usb2ser_fnct_data->AdapterDrvData,
                                               &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    return;
  }

  sw_en = p_adapter_dev_drv->SW_FlowCtrlGet(&p_usb2ser_fnct_data->AdapterDrvData,
                                            DEF_NULL,
                                            DEF_NULL,
                                            &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Output flow control.
  switch (protocol) {
    case USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_NONE:
      trace_fnct("HW: None    / SW: ");
      break;

    case USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_DTR_DSR:
      trace_fnct("HW: DTR-DSR / SW: ");
      break;

    case USBH_USB2SER_HW_FLOW_CTRL_PROTOCOL_RTS_CTS:
      trace_fnct("HW: RTS-CTS / SW: ");
      break;

    default:
      trace_fnct("HW: ??????? / SW: ");
      break;
  }

  if (sw_en == DEF_ENABLED) {
    trace_fnct("EN      |");
  } else {
    trace_fnct("DIS     |");
  }

  trace_fnct("\r\n");

  //                                                               Output adapter driver name.
  p_adapter_class_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->ClassDrvPtr;
  if (p_adapter_class_drv->NamePtr != DEF_NULL) {
    trace_fnct("    | Adapter driver: ");
    trace_fnct(p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->ClassDrvPtr->NamePtr);
    trace_fnct("\r\n");
  }

  if (p_adapter_class_drv->TraceDump != DEF_NULL) {
    p_adapter_class_drv->TraceDump(p_usb2ser_fnct_data->AdapterDrvData.DataPtr,
                                   opt,
                                   trace_fnct);
  }

  USBH_ClassFnctRelease(&USBH_USB2SER_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);
}
#endif

/****************************************************************************************************//**
 *                                       USBH_USB2SER_ModemCtrlApply()
 *
 * @brief    Applies modem control to adapter driver.
 *
 * @param    p_usb2ser_fnct_data     Pointer to the USB-To-serial function.
 *
 * @param    dtr_en                  Flag that indicates if Data Terminal Ready (DTR) pin manual control
 *                                   is enabled. 0xFF means no change.
 *
 * @param    dtr_set                 Flag that indicates, when DTR pin control is enabled, if the pin state
 *                                   is set or not. 0xFF means no change.
 *
 * @param    rts_en                  Flag that indicates if Ready To Send (RTS) pin manual  control is
 *                                   enabled. 0xFF means no change.
 *
 * @param    rts_set                 Flag that indicates, when RTS pin control is enabled, if the pin state
 *                                   is set or not. 0xFF means no change.
 *
 * @param    p_err                   Pointer to the variable that will receive the error code from this function.
 *******************************************************************************************************/
static void USBH_USB2SER_ModemCtrlApply(USBH_USB2SER_FNCT_DATA *p_usb2ser_fnct_data,
                                        CPU_INT08U             dtr_en,
                                        CPU_INT08U             dtr_set,
                                        CPU_INT08U             rts_en,
                                        CPU_INT08U             rts_set,
                                        RTOS_ERR               *p_err)
{
  USBH_USB2SER_DEV_ADAPTER_DRV *p_dev_adapter_drv = p_usb2ser_fnct_data->AdapterDrvEntryMgmtPtr->DrvPtr->DevAdapterDrvPtr;
  CPU_BOOLEAN                  apply = DEF_NO;
  CPU_BOOLEAN                  dtr_en_cur;
  CPU_BOOLEAN                  dtr_set_cur;
  CPU_BOOLEAN                  rts_en_cur;
  CPU_BOOLEAN                  rts_set_cur;

  p_dev_adapter_drv->ModemCtrlGet(&p_usb2ser_fnct_data->AdapterDrvData,
                                  &dtr_en_cur,
                                  &dtr_set_cur,
                                  &rts_en_cur,
                                  &rts_set_cur,
                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if ((dtr_en != 0xFF)
      && (dtr_en != dtr_en_cur)) {
    dtr_en_cur = dtr_en;
    apply = DEF_YES;
  }

  if ((dtr_set != 0xFF)
      && (dtr_set != dtr_set_cur)) {
    dtr_set_cur = dtr_set;
    apply = DEF_YES;
  }

  if ((rts_en != 0xFF)
      && (rts_en != rts_en_cur)) {
    rts_en_cur = rts_en;
    apply = DEF_YES;
  }

  if ((rts_set != 0xFF)
      && (rts_set != rts_set_cur)) {
    rts_set_cur = rts_set;
    apply = DEF_YES;
  }

  if (apply == DEF_YES) {
    p_dev_adapter_drv->ModemCtrlSet(&p_usb2ser_fnct_data->AdapterDrvData,
                                    dtr_en_cur,
                                    dtr_set_cur,
                                    rts_en_cur,
                                    rts_set_cur,
                                    p_err);
  }
}

/****************************************************************************************************//**
 *                                       USBH_USB2SER_FnctAllocCallback()
 *
 * @brief    Callback when USB-to-serial function is allocated.
 *
 * @param    p_class_fnct_data   Pointer to the class function data.
 *
 * @return   DEF_OK,   if NO error(s),
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_USB2SER_FnctAllocCallback(void *p_class_fnct_data)
{
  CPU_INT08U                         cnt;
  RTOS_ERR                           err_lib;
  USBH_USB2SER_FNCT_DATA             *p_usb2ser_fnct_data = (USBH_USB2SER_FNCT_DATA *)p_class_fnct_data;
  USBH_USB2SER_FNCT_ADAPTER_DRV_DATA *p_adapter_drv_data = &p_usb2ser_fnct_data->AdapterDrvData;

  //                                                               -------------- ALLOC RX BUFFERS TABLE --------------
  p_usb2ser_fnct_data->RxBufPtrTbl = (CPU_INT08U **)Mem_SegAlloc("USBH - USB2SER Fnct Rx Buf tbl",
                                                                 USBH_USB2SER_InitCfg.MemSegPtr,
                                                                 (sizeof(CPU_INT08U *) * USBH_USB2SER_InitCfg.RxBufQty),
                                                                 &err_lib);
  if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  for (cnt = 0u; cnt < USBH_USB2SER_InitCfg.RxBufQty; cnt++) {
    p_usb2ser_fnct_data->RxBufPtrTbl[cnt] = (CPU_INT08U *)Mem_SegAllocHW("USBH - USB2SER Fnct Rx Buf",
                                                                         USBH_USB2SER_InitCfg.MemSegBufPtr,
                                                                         USBH_USB2SER_Ptr->MaxAdapterRxBufLen,
                                                                         USBH_USB2SER_InitCfg.BufAlignOctets,
                                                                         DEF_NULL,
                                                                         &err_lib);
    if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
      return (DEF_FAIL);
    }
  }

  //                                                               ------------- ALLOC SPACE FOR CTRL BUF -------------
  if (USBH_USB2SER_Ptr->MaxAdapterCtrlBufLen > 0u) {
    p_adapter_drv_data->AdapterCtrlBufPtr = (CPU_INT08U *)Mem_SegAllocHW("USBH - USB2SER adapter ctrl buf ",
                                                                         USBH_USB2SER_InitCfg.MemSegBufPtr,
                                                                         USBH_USB2SER_Ptr->MaxAdapterCtrlBufLen * sizeof(CPU_INT08U),
                                                                         USBH_USB2SER_InitCfg.BufAlignOctets,
                                                                         DEF_NULL,
                                                                         &err_lib);
    if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
      return (DEF_FAIL);
    }
  } else {
    p_adapter_drv_data->AdapterCtrlBufPtr = DEF_NULL;
  }

  //                                                               ------------ ALLOC SPACE FOR STATUS BUF ------------
  if (USBH_USB2SER_Ptr->MaxAdapterStatusBufLen > 0u) {
    p_adapter_drv_data->AdapterStatusBufPtr = (CPU_INT08U *)Mem_SegAllocHW("USBH - USB2SER adapter status buf ",
                                                                           USBH_USB2SER_InitCfg.MemSegBufPtr,
                                                                           USBH_USB2SER_Ptr->MaxAdapterStatusBufLen * sizeof(CPU_INT08U),
                                                                           USBH_USB2SER_InitCfg.BufAlignOctets,
                                                                           DEF_NULL,
                                                                           &err_lib);
    if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
      return (DEF_FAIL);
    }
  } else {
    p_adapter_drv_data->AdapterStatusBufPtr = DEF_NULL;
  }

  //                                                               --------- ALLOC SPACE FOR ADAPTER DRV FNCT ---------
  if (USBH_USB2SER_Ptr->MaxAdapterDrvFnctLen > 0u) {
    p_usb2ser_fnct_data->AllocAdapterFnctPtr = Mem_SegAlloc("USBH - USB2SER adapter drv fnct ",
                                                            USBH_USB2SER_InitCfg.MemSegPtr,
                                                            USBH_USB2SER_Ptr->MaxAdapterDrvFnctLen * sizeof(CPU_INT08U),
                                                            &err_lib);
    if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
      return (DEF_FAIL);
    }
  } else {
    p_usb2ser_fnct_data->AllocAdapterFnctPtr = DEF_NULL;
  }

  return (DEF_OK);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_HOST_USB2SER_AVAIL))
