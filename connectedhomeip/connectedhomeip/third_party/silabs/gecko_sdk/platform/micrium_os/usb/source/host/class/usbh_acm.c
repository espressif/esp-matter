/***************************************************************************//**
 * @file
 * @brief USB Host - Communications Device Class (Cdc) - Abstract Control Model (Acm)
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

#if (defined(RTOS_MODULE_USB_HOST_ACM_AVAIL))

#if (!defined(RTOS_MODULE_USB_HOST_CDC_AVAIL))

#error USB HOST ACM subclass requires USB Host CDC class. Make sure it is part of your project and that \
  RTOS_MODULE_USB_HOST_CDC_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  <cpu/include/cpu.h>

#include  <common/include/lib_mem.h>
#include  <common/source/op_lock/op_lock_priv.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>
#include  <rtos_description.h>

#include  <usb/include/host/usbh_cdc.h>
#include  <usb/include/host/usbh_cdc.h>
#include  <usb/include/host/usbh_acm.h>
#include  <usb/include/host/usbh_core_if.h>

#include  <usb/source/host/cmd/usbh_cmd_priv.h>
#include  <usb/source/host/class/usbh_class_common_priv.h>
#include  <usb/source/host/class/usbh_cdc_subclass_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (USBH, CLASS, CDC, ACM)
#define  RTOS_MODULE_CUR                                 RTOS_CFG_MODULE_USBH

#define  USBH_ACM_FNCT_HANDLE_CREATE(acm_ix)            ((USBH_ACM_FNCT_HANDLE)(acm_ix))
#define  USBH_ACM_FNCT_HANDLE_IX_GET(acm_fnct_handle)   ((CPU_INT08U)(acm_fnct_handle))

#define  USBH_ACM_DCI_IX                                 1u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       DEFAULT CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const USBH_ACM_INIT_CFG USBH_ACM_InitCfgDflt = {
  .MemSegPtr = DEF_NULL,

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  .OptimizeSpd = { .FnctQty = 1u },
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  .InitAlloc = { .FnctQty = 1u, .AsyncXferQty = 2u }
#endif
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           CDC ACM ASYNC XFER DATA
 *******************************************************************************************************/

typedef struct usbh_acm_async_xfer {
  USBH_CDC_FNCT_HANDLE ACM_FnctHandle;                          // Handle to CDC ACM fnct.
  USBH_ACM_ASYNC_FNCT  AsyncFnct;                               // Async function to call on xfer completion.
  void                 *ArgPtr;                                 // Pointer to app/subclass specific data.
} USBH_ACM_ASYNC_XFER;

/********************************************************************************************************
 *                                           CDC ACM FUNCTION
 *******************************************************************************************************/

typedef struct usbh_acm_fnct {
  USBH_DEV_HANDLE      DevHandle;                               // Handle to device.
  USBH_FNCT_HANDLE     FnctHandle;                              // Handle to function.
  USBH_CDC_FNCT_HANDLE CDC_FnctHandle;                          // Handle to CDC function.

  CPU_INT08U           CallMgmtCapabilities;                    // Capabilities reported by call mgmt fnctl desc.
  CPU_INT08U           Capabilities;                            // Capabilities reported by ACM       fnctl desc.
  CPU_INT08U           SerialStatePrev;                         // Previous serial state.

  void                 *ArgPtr;                                 // Ptr to app arg to pass to app fnct.
} USBH_ACM_FNCT;

/********************************************************************************************************
 *                                           CDC ACM SUBCLASS
 *******************************************************************************************************/

typedef struct usbh_acm {
  CPU_INT32U           StdReqTimeoutMs;

  USBH_CLASS_FNCT_POOL FnctPool;                                // CDC ACM fnct pool.

  MEM_DYN_POOL         AsyncXferPool;                           // Pool of asynchronous xfer data.

  USBH_ACM_APP_FNCTS   *AppFnctsPtr;                            // Ptr to app functions.
} USBH_ACM;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static USBH_ACM *USBH_ACM_Ptr = DEF_NULL;                       // Ptr to CDC ACM internal structure.

/********************************************************************************************************
 *                                           CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
USBH_ACM_INIT_CFG USBH_ACM_InitCfg = {
  .MemSegPtr = DEF_NULL,

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  .OptimizeSpd = { .FnctQty = 0u },
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  .InitAlloc = { .FnctQty = 0u, .AsyncXferQty = 0u }
#endif
};
#else
extern USBH_ACM_INIT_CFG USBH_ACM_InitCfg;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void USBH_ACM_Disconn(void *p_class_fnct);

#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
static void USBH_ACM_UnInit(void);
#endif

#ifdef RTOS_MODULE_COMMON_SHELL_AVAIL
static void USBH_ACM_TraceDump(void                *p_class_fnct,
                               CPU_INT32U          opt,
                               USBH_CMD_TRACE_FNCT trace_fnct);
#endif

static CPU_BOOLEAN USBH_ACM_Probe(USBH_DEV_HANDLE  dev_handle,
                                  USBH_FNCT_HANDLE fnct_handle,
                                  CPU_INT08U       class_code,
                                  void             **pp_cdc_sublass,
                                  RTOS_ERR         *p_err);

static void USBH_ACM_Conn(USBH_DEV_HANDLE      dev_handle,
                          USBH_FNCT_HANDLE     fnct_handle,
                          USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                          void                 **pp_cdc_sublass);

static void USBH_ACM_NetConn(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                             void                 *p_cdc_sublass,
                             CPU_BOOLEAN          is_conn);

static void USBH_ACM_RespAvail(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                               void                 *p_cdc_sublass);

static void USBH_ACM_SerStateChng(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                                  void                 *p_cdc_sublass,
                                  CPU_INT08U           serial_state);

static void USBH_ACM_XferCmpl(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                              CPU_INT08U           dci_ix,
                              CPU_INT08U           *p_buf,
                              CPU_INT32U           buf_len,
                              CPU_INT32U           xfer_len,
                              void                 *p_arg,
                              RTOS_ERR             err);

/********************************************************************************************************
 *                                           CDC ACM CLASS DRIVER
 *******************************************************************************************************/

USBH_CLASS_DRV USBH_ACM_Drv = {
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,
  USBH_ACM_Disconn,
  DEF_NULL,
  DEF_NULL,
#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
  USBH_ACM_UnInit,
#else
  DEF_NULL,
#endif
#ifdef RTOS_MODULE_COMMON_SHELL_AVAIL
  USBH_ACM_TraceDump,
#endif
  (CPU_CHAR *)"ACM",
};

