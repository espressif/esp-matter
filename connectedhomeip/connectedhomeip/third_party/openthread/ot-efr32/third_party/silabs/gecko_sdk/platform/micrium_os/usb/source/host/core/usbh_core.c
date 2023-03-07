/***************************************************************************//**
 * @file
 * @brief USB Host General Operations
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

#if (defined(RTOS_MODULE_USB_HOST_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  <cpu/include/cpu.h>

#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/kal/kal_priv.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <common/include/rtos_prio.h>
#include  <usbh_cfg.h>

#include  <usb/include/host/usbh_core.h>
#include  <usb/include/host/usbh_core_langid.h>
#include  <usb/include/host/usbh_core_handle.h>

#include  <usb/source/host/cmd/usbh_cmd_priv.h>
#include  <usb/source/host/core/usbh_core_types_priv.h>
#include  <usb/source/host/core/usbh_core_priv.h>
#include  <usb/source/host/core/usbh_core_hub_priv.h>
#include  <usb/source/host/core/usbh_core_dev_priv.h>
#include  <usb/source/host/core/usbh_core_fnct_priv.h>
#include  <usb/source/host/core/usbh_core_if_priv.h>
#include  <usb/source/host/core/usbh_core_ep_priv.h>
#include  <usb/source/host/core/usbh_core_config_priv.h>
#include  <usb/source/host/core/usbh_core_class_priv.h>
#include  <usb/source/usb_ctrlr_priv.h>

#ifdef RTOS_MODULE_COMMON_SHELL_AVAIL
#include  <usb/source/host/cmd/usbh_cmd_priv.h>
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (USBH)
#define  RTOS_MODULE_CUR                                 RTOS_CFG_MODULE_USBH

#define  USBH_DBG_HOST_STATS_EN                          DEF_DISABLED

/********************************************************************************************************
 *                                           CONST CFG VALUES
 *******************************************************************************************************/

#define  USBH_CFG_OPTIMIZE_SPD_CFG_VAL                  .OptimizeSpd = { .HC_PerHostQty = 2u, .DevPerHostQty = 2u, .FnctPerConfigQty = 2u, .IF_PerFnctQty = 2u }

#define  USBH_CFG_INIT_ALLOC_CFG_VAL                    .InitAlloc = { .DevQtyTot = 2u, .FnctQtyTot = 2u, .IF_QtyTot = 4u, .EP_QtyTot = 8u, .EP_QtyOpenTot = 6u, \
                                                                       .URB_QtyTot = 12u, .HubFnctQtyTot = 1u, .HubEventQtyTot = 2u }

#define  USBH_CFG_INIT_VAL                              .MaxDescLen = 128u, \
  .BufAlignOctets = LIB_MEM_BUF_ALIGN_AUTO,                                 \
  .EventFnctsPtr = DEF_NULL,                                                \
  .MemSegPtr = DEF_NULL,                                                    \
  .MemSegBufPtr = DEF_NULL,                                                 \
  .AsyncTaskStkSizeElements = 512u,                                         \
  .AsyncTaskStkPtr = DEF_NULL,                                              \
  .HubTaskStkSizeElements = 768u,                                           \
  .HubTaskStkPtr = DEF_NULL

#if (USBH_DBG_HOST_STATS_EN == DEF_ENABLED)
#define  USBH_DBG_HOST_DEV_QTY                              8u

#define  USBH_DBG_HOST_STATS_RESET()                        Mem_Clr((void *)&USBH_DbgHostStats, \
                                                                    (CPU_SIZE_T) sizeof(USBH_DBG_HOST_STATS));
#define  USBH_DBG_HOST_STATS_GLOBAL_INC(stat) \
  USBH_DbgHostStats.stat++;
#define  USBH_DBG_HOST_STATS_EP_INC(ep_phy_nbr, dev_addr, stat) \
  USBH_DbgHostStats.EpTbl[dev_addr][ep_phy_nbr].stat++;

typedef struct USBH_Dbg_Host_Stats_Ctr {
  CPU_INT32U EventURB_Cmpl;
  CPU_INT32U EventURB_ListFree;
  CPU_INT32U EventNone;
} USBH_DBG_HOST_STATS_CTR;

typedef struct USBH_Dbg_Host_Stats {
  USBH_DBG_HOST_STATS_CTR EpTbl[USBH_DBG_HOST_DEV_QTY][USBH_DEV_NBR_EP];
  CPU_INT32U              EventPendCnt;
  CPU_INT32U              EventPostCnt;
  CPU_INT32U              EventErrCnt;
  CPU_INT32U              EventEmptyCnt;
} USBH_DBG_HOST_STATS;

static USBH_DBG_HOST_STATS USBH_DbgHostStats;
#else
#define  USBH_DBG_HOST_STATS_RESET()
#define  USBH_DBG_HOST_STATS_GLOBAL_INC(stat)
#define  USBH_DBG_HOST_STATS_EP_INC(ep_phy_nbr, dev_addr, stat)
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       DEFAULT CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const USBH_INIT_CFG USBH_InitCfgDflt = {
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  USBH_CFG_OPTIMIZE_SPD_CFG_VAL,
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  USBH_CFG_INIT_ALLOC_CFG_VAL,
#endif

  USBH_CFG_INIT_VAL
};
#endif

/********************************************************************************************************
 *                               AVAILABLE BANDWIDTH PER FRAME / BRANCH
 *
 * Note(s) : (1) Following table contains the available bandwidth in bytes per frame for periodic
 *               transfers in full- and high-speed.
 *
 *               (a) In low-, full-speed, a maximum of 1500 bytes can be transferred during a frame.
 *                   However, periodic transfers can use AT MOST 90% of the frame. The bandwidth in number
 *                   of bytes is then 1350.
 *
 *               (b) In high-speed, a maximum of 7500 bytes can be transferred during a micro-frame.
 *                   However, periodic transfers can use AT MOST 80% of the micro-frame. The bandwidth in
 *                   number of bytes is then 6000.
 *
 *           (2) See 'Universal Serial Bus specification, revision 2.0, section 5.6.4 and 5.7.4' for more
 *               information.
 *******************************************************************************************************/

const CPU_INT16U USBH_HC_BranchMaxPeriodicBW[] = { 1350u, 1350u, 6000u };

/********************************************************************************************************
 *                                           BANDWIDTH OVERHEAD
 *
 * Note(s) : (1) The unit of measure of bandwidth used is a byte.
 *
 *           (2) This table gives the bandwith overhead for interrupt and isochronous transfers. Note that
 *               these values are approximations.
 *
 *           +------------------+---------------------------------+----------------------------------+
 *           |\   Transfer type |                                 |                                  |
 *           | ---------------- |           Interrupt             |           Isochronous            |
 *           | Dev speed       \|                                 |                                  |
 *           +------------------+---------------------------------+----------------------------------+
 *           |                  | Protocol overhead: 19           |                                  |
 *           |       Low        | Bitstuff, Hub_LS_Setup, etc: 2  |               N/A                |
 *           |                  | Total: 21                       |                                  |
 *           +------------------+---------------------------------+----------------------------------+
 *           |                  | Protocol overhead: 13           | Protocol overhead: 9             |
 *           |       Full       | Bitstuff, etc: 5                | Bitstuff, etc: 4                 |
 *           |                  | Total: 18                       | Total: 13                        |
 *           +------------------+---------------------------------+----------------------------------+
 *           |                  | Protocol overhead: 55           | Protocol overhead: 38            |
 *           |       High       | Bitstuff, etc: 150              | Bitstuff, etc: 130               |
 *           |                  | Total: 205                      | Total: 168                       |
 *           +------------------+---------------------------------+----------------------------------+
 *
 *           (3) See 'Universal Serial Bus specification, revision 2.0, section 5.6.3, 5.7.3 and 5.11.3'
 *               for more information.
 *******************************************************************************************************/

const CPU_INT08U USBH_HC_BW_Overhead[][2u] = { { 21u, 0u },
                                               { 18u, 13u },
                                               { 205u, 168u } };

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

USBH *USBH_Ptr = DEF_NULL;                                      // Ptr to root USBH struct.

