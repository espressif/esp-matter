/***************************************************************************//**
 * @file
 * @brief IO - Sd Core
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_IO_SD_AVAIL))

#if (!defined(RTOS_MODULE_IO_AVAIL))

#error IO SD module requires IO module. Make sure it is part of your project and that \
  RTOS_MODULE_IO_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_prio.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/kal/kal_priv.h>

#include  <io/source/sd/sd_priv.h>
#include  <io/source/sd/sd_io_fnct_priv.h>
#include  <io/source/sd/sd_mem_priv.h>
#include  <io/include/sd.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 *
 * @note     (1) Some SD cards implement the CMD8 incorrectly and always sends 0 in the command response
 *               in the check pattern echo back field. For this reason, the check pattern should always be
 *               0.
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                        (IO, SD)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_IO

#if (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_DISABLED)
#define  SD_INIT_CFG_DFLT                   {   \
    .MemSegPtr = DEF_NULL,                      \
    .MemSegBufPtr = DEF_NULL,                   \
    .IO_FnctQtyTot = LIB_MEM_BLK_QTY_UNLIMITED, \
    .EventQtyTot = LIB_MEM_BLK_QTY_UNLIMITED,   \
    .XferQtyTot = LIB_MEM_BLK_QTY_UNLIMITED,    \
    .CoreTaskStkPtr = DEF_NULL,                 \
    .CoreTaskStkSizeElements = 512u,            \
    .AsyncTaskStkPtr = DEF_NULL,                \
    .AsyncTaskStkSizeElements = 512u,           \
    .EventFnctsPtr = DEF_NULL                   \
}
#else
#define  SD_INIT_CFG_DFLT                   {   \
    .MemSegPtr = DEF_NULL,                      \
    .MemSegBufPtr = DEF_NULL,                   \
    .IO_FnctQtyTot = LIB_MEM_BLK_QTY_UNLIMITED, \
    .EventQtyTot = LIB_MEM_BLK_QTY_UNLIMITED,   \
    .XferQtyTot = LIB_MEM_BLK_QTY_UNLIMITED,    \
    .CoreTaskStkPtr = DEF_NULL,                 \
    .CoreTaskStkSizeElements = 512u,            \
    .AsyncTaskStkPtr = DEF_NULL,                \
    .AsyncTaskStkSizeElements = 512u,           \
    .EventFnctsPtr = DEF_NULL,                  \
    .CoreCardPollingPeriodMs = 500u             \
}
#endif

//                                                                 ------------- SD CMD0 RETRY PARAMETERS -------------
#define  SD_CMD0_RETRY_CNT                          128u
#define  SD_CMD0_RETRY_DLY_MS                       5u

//                                                                 ----------- SD CMD SEND_IF_COND CMD8 ARG -----------
#define  SD_CMD8_ARG_VHS_27_36_V            DEF_BIT_08
#define  SD_CMD8_ARG_VHS_LOW                DEF_BIT_09
#define  SD_CMD8_ARG_CHK_PATTERN            0x00u               // CMD8 Check pattern. See note 1.

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const SD_INIT_CFG SD_InitCfgDflt = SD_INIT_CFG_DFLT;
SD_INIT_CFG       SD_InitCfg = SD_INIT_CFG_DFLT;
#endif

SD *SD_Ptr = DEF_NULL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

const SD_BUS_HANDLE SD_BusHandleNull = DEF_NULL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void SD_CoreTaskHandler(void *p_data);

static void SD_AsyncTaskHandler(void *p_data);

static void SD_CardInit(SD_BUS_HANDLE bus_handle,
                        RTOS_ERR      *p_err);

static void SD_CardRem(SD_BUS_HANDLE bus_handle,
                       RTOS_ERR      *p_err);

static void SD_BusWidthUpdate(SD_BUS_HANDLE bus_handle,
                              RTOS_ERR      *p_err);

static void SD_BusXferAdd(SD_XFER_DATA   *p_xfer_data,
                          SD_BUS_HANDLE  bus_handle,
                          SD_FNCT_HANDLE fnct_handle,
                          SD_XFER_TYPE   xfer_type,
                          CPU_INT08U     cmd_nbr,
                          CPU_INT32U     cmd_arg,
                          CPU_BOOLEAN    dir_is_rd,
                          CPU_INT08U     *p_buf,
                          CPU_INT32U     blk_qty,
                          CPU_INT32U     blk_len,
                          RTOS_ERR       *p_err);

static void SD_BusXferNextSubmit(SD_BUS_HANDLE bus_handle,
                                 RTOS_ERR      *p_err);

static void SD_BusXferCmplProcess(SD_BUS_HANDLE bus_handle,
                                  RTOS_ERR      err);

#if (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_ENABLED)
static void SD_CoreCardPollingTmrCb(void *p_arg);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                        SD_ConfigureMemSeg()
 *
 * @brief      Configures the memory segment where SD module data structures will be allocated.
 *
 * @param      p_seg       Pointer to memory segment to use when allocating control data.
 *                         Can be the same segment used for p_seg_buf.
 *                         DEF_NULL means general purpose heap segment.
 *
 * @param      p_seg_buf   Pointer to memory segment to use when allocating data buffers.
 *                         Can be the same segment used for p_seg.
 *                         DEF_NULL means general purpose heap segment.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the IO SD module is initialized via the IO_Init()
 *               function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void SD_ConfigureMemSeg(MEM_SEG *p_seg,
                        MEM_SEG *p_seg_buf)
{
  RTOS_ASSERT_CRITICAL((SD_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  SD_InitCfg.MemSegPtr = p_seg;
  SD_InitCfg.MemSegBufPtr = p_seg_buf;
}
#endif

/****************************************************************************************************//**
 *                                       SD_ConfigureIO_FnctHandleQty()
 *
 * @brief    Configures the maximum number of SD IO Functions for all the SD busses.
 *
 * @param    fnct_handle_qty     Max number of SD IO function handles.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the IO SD module is initialized via the IO_Init()
 *               function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void SD_ConfigureIO_FnctHandleQty(CPU_SIZE_T fnct_handle_qty)
{
  RTOS_ASSERT_CRITICAL((SD_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  SD_InitCfg.IO_FnctQtyTot = fnct_handle_qty;
}
#endif

/****************************************************************************************************//**
 *                                       SD_ConfigureEventQty()
 *
 * @brief    Configures the maximum number of events for all the SD busses.
 *
 * @param    event_qty   Quantity of events.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the IO SD module is initialized via the IO_Init()
 *               function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void SD_ConfigureEventQty(CPU_SIZE_T event_qty)
{
  RTOS_ASSERT_CRITICAL((SD_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  SD_InitCfg.EventQtyTot = event_qty;
}
#endif

/****************************************************************************************************//**
 *                                       SD_ConfigureXferQty()
 *
 * @brief    Configures the maximum number of simultaneous transfers for all the SD busses.
 *
 * @param    xfer_qty    Quantity of transfers.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the IO SD module is initialized via the IO_Init()
 *               function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void SD_ConfigureXferQty(CPU_SIZE_T xfer_qty)
{
  RTOS_ASSERT_CRITICAL((SD_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  SD_InitCfg.XferQtyTot = xfer_qty;
}
#endif

/****************************************************************************************************//**
 *                                       SD_ConfigureEventFncts()
 *
 * @brief    Sets the structure of callback that will be used by the IO SD module to notify the
 *           application of certain events.
 *
 * @param    p_event_fncts   Pointer to a structure containing the event functions to call.
 *                             [Content MUST be persistent]
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the IO SD module is initialized via the IO_Init()
 *               function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void SD_ConfigureEventFncts(const SD_EVENT_FNCTS *p_event_fncts)
{
  RTOS_ASSERT_CRITICAL((SD_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  SD_InitCfg.EventFnctsPtr = p_event_fncts;
}
#endif

/****************************************************************************************************//**
 *                                       SD_ConfigureCoreTaskStk()
 *
 * @brief    Configures the SD core task's stack.
 *
 * @param    stk_size_elements   Size, in stack elements, of the task's stack.
 *
 * @param    p_stk               Pointer to base of the task's stack. If DEF_NULL, stack will be allocated
 *                               from KAL's memory segment.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the IO SD module is initialized via the IO_Init()
 *               function.
 *
 * @note     (3) In order to change the priority of the IO SD core task, use the function
 *               SD_CoreTaskPrioSet().
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void SD_ConfigureCoreTaskStk(CPU_INT32U stk_size_elements,
                             void       *p_stk)
{
  RTOS_ASSERT_CRITICAL((SD_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  SD_InitCfg.CoreTaskStkPtr = p_stk;
  SD_InitCfg.CoreTaskStkSizeElements = stk_size_elements;
}
#endif

/****************************************************************************************************//**
 *                                     SD_ConfigureAsyncTaskStk()
 *
 * @brief    Configures the SD async task's stack.
 *
 * @param    stk_size_elements   Size, in stack elements, of the task's stack.
 *
 * @param    p_stk               Pointer to base of the task's stack. If DEF_NULL, stack will be allocated
 *                               from KAL's memory segment.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the IO SD module is initialized via the IO_Init()
 *               function.
 *
 * @note     (3) In order to change the priority of the IO SD core task, use the function
 *               SD_AsyncTaskPrioSet().
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void SD_ConfigureAsyncTaskStk(CPU_INT32U stk_size_elements,
                              void       *p_stk)
{
  RTOS_ASSERT_CRITICAL((SD_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  SD_InitCfg.AsyncTaskStkPtr = p_stk;
  SD_InitCfg.AsyncTaskStkSizeElements = stk_size_elements;
}
#endif

/****************************************************************************************************//**
 *                                 SD_ConfigureCoreCardPollingPeriod()
 *
 * @brief    Set the core card polling period.
 *
 * @param    period_ms    Interval of time (in milliseconds) between two polling.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the IO SD module is initialized via the IO_Init()
 *               function.
 *******************************************************************************************************/