//                                                                 --------------- CDC SUBCLASS DRIVER ----------------
USBH_CDC_SUBCLASS_DRV USBH_ACM_SubDrv = {
  USBH_ACM_Probe,
  USBH_ACM_Conn,
  DEF_NULL,
  USBH_ACM_NetConn,
  USBH_ACM_RespAvail,
  DEF_NULL,
  DEF_NULL,
  USBH_ACM_SerStateChng,
  DEF_NULL,
  DEF_NULL,
  DEF_NULL,

  10u
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       USBH_ACM_ConfigureMemSeg()
 *
 * @brief    Configures the memory segment to use when allocating control data and buffers.
 *
 * @param    p_mem_seg   Pointer to memory segment to use when allocating control data.
 *                       Can be the same segment used for p_mem_seg_buf.
 *                       DEF_NULL means general purpose heap segment.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the ACM class is initialized via the
 *               USBH_ACM_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_ACM_ConfigureMemSeg(MEM_SEG *p_mem_seg)
{
  RTOS_ASSERT_CRITICAL((USBH_ACM_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_ACM_InitCfg.MemSegPtr = p_mem_seg;
}
#endif

/****************************************************************************************************//**
 *                                   USBH_ACM_ConfigureOptimizeSpdCfg()
 *
 * @brief    Sets the configurations required when optimize speed mode is enabled.
 *
 * @param    p_optimize_spd_cfg  Pointer to the structure containing the configurations for the
 *                               optimize speed mode.
 *
 * @note     (1) This function MUST be called before the ACM class is initialized via the
 *               USBH_ACM_Init() function.
 *
 * @note     (2) This function MUST be called when the USBH_CFG_OPTIMIZE_SPD_EN configuration is set
 *               to DEF_ENABLED.
 *******************************************************************************************************/

#if ((RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED) \
  && (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED))
void USBH_ACM_ConfigureOptimizeSpdCfg(const USBH_ACM_CFG_OPTIMIZE_SPD *p_optimize_spd_cfg)
{
  RTOS_ASSERT_CRITICAL((USBH_ACM_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_ACM_InitCfg.OptimizeSpd = *p_optimize_spd_cfg;
}
#endif

/****************************************************************************************************//**
 *                                       USBH_ACM_ConfigureInitAllocCfg()
 *
 * @brief    Sets the configurations required when allocation at initialization mode is enabled.
 *
 * @param    p_init_alloc_cfg    Pointer to the structure containing the configurations for the
 *                               allocation at initialization mode.
 *
 * @note     (1) This function MUST be called before the ACM class is initialized via the
 *               USBH_ACM_Init() function.
 *
 * @note     (2) This function MUST be called when the USBH_CFG_INIT_ALLOC_EN configuration is set
 *               to DEF_ENABLED.
 *******************************************************************************************************/

#if ((RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED) \
  && (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED))
void USBH_ACM_ConfigureInitAllocCfg(const USBH_ACM_CFG_INIT_ALLOC *p_init_alloc_cfg)
{
  RTOS_ASSERT_CRITICAL((USBH_ACM_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_ACM_InitCfg.InitAlloc = *p_init_alloc_cfg;
}
#endif

/****************************************************************************************************//**
 *                                               USBH_ACM_Init()
 *
 * @brief    Initializes the Communication Device Class (CDC) Abstract Control Model (ACM) subclass driver.
 *
 * @param    p_acm_app_fncts     Pointer to the ACM application callback functions.
 *                               [Content MUST be persistent]
 *
 * @param    p_err               Pointer to the variable that will receive the return error code from this
 *                               function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *******************************************************************************************************/
void USBH_ACM_Init(USBH_ACM_APP_FNCTS *p_acm_app_fncts,
                   RTOS_ERR           *p_err)
{
  CPU_INT08U subclass_fnct_qty;
  USBH_ACM   *p_acm;
  CORE_DECLARE_IRQ_STATE;

  //                                                               Validate configurations.
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  RTOS_ASSERT_DBG_ERR_SET((USBH_ACM_InitCfg.OptimizeSpd.FnctQty != 0u), *p_err, RTOS_ERR_NOT_READY,; );
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  RTOS_ASSERT_DBG_ERR_SET((USBH_ACM_InitCfg.InitAlloc.FnctQty != 0u), *p_err, RTOS_ERR_NOT_READY,; );

  RTOS_ASSERT_DBG_ERR_SET((USBH_ACM_InitCfg.InitAlloc.AsyncXferQty > 0u), *p_err, RTOS_ERR_INVALID_CFG,; );
#endif

#if ((USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED) \
  && (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED))
  RTOS_ASSERT_DBG_ERR_SET((USBH_ACM_InitCfg.OptimizeSpd.FnctQty == USBH_ACM_InitCfg.InitAlloc.FnctQty), *p_err, RTOS_ERR_INVALID_CFG,; );
#endif // ((USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED) &&
  //                                                               (USBH_CFG_INIT_ALLOC_EN   == DEF_ENABLED))

  //                                                               -------- ALLOC CDC ACM CLASS INTERNAL DATA ---------
  //                                                               Alloc CDC ACM class root struct.
  p_acm = (USBH_ACM *)Mem_SegAlloc("USBH - CDC ACM root struct",
                                   USBH_ACM_InitCfg.MemSegPtr,
                                   sizeof(USBH_ACM),
                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_acm->StdReqTimeoutMs = USBH_ACM_STD_REQ_TIMEOUT_DFLT;
  p_acm->AppFnctsPtr = p_acm_app_fncts;

  CORE_ENTER_ATOMIC();
  USBH_ACM_Ptr = p_acm;
  CORE_EXIT_ATOMIC();

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  subclass_fnct_qty = USBH_ACM_InitCfg.InitAlloc.FnctQty;
#elif (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  subclass_fnct_qty = USBH_ACM_InitCfg.OptimizeSpd.FnctQty;
#else
  subclass_fnct_qty = USBH_CLASS_FNCT_QTY_UNLIMITED;
#endif

  USBH_ClassFnctPoolCreate("USBH - ACM subclass function pool",
                           &USBH_ACM_Ptr->FnctPool,
                           USBH_ACM_InitCfg.MemSegPtr,
                           sizeof(USBH_ACM_FNCT),
                           subclass_fnct_qty,
                           DEF_NULL,
                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  Mem_DynPoolCreate("USBH - ACM async xfer pool",
                    &USBH_ACM_Ptr->AsyncXferPool,
                    USBH_ACM_InitCfg.MemSegPtr,
                    sizeof(USBH_ACM_ASYNC_XFER),
                    sizeof(CPU_ALIGN),
#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
                    USBH_ACM_InitCfg.InitAlloc.AsyncXferQty,
                    USBH_ACM_InitCfg.InitAlloc.AsyncXferQty,
#else
                    0u,
                    LIB_MEM_BLK_QTY_UNLIMITED,
#endif
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Register ACM subclass to CDC.
  USBH_CDC_SubclassDrvReg(&USBH_ACM_Drv,
                          &USBH_ACM_SubDrv,
                          p_err);
}

/****************************************************************************************************//**
 *                                       USBH_ACM_StdReqTimeoutSet()
 *
 * @brief    Assigns a new timeout delay for the ACM standard requests.
 *
 * @param    std_req_timeout_ms  New timeout, in milliseconds.
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error
 *                               code(s) from this function:
 *                                   - RTOS_ERR_NONE
 *******************************************************************************************************/
void USBH_ACM_StdReqTimeoutSet(CPU_INT32U std_req_timeout_ms,
                               RTOS_ERR   *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((USBH_ACM_Ptr != DEF_NULL), RTOS_ERR_NOT_INIT,; );

  USBH_ACM_Ptr->StdReqTimeoutMs = std_req_timeout_ms;
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                       USBH_ACM_CapabilitiesGet()
 *
 * @brief    Gets the CDC ACM function capabilities.
 *
 * @param    acm_fnct_handle     Handle to the CDC ACM function.
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
 * @return   Bitmap that represents the following CDC ACM features:
 *               - USBH_ACM_CAP_CALL_MGMT_HANDLE
 *               - USBH_ACM_CAP_CALL_MGMT_DCI
 *               - USBH_ACM_CAP_COMM_FEATURE
 *               - USBH_ACM_CAP_LINE_CODING
 *               - USBH_ACM_CAP_SEND_BREAK
 *               - USBH_ACM_CAP_NET_CONN
 *
 * @note     (1) This function returns a bitmap representing the features supported by the CDC ACM
 *               function. This information comes from the Call Management and Abstract Control
 *               Model functional descriptors.
 *******************************************************************************************************/
CPU_INT08U USBH_ACM_CapabilitiesGet(USBH_ACM_FNCT_HANDLE acm_fnct_handle,
                                    RTOS_ERR             *p_err)
{
  CPU_INT08U    capabilities;
  USBH_ACM_FNCT *p_acm_fnct;
  CPU_INT32U    std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_ACM_Ptr->StdReqTimeoutMs;
  CORE_EXIT_ATOMIC();

  p_acm_fnct = (USBH_ACM_FNCT *)USBH_ClassFnctAcquire(&USBH_ACM_Ptr->FnctPool,
                                                      acm_fnct_handle,
                                                      DEF_NO,
                                                      std_req_timeout,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  capabilities = (DEF_BIT_IS_SET(p_acm_fnct->CallMgmtCapabilities, USBH_CDC_CALL_MGMT_CAP_HANDLE) == DEF_YES) ? USBH_ACM_CAP_CALL_MGMT_HANDLE : 0u;
  capabilities |= (DEF_BIT_IS_SET(p_acm_fnct->CallMgmtCapabilities, USBH_CDC_CALL_MGMT_CAP_DCI) == DEF_YES) ? USBH_ACM_CAP_CALL_MGMT_DCI    : 0u;
  capabilities |= (DEF_BIT_IS_SET(p_acm_fnct->Capabilities, USBH_CDC_ACM_CAP_COMM_FEATURE) == DEF_YES) ? USBH_ACM_CAP_COMM_FEATURE     : 0u;
  capabilities |= (DEF_BIT_IS_SET(p_acm_fnct->Capabilities, USBH_CDC_ACM_CAP_LINE_CODING) == DEF_YES) ? USBH_ACM_CAP_LINE_CODING      : 0u;
  capabilities |= (DEF_BIT_IS_SET(p_acm_fnct->Capabilities, USBH_CDC_ACM_CAP_SEND_BREAK) == DEF_YES) ? USBH_ACM_CAP_SEND_BREAK       : 0u;
  capabilities |= (DEF_BIT_IS_SET(p_acm_fnct->Capabilities, USBH_CDC_ACM_CAP_NET_CONN) == DEF_YES) ? USBH_ACM_CAP_NET_CONN         : 0u;

  USBH_ClassFnctRelease(&USBH_ACM_Ptr->FnctPool,
                        acm_fnct_handle,
                        DEF_NO);

  return (capabilities);
}

/****************************************************************************************************//**
 *                                       USBH_ACM_EncapsulatedCmdTx()
 *
 * @brief    Sends the CDC ACM encapsulated command.
 *
 * @param    acm_fnct_handle     Handle to the CDC ACM function.
 *
 * @param    p_buf               Pointer to the buffer that contains command.
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
 * @return   Number of bytes transferred.
 *
 * @note     (1) For more information on SendEncapsulatedCommand, see 'USB Class Definitions for
 *               Communication Devices Specification", version 1.2, Section 6.2.1'.
 *******************************************************************************************************/
CPU_INT16U USBH_ACM_EncapsulatedCmdTx(USBH_ACM_FNCT_HANDLE acm_fnct_handle,
                                      CPU_INT08U           *p_buf,
                                      CPU_INT16U           buf_len,
                                      CPU_INT32U           timeout,
                                      RTOS_ERR             *p_err)
{
  CPU_INT16U    xfer_len;
  USBH_ACM_FNCT *p_acm_fnct;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_acm_fnct = (USBH_ACM_FNCT *)USBH_ClassFnctAcquire(&USBH_ACM_Ptr->FnctPool,
                                                      acm_fnct_handle,
                                                      DEF_NO,
                                                      timeout,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  xfer_len = USBH_CDC_EncapsulatedCmdTx(p_acm_fnct->CDC_FnctHandle,
                                        p_buf,
                                        buf_len,
                                        timeout,
                                        p_err);

  USBH_ClassFnctRelease(&USBH_ACM_Ptr->FnctPool,
                        acm_fnct_handle,
                        DEF_NO);

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                       USBH_ACM_EncapsulatedCmdRx()
 *
 * @brief    Receives the CDC ACM encapsulated command.
 *
 * @param    acm_fnct_handle     Handle to the CDC ACM function.
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
CPU_INT16U USBH_ACM_EncapsulatedRespRx(USBH_ACM_FNCT_HANDLE acm_fnct_handle,
                                       CPU_INT08U           *p_buf,
                                       CPU_INT16U           buf_len,
                                       CPU_INT32U           timeout,
                                       RTOS_ERR             *p_err)
{
  CPU_INT16U    xfer_len;
  USBH_ACM_FNCT *p_acm_fnct;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_acm_fnct = (USBH_ACM_FNCT *)USBH_ClassFnctAcquire(&USBH_ACM_Ptr->FnctPool,
                                                      acm_fnct_handle,
                                                      DEF_NO,
                                                      timeout,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  xfer_len = USBH_CDC_EncapsulatedRespRx(p_acm_fnct->CDC_FnctHandle,
                                         p_buf,
                                         buf_len,
                                         timeout,
                                         p_err);

  USBH_ClassFnctRelease(&USBH_ACM_Ptr->FnctPool,
                        acm_fnct_handle,
                        DEF_NO);

  return (xfer_len);
}

/****************************************************************************************************//**
 *                                           USBH_ACM_CommFeatureSet()
 *
 * @brief    Configures the CDC ACM communication feature.
 *
 * @param    acm_fnct_handle     Handle to the CDC ACM function.
 *
 * @param    feature             Feature to configure using the following:
 *                                   - USBH_CDC_COMM_FEATURE_ABSTRACT_STATE
 *                                   - USBH_CDC_COMM_FEATURE_COUNTRY_SETTING
 *
 * @param    data                Data of the communication feature request.
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
 *                                   - RTOS_ERR_NOT_SUPPORTED
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
void USBH_ACM_CommFeatureSet(USBH_ACM_FNCT_HANDLE acm_fnct_handle,
                             CPU_INT08U           feature,
                             CPU_INT16U           data,
                             CPU_INT32U           timeout,
                             RTOS_ERR             *p_err)
{
  USBH_ACM_FNCT *p_acm_fnct;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  p_acm_fnct = (USBH_ACM_FNCT *)USBH_ClassFnctAcquire(&USBH_ACM_Ptr->FnctPool,
                                                      acm_fnct_handle,
                                                      DEF_NO,
                                                      timeout,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (DEF_BIT_IS_SET(p_acm_fnct->Capabilities, USBH_CDC_ACM_CAP_COMM_FEATURE) == DEF_NO) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
    goto end_rel;
  }

  USBH_CDC_CommFeatureSet(p_acm_fnct->CDC_FnctHandle,
                          feature,
                          data,
                          timeout,
                          p_err);

end_rel:
  USBH_ClassFnctRelease(&USBH_ACM_Ptr->FnctPool,
                        acm_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                           USBH_ACM_CommFeatureGet()
 *
 * @brief    Gets the CDC ACM function line coding.
 *
 * @param    acm_fnct_handle     Handle to the CDC ACM function.
 *
 * @param    feature             Feature to Get using the following:.
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
 *                                   - RTOS_ERR_NOT_SUPPORTED
 *                                   - RTOS_ERR_INVALID_ARG
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_TX
 *                                   - RTOS_ERR_NOT_AVAIL
 *                                   - RTOS_ERR_WOULD_OVF
 *
 * @return   Data of the Get communication feature request.
 *
 * @note     (1) For more information on GetCommFeature request, see 'Communication Class Subclass
 *               Specification for PSTN Devices, version 1.2, Section 6.3.2'.
 *******************************************************************************************************/
CPU_INT16U USBH_ACM_CommFeatureGet(USBH_ACM_FNCT_HANDLE acm_fnct_handle,
                                   CPU_INT08U           feature,
                                   CPU_INT32U           timeout,
                                   RTOS_ERR             *p_err)
{
  CPU_INT16U    data = 0u;
  USBH_ACM_FNCT *p_acm_fnct;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  p_acm_fnct = (USBH_ACM_FNCT *)USBH_ClassFnctAcquire(&USBH_ACM_Ptr->FnctPool,
                                                      acm_fnct_handle,
                                                      DEF_NO,
                                                      timeout,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  if (DEF_BIT_IS_SET(p_acm_fnct->Capabilities, USBH_CDC_ACM_CAP_COMM_FEATURE) == DEF_NO) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
    goto end_rel;
  }

  data = USBH_CDC_CommFeatureGet(p_acm_fnct->CDC_FnctHandle,
                                 feature,
                                 timeout,
                                 p_err);

end_rel:
  USBH_ClassFnctRelease(&USBH_ACM_Ptr->FnctPool,
                        acm_fnct_handle,
                        DEF_NO);

  return (data);
}

/****************************************************************************************************//**
 *                                           USBH_ACM_CommFeatureClr()
 *
 * @brief    Clears the CDC ACM function communication feature.
 *
 * @param    acm_fnct_handle     Handle to the CDC ACM function.
 *
 * @param    feature             Feature to clear including the following:
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
 *                                   - RTOS_ERR_NOT_SUPPORTED
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
void USBH_ACM_CommFeatureClr(USBH_ACM_FNCT_HANDLE acm_fnct_handle,
                             CPU_INT08U           feature,
                             CPU_INT32U           timeout,
                             RTOS_ERR             *p_err)
{
  USBH_ACM_FNCT *p_acm_fnct;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  p_acm_fnct = (USBH_ACM_FNCT *)USBH_ClassFnctAcquire(&USBH_ACM_Ptr->FnctPool,
                                                      acm_fnct_handle,
                                                      DEF_NO,
                                                      timeout,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (DEF_BIT_IS_SET(p_acm_fnct->Capabilities, USBH_CDC_ACM_CAP_COMM_FEATURE) == DEF_NO) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
    goto end_rel;
  }

  USBH_CDC_CommFeatureClr(p_acm_fnct->CDC_FnctHandle,
                          feature,
                          timeout,
                          p_err);

end_rel:
  USBH_ClassFnctRelease(&USBH_ACM_Ptr->FnctPool,
                        acm_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                           USBH_ACM_LineCodingSet()
 *
 * @brief    Sets the CDC ACM function line coding.
 *
 * @param    acm_fnct_handle     Handle to the CDC ACM function.
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
 *                                   - RTOS_ERR_NOT_SUPPORTED
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
void USBH_ACM_LineCodingSet(USBH_ACM_FNCT_HANDLE acm_fnct_handle,
                            USBH_CDC_LINECODING  *p_line_coding,
                            CPU_INT32U           timeout,
                            RTOS_ERR             *p_err)
{
  USBH_ACM_FNCT *p_acm_fnct;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  p_acm_fnct = (USBH_ACM_FNCT *)USBH_ClassFnctAcquire(&USBH_ACM_Ptr->FnctPool,
                                                      acm_fnct_handle,
                                                      DEF_NO,
                                                      timeout,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (DEF_BIT_IS_SET(p_acm_fnct->Capabilities, USBH_CDC_ACM_CAP_LINE_CODING) == DEF_NO) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
    goto end_rel;
  }

  USBH_CDC_LineCodingSet(p_acm_fnct->CDC_FnctHandle,
                         p_line_coding,
                         timeout,
                         p_err);

end_rel:
  USBH_ClassFnctRelease(&USBH_ACM_Ptr->FnctPool,
                        acm_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                           USBH_ACM_LineCodingGet()
 *
 * @brief    Gets the CDC ACM function line coding.
 *
 * @param    acm_fnct_handle     Handle to the CDC ACM function.
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
 *                                   - RTOS_ERR_NOT_SUPPORTED
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
void USBH_ACM_LineCodingGet(USBH_ACM_FNCT_HANDLE acm_fnct_handle,
                            USBH_CDC_LINECODING  *p_line_coding,
                            CPU_INT32U           timeout,
                            RTOS_ERR             *p_err)
{
  USBH_ACM_FNCT *p_acm_fnct;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  p_acm_fnct = (USBH_ACM_FNCT *)USBH_ClassFnctAcquire(&USBH_ACM_Ptr->FnctPool,
                                                      acm_fnct_handle,
                                                      DEF_NO,
                                                      timeout,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (DEF_BIT_IS_SET(p_acm_fnct->Capabilities, USBH_CDC_ACM_CAP_LINE_CODING) == DEF_NO) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
    goto end_rel;
  }

  USBH_CDC_LineCodingGet(p_acm_fnct->CDC_FnctHandle,
                         p_line_coding,
                         timeout,
                         p_err);

end_rel:
  USBH_ClassFnctRelease(&USBH_ACM_Ptr->FnctPool,
                        acm_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                       USBH_CDC_CtrlLineStateSet()
 *
 * @brief    Sets the CDC ACM function control line state.
 *
 * @param    acm_fnct_handle     Handle to the CDC ACM function.
 *
 * @param    ctrl_signal         Configures the control signal using the following:
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
 *                                   - RTOS_ERR_NOT_SUPPORTED
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
void USBH_ACM_CtrlLineStateSet(USBH_ACM_FNCT_HANDLE acm_fnct_handle,
                               CPU_INT16U           ctrl_signal,
                               CPU_INT32U           timeout,
                               RTOS_ERR             *p_err)
{
  USBH_ACM_FNCT *p_acm_fnct;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  p_acm_fnct = (USBH_ACM_FNCT *)USBH_ClassFnctAcquire(&USBH_ACM_Ptr->FnctPool,
                                                      acm_fnct_handle,
                                                      DEF_NO,
                                                      timeout,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (DEF_BIT_IS_SET(p_acm_fnct->Capabilities, USBH_CDC_ACM_CAP_LINE_CODING) == DEF_NO) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
    goto end_rel;
  }

  USBH_CDC_CtrlLineStateSet(p_acm_fnct->CDC_FnctHandle,
                            ctrl_signal,
                            timeout,
                            p_err);

end_rel:
  USBH_ClassFnctRelease(&USBH_ACM_Ptr->FnctPool,
                        acm_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                           USBH_CDC_BrkSend()
 *
 * @brief    Sends the break signal to CDC ACM function.
 *
 * @param    acm_fnct_handle     Handle to the CDC ACM function.
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
 *                                   - RTOS_ERR_NOT_SUPPORTED
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
void USBH_ACM_BrkSend(USBH_ACM_FNCT_HANDLE acm_fnct_handle,
                      CPU_INT16U           dur,
                      CPU_INT32U           timeout,
                      RTOS_ERR             *p_err)
{
  USBH_ACM_FNCT *p_acm_fnct;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  p_acm_fnct = (USBH_ACM_FNCT *)USBH_ClassFnctAcquire(&USBH_ACM_Ptr->FnctPool,
                                                      acm_fnct_handle,
                                                      DEF_NO,
                                                      timeout,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (DEF_BIT_IS_SET(p_acm_fnct->Capabilities, USBH_CDC_ACM_CAP_SEND_BREAK) == DEF_NO) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
    goto end_rel;
  }

  USBH_CDC_BrkSend(p_acm_fnct->CDC_FnctHandle,
                   dur,
                   timeout,
                   p_err);

end_rel:
  USBH_ClassFnctRelease(&USBH_ACM_Ptr->FnctPool,
                        acm_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                           USBH_ACM_RxAsync()
 *
 * @brief    Receives the data from CDC ACM device. This function is asynchronous.
 *
 * @param    acm_fnct_handle     Handle to the CDC ACM function.
 *
 * @param    p_buf               Pointer to the destination buffer to receive data.
 *
 * @param    buf_len             Buffer length in octets.
 *
 * @param    async_fnct          Function that will be invoked upon completion of receive operation.
 *
 * @param    p_async_arg         Pointer to the argument that will be passed as parameter of 'async_fnct'.
 *
 * @param    p_err               Pointer to the variable that will receive one of these return error codes
 *                               from this function ::
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
void USBH_ACM_RxAsync(USBH_ACM_FNCT_HANDLE acm_fnct_handle,
                      CPU_INT08U           *p_buf,
                      CPU_INT32U           buf_len,
                      USBH_ACM_ASYNC_FNCT  async_fnct,
                      void                 *p_async_arg,
                      RTOS_ERR             *p_err)
{
  USBH_ACM_FNCT       *p_acm_fnct;
  USBH_ACM_ASYNC_XFER *p_async_xfer;
  CPU_INT32U          std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((async_fnct != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_ACM_Ptr->StdReqTimeoutMs;
  CORE_EXIT_ATOMIC();

  p_acm_fnct = (USBH_ACM_FNCT *)USBH_ClassFnctAcquire(&USBH_ACM_Ptr->FnctPool,
                                                      acm_fnct_handle,
                                                      DEF_NO,
                                                      std_req_timeout,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_async_xfer = (USBH_ACM_ASYNC_XFER *)Mem_DynPoolBlkGet(&USBH_ACM_Ptr->AsyncXferPool,
                                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Failed to allocate asynchronous transfer structure."));
    goto end_rel;
  }

  p_async_xfer->ACM_FnctHandle = acm_fnct_handle;
  p_async_xfer->AsyncFnct = async_fnct;
  p_async_xfer->ArgPtr = p_async_arg;

  USBH_CDC_DCI_RxAsync(p_acm_fnct->CDC_FnctHandle,
                       USBH_ACM_DCI_IX,
                       p_buf,
                       buf_len,
                       USBH_ACM_XferCmpl,
                       (void *)p_async_xfer,
                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR err_local;

    Mem_DynPoolBlkFree(&USBH_ACM_Ptr->AsyncXferPool,
                       (void *)p_async_xfer,
                       &err_local);
    if (RTOS_ERR_CODE_GET(err_local) != RTOS_ERR_NONE) {
      LOG_ERR(("Freeing async xfer -> ", RTOS_ERR_LOG_ARG_GET(err_local)));
    }
  }

end_rel:
  USBH_ClassFnctRelease(&USBH_ACM_Ptr->FnctPool,
                        acm_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                           USBH_ACM_TxAsync()
 *
 * @brief    Transmits the data to CDC ACM device. This function is asynchronous.
 *
 * @param    acm_fnct_handle     Handle to the CDC ACM function.
 *
 * @param    p_buf               Pointer to the buffer of data that will be transmitted.
 *
 * @param    buf_len             Buffer length in octets.
 *
 * @param    async_fnct          Function that will be invoked upon completion of transmit operation.
 *
 * @param    p_async_arg         Pointer to the argument that will be passed as parameter of 'async_fnct'.
 *
 * @param    p_err               Pointer to the variable that will receive one of these return error codes
 *                               from this function :
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
void USBH_ACM_TxAsync(USBH_ACM_FNCT_HANDLE acm_fnct_handle,
                      CPU_INT08U           *p_buf,
                      CPU_INT32U           buf_len,
                      USBH_ACM_ASYNC_FNCT  async_fnct,
                      void                 *p_async_arg,
                      RTOS_ERR             *p_err)
{
  USBH_ACM_FNCT       *p_acm_fnct;
  USBH_ACM_ASYNC_XFER *p_async_xfer;
  CPU_INT32U          std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((async_fnct != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_ACM_Ptr->StdReqTimeoutMs;
  CORE_EXIT_ATOMIC();

  p_acm_fnct = (USBH_ACM_FNCT *)USBH_ClassFnctAcquire(&USBH_ACM_Ptr->FnctPool,
                                                      acm_fnct_handle,
                                                      DEF_NO,
                                                      std_req_timeout,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_async_xfer = (USBH_ACM_ASYNC_XFER *)Mem_DynPoolBlkGet(&USBH_ACM_Ptr->AsyncXferPool,
                                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Failed to allocate asynchronous transfer structure."));
    goto end_rel;
  }

  p_async_xfer->ACM_FnctHandle = acm_fnct_handle;
  p_async_xfer->AsyncFnct = async_fnct;
  p_async_xfer->ArgPtr = p_async_arg;

  USBH_CDC_DCI_TxAsync(p_acm_fnct->CDC_FnctHandle,
                       USBH_ACM_DCI_IX,
                       p_buf,
                       buf_len,
                       USBH_ACM_XferCmpl,
                       (void *)p_async_xfer,
                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR err_local;

    Mem_DynPoolBlkFree(&USBH_ACM_Ptr->AsyncXferPool,
                       (void *)p_async_xfer,
                       &err_local);
    if (RTOS_ERR_CODE_GET(err_local) != RTOS_ERR_NONE) {
      LOG_ERR(("Freeing async xfer -> ", RTOS_ERR_LOG_ARG_GET(err_local)));
    }
  }

end_rel:
  USBH_ClassFnctRelease(&USBH_ACM_Ptr->FnctPool,
                        acm_fnct_handle,
                        DEF_NO);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTION
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBH_ACM_Disconn()
 *
 * @brief    Disconnects an ACM function.
 *
 * @param    p_class_fnct    Pointer to the ACM subclass data structure.
 *******************************************************************************************************/
static void USBH_ACM_Disconn(void *p_class_fnct)
{
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  USBH_ACM_FNCT          *p_acm_fnct;
  USBH_ACM_APP_FNCTS     *p_acm_app_fncts;
  RTOS_ERR               err_usbh;

  p_acm_fnct = (USBH_ACM_FNCT *)USBH_ClassFnctAcquire(&USBH_ACM_Ptr->FnctPool,
                                                      class_fnct_handle,
                                                      DEF_YES,
                                                      OP_LOCK_TIMEOUT_INFINITE,
                                                      &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring class function ->", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

  //                                                               Notify app of disconnected CDC ACM function.
  p_acm_app_fncts = USBH_ACM_Ptr->AppFnctsPtr;
  if ((p_acm_app_fncts != DEF_NULL)
      && (p_acm_app_fncts->Disconn != DEF_NULL)) {
    p_acm_app_fncts->Disconn(class_fnct_handle,
                             p_acm_fnct->ArgPtr);
  }

  USBH_ClassFnctRelease(&USBH_ACM_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);

  USBH_ClassFnctFree(&USBH_ACM_Ptr->FnctPool,
                     class_fnct_handle,
                     &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Freeing class function ->", RTOS_ERR_LOG_ARG_GET(err_usbh)));
  }
}

/****************************************************************************************************//**
 *                                               USBH_ACM_UnInit()
 *
 * @brief    Un-initializes the ACM subclass.
 *******************************************************************************************************/

#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
static void USBH_ACM_UnInit(void)
{
  RTOS_ERR err_usbh;

  USBH_ClassFnctPoolDel(&USBH_ACM_Ptr->FnctPool,
                        &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Un-initalizing ACM ->", RTOS_ERR_LOG_ARG_GET(err_usbh)));
  }

  USBH_ACM_Ptr = DEF_NULL;
}
#endif

/****************************************************************************************************//**
 *                                           USBH_ACM_TraceDump()
 *
 * @brief    Output subclass specific debug information on function.
 *
 * @param    p_class_fnct    Pointer to the internal structure of the function.
 *
 * @param    opt             Trace dump options.
 *
 * @param    trace_fnct      Function to call to output traces.
 *******************************************************************************************************/

#ifdef RTOS_MODULE_COMMON_SHELL_AVAIL
static void USBH_ACM_TraceDump(void                *p_class_fnct,
                               CPU_INT32U          opt,
                               USBH_CMD_TRACE_FNCT trace_fnct)
{
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_class_fnct;
  USBH_ACM_FNCT          *p_acm_fnct;
  RTOS_ERR               err_usbh;

  (void)&opt;

  p_acm_fnct = (USBH_ACM_FNCT *)USBH_ClassFnctAcquire(&USBH_ACM_Ptr->FnctPool,
                                                      class_fnct_handle,
                                                      DEF_YES,
                                                      OP_LOCK_TIMEOUT_INFINITE,
                                                      &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    trace_fnct("    -------------- ERROR! Unable to acquire ACM fnct ---------------\r\n");
    return;
  }

  trace_fnct("    -------- USB Host (CDC ACM specific debug information) ---------\r\n");
  trace_fnct("    | Handle CM | CM DCI | Comm feat | Line cod | Brk  | Net Conn  |\r\n");
  trace_fnct("    | ");

  if (DEF_BIT_IS_SET(p_acm_fnct->CallMgmtCapabilities, USBH_CDC_CALL_MGMT_CAP_HANDLE) == DEF_YES) {
    trace_fnct("YES       | ");
  } else {
    trace_fnct("NO        | ");
  }

  if (DEF_BIT_IS_SET(p_acm_fnct->CallMgmtCapabilities, USBH_CDC_CALL_MGMT_CAP_DCI) == DEF_YES) {
    trace_fnct("YES    | ");
  } else {
    trace_fnct("NO     | ");
  }

  if (DEF_BIT_IS_SET(p_acm_fnct->Capabilities, USBH_CDC_ACM_CAP_COMM_FEATURE) == DEF_YES) {
    trace_fnct("YES       | ");
  } else {
    trace_fnct("NO        | ");
  }

  if (DEF_BIT_IS_SET(p_acm_fnct->Capabilities, USBH_CDC_ACM_CAP_LINE_CODING) == DEF_YES) {
    trace_fnct("YES      | ");
  } else {
    trace_fnct("NO       | ");
  }

  if (DEF_BIT_IS_SET(p_acm_fnct->Capabilities, USBH_CDC_ACM_CAP_SEND_BREAK) == DEF_YES) {
    trace_fnct("YES  | ");
  } else {
    trace_fnct("NO   | ");
  }

  if (DEF_BIT_IS_SET(p_acm_fnct->Capabilities, USBH_CDC_ACM_CAP_NET_CONN) == DEF_YES) {
    trace_fnct("YES       |\r\n");
  } else {
    trace_fnct("NO        |\r\n");
  }

  USBH_ClassFnctRelease(&USBH_ACM_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_YES);
}
#endif

/****************************************************************************************************//**
 *                                               USBH_ACM_Probe()
 *
 * @brief    Determines if subclass driver can handle connected CDC function.
 *
 * @param    dev_handle      Handle to device.
 *
 * @param    fnct_handle     Handle to function.
 *
 * @param    class           Function class code
 *
 * @param    pp_cdc_sublass  Pointer that will receive the internal structure of the function.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this
 *                           function.
 *
 * @return   DEF_OK,   if device should     be handled by this class driver.
 *           DEF_FAIL, if device should not be handled by this class driver.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_ACM_Probe(USBH_DEV_HANDLE  dev_handle,
                                  USBH_FNCT_HANDLE fnct_handle,
                                  CPU_INT08U       class_code,
                                  void             **pp_cdc_sublass,
                                  RTOS_ERR         *p_err)
{
  USBH_CLASS_FNCT_HANDLE class_fnct_handle;
  USBH_ACM_FNCT          *p_acm_fnct;

  if (class_code != USBH_CDC_CTRL_SUBCLASS_CODE_ACM) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    return (DEF_FAIL);
  } else {
    //                                                             Alloc CDC ACM fnct.
    class_fnct_handle = USBH_ClassFnctAlloc(&USBH_ACM_Ptr->FnctPool,
                                            dev_handle,
                                            p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_CDC_ACM_FNCT_ALLOC);

      return (DEF_OK);
    }

    p_acm_fnct = (USBH_ACM_FNCT *)USBH_ClassFnctAcquire(&USBH_ACM_Ptr->FnctPool,
                                                        class_fnct_handle,
                                                        DEF_YES,
                                                        OP_LOCK_TIMEOUT_INFINITE,
                                                        p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      RTOS_ERR err_free;

      USBH_ClassFnctFree(&USBH_ACM_Ptr->FnctPool,
                         class_fnct_handle,
                         &err_free);
      if (RTOS_ERR_CODE_GET(err_free) != RTOS_ERR_NONE) {
        LOG_ERR(("Freeing class function -> ", RTOS_ERR_LOG_ARG_GET(err_free)));
      }

      return (DEF_OK);
    }

    p_acm_fnct->DevHandle = dev_handle;
    p_acm_fnct->FnctHandle = fnct_handle;
    p_acm_fnct->CDC_FnctHandle = USBH_CLASS_FNCT_HANDLE_INVALID;
    p_acm_fnct->Capabilities = 0u;
    p_acm_fnct->CallMgmtCapabilities = 0u;
    p_acm_fnct->SerialStatePrev = 0xFFu;
    p_acm_fnct->ArgPtr = DEF_NULL;

    *pp_cdc_sublass = (void *)(CPU_ADDR)class_fnct_handle;

    USBH_ClassFnctRelease(&USBH_ACM_Ptr->FnctPool,
                          class_fnct_handle,
                          DEF_YES);

    return (DEF_OK);
  }
}

/****************************************************************************************************//**
 *                                               USBH_ACM_Conn()
 *
 * @brief    Connects an ACM function.
 *
 * @param    dev_handle          Handle to device.
 *
 * @param    fnct_handle         Handle to function.
 *
 * @param    cdc_fnct_handle     Handle to CDC function.
 *
 * @param    pp_cdc_sublass      Pointer to the a variable that will receive pointer to CDC subclass data
 *                               structure.
 *******************************************************************************************************/
static void USBH_ACM_Conn(USBH_DEV_HANDLE      dev_handle,
                          USBH_FNCT_HANDLE     fnct_handle,
                          USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                          void                 **pp_cdc_sublass)
{
  CPU_INT08U             fnctl_desc_subtype;
  CPU_INT08U             desc_len;
  CPU_INT08U             *p_extra_desc_buf;
  CPU_INT16U             cnt;
  CPU_INT16U             desc_extra_len;
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)*pp_cdc_sublass;
  USBH_ACM_APP_FNCTS     *p_acm_app_fncts = USBH_ACM_Ptr->AppFnctsPtr;
  USBH_ACM_FNCT          *p_acm_fnct;
  RTOS_ERR               err_usbh;
  CPU_INT32U             std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_ACM_Ptr->StdReqTimeoutMs;
  CORE_EXIT_ATOMIC();

  p_acm_fnct = (USBH_ACM_FNCT *)USBH_ClassFnctAcquire(&USBH_ACM_Ptr->FnctPool,
                                                      class_fnct_handle,
                                                      DEF_NO,
                                                      std_req_timeout,
                                                      &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring class function ->", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

  p_acm_fnct->CDC_FnctHandle = cdc_fnct_handle;

  //                                                               Parse fnctl desc.
  p_extra_desc_buf = USBH_IF_DescExtraGet(p_acm_fnct->FnctHandle,
                                          0u,
                                          &desc_extra_len,
                                          &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("While retrieving extra desc -> ", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

  cnt = USBH_DESC_LEN_IF;                                       // Retrieve ACM info from fnctl desc.
  while (cnt < desc_extra_len) {
    desc_len = MEM_VAL_GET_INT08U_LITTLE(&p_extra_desc_buf[cnt]);
    fnctl_desc_subtype = MEM_VAL_GET_INT08U_LITTLE(&p_extra_desc_buf[cnt + 2u]);

    switch (fnctl_desc_subtype) {
      case USBH_CDC_FNCTL_DESC_SUB_CM:
        p_acm_fnct->CallMgmtCapabilities = MEM_VAL_GET_INT08U_LITTLE(&p_extra_desc_buf[cnt + 3u]);
        break;

      case USBH_CDC_FNCTL_DESC_SUB_ACM:
        p_acm_fnct->Capabilities = MEM_VAL_GET_INT08U_LITTLE(&p_extra_desc_buf[cnt + 3u]);
        break;

      default:
        break;
    }

    cnt += desc_len;
  }

  //                                                               Notify app of connected CDC ACM function.
  if ((p_acm_app_fncts != DEF_NULL)
      && (p_acm_app_fncts->Conn != DEF_NULL)) {
    p_acm_fnct->ArgPtr = p_acm_app_fncts->Conn(dev_handle,
                                               fnct_handle,
                                               cdc_fnct_handle,
                                               class_fnct_handle);
  }

  USBH_ClassFnctRelease(&USBH_ACM_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                           USBH_ACM_NetConn()
 *
 * @brief    Notifies of network connection.
 *
 * @param    cdc_fnct_handle     Handle to CDC function.
 *
 * @param    p_cdc_sublass       Pointer to the CDC subclass data structure.
 *
 * @param    is_conn             Boolean value that indicates if is connected.
 *******************************************************************************************************/
static void USBH_ACM_NetConn(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                             void                 *p_cdc_sublass,
                             CPU_BOOLEAN          is_conn)
{
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_cdc_sublass;
  USBH_ACM_APP_FNCTS     *p_acm_app_fncts;
  USBH_ACM_FNCT          *p_acm_fnct;
  RTOS_ERR               err_usbh;
  CPU_INT32U             std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  (void)&cdc_fnct_handle;

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_ACM_Ptr->StdReqTimeoutMs;
  CORE_EXIT_ATOMIC();

  p_acm_fnct = (USBH_ACM_FNCT *)USBH_ClassFnctAcquire(&USBH_ACM_Ptr->FnctPool,
                                                      class_fnct_handle,
                                                      DEF_NO,
                                                      std_req_timeout,
                                                      &err_usbh);
  if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
    LOG_ERR(("Acquiring class function ->", RTOS_ERR_LOG_ARG_GET(err_usbh)));
    return;
  }

  //                                                               Notify app of net conn.
  p_acm_app_fncts = USBH_ACM_Ptr->AppFnctsPtr;
  if ((p_acm_app_fncts != DEF_NULL)
      && (p_acm_app_fncts->NetConn != DEF_NULL)) {
    p_acm_app_fncts->NetConn(class_fnct_handle,
                             p_acm_fnct->ArgPtr,
                             is_conn);
  }

  USBH_ClassFnctRelease(&USBH_ACM_Ptr->FnctPool,
                        class_fnct_handle,
                        DEF_NO);
}

/****************************************************************************************************//**
 *                                           USBH_ACM_RespAvail()
 *
 * @brief    Notifies of response availability.
 *
 * @param    cdc_fnct_handle     Handle to CDC function.
 *
 * @param    p_cdc_sublass       Pointer to the CDC subclass data structure.
 *******************************************************************************************************/
static void USBH_ACM_RespAvail(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                               void                 *p_cdc_sublass)
{
  void                   *p_acm_fnct_arg;
  USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_cdc_sublass;
  USBH_ACM_APP_FNCTS     *p_acm_app_fncts = USBH_ACM_Ptr->AppFnctsPtr;
  USBH_ACM_FNCT          *p_acm_fnct;
  RTOS_ERR               err_usbh;
  CPU_INT32U             std_req_timeout;
  CORE_DECLARE_IRQ_STATE;

  (void)&cdc_fnct_handle;

  CORE_ENTER_ATOMIC();
  std_req_timeout = USBH_ACM_Ptr->StdReqTimeoutMs;
  CORE_EXIT_ATOMIC();

  if ((p_acm_app_fncts != DEF_NULL)
      && (p_acm_app_fncts->RespAvail != DEF_NULL)) {
    p_acm_fnct = (USBH_ACM_FNCT *)USBH_ClassFnctAcquire(&USBH_ACM_Ptr->FnctPool,
                                                        class_fnct_handle,
                                                        DEF_NO,
                                                        std_req_timeout,
                                                        &err_usbh);
    if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
      LOG_ERR(("Acquiring class function ->", RTOS_ERR_LOG_ARG_GET(err_usbh)));
      return;
    }

    p_acm_fnct_arg = p_acm_fnct->ArgPtr;

    USBH_ClassFnctRelease(&USBH_ACM_Ptr->FnctPool,
                          class_fnct_handle,
                          DEF_NO);

    //                                                             Notify app of response availability.
    p_acm_app_fncts->RespAvail(class_fnct_handle,
                               p_acm_fnct_arg);
  }
}

/****************************************************************************************************//**
 *                                           USBH_ACM_SerStateChng()
 *
 * @brief    Notifies of serial state change.
 *
 * @param    cdc_fnct_handle     Handle to CDC function.
 *
 * @param    p_cdc_sublass       Pointer to the a variable that will receive pointer to CDC subclass data
 *                               structure.
 *
 * @param    serial_state        Bitmap that represents serial state.
 *******************************************************************************************************/
static void USBH_ACM_SerStateChng(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                                  void                 *p_cdc_sublass,
                                  CPU_INT08U           serial_state)
{
  USBH_ACM_APP_FNCTS *p_acm_app_fncts = USBH_ACM_Ptr->AppFnctsPtr;

  (void)&cdc_fnct_handle;

  if ((p_acm_app_fncts != DEF_NULL)
      && (p_acm_app_fncts->SerialStateChng != DEF_NULL)) {
    CPU_INT08U             serial_state_prev;
    USBH_CLASS_FNCT_HANDLE class_fnct_handle = (USBH_CLASS_FNCT_HANDLE)(CPU_ADDR)p_cdc_sublass;
    USBH_ACM_FNCT          *p_acm_fnct;
    void                   *p_acm_fnct_arg;
    RTOS_ERR               err_usbh;
    CPU_INT32U             std_req_timeout;
    CORE_DECLARE_IRQ_STATE;

    CORE_ENTER_ATOMIC();
    std_req_timeout = USBH_ACM_Ptr->StdReqTimeoutMs;
    CORE_EXIT_ATOMIC();

    p_acm_fnct = (USBH_ACM_FNCT *)USBH_ClassFnctAcquire(&USBH_ACM_Ptr->FnctPool,
                                                        class_fnct_handle,
                                                        DEF_NO,
                                                        std_req_timeout,
                                                        &err_usbh);
    if (RTOS_ERR_CODE_GET(err_usbh) != RTOS_ERR_NONE) {
      LOG_ERR(("Acquiring class function ->", RTOS_ERR_LOG_ARG_GET(err_usbh)));
      return;
    }

    p_acm_fnct_arg = p_acm_fnct->ArgPtr;
    serial_state_prev = p_acm_fnct->SerialStatePrev;
    p_acm_fnct->SerialStatePrev = serial_state;

    USBH_ClassFnctRelease(&USBH_ACM_Ptr->FnctPool,
                          class_fnct_handle,
                          DEF_NO);

    if (serial_state != serial_state_prev) {
      //                                                           Notify app of serial state change.
      p_acm_app_fncts->SerialStateChng(class_fnct_handle,
                                       p_acm_fnct_arg,
                                       serial_state);
    }
  }
}

/****************************************************************************************************//**
 *                                           USBH_ACM_XferCmpl()
 *
 * @brief    Callback function invoked when data reception is completed.
 *
 * @param    cdc_fnct_handle     Pointer to the CDC ACM device.
 *
 * @param    dci_ix              DCI index.
 *
 * @param    p_buf               Pointer to the buffer that contains data received.
 *
 * @param    buf_len             Buffer length in bytes.
 *
 * @param    xfer_len            Number of bytes received.
 *
 * @param    p_arg               Pointer to the argument.
 *
 * @param    err                 Receive status.
 *******************************************************************************************************/
static void USBH_ACM_XferCmpl(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                              CPU_INT08U           dci_ix,
                              CPU_INT08U           *p_buf,
                              CPU_INT32U           buf_len,
                              CPU_INT32U           xfer_len,
                              void                 *p_arg,
                              RTOS_ERR             err)
{
  RTOS_ERR             err_local;
  void                 *p_async_arg;
  USBH_ACM_FNCT_HANDLE acm_fnct_handle;
  USBH_ACM_ASYNC_FNCT  async_fnct;
  USBH_ACM_ASYNC_XFER  *p_async_xfer = (USBH_ACM_ASYNC_XFER *)p_arg;

  (void)&dci_ix;
  (void)&cdc_fnct_handle;

  acm_fnct_handle = p_async_xfer->ACM_FnctHandle;
  async_fnct = p_async_xfer->AsyncFnct;
  p_async_arg = p_async_xfer->ArgPtr;

  Mem_DynPoolBlkFree(&USBH_ACM_Ptr->AsyncXferPool,
                     (void *)p_async_xfer,
                     &err_local);
  if (RTOS_ERR_CODE_GET(err_local) != RTOS_ERR_NONE) {
    LOG_ERR(("Freeing async xfer -> ", RTOS_ERR_LOG_ARG_GET(err_local)));
  }

  async_fnct(acm_fnct_handle,
             p_buf,
             buf_len,
             xfer_len,
             p_async_arg,
             err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_HOST_ACM_AVAIL))