/********************************************************************************************************
 *                                           CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
USBH_INIT_CFG USBH_InitCfg = {
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  .OptimizeSpd = { .HC_PerHostQty = 0u, .DevPerHostQty = 0u, .FnctPerConfigQty = 0u, .IF_PerFnctQty = 0u },
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  .InitAlloc = { .DevQtyTot = 0u, .FnctQtyTot = 0u, .IF_QtyTot = 0u, .EP_QtyTot = 0u, .EP_QtyOpenTot = 0u,
                 .URB_QtyTot = 0u, .HubFnctQtyTot = 0u, .HubEventQtyTot = 0u },
#endif

  USBH_CFG_INIT_VAL
};
#else
extern USBH_INIT_CFG USBH_InitCfg;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void USBH_KernelInit(RTOS_ERR *p_err);

static void USBH_AsyncTaskHandler(void *p_arg);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       USBH_ConfigureBufAlignOctets()
 *
 * @brief    Configures the alignment of the internal buffers.
 *
 * @param    buf_align_octets    Buffer alignment, in octets.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the USB Host module is initialized via the
 *               USBH_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_ConfigureBufAlignOctets(CPU_SIZE_T buf_align_octets)
{
  RTOS_ASSERT_CRITICAL((USBH_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_InitCfg.BufAlignOctets = buf_align_octets;
}
#endif

/****************************************************************************************************//**
 *                                       USBH_ConfigureMaxDescLen()
 *
 * @brief    Configures the length of the buffer used to retrieve the USB descriptors from the
 *           devices.
 *
 * @param    max_desc_len    Length of the descriptor buffer.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the USB Host module is initialized via the
 *               USBH_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_ConfigureMaxDescLen(CPU_INT16U max_desc_len)
{
  RTOS_ASSERT_CRITICAL((USBH_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_InitCfg.MaxDescLen = max_desc_len;
}
#endif

/****************************************************************************************************//**
 *                                       USBH_ConfigureOptimizeSpdCfg()
 *
 * @brief    Sets the configurations required when optimize speed mode is enabled.
 *
 * @param    p_optimize_spd_cfg  Pointer to the structure containing the configurations for the
 *                               optimize speed mode.
 *
 * @note     (1) This function MUST be called before the USB Host module is initialized via the
 *               USBH_Init() function.
 *
 * @note     (2) This function MUST be called when the USBH_CFG_OPTIMIZE_SPD_EN configuration is set
 *               to DEF_ENABLED.
 *******************************************************************************************************/

#if ((RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED) \
  && (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED))
void USBH_ConfigureOptimizeSpdCfg(const USBH_CFG_OPTIMIZE_SPD *p_optimize_spd_cfg)
{
  RTOS_ASSERT_CRITICAL((USBH_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );
  RTOS_ASSERT_CRITICAL((p_optimize_spd_cfg != DEF_NULL), RTOS_ERR_NULL_PTR,; );

  USBH_InitCfg.OptimizeSpd = *p_optimize_spd_cfg;
}
#endif

/****************************************************************************************************//**
 *                                       USBH_ConfigureInitAllocCfg()
 *
 * @brief    Sets the configurations required when allocation at initialization mode is enabled.
 *
 * @param    p_init_alloc_cfg    Pointer to the structure containing the configurations for the
 *                               allocation at initialization mode.
 *
 * @note     (1) This function MUST be called before the USB Host module is initialized via the
 *               USBH_Init() function.
 *
 * @note     (2) This function MUST be called when the USBH_CFG_INIT_ALLOC_EN configuration is set
 *               to DEF_ENABLED.
 *******************************************************************************************************/

#if ((RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED) \
  && (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED))
