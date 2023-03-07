/***************************************************************************//**
 * @file
 * @brief USB Host - Communications Device Class (Cdc)
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

#if (defined(RTOS_MODULE_USB_HOST_CDC_AVAIL))

#if (!defined(RTOS_MODULE_USB_HOST_AVAIL))

#error USB HOST CDC class requires USB Host Core. Make sure it is part of your project and that \
  RTOS_MODULE_USB_HOST_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define   MICRIUM_SOURCE

#include  <em_core.h>

#include  <cpu/include/cpu.h>

#include  <common/include/lib_mem.h>
#include  <common/include/lib_str.h>
#include  <common/include/logging.h>
#include  <common/source/op_lock/op_lock_priv.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>
#include  <rtos_description.h>

#include  <usb/include/host/usbh_cdc.h>
#include  <usb/include/host/usbh_core_fnct.h>
#include  <usb/include/host/usbh_core_if.h>
#include  <usb/include/host/usbh_core_handle.h>

#include  <usb/source/host/cmd/usbh_cmd_priv.h>
#include  <usb/source/host/class/usbh_class_core_priv.h>
#include  <usb/source/host/class/usbh_class_ep_priv.h>
#include  <usb/source/host/class/usbh_class_common_priv.h>
#include  <usb/source/host/class/usbh_cdc_subclass_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (USBH, CLASS, CDC)
#define  RTOS_MODULE_CUR                                 RTOS_CFG_MODULE_USBH

#define  USBH_CDC_FNCT_HANDLE_CREATE(cdc_ix)            ((USBH_CDC_FNCT_HANDLE)(cdc_ix))
#define  USBH_CDC_FNCT_HANDLE_IX_GET(cdc_fnct_handle)   ((CPU_INT08U)(cdc_fnct_handle))

#define  USBH_CDC_MGMT_REQ_BUF_LEN                       USBH_CDC_REQ_LEN_SET_LINE_CODING

/********************************************************************************************************
 *                                           NOTIFICATION CODES
 *
 * Note(s) : (1) For more information on CDC notifications, see 'USB Class Definition for Communication
 *               Devices, version 1.1, section 6.3'.
 *******************************************************************************************************/

#define  USBH_CDC_NOTIFICATION_NET_CONN                 0x00u
#define  USBH_CDC_NOTIFICATION_RESP_AVAIL               0x01u
#define  USBH_CDC_NOTIFICATION_AUX_JACK_HOOK_STATE      0x08u
#define  USBH_CDC_NOTIFICATION_RING_DETECT              0x09u
#define  USBH_CDC_NOTIFICATION_SERIAL_STATE             0x20u
#define  USBH_CDC_NOTIFICATION_CALL_STATE_CHNG          0x28u
#define  USBH_CDC_NOTIFICATION_LINE_STATE_CHNG          0x29u
#define  USBH_CDC_NOTIFICATION_CONN_SPD_CHNG            0x2Au

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       DEFAULT CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const USBH_CDC_INIT_CFG USBH_CDC_InitCfgDflt = {
  .BufAlignOctets = LIB_MEM_BUF_ALIGN_AUTO,
  .EventURB_Qty = 1u,
  .MemSegPtr = DEF_NULL,
  .MemSegBufPtr = DEF_NULL,

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  .OptimizeSpd = { .FnctQty = 1u, .IF_PerFnctQty = 2u },
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  .InitAlloc = { .FnctQty = 1u, .AsyncXferQty = 2u, .DCI_Qty = 1u }
#endif
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               ASYNC XFER DATA
 *******************************************************************************************************/

typedef struct usbh_cdc_async_xfer {
  USBH_CDC_FNCT_HANDLE CDC_FnctHandle;                          // Handle to CDC function.
  USBH_CDC_ASYNC_FNCT  AsyncFnct;                               // Async function to call on xfer completion.
  void                 *ArgPtr;                                 // Pointer to app/subclass specific data.

  CPU_INT08U           DCI_Ix;                                  // Index of DCI.
} USBH_CDC_ASYNC_XFER;

/********************************************************************************************************
 *                                       DATA CONTROL INTERFACE
 *
 * Note(s) : (1) This structure contains information related to a DCI (Endpoint handles, types, etc.).
 *******************************************************************************************************/

typedef struct usbh_cdc_dci {
  CPU_INT08U          Ix;                                       // DCI index.
  CPU_BOOLEAN         IsBulk;                                   // DCI index.

  USBH_EP_HANDLE      EP_HandleIn;                              // Handle to IN EP.
  USBH_EP_HANDLE      EP_HandleOut;                             // Handle to OUT EP.

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_DISABLED)
  struct usbh_cdc_dci *NextPtr;
#endif
} USBH_CDC_DCI;

/********************************************************************************************************
 *                                               CDC SUBCLASS
 *******************************************************************************************************/

typedef struct usbh_cdc_subclass {
  USBH_CLASS_DRV           *SubclassDrvPtr;                     // Ptr to std class drv.
  USBH_CDC_SUBCLASS_DRV    *CDC_SubclassDrvPtr;                 // Ptr to CDC specific subclass drv.

  struct usbh_cdc_subclass *NextPtr;
} USBH_CDC_SUBCLASS;

/********************************************************************************************************
 *                                               CDC FUNCTION
 *******************************************************************************************************/

typedef struct usbh_cdc_fnct {
#if (USBH_CDC_CFG_NOTIFICATIONS_RX_EN == DEF_ENABLED)
  CPU_INT08U        **EventBufPtrTbl;                           // CDC function table of event buffer.
#endif

  CPU_INT16U        RelNbr;                                     // CDC function release number (bcdCDC).

  USBH_DEV_HANDLE   DevHandle;                                  // Handle to device.
  USBH_FNCT_HANDLE  FnctHandle;                                 // Handle to function.
  USBH_EP_HANDLE    EventEP_Handle;                             // Handle to EP used for event (notifications).
  CPU_BOOLEAN       EventEP_IsIntr;                             // Fag that indicate if EP is intr (bulk otherwise).

  USBH_CDC_SUBCLASS *SubclassPtr;                               // Ptr to subclass structure.
  void              *SubclassFnctPtr;                           // Ptr to subclass function.
  void              *SubclassArgPtr;                            // Ptr to subclass argument.

  CPU_INT08U        DCI_Qty;                                    // Quantity of DCI.
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  USBH_CDC_DCI      **DCI_PtrTbl;                               // Ptr to DCI table.
#else
  USBH_CDC_DCI      *DCI_HeadPtr;                               // Ptr to head of DCI list.
#endif
} USBH_CDC_FNCT;

/********************************************************************************************************
 *                                               CDC CLASS
 *******************************************************************************************************/

typedef struct usbh_cdc {
  CPU_INT32U           StdReqTimeoutMs;                         // Timeout, in milliseconds, for std requests.

#if (USBH_CDC_CFG_NOTIFICATIONS_RX_EN == DEF_ENABLED)
  CPU_INT16U           MaxBufLenNotifications;                  // Max len of notifications buf among all subclasses.
#endif

  USBH_CDC_SUBCLASS    *SubclassHeadPtr;                        // Pointer to head of subclass drivers list.

  USBH_CLASS_FNCT_POOL FnctPool;                                // CDC function pool.
  MEM_DYN_POOL         DCI_Pool;                                // DCI pool.

  MEM_DYN_POOL         AsyncXferPool;                           // Pool of asynchronous xfer data.

  CPU_INT08U           *MgmtReqBufPtr;                          // Pointer to mgmt req buffer.
  KAL_LOCK_HANDLE      MgmtReqBufLock;                          // Lock on mgmt req buf.
} USBH_CDC;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static USBH_CDC *USBH_CDC_Ptr = DEF_NULL;                       // Ptr to CDC internal structure.

/********************************************************************************************************
 *                                           CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
USBH_CDC_INIT_CFG USBH_CDC_InitCfg = {
  .BufAlignOctets = LIB_MEM_BUF_ALIGN_AUTO,
  .EventURB_Qty = 1u,
  .MemSegPtr = DEF_NULL,
  .MemSegBufPtr = DEF_NULL,

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  .OptimizeSpd = { .FnctQty = 0u, .IF_PerFnctQty = 0u },
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  .InitAlloc = { .FnctQty = 0u, .AsyncXferQty = 0u, .DCI_Qty = 0u }
#endif
};
#else
extern USBH_CDC_INIT_CFG USBH_CDC_InitCfg;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_BOOLEAN USBH_CDC_ProbeDev(USBH_DEV_HANDLE dev_handle,
                                     CPU_INT08U      class_code,
                                     void            **pp_class_fnct,
                                     RTOS_ERR        *p_err);

static CPU_BOOLEAN USBH_CDC_ProbeFnct(USBH_DEV_HANDLE  dev_handle,
                                      USBH_FNCT_HANDLE fnct_handle,
                                      CPU_INT08U       class_code,
                                      void             **pp_class_fnct,
                                      RTOS_ERR         *p_err);

#if (USBH_CDC_CFG_NOTIFICATIONS_RX_EN == DEF_DISABLED)
static CPU_BOOLEAN USBH_CDC_EP_Probe(void        *p_class_fnct,
                                     CPU_INT08U  if_ix,
                                     CPU_INT08U  ep_type,
                                     CPU_BOOLEAN ep_dir_in);
#endif

static void USBH_CDC_EP_Open(void           *p_class_fnct,
                             USBH_EP_HANDLE ep_handle,
                             CPU_INT08U     if_ix,
                             CPU_INT08U     ep_type,
                             CPU_BOOLEAN    ep_dir_in);

static void USBH_CDC_EP_Close(void           *p_class_fnct,
                              USBH_EP_HANDLE ep_handle,
                              CPU_INT08U     if_ix);

static void USBH_CDC_Conn(void *p_class_fnct);

static void USBH_CDC_IF_AltSet(void       *p_class_fnct,
                               CPU_INT08U if_ix);

static void USBH_CDC_Disconn(void *p_class_fnct);

static void USBH_CDC_Suspend(void *p_class_fnct);

static void USBH_CDC_Resume(void *p_class_fnct);

#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
static void USBH_CDC_UnInit(void);
#endif

#ifdef RTOS_MODULE_COMMON_SHELL_AVAIL
static void USBH_CDC_TraceDump(void                *p_class_fnct,
                               CPU_INT32U          opt,
                               USBH_CMD_TRACE_FNCT trace_fnct);
#endif

#if (USBH_CDC_CFG_NOTIFICATIONS_RX_EN == DEF_ENABLED)
static void USBH_CDC_EventRxCmpl(USBH_DEV_HANDLE dev_handle,
                                 USBH_EP_HANDLE  ep_handle,
                                 CPU_INT08U      *p_buf,
                                 CPU_INT32U      buf_len,
                                 CPU_INT32U      xfer_len,
                                 void            *p_arg,
                                 RTOS_ERR        err);
#endif

static void USBH_CDC_DCI_XferCmpl(USBH_DEV_HANDLE dev_handle,
                                  USBH_EP_HANDLE  ep_handle,
                                  CPU_INT08U      *p_buf,
                                  CPU_INT32U      buf_len,
                                  CPU_INT32U      xfer_len,
                                  void            *p_arg,
                                  RTOS_ERR        err);

static USBH_CDC_DCI *USBH_CDC_DCI_PtrGet(USBH_CDC_FNCT *p_cdc_fnct,
                                         CPU_INT08U    dci_ix,
                                         RTOS_ERR      *p_err);

static void USBH_CDC_MgmtReqBufLock(CPU_INT32U timeout,
                                    RTOS_ERR   *p_err);

static void USBH_CDC_MgmtReqBufRelease(void);

static CPU_BOOLEAN USBH_CDC_FnctAllocCallback(void *p_class_fnct_data);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#if  (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_IF_CLASS) != DEF_YES)
#error  "USBH_CFG_FIELD_EN_IF_CLASS  Must be enabled"
#endif

#if  (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_IF_SUBCLASS) != DEF_YES)
#error  "USBH_CFG_FIELD_EN_IF_SUBCLASS  Must be enabled"
#endif

#if  (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_IF_PROTOCOL) != DEF_YES)
#error  "USBH_CFG_FIELD_EN_IF_PROTOCOL  Must be enabled"
#endif

#if ((USBH_CDC_CFG_NOTIFICATIONS_RX_EN == DEF_ENABLED) \
  && (USBH_CFG_PERIODIC_XFER_EN != DEF_ENABLED))
#error  "USBH_CFG_PERIODIC_XFER_EN Must be set to DEF_ENABLED when USBH_CDC_CFG_NOTIFICATIONS_RX_EN is set to DEF_ENABLED"
#endif

/********************************************************************************************************
 *                                           CDC CLASS DRIVER
 *******************************************************************************************************/