#if ((RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED) \
  && (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_ENABLED))
void SD_ConfigureCoreCardPollingPeriod(CPU_INT32U period_ms)
{
  RTOS_ASSERT_CRITICAL((SD_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  SD_InitCfg.CoreCardPollingPeriodMs = period_ms;
}
#endif

/****************************************************************************************************//**
 *                                              SD_Init()
 *
 * @brief    Initializes the SD module.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from this
 *                   function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_SEG_OVF
 *
 * @note     (1) This function is NOT thread-safe and should be called ONCE during system initialization,
 *               before multi-threading has begun.
 *
 * @note     (2) This function will not return an error -- any error conditions will trigger a failed
 *               assertion.
 *******************************************************************************************************/
void SD_Init(RTOS_ERR *p_err)
{
  CPU_BOOLEAN     kal_complete;
  KAL_TASK_HANDLE core_task_handle;
  KAL_TASK_HANDLE async_task_handle;
  SD              *p_sd;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               Assert that the system has all necessary features.
  kal_complete = KAL_FeatureQuery(KAL_FEATURE_LOCK_CREATE, KAL_OPT_NONE);
  kal_complete = kal_complete && KAL_FeatureQuery(KAL_FEATURE_LOCK_ACQUIRE, KAL_OPT_NONE);
  kal_complete = kal_complete && KAL_FeatureQuery(KAL_FEATURE_LOCK_RELEASE, KAL_OPT_NONE);
  kal_complete = kal_complete && KAL_FeatureQuery(KAL_FEATURE_LOCK_DEL, KAL_OPT_NONE);
  kal_complete = kal_complete && KAL_FeatureQuery(KAL_FEATURE_SEM_CREATE, KAL_OPT_NONE);
  kal_complete = kal_complete && KAL_FeatureQuery(KAL_FEATURE_SEM_PEND, KAL_OPT_NONE);
  kal_complete = kal_complete && KAL_FeatureQuery(KAL_FEATURE_SEM_POST, KAL_OPT_NONE);
  kal_complete = kal_complete && KAL_FeatureQuery(KAL_FEATURE_SEM_DEL, KAL_OPT_NONE);
  kal_complete = kal_complete && KAL_FeatureQuery(KAL_FEATURE_TASK_CREATE, KAL_OPT_NONE);
#if (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_ENABLED)
  kal_complete = kal_complete && KAL_FeatureQuery(KAL_FEATURE_TMR, KAL_OPT_NONE);
#endif
  RTOS_ASSERT_CRITICAL(kal_complete, RTOS_ERR_NOT_SUPPORTED,; );

  p_sd = (SD *)Mem_SegAlloc("IO - SD root struct",
                            SD_InitCfg.MemSegPtr,
                            sizeof(SD),
                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return;
  }

  SList_Init(&p_sd->BusHandleListHeadPtr);
  p_sd->CoreEventHeadPtr = DEF_NULL;
  p_sd->CoreEventTailPtr = DEF_NULL;
  p_sd->AsyncEventHeadPtr = DEF_NULL;
  p_sd->AsyncEventTailPtr = DEF_NULL;
  p_sd->PreAllocEventHeadPtr = DEF_NULL;
  p_sd->CoreTaskHandle = KAL_TaskHandleNull;
  p_sd->AsyncTaskHandle = KAL_TaskHandleNull;
  p_sd->LockAcquireTimeoutMs = KAL_TIMEOUT_INFINITE;

  CORE_ENTER_ATOMIC();
  SD_Ptr = p_sd;
  CORE_EXIT_ATOMIC();

  Mem_DynPoolCreate("IO - SD event pool",                       // Create pool of SD events.
                    &p_sd->EventPool,
                    SD_InitCfg.MemSegPtr,
                    sizeof(SD_EVENT),
                    sizeof(CPU_ALIGN),
                    (SD_InitCfg.EventQtyTot == LIB_MEM_BLK_QTY_UNLIMITED) ? 0u : SD_InitCfg.EventQtyTot,
                    SD_InitCfg.EventQtyTot,
                    p_err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  Mem_DynPoolCreate("IO - SD xfer data pool",                   // Create pool of SD transfers data.
                    &p_sd->XferDataPool,
                    SD_InitCfg.MemSegPtr,
                    DEF_MAX(sizeof(SD_ASYNC_XFER_DATA), sizeof(SD_SYNC_XFER_DATA)),
                    sizeof(CPU_ALIGN),
                    (SD_InitCfg.XferQtyTot == LIB_MEM_BLK_QTY_UNLIMITED) ? 0u : SD_InitCfg.XferQtyTot,
                    SD_InitCfg.XferQtyTot,
                    p_err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  //                                                               Create core task and its semaphore.
  SD_Ptr->CoreEventSemHandle = KAL_SemCreate("IO - SD core event sem",
                                             DEF_NULL,
                                             p_err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  core_task_handle = KAL_TaskAlloc("IO - Core task",
                                   (CPU_STK *)SD_InitCfg.CoreTaskStkPtr,
                                   SD_InitCfg.CoreTaskStkSizeElements,
                                   DEF_NULL,
                                   p_err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  KAL_TaskCreate(core_task_handle,
                 SD_CoreTaskHandler,
                 DEF_NULL,
                 IO_SD_CORE_TASK_PRIO_DFLT,
                 DEF_NULL,
                 p_err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

#if (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_ENABLED)
  {
    KAL_TMR_EXT_CFG tmr_cfg;
    KAL_TMR_HANDLE  tmr_handle;

    tmr_cfg.Opt = KAL_OPT_TMR_ONE_SHOT;
    tmr_handle = KAL_TmrCreate("IO - SD Core card polling timer",
                               SD_CoreCardPollingTmrCb,
                               DEF_NULL,
                               SD_InitCfg.CoreCardPollingPeriodMs,
                               &tmr_cfg,
                               p_err);
    RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

    CORE_ENTER_ATOMIC();
    SD_Ptr->CoreCardPollingTmr = tmr_handle;
    CORE_EXIT_ATOMIC();
  }
#endif

  //                                                               Create async task and its semaphore.
  SD_Ptr->AsyncEventSemHandle = KAL_SemCreate("IO - SD Async event sem",
                                              DEF_NULL,
                                              p_err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  async_task_handle = KAL_TaskAlloc("IO - Async task",
                                    (CPU_STK *)SD_InitCfg.AsyncTaskStkPtr,
                                    SD_InitCfg.AsyncTaskStkSizeElements,
                                    DEF_NULL,
                                    p_err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  KAL_TaskCreate(async_task_handle,
                 SD_AsyncTaskHandler,
                 DEF_NULL,
                 IO_SD_ASYNC_TASK_PRIO_DFLT,
                 DEF_NULL,
                 p_err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  //                                                               Save created task handles.
  CORE_ENTER_ATOMIC();
  SD_Ptr->CoreTaskHandle = core_task_handle;
  SD_Ptr->AsyncTaskHandle = async_task_handle;
  CORE_EXIT_ATOMIC();

  SD_IO_Init(p_err);                                            // Initializes SD IO module.
}

/****************************************************************************************************//**
 *                                           SD_CoreTaskPrioSet()
 *
 * @brief    Assigns a new priority to the IO SD core task.
 *
 * @param    prio    New priority of the the core task.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from this
 *                   function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_ARG
 *
 * @note     (1) This function cannot be called after the IO SD module has been initialized via the
 *               IO_Init() function.
 *******************************************************************************************************/
void SD_CoreTaskPrioSet(CPU_INT08U prio,
                        RTOS_ERR   *p_err)
{
  KAL_TASK_HANDLE core_task_handle;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((SD_Ptr != DEF_NULL), RTOS_ERR_NOT_INIT,; );

  core_task_handle = SD_Ptr->CoreTaskHandle;
  CORE_EXIT_ATOMIC();

  KAL_TaskPrioSet(core_task_handle,
                  prio,
                  p_err);
}

/****************************************************************************************************//**
 *                                        SD_AsyncTaskPrioSet()
 *
 * @brief    Assigns a new priority to the IO SD async task.
 *
 * @param    prio    New priority of the the async task.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from this
 *                   function:
 *
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_ARG
 *
 * @note     (1) This function cannot be called after the IO SD module has been initialized via the
 *               IO_Init() function.
 *******************************************************************************************************/
void SD_AsyncTaskPrioSet(CPU_INT08U prio,
                         RTOS_ERR   *p_err)
{
  KAL_TASK_HANDLE async_task_handle;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((SD_Ptr != DEF_NULL), RTOS_ERR_NOT_INIT,; );

  async_task_handle = SD_Ptr->AsyncTaskHandle;
  CORE_EXIT_ATOMIC();

  KAL_TaskPrioSet(async_task_handle,
                  prio,
                  p_err);
}

/****************************************************************************************************//**
 *                                         SD_OperTimeoutSet()
 *
 * @brief    Assigns a new timeout value for SD operations.
 *
 * @brief      Assigns a new timeout value for SD operations.
 *
 * @param      timout_ms     New timeout value, in ms.
 *
 * @param      p_err         Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *
 * @note     (1) This function cannot be called before the IO SD module has been initialized via the
 *               IO_Init() function.
 *******************************************************************************************************/
void SD_OperTimeoutSet(CPU_INT32U timeout_ms,
                       RTOS_ERR   *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((SD_Ptr != DEF_NULL), RTOS_ERR_NOT_INIT,; );

  SD_Ptr->LockAcquireTimeoutMs = timeout_ms;
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                               SD_BusAdd()
 *
 * @brief    Adds a SD bus.
 *
 * @param    name    Name of SD bus controller.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from this
 *                   function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_WOULD_OVF
 *                       - RTOS_ERR_OS_OBJ_DEL
 *                       - RTOS_ERR_NOT_FOUND
 *                       - RTOS_ERR_WOULD_BLOCK
 *                       - RTOS_ERR_IS_OWNER
 *                       - RTOS_ERR_SEG_OVF
 *                       - RTOS_ERR_OS_SCHED_LOCKED
 *                       - RTOS_ERR_ABORT
 *                       - RTOS_ERR_TIMEOUT
 *
 * @return   Handle to SD bus.
 *******************************************************************************************************/
SD_BUS_HANDLE SD_BusAdd(const CPU_CHAR *name,
                        RTOS_ERR       *p_err)
{
  SD_BUS_HANDLE     bus_handle;
  PLATFORM_MGR_ITEM *p_item;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, SD_BusHandleNull);

  p_item = PlatformMgrItemGetByName(name, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (SD_BusHandleNull);
  }

  RTOS_ASSERT_DBG(p_item != DEF_NULL, RTOS_ERR_NOT_FOUND, SD_BusHandleNull);
  RTOS_ASSERT_DBG(p_item->Type == PLATFORM_MGR_ITEM_TYPE_HW_INFO_IO_SD_CTRLR, RTOS_ERR_INVALID_TYPE, SD_BusHandleNull);

  bus_handle = (SD_BUS_HANDLE)Mem_SegAlloc("IO - SD Bus handle",
                                           SD_InitCfg.MemSegPtr,
                                           sizeof(struct  sd_bus_handle),
                                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return (SD_BusHandleNull);
  }

  bus_handle->Name = p_item->StrID;
  bus_handle->BusDrvApiPtr = ((struct  _sd_ctrlr_pm_item *)p_item)->BusDrvAPI_Ptr;
  bus_handle->TransportMode = SD_TRANSPORT_MODE_SD;
  bus_handle->CardPresent = DEF_NO;
  bus_handle->CardEn = DEF_DISABLED;

  bus_handle->BusEvent.BusHandle = bus_handle;
  bus_handle->BusEvent.Type = SD_EVENT_TYPE_NONE;
  bus_handle->BusEvent.NextPtr = DEF_NULL;
#if (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_ENABLED)
  bus_handle->BusStart = DEF_NO;
#endif
  //                                                               Init capabilities.
  bus_handle->Capabilities.Host.Capabilities = DEF_BIT_NONE;
  bus_handle->Capabilities.Host.OCR = DEF_BIT_NONE;
  bus_handle->Capabilities.Card.Capabilities = DEF_BIT_NONE;

  //                                                               Create exclusive bus access lock.
  bus_handle->BusLockHandle = KAL_LockCreate("IO - SD bus lock",
                                             DEF_NULL,
                                             p_err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL, SD_BusHandleNull);

  //                                                               Init driver.
  if (bus_handle->BusDrvApiPtr->Init != DEF_NULL) {
    bus_handle->BusDrvDataPtr = bus_handle->BusDrvApiPtr->Init(bus_handle,
                                                               ((struct  _sd_ctrlr_pm_item *)p_item)->BusDrvInfoPtr,
                                                               p_err);
    RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL, SD_BusHandleNull);
  }

  CORE_ENTER_ATOMIC();
  SList_Push(&SD_Ptr->BusHandleListHeadPtr,
             &bus_handle->ListNode);
  CORE_EXIT_ATOMIC();

  return (bus_handle);
}

/****************************************************************************************************//**
 *                                       SD_BusHandleGetFromName()
 *
 * @brief    Gets SD bus handle from its name.
 *
 * @param    name    Name of SD bus controller.
 *
 * @return   Handle to SD bus.
 *******************************************************************************************************/
SD_BUS_HANDLE SD_BusHandleGetFromName(const CPU_CHAR *name)
{
  struct  sd_bus_handle *p_bus_handle;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  SLIST_FOR_EACH_ENTRY(SD_Ptr->BusHandleListHeadPtr, p_bus_handle, struct sd_bus_handle, ListNode) {
    CPU_INT16S cmp_res;

    CORE_EXIT_ATOMIC();

    cmp_res = Str_Cmp(p_bus_handle->Name, name);
    if (cmp_res == 0u) {
      return ((SD_BUS_HANDLE)p_bus_handle);
    }

    CORE_ENTER_ATOMIC();
  }
  CORE_EXIT_ATOMIC();

  return (SD_BusHandleNull);
}

/****************************************************************************************************//**
 *                                               SD_BusStart()
 *
 * @brief    Starts SD bus controller.
 *
 * @param    bus_handle  Handle to SD bus.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *******************************************************************************************************/
void SD_BusStart(SD_BUS_HANDLE bus_handle,
                 RTOS_ERR      *p_err)
{
  const SD_BUS_DRV_API *p_bus_drv_api = bus_handle->BusDrvApiPtr;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  RTOS_ASSERT_DBG_ERR_SET((bus_handle != SD_BusHandleNull), *p_err, RTOS_ERR_INVALID_HANDLE,; );

  //                                                               Init SD host controller and retrieve capabilities.
  bus_handle->Capabilities = p_bus_drv_api->InitHW(bus_handle->BusDrvDataPtr,
                                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Set bus properties.
  p_bus_drv_api->ClkFreqSet(bus_handle->BusDrvDataPtr,
                            SD_FREQ_DFLT_HZ,
                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_bus_drv_api->BusSupplyVoltSet(bus_handle->BusDrvDataPtr,
                                  SD_CARD_BUS_VOLT_3_3,
                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_bus_drv_api->BusSignalVoltInit(bus_handle->BusDrvDataPtr,
                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  bus_handle->XferListHeadPtr = DEF_NULL;
  bus_handle->XferListTailPtr = DEF_NULL;

  p_bus_drv_api->Start(bus_handle->BusDrvDataPtr,
                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (bus_handle->CardDetectMode == SD_CARD_DETECT_MODE_WIRED) {
    SD_CardInit(bus_handle, p_err);                             // Card wired, directly initialize it upon ctrlr start.
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
#if (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_ENABLED)
  } else if (bus_handle->CardDetectMode == SD_CARD_DETECT_MODE_POLLING) {
    bus_handle->BusStart = DEF_YES;
    KAL_TmrStart(SD_Ptr->CoreCardPollingTmr, p_err);            // Start one-shot timer countdown.
#endif
  }
}

/****************************************************************************************************//**
 *                                               SD_BusStop()
 *
 * @brief    Stops SD bus controller.
 *
 * @param    bus_handle    Handle to SD bus.
 *
 * @param    p_err         Pointer to the variable that will receive one of the following error code(s)
 *                         from this function:
 *                             - RTOS_ERR_NONE
 *                             - RTOS_ERR_INVALID_HANDLE
 *******************************************************************************************************/
void SD_BusStop(SD_BUS_HANDLE bus_handle,
                RTOS_ERR      *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  RTOS_ASSERT_DBG_ERR_SET((bus_handle != SD_BusHandleNull), *p_err, RTOS_ERR_INVALID_HANDLE,; );

  bus_handle->BusDrvApiPtr->Stop(bus_handle->BusDrvDataPtr,
                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (bus_handle->CardDetectMode == SD_CARD_DETECT_MODE_WIRED) {
    SD_CardRem(bus_handle, p_err);                              // Card wired: remove it when controller stops.

#if (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_ENABLED)
  } else if (bus_handle->CardDetectMode == SD_CARD_DETECT_MODE_POLLING) {
    bus_handle->BusStart = DEF_NO;                              // Mark bus as stopped to stop timer sending...
                                                                // periodically an event to Core task.
#endif
  }
  //                                                               MICRIUM-686 Remove card if any
}

/****************************************************************************************************//**
 *                                           SD_AlignReqGet()
 *
 * @brief    Gets required buffer alignment.
 *
 * @param    bus_handle  Handle to SD bus.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *
 * @return   Required alignment, in octets.
 *******************************************************************************************************/
CPU_SIZE_T SD_AlignReqGet(SD_BUS_HANDLE bus_handle,
                          RTOS_ERR      *p_err)
{
  CPU_SIZE_T align_req;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  RTOS_ASSERT_DBG_ERR_SET((bus_handle != SD_BusHandleNull), *p_err, RTOS_ERR_INVALID_HANDLE, 0u);

  align_req = bus_handle->BusDrvApiPtr->AlignReqGet(bus_handle->BusDrvDataPtr,
                                                    p_err);

  return (align_req);
}

/****************************************************************************************************//**
 *                                           SD_CardTypeGet()
 *
 * @brief    Gets type of SD card currently connected.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *
 * @return   Type of SD card.
 *******************************************************************************************************/
SD_CARDTYPE SD_CardTypeGet(SD_BUS_HANDLE bus_handle,
                           RTOS_ERR      *p_err)
{
  SD_CARDTYPE card_type = SD_CARDTYPE_NONE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  RTOS_ASSERT_DBG_ERR_SET((bus_handle != SD_BusHandleNull), *p_err, RTOS_ERR_INVALID_HANDLE, SD_CARDTYPE_NONE);

  card_type = bus_handle->CardType;

  return (card_type);
}

/****************************************************************************************************//**
 *                                     SD_BSP_BusCardDetectEvent()
 *
 * @brief    Reports card detect event to IO-SD core task.
 *
 * @param    bus_handle      Handle to SD bus.
 *******************************************************************************************************/
void SD_BSP_BusCardDetectEvent(SD_BUS_HANDLE bus_handle)
{
  SD_BusCardDetectEvent(bus_handle);
}

/****************************************************************************************************//**
 *                                     SD_BSP_BusCardRemoveEvent()
 *
 * @brief    Reports card remove event to IO-SD core task.
 *
 * @param    bus_handle      Handle to SD bus.
 *******************************************************************************************************/
void SD_BSP_BusCardRemoveEvent(SD_BUS_HANDLE bus_handle)
{
  SD_BusCardRemoveEvent(bus_handle);
}

/****************************************************************************************************//**
 *                                        SD_BusCardDetectEvent()
 *
 * @brief    Reports card detect event.
 *
 * @param    bus_handle      Handle to SD bus.
 *******************************************************************************************************/
void SD_BusCardDetectEvent(SD_BUS_HANDLE bus_handle)
{
  RTOS_ERR err;

  if (bus_handle->CardDetectMode == SD_CARD_DETECT_MODE_WIRED) {
    return;                                                     // Card has already been detected upon ctrlr start.
  }

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  SD_CoreEventPost(bus_handle,
                   SD_EVENT_TYPE_CARD_DETECT,
                   err);
}

/****************************************************************************************************//**
 *                                           SD_BusCardRemoveEvent()
 *
 * @brief    Reports card remove event.
 *
 * @param    bus_handle      Handle to SD bus.
 *******************************************************************************************************/
void SD_BusCardRemoveEvent(SD_BUS_HANDLE bus_handle)
{
  RTOS_ERR err;

  if (bus_handle->CardDetectMode == SD_CARD_DETECT_MODE_WIRED) {
    return;                                                     // Card has already been detected upon ctrlr start.
  }

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  SD_CoreEventPost(bus_handle,
                   SD_EVENT_TYPE_CARD_REMOVE,
                   err);
}

/****************************************************************************************************//**
 *                                               SD_BusLock()
 *
 * @brief    Locks SD bus.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
void SD_BusLock(SD_BUS_HANDLE bus_handle,
                RTOS_ERR      *p_err)
{
  KAL_LockAcquire(bus_handle->BusLockHandle,
                  KAL_OPT_PEND_NONE,
                  SD_Ptr->LockAcquireTimeoutMs,
                  p_err);
}

/****************************************************************************************************//**
 *                                               SD_BusUnlock()
 *
 * @brief    Unlocks SD bus.
 *
 * @param    bus_handle  Handle to SD bus.
 *******************************************************************************************************/
void SD_BusUnlock(SD_BUS_HANDLE bus_handle)
{
  RTOS_ERR err_local;

  KAL_LockRelease(bus_handle->BusLockHandle,
                  &err_local);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err_local) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                         SD_CoreEventPost()
 *
 * @brief    Posts a SD event to Core task.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    event_type      Type of event.
 *
 * @param    err             Error variable associated to event.
 *******************************************************************************************************/
void SD_CoreEventPost(SD_BUS_HANDLE bus_handle,
                      SD_EVENT_TYPE event_type,
                      RTOS_ERR      err)
{
  SD_EVENT *p_event;
  RTOS_ERR err_local;
  CORE_DECLARE_IRQ_STATE;

  p_event = &bus_handle->BusEvent;
  RTOS_ASSERT_DBG(p_event != DEF_NULL, RTOS_ERR_NULL_PTR,; );

  p_event->BusHandle = bus_handle;
  p_event->Type = event_type;
  p_event->Err = err;

  CORE_ENTER_ATOMIC();
  p_event->NextPtr = DEF_NULL;

  if (SD_Ptr->CoreEventTailPtr != DEF_NULL) {
    SD_Ptr->CoreEventTailPtr->NextPtr = p_event;
  }
  SD_Ptr->CoreEventTailPtr = p_event;

  if (SD_Ptr->CoreEventHeadPtr == DEF_NULL) {
    SD_Ptr->CoreEventHeadPtr = SD_Ptr->CoreEventTailPtr;
  }
  CORE_EXIT_ATOMIC();

  KAL_SemPost(SD_Ptr->CoreEventSemHandle,
              KAL_OPT_POST_NONE,
              &err_local);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err_local) == RTOS_ERR_NONE, RTOS_ERR_CODE_GET(err_local),; );
}

/****************************************************************************************************//**
 *                                         SD_AsyncEventPost()
 *
 * @brief    Posts a SD event to Async task.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    event_type      Type of event.
 *
 * @param    err             Error variable associated to event.
 *******************************************************************************************************/
void SD_AsyncEventPost(SD_BUS_HANDLE bus_handle,
                       SD_EVENT_TYPE event_type,
                       RTOS_ERR      err)
{
  SD_EVENT *p_event;
  RTOS_ERR err_local;
  CORE_DECLARE_IRQ_STATE;

  p_event = SD_PreAllocEventGet();
  RTOS_ASSERT_DBG(p_event != DEF_NULL, RTOS_ERR_NULL_PTR,; );

  p_event->BusHandle = bus_handle;
  p_event->Type = event_type;
  p_event->Err = err;

  CORE_ENTER_ATOMIC();
  p_event->NextPtr = DEF_NULL;

  if (SD_Ptr->AsyncEventTailPtr != DEF_NULL) {
    SD_Ptr->AsyncEventTailPtr->NextPtr = p_event;
  }
  SD_Ptr->AsyncEventTailPtr = p_event;

  if (SD_Ptr->AsyncEventHeadPtr == DEF_NULL) {
    SD_Ptr->AsyncEventHeadPtr = SD_Ptr->AsyncEventTailPtr;
  }
  CORE_EXIT_ATOMIC();

  KAL_SemPost(SD_Ptr->AsyncEventSemHandle,
              KAL_OPT_POST_NONE,
              &err_local);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err_local) == RTOS_ERR_NONE, RTOS_ERR_CODE_GET(err_local),; );
}

/****************************************************************************************************//**
 *                                           SD_EventPreAlloc()
 *
 * @brief    Pre-allocates an event structure. This should be called before a call is made that is
 *           susceptible to trigger an interrupt later that may have to post an event.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
void SD_EventPreAlloc(RTOS_ERR *p_err)
{
  SD_EVENT *p_event;
  CORE_DECLARE_IRQ_STATE;

  p_event = (SD_EVENT *)Mem_DynPoolBlkGet(&SD_Ptr->EventPool,
                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  CORE_ENTER_ATOMIC();
  p_event->NextPtr = SD_Ptr->PreAllocEventHeadPtr;
  SD_Ptr->PreAllocEventHeadPtr = p_event;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                           SD_PreAllocEventGet()
 *
 * @brief    Gets a pre-allocated event structure from the list.
 *
 * @return   A pre-allocated SD event structure.
 *******************************************************************************************************/
SD_EVENT *SD_PreAllocEventGet(void)
{
  SD_EVENT *p_event;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  p_event = SD_Ptr->PreAllocEventHeadPtr;
  RTOS_ASSERT_CRITICAL(p_event != DEF_NULL, RTOS_ERR_NULL_PTR, DEF_NULL);

  SD_Ptr->PreAllocEventHeadPtr = p_event->NextPtr;
  CORE_EXIT_ATOMIC();

  p_event->NextPtr = DEF_NULL;

  return (p_event);
}

/****************************************************************************************************//**
 *                                       SD_PreAllocEventFree()
 *
 * @brief    Frees a pre-allocated event in the list. Useful when an event was pre-allocate but an error
 *           occured before it was used.
 *******************************************************************************************************/
void SD_PreAllocEventFree(void)
{
  SD_EVENT *p_event;
  RTOS_ERR err_local;

  p_event = SD_PreAllocEventGet();

  Mem_DynPoolBlkFree(&SD_Ptr->EventPool,
                     (void *)p_event,
                     &err_local);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err_local) == RTOS_ERR_NONE, RTOS_ERR_CODE_GET(err_local),; );
}

/****************************************************************************************************//**
 *                                       SD_BusSyncXferExec()
 *
 * @brief    Executes a synchronous transfer. Transfer is added to the list of transfer to execute and
 *           function pends until transfer is completed.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    fnct_handle     Handle to SD function.
 *
 * @param    cmd_nbr         Command number.
 *
 * @param    cmd_arg         Command argument.
 *
 * @param    dir_is_rd       Flag that indicates if command is data read.
 *
 * @param    p_buf           Pointer to transferred buffer.
 *
 * @param    blk_qty         Number of blocks to transfer.
 *
 * @param    blk_len         Size of blocks, in octets.
 *
 * @param    timeout         Timeout, in milliseconds.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
void SD_BusSyncXferExec(SD_BUS_HANDLE  bus_handle,
                        SD_FNCT_HANDLE fnct_handle,
                        CPU_INT08U     cmd_nbr,
                        CPU_INT32U     cmd_arg,
                        CPU_BOOLEAN    dir_is_rd,
                        CPU_INT08U     *p_buf,
                        CPU_INT32U     blk_qty,
                        CPU_INT32U     blk_len,
                        CPU_INT32U     timeout,
                        RTOS_ERR       *p_err)
{
  KAL_SEM_HANDLE    sem_handle;
  SD_SYNC_XFER_DATA *p_sync_xfer_data;
  RTOS_ERR          cmpl_err;
  RTOS_ERR          err_local;

  p_sync_xfer_data = (SD_SYNC_XFER_DATA *)Mem_DynPoolBlkGet(&SD_Ptr->XferDataPool,
                                                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return;
  }

  p_sync_xfer_data->ErrPtr = &cmpl_err;
  sem_handle = KAL_SemCreate("IO - SD sync xfer sem",
                             DEF_NULL,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    goto err_free;
  }

  p_sync_xfer_data->SemHandle = sem_handle;

  SD_BusXferAdd((SD_XFER_DATA *)p_sync_xfer_data,
                bus_handle,
                fnct_handle,
                SD_XFER_TYPE_SYNC,
                cmd_nbr,
                cmd_arg,
                dir_is_rd,
                p_buf,
                blk_qty,
                blk_len,
                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto err_free;
  }

  KAL_SemPend(sem_handle,
              KAL_OPT_PEND_NONE,
              timeout,
              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);

    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_TIMEOUT) {
      goto err_free;
    }
    return;
  }

  KAL_SemDel(sem_handle);

  RTOS_ERR_COPY(*p_err, cmpl_err);

  return;

err_free:
  Mem_DynPoolBlkFree(&SD_Ptr->XferDataPool,
                     (void *)p_sync_xfer_data,
                     &err_local);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err_local) == RTOS_ERR_NONE, RTOS_ERR_CODE_GET(err_local),; );
}

/****************************************************************************************************//**
 *                                       SD_BusAsyncXferAdd()
 *
 * @brief    Adds an asynchronous transfer to the list and submits it if no transfer currently in process.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    fnct_handle     Handle to SD function.
 *
 * @param    async_fnct      Callback function to call when transfer is completed.
 *
 * @param    cmd_nbr         Command number.
 *
 * @param    cmd_arg         Command argument.
 *
 * @param    dir_is_rd       Flag that indicates if command is data read.
 *
 * @param    p_buf           Pointer to transferred buffer.
 *
 * @param    blk_qty         Number of blocks to transfer.
 *
 * @param    blk_len         Size of blocks, in octets.
 *
 * @param    p_async_data    Pointer to asynchronous function data.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
void SD_BusAsyncXferAdd(SD_BUS_HANDLE  bus_handle,
                        SD_FNCT_HANDLE fnct_handle,
                        SD_ASYNC_FNCT  async_fnct,
                        CPU_INT08U     cmd_nbr,
                        CPU_INT32U     cmd_arg,
                        CPU_BOOLEAN    dir_is_rd,
                        CPU_INT08U     *p_buf,
                        CPU_INT32U     blk_qty,
                        CPU_INT32U     blk_len,
                        void           *p_async_data,
                        RTOS_ERR       *p_err)
{
  SD_ASYNC_XFER_DATA *p_async_xfer_data;

  p_async_xfer_data = (SD_ASYNC_XFER_DATA *)Mem_DynPoolBlkGet(&SD_Ptr->XferDataPool,
                                                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return;
  }

  p_async_xfer_data->AsyncFnct = async_fnct;
  p_async_xfer_data->AsyncDataPtr = p_async_data;

  SD_BusXferAdd((SD_XFER_DATA *)p_async_xfer_data,
                bus_handle,
                fnct_handle,
                SD_XFER_TYPE_ASYNC,
                cmd_nbr,
                cmd_arg,
                dir_is_rd,
                p_buf,
                blk_qty,
                blk_len,
                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR err_local;

    Mem_DynPoolBlkFree(&SD_Ptr->XferDataPool,
                       (void *)p_async_xfer_data,
                       &err_local);
    RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err_local) == RTOS_ERR_NONE, RTOS_ERR_CODE_GET(err_local),; );
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           SD_CoreTaskHandler()
 *
 * @brief    SD core task handler.
 *
 * @param    p_data     Pointer to task data.
 *******************************************************************************************************/
static void SD_CoreTaskHandler(void *p_data)
{
  PP_UNUSED_PARAM(p_data);

  while (DEF_ON) {
    RTOS_ERR err_local;
    SD_EVENT event;
    SD_EVENT *p_event;
    CORE_DECLARE_IRQ_STATE;

    RTOS_ERR_SET(err_local, RTOS_ERR_NONE);

    KAL_SemPend(SD_Ptr->CoreEventSemHandle,
                KAL_OPT_NONE,
                KAL_TIMEOUT_INFINITE,
                &err_local);
    RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err_local) == RTOS_ERR_NONE, RTOS_ERR_CODE_GET(err_local),; );

    CORE_ENTER_ATOMIC();
    p_event = SD_Ptr->CoreEventHeadPtr;
    event = *p_event;
    SD_Ptr->CoreEventHeadPtr = (SD_EVENT *)event.NextPtr;

    if (SD_Ptr->CoreEventHeadPtr == DEF_NULL) {
      SD_Ptr->CoreEventTailPtr = DEF_NULL;
    }
    CORE_EXIT_ATOMIC();

    switch (event.Type) {
      case SD_EVENT_TYPE_CARD_DETECT:                           // Card detected. Initialize it.

        SD_CardInit(event.BusHandle,
                    &err_local);

#if (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_ENABLED)
        if (event.BusHandle->CardDetectMode != SD_CARD_DETECT_MODE_POLLING) {
#endif
        if (RTOS_ERR_CODE_GET(err_local) != RTOS_ERR_NONE) {
          SD_EVENT_FNCTS *p_event_fncts = SD_InitCfg.EventFnctsPtr;

          LOG_ERR(("Initializing SD card -> ", RTOS_ERR_LOG_ARG_GET(err_local)));

          //                                                       Notify application of card connection failure.
          if ((p_event_fncts != DEF_NULL)
              && (p_event_fncts->CardConnFail != DEF_NULL)) {
            p_event_fncts->CardConnFail(event.BusHandle, err_local);
          }
        }
#if (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_ENABLED)
    }
#endif
        break;

      case SD_EVENT_TYPE_CARD_REMOVE:
        SD_CardRem(event.BusHandle,
                   &err_local);
        if (RTOS_ERR_CODE_GET(err_local) != RTOS_ERR_NONE) {
          LOG_ERR(("Removing SD card -> ", RTOS_ERR_LOG_ARG_GET(err_local)));
        }

#if (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_ENABLED)
        if (event.BusHandle->CardDetectMode == SD_CARD_DETECT_MODE_POLLING) {
          //                                                       Restart one-shot timer for next card insertion.
          KAL_TmrStart(SD_Ptr->CoreCardPollingTmr, &err_local);
          if (RTOS_ERR_CODE_GET(err_local) != RTOS_ERR_NONE) {
            LOG_ERR(("Error restarting one-shot timer for Core polling mode -> ", RTOS_ERR_LOG_ARG_GET(err_local)));
          }
        }
#endif
        event.BusHandle->CardPresent = DEF_NO;
        break;

      case SD_EVENT_TYPE_CARD_IO_INT:
        SD_IO_CardIntHandle(event.BusHandle,
                            &err_local);
        if (RTOS_ERR_CODE_GET(err_local) != RTOS_ERR_NONE) {
          LOG_ERR(("SD card Interrupt -> ", RTOS_ERR_LOG_ARG_GET(err_local)));
        }
        break;

      case SD_EVENT_TYPE_NONE:
      default:
        LOG_ERR(("Invalid SD bus event received."));
        RTOS_ASSERT_CRITICAL(DEF_FAIL, RTOS_ERR_INVALID_TYPE,; );
        break;
    }
  }
}

/****************************************************************************************************//**
 *                                        SD_AsyncTaskHandler()
 *
 * @brief    SD async task handler.
 *
 * @param    p_data      Pointer to task data.
 *******************************************************************************************************/
static void SD_AsyncTaskHandler(void *p_data)
{
  PP_UNUSED_PARAM(p_data);

  while (DEF_ON) {
    RTOS_ERR err_local;
    SD_EVENT event;
    SD_EVENT *p_event;
    CORE_DECLARE_IRQ_STATE;

    RTOS_ERR_SET(err_local, RTOS_ERR_NONE);

    KAL_SemPend(SD_Ptr->AsyncEventSemHandle,
                KAL_OPT_NONE,
                KAL_TIMEOUT_INFINITE,
                &err_local);
    RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err_local) == RTOS_ERR_NONE, RTOS_ERR_CODE_GET(err_local),; );

    CORE_ENTER_ATOMIC();
    p_event = SD_Ptr->AsyncEventHeadPtr;
    event = *p_event;
    SD_Ptr->AsyncEventHeadPtr = (SD_EVENT *)event.NextPtr;

    if (SD_Ptr->AsyncEventHeadPtr == DEF_NULL) {
      SD_Ptr->AsyncEventTailPtr = DEF_NULL;
    }
    CORE_EXIT_ATOMIC();

    Mem_DynPoolBlkFree(&SD_Ptr->EventPool,                      // Event belongs to pool. Free it.
                       (void *)p_event,
                       &err_local);
    RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err_local) == RTOS_ERR_NONE, RTOS_ERR_CODE_GET(err_local),; );

    switch (event.Type) {
      case SD_EVENT_TYPE_DATA_XFER_CMPL:
        SD_BusXferCmplProcess(event.BusHandle,
                              event.Err);
        break;

      case SD_EVENT_TYPE_CARD_DETECT:
      case SD_EVENT_TYPE_CARD_REMOVE:
      case SD_EVENT_TYPE_CARD_IO_INT:
      case SD_EVENT_TYPE_NONE:
      default:
        LOG_ERR(("Invalid SD bus event received."));
        RTOS_ASSERT_CRITICAL(DEF_FAIL, RTOS_ERR_INVALID_TYPE,; );
        break;
    }
  }
}

/****************************************************************************************************//**
 *                                               SD_CardInit()
 *
 * @brief    Initializes an SD card.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) This function performs the basic initialization of an SD card. The SDIO specific
 *               initialization are mode in the functions SD_IO_CardInit() and SD_IO_CardFnctsInit() called
 *               by this function. The initialization sequence is described in "SD Specifications: SDIO
 *               Simplified Specification", version 3.00, section 3.1.2.
 *
 * @note     (2) Some SD cards violate the SD specification and return a response to the CMD0. This can
 *               affect the processing of further commands. This is why a short delay is added here.
 *******************************************************************************************************/
static void SD_CardInit(SD_BUS_HANDLE bus_handle,
                        RTOS_ERR      *p_err)
{
  CPU_INT16U retry_cnt;
  CPU_INT32U cmd_arg;
  const SD_BUS_DRV_API *p_bus_drv_api = bus_handle->BusDrvApiPtr;
  SD_CMD_R7_DATA r7_resp;
  SD_EVENT_FNCTS *p_event_fncts = SD_InitCfg.EventFnctsPtr;
  RTOS_ERR err_rem;

  bus_handle->CardType = SD_CARDTYPE_NONE;
  bus_handle->CardSupports1_8v = DEF_NO;

  //                                                               Resets bus properties.
  p_bus_drv_api->BusSupplyVoltSet(bus_handle->BusDrvDataPtr,
                                  SD_CARD_BUS_VOLT_3_3,
                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_fail;
  }

  p_bus_drv_api->BusSignalVoltInit(bus_handle->BusDrvDataPtr,
                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_fail;
  }

  p_bus_drv_api->ClkFreqSet(bus_handle->BusDrvDataPtr,
                            SD_FREQ_DFLT_HZ,
                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_fail;
  }

  p_bus_drv_api->BusWidthSet(bus_handle->BusDrvDataPtr,
                             1u,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_fail;
  }

  //                                                               Issue GO_IDLE_STATE command.
  for (retry_cnt = 0u; retry_cnt < SD_CMD0_RETRY_CNT; retry_cnt++) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    p_bus_drv_api->CmdNoRespExec(bus_handle->BusDrvDataPtr,
                                 bus_handle->CardType,
                                 SD_CMD_GO_IDLE_STATE,
                                 DEF_BIT_NONE,
                                 p_err);
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
      break;
    }

    KAL_Dly(SD_CMD0_RETRY_DLY_MS);
  }

  if (retry_cnt == SD_CMD0_RETRY_CNT) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);

#if (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_ENABLED)
    if (bus_handle->CardDetectMode == SD_CARD_DETECT_MODE_POLLING) {
      RTOS_ERR err_local;

      LOG_DBG(("Max nbr of retries reached for GO_IDLE_STATE cmd. No card detected."));
      //                                                           Restart one-shot timer for next card insertion.
      KAL_TmrStart(SD_Ptr->CoreCardPollingTmr, &err_local);
      if (RTOS_ERR_CODE_GET(err_local) != RTOS_ERR_NONE) {
        LOG_ERR(("Error restarting one-shot timer for Core polling mode -> ", RTOS_ERR_LOG_ARG_GET(err_local)));
      }

      return;                                                   // Card not yet inserted, retry detection at next period
    }
#endif
    goto end_fail;
  }

  KAL_Dly(2u);                                                  // Delay after CMD0. See note 2.

  //                                                               Issue SEND_IF_COND command.
  cmd_arg = SD_CMD8_ARG_CHK_PATTERN | SD_CMD8_ARG_VHS_27_36_V;
  p_bus_drv_api->CmdR7Exec(bus_handle->BusDrvDataPtr,
                           bus_handle->CardType,
                           SD_CMD_SEND_IF_COND,
                           cmd_arg,
                           &r7_resp,
                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_IO_TIMEOUT) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    bus_handle->CardType = SD_CARDTYPE_SD_V1_X;
  } else if ((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE)
             && (r7_resp.ChkPatternEcho == SD_CMD8_ARG_CHK_PATTERN)) {
    bus_handle->CardType = SD_CARDTYPE_SD_V2_0;
  } else {
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    }

    goto end_fail;
  }