void USBH_ConfigureInitAllocCfg(const USBH_CFG_INIT_ALLOC *p_init_alloc_cfg)
{
  RTOS_ASSERT_CRITICAL((USBH_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );
  RTOS_ASSERT_CRITICAL((p_init_alloc_cfg != DEF_NULL), RTOS_ERR_NULL_PTR,; );

  USBH_InitCfg.InitAlloc = *p_init_alloc_cfg;
}
#endif

/****************************************************************************************************//**
 *                                       USBH_ConfigureEventFncts()
 *
 * @brief    Sets the structure of callback that will be used by the USB host module to notify the
 *           application of certain events.
 *
 * @param    p_event_fncts   Pointer to a structure containing the event functions to call.
 *                           [Content MUST be persistent]
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the USB Host module is initialized via the
 *               USBH_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_ConfigureEventFncts(const USBH_EVENT_FNCTS *p_event_fncts)
{
  RTOS_ASSERT_CRITICAL((USBH_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_InitCfg.EventFnctsPtr = p_event_fncts;
}
#endif

/****************************************************************************************************//**
 *                                           USBH_ConfigureMemSeg()
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
 * @note     (2) This function MUST be called before the USB Host module is initialized via the
 *               USBH_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_ConfigureMemSeg(MEM_SEG *p_mem_seg,
                          MEM_SEG *p_mem_seg_buf)
{
  RTOS_ASSERT_CRITICAL((USBH_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_InitCfg.MemSegPtr = p_mem_seg;
  USBH_InitCfg.MemSegBufPtr = p_mem_seg_buf;
}
#endif

/****************************************************************************************************//**
 *                                       USBH_ConfigureAsyncTaskStk()
 *
 * @brief    Configures the USB host asynchronous task's stack.
 *
 * @param    stk_size_elements   Size, in stack elements, of the task's stack.
 *
 * @param    p_stk               Pointer to base of the task's stack. If DEF_NULL, stack will be
 *                               allocated from KAL's memory segment.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the USB Host module is initialized via the
 *               USBH_Init() function.
 *
 * @note     (3) In order to change the priority of the USB host asynchronous task, use the function
 *               USBH_AsyncTaskPrioSet().
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_ConfigureAsyncTaskStk(CPU_INT32U stk_size_elements,
                                void       *p_stk)
{
  RTOS_ASSERT_CRITICAL((USBH_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_InitCfg.AsyncTaskStkPtr = p_stk;
  USBH_InitCfg.AsyncTaskStkSizeElements = stk_size_elements;
}
#endif

/****************************************************************************************************//**
 *                                       USBH_ConfigureHubTaskStk()
 *
 * @brief    Configures the USB host hub task's stack.
 *
 * @param    stk_size_elements   Size, in stack elements, of the task's stack.
 *
 * @param    p_stk               Pointer to base of the task's stack. If DEF_NULL, stack will be
 *                               allocated from KAL's memory segment.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the USB Host module is initialized via the
 *               USBH_Init() function.
 *
 * @note     (3) In order to change the priority of the USB host hub task, use the function
 *               USBH_HUB_TaskPrioSet() available in file "usbh_core_hub.h".
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBH_ConfigureHubTaskStk(CPU_INT32U stk_size_elements,
                              void       *p_stk)
{
  RTOS_ASSERT_CRITICAL((USBH_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  USBH_InitCfg.HubTaskStkPtr = p_stk;
  USBH_InitCfg.HubTaskStkSizeElements = stk_size_elements;
}
#endif

/****************************************************************************************************//**
 *                                               USBH_Init()
 *
 * @brief    Initializes USB Host stack.
 *
 * @param    host_qty    Quantity of USB host.
 *
 * @param    p_err       Pointer to the variable that will receive one of these return error codes
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                           - RTOS_ERR_SEG_OVF
 *                           - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                           - RTOS_ERR_POOL_EMPTY
 *                           - RTOS_ERR_NOT_AVAIL
 *
 * @note     (1) USBH_Init() must be called:
 *           - (a) Only once from a product's application.
 *           - (b) After product's OS has been initialized.
 *******************************************************************************************************/
void USBH_Init(CPU_INT08U host_qty,
               RTOS_ERR   *p_err)
{
  CPU_INT08U      host_ix;
  CPU_INT16U      ep_qty;
  CPU_INT16U      ep_open_qty;
  CPU_INT16U      urb_qty;
  CPU_INT16U      if_qty;
  CPU_INT16U      fnct_qty;
  CPU_INT16U      dev_qty;
  CPU_INT16U      hub_fnct_qty;
  CPU_INT16U      hub_event_qty;
  KAL_TASK_HANDLE async_task_handle;
  USBH            *p_usbh;
  USBH_HOST       *p_host;
  CORE_DECLARE_IRQ_STATE;

  //                                                               ------------------- VALIDATE ARG -------------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  //                                                               ------------------- VALIDATE CFG -------------------
  RTOS_ASSERT_DBG_ERR_SET(((USBH_InitCfg.MaxDescLen >= USBH_DESC_LEN_DEV)
                           && (host_qty > 0u)
                           && (host_qty <= 16u)), *p_err, RTOS_ERR_INVALID_CFG,; );

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  //                                                               Ensure USBH_ConfigureOptimizeSpdCfg() was called.
  RTOS_ASSERT_DBG_ERR_SET((USBH_InitCfg.OptimizeSpd.DevPerHostQty != 0u), *p_err, RTOS_ERR_NOT_READY,; );

  RTOS_ASSERT_DBG_ERR_SET(((USBH_InitCfg.OptimizeSpd.HC_PerHostQty > 0u)
                           && (USBH_InitCfg.OptimizeSpd.HC_PerHostQty <= 8u)
                           && (USBH_InitCfg.OptimizeSpd.DevPerHostQty > 0u)
                           && (USBH_InitCfg.OptimizeSpd.FnctPerConfigQty > 0u)
                           && (USBH_InitCfg.OptimizeSpd.IF_PerFnctQty > 0u)), *p_err, RTOS_ERR_INVALID_CFG,; );
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  //                                                               Ensure USBH_ConfigureInitAllocCfg() was called.
  RTOS_ASSERT_DBG_ERR_SET((USBH_InitCfg.InitAlloc.DevQtyTot != 0u), *p_err, RTOS_ERR_NOT_READY,; );

  RTOS_ASSERT_DBG_ERR_SET(((USBH_InitCfg.InitAlloc.EP_QtyOpenTot > 0u)
                           && (USBH_InitCfg.InitAlloc.EP_QtyTot >= USBH_InitCfg.InitAlloc.EP_QtyOpenTot)
                           && (USBH_InitCfg.InitAlloc.IF_QtyTot > 0u)
                           && (USBH_InitCfg.InitAlloc.FnctQtyTot > 0u)
                           && (USBH_InitCfg.InitAlloc.IF_QtyTot >= USBH_InitCfg.InitAlloc.FnctQtyTot)
                           && (USBH_InitCfg.InitAlloc.URB_QtyTot > 0u)), *p_err, RTOS_ERR_INVALID_CFG,; );
#endif // ----- (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED) ------

  USBH_KernelInit(p_err);                                       // Initialize Kernel Abstraction Layer (KAL).
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_usbh = (USBH *)Mem_SegAlloc("USBH - Root struct",           // Alloc root usbh struct from mem seg.
                                USBH_InitCfg.MemSegPtr,
                                sizeof(USBH),
                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_usbh->AsyncTaskHandle = KAL_TaskHandleNull;
  p_usbh->HostQty = host_qty;
  p_usbh->StdReqTimeout = 5000u;
  p_usbh->ClassDrvItemHeadPtr = DEF_NULL;
  p_usbh->DescDevPtr = DEF_NULL;
  p_usbh->AsyncEventQHeadPtr = DEF_NULL;
  p_usbh->AsyncEventQTailPtr = DEF_NULL;
  p_usbh->AsyncEventFreeHeadPtr = DEF_NULL;
  p_usbh->HostNbrNext = 0u;
#if (USBH_CFG_STR_EN == DEF_ENABLED)
  p_usbh->PreferredStrLangID = USBH_LANGID_ENGLISH_UNITEDSTATES;
#endif

  //                                                               Alloc desc buf.
  p_usbh->DescBufPtr = (CPU_INT08U *)Mem_SegAllocHW("USBH - Descriptor buffer",
                                                    USBH_InitCfg.MemSegPtr,
                                                    USBH_InitCfg.MaxDescLen,
                                                    USBH_InitCfg.BufAlignOctets,
                                                    DEF_NULL,
                                                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Alloc hosts.
  p_usbh->HostTblPtr = (USBH_HOST *)Mem_SegAlloc("USBH - host table",
                                                 USBH_InitCfg.MemSegPtr,
                                                 (sizeof(USBH_HOST) * host_qty),
                                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               -------------------- INIT HOSTS --------------------
  for (host_ix = 0u; host_ix < host_qty; host_ix++) {
    p_host = &p_usbh->HostTblPtr[host_ix];

    p_host->HC_NbrNext = 0u;
    p_host->Ix = host_ix;

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
    {
      CPU_INT08U nbr_entry = (USBH_InitCfg.OptimizeSpd.DevPerHostQty + DEF_INT_32_NBR_BITS - 1u) / DEF_INT_32_NBR_BITS;
      CPU_INT08U cnt;

      //                                                           Alloc HC tbl.
      p_host->HC_TblPtr = (USBH_HC **)Mem_SegAlloc("USBH - HC ptr table",
                                                   USBH_InitCfg.MemSegPtr,
                                                   sizeof(USBH_HC *) * USBH_InitCfg.OptimizeSpd.HC_PerHostQty,
                                                   p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }

      //                                                           Alloc dev tbl.
      p_host->DevTbl = (USBH_DEV **)Mem_SegAlloc("USBH - Dev ptr table",
                                                 USBH_InitCfg.MemSegPtr,
                                                 sizeof(USBH_DEV *) * USBH_InitCfg.OptimizeSpd.DevPerHostQty,
                                                 p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }

      //                                                           -------------- PREPARE DEV ADDR POOL ---------------
      //                                                           Alloc dev addr pool.
      p_host->DevAddrPool = (CPU_INT32U *)Mem_SegAlloc("USBH - Dev address pool",
                                                       USBH_InitCfg.MemSegPtr,
                                                       sizeof(CPU_INT32U) * nbr_entry,
                                                       p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }

      //                                                           Mark all dev addr as avail.
      for (cnt = 0u; cnt < (nbr_entry - 1u); cnt++) {
        p_host->DevAddrPool[cnt] = DEF_INT_32_MASK;
      }

      p_host->DevAddrPool[cnt] = DEF_BIT_FIELD(USBH_InitCfg.OptimizeSpd.DevPerHostQty % DEF_INT_32_NBR_BITS, 0u);
    }
#else
    p_host->DevAddrPool = DEF_INT_32_MASK;                      // All dev addr available.
    p_host->DevHeadPtr = DEF_NULL;

    p_host->HC_HeadPtr = DEF_NULL;
#endif
  }

  CORE_ENTER_ATOMIC();
  USBH_Ptr = p_usbh;
  CORE_EXIT_ATOMIC();

  //                                                               ---------- CREATE REQUIRED KERNEL OBJECTS ----------
  USBH_Ptr->AsyncEventSem = KAL_SemCreate("USBH - Async event sem",
                                          DEF_NULL,
                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Create async task.
  async_task_handle = KAL_TaskAlloc("USBH - EP Async task",
                                    (CPU_STK *)USBH_InitCfg.AsyncTaskStkPtr,
                                    USBH_InitCfg.AsyncTaskStkSizeElements,
                                    DEF_NULL,
                                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  KAL_TaskCreate(async_task_handle,
                 USBH_AsyncTaskHandler,
                 DEF_NULL,
                 USBH_ASYNC_TASK_PRIO_DFLT,
                 DEF_NULL,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  CORE_ENTER_ATOMIC();
  USBH_Ptr->AsyncTaskHandle = async_task_handle;
  CORE_EXIT_ATOMIC();

  USBH_Ptr->AsyncEventListLock = KAL_LockCreate("USBH - Async Event List Lock",
                                                DEF_NULL,
                                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

#if (USBH_CFG_INIT_ALLOC_EN == DEF_DISABLED)
  ep_qty = USBH_OBJ_QTY_UNLIMITED;
  ep_open_qty = USBH_OBJ_QTY_UNLIMITED;
  urb_qty = USBH_OBJ_QTY_UNLIMITED;
  if_qty = USBH_OBJ_QTY_UNLIMITED;
  fnct_qty = USBH_OBJ_QTY_UNLIMITED;
  dev_qty = USBH_OBJ_QTY_UNLIMITED;
  hub_fnct_qty = USBH_OBJ_QTY_UNLIMITED;
  hub_event_qty = USBH_OBJ_QTY_UNLIMITED;
#else
  ep_qty = USBH_InitCfg.InitAlloc.EP_QtyTot;
  ep_open_qty = USBH_InitCfg.InitAlloc.EP_QtyOpenTot;
  urb_qty = USBH_InitCfg.InitAlloc.URB_QtyTot;
  if_qty = USBH_InitCfg.InitAlloc.IF_QtyTot;
  fnct_qty = USBH_InitCfg.InitAlloc.FnctQtyTot;
  dev_qty = USBH_InitCfg.InitAlloc.DevQtyTot;
  hub_fnct_qty = USBH_InitCfg.InitAlloc.HubFnctQtyTot;
  hub_event_qty = USBH_InitCfg.InitAlloc.HubEventQtyTot;
#endif

  USBH_EP_Init(ep_qty, ep_open_qty, urb_qty, p_err);            // Init endpoint module.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBH_IF_Init(if_qty, p_err);                                  // Init interface module.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBH_FnctInit(fnct_qty, p_err);                               // Init function module.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBH_DevInit(dev_qty, p_err);                                 // Init device module.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               --------------- ADD HUB CLASS DRIVER ---------------
  USBH_HUB_Init(hub_fnct_qty,
                hub_event_qty,
                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

#ifdef RTOS_MODULE_COMMON_SHELL_AVAIL
  USBH_ShellCmdInit(p_err);
#endif
}

/****************************************************************************************************//**
 *                                               USBH_UnInit()
 *
 * @brief    Un-initializes the USB Host stack.
 *
 * @param    p_err   Pointer to the variable that will receive one of these return error codes
 *                   from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_POOL_UNLIMITED
 *                       - RTOS_ERR_SEG_OVF
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_INVALID_STATE
 *
 * @note     (1) USBH_UnInit() must be called after all the HCs have been stopped.
 *
 * @note     (2) Once the USB host has been uninitialized, all the used memory segments can be
 *               cleared and re-used for other purposes.
 *******************************************************************************************************/

#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
void USBH_UnInit(RTOS_ERR *p_err)
{
  USBH_HOST         *p_host;
  MEM_DYN_POOL_INFO dev_pool_info;
  MEM_DYN_POOL_INFO ep_mgmt_pool_info;
  CPU_INT08U        host_ix;
  CORE_DECLARE_IRQ_STATE;

  //                                                               Make sure HCs are stopped. See Note #1.
  for (host_ix = 0u; host_ix < USBH_Ptr->HostQty; host_ix++) {
    USBH_HC    *p_hc;
    CPU_INT08U hc_nbr_ix = 0u;

    p_host = &USBH_Ptr->HostTblPtr[host_ix];

    do {
      p_hc = USBH_HC_PtrGet(p_host,
                            hc_nbr_ix,
                            p_err);
      if (p_hc == DEF_NULL) {
        break;
      }
      hc_nbr_ix++;

      CORE_ENTER_ATOMIC();
      if (p_hc->IsStopped == DEF_NO) {
        CORE_EXIT_ATOMIC();
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      }
      CORE_EXIT_ATOMIC();
    } while (p_hc != DEF_NULL);
  }

  (void)Mem_DynPoolBlkNbrAvailGet(&USBH_Ptr->DevPool,
                                  &dev_pool_info,
                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  (void)Mem_DynPoolBlkNbrAvailGet(&USBH_Ptr->EP_MgmtPool,
                                  &ep_mgmt_pool_info,
                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Un-Init HCs.
  for (host_ix = 0u; host_ix < USBH_Ptr->HostQty; host_ix++) {
    USBH_HC    *p_hc;
    CPU_INT08U hc_nbr_ix = 0u;

    p_host = &USBH_Ptr->HostTblPtr[host_ix];

    do {
      p_hc = USBH_HC_PtrGet(p_host,
                            hc_nbr_ix,
                            p_err);
      if (p_hc == DEF_NULL) {
        break;
      }
      hc_nbr_ix++;

      if (p_hc->Drv.API_Ptr->UnInit != DEF_NULL) {
        p_hc->Drv.API_Ptr->UnInit(&p_hc->Drv,
                                  p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          return;
        }
      }
    } while (p_hc != DEF_NULL);
  }

  USBH_ClassesUnInit();                                         // Un-init class drivers.

  USBH_HUB_UnInit(p_err);                                       // Un-init HUB class driver.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Uninit devices KAL rsrc.
  while (dev_pool_info.BlkNbrAvailCnt != 0u) {
    USBH_DEV *p_dev;

    p_dev = (USBH_DEV *)Mem_DynPoolBlkGet(&USBH_Ptr->DevPool,
                                          p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Unable to retrieve device to free KAL rsrcs -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    }
    dev_pool_info.BlkNbrAvailCnt--;

    KAL_LockDel(p_dev->DfltEP_LockHandle);
    KAL_MonDel(p_dev->MonHandle);
  }

  //                                                               Uninit EP mgmt KAL rsrc.
  while (ep_mgmt_pool_info.BlkNbrAvailCnt != 0u) {
    USBH_EP_MGMT *p_ep_mgmt;

    p_ep_mgmt = (USBH_EP_MGMT *)Mem_DynPoolBlkGet(&USBH_Ptr->EP_MgmtPool,
                                                  p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("Unable to retrieve EP mgmt to free KAL rsrcs -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    }

    ep_mgmt_pool_info.BlkNbrAvailCnt--;

    KAL_MonDel(p_ep_mgmt->MonHandle);
  }

  KAL_SemDel(USBH_Ptr->AsyncEventSem);
  KAL_TaskDel(USBH_Ptr->AsyncTaskHandle);
  KAL_LockDel(USBH_Ptr->AsyncEventListLock);

  USBH_Ptr = DEF_NULL;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return;
}
#endif

/****************************************************************************************************//**
 *                                               USBH_HC_Add()
 *
 * @brief    Add host controller.
 *
 * @param    name            Name of USB controller.
 *
 * @param    p_hc_cfg_ext    Pointer to the extended USB host controller configuration. Can be null
 *                           when USBH_CFG_OPTIMIZE_SPD_EN and USBH_CFG_INIT_ALLOC_EN are set
 *                           to DEF_DISABLED.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                               - RTOS_ERR_POOL_EMPTY
 *                               - RTOS_ERR_ALLOC
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                               - RTOS_ERR_SEG_OVF
 *
 * @return   Handle to host controller.
 *******************************************************************************************************/
USBH_HC_HANDLE USBH_HC_Add(const CPU_CHAR        *name,
                           const USBH_HC_CFG_EXT *p_hc_cfg_ext,
                           RTOS_ERR              *p_err)
{
  USB_CTRLR_TYPE  ctrlr_type;
  USBH_HC_HW_INFO *p_hw_info;
  USBH_HC_HANDLE  main_ctrlr_handle;
  USBH_HC_HANDLE  hc_handle;
  CPU_CHAR        *main_ctrlr_name;
  CPU_INT08U      host_ix;
  CPU_INT08U      hc_ix = USBH_HC_NBR_NONE;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, USBH_HC_HANDLE_INVALID);

  p_hw_info = USB_CtrlrHostHwInfoGet(name,
                                     &main_ctrlr_name,
                                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBH_HC_HANDLE_INVALID);
  }

  RTOS_ASSERT_DBG(p_hw_info != DEF_NULL, RTOS_ERR_NOT_FOUND, USBH_HC_HANDLE_INVALID);

  ctrlr_type = USB_CtrlrTypeGet(name, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (USBH_HC_HANDLE_INVALID);
  }

  switch (ctrlr_type) {
    case USB_CTRLR_TYPE_HOST_COMPANION:
      main_ctrlr_handle = USBH_HC_HandleGetFromName(main_ctrlr_name);
      if (main_ctrlr_handle == USBH_HC_HANDLE_INVALID) {
        return (USBH_HC_HANDLE_INVALID);
      }

      host_ix = USBH_HC_HANDLE_HOST_IX_GET(main_ctrlr_handle);
      break;

    case USB_CTRLR_TYPE_STD:
    default:
      CORE_ENTER_ATOMIC();
      if (USBH_Ptr->HostNbrNext >= USBH_Ptr->HostQty) {
        CORE_EXIT_ATOMIC();

        RTOS_ERR_SET(*p_err, RTOS_ERR_ALLOC);
        return (USBH_HC_HANDLE_INVALID);
      }

      host_ix = USBH_Ptr->HostNbrNext;
      USBH_Ptr->HostNbrNext++;
      CORE_EXIT_ATOMIC();
      break;
  }

  switch (p_hw_info->HC_Type) {
    case USBH_HC_TYPE_PIPE:
#ifdef RTOS_MODULE_USB_HOST_PBHCI_AVAIL
      hc_ix = USBH_PBHCI_HC_AddInternal(name,
                                        host_ix,
                                        (USBH_PBHCI_HC_HW_INFO *)p_hw_info,
                                        p_hc_cfg_ext,
                                        p_err);
#else
      RTOS_ASSERT_DBG(DEF_TRUE, RTOS_ERR_NOT_AVAIL,; );
#endif
      break;

    case USBH_HC_TYPE_LIST:
      hc_ix = USBH_HC_AddInternal(name,
                                  host_ix,
                                  &((USBH_HC_HCD_HW_INFO *)p_hw_info)->HW_Info,
                                  ((USBH_HC_HCD_HW_INFO *)p_hw_info)->DrvAPI_Ptr,
                                  ((USBH_HC_HCD_HW_INFO *)p_hw_info)->RH_API_Ptr,
                                  ((USBH_HC_HCD_HW_INFO *)p_hw_info)->BSP_API_Ptr,
                                  p_hc_cfg_ext,
                                  p_err);
      break;

    default:
      RTOS_ASSERT_DBG(DEF_TRUE, RTOS_ERR_INVALID_TYPE, USBH_HC_HANDLE_INVALID);
      break;
  }

  hc_handle = (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) ? USBH_HC_HANDLE_CREATE(host_ix, hc_ix)
              : USBH_HC_HANDLE_INVALID;

  return (hc_handle);
}

/****************************************************************************************************//**
 *                                       USBH_HC_HandleGetFromName()
 *
 * @brief    Gets Host Controller handle from it's name.
 *
 * @param    name    Pointer to a string containing host controller's name.
 *
 * @return   Handle to host controller.
 *******************************************************************************************************/
USBH_HC_HANDLE USBH_HC_HandleGetFromName(const CPU_CHAR *name)
{
  CPU_INT08U host_qty;
  CPU_INT08U host_ix;
  CPU_INT08U hc_handle = USBH_HC_HANDLE_INVALID;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  host_qty = USBH_Ptr->HostQty;
  CORE_EXIT_ATOMIC();

  for (host_ix = 0u; host_ix < host_qty; host_ix++) {
    CPU_INT08U hc_qty;
    CPU_INT08U hc_ix;
    USBH_HOST  *p_host = &USBH_Ptr->HostTblPtr[host_ix];

    CORE_ENTER_ATOMIC();
    hc_qty = p_host->HC_NbrNext;
    CORE_EXIT_ATOMIC();

    for (hc_ix = 0u; hc_ix < hc_qty; hc_ix++) {
      CPU_INT16S cmp_result;
      USBH_HC    *p_hc;
      RTOS_ERR   err;

      p_hc = USBH_HC_PtrGet(p_host, hc_ix, &err);
      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
        return (hc_handle);
      }

      cmp_result = Str_Cmp(name, p_hc->Name);
      if (cmp_result == 0) {
        hc_handle = USBH_HC_HANDLE_CREATE(host_ix, hc_ix);

        return (hc_handle);
      }
    }
  }

  return (hc_handle);
}

/****************************************************************************************************//**
 *                                               USBH_HC_Start()
 *
 * @brief    Starts the given host controller.
 *
 * @param    hc_handle   Handle to host controller.
 *
 * @param    p_err       Pointer to the variable that will receive one of these return error codes
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_FAIL
 *                           - RTOS_ERR_IO
 *                           - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                           - RTOS_ERR_POOL_EMPTY
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_INVALID_STATE
 *                           - RTOS_ERR_USB_URB_ALLOC
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_SEG_OVF
 *                           - RTOS_ERR_USB_INVALID_EP
 *                           - RTOS_ERR_TIMEOUT
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_TX
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_WOULD_OVF
 *******************************************************************************************************/
void USBH_HC_Start(USBH_HC_HANDLE hc_handle,
                   RTOS_ERR       *p_err)
{
  CPU_BOOLEAN init_rh = DEF_NO;
  CPU_INT08U  host_nbr;
  CPU_INT08U  hc_nbr;
  USBH_HOST   *p_host;
  USBH_HC     *p_hc;
  USBH_HC_DRV *p_hc_drv;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((hc_handle != USBH_HC_HANDLE_INVALID), *p_err, RTOS_ERR_INVALID_HANDLE,; );

  host_nbr = USBH_HC_HANDLE_HOST_IX_GET(hc_handle);
  hc_nbr = USBH_HC_HANDLE_HC_IX_GET(hc_handle);

  p_host = &USBH_Ptr->HostTblPtr[host_nbr];
  RTOS_ASSERT_DBG_ERR_SET((hc_nbr < p_host->HC_NbrNext), *p_err, RTOS_ERR_INVALID_ARG,; );

  p_hc = USBH_HC_PtrGet(p_host, hc_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  CORE_ENTER_ATOMIC();
  if (p_hc->RH_Fnct.State == USBH_HUB_STATE_CONN) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
    return;
  } else if (p_hc->RH_Fnct.State == USBH_HUB_STATE_NONE) {
    p_hc->RH_Fnct.State = USBH_HUB_STATE_CONN;
    CORE_EXIT_ATOMIC();

    init_rh = DEF_YES;
  } else {
    CORE_EXIT_ATOMIC();
  }

  p_hc_drv = &p_hc->Drv;

#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
  CORE_ENTER_ATOMIC();
  p_hc->IsStopped = DEF_NO;
  CORE_EXIT_ATOMIC();
#endif

  p_hc_drv->API_Ptr->Start(p_hc_drv, p_err);                    // Start host controller driver.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
    CORE_ENTER_ATOMIC();
    p_hc->IsStopped = DEF_YES;
    CORE_EXIT_ATOMIC();
#endif

    goto err_return;
  }

  if (init_rh == DEF_YES) {
    USBH_HUB_RootInit(&p_hc->RH_Fnct, p_host, p_hc, p_err);     // Init Root Hub.
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto err_return;
    }
  }

  USBH_HUB_RootStart(p_hc, p_err);                              // Start root hub.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto err_return;
  }

  LOG_VRB(("Started HC #", (u)hc_nbr, " of host #", (u)host_nbr));

  return;

err_return:

  p_hc->RH_Fnct.State = USBH_HUB_STATE_NONE;

  return;
}

/****************************************************************************************************//**
 *                                               USBH_HC_Stop()
 *
 * @brief    Stops the given host controller.
 *
 * @param    hc_handle       Handle to host controller.
 *
 * @param    callback_fnct   Function that will be called after operation is completed. Can be null.
 *
 * @param    p_arg           Pointer to the argument that will be passed to the callback function.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_ALLOC
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_NOT_AVAIL
 *******************************************************************************************************/
void USBH_HC_Stop(USBH_HC_HANDLE    hc_handle,
                  USBH_HC_OPER_CMPL callback_fnct,
                  void              *p_arg,
                  RTOS_ERR          *p_err)
{
  CPU_INT08U             host_nbr;
  CPU_INT08U             hc_nbr;
  USBH_HOST              *p_host;
  USBH_HC                *p_hc;
  USBH_HUB_EVENT         *p_hub_event;
  USBH_HUB_EVENT_HC_OPER *p_hc_oper_arg;

  //                                                               Check as much as possible, before posting event.
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((hc_handle != USBH_HC_HANDLE_INVALID), *p_err, RTOS_ERR_INVALID_HANDLE,; );

  host_nbr = USBH_HC_HANDLE_HOST_IX_GET(hc_handle);
  hc_nbr = USBH_HC_HANDLE_HC_IX_GET(hc_handle);

  p_host = &USBH_Ptr->HostTblPtr[host_nbr];

  p_hc = USBH_HC_PtrGet(p_host, hc_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_hub_event = USBH_HUB_EventAlloc();
  if (p_hub_event == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALLOC);
    return;
  }

  p_hub_event->Type = USBH_HUB_EVENT_TYPE_HC_OPER;

  p_hc_oper_arg = (USBH_HUB_EVENT_HC_OPER *)p_hub_event->ArgPtr;

  p_hc_oper_arg->HC_Ptr = p_hc;
  p_hc_oper_arg->Type = USBH_HC_OPER_TYPE_STOP;
  p_hc_oper_arg->HC_OperCmplCallback = callback_fnct;
  p_hc_oper_arg->CallbackArgPtr = p_arg;

  USBH_HUB_EventQPost(p_hub_event,
                      p_err);

  return;
}

/****************************************************************************************************//**
 *                                               USBH_HC_Suspend()
 *
 * @brief    Suspends the host controller.
 *
 * @param    hc_handle       Handle to host controller.
 *
 * @param    callback_fnct   Pointer to the function that will be called after the operation is completed.
 *
 * @param    p_arg           Pointer to the argument that will be passed to the callback function.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_ALLOC
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_NOT_AVAIL
 *******************************************************************************************************/
void USBH_HC_Suspend(USBH_HC_HANDLE    hc_handle,
                     USBH_HC_OPER_CMPL callback_fnct,
                     void              *p_arg,
                     RTOS_ERR          *p_err)
{
  CPU_INT08U             host_nbr;
  CPU_INT08U             hc_nbr;
  USBH_HOST              *p_host;
  USBH_HC                *p_hc;
  USBH_HUB_EVENT         *p_hub_event;
  USBH_HUB_EVENT_HC_OPER *p_hc_oper_arg;

  //                                                               Check as much as possible, before posting event.
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((hc_handle != USBH_HC_HANDLE_INVALID), *p_err, RTOS_ERR_INVALID_HANDLE,; );

  host_nbr = USBH_HC_HANDLE_HOST_IX_GET(hc_handle);
  hc_nbr = USBH_HC_HANDLE_HC_IX_GET(hc_handle);

  p_host = &USBH_Ptr->HostTblPtr[host_nbr];
  p_hc = USBH_HC_PtrGet(p_host, hc_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_hub_event = USBH_HUB_EventAlloc();
  if (p_hub_event == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALLOC);
    return;
  }

  p_hub_event->Type = USBH_HUB_EVENT_TYPE_HC_OPER;

  p_hc_oper_arg = (USBH_HUB_EVENT_HC_OPER *)p_hub_event->ArgPtr;

  p_hc_oper_arg->HC_Ptr = p_hc;
  p_hc_oper_arg->Type = USBH_HC_OPER_TYPE_SUSPEND;
  p_hc_oper_arg->HC_OperCmplCallback = callback_fnct;
  p_hc_oper_arg->CallbackArgPtr = p_arg;

  USBH_HUB_EventQPost(p_hub_event,
                      p_err);

  return;
}

/****************************************************************************************************//**
 *                                               USBH_HC_Resume()
 *
 * @brief    Resumes the host controller.
 *
 * @param    hc_handle       Handle to host controller.
 *
 * @param    callback_fnct   Function that will be called after the operation is completed.
 *
 * @param    p_arg           Pointer to the argument that will be passed to the callback function.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_ALLOC
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_NOT_AVAIL
 *******************************************************************************************************/
void USBH_HC_Resume(USBH_HC_HANDLE    hc_handle,
                    USBH_HC_OPER_CMPL callback_fnct,
                    void              *p_arg,
                    RTOS_ERR          *p_err)
{
  CPU_INT08U             host_nbr;
  CPU_INT08U             hc_nbr;
  USBH_HOST              *p_host;
  USBH_HC                *p_hc;
  USBH_HUB_EVENT         *p_hub_event;
  USBH_HUB_EVENT_HC_OPER *p_hc_oper_arg;

  //                                                               Check as much as possible, before posting event.
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((hc_handle != USBH_HC_HANDLE_INVALID), *p_err, RTOS_ERR_INVALID_HANDLE,; );

  host_nbr = USBH_HC_HANDLE_HOST_IX_GET(hc_handle);
  hc_nbr = USBH_HC_HANDLE_HC_IX_GET(hc_handle);

  p_host = &USBH_Ptr->HostTblPtr[host_nbr];
  p_hc = USBH_HC_PtrGet(p_host, hc_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_hub_event = USBH_HUB_EventAlloc();
  if (p_hub_event == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALLOC);
    return;
  }

  p_hub_event->Type = USBH_HUB_EVENT_TYPE_HC_OPER;

  p_hc_oper_arg = (USBH_HUB_EVENT_HC_OPER *)p_hub_event->ArgPtr;

  p_hc_oper_arg->HC_Ptr = p_hc;
  p_hc_oper_arg->Type = USBH_HC_OPER_TYPE_RESUME;
  p_hc_oper_arg->HC_OperCmplCallback = callback_fnct;
  p_hc_oper_arg->CallbackArgPtr = p_arg;

  USBH_HUB_EventQPost(p_hub_event,
                      p_err);

  return;
}

/****************************************************************************************************//**
 *                                           USBH_HC_FrameNbrGet()
 *
 * @brief    Retrieves the current frame number for the given host controller.
 *
 * @param    hc_handle   Handle to host controller.
 *
 * @param    p_err       Pointer to the variable that will receive this return error code
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *
 * @return   Current frame number processed by Host Controller, if successful,
 *           0,                                                 if unsuccessful.
 *******************************************************************************************************/
CPU_INT16U USBH_HC_FrameNbrGet(USBH_HC_HANDLE hc_handle,
                               RTOS_ERR       *p_err)
{
  CPU_INT08U  host_nbr;
  CPU_INT08U  hc_nbr;
  CPU_INT16U  frame_nbr;
  USBH_HOST   *p_host;
  USBH_HC_DRV *p_hc_drv;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);
  RTOS_ASSERT_DBG_ERR_SET((hc_handle != USBH_HC_HANDLE_INVALID), *p_err, RTOS_ERR_INVALID_HANDLE, 0u);

  host_nbr = USBH_HC_HANDLE_HOST_IX_GET(hc_handle);
  hc_nbr = USBH_HC_HANDLE_HC_IX_GET(hc_handle);

  p_host = &USBH_Ptr->HostTblPtr[host_nbr];
  p_hc_drv = &USBH_HC_PtrGet(p_host, hc_nbr, p_err)->Drv;
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (0u);
  }

  frame_nbr = p_hc_drv->API_Ptr->FrmNbrGet(p_hc_drv);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (frame_nbr);
}

/****************************************************************************************************//**
 *                                           USBH_AsyncTaskPrioSet()
 *
 * @brief    Assigns a new priority to the USB host async task.
 *
 * @param    prio    New priority of the the async task.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_ARG
 *
 * @note     (1) This function cannot be called before the USB Host module has been initialized via
 *               the USBH_Init() function.
 *******************************************************************************************************/
void USBH_AsyncTaskPrioSet(CPU_INT08U prio,
                           RTOS_ERR   *p_err)
{
  KAL_TASK_HANDLE async_task_handle;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((USBH_Ptr != DEF_NULL), RTOS_ERR_NOT_INIT,; );

  async_task_handle = USBH_Ptr->AsyncTaskHandle;
  CORE_EXIT_ATOMIC();

  KAL_TaskPrioSet(async_task_handle,
                  prio,
                  p_err);
}

/****************************************************************************************************//**
 *                                           USBH_StdReqTimeoutSet()
 *
 * @brief    Assigns a new timeout delay for the USB standard requests.
 *
 * @param    std_req_timeout_ms  New timeout, in milliseconds.
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error
 *                               code(s) from this function:
 *                                   - RTOS_ERR_NONE
 *
 * @note     (1) This function cannot be called before the USB Host module has been initialized via
 *               the USBH_Init() function.
 *******************************************************************************************************/
void USBH_StdReqTimeoutSet(CPU_INT32U std_req_timeout_ms,
                           RTOS_ERR   *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((USBH_Ptr != DEF_NULL), RTOS_ERR_NOT_INIT,; );

  USBH_Ptr->StdReqTimeout = std_req_timeout_ms;
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                       USBH_PreferredStrLangID_Set()
 *
 * @brief    Sets the prefferred language ID to use when retrieving strings from the device.
 *
 * @param    preferred_str_lang_id   ID of the prefferred language.
 *                                   See file "usbh_core_langid.h" for a list of language IDs.
 *
 * @param    p_err                   Pointer to the variable that will receive one of the following
 *                                   error code(s) from this function:
 *                                       - RTOS_ERR_NONE
 *
 * @note     (1) This function cannot be called before the USB Host module has been initialized via
 *               the USBH_Init() function.
 *******************************************************************************************************/

#if (USBH_CFG_STR_EN == DEF_ENABLED)
void USBH_PreferredStrLangID_Set(CPU_INT16U preferred_str_lang_id,
                                 RTOS_ERR   *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((USBH_Ptr != DEF_NULL), RTOS_ERR_NOT_INIT,; );

  USBH_Ptr->PreferredStrLangID = preferred_str_lang_id;
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           STACK LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBH_HC_AddInternal()
 *
 * @brief    Adds the host controller to a given host.
 *
 * @param    name            USB controller name.
 *
 * @param    host_nbr        Host number.
 *
 * @param    p_hw_info       Pointer to hardware information structure.
 *
 * @param    p_drv_api       Pointer to driver API structure.
 *
 * @param    p_rh_api        Pointer to root hub driver API structure.
 *
 * @param    p_bsp_api       Pointer to BSP API structure.
 *
 * @param    p_hc_cfg_ext    Pointer to the extended USB host controller configuration. Can be null
 *                           when USBH_CFG_OPTIMIZE_SPD_EN and USBH_CFG_INIT_ALLOC_EN are set
 *                           to DEF_DISABLED.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_SEG_OVF
 *                               - RTOS_ERR_ALLOC
 *
 * @return   Host controller number.
 *******************************************************************************************************/
CPU_INT08U USBH_HC_AddInternal(const CPU_CHAR        *name,
                               CPU_INT08U            host_nbr,
                               USBH_HC_HW_INFO       *p_hw_info,
                               USBH_HC_DRV_API       *p_drv_api,
                               USBH_HC_RH_API        *p_rh_api,
                               USBH_HC_BSP_API       *p_bsp_api,
                               const USBH_HC_CFG_EXT *p_hc_cfg_ext,
                               RTOS_ERR              *p_err)
{
  CPU_INT08U  hc_nbr;
  USBH_HOST   *p_host;
  USBH_HC     *p_hc;
  USBH_HC_DRV *p_hc_drv;
  CORE_DECLARE_IRQ_STATE;

  if (p_hw_info->DataBufDedicatedMemInfoPtr != DEF_NULL) {
    RTOS_ASSERT_DBG_ERR_SET(((p_hc_cfg_ext != DEF_NULL)
                             && (p_hc_cfg_ext->DedicatedMemCfgPtr != DEF_NULL)), *p_err, RTOS_ERR_NULL_PTR, USBH_HC_NBR_NONE);
  } else {
    RTOS_ASSERT_DBG_ERR_SET(((p_hc_cfg_ext == DEF_NULL)
                             || (p_hc_cfg_ext->DedicatedMemCfgPtr == DEF_NULL)), *p_err, RTOS_ERR_NULL_PTR, USBH_HC_NBR_NONE);
  }

  p_host = &USBH_Ptr->HostTblPtr[host_nbr];

  CORE_ENTER_ATOMIC();
  hc_nbr = p_host->HC_NbrNext;
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  if (hc_nbr >= USBH_InitCfg.OptimizeSpd.HC_PerHostQty) {
    CORE_EXIT_ATOMIC();

    RTOS_ERR_SET(*p_err, RTOS_ERR_ALLOC);
    return (USBH_HC_NBR_NONE);
  }
#endif
  p_host->HC_NbrNext++;
  CORE_EXIT_ATOMIC();

  p_hc = (USBH_HC *)Mem_SegAlloc("USBH - Host controller",      // Alloc HC from mem seg.
                                 USBH_InitCfg.MemSegPtr,
                                 sizeof(USBH_HC),
                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (hc_nbr);
  }

  //                                                               Insert HC in host's HC list.
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  p_host->HC_TblPtr[hc_nbr] = p_hc;
#else
  p_hc->NextPtr = p_host->HC_HeadPtr;
  p_host->HC_HeadPtr = p_hc;
#endif

  p_hc_drv = &p_hc->Drv;
  p_hc->Ix = hc_nbr;
  p_hc->Name = (CPU_CHAR *)name;
  p_hc->RH_Fnct.State = USBH_HUB_STATE_NONE;
#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
  p_hc->IsStopped = DEF_YES;
#endif

  p_hc_drv->Nbr = hc_nbr;
  p_hc_drv->HostNbr = host_nbr;
  p_hc_drv->HW_InfoPtr = p_hw_info;
  p_hc_drv->API_Ptr = p_drv_api;
  p_hc_drv->BSP_API_Ptr = p_bsp_api;
  p_hc_drv->RH_API_Ptr = p_rh_api;
  p_hc_drv->DataPtr = DEF_NULL;

  if (p_hc_drv->API_Ptr->Init != DEF_NULL) {
    p_hc_drv->API_Ptr->Init(p_hc_drv,                           // Init HCD.
                            USBH_InitCfg.MemSegPtr,
                            USBH_InitCfg.MemSegBufPtr,
                            (USBH_HC_CFG_EXT *)p_hc_cfg_ext,
                            p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (hc_nbr);
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  LOG_VRB(("HC added to host #", (u)host_nbr));

  return (hc_nbr);
}

/****************************************************************************************************//**
 *                                           USBH_AsyncEventPost()
 *
 * @brief    Creates and posts an async event.
 *
 * @param    event_type  Type of event.
 *
 * @param    dev_handle  Handle to device.
 *
 * @param    ep_handle   Handle to endpoint.
 *
 * @param    p_arg       Pointer to the type-specific argument.
 *******************************************************************************************************/
void USBH_AsyncEventPost(USBH_ASYNC_EVENT_TYPE event_type,
                         USBH_DEV_HANDLE       dev_handle,
                         USBH_EP_HANDLE        ep_handle,
                         void                  *p_arg)
{
  RTOS_ERR         err_kal;
  USBH_ASYNC_EVENT *p_async_event;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  p_async_event = USBH_Ptr->AsyncEventFreeHeadPtr;
  if (p_async_event == DEF_NULL) {
    CORE_EXIT_ATOMIC();

    LOG_ERR(("No more async event in Free list -> ", RTOS_ERR_CODE_LOG_ARG_GET(RTOS_ERR_ALLOC)));
    return;
  }
  USBH_Ptr->AsyncEventFreeHeadPtr = p_async_event->NextPtr;
  CORE_EXIT_ATOMIC();

  p_async_event->Type = event_type;
  p_async_event->DevHandle = dev_handle;
  p_async_event->EP_Handle = ep_handle;
  p_async_event->ArgPtr = p_arg;
  p_async_event->NextPtr = DEF_NULL;

  CORE_ENTER_ATOMIC();                                          // Add async event to list.
  if (USBH_Ptr->AsyncEventQHeadPtr != DEF_NULL) {
    USBH_Ptr->AsyncEventQTailPtr->NextPtr = p_async_event;
  } else {
    USBH_Ptr->AsyncEventQHeadPtr = p_async_event;
  }
  USBH_Ptr->AsyncEventQTailPtr = p_async_event;
  CORE_EXIT_ATOMIC();

  KAL_SemPost(USBH_Ptr->AsyncEventSem,
              KAL_OPT_POST_NONE,
              &err_kal);
  if (RTOS_ERR_CODE_GET(err_kal) != RTOS_ERR_NONE) {
    LOG_ERR(("Submitting async end of xfer to Q -> ", RTOS_ERR_LOG_ARG_GET(err_kal)));
  }
}

/****************************************************************************************************//**
 *                                           USBH_AsyncEventFree()
 *
 * @brief    Frees an async event.
 *
 * @param    p_async_event   Pointer to the async event structure that will be freed.
 *
 * @note     (1) Caller must be in a critical section.
 *******************************************************************************************************/
void USBH_AsyncEventFree(USBH_ASYNC_EVENT *p_async_event)
{
  p_async_event->NextPtr = USBH_Ptr->AsyncEventFreeHeadPtr;
  USBH_Ptr->AsyncEventFreeHeadPtr = p_async_event;
}

/****************************************************************************************************//**
 *                                               USBH_HC_PtrGet()
 *
 * @brief    Gets pointer to given host controller.
 *
 * @param    p_host  Pointer to the host.
 *
 * @param    hc_nbr  Host Controller number.
 *
 * @param    p_err   Pointer to the variable that will receive the error code from this function.
 *
 * @return   Pointer to the host controller, if successful.
 *           Null,                       otherwise.
 *******************************************************************************************************/
USBH_HC *USBH_HC_PtrGet(USBH_HOST  *p_host,
                        CPU_INT08U hc_nbr,
                        RTOS_ERR   *p_err)
{
  USBH_HC *p_hc;

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  {
    CORE_DECLARE_IRQ_STATE;

    CORE_ENTER_ATOMIC();
    if (hc_nbr >= p_host->HC_NbrNext) {
      CORE_EXIT_ATOMIC();
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG, DEF_NULL);
    }
    CORE_EXIT_ATOMIC();
  }
#endif

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  p_hc = p_host->HC_TblPtr[hc_nbr];
#else
  p_hc = p_host->HC_HeadPtr;
  while ((p_hc != DEF_NULL)
         && (hc_nbr != p_hc->Ix)) {
    p_hc = p_hc->NextPtr;
  }
#endif

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (p_hc);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               USBH_KernelInit()
 *
 * @brief    Ensures Kernel offers all services necessary for proper functioning of USB host stack.
 *
 * @param    p_err   Pointer to the variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void USBH_KernelInit(RTOS_ERR *p_err)
{
  CPU_BOOLEAN is_en;

  //                                                               -------- ENSURE REQUIRED SERVICES ARE AVAIL --------
  is_en = KAL_FeatureQuery(KAL_FEATURE_DLY, KAL_OPT_DLY_NONE);
  is_en &= KAL_FeatureQuery(KAL_FEATURE_PEND_TIMEOUT, KAL_OPT_NONE);
  is_en &= KAL_FeatureQuery(KAL_FEATURE_LOCK_CREATE, KAL_OPT_CREATE_NONE);
  is_en &= KAL_FeatureQuery(KAL_FEATURE_LOCK_ACQUIRE, KAL_OPT_PEND_NONE);
  is_en &= KAL_FeatureQuery(KAL_FEATURE_LOCK_RELEASE, KAL_OPT_POST_NONE);
  is_en &= KAL_FeatureQuery(KAL_FEATURE_SEM_CREATE, KAL_OPT_CREATE_NONE);
  is_en &= KAL_FeatureQuery(KAL_FEATURE_SEM_PEND, KAL_OPT_PEND_BLOCKING);
  is_en &= KAL_FeatureQuery(KAL_FEATURE_SEM_POST, KAL_OPT_PEND_BLOCKING);
  is_en &= KAL_FeatureQuery(KAL_FEATURE_TASK_CREATE, KAL_OPT_CREATE_NONE);
  is_en &= KAL_FeatureQuery(KAL_FEATURE_MON, KAL_OPT_CREATE_NONE);
  is_en &= KAL_FeatureQuery(KAL_FEATURE_MON_DEL, KAL_OPT_DEL_NONE);
#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
  is_en &= KAL_FeatureQuery(KAL_FEATURE_LOCK_DEL, KAL_OPT_DEL_NONE);
  is_en &= KAL_FeatureQuery(KAL_FEATURE_SEM_DEL, KAL_OPT_DEL_NONE);
  is_en &= KAL_FeatureQuery(KAL_FEATURE_TASK_DEL, KAL_OPT_DEL_NONE);
#endif

  if (is_en == DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);
  }
}

/****************************************************************************************************//**
 *                                           USBH_AsyncTaskHandler()
 *
 * @brief    Processes asynchronous events.
 *
 * @param    p_arg   Pointer to the task argument. Unused in this case.
 *******************************************************************************************************/
static void USBH_AsyncTaskHandler(void *p_arg)
{
  RTOS_ERR         err_kal;
  USBH_ASYNC_EVENT async_event_cur;
  USBH_ASYNC_EVENT *p_async_event;
  CORE_DECLARE_IRQ_STATE;

  PP_UNUSED_PARAM(p_arg);

  USBH_DBG_HOST_STATS_RESET();

  while (DEF_ON) {
    USBH_DBG_HOST_STATS_GLOBAL_INC(EventPendCnt);
    KAL_SemPend(USBH_Ptr->AsyncEventSem,
                KAL_OPT_PEND_NONE,
                KAL_TIMEOUT_INFINITE,
                &err_kal);
    if (RTOS_ERR_CODE_GET(err_kal) == RTOS_ERR_NONE) {
      USBH_DBG_HOST_STATS_GLOBAL_INC(EventPostCnt);

      KAL_LockAcquire(USBH_Ptr->AsyncEventListLock,             // Lock async list to prevent modification from Abort.
                      KAL_OPT_PEND_NONE,
                      KAL_TIMEOUT_INFINITE,
                      &err_kal);
      if (RTOS_ERR_CODE_GET(err_kal) != RTOS_ERR_NONE) {
        LOG_ERR(("Locking async event list -> ", RTOS_ERR_LOG_ARG_GET(err_kal)));
        continue;
      }

      CORE_ENTER_ATOMIC();
      p_async_event = USBH_Ptr->AsyncEventQHeadPtr;
      if (p_async_event == DEF_NULL) {                          // If list has been emptied (by a Abort) after Post.
        CORE_EXIT_ATOMIC();
        KAL_LockRelease(USBH_Ptr->AsyncEventListLock,
                        &err_kal);
        USBH_DBG_HOST_STATS_GLOBAL_INC(EventEmptyCnt);
        (void)err_kal;

        continue;
      }

      USBH_Ptr->AsyncEventQHeadPtr = p_async_event->NextPtr;

      async_event_cur = *p_async_event;

      //                                                           Free async event immediately to prevent race condition if  submitting in callback.
      USBH_AsyncEventFree(p_async_event);
      CORE_EXIT_ATOMIC();

      switch (async_event_cur.Type) {
        case USBH_ASYNC_EVENT_TYPE_URB_COMPLETE:
          USBH_URB_Complete(async_event_cur.DevHandle,
                            async_event_cur.EP_Handle);

          KAL_LockRelease(USBH_Ptr->AsyncEventListLock,
                          &err_kal);
          USBH_DBG_HOST_STATS_EP_INC((CPU_INT08U)async_event_cur.EP_Handle,
                                     (CPU_INT08U)async_event_cur.DevHandle,
                                     EventURB_Cmpl);
          (void)err_kal;
          break;

        case USBH_ASYNC_EVENT_TYPE_URB_LIST_FREE:
          KAL_LockRelease(USBH_Ptr->AsyncEventListLock,
                          &err_kal);
          USBH_DBG_HOST_STATS_EP_INC((CPU_INT08U)async_event_cur.EP_Handle,
                                     (CPU_INT08U)async_event_cur.DevHandle,
                                     EventURB_ListFree);
          (void)err_kal;

          USBH_EP_URB_ListAbortedFree(async_event_cur.DevHandle,
                                      async_event_cur.EP_Handle,
                                      (USBH_URB_MGMT *)async_event_cur.ArgPtr);
          break;

        case USBH_ASYNC_EVENT_TYPE_NONE:
        default:
          KAL_LockRelease(USBH_Ptr->AsyncEventListLock,
                          &err_kal);
          USBH_DBG_HOST_STATS_EP_INC((CPU_INT08U)async_event_cur.EP_Handle,
                                     (CPU_INT08U)async_event_cur.DevHandle,
                                     EventNone);
          break;
      }
    } else {
      USBH_DBG_HOST_STATS_GLOBAL_INC(EventErrCnt);
      LOG_ERR(("Pending on async event -> ", RTOS_ERR_LOG_ARG_GET(err_kal)));
    }
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_HOST_AVAIL))