USBH_CLASS_DRV USBH_CDC_Drv = {
  USBH_CDC_ProbeDev,
  USBH_CDC_ProbeFnct,
#if (USBH_CDC_CFG_NOTIFICATIONS_RX_EN == DEF_DISABLED)
  USBH_CDC_EP_Probe,
#else
  DEF_NULL,
#endif
  USBH_CDC_EP_Open,
  USBH_CDC_EP_Close,
  USBH_CDC_Conn,
  USBH_CDC_IF_AltSet,
  USBH_CDC_Disconn,
  USBH_CDC_Suspend,
  USBH_CDC_Resume,
#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
  USBH_CDC_UnInit,
#else
  DEF_NULL,
#endif
#ifdef RTOS_MODULE_COMMON_SHELL_AVAIL
  USBH_CDC_TraceDump,
#endif
  (CPU_CHAR *)"CDC",
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                   USBH_CDC_ConfigureBufAlignOctets()
 *
 * @brief    Configures the alignment of the internal buffers.
 *
 * @param    buf_align_octets    Buffer alignment, in octets.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the CDC class is initialized via the
 *               USBH_CDC_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_CDC_ConfigureBufAlignOctets(CPU_SIZE_T buf_align_octets)
{
  RTOS_ASSERT_CRITICAL((USBH_CDC_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_CDC_InitCfg.BufAlignOctets = buf_align_octets;
}
#endif

/****************************************************************************************************//**
 *                                       USBH_CDC_ConfigureEventURB_Qty()
 *
 * @brief    Configures the quantity of URBs used to retrieve CDC status to allocate/submit.
 *
 * @param    event_urb_qty   Quantity of event URBs.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the CDC class is initialized via the
 *               USBH_CDC_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_CDC_ConfigureEventURB_Qty(CPU_INT08U event_urb_qty)
{
  RTOS_ASSERT_CRITICAL((USBH_CDC_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_CDC_InitCfg.EventURB_Qty = event_urb_qty;
}
#endif

/****************************************************************************************************//**
 *                                       USBH_CDC_ConfigureMemSeg()
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
 * @note     (2) This function MUST be called before the CDC class is initialized via the
 *               USBH_CDC_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_CDC_ConfigureMemSeg(MEM_SEG *p_mem_seg,
                              MEM_SEG *p_mem_seg_buf)
{
  RTOS_ASSERT_CRITICAL((USBH_CDC_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_CDC_InitCfg.MemSegPtr = p_mem_seg;
  USBH_CDC_InitCfg.MemSegBufPtr = p_mem_seg_buf;
}
#endif

/****************************************************************************************************//**
 *                                   USBH_CDC_ConfigureOptimizeSpdCfg()
 *
 * @brief    Sets the configurations required when optimize speed mode is enabled.
 *
 * @param    p_optimize_spd_cfg  Pointer to the structure containing the configurations for the
 *                               optimize speed mode.
 *
 * @note     (1) This function MUST be called before the CDC class is initialized via the
 *               USBH_CDC_Init() function.
 *
 * @note     (2) This function MUST be called when the USBH_CFG_OPTIMIZE_SPD_EN configuration is set
 *               to DEF_ENABLED.
 *******************************************************************************************************/

#if ((RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED) \
  && (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED))
void USBH_CDC_ConfigureOptimizeSpdCfg(const USBH_CDC_CFG_OPTIMIZE_SPD *p_optimize_spd_cfg)
{
  RTOS_ASSERT_CRITICAL((USBH_CDC_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_CDC_InitCfg.OptimizeSpd = *p_optimize_spd_cfg;
}
#endif

/****************************************************************************************************//**
 *                                       USBH_CDC_ConfigureInitAllocCfg()
 *
 * @brief    Sets the configurations required when allocation at initialization mode is enabled.
 *
 * @param    p_init_alloc_cfg    Pointer to the structure containing the configurations for the
 *                               allocation at initialization mode.
 *
 * @note     (1) This function MUST be called before the CDC class is initialized via the
 *               USBH_CDC_Init() function.
 *
 * @note     (2) This function MUST be called when the USBH_CFG_INIT_ALLOC_EN configuration is set
 *               to DEF_ENABLED.
 *******************************************************************************************************/

#if ((RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED) \
  && (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED))
void USBH_CDC_ConfigureInitAllocCfg(const USBH_CDC_CFG_INIT_ALLOC *p_init_alloc_cfg)
{
  RTOS_ASSERT_CRITICAL((USBH_CDC_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_CDC_InitCfg.InitAlloc = *p_init_alloc_cfg;
}
#endif

/****************************************************************************************************//**
 *                                               USBH_CDC_Init()
 *
 * @brief    Initializes the Communication Device Class (CDC) driver.
 *
 * @param    p_err   Pointer to the variable that will receive one of these return error codes
 *                   from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                       - RTOS_ERR_SEG_OVF
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_NOT_AVAIL
 *******************************************************************************************************/
void USBH_CDC_Init(RTOS_ERR *p_err)
{
  USBH_CDC *p_cdc;
  CORE_DECLARE_IRQ_STATE;

  //                                                               Validate configurations.
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  if (USBH_CDC_Ptr != DEF_NULL) {                               // Chk if CDC already initialized.
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    return;
  }

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  RTOS_ASSERT_DBG_ERR_SET((USBH_CDC_InitCfg.OptimizeSpd.FnctQty != 0u), *p_err, RTOS_ERR_NOT_READY,; );

  RTOS_ASSERT_DBG_ERR_SET(((USBH_CDC_InitCfg.OptimizeSpd.IF_PerFnctQty > 0u)
                           && (USBH_CDC_InitCfg.OptimizeSpd.FnctQty > 0u)), *p_err, RTOS_ERR_INVALID_CFG,; );
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  RTOS_ASSERT_DBG_ERR_SET((USBH_CDC_InitCfg.InitAlloc.FnctQty != 0u), *p_err, RTOS_ERR_NOT_READY,; );

  RTOS_ASSERT_DBG_ERR_SET(((USBH_CDC_InitCfg.InitAlloc.FnctQty > 0u)
                           && (USBH_CDC_InitCfg.InitAlloc.DCI_Qty > 0u)), *p_err, RTOS_ERR_INVALID_CFG,; );
#endif

#if ((USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED) \
  && (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED))
  RTOS_ASSERT_DBG_ERR_SET((USBH_CDC_InitCfg.OptimizeSpd.FnctQty == USBH_CDC_InitCfg.InitAlloc.FnctQty), *p_err, RTOS_ERR_INVALID_CFG,; );
#endif // ((USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED) &&
  //                                                               (USBH_CFG_INIT_ALLOC_EN   == DEF_ENABLED))

  //                                                               ---------- ALLOC CDC CLASS INTERNAL DATA -----------
  //                                                               Alloc cdc class root struct.
  p_cdc = (USBH_CDC *)Mem_SegAlloc("USBH - CDC root struct",
                                   USBH_CDC_InitCfg.MemSegPtr,
                                   sizeof(USBH_CDC),
                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_cdc->SubclassHeadPtr = DEF_NULL;
  p_cdc->StdReqTimeoutMs = USBH_CDC_STD_REQ_TIMEOUT_DFLT;
#if (USBH_CDC_CFG_NOTIFICATIONS_RX_EN == DEF_ENABLED)
  p_cdc->MaxBufLenNotifications = 0u;
#endif

  CORE_ENTER_ATOMIC();
  USBH_CDC_Ptr = p_cdc;
  CORE_EXIT_ATOMIC();

  Mem_DynPoolCreate("USBH - CDC async xfer pool",
                    &USBH_CDC_Ptr->AsyncXferPool,
                    USBH_CDC_InitCfg.MemSegPtr,
                    sizeof(USBH_CDC_ASYNC_XFER),
                    sizeof(CPU_ALIGN),
#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
                    USBH_CDC_InitCfg.InitAlloc.AsyncXferQty,
                    USBH_CDC_InitCfg.InitAlloc.AsyncXferQty,
#else
                    0u,
                    LIB_MEM_BLK_QTY_UNLIMITED,
#endif
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  Mem_DynPoolCreate("USBH - CDC DCI",
                    &USBH_CDC_Ptr->DCI_Pool,
                    USBH_CDC_InitCfg.MemSegPtr,
                    sizeof(USBH_CDC_DCI),
                    sizeof(CPU_ALIGN),
#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
                    USBH_CDC_InitCfg.InitAlloc.DCI_Qty,
                    USBH_CDC_InitCfg.InitAlloc.DCI_Qty,
#else
                    0u,
                    LIB_MEM_BLK_QTY_UNLIMITED,
#endif
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Alloc buffer for management element requests.
  USBH_CDC_Ptr->MgmtReqBufPtr = (CPU_INT08U *)Mem_SegAllocHW("USBH - CDC mgmt req buf",
                                                             USBH_CDC_InitCfg.MemSegBufPtr,
                                                             USBH_CDC_MGMT_REQ_BUF_LEN,
                                                             USBH_CDC_InitCfg.BufAlignOctets,
                                                             DEF_NULL,
                                                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Create lock to protect mgmt req buffer.
  USBH_CDC_Ptr->MgmtReqBufLock = KAL_LockCreate("USBH - CDC mgmt req buf lock",
                                                DEF_NULL,
                                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBH_ClassDrvReg(&USBH_CDC_Drv, p_err);                       // Register CDC class driver to core.
}

/****************************************************************************************************//**
 *                                       USBH_CDC_StdReqTimeoutSet()
 *
 * @brief    Assigns a new timeout delay for the CDC standard requests.
 *
 * @param    std_req_timeout_ms  New timeout, in milliseconds.
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error
 *                               code(s) from this function:
 *                                   - RTOS_ERR_NONE
 *******************************************************************************************************/
void USBH_CDC_StdReqTimeoutSet(CPU_INT32U std_req_timeout_ms,
                               RTOS_ERR   *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((USBH_CDC_Ptr != DEF_NULL), RTOS_ERR_NOT_INIT,; );

  USBH_CDC_Ptr->StdReqTimeoutMs = std_req_timeout_ms;
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           USBH_CDC_PostInit()
 *
 * @brief    Post-initializes the Communication Device Class (CDC) driver once all the subclass drivers added.
 *
 * @param    p_err   Pointer to the variable that will receive one of these return error codes
 *                   from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_SEG_OVF
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *
 * @note     (1) This function MUST be called by the application once all the subclass drivers are
 *               initialized and BEFORE starting the USB Host product.
 *******************************************************************************************************/
void USBH_CDC_PostInit(RTOS_ERR *p_err)
{
  CPU_INT08U class_fnct_qty;

  //                                                               Validate configurations.
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

#if (USBH_CDC_CFG_NOTIFICATIONS_RX_EN == DEF_ENABLED)
  {                                                             // Retrieve max len of required notification buffer ...
                                                                // ... among all subclass drivers.
    USBH_CDC_SUBCLASS *p_subclass = USBH_CDC_Ptr->SubclassHeadPtr;

    while (p_subclass != DEF_NULL) {
      if (USBH_CDC_Ptr->MaxBufLenNotifications < p_subclass->CDC_SubclassDrvPtr->BufLenNotifications) {
        USBH_CDC_Ptr->MaxBufLenNotifications = p_subclass->CDC_SubclassDrvPtr->BufLenNotifications;
      }

      p_subclass = p_subclass->NextPtr;
    }
  }
#endif

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  class_fnct_qty = USBH_CDC_InitCfg.OptimizeSpd.FnctQty;
#elif (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  class_fnct_qty = USBH_CDC_InitCfg.InitAlloc.FnctQty;
#else
  class_fnct_qty = USBH_CLASS_FNCT_QTY_UNLIMITED;
#endif

  USBH_ClassFnctPoolCreate("USBH - Class function pool",
                           &USBH_CDC_Ptr->FnctPool,
                           USBH_CDC_InitCfg.MemSegPtr,
                           sizeof(USBH_CDC_FNCT),
                           class_fnct_qty,
                           USBH_CDC_FnctAllocCallback,
                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}

/****************************************************************************************************//**
 *                                           USBH_CDC_DevHandleGet()
 *
 * @brief    Gets the device handle of a given CDC function.
 *
 * @param    cdc_fnct_handle     Handle to the CDC function.
 *
 * @param    p_err               Pointer to the variable that will receive one of these return error codes
 *                               from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_NOT_AVAIL
 *
 * @return   Device handle.
 *******************************************************************************************************/
USBH_DEV_HANDLE USBH_CDC_DevHandleGet(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                                      RTOS_ERR             *p_err)
{
  USBH_DEV_HANDLE dev_handle;
  USBH_CDC_FNCT   *p_cdc_fnct_data;
  CPU_INT32U      std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_CDC_Ptr->StdReqTimeoutMs;
  CORE_EXIT_ATOMIC();

  p_cdc_fnct_data = (USBH_CDC_FNCT *)USBH_ClassFnctAcquire(&USBH_CDC_Ptr->FnctPool,
                                                           (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                                                           DEF_NO,
                                                           std_req_timeout,
                                                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  dev_handle = p_cdc_fnct_data->DevHandle;

  USBH_ClassFnctRelease(&USBH_CDC_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                        DEF_NO);

  return (dev_handle);
}

/****************************************************************************************************//**
 *                                           USBH_CDC_FnctHandleGet()
 *
 * @brief    Gets the function handle of a given CDC function.
 *
 * @param    cdc_fnct_handle     Handle to the CDC function.
 *
 * @param    p_err               Pointer to the variable that will receive one of these return error codes
 *                               from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_NOT_AVAIL
 *
 * @return   Function handle.
 *******************************************************************************************************/
USBH_FNCT_HANDLE USBH_CDC_FnctHandleGet(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                                        RTOS_ERR             *p_err)
{
  USBH_FNCT_HANDLE fnct_handle;
  USBH_CDC_FNCT    *p_cdc_fnct_data;
  CPU_INT32U       std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_CDC_Ptr->StdReqTimeoutMs;
  CORE_EXIT_ATOMIC();

  p_cdc_fnct_data = (USBH_CDC_FNCT *)USBH_ClassFnctAcquire(&USBH_CDC_Ptr->FnctPool,
                                                           (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                                                           DEF_NO,
                                                           std_req_timeout,
                                                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  fnct_handle = p_cdc_fnct_data->FnctHandle;

  USBH_ClassFnctRelease(&USBH_CDC_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                        DEF_NO);

  return (fnct_handle);
}

/****************************************************************************************************//**
 *                                           USBH_CDC_DCI_QtyGet()
 *
 * @brief    Gets the quantity of Data Class Interface (DCI) for a given CDC function.
 *
 * @param    cdc_fnct_handle     Handle to CDC function.
 *
 * @param    p_err               Pointer to the variable that will receive one of these return error codes
 *                               from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_NOT_AVAIL
 *
 * @return   Number of DCI.
 *******************************************************************************************************/
CPU_INT08U USBH_CDC_DCI_QtyGet(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                               RTOS_ERR             *p_err)
{
  CPU_INT08U    dci_qty;
  USBH_CDC_FNCT *p_cdc_fnct_data;
  CPU_INT32U    std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_CDC_Ptr->StdReqTimeoutMs;
  CORE_EXIT_ATOMIC();

  p_cdc_fnct_data = (USBH_CDC_FNCT *)USBH_ClassFnctAcquire(&USBH_CDC_Ptr->FnctPool,
                                                           (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                                                           DEF_NO,
                                                           std_req_timeout,
                                                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  dci_qty = p_cdc_fnct_data->DCI_Qty;

  USBH_ClassFnctRelease(&USBH_CDC_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                        DEF_NO);

  return (dci_qty);
}

/****************************************************************************************************//**
 *                                           USBH_CDC_RelNbrGet()
 *
 * @brief    Gets the CDC function release number (bcdCDC).
 *
 * @param    cdc_fnct_handle     Handle to CDC function.
 *
 * @param    p_err               Pointer to the variable that will receive one of these return error codes
 *                               from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_NOT_AVAIL
 *
 * @return   Release number.
 *
 * @note     (1) For more information on CDC release number, see 'USB Class Definitions for
 *               Communication Devices Specification", version 1.2, Section 5.2.3.1'.
 *******************************************************************************************************/
CPU_INT16U USBH_CDC_RelNbrGet(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                              RTOS_ERR             *p_err)
{
  CPU_INT16U    rel_nbr;
  USBH_CDC_FNCT *p_cdc_fnct_data;
  CPU_INT32U    std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_CDC_Ptr->StdReqTimeoutMs;
  CORE_EXIT_ATOMIC();

  p_cdc_fnct_data = (USBH_CDC_FNCT *)USBH_ClassFnctAcquire(&USBH_CDC_Ptr->FnctPool,
                                                           (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                                                           DEF_NO,
                                                           std_req_timeout,
                                                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  rel_nbr = p_cdc_fnct_data->RelNbr;

  USBH_ClassFnctRelease(&USBH_CDC_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                        DEF_NO);

  return (rel_nbr);
}

/****************************************************************************************************//**
 *                                       USBH_CDC_EncapsulatedCmdTx()
 *
 * @brief    Sends the CDC encapsulated command.
 *
 * @param    cdc_fnct_handle     Handle to the CDC function.
 *
 * @param    p_buf               Pointer to the buffer that contains the command.
 *
 * @param    buf_len             Buffer length in octets.
 *
 * @param    timeout             Timeout in milliseconds.
 *
 * @param    p_err               Pointer to the variable that will receive one of these return error codes
 *                               from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_WOULD_BLOCK
 *                                   - RTOS_ERR_OS_SCHED_LOCKED
 *                                   - RTOS_ERR_IS_OWNER
 *                                   - RTOS_ERR_INVALID_STATE
 *                                   - RTOS_ERR_USB_URB_ALLOC
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_USB_INVALID_EP
 *                                   - RTOS_ERR_INVALID_ARG
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_NOT_AVAIL
 *                                   - RTOS_ERR_TX
 *                                   - RTOS_ERR_WOULD_OVF
 *
 * @return   Number of octets transferred.
 *
 * @note     (1) For more information on SendEncapsulatedCommand, see 'USB Class Definitions for
 *               Communication Devices Specification", version 1.2, Section 6.2.1'.
 *******************************************************************************************************/
CPU_INT16U USBH_CDC_EncapsulatedCmdTx(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                                      CPU_INT08U           *p_buf,
                                      CPU_INT16U           buf_len,
                                      CPU_INT32U           timeout,
                                      RTOS_ERR             *p_err)
{
  CPU_INT08U    if_nbr;
  CPU_INT16U    xfer_len = 0u;
  USBH_CDC_FNCT *p_cdc_fnct_data;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_cdc_fnct_data = (USBH_CDC_FNCT *)USBH_ClassFnctAcquire(&USBH_CDC_Ptr->FnctPool,
                                                           (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                                                           DEF_NO,
                                                           timeout,
                                                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if_nbr = USBH_IF_NbrGet(p_cdc_fnct_data->DevHandle,
                          p_cdc_fnct_data->FnctHandle,
                          0u,
                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  xfer_len = USBH_EP_CtrlXfer(p_cdc_fnct_data->DevHandle,
                              USBH_CDC_REQ_SEND_ENCAPSULATED_CMD,
                              (USBH_DEV_REQ_RECIPIENT_IF | USBH_DEV_REQ_TYPE_CLASS | USBH_DEV_REQ_DIR_HOST_TO_DEV),
                              0u,
                              if_nbr,
                              p_buf,
                              buf_len,
                              buf_len,
                              timeout,
                              p_err);

end_rel:
  USBH_ClassFnctRelease(&USBH_CDC_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                        DEF_NO);

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                       USBH_CDC_EncapsulatedRespRx()
 *
 * @brief    Send the CDC encapsulated command.
 *
 * @param    cdc_fnct_handle     Handle to the CDC function.
 *
 * @param    p_buf               Pointer to the buffer that will receive data.
 *
 * @param    buf_len             Buffer length in octets.
 *
 * @param    timeout             Timeout in milliseconds.
 *
 * @param    p_err               Pointer to the variable that will receive one of these return error codes
 *                               from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_WOULD_BLOCK
 *                                   - RTOS_ERR_OS_SCHED_LOCKED
 *                                   - RTOS_ERR_IS_OWNER
 *                                   - RTOS_ERR_INVALID_STATE
 *                                   - RTOS_ERR_USB_URB_ALLOC
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_USB_INVALID_EP
 *                                   - RTOS_ERR_INVALID_ARG
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_NOT_AVAIL
 *                                   - RTOS_ERR_TX
 *                                   - RTOS_ERR_WOULD_OVF
 *
 * @return   Number of octets received.
 *
 * @note     (1) For more information on GetEncapsulatedCommand, see 'USB Class Definitions for
 *               Communication Devices Specification", version 1.2, Section 6.2.2'.
 *******************************************************************************************************/
CPU_INT16U USBH_CDC_EncapsulatedRespRx(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                                       CPU_INT08U           *p_buf,
                                       CPU_INT16U           buf_len,
                                       CPU_INT32U           timeout,
                                       RTOS_ERR             *p_err)
{
  CPU_INT08U    if_nbr;
  CPU_INT16U    xfer_len = 0u;
  USBH_CDC_FNCT *p_cdc_fnct;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_cdc_fnct = (USBH_CDC_FNCT *)USBH_ClassFnctAcquire(&USBH_CDC_Ptr->FnctPool,
                                                      (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                                                      DEF_NO,
                                                      timeout,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if_nbr = USBH_IF_NbrGet(p_cdc_fnct->DevHandle,
                          p_cdc_fnct->FnctHandle,
                          0u,
                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  xfer_len = USBH_EP_CtrlXfer(p_cdc_fnct->DevHandle,
                              USBH_CDC_REQ_GET_ENCAPSULATED_RESP,
                              (USBH_DEV_REQ_RECIPIENT_IF | USBH_DEV_REQ_TYPE_CLASS | USBH_DEV_REQ_DIR_DEV_TO_HOST),
                              0u,
                              if_nbr,
                              p_buf,
                              buf_len,
                              buf_len,
                              timeout,
                              p_err);

end_rel:
  USBH_ClassFnctRelease(&USBH_CDC_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                        DEF_NO);

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                           USBH_CDC_CommFeatureSet()
 *
 * @brief    Configures the CDC communication feature.
 *
 * @param    cdc_fnct_handle     Handle to CDC function.
 *
 * @param    feature             Feature to configure, which includes:
 *                                   - USBH_CDC_COMM_FEATURE_ABSTRACT_STATE
 *                                   - USBH_CDC_COMM_FEATURE_COUNTRY_SETTING
 *
 * @param    data                Data of the configured communication feature request.
 *
 * @param    timeout             Timeout in milliseconds.
 *
 * @param    p_err               Pointer to the variable that will receive one of these return error codes
 *                               from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_WOULD_BLOCK
 *                                   - RTOS_ERR_OS_SCHED_LOCKED
 *                                   - RTOS_ERR_IS_OWNER
 *                                   - RTOS_ERR_INVALID_STATE
 *                                   - RTOS_ERR_USB_URB_ALLOC
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_USB_INVALID_EP
 *                                   - RTOS_ERR_INVALID_ARG
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_TX
 *                                   - RTOS_ERR_NOT_AVAIL
 *                                   - RTOS_ERR_WOULD_OVF
 *
 * @note     (1) For more information on SetCommFeature request, see 'Communication Class Subclass
 *               Specification for PSTN Devices, version 1.2, Section 6.3.1'.
 *******************************************************************************************************/
void USBH_CDC_CommFeatureSet(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                             CPU_INT08U           feature,
                             CPU_INT16U           data,
                             CPU_INT32U           timeout,
                             RTOS_ERR             *p_err)
{
  CPU_INT08U    if_nbr;
  CPU_INT08U    *p_mgmt_req_buf;
  USBH_CDC_FNCT *p_cdc_fnct;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  p_cdc_fnct = (USBH_CDC_FNCT *)USBH_ClassFnctAcquire(&USBH_CDC_Ptr->FnctPool,
                                                      (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                                                      DEF_NO,
                                                      timeout,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if_nbr = USBH_IF_NbrGet(p_cdc_fnct->DevHandle,
                          p_cdc_fnct->FnctHandle,
                          0u,
                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  USBH_CDC_MgmtReqBufLock(timeout, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  p_mgmt_req_buf = USBH_CDC_Ptr->MgmtReqBufPtr;
  *((CPU_INT16U *)&p_mgmt_req_buf[0u]) = MEM_VAL_GET_INT16U_LITTLE(&data);

  (void)USBH_EP_CtrlXfer(p_cdc_fnct->DevHandle,
                         USBH_CDC_REQ_SET_COMM_FEATURE,
                         (USBH_DEV_REQ_RECIPIENT_IF | USBH_DEV_REQ_TYPE_CLASS | USBH_DEV_REQ_DIR_HOST_TO_DEV),
                         feature,
                         if_nbr,
                         p_mgmt_req_buf,
                         USBH_CDC_REQ_LEN_SET_COMM_FEATURE,
                         USBH_CDC_REQ_LEN_SET_COMM_FEATURE,
                         timeout,
                         p_err);

  USBH_CDC_MgmtReqBufRelease();

end_rel:
  USBH_ClassFnctRelease(&USBH_CDC_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                           USBH_CDC_CommFeatureGet()
 *
 * @brief    Gets CDC function line coding.
 *
 * @param    cdc_fnct_handle     Handle to the CDC function.
 *
 * @param    feature             Feature to Get, which includes:
 *                                   - USBH_CDC_COMM_FEATURE_ABSTRACT_STATE
 *                                   - USBH_CDC_COMM_FEATURE_COUNTRY_SETTING
 *
 * @param    timeout             Timeout in milliseconds.
 *
 * @param    p_err               Pointer to the variable that will receive one of these return error codes
 *                               from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_WOULD_BLOCK
 *                                   - RTOS_ERR_OS_SCHED_LOCKED
 *                                   - RTOS_ERR_IS_OWNER
 *                                   - RTOS_ERR_INVALID_STATE
 *                                   - RTOS_ERR_USB_URB_ALLOC
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_USB_INVALID_EP
 *                                   - RTOS_ERR_INVALID_ARG
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_TX
 *                                   - RTOS_ERR_NOT_AVAIL
 *                                   - RTOS_ERR_WOULD_OVF
 *
 * @return   Data of Get communication feature request.
 *
 * @note     (1) For more information on GetCommFeature request, see 'Communication Class Subclass
 *               Specification for PSTN Devices, version 1.2, Section 6.3.2'.
 *******************************************************************************************************/
CPU_INT16U USBH_CDC_CommFeatureGet(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                                   CPU_INT08U           feature,
                                   CPU_INT32U           timeout,
                                   RTOS_ERR             *p_err)
{
  CPU_INT08U    if_nbr;
  CPU_INT08U    *p_mgmt_req_buf;
  CPU_INT16U    data = 0u;
  USBH_CDC_FNCT *p_cdc_fnct;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_cdc_fnct = (USBH_CDC_FNCT *)USBH_ClassFnctAcquire(&USBH_CDC_Ptr->FnctPool,
                                                      (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                                                      DEF_NO,
                                                      timeout,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if_nbr = USBH_IF_NbrGet(p_cdc_fnct->DevHandle,
                          p_cdc_fnct->FnctHandle,
                          0u,
                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  USBH_CDC_MgmtReqBufLock(timeout, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  p_mgmt_req_buf = USBH_CDC_Ptr->MgmtReqBufPtr;

  (void)USBH_EP_CtrlXfer(p_cdc_fnct->DevHandle,
                         USBH_CDC_REQ_GET_COMM_FEATURE,
                         (USBH_DEV_REQ_RECIPIENT_IF | USBH_DEV_REQ_TYPE_CLASS | USBH_DEV_REQ_DIR_DEV_TO_HOST),
                         feature,
                         if_nbr,
                         p_mgmt_req_buf,
                         USBH_CDC_REQ_LEN_GET_COMM_FEATURE,
                         USBH_CDC_REQ_LEN_GET_COMM_FEATURE,
                         timeout,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    data = MEM_VAL_GET_INT16U_LITTLE(&p_mgmt_req_buf[0u]);
  }

  USBH_CDC_MgmtReqBufRelease();

end_rel:
  USBH_ClassFnctRelease(&USBH_CDC_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                        DEF_NO);

  return (data);
}

/****************************************************************************************************//**
 *                                           USBH_CDC_CommFeatureClr()
 *
 * @brief    Clears the CDC function communication feature.
 *
 * @param    cdc_fnct_handle     Handle to CDC function.
 *
 * @param    feature             Feature to clear, which includes:
 *                                   - USBH_CDC_COMM_FEATURE_ABSTRACT_STATE
 *                                   - USBH_CDC_COMM_FEATURE_COUNTRY_SETTING
 *
 * @param    timeout             Timeout in milliseconds.
 *
 * @param    p_err               Pointer to the variable that will receive one of these return error codes
 *                               from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_WOULD_BLOCK
 *                                   - RTOS_ERR_OS_SCHED_LOCKED
 *                                   - RTOS_ERR_IS_OWNER
 *                                   - RTOS_ERR_INVALID_STATE
 *                                   - RTOS_ERR_USB_URB_ALLOC
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_USB_INVALID_EP
 *                                   - RTOS_ERR_INVALID_ARG
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_NOT_AVAIL
 *                                   - RTOS_ERR_TX
 *                                   - RTOS_ERR_WOULD_OVF
 *
 * @note     (1) For more information on ClearCommFeature request, see 'Communication Class Subclass
 *               Specification for PSTN Devices, version 1.2, Section 6.3.3'.
 *******************************************************************************************************/
void USBH_CDC_CommFeatureClr(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                             CPU_INT08U           feature,
                             CPU_INT32U           timeout,
                             RTOS_ERR             *p_err)
{
  CPU_INT08U    if_nbr;
  USBH_CDC_FNCT *p_cdc_fnct;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  p_cdc_fnct = (USBH_CDC_FNCT *)USBH_ClassFnctAcquire(&USBH_CDC_Ptr->FnctPool,
                                                      (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                                                      DEF_NO,
                                                      timeout,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if_nbr = USBH_IF_NbrGet(p_cdc_fnct->DevHandle,
                          p_cdc_fnct->FnctHandle,
                          0u,
                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  (void)USBH_EP_CtrlXfer(p_cdc_fnct->DevHandle,
                         USBH_CDC_REQ_CLR_COMM_FEATURE,
                         (USBH_DEV_REQ_RECIPIENT_IF | USBH_DEV_REQ_TYPE_CLASS | USBH_DEV_REQ_DIR_HOST_TO_DEV),
                         feature,
                         if_nbr,
                         DEF_NULL,
                         0u,
                         0u,
                         timeout,
                         p_err);

end_rel:
  USBH_ClassFnctRelease(&USBH_CDC_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                           USBH_CDC_LineCodingSet()
 *
 * @brief    Sets the CDC function line coding.
 *
 * @param    cdc_fnct_handle     Handle to the CDC function.
 *
 * @param    p_line_coding       Pointer to the structure that contains line coding to set.
 *
 * @param    timeout             Timeout in milliseconds.
 *
 * @param    p_err               Pointer to the variable that will receive one of these return error codes
 *                               from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_WOULD_BLOCK
 *                                   - RTOS_ERR_OS_SCHED_LOCKED
 *                                   - RTOS_ERR_IS_OWNER
 *                                   - RTOS_ERR_INVALID_STATE
 *                                   - RTOS_ERR_USB_URB_ALLOC
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_USB_INVALID_EP
 *                                   - RTOS_ERR_INVALID_ARG
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_TX
 *                                   - RTOS_ERR_NOT_AVAIL
 *                                   - RTOS_ERR_WOULD_OVF
 *
 * @note     (1) For more information on SetLineCoding command, see 'Communication Class Subclass
 *               Specification for PSTN Devices, version 1.2, Section 6.3.10'.
 *******************************************************************************************************/
void USBH_CDC_LineCodingSet(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                            USBH_CDC_LINECODING  *p_line_coding,
                            CPU_INT32U           timeout,
                            RTOS_ERR             *p_err)
{
  CPU_INT08U    if_nbr;
  CPU_INT08U    *p_mgmt_req_buf;
  USBH_CDC_FNCT *p_cdc_fnct;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_line_coding != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  p_cdc_fnct = (USBH_CDC_FNCT *)USBH_ClassFnctAcquire(&USBH_CDC_Ptr->FnctPool,
                                                      (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                                                      DEF_NO,
                                                      timeout,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if_nbr = USBH_IF_NbrGet(p_cdc_fnct->DevHandle,
                          p_cdc_fnct->FnctHandle,
                          0u,
                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  USBH_CDC_MgmtReqBufLock(timeout, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  p_mgmt_req_buf = USBH_CDC_Ptr->MgmtReqBufPtr;
  *((CPU_INT32U *)&p_mgmt_req_buf[0u]) = MEM_VAL_GET_INT32U_LITTLE(&p_line_coding->Rate);
  p_mgmt_req_buf[4u] = MEM_VAL_GET_INT08U_LITTLE(&p_line_coding->CharFmt);
  p_mgmt_req_buf[5u] = MEM_VAL_GET_INT08U_LITTLE(&p_line_coding->ParityType);
  p_mgmt_req_buf[6u] = MEM_VAL_GET_INT08U_LITTLE(&p_line_coding->DataBit);

  (void)USBH_EP_CtrlXfer(p_cdc_fnct->DevHandle,
                         USBH_CDC_REQ_SET_LINE_CODING,
                         (USBH_DEV_REQ_RECIPIENT_IF | USBH_DEV_REQ_TYPE_CLASS | USBH_DEV_REQ_DIR_HOST_TO_DEV),
                         0u,
                         if_nbr,
                         p_mgmt_req_buf,
                         USBH_CDC_REQ_LEN_SET_LINE_CODING,
                         USBH_CDC_REQ_LEN_SET_LINE_CODING,
                         timeout,
                         p_err);

  USBH_CDC_MgmtReqBufRelease();

end_rel:
  USBH_ClassFnctRelease(&USBH_CDC_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                           USBH_CDC_LineCodingGet()
 *
 * @brief    Gets the CDC function line coding.
 *
 * @param    cdc_fnct_handle     Handle to the CDC function.
 *
 * @param    p_line_coding       Pointer to the structure that will receive line coding.
 *
 * @param    timeout             Timeout in milliseconds.
 *
 * @param    p_err               Pointer to the variable that will receive one of these return error codes
 *                               from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_WOULD_BLOCK
 *                                   - RTOS_ERR_OS_SCHED_LOCKED
 *                                   - RTOS_ERR_IS_OWNER
 *                                   - RTOS_ERR_INVALID_STATE
 *                                   - RTOS_ERR_USB_URB_ALLOC
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_USB_INVALID_EP
 *                                   - RTOS_ERR_INVALID_ARG
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_TX
 *                                   - RTOS_ERR_NOT_AVAIL
 *                                   - RTOS_ERR_WOULD_OVF
 *
 * @note     (1) For more information on GetLineCoding command, see 'Communication Class Subclass
 *               Specification for PSTN Devices, version 1.2, Section 6.3.11'.
 *******************************************************************************************************/
void USBH_CDC_LineCodingGet(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                            USBH_CDC_LINECODING  *p_line_coding,
                            CPU_INT32U           timeout,
                            RTOS_ERR             *p_err)
{
  CPU_INT08U    if_nbr;
  CPU_INT08U    *p_mgmt_req_buf;
  USBH_CDC_FNCT *p_cdc_fnct;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_line_coding != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  p_cdc_fnct = (USBH_CDC_FNCT *)USBH_ClassFnctAcquire(&USBH_CDC_Ptr->FnctPool,
                                                      (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                                                      DEF_NO,
                                                      timeout,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if_nbr = USBH_IF_NbrGet(p_cdc_fnct->DevHandle,
                          p_cdc_fnct->FnctHandle,
                          0u,
                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  USBH_CDC_MgmtReqBufLock(timeout, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  p_mgmt_req_buf = USBH_CDC_Ptr->MgmtReqBufPtr;

  (void)USBH_EP_CtrlXfer(p_cdc_fnct->DevHandle,
                         USBH_CDC_REQ_GET_LINE_CODING,
                         (USBH_DEV_REQ_RECIPIENT_IF | USBH_DEV_REQ_TYPE_CLASS | USBH_DEV_REQ_DIR_DEV_TO_HOST),
                         0u,
                         if_nbr,
                         p_mgmt_req_buf,
                         USBH_CDC_REQ_LEN_GET_LINE_CODING,
                         USBH_CDC_REQ_LEN_GET_LINE_CODING,
                         timeout,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    p_line_coding->Rate = MEM_VAL_GET_INT32U_LITTLE(&p_mgmt_req_buf[0u]);
    p_line_coding->CharFmt = MEM_VAL_GET_INT08U_LITTLE(&p_mgmt_req_buf[4u]);
    p_line_coding->ParityType = MEM_VAL_GET_INT08U_LITTLE(&p_mgmt_req_buf[5u]);
    p_line_coding->DataBit = MEM_VAL_GET_INT08U_LITTLE(&p_mgmt_req_buf[6u]);
  }

  USBH_CDC_MgmtReqBufRelease();

end_rel:
  USBH_ClassFnctRelease(&USBH_CDC_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                       USBH_CDC_CtrlLineStateSet()
 *
 * @brief    Sets the CDC function control line state.
 *
 * @param    cdc_fnct_handle     Handle to the CDC function.
 *
 * @param    ctrl_signal         Control signal, which includes:
 *                                   - USBH_CDC_CTRL_LINE_STATE_CARRIER
 *                                   - USBH_CDC_CTRL_LINE_STATE_DTE
 *
 * @param    timeout             Timeout in milliseconds.
 *
 * @param    p_err               Pointer to the variable that will receive one of these return error codes
 *                               from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_WOULD_BLOCK
 *                                   - RTOS_ERR_OS_SCHED_LOCKED
 *                                   - RTOS_ERR_IS_OWNER
 *                                   - RTOS_ERR_INVALID_STATE
 *                                   - RTOS_ERR_USB_URB_ALLOC
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_USB_INVALID_EP
 *                                   - RTOS_ERR_INVALID_ARG
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_NOT_AVAIL
 *                                   - RTOS_ERR_TX
 *                                   - RTOS_ERR_WOULD_OVF
 *
 * @note     (1) For more information on SetControlLineState request, see 'Communication Class
 *               Subclass Specification for PSTN Devices, version 1.2, Section 6.3.12'.
 *******************************************************************************************************/
void USBH_CDC_CtrlLineStateSet(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                               CPU_INT16U           ctrl_signal,
                               CPU_INT32U           timeout,
                               RTOS_ERR             *p_err)
{
  CPU_INT08U    if_nbr;
  USBH_CDC_FNCT *p_cdc_fnct;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  p_cdc_fnct = (USBH_CDC_FNCT *)USBH_ClassFnctAcquire(&USBH_CDC_Ptr->FnctPool,
                                                      (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                                                      DEF_NO,
                                                      timeout,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if_nbr = USBH_IF_NbrGet(p_cdc_fnct->DevHandle,
                          p_cdc_fnct->FnctHandle,
                          0u,
                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  (void)USBH_EP_CtrlXfer(p_cdc_fnct->DevHandle,
                         USBH_CDC_REQ_SET_CTRL_LINESTATE,
                         (USBH_DEV_REQ_RECIPIENT_IF | USBH_DEV_REQ_TYPE_CLASS | USBH_DEV_REQ_DIR_HOST_TO_DEV),
                         ctrl_signal,
                         if_nbr,
                         DEF_NULL,
                         0u,
                         0u,
                         timeout,
                         p_err);

end_rel:
  USBH_ClassFnctRelease(&USBH_CDC_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                           USBH_CDC_BrkSend()
 *
 * @brief    Sends the break signal to CDC function.
 *
 * @param    cdc_fnct_handle     Handle to the CDC function.
 *
 * @param    dur                 Duration of break.
 *
 * @param    timeout             Timeout in milliseconds.
 *
 * @param    p_err               Pointer to the variable that will receive one of these return error codes
 *                               from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_WOULD_BLOCK
 *                                   - RTOS_ERR_OS_SCHED_LOCKED
 *                                   - RTOS_ERR_IS_OWNER
 *                                   - RTOS_ERR_INVALID_STATE
 *                                   - RTOS_ERR_USB_URB_ALLOC
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_USB_INVALID_EP
 *                                   - RTOS_ERR_INVALID_ARG
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_NOT_AVAIL
 *                                   - RTOS_ERR_TX
 *                                   - RTOS_ERR_WOULD_OVF
 *
 * @note     (1) For more information on SendBreak request, see 'Communication Class Subclass
 *               Specification for PSTN Devices, version 1.2, Section 6.3.13'.
 *******************************************************************************************************/
void USBH_CDC_BrkSend(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                      CPU_INT16U           dur,
                      CPU_INT32U           timeout,
                      RTOS_ERR             *p_err)
{
  CPU_INT08U    if_nbr;
  USBH_CDC_FNCT *p_cdc_fnct;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  p_cdc_fnct = (USBH_CDC_FNCT *)USBH_ClassFnctAcquire(&USBH_CDC_Ptr->FnctPool,
                                                      (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                                                      DEF_NO,
                                                      timeout,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if_nbr = USBH_IF_NbrGet(p_cdc_fnct->DevHandle,
                          p_cdc_fnct->FnctHandle,
                          0u,
                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  (void)USBH_EP_CtrlXfer(p_cdc_fnct->DevHandle,
                         USBH_CDC_REQ_SEND_BREAK,
                         (USBH_DEV_REQ_RECIPIENT_IF | USBH_DEV_REQ_TYPE_CLASS | USBH_DEV_REQ_DIR_HOST_TO_DEV),
                         dur,
                         if_nbr,
                         DEF_NULL,
                         0u,
                         0u,
                         timeout,
                         p_err);

end_rel:
  USBH_ClassFnctRelease(&USBH_CDC_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                        DEF_NO);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           CDC SUBCLASS FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBH_CDC_SubclassDrvReg()
 *
 * @brief    Registers a subclass driver to CDC base class.
 *
 * @param    p_subclass_drv      Pointer to the standard class driver structure for subclass.
 *
 * @param    p_cdc_subclass_drv  Pointer to the CDC specific subclass driver structure.
 *
 * @param    p_err               Pointer to the variable that will receive the return error code from
 *                               this function.
 *******************************************************************************************************/
void USBH_CDC_SubclassDrvReg(USBH_CLASS_DRV        *p_subclass_drv,
                             USBH_CDC_SUBCLASS_DRV *p_cdc_subclass_drv,
                             RTOS_ERR              *p_err)
{
  USBH_CDC_SUBCLASS *p_cdc_subclass;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  //                                                               Chk if CDC initialized.
  RTOS_ASSERT_DBG_ERR_SET((USBH_CDC_Ptr != DEF_NULL), *p_err, RTOS_ERR_NOT_READY,; );

  RTOS_ASSERT_DBG_ERR_SET((p_subclass_drv != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_cdc_subclass_drv != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((((p_subclass_drv->ProbeFnct == DEF_NULL)
                            ^ (p_cdc_subclass_drv->Probe == DEF_NULL))), *p_err, RTOS_ERR_INVALID_ARG,; );

  //                                                               Alloc CDC subclass.
  p_cdc_subclass = (USBH_CDC_SUBCLASS *)Mem_SegAlloc("USBH - CDC subclass",
                                                     USBH_CDC_InitCfg.MemSegPtr,
                                                     sizeof(USBH_CDC_SUBCLASS),
                                                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_cdc_subclass->SubclassDrvPtr = p_subclass_drv;
  p_cdc_subclass->CDC_SubclassDrvPtr = p_cdc_subclass_drv;

  CORE_ENTER_ATOMIC();                                          // Add CDC subclass to list.
  p_cdc_subclass->NextPtr = USBH_CDC_Ptr->SubclassHeadPtr;
  USBH_CDC_Ptr->SubclassHeadPtr = p_cdc_subclass;
  CORE_EXIT_ATOMIC();

  LOG_VRB(("New subclass driver added -> ", (s)p_subclass_drv->NamePtr));
}

/****************************************************************************************************//**
 *                                           USBH_CDC_DataRxAsync()
 *
 * @brief    Receives data on given Data Control Interface (DCI) asynchronously.
 *
 * @param    cdc_fnct_handle     Handle to CDC function.
 *
 * @param    dci_ix              Index of DCI.
 *
 * @param    p_buf               Pointer to the destination buffer to receive data.
 *
 * @param    buf_len             Buffer length in octets.
 *
 * @param    async_fnct          Function that will be invoked upon completion of receive operation.
 *
 * @param    p_async_arg         Pointer to the argument that will be passed as parameter of 'async_fnct'.
 *
 * @param    p_err               Pointer to the variable that will receive the return error code from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_INVALID_STATE
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_USB_URB_ALLOC
 *                                   - RTOS_ERR_USB_INVALID_EP
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_POOL_FULL
 *                                   - RTOS_ERR_INVALID_ARG
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_NOT_AVAIL
 *******************************************************************************************************/
void USBH_CDC_DCI_RxAsync(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                          CPU_INT08U           dci_ix,
                          CPU_INT08U           *p_buf,
                          CPU_INT32U           buf_len,
                          USBH_CDC_ASYNC_FNCT  async_fnct,
                          void                 *p_async_arg,
                          RTOS_ERR             *p_err)
{
  USBH_EP_HANDLE      ep_handle;
  USBH_CDC_FNCT       *p_cdc_fnct;
  USBH_CDC_DCI        *p_dci;
  USBH_CDC_ASYNC_XFER *p_async_xfer;
  CPU_INT32U          std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((async_fnct != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_CDC_Ptr->StdReqTimeoutMs;
  CORE_EXIT_ATOMIC();

  p_cdc_fnct = (USBH_CDC_FNCT *)USBH_ClassFnctAcquire(&USBH_CDC_Ptr->FnctPool,
                                                      (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                                                      DEF_NO,
                                                      std_req_timeout,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring CDC fnct in rx async", RTOS_ERR_LOG_ARG_GET(*p_err)));
    return;
  }

  p_dci = USBH_CDC_DCI_PtrGet(p_cdc_fnct, dci_ix, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  p_async_xfer = (USBH_CDC_ASYNC_XFER *)Mem_DynPoolBlkGet(&USBH_CDC_Ptr->AsyncXferPool,
                                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  p_async_xfer->CDC_FnctHandle = cdc_fnct_handle;
  p_async_xfer->DCI_Ix = dci_ix;
  p_async_xfer->AsyncFnct = async_fnct;
  p_async_xfer->ArgPtr = p_async_arg;

  ep_handle = p_dci->EP_HandleIn;
  USBH_EP_BulkRxAsync(p_cdc_fnct->DevHandle,
                      ep_handle,
                      p_buf,
                      buf_len,
                      USBH_CDC_DCI_XferCmpl,
                      (void *)p_async_xfer,
                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    Mem_DynPoolBlkFree(&USBH_CDC_Ptr->AsyncXferPool,
                       (void *)p_async_xfer,
                       p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Freeing async xfer -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    }
  }

end_rel:
  USBH_ClassFnctRelease(&USBH_CDC_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                           USBH_CDC_DCI_TxAsync()
 *
 * @brief    Sends data on given Data Control Interface (DCI) asynchronously.
 *
 * @param    cdc_fnct_handle     Handle to CDC function.
 *
 * @param    dci_ix              Index of DCI.
 *
 * @param    p_buf               Pointer to the buffer of data that will be transmitted.
 *
 * @param    buf_len             Buffer length in octets.
 *
 * @param    async_fnct          Function that will be invoked upon completion of transmit operation.
 *
 * @param    p_async_arg         Pointer to the argument that will be passed as parameter of 'async_fnct'.
 *
 * @param    p_err               Pointer to the variable that will receive the return error code from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_INVALID_STATE
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_USB_URB_ALLOC
 *                                   - RTOS_ERR_USB_INVALID_EP
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_POOL_FULL
 *                                   - RTOS_ERR_INVALID_ARG
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_NOT_AVAIL
 *******************************************************************************************************/
void USBH_CDC_DCI_TxAsync(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                          CPU_INT08U           dci_ix,
                          CPU_INT08U           *p_buf,
                          CPU_INT32U           buf_len,
                          USBH_CDC_ASYNC_FNCT  async_fnct,
                          void                 *p_async_arg,
                          RTOS_ERR             *p_err)
{
  USBH_EP_HANDLE      ep_handle;
  USBH_CDC_ASYNC_XFER *p_async_xfer;
  USBH_CDC_FNCT       *p_cdc_fnct;
  USBH_CDC_DCI        *p_dci;
  CPU_INT32U          std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((async_fnct != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_CDC_Ptr->StdReqTimeoutMs;
  CORE_EXIT_ATOMIC();

  p_cdc_fnct = (USBH_CDC_FNCT *)USBH_ClassFnctAcquire(&USBH_CDC_Ptr->FnctPool,
                                                      (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                                                      DEF_NO,
                                                      std_req_timeout,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_dci = USBH_CDC_DCI_PtrGet(p_cdc_fnct, dci_ix, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  p_async_xfer = (USBH_CDC_ASYNC_XFER *)Mem_DynPoolBlkGet(&USBH_CDC_Ptr->AsyncXferPool,
                                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  }

  p_async_xfer->CDC_FnctHandle = cdc_fnct_handle;
  p_async_xfer->DCI_Ix = dci_ix;
  p_async_xfer->AsyncFnct = async_fnct;
  p_async_xfer->ArgPtr = p_async_arg;

  ep_handle = p_dci->EP_HandleOut;
  USBH_EP_BulkTxAsync(p_cdc_fnct->DevHandle,
                      ep_handle,
                      p_buf,
                      buf_len,
                      USBH_CDC_DCI_XferCmpl,
                      (void *)p_async_xfer,
                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    Mem_DynPoolBlkFree(&USBH_CDC_Ptr->AsyncXferPool,
                       (void *)p_async_xfer,
                       p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Freeing async xfer -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    }
  }

end_rel:
  USBH_ClassFnctRelease(&USBH_CDC_Ptr->FnctPool,
                        (USBH_CLASS_FNCT_HANDLE)cdc_fnct_handle,
                        DEF_NO);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBH_CDC_ProbeDev()
 *
 * @brief    Determines if class driver can handle connected USB device.
 *
 * @param    dev_handle      Handle to device.
 *
 * @param    class_code      Function class code
 *
 * @param    pp_class_fnct   Pointer that will receive the internal structure of the function.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this
 *                           function.
 *
 * @return   DEF_OK,   if device should     be handled by this class driver.
 *           DEF_FAIL, if device should not be handled by this class driver.
 *
 * @note     (1) This function is called by the core to determine if a USB device can be supported
 *               by this class driver. At the moment of this call, the device is not ready for
 *               communication.
 *
 * @note     (2) This function will return DEF_OK as soon as the device class code is 'CDC Control'.
 *               The CDC function will be allocated in the USBH_CDC_ProbeFnct() function.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_CDC_ProbeDev(USBH_DEV_HANDLE dev_handle,
                                     CPU_INT08U      class_code,
                                     void            **pp_class_fnct,
                                     RTOS_ERR        *p_err)
{
  (void)&dev_handle;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  if (class_code != USBH_CLASS_CODE_CDC_CTRL) {
    return (DEF_FAIL);
  } else {
    *pp_class_fnct = DEF_NULL;                                  // Indicates to core that function has not been alloc.

    return (DEF_OK);
  }
}

/****************************************************************************************************//**
 *                                           USBH_CDC_ProbeFnct()
 *
 * @brief    Determines if class driver can handle connected USB function.
 *
 * @param    dev_handle      Handle to device.
 *
 * @param    fnct_handle     Handle to function.
 *
 * @param    class_code      Function class code
 *
 * @param    pp_class_fnct   Pointer that will receive the internal structure of the function.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this
 *                           function.
 *
 * @return   DEF_OK,   if function should     be handled by this class driver.
 *           DEF_FAIL, if function should not be handled by this class driver.
 *
 * @note     (1) This function is called by the core to determine if a USB device can be supported
 *               by this class driver. At the moment of this call, the device is not ready for
 *               communication.
 *
 * @note     (2) This function will return DEF_OK as soon as the device class code is 'CDC CTRL'. If
 *               any error happens (cannot allocate hub function, ...), p_err will be set accordingly.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_CDC_ProbeFnct(USBH_DEV_HANDLE  dev_handle,
                                      USBH_FNCT_HANDLE fnct_handle,
                                      CPU_INT08U       class_code,
                                      void             **pp_class_fnct,
                                      RTOS_ERR         *p_err)
{
  CPU_BOOLEAN            probe_ok;
  CPU_INT08U             subclass;
  CPU_INT08U             class_probe;
  CPU_INT08U             if_qty;
  CPU_INT08U             if_ix;
  CPU_INT08U             fnctl_desc_type;
  CPU_INT08U             fnctl_desc_subtype;
  CPU_INT08U             *p_extra_desc_buf;
  CPU_INT16U             desc_extra_len;
  void                   *p_subclass_fnct = DEF_NULL;
  void                   *p_subclass_arg = DEF_NULL;
  USBH_CLASS_FNCT_HANDLE class_fnct_handle;
  USBH_CDC_SUBCLASS      *p_subclass;
  USBH_CDC_FNCT          *p_cdc_fnct;

  if (class_code != USBH_CLASS_CODE_CDC_CTRL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    return (DEF_FAIL);
  } else {
    //                                                             Retrieve CCI subclass code.
    subclass = USBH_IF_SubclassGet(dev_handle,
                                   fnct_handle,
                                   0u,
                                   p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (DEF_OK);
    }

    //                                                             Retrieve (sub)class code to use to probe subclasses.
    if ((subclass == USBH_CDC_CTRL_SUBCLASS_CODE_DLCM)
        || (subclass == USBH_CDC_CTRL_SUBCLASS_CODE_ACM)
        || (subclass == USBH_CDC_CTRL_SUBCLASS_CODE_TCM)
        || (subclass == USBH_CDC_CTRL_SUBCLASS_CODE_MCCM)
        || (subclass == USBH_CDC_CTRL_SUBCLASS_CODE_CAPICM)
        || (subclass == USBH_CDC_CTRL_SUBCLASS_CODE_ENCM)
        || (subclass == USBH_CDC_CTRL_SUBCLASS_CODE_ATMNCM)
        || (subclass == USBH_CDC_CTRL_SUBCLASS_CODE_WHCM)
        || (subclass == USBH_CDC_CTRL_SUBCLASS_CODE_DEV_MGMT)
        || (subclass == USBH_CDC_CTRL_SUBCLASS_CODE_MDLM)
        || (subclass == USBH_CDC_CTRL_SUBCLASS_CODE_OBEX)
        || (subclass == USBH_CDC_CTRL_SUBCLASS_CODE_EEM)
        || (subclass == USBH_CDC_CTRL_SUBCLASS_CODE_NCM)
        || (subclass == USBH_CDC_CTRL_SUBCLASS_CODE_MBIM)
        || (subclass >= 0x80u)) {
      class_probe = subclass;
    } else {
      class_probe = USBH_IF_ClassGet(dev_handle,                // If not known subclass code, use class code from ...
                                     fnct_handle,               // ... 1st subinterface.
                                     1u,
                                     p_err);
      if ((class_probe == USBH_CLASS_CODE_CDC_CTRL)
          || (class_probe == USBH_CLASS_CODE_CDC_DATA)) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
        return (DEF_OK);
      }
    }

    //                                                             Probe subclass drv.
    p_subclass = USBH_CDC_Ptr->SubclassHeadPtr;
    probe_ok = DEF_FAIL;
    while ((probe_ok == DEF_FAIL)
           && (p_subclass != DEF_NULL)) {
      if (p_subclass->SubclassDrvPtr->ProbeFnct != DEF_NULL) {
        //                                                         Subclass is a standard USB class.
        probe_ok = p_subclass->SubclassDrvPtr->ProbeFnct(dev_handle,
                                                         fnct_handle,
                                                         class_probe,
                                                         &p_subclass_fnct,
                                                         p_err);
      } else {
        //                                                         Subclass is an official CDC subclass.
        probe_ok = p_subclass->CDC_SubclassDrvPtr->Probe(dev_handle,
                                                         fnct_handle,
                                                         class_probe,
                                                         &p_subclass_fnct,
                                                         p_err);

        p_subclass_arg = p_subclass_fnct;
      }

      if (probe_ok != DEF_OK) {
        p_subclass = p_subclass->NextPtr;
      }
    }

    if (probe_ok != DEF_OK) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_SUBCLASS_DRV_NOT_FOUND);
      return (DEF_OK);
    }

    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (DEF_OK);
    }

    LOG_VRB(("Subclass driver found -> ", (s)p_subclass->SubclassDrvPtr->NamePtr));

    class_fnct_handle = USBH_ClassFnctAlloc(&USBH_CDC_Ptr->FnctPool,
                                            dev_handle,
                                            p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_CDC_FNCT_ALLOC);

      if (p_subclass->SubclassDrvPtr->Disconn != DEF_NULL) {
        p_subclass->SubclassDrvPtr->Disconn(p_subclass_fnct);
      }

      return (DEF_OK);
    }

    p_cdc_fnct = (USBH_CDC_FNCT *)USBH_ClassFnctAcquire(&USBH_CDC_Ptr->FnctPool,
                                                        class_fnct_handle,
                                                        DEF_YES,
                                                        OP_LOCK_TIMEOUT_INFINITE,
                                                        p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto end_rel;
    }

    p_cdc_fnct->DevHandle = dev_handle;
    p_cdc_fnct->FnctHandle = fnct_handle;
    p_cdc_fnct->SubclassPtr = p_subclass;
    p_cdc_fnct->SubclassFnctPtr = p_subclass_fnct;
    p_cdc_fnct->SubclassArgPtr = p_subclass_arg;
    p_cdc_fnct->EventEP_Handle = USBH_EP_HANDLE_INVALID;
    p_cdc_fnct->EventEP_IsIntr = DEF_YES;
    p_cdc_fnct->DCI_Qty = 0u;

    *pp_class_fnct = (void *)(CPU_ADDR)class_fnct_handle;

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
    Mem_Clr((void *)p_cdc_fnct->DCI_PtrTbl,
            USBH_CDC_InitCfg.OptimizeSpd.IF_PerFnctQty * sizeof(USBH_CDC_DCI *));
#else
    p_cdc_fnct->DCI_HeadPtr = DEF_NULL;
#endif

    //                                                             Retrieve qty of interface in CDC function.
    if_qty = USBH_FnctIF_QtyGet(p_cdc_fnct->DevHandle,
                                p_cdc_fnct->FnctHandle,
                                p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto end_rel;
    }

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
    if_qty = DEF_MIN(if_qty, USBH_CDC_InitCfg.OptimizeSpd.IF_PerFnctQty);
#endif

    //                                                             Alloc DCIs.
    for (if_ix = 0u; if_ix < if_qty; if_ix++) {
      class_code = USBH_IF_ClassGet(p_cdc_fnct->DevHandle,
                                    p_cdc_fnct->FnctHandle,
                                    if_ix,
                                    p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto end_rel;
      }

      if (class_code == USBH_CLASS_CODE_CDC_DATA) {
        USBH_CDC_DCI *p_dci;

        p_dci = (USBH_CDC_DCI *)Mem_DynPoolBlkGet(&USBH_CDC_Ptr->DCI_Pool,
                                                  p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto end_rel;
        }

        p_dci->Ix = if_ix;
        p_dci->EP_HandleIn = USBH_EP_HANDLE_INVALID;
        p_dci->EP_HandleOut = USBH_EP_HANDLE_INVALID;

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
        p_cdc_fnct->DCI_PtrTbl[p_dci->Ix] = p_dci;
#else
        p_dci->NextPtr = p_cdc_fnct->DCI_HeadPtr;
        p_cdc_fnct->DCI_HeadPtr = p_dci;
#endif

        p_cdc_fnct->DCI_Qty++;
      }
    }

    //                                                             Parse fnctl desc. Only CCI should have fnctl desc.
    p_extra_desc_buf = USBH_IF_DescExtraGet(p_cdc_fnct->FnctHandle,
                                            0u,
                                            &desc_extra_len,
                                            p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto end_rel;
    }

    if (desc_extra_len < (USBH_DESC_LEN_IF + USBH_CDC_FNCTL_DESC_LEN_HDR)) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
      goto end_rel;
    }

    //                                                             First desc returned is always IF desc itself.
    fnctl_desc_type = MEM_VAL_GET_INT08U_LITTLE(&p_extra_desc_buf[USBH_DESC_LEN_IF + 1u]);
    fnctl_desc_subtype = MEM_VAL_GET_INT08U_LITTLE(&p_extra_desc_buf[USBH_DESC_LEN_IF + 2u]);

    if ((fnctl_desc_type != USBH_CDC_FNCTL_DESC_IF)             // First fnctl desc should be of subtype hdr.
        || (fnctl_desc_subtype != USBH_CDC_FNCTL_DESC_SUB_HDR)) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_DESC);
      goto end_rel;
    }

    p_cdc_fnct->RelNbr = MEM_VAL_GET_INT16U_LITTLE(&p_extra_desc_buf[USBH_DESC_LEN_IF + 3u]);

    USBH_ClassFnctRelease(&USBH_CDC_Ptr->FnctPool,
                          class_fnct_handle,
                          DEF_YES);

    return (DEF_OK);
  }

end_rel:
  if (class_fnct_handle != USBH_CLASS_FNCT_HANDLE_INVALID) {
    USBH_ClassFnctRelease(&USBH_CDC_Ptr->FnctPool,
                          class_fnct_handle,
                          DEF_YES);

    USBH_CDC_Disconn((void *)(CPU_ADDR)class_fnct_handle);
  }

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           USBH_CDC_EP_Probe()
 *
 * @brief    Probes class driver before opening endpoint to ensure class driver needs it.
 *
 * @param    p_class_fnct    Pointer to the internal structure of the function.
 *
 * @param    if_ix           Endpoint's interface index.
 *
 * @param    ep_type         Endpoint type.
 *                               - USBH_EP_TYPE_INTR
 *                               - USBH_EP_TYPE_BULK
 *                               - USBH_EP_TYPE_ISOC
 *
 * @param    ep_dir_in       Endpoint direction.
 *                               - DEF_YES     Direction IN
 *                               - DEF_NO      Direction OUT
 *
 * @return   DEF_YES, if endpoint is needed.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/

#if (USBH_CDC_CFG_NOTIFICATIONS_RX_EN == DEF_DISABLED)
static CPU_BOOLEAN USBH_CDC_EP_Probe(void        *p_class_fnct,
                                     CPU_INT08U  if_ix,
                                     CPU_INT08U  ep_type,
                                     CPU_BOOLEAN ep_dir_in)
{
  CPU_BOOLEAN            ep_needed = DEF_YES;
  CPU_INT08U             if_class;
  USBH_CDC_FNCT          *p_cdc_fnct_data;
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  RTOS_ERR               err_usbh;

  p_cdc_fnct_data = (USBH_CDC_FNCT *)USBH_ClassFnctAcquire(&USBH_CDC_Ptr->FnctPool,
                                                           class_fnct_handle,
                                                           DEF_NO,
                                                           OP_LOCK_TIMEOUT_INFINITE,
                                                           &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring CDC class fnct -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return (DEF_YES);
  }

  if_class = USBH_IF_ClassGet(p_cdc_fnct_data->DevHandle,
                              p_cdc_fnct_data->FnctHandle,
                              if_ix,
                              &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Retrieving IF class -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    goto end_rel;
  }

  if ((if_class == USBH_CLASS_CODE_CDC_CTRL)
      && (ep_type == USBH_EP_TYPE_INTR)
      && (ep_dir_in == DEF_YES)) {
    ep_needed = DEF_NO;
  }

end_rel:
  USBH_ClassFnctRelease(&USBH_CDC_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_NO);

  return (ep_needed);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_CDC_EP_Open()
 *
 * @brief    Endpoint opened by core on given function's interface.
 *
 * @param    p_class_fnct    Pointer to the internal structure of the function.
 *
 * @param    ep_handle       Handle to endpoint.
 *
 * @param    if_ix           Endpoint's interface index.
 *
 * @param    ep_type         Endpoint type.
 *                               - USBH_EP_TYPE_INTR
 *                               - USBH_EP_TYPE_BULK
 *                               - USBH_EP_TYPE_ISOC
 *
 * @param    ep_dir_in       Endpoint direction.
 *                               - DEF_YES     Direction IN
 *                               - DEF_NO      Direction OUT
 *
 * @note     (1) This function will be called by the core for each endpoint declared by the function
 *               via endpoint descriptors and that were successfully opened.
 *
 * @note     (2) This function will never be called for a given function if the Probe() returned
 *               DEF_FAIL and/or *p_err != RTOS_ERR_NONE.
 *******************************************************************************************************/
static void USBH_CDC_EP_Open(void           *p_class_fnct,
                             USBH_EP_HANDLE ep_handle,
                             CPU_INT08U     if_ix,
                             CPU_INT08U     ep_type,
                             CPU_BOOLEAN    ep_dir_in)
{
  CPU_INT08U             dci_ix;
  CPU_INT08U             if_class;
  USBH_CDC_FNCT          *p_cdc_fnct_data;
  USBH_CDC_DCI           *p_dci;
  USBH_CLASS_DRV         *p_subclass_drv;
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  RTOS_ERR               err_usbh;

  p_cdc_fnct_data = (USBH_CDC_FNCT *)USBH_ClassFnctAcquire(&USBH_CDC_Ptr->FnctPool,
                                                           class_fnct_handle,
                                                           DEF_YES,
                                                           OP_LOCK_TIMEOUT_INFINITE,
                                                           &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring CDC class fnct -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

  if_class = USBH_IF_ClassGet(p_cdc_fnct_data->DevHandle,
                              p_cdc_fnct_data->FnctHandle,
                              if_ix,
                              &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Retrieving IF class -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    goto end_rel;
  }

  //                                                               If not a CDC CCI or DCI, call subclass' driver.
  if ((if_class != USBH_CLASS_CODE_CDC_DATA)
      && (if_class != USBH_CLASS_CODE_CDC_CTRL)) {
    p_subclass_drv = p_cdc_fnct_data->SubclassPtr->SubclassDrvPtr;
    if (p_subclass_drv->EP_Open != DEF_NULL) {
      p_subclass_drv->EP_Open(p_cdc_fnct_data->SubclassFnctPtr,
                              ep_handle,
                              if_ix,
                              ep_type,
                              ep_dir_in);
    }

    goto end_rel;
  }

  switch (ep_type) {
    case USBH_EP_TYPE_INTR:
      if (ep_dir_in == DEF_YES) {
        p_cdc_fnct_data->EventEP_Handle = ep_handle;
        p_cdc_fnct_data->EventEP_IsIntr = DEF_YES;
      }
      break;

    case USBH_EP_TYPE_BULK:
      if (if_class == USBH_CLASS_CODE_CDC_CTRL) {
        p_cdc_fnct_data->EventEP_Handle = ep_handle;
        p_cdc_fnct_data->EventEP_IsIntr = DEF_NO;
        break;
      }
    /* Fallthrough */

    case USBH_EP_TYPE_ISOC:
      dci_ix = if_ix;
      p_dci = USBH_CDC_DCI_PtrGet(p_cdc_fnct_data, dci_ix, &err_usbh);
      if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
        LOG_ERR(("Cannot retrieve DCI for opened EP -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
      }

      if (ep_dir_in == DEF_YES) {
        p_dci->EP_HandleIn = ep_handle;
      } else {
        p_dci->EP_HandleOut = ep_handle;
      }

      p_dci->IsBulk = (ep_type == USBH_EP_TYPE_BULK) ? DEF_YES : DEF_NO;
      break;

    default:
      break;
  }

end_rel:
  USBH_ClassFnctRelease(&USBH_CDC_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);
}

/****************************************************************************************************//**
 *                                           USBH_CDC_EP_Close()
 *
 * @brief    Endpoint closed by core on given function's interface.
 *
 * @param    p_class_fnct    Pointer to the internal structure of the function.
 *
 * @param    ep_handle       Handle to endpoint.
 *
 * @param    if_ix           Endpoint's interface index.
 *
 * @note     (1) This function will be called by the core when a endpoint is closed.
 *               - (a) When a new alternate interface is selected.
 *
 * @note     (2) This function will not be called if an endpoint is closed in the following circumstances.
 *               The Disconn() function will be called instead.
 *               - (a) When the device is disconnected.
 *               - (b) When a new configuration is selected.
 *
 * @note     (3) This function will never be called for a given function if the Probe() returned
 *               DEF_FAIL and/or *p_err != RTOS_ERR_NONE.
 *******************************************************************************************************/
static void USBH_CDC_EP_Close(void           *p_class_fnct,
                              USBH_EP_HANDLE ep_handle,
                              CPU_INT08U     if_ix)
{
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  USBH_CDC_FNCT          *p_cdc_fnct;
  RTOS_ERR               err_usbh;

  p_cdc_fnct = (USBH_CDC_FNCT *)USBH_ClassFnctAcquire(&USBH_CDC_Ptr->FnctPool,
                                                      class_fnct_handle,
                                                      DEF_YES,
                                                      OP_LOCK_TIMEOUT_INFINITE,
                                                      &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring CDC class fnct -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

  if (ep_handle == p_cdc_fnct->EventEP_Handle) {
    p_cdc_fnct->EventEP_Handle = USBH_EP_HANDLE_INVALID;
  } else {
    CPU_INT08U if_class;

    if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
      goto end_rel;
    }

    if_class = USBH_IF_ClassGet(p_cdc_fnct->DevHandle,
                                p_cdc_fnct->FnctHandle,
                                if_ix,
                                &err_usbh);
    if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
      goto end_rel;
    }

    if ((if_class != USBH_CLASS_CODE_CDC_DATA)
        && (if_class != USBH_CLASS_CODE_CDC_CTRL)) {
      USBH_CLASS_DRV *p_subclass_drv = p_cdc_fnct->SubclassPtr->SubclassDrvPtr;

      if (p_subclass_drv->EP_Close != DEF_NULL) {
        p_subclass_drv->EP_Close(p_cdc_fnct->SubclassFnctPtr,
                                 ep_handle,
                                 if_ix);
      }
    }
  }

end_rel:
  USBH_ClassFnctRelease(&USBH_CDC_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);
}

/****************************************************************************************************//**
 *                                               USBH_CDC_Conn()
 *
 * @brief    Initializes CDC communication.
 *
 * @param    p_class_fnct    Pointer to the internal structure of the function.
 *
 * @note     (1) This function will be called by the core when all the endpoints have been successfully
 *               opened for a given function and after it issued the SetConfiguration request. The
 *               function is hence ready for communication.
 *
 * @note     (2) This function will never be called for a given function if the Probe() returned
 *               DEF_FAIL and/or *p_err != RTOS_ERR_NONE.
 *******************************************************************************************************/
static void USBH_CDC_Conn(void *p_class_fnct)
{
  USBH_CLASS_DRV         *p_subclass_drv;
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  USBH_CDC_SUBCLASS_DRV  *p_cdc_subclass_drv;
  USBH_CDC_FNCT          *p_cdc_fnct;
  RTOS_ERR               err_usbh;

  p_cdc_fnct = (USBH_CDC_FNCT *)USBH_ClassFnctAcquire(&USBH_CDC_Ptr->FnctPool,
                                                      class_fnct_handle,
                                                      DEF_NO,
                                                      OP_LOCK_TIMEOUT_INFINITE,
                                                      &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring CDC class fnct -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

  p_subclass_drv = p_cdc_fnct->SubclassPtr->SubclassDrvPtr;
  if (p_subclass_drv->Conn != DEF_NULL) {
    p_subclass_drv->Conn(p_cdc_fnct->SubclassFnctPtr);
  }

  p_cdc_subclass_drv = p_cdc_fnct->SubclassPtr->CDC_SubclassDrvPtr;
  if (p_cdc_subclass_drv->Conn != DEF_NULL) {
    p_cdc_subclass_drv->Conn(p_cdc_fnct->DevHandle,
                             p_cdc_fnct->FnctHandle,
                             (USBH_CDC_FNCT_HANDLE)class_fnct_handle,
                             &p_cdc_fnct->SubclassArgPtr);
  }

  USBH_ClassFnctRelease(&USBH_CDC_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                           USBH_CDC_IF_AltSet()
 *
 * @brief    Notifies CDC class that alternate interface was set.
 *
 * @param    p_class_fnct    Pointer to the internal structure of the function.
 *
 * @param    if_ix           Interface index that has been set.
 *******************************************************************************************************/
static void USBH_CDC_IF_AltSet(void       *p_class_fnct,
                               CPU_INT08U if_ix)
{
  USBH_CLASS_DRV         *p_subclass_drv;
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  USBH_CDC_FNCT          *p_cdc_fnct;
  RTOS_ERR               err_usbh;

  p_cdc_fnct = (USBH_CDC_FNCT *)USBH_ClassFnctAcquire(&USBH_CDC_Ptr->FnctPool,
                                                      class_fnct_handle,
                                                      DEF_NO,
                                                      OP_LOCK_TIMEOUT_INFINITE,
                                                      &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring CDC class fnct -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

  p_subclass_drv = p_cdc_fnct->SubclassPtr->SubclassDrvPtr;
  if (p_subclass_drv->IF_AltSet != DEF_NULL) {
    p_subclass_drv->IF_AltSet(p_cdc_fnct->SubclassFnctPtr,
                              if_ix);
  }

#if (USBH_CDC_CFG_NOTIFICATIONS_RX_EN == DEF_ENABLED)
  //                                                               Submit event buffers.
  if (p_cdc_fnct->EventEP_Handle != USBH_EP_HANDLE_INVALID) {
    CPU_INT08U if_class;

    if_class = USBH_IF_ClassGet(p_cdc_fnct->DevHandle,
                                p_cdc_fnct->FnctHandle,
                                if_ix,
                                &err_usbh);
    if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
      LOG_ERR(("Retrieving interface class -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
      goto end_rel;
    }

    if (if_class == USBH_CLASS_CODE_CDC_CTRL) {
      CPU_INT08U event_urb_ix;
      CPU_INT08U event_urb_qty = USBH_CDC_InitCfg.EventURB_Qty;

      for (event_urb_ix = 0u; event_urb_ix < event_urb_qty; event_urb_ix++) {
        void* p_arg = (void *)(CPU_ADDR)class_fnct_handle;

        if (p_cdc_fnct->EventEP_IsIntr == DEF_YES) {
          USBH_EP_IntrRxAsync(p_cdc_fnct->DevHandle,
                              p_cdc_fnct->EventEP_Handle,
                              p_cdc_fnct->EventBufPtrTbl[event_urb_ix],
                              p_cdc_fnct->SubclassPtr->CDC_SubclassDrvPtr->BufLenNotifications,
                              USBH_CDC_EventRxCmpl,
                              p_arg,
                              &err_usbh);
        } else {
          USBH_EP_BulkRxAsync(p_cdc_fnct->DevHandle,
                              p_cdc_fnct->EventEP_Handle,
                              p_cdc_fnct->EventBufPtrTbl[event_urb_ix],
                              p_cdc_fnct->SubclassPtr->CDC_SubclassDrvPtr->BufLenNotifications,
                              USBH_CDC_EventRxCmpl,
                              p_arg,
                              &err_usbh);
        }
        if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
          LOG_ERR(("While submitting CDC event URB -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
        }
      }
    }
  }

end_rel:
#endif

  USBH_ClassFnctRelease(&USBH_CDC_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                           USBH_CDC_Disconn()
 *
 * @brief    Disconnects CDC function.
 *
 * @param    p_class_fnct    Pointer to the internal structure of the function.
 *
 * @note     (1) This function will be called by the core in the following circumstances.
 *               - (a) The core is unable to open an endpoint for this function.
 *               - (b) An error occurs in the core before the SetConfiguration request is issued.
 *               - (c) The device has been disconnected.
 *               - (d) A new configuration has been selected.
 *
 * @note     (2) This function will never be called for a given function if the Probe() returned
 *               DEF_FAIL and/or *p_err != RTOS_ERR_NONE.
 *******************************************************************************************************/
static void USBH_CDC_Disconn(void *p_class_fnct)
{
  USBH_CLASS_DRV         *p_subclass_drv;
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  USBH_CDC_SUBCLASS_DRV  *p_cdc_subclass_drv;
  USBH_CDC_FNCT          *p_cdc_fnct = (USBH_CDC_FNCT *)p_class_fnct;
  USBH_CDC_DCI           *p_dci;
  RTOS_ERR               err;

  p_cdc_fnct = (USBH_CDC_FNCT *)USBH_ClassFnctAcquire(&USBH_CDC_Ptr->FnctPool,
                                                      class_fnct_handle,
                                                      DEF_YES,
                                                      OP_LOCK_TIMEOUT_INFINITE,
                                                      &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring CDC class fnct -> ", RTOS_ERR_LOG_ARG_GET(err)));
    return;
  }

  p_subclass_drv = p_cdc_fnct->SubclassPtr->SubclassDrvPtr;
  if (p_subclass_drv->Disconn != DEF_NULL) {
    p_subclass_drv->Disconn(p_cdc_fnct->SubclassFnctPtr);
  }

  p_cdc_subclass_drv = p_cdc_fnct->SubclassPtr->CDC_SubclassDrvPtr;
  if (p_cdc_subclass_drv->Disconn != DEF_NULL) {
    p_cdc_subclass_drv->Disconn(p_cdc_fnct->DevHandle,
                                p_cdc_fnct->SubclassArgPtr);
  }

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  {
    CPU_INT08U dci_ix;

    //                                                             Free DCIs.
    for (dci_ix = 0u; dci_ix < USBH_CDC_InitCfg.OptimizeSpd.IF_PerFnctQty; dci_ix++) {
      p_dci = USBH_CDC_DCI_PtrGet(p_cdc_fnct,
                                  dci_ix,
                                  &err);
      if (p_dci != DEF_NULL) {
        Mem_DynPoolBlkFree(&USBH_CDC_Ptr->DCI_Pool,
                           (void *)p_dci,
                           &err);
      }
    }
  }
#else
  {
    USBH_CDC_DCI *p_dci_next;

    //                                                             Free DCIs.
    p_dci = p_cdc_fnct->DCI_HeadPtr;
    while (p_dci != DEF_NULL) {
      p_dci_next = p_dci->NextPtr;

      Mem_DynPoolBlkFree(&USBH_CDC_Ptr->DCI_Pool,
                         (void *)p_dci,
                         &err);

      p_dci = p_dci_next;
    }
  }
#endif

  USBH_ClassFnctRelease(&USBH_CDC_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);

  USBH_ClassFnctFree(&USBH_CDC_Ptr->FnctPool,
                     class_fnct_handle,
                     &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Freeing class function ->", RTOS_ERR_LOG_ARG_GET(err)));
  }
}

/****************************************************************************************************//**
 *                                           USBH_CDC_Suspend()
 *
 * @brief    Puts CDC function in suspend state.
 *
 * @param    p_class_fnct    Pointer to the internal structure of the function.
 *
 * @note     (1) This function will never be called for a given function if the Probe() returned
 *               DEF_FAIL and/or *p_err != RTOS_ERR_NONE.
 *******************************************************************************************************/
static void USBH_CDC_Suspend(void *p_class_fnct)
{
  USBH_CLASS_DRV         *p_subclass_drv;
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  USBH_CDC_FNCT          *p_cdc_fnct;
  RTOS_ERR               err_usbh;

  p_cdc_fnct = (USBH_CDC_FNCT *)USBH_ClassFnctAcquire(&USBH_CDC_Ptr->FnctPool,
                                                      class_fnct_handle,
                                                      DEF_YES,
                                                      OP_LOCK_TIMEOUT_INFINITE,
                                                      &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring CDC class fnct -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

  p_subclass_drv = p_cdc_fnct->SubclassPtr->SubclassDrvPtr;
  if (p_subclass_drv->Suspend != DEF_NULL) {
    p_subclass_drv->Suspend(p_cdc_fnct->SubclassFnctPtr);
  }

  USBH_ClassFnctRelease(&USBH_CDC_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);

  LOG_VRB(("CDC Device suspended"));
}

/****************************************************************************************************//**
 *                                               USBH_CDC_Resume()
 *
 * @brief    Resumes CDC function.
 *
 * @param    p_class_fnct    Pointer to the internal structure of the function.
 *
 * @note     (1) This function will never be called for a given function if the Probe() returned
 *               DEF_FAIL and/or *p_err != RTOS_ERR_NONE.
 *******************************************************************************************************/
static void USBH_CDC_Resume(void *p_class_fnct)
{
  USBH_CLASS_DRV         *p_subclass_drv;
  USBH_CDC_FNCT          *p_cdc_fnct;
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  RTOS_ERR               err_usbh;

  p_cdc_fnct = (USBH_CDC_FNCT *)USBH_ClassFnctAcquire(&USBH_CDC_Ptr->FnctPool,
                                                      class_fnct_handle,
                                                      DEF_YES,
                                                      OP_LOCK_TIMEOUT_INFINITE,
                                                      &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring CDC class fnct -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

  p_subclass_drv = p_cdc_fnct->SubclassPtr->SubclassDrvPtr;
  if (p_subclass_drv->Resume != DEF_NULL) {
    p_subclass_drv->Resume(p_cdc_fnct->SubclassFnctPtr);
  }

  USBH_ClassFnctRelease(&USBH_CDC_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);

  LOG_VRB(("CDC Device resumed"));
}

/****************************************************************************************************//**
 *                                               USBH_CDC_UnInit()
 *
 * @brief    Un-initializes the CDC class, and all of its subclasses.
 *******************************************************************************************************/

#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
static void USBH_CDC_UnInit(void)
{
  USBH_CDC_SUBCLASS *p_cdc_subclass;
  RTOS_ERR          err_usbh;

  p_cdc_subclass = USBH_CDC_Ptr->SubclassHeadPtr;
  while (p_cdc_subclass != DEF_NULL) {
    if (p_cdc_subclass->SubclassDrvPtr->UnInit != DEF_NULL) {
      p_cdc_subclass->SubclassDrvPtr->UnInit();
    }
    p_cdc_subclass = p_cdc_subclass->NextPtr;
  }

  USBH_ClassFnctPoolDel(&USBH_CDC_Ptr->FnctPool,
                        &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Deleting CDC Fnct Pool -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
  }

  KAL_LockDel(USBH_CDC_Ptr->MgmtReqBufLock);

  USBH_CDC_Ptr = DEF_NULL;
}
#endif

/****************************************************************************************************//**
 *                                           USBH_CDC_TraceDump()
 *
 * @brief    Output class specific debug information on function.
 *
 * @param    p_class_fnct    Pointer to the internal structure of the function.
 *
 * @param    opt             Trace dump options.
 *
 * @param    trace_fnct      Function to call to output traces.
 *
 * @note     (1) This function will never be called for a given function if the Probe() returned
 *               DEF_FAIL and/or *p_err != RTOS_ERR_NONE.
 *******************************************************************************************************/

#ifdef RTOS_MODULE_COMMON_SHELL_AVAIL
static void USBH_CDC_TraceDump(void                *p_class_fnct,
                               CPU_INT32U          opt,
                               USBH_CMD_TRACE_FNCT trace_fnct)
{
  CPU_CHAR               str[5u];
  USBH_CLASS_DRV         *p_subclass_drv;
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  USBH_CDC_FNCT          *p_cdc_fnct;
  RTOS_ERR               err_usbh;

  (void)&opt;

  p_cdc_fnct = (USBH_CDC_FNCT *)USBH_ClassFnctAcquire(&USBH_CDC_Ptr->FnctPool,
                                                      class_fnct_handle,
                                                      DEF_NO,
                                                      USBH_CDC_Ptr->StdReqTimeoutMs,
                                                      &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring CDC class fnct -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

  trace_fnct("    ---------- USB Host (CDC specific debug information) -----------\r\n");
  trace_fnct("    | Nbr DCI | Rel nbr | Has notif EP | Subclass driver           |\r\n");
  trace_fnct("    | ");

  //                                                               Output nbr DCI.
  (void)Str_FmtNbr_Int32U(p_cdc_fnct->DCI_Qty,
                          3u,
                          DEF_NBR_BASE_DEC,
                          ' ',
                          DEF_NO,
                          DEF_YES,
                          &str[0u]);
  trace_fnct(str);
  trace_fnct("     | ");

  //                                                               Output release number.
  (void)Str_FmtNbr_Int32U(p_cdc_fnct->RelNbr,
                          5u,
                          DEF_NBR_BASE_DEC,
                          ' ',
                          DEF_NO,
                          DEF_YES,
                          &str[0u]);
  trace_fnct(str);
  trace_fnct("   | ");

  //                                                               Output notification EP type.
  if (p_cdc_fnct->EventEP_Handle != USBH_EP_HANDLE_INVALID) {
    if (p_cdc_fnct->EventEP_IsIntr == DEF_YES) {
      trace_fnct("YES (INTR)   | ");
    } else {
      trace_fnct("YES (BULK)   | ");
    }
  } else {
    trace_fnct("NO           | ");
  }

  //                                                               Output subclass driver name.
  if (p_cdc_fnct->SubclassPtr->SubclassDrvPtr != DEF_NULL) {
    trace_fnct(p_cdc_fnct->SubclassPtr->SubclassDrvPtr->NamePtr);
    trace_fnct("\r\n");
  } else {
    trace_fnct("NONE\r\n");
  }

  p_subclass_drv = p_cdc_fnct->SubclassPtr->SubclassDrvPtr;
  if (p_subclass_drv->TraceDump != DEF_NULL) {
    p_subclass_drv->TraceDump(p_cdc_fnct->SubclassFnctPtr,
                              opt,
                              trace_fnct);
  }

  USBH_ClassFnctRelease(&USBH_CDC_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_NO);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_CDC_EventRxCmpl()
 *
 * @brief    Asynchronous event reception completion function.
 *
 * @param    dev_handle  Handle to device.
 *
 * @param    ep_handle   Handle to endpoint.
 *
 * @param    p_buf       Pointer to the buffer containing received data.
 *
 * @param    buf_len     Buffer length in bytes.
 *
 * @param    xfer_len    Number of bytes received.
 *
 * @param    p_arg       Asynchronous function argument.
 *
 * @param    err         Status of transaction.
 *
 * @note     (1) This function will parse received notification and will call appropriate callback.
 *******************************************************************************************************/

#if (USBH_CDC_CFG_NOTIFICATIONS_RX_EN == DEF_ENABLED)
static void USBH_CDC_EventRxCmpl(USBH_DEV_HANDLE dev_handle,
                                 USBH_EP_HANDLE  ep_handle,
                                 CPU_INT08U      *p_buf,
                                 CPU_INT32U      buf_len,
                                 CPU_INT32U      xfer_len,
                                 void            *p_arg,
                                 RTOS_ERR        err)
{
  USBH_CDC_FNCT          *p_cdc_fnct;
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_arg;
  RTOS_ERR               err_local;
  CPU_INT32U             std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_CDC_Ptr->StdReqTimeoutMs;
  CORE_EXIT_ATOMIC();

  p_cdc_fnct = (USBH_CDC_FNCT *)USBH_ClassFnctAcquire(&USBH_CDC_Ptr->FnctPool,
                                                      class_fnct_handle,
                                                      DEF_NO,
                                                      std_req_timeout,
                                                      &err_local);
  if (RTOS_ERR_CODE_GET(err_local) != RTOS_ERR_NONE) {
    if (RTOS_ERR_CODE_GET(err_local) != RTOS_ERR_INVALID_HANDLE) {
      LOG_ERR(("Acquiring class function -> ", RTOS_ERR_LOG_ARG_GET(err_local)));
    }

    return;
  }

  if ((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE)
      && (xfer_len >= 8u)) {
    CPU_INT08U            notification;
    CPU_INT16U            val;
    CPU_INT16U            len;
    USBH_CDC_SUBCLASS_DRV *p_cdc_subclass_drv;

    //                                                             --------- PARSE AND DISPATCH NOTIFICATION ----------
    notification = MEM_VAL_GET_INT08U_LITTLE(&p_buf[1u]);
    val = MEM_VAL_GET_INT16U_LITTLE(&p_buf[2u]);
    len = MEM_VAL_GET_INT16U_LITTLE(&p_buf[6u]);
    p_cdc_subclass_drv = p_cdc_fnct->SubclassPtr->CDC_SubclassDrvPtr;

    switch (notification) {
      case USBH_CDC_NOTIFICATION_NET_CONN:
        if (p_cdc_subclass_drv->NetConn != DEF_NULL) {
          p_cdc_subclass_drv->NetConn(class_fnct_handle,
                                      p_cdc_fnct->SubclassArgPtr,
                                      (val == 1u) ? DEF_YES : DEF_NO);
        }
        break;

      case USBH_CDC_NOTIFICATION_RESP_AVAIL:
        if (p_cdc_subclass_drv->RespAvail != DEF_NULL) {
          p_cdc_subclass_drv->RespAvail(class_fnct_handle,
                                        p_cdc_fnct->SubclassArgPtr);
        }
        break;

      case USBH_CDC_NOTIFICATION_AUX_JACK_HOOK_STATE:
        if (p_cdc_subclass_drv->AuxJackHookState != DEF_NULL) {
          p_cdc_subclass_drv->AuxJackHookState(class_fnct_handle,
                                               p_cdc_fnct->SubclassArgPtr,
                                               (val == 1u) ? DEF_YES : DEF_NO);
        }
        break;

      case USBH_CDC_NOTIFICATION_RING_DETECT:
        if (p_cdc_subclass_drv->RingDetect != DEF_NULL) {
          p_cdc_subclass_drv->RingDetect(class_fnct_handle,
                                         p_cdc_fnct->SubclassArgPtr);
        }
        break;

      case USBH_CDC_NOTIFICATION_SERIAL_STATE:
        if (len != 2u) {
          LOG_ERR(("Invalid serial state received -> ", RTOS_ERR_CODE_LOG_ARG_GET(RTOS_ERR_INVALID_DESC)));
          break;
        }

        if (p_cdc_subclass_drv->SerialState != DEF_NULL) {
          p_cdc_subclass_drv->SerialState(class_fnct_handle,
                                          p_cdc_fnct->SubclassArgPtr,
                                          MEM_VAL_GET_INT08U_LITTLE(&p_buf[USBH_CDC_LEN_NOTIFICATION_HDR]));
        }
        break;

      case USBH_CDC_NOTIFICATION_CALL_STATE_CHNG:
        if (p_cdc_subclass_drv->CallStateChng != DEF_NULL) {
          p_cdc_subclass_drv->CallStateChng(class_fnct_handle,
                                            p_cdc_fnct->SubclassArgPtr,
                                            (CPU_INT08U)(val >> 8u),
                                            (CPU_INT08U) val,
                                            &p_buf[USBH_CDC_LEN_NOTIFICATION_HDR],
                                            len);
        }
        break;

      case USBH_CDC_NOTIFICATION_LINE_STATE_CHNG:
        if (p_cdc_subclass_drv->LineStateChng != DEF_NULL) {
          p_cdc_subclass_drv->LineStateChng(class_fnct_handle,
                                            p_cdc_fnct->SubclassArgPtr,
                                            val,
                                            &p_buf[USBH_CDC_LEN_NOTIFICATION_HDR],
                                            len);
        }
        break;

      case USBH_CDC_NOTIFICATION_CONN_SPD_CHNG:
        if (p_cdc_subclass_drv->ConnSpdChng != DEF_NULL) {
          p_cdc_subclass_drv->ConnSpdChng(class_fnct_handle,
                                          p_cdc_fnct->SubclassArgPtr,
                                          MEM_VAL_GET_INT32U_LITTLE(&p_buf[USBH_CDC_LEN_NOTIFICATION_HDR]),
                                          MEM_VAL_GET_INT32U_LITTLE(&p_buf[USBH_CDC_LEN_NOTIFICATION_HDR + 4u]));
        }
        break;

      default:
        LOG_DBG(("!! WARNING !! -> Unknown CDC notification received"));
        break;
    }
  } else if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_ABORT) {
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      LOG_ERR(("In CDC event rx -> ", RTOS_ERR_LOG_ARG_GET(err)));
    } else {
      LOG_ERR(("In CDC event rx -> ", RTOS_ERR_CODE_LOG_ARG_GET(RTOS_ERR_INVALID_DESC)));
    }
  }

  //                                                               ------------------ RE-SUBMIT URB -------------------
  if (p_cdc_fnct->EventEP_Handle != USBH_EP_HANDLE_INVALID) {
    if (p_cdc_fnct->EventEP_IsIntr == DEF_YES) {
      USBH_EP_IntrRxAsync(dev_handle,
                          ep_handle,
                          p_buf,
                          buf_len,
                          USBH_CDC_EventRxCmpl,
                          (void *)(CPU_ADDR)class_fnct_handle,
                          &err_local);
    } else {
      USBH_EP_BulkRxAsync(dev_handle,
                          ep_handle,
                          p_buf,
                          buf_len,
                          USBH_CDC_EventRxCmpl,
                          (void *)(CPU_ADDR)class_fnct_handle,
                          &err_local);
    }
    if (RTOS_ERR_CODE_GET(err_local) != RTOS_ERR_NONE) {
      LOG_ERR(("While submitting CDC event URB -> ", RTOS_ERR_LOG_ARG_GET(err_local)));
    }
  }

  USBH_ClassFnctRelease(&USBH_CDC_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_NO);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_CDC_DCI_XferCmpl()
 *
 * @brief    Asynchronous data transfer completion function.
 *
 * @param    dev_handle  Handle to device.
 *
 * @param    ep_handle   Handle to endpoint.
 *
 * @param    p_buf       Pointer to the transmitted buffer.
 *
 * @param    buf_len     Buffer length in bytes.
 *
 * @param    xfer_len    Number of bytes transferred.
 *
 * @param    p_arg       Asynchronous function argument.
 *
 * @param    err         Status of transaction.
 *******************************************************************************************************/
static void USBH_CDC_DCI_XferCmpl(USBH_DEV_HANDLE dev_handle,
                                  USBH_EP_HANDLE  ep_handle,
                                  CPU_INT08U      *p_buf,
                                  CPU_INT32U      buf_len,
                                  CPU_INT32U      xfer_len,
                                  void            *p_arg,
                                  RTOS_ERR        err)
{
  CPU_INT08U           dci_ix;
  void                 *p_async_arg;
  RTOS_ERR             err_local;
  USBH_CDC_ASYNC_FNCT  async_fnct;
  USBH_CDC_FNCT_HANDLE cdc_fnct_handle;
  USBH_CDC_ASYNC_XFER  *p_async_xfer = (USBH_CDC_ASYNC_XFER *)p_arg;

  (void)&dev_handle;
  (void)&ep_handle;

  cdc_fnct_handle = p_async_xfer->CDC_FnctHandle;
  dci_ix = p_async_xfer->DCI_Ix;
  async_fnct = p_async_xfer->AsyncFnct;
  p_async_arg = p_async_xfer->ArgPtr;

  Mem_DynPoolBlkFree(&USBH_CDC_Ptr->AsyncXferPool,
                     (void *)p_async_xfer,
                     &err_local);
  if (RTOS_ERR_CODE_GET(err_local) != RTOS_ERR_NONE) {
    LOG_ERR(("Freeing async xfer -> ", RTOS_ERR_LOG_ARG_GET(err_local)));
  }

  async_fnct(cdc_fnct_handle,
             dci_ix,
             p_buf,
             buf_len,
             xfer_len,
             p_async_arg,
             err);
}

/****************************************************************************************************//**
 *                                           USBH_CDC_DCI_PtrGet()
 *
 * @brief    Gets a pointer to given CDC DCI structure.
 *
 * @param    p_cdc_fnct  Pointer to the CDC function.
 *
 * @param    dci_ix      Index of Data Control Interface (DCI).
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this
 *                       function.
 *
 * @return   Pointer to the CDC DCI structure, if successful.
 *           0,                            otherwise.
 *******************************************************************************************************/
static USBH_CDC_DCI *USBH_CDC_DCI_PtrGet(USBH_CDC_FNCT *p_cdc_fnct,
                                         CPU_INT08U    dci_ix,
                                         RTOS_ERR      *p_err)
{
  USBH_CDC_DCI *p_dci;

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  RTOS_ASSERT_DBG_ERR_SET((dci_ix < USBH_CDC_InitCfg.OptimizeSpd.IF_PerFnctQty), *p_err, RTOS_ERR_INVALID_ARG, DEF_NULL);

  p_dci = p_cdc_fnct->DCI_PtrTbl[dci_ix];
#else
  p_dci = p_cdc_fnct->DCI_HeadPtr;
  while ((p_dci != DEF_NULL)
         && (p_dci->Ix != dci_ix)) {
    p_dci = p_dci->NextPtr;
  }
#endif

  if (p_dci != DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
  }

  return (p_dci);
}

/****************************************************************************************************//**
 *                                           USBH_CDC_MgmtReqBufLock()
 *
 * @brief    Locks buffer used for management requests.
 *
 * @param    timeout     Timeout in milliseconds.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void USBH_CDC_MgmtReqBufLock(CPU_INT32U timeout,
                                    RTOS_ERR   *p_err)
{
  KAL_LockAcquire(USBH_CDC_Ptr->MgmtReqBufLock,
                  KAL_OPT_PEND_NONE,
                  timeout,
                  p_err);
}

/****************************************************************************************************//**
 *                                       USBH_CDC_MgmtReqBufRelease()
 *
 * @brief    Releases buffer used for management requests.
 *******************************************************************************************************/
static void USBH_CDC_MgmtReqBufRelease(void)
{
  RTOS_ERR err;

  KAL_LockRelease(USBH_CDC_Ptr->MgmtReqBufLock, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Mgmt buf lock release -> ", RTOS_ERR_LOG_ARG_GET(err)));
  }
}

/****************************************************************************************************//**
 *                                       USBH_CDC_FnctAllocCallback()
 *
 * @brief    Function called when a new CDC function is allocated.
 *
 * @param    p_class_fnct_data   Pointer to the class function data.
 *
 * @return   DEF_OK,   if class function extra allocation(s) are successful.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_CDC_FnctAllocCallback(void *p_class_fnct_data)
{
  USBH_CDC_FNCT *p_cdc_fnct = (USBH_CDC_FNCT *)p_class_fnct_data;

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  {
    //                                                             Alloc DCI ptr table.
    RTOS_ERR err;

    p_cdc_fnct->DCI_PtrTbl = (USBH_CDC_DCI **)Mem_SegAlloc("USBH - CDC fnct DCI table",
                                                           USBH_CDC_InitCfg.MemSegPtr,
                                                           (USBH_CDC_InitCfg.OptimizeSpd.IF_PerFnctQty * sizeof(USBH_CDC_DCI *)),
                                                           &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      LOG_ERR(("Allocating DCI ptr tbl -> ", RTOS_ERR_LOG_ARG_GET(err)));
      return (DEF_FAIL);
    }
  }
#else
  p_cdc_fnct->DCI_HeadPtr = DEF_NULL;
#endif

#if (USBH_CDC_CFG_NOTIFICATIONS_RX_EN == DEF_ENABLED)
  if (USBH_CDC_Ptr->MaxBufLenNotifications > 0u) {
    CPU_INT08U event_urb_ix;
    RTOS_ERR   err;

    //                                                             Alloc event buf table.
    p_cdc_fnct->EventBufPtrTbl = (CPU_INT08U **)Mem_SegAlloc("USBH - CDC fnct event buf ptr tbl",
                                                             USBH_CDC_InitCfg.MemSegPtr,
                                                             (USBH_CDC_InitCfg.EventURB_Qty * sizeof(CPU_INT08U *)),
                                                             &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      LOG_ERR(("Allocating event buf ptr tbl -> ", RTOS_ERR_LOG_ARG_GET(err)));
      return (DEF_FAIL);
    }

    for (event_urb_ix = 0u; event_urb_ix < USBH_CDC_InitCfg.EventURB_Qty; event_urb_ix++) {
      p_cdc_fnct->EventBufPtrTbl[event_urb_ix] = (CPU_INT08U *)Mem_SegAllocHW("USBH - CDC fnct event buf",
                                                                              USBH_CDC_InitCfg.MemSegBufPtr,
                                                                              USBH_CDC_Ptr->MaxBufLenNotifications,
                                                                              USBH_CDC_InitCfg.BufAlignOctets,
                                                                              DEF_NULL,
                                                                              &err);
      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
        LOG_ERR(("Allocating event buffer -> ", RTOS_ERR_LOG_ARG_GET(err)));
        return (DEF_FAIL);
      }
    }
  } else {
    p_cdc_fnct->EventBufPtrTbl = DEF_NULL;
  }
#endif

  return (DEF_OK);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_HOST_CDC_AVAIL))