#if (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_ENABLED)
  if (bus_handle->CardDetectMode == SD_CARD_DETECT_MODE_POLLING) {
    if (!bus_handle->CardPresent) {
      bus_handle->CardPresent = DEF_YES;                        // Card just connected.
    } else {
      //                                                           Card re-inserted but app did not detect removal, thus perform remove operations for clean state.
      SD_CardRem(bus_handle, &err_rem);
      if (RTOS_ERR_CODE_GET(err_rem) != RTOS_ERR_NONE) {
        LOG_ERR(("Removing SD card -> ", RTOS_ERR_LOG_ARG_GET(err_rem)));
      }
    }
  }
#endif
  //                                                               Initializes IO portion of SD card.
  SD_IO_CardInit(bus_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_fail;
  }

  if (bus_handle->IO_FnctNbr == 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);               // SD MEM only card not supported yet.
    return;
  }

  //                                                               Switch to 1.8v if card and host supports it.
  p_bus_drv_api->BusSignalVoltSwitch(bus_handle->BusDrvDataPtr,
                                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_fail;
  }

  //                                                               Select card if in SD mode.
  if (bus_handle->TransportMode == SD_TRANSPORT_MODE_SD) {
    //                                                             Retrieve Relative Card Address (RCA).
    (void)p_bus_drv_api->Cmd3Exec(bus_handle->BusDrvDataPtr,
                                  bus_handle->CardType,
                                  p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto end_fail;
    }

    //                                                             Select card.
    (void)p_bus_drv_api->Cmd7Exec(bus_handle->BusDrvDataPtr,
                                  bus_handle->CardType,
                                  DEF_YES,
                                  p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto end_fail;
    }

    bus_handle->CardEn = DEF_ENABLED;

    //                                                             Update bus width to the maximum supported.
    SD_BusWidthUpdate(bus_handle, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto end_fail;
    }
  }

  //                                                               Initialize SD IO functions.
  SD_IO_CardFnctsInit(bus_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_fail;
  }

  //                                                               Notify application of card connection.
  if ((p_event_fncts != DEF_NULL)
      && (p_event_fncts->CardConn != DEF_NULL)) {
    p_event_fncts->CardConn(bus_handle);
  }

  return;

end_fail:
  if (bus_handle->CardEn) {                                     // De-select card.
    RTOS_ERR_SET(err_rem, RTOS_ERR_NONE);

    (void)p_bus_drv_api->Cmd7Exec(bus_handle->BusDrvDataPtr,
                                  bus_handle->CardType,
                                  DEF_NO,
                                  &err_rem);
    if (RTOS_ERR_CODE_GET(err_rem) != RTOS_ERR_NONE) {
      LOG_ERR(("De-selecting SD card -> ", RTOS_ERR_LOG_ARG_GET(err_rem)));
    }
  }

  RTOS_ERR_SET(err_rem, RTOS_ERR_NONE);

  SD_CardRem(bus_handle, &err_rem);
  if (RTOS_ERR_CODE_GET(err_rem) != RTOS_ERR_NONE) {
    LOG_ERR(("Removing SD card -> ", RTOS_ERR_LOG_ARG_GET(err_rem)));
  }
}

/****************************************************************************************************//**
 *                                               SD_CardRem()
 *
 * @brief    Removes an SD card.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void SD_CardRem(SD_BUS_HANDLE bus_handle,
                       RTOS_ERR      *p_err)
{
  SD_EVENT_FNCTS *p_event_fncts = SD_InitCfg.EventFnctsPtr;

  //                                                               Notify application of card disconnection.
  if ((p_event_fncts != DEF_NULL)
      && (p_event_fncts->CardDisconn != DEF_NULL)) {
    p_event_fncts->CardDisconn(bus_handle);
  }

  SD_IO_CardRem(bus_handle, p_err);
}

/****************************************************************************************************//**
 *                                           SD_BusWidthUpdate()
 *
 * @brief    Update the bus width to the maximum supported by the SD card, the Host Controller and
 *           the board.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void SD_BusWidthUpdate(SD_BUS_HANDLE bus_handle,
                              RTOS_ERR      *p_err)
{
  const SD_BUS_DRV_API *p_bus_drv_api = bus_handle->BusDrvApiPtr;
  SD_CAP_BITMAP bus_width_flags = 0u;
  CPU_INT08U bus_width_supported;

  //                                                               - GET THE MAXIMUM BUS WIDTH SUPPORTED BY THE CARD --
  if (bus_handle->IO_FnctNbr != 0u) {
    //                                                             For SDIO Card.
    bus_width_flags = SD_IO_CardBusWidthCapGet(bus_handle,
                                               p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  } else {
    //                                                             For SD-Mem Card.
    //                                                             MICRIUM-146 function for SD-MEM
  }

  //                                                               GET THE MAX WIDTH SUPPORTED BY THE CARD,HC AND BOARD
  bus_width_flags &= bus_handle->Capabilities.Host.Capabilities;

  if (DEF_BIT_IS_SET(bus_width_flags, SD_CAP_BUS_WIDTH_8_BIT)) {
    bus_width_supported = 8u;
  } else if (DEF_BIT_IS_SET(bus_width_flags, SD_CAP_BUS_WIDTH_4_BIT)) {
    bus_width_supported = 4u;
  } else {
    bus_width_supported = 1u;
  }

  if (bus_width_supported == 1u) {
    return;                                                     // Nothing to change if bus width supported is only 1.
  }
  //                                                               ---------- DISABLE CARD INTERRUPT IN HOST ----------
  p_bus_drv_api->CardIntEnDis(bus_handle->BusDrvDataPtr,
                              DEF_NO);

  //                                                               ---------- DISABLE CARD INTERRUPT IN CARD ----------
  if (bus_handle->IO_FnctNbr != 0u) {
    //                                                             Only for SDIO Card.
    SD_IO_CardIntMasterEnDis(bus_handle,
                             DEF_NO,
                             p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }

  //                                                               ------------- CHANGE BIT MODE FOR CARD -------------
  if (bus_handle->IO_FnctNbr != 0u) {
    //                                                             For SDIO Card.
    SD_IO_CardBusWidthSet(bus_handle,
                          bus_width_supported,
                          p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  } else {
    //                                                             For SD-Mem Card.
    //                                                             MICRIUM-146 function for SD-MEM
  }

  //                                                               ------------- CHANGE BIT MODE FOR HOST -------------
  p_bus_drv_api->BusWidthSet(bus_handle->BusDrvDataPtr,
                             bus_width_supported,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               -------- ENABLE BACK CARD INTERRUPT IN CARD --------
  if (bus_handle->IO_FnctNbr != 0u) {
    //                                                             Only for SDIO Card.
    SD_IO_CardIntMasterEnDis(bus_handle,
                             DEF_YES,
                             p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }

  //                                                               -------- ENABLE BACK CARD INTERRUPT IN HOST --------
  p_bus_drv_api->CardIntEnDis(bus_handle->BusDrvDataPtr,
                              DEF_YES);
}

/****************************************************************************************************//**
 *                                       SD_BusXferAdd()
 *
 * @brief    Adds a transfer to the list and submits it if no transfer currently in process.
 *
 * @param    p_xfer_data     Pointer to transfer data structure. This must be allocated by the caller
 *                           function.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    fnct_handle     Handle to SD function.
 *
 * @param    xfer_type       Type of transfer.
 *
 * @param    cmd_nbr         Command number.
 *
 * @param    cmd_arg         Command argument.
 *
 * @param    dir_is_rd       Flag that indicates if command is data read.
 *
 * @param    p_buf           Pointer to transferred buffer.
 *
 * @param    blk_qty         Number of blocks to transfer.
 *
 * @param    blk_len         Size of blocks, in octets.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void SD_BusXferAdd(SD_XFER_DATA   *p_xfer_data,
                          SD_BUS_HANDLE  bus_handle,
                          SD_FNCT_HANDLE fnct_handle,
                          SD_XFER_TYPE   xfer_type,
                          CPU_INT08U     cmd_nbr,
                          CPU_INT32U     cmd_arg,
                          CPU_BOOLEAN    dir_is_rd,
                          CPU_INT08U     *p_buf,
                          CPU_INT32U     blk_qty,
                          CPU_INT32U     blk_len,
                          RTOS_ERR       *p_err)
{
  SD_EventPreAlloc(p_err);                                      // Alloc upfront SD event to avoid any lock op from ISR.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_xfer_data->FnctHandle = fnct_handle;
  p_xfer_data->Type = xfer_type;
  p_xfer_data->BufPtr = p_buf;
  p_xfer_data->BlkQty = blk_qty;
  p_xfer_data->BlkLen = blk_len;
  p_xfer_data->CmdArg = cmd_arg;
  p_xfer_data->CmdNbr = cmd_nbr;
  p_xfer_data->DirIsRd = dir_is_rd;

  //                                                               Add xfer data to list.
  SD_BusLock(bus_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);

    SD_PreAllocEventFree();
    return;
  }

  p_xfer_data->NextPtr = DEF_NULL;
  if (bus_handle->XferListTailPtr != DEF_NULL) {
    bus_handle->XferListTailPtr->NextPtr = p_xfer_data;
  }
  bus_handle->XferListTailPtr = p_xfer_data;

  if (bus_handle->XferListHeadPtr == DEF_NULL) {
    bus_handle->XferListHeadPtr = bus_handle->XferListTailPtr;

    SD_BusXferNextSubmit(bus_handle, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      SD_PreAllocEventFree();
    }
  }

  SD_BusUnlock(bus_handle);
}

/****************************************************************************************************//**
 *                                       SD_BusXferSubmitNext()
 *
 * @brief    Submits next data transfer, if any.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    p_err           Error pointer.
 *
 * @note     The bus must be locked by the caller function.
 *******************************************************************************************************/
static void SD_BusXferNextSubmit(SD_BUS_HANDLE bus_handle,
                                 RTOS_ERR      *p_err)
{
  SD_XFER_DATA *p_xfer_data = bus_handle->XferListHeadPtr;

  if (p_xfer_data != DEF_NULL) {
    switch (p_xfer_data->CmdNbr) {
      case SD_CMD_IO_RW_EXTENDED:
        (void)SD_IO_DataXferSubmit(bus_handle,
                                   p_xfer_data->DirIsRd,
                                   p_xfer_data->CmdArg,
                                   p_xfer_data->BufPtr,
                                   p_xfer_data->BlkQty,
                                   p_xfer_data->BlkLen,
                                   p_err);
        break;

      default:
        RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
        break;
    }
  } else {
    bus_handle->XferListTailPtr = bus_handle->XferListHeadPtr;
  }
}

/****************************************************************************************************//**
 *                                       SD_BusXferCmplProcess()
 *
 * @brief    Processes latest completed transfer on SD bus.
 *
 * @param    bus_handle      Handle to SD bus.
 *
 * @param    err             Error variable from transfer completion.
 *******************************************************************************************************/
static void SD_BusXferCmplProcess(SD_BUS_HANDLE bus_handle,
                                  RTOS_ERR      err)
{
  SD_XFER_DATA *p_xfer_data;
  RTOS_ERR err_local;

  SD_BusLock(bus_handle, &err_local);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err_local) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  p_xfer_data = bus_handle->XferListHeadPtr;
  bus_handle->XferListHeadPtr = p_xfer_data->NextPtr;

  if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
    const SD_BUS_DRV_API *p_api = bus_handle->BusDrvApiPtr;

    p_api->DataXferCmpl(bus_handle->BusDrvDataPtr,
                        bus_handle->CardType,
                        p_xfer_data->BufPtr,
                        p_xfer_data->BlkQty,
                        p_xfer_data->BlkLen,
                        p_xfer_data->DirIsRd,
                        &err);
  }

  SD_BusXferNextSubmit(bus_handle, &err_local);                 // Submit next transfer, if any.
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err_local) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  SD_BusUnlock(bus_handle);

  switch (p_xfer_data->Type) {
    case SD_XFER_TYPE_ASYNC:
      ((SD_ASYNC_XFER_DATA *)p_xfer_data)->AsyncFnct(bus_handle,
                                                     p_xfer_data->FnctHandle,
                                                     p_xfer_data->BufPtr,
                                                     p_xfer_data->BlkQty,
                                                     p_xfer_data->BlkLen,
                                                     ((SD_ASYNC_XFER_DATA *)p_xfer_data)->AsyncDataPtr,
                                                     err);
      break;

    case SD_XFER_TYPE_SYNC:
      RTOS_ERR_COPY(*(((SD_SYNC_XFER_DATA *)p_xfer_data)->ErrPtr), err);

      KAL_SemPost(((SD_SYNC_XFER_DATA *)p_xfer_data)->SemHandle,
                  KAL_OPT_POST_NONE,
                  &err_local);
      RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err_local) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      break;

    case SD_XFER_TYPE_NONE:
    default:
      RTOS_ASSERT_CRITICAL(DEF_TRUE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      break;
  }

  Mem_DynPoolBlkFree(&SD_Ptr->XferDataPool,
                     (void *)p_xfer_data,
                     &err_local);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err_local) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/********************************************************************************************************
 *                                       SD_CoreCardPollingTmrCb()
 *
 * @brief    Timer callback that sends periodically a card detect event to the Core task for card polling
 *           mode.
 *
 * @param    p_arg       Pointer to timer callback arguments.
 *******************************************************************************************************/

#if (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_ENABLED)
static void SD_CoreCardPollingTmrCb(void *p_arg)
{
  struct  sd_bus_handle *p_bus_handle;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  //                                                               For each SD controller, send card detect event.
  SLIST_FOR_EACH_ENTRY(SD_Ptr->BusHandleListHeadPtr, p_bus_handle, struct sd_bus_handle, ListNode) {
    CORE_EXIT_ATOMIC();

    if (p_bus_handle->BusStart) {                               // If bus op stopped by app, do not send event.
      SD_BusCardDetectEvent(p_bus_handle);
    }

    CORE_ENTER_ATOMIC();
  }
  CORE_EXIT_ATOMIC();
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_IO_SD_AVAIL))
